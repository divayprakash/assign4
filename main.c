//**********Header files**********//
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
//********************************//

//**********Macro declarations**********//
#define _MMIO_BYTE(mem_addr) (*(volatile uint8_t *)(mem_addr))
#define _SFR_IO8(io_addr) _MMIO_BYTE((io_addr) + __SFR_OFFSET)
#define __SFR_OFFSET 0x20

#define _MMIO_WORD(mem_addr) (*(volatile uint16_t *)(mem_addr))
#define _SFR_MEM8(mem_addr) _MMIO_BYTE(mem_addr)
#define _SFR_MEM16(mem_addr) _MMIO_WORD(mem_addr)
//**************************************//

//**********Register Declarations**********//
#define DDRB _SFR_IO8(0x04)
#define PORTB _SFR_IO8(0x05)
#define DDRD _SFR_IO8(0x0A)
#define PORTD _SFR_IO8(0x0B)

#define TCCR1A _SFR_MEM8(0x80)
#define WGM10 0
#define WGM11 1

#define TCCR1B _SFR_MEM8(0x81)
#define CS10 0
#define CS11 1
#define CS12 2
#define WGM12 3
#define WGM13 4

#define TCNT1 _SFR_MEM16(0x84)
#define OCR1A _SFR_MEM16(0x88)
#define TIMSK1 _SFR_MEM8(0x6F)
#define OCIE1A 1
//*****************************************//

//**********LCD Functions**********//
#define	clearLCD 0b00000001
//Clear Display and Home the Cursor
#define	returnToHomeLCD 0b00000010
//Return Cursor and LCD to Home Position
#define	setCursorMoveDirectionLCD 0b00000110
//Set Cursor Move Direction
#define	turnOffLCD 0b00001000
//Turn off LCD
#define	turnOnLCD 0b00001100
//Turn on LCD
#define	resetLCD 0b00110000
//Reset LCD
#define	bitMode4LCD 0b00101000
//Set LCD to 4bit mode
//*********************************//

//**********Global Variable**********//
int flag = 0;
//***********************************//

//**********Function Prototypes**********//
void initializeLCD (void );
void writeStringLCD (uint8_t *);
void writeCharLCD (uint8_t );
void writeInstructionLCD (uint8_t );
void writeToLCD (uint8_t );
void set4bitModeLCD();
void funcResetLCD();
void setup_delay();
//***************************************//

//**********Function to setup timer**********//
void setup_delay() 
{
	TCCR1A = 0;
	// set entire TCCR1A register to 0
  	TCCR1B = 0;
  	// same for TCCR1B
  	TCNT1  = 0;
  	//initialize counter value to 0
  	// set compare match register for 1hz increments
  	OCR1A = 31249;
  	// = (16*10^6) / (2*1024) - 1 (must be <65536)
  	// turn on CTC mode
  	TCCR1B |= (1 << WGM12);
  	// Set CS10 and CS12 bits for 1024 prescaler
  	TCCR1B |= (1 << CS12) | (1 << CS10);  
  	// enable timer compare interrupt
  	TIMSK1 |= (1 << OCIE1A);
}
//*******************************************//

//**********Names to display**********//
uint8_t name1[] = "Divay Prakash";
uint8_t name2[] = "Sahil Ruhela";
//************************************//

//**********Reset LCD**********//
void funcResetLCD()
{
	writeInstructionLCD(resetLCD);	//2
	_delay_ms(10);					//3
	writeInstructionLCD(resetLCD);	//4
	_delay_us(200);					//5
	writeInstructionLCD(resetLCD);	//6
	_delay_us(200);					//7
}
//*****************************//

//**********Set LCD to 4-bit mode**********//
void set4bitModeLCD()
{
	writeInstructionLCD(bitMode4LCD);	
	// Set 4bit mode		
	_delay_us(80);				
	writeInstructionLCD(bitMode4LCD);			
	// Set Interface Length
	_delay_us(80);		
}
//*****************************************//

//**********Function to initialize LCD**********//
void initializeLCD()
{
	_delay_ms(100);						
	//Power on delay		
	PORTB &= 0b11111110;						
	//set RS 0
	PORTB &= 0b11111101;						
	//set EN 0
	funcResetLCD();
	//reset LCD
	set4bitModeLCD();
	//set LCD to 4 bit
	writeInstructionLCD(turnOffLCD);				
	//Turn Display Off
	_delay_ms(80);
	writeInstructionLCD(clearLCD);				
	//Clear Display RAM
	_delay_ms(4);
	writeInstructionLCD(setCursorMoveDirectionLCD);				
	//Set Shift
	_delay_us(80);
	writeInstructionLCD(turnOnLCD);				
	//Turn Display On
	_delay_us(80);
}
//**********************************************//

//**********Interrupt Service Routine to print names**********//
ISR(TIMER1_COMPA_vect)
{
	if (flag == 0) 
	{
		writeInstructionLCD(returnToHomeLCD);
		_delay_ms(4);
		writeInstructionLCD(clearLCD);
		_delay_ms(4);
		writeStringLCD(name1);
		_delay_ms(2000);
		flag = 1;
	}
	else 
	{
		writeInstructionLCD(returnToHomeLCD);
		_delay_ms(4);
		writeInstructionLCD(clearLCD);
		_delay_ms(4);
		writeStringLCD(name2);
		_delay_ms(2000);
		flag = 0;
	}
	TCNT1 = 0;
  // Reset Timer1 Count Register
}
//************************************************************//

//**********Write a string to LCD**********//
void writeStringLCD(uint8_t s[])
{
	volatile int i = 0;
	while (s[i] != 0)
	{
		writeCharLCD(s[i++]);
		_delay_us(80);					
	}
}
//*****************************************//

//**********Write a char to LCD**********//
void writeCharLCD(uint8_t c)
{
	PORTB |= 0b00000001;
	PORTB &= 0b11111101;						
	writeToLCD(c);
	writeToLCD(c << 4);
}
//***************************************//

//**********Write instruction to LCD**********//
void writeInstructionLCD(uint8_t i)
{
	PORTB &= 0b11111110;						
	PORTB &= 0b11111101;						
	writeToLCD(i);
	writeToLCD(i << 4);
}
//********************************************//

//**********Pulse enable**********//
void pulse_enable()
{	
	PORTB |= 0b00000010;
	_delay_us(1);
	PORTB &= 0b11111101;
	_delay_us(1);
}
//********************************//

//**********Write byte to lcd**********//
void writeToLCD(uint8_t b)
{
	PORTD &= 0b01111111;						
	if (b & 1 << 7)	PORTD |= 0b10000000;					
	PORTD &= 0b10111111;						
	if (b & 1 << 6)	PORTD |= 0b01000000;					
	PORTD &= 0b11011111;						
	if (b & 1 << 5)	PORTD |= 0b00100000;					
	PORTD &= 0b11101111;						
	if (b & 1 << 4)	PORTD |= 0b00010000;					
	pulse_enable();
}
//************************************//

//**********Main function**********//
int main(void)
{
	cli();
	setup_delay();
	DDRD |= 0b11110000;
	// Pins 14-11 on LCD (D7-4) to PORTD7-4 on ATMega328 (pin 7-4 on Arduino)
	DDRB |= 0b00000011;
	// RS and EN on LCD (4, 6) to PORTB1,0 on ATMega328 (pin 9,8 on Arduino)
	initializeLCD();
	sei();							
	while (1) {}		
	return 0;
}
//*********************************//