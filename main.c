//**********Header files**********//
#include <util/delay.h>
//********************************//

//**********Macro declarations**********//
#define _MMIO_BYTE(mem_addr) (*(volatile uint8_t *)(mem_addr))
#define _SFR_IO8(io_addr) _MMIO_BYTE((io_addr) + __SFR_OFFSET)
#define __SFR_OFFSET 0x20
//**************************************//

//**********Register Declarations**********//
#define DDRB _SFR_IO8(0x04)
#define PORTB _SFR_IO8(0x05)
#define DDRD _SFR_IO8(0x0A)
#define PORTD _SFR_IO8(0x0B)
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

//**********Function Prototypes**********//
void initializeLCD (void );
void writeStringLCD (uint8_t *);
void writeCharLCD (uint8_t );
void writeInstructionLCD (uint8_t );
void writeToLCD (uint8_t );
void set4bitModeLCD();
void funcResetLCD();
//***************************************//

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

//**********Print names**********//
void print_names()
{
	writeInstructionLCD(returnToHomeLCD);
	_delay_ms(4);
	writeInstructionLCD(clearLCD);
	_delay_ms(4);
	writeStringLCD(name1);
	_delay_ms(2000);
	writeInstructionLCD(returnToHomeLCD);
	_delay_ms(4);
	writeInstructionLCD(clearLCD);
	_delay_ms(4);
	writeStringLCD(name2);
	_delay_ms(2000);
}
//*******************************//

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
	DDRD |= 0b11110000;
	// Pins 14-11 on LCD (D7-4) to PORTD7-4 on ATMega328 (pin 7-4 on Arduino)
	DDRB |= 0b00000011;
	// RS and EN on LCD (4, 6) to PORTB1,0 on ATMega328 (pin 9,8 on Arduino)
	initializeLCD();							
	while (1) print_names();		
	return 0;
}
//*********************************//