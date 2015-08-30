//**********Header files**********//
#include <avr/io.h>
#include <util/delay.h>
//********************************//

//**********LCD Functions**********//
#define	lcd_Clear 0b00000001
//Clear Display and Home the Cursor
#define	lcd_Home 0b00000010
//Return Cursor and LCD to Home Position
#define	lcd_EntryMode 0b00000110
//Set Cursor Move Direction
#define	lcd_DisplayOff 0b00001000
//Turn off LCD
#define	lcd_DisplayOn 0b00001100
//Turn on LCD
#define	lcd_FunctionReset 0b00110000
#define	lcd_FunctionSet4bit 0b00101000
//*********************************//

//**********Function Prototypes**********//
void lcd_init (void );
void lcd_write_string (uint8_t *);
void lcd_write_character (uint8_t );
void lcd_write_instruction (uint8_t );
void lcd_write (uint8_t );
void lcd_set_4bit();
void lcd_reset();
//***************************************//

//**********Names**********//
uint8_t name1[] = "Divay Prakash";
uint8_t name2[] = "Sahil Ruhela";
//*************************//

//**********Print names**********//
void print_names()
{
	lcd_write_instruction (lcd_Home);
	_delay_ms(4);
	lcd_write_instruction (lcd_Clear);
	_delay_ms(4);
	lcd_write_string (name1);
	_delay_ms(2000);
	lcd_write_instruction (lcd_Home);
	_delay_ms(4);
	lcd_write_instruction (lcd_Clear);
	_delay_ms(4);
	lcd_write_string (name2);
	_delay_ms(2000);
}
//*******************************//

//**********Reset LCD**********//
void lcd_reset()
{
	lcd_write_instruction (lcd_FunctionReset);	//2
	_delay_ms(10);								//3
	lcd_write_instruction (lcd_FunctionReset);	//4
	_delay_us(200);								//5
	lcd_write_instruction (lcd_FunctionReset);	//6
	_delay_us(200);								//7
}
//*****************************//

//**********Set LCD to 4-bit mode**********//
void lcd_set_4bit()
{
	lcd_write_instruction (lcd_FunctionSet4bit);	
	// Set 4bit mode		
	_delay_us (80);				
	lcd_write_instruction (lcd_FunctionSet4bit);			
	// Set Interface Length
	_delay_us (80);		
}
//*****************************************//

//**********Function to initialize LCD**********//
void lcd_init (void )
{
	_delay_ms(100);						
	//Power on delay		
	PORTB &= 0b11111110;						
	//set RS 0
	PORTB &= 0b11111101;						
	//set EN 0
	lcd_reset();
	//reset LCD
	lcd_set_4bit();
	//set LCD to 4 bit
	lcd_write_instruction (lcd_DisplayOff);				
	// Turn Display Off
	_delay_ms (80);
	lcd_write_instruction (lcd_Clear);				
	// Clear Display RAM
	_delay_ms (4);
	lcd_write_instruction (lcd_EntryMode);				
	// Set Shift Characteristics
	_delay_us (80);
	lcd_write_instruction (lcd_DisplayOn);				
	// Turn Display Back On
	_delay_us (80);
}
//**********************************************//
	
//**********Write a string to LCD**********//
void lcd_write_string (uint8_t theString[])
{
	volatile int i = 0;
	while (theString[i] != 0)
	{
		lcd_write_character (theString[i]);
		i ++;
		_delay_us(80);					
	}
}
//*****************************************//

//**********Write a char to LCD**********//
void lcd_write_character (uint8_t theData)
{
	PORTB |= 0b00000001;
	PORTB &= 0b11111101;						
	lcd_write (theData);
	lcd_write (theData << 4);
}
//***************************************//

//**********Write instruction to LCD**********//
void lcd_write_instruction (uint8_t theInstruction)
{
	PORTB &= 0b11111110;						
	PORTB &= 0b11111101;						
	lcd_write (theInstruction);
	lcd_write (theInstruction << 4);
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
void lcd_write (uint8_t theByte)
{
	PORTD &= 0b01111111;						
	if (theByte & 1 << 7)						
		PORTD |= 0b10000000;					
	PORTD &= 0b10111111;						
	if (theByte & 1 << 6)						
		PORTD |= 0b01000000;					
	PORTD &= 0b11011111;						
	if (theByte & 1 << 5)						
		PORTD |= 0b00100000;					
	PORTD &= 0b11101111;						
	if (theByte & 1 << 4)						
		PORTD |= 0b00010000;					
	pulse_enable();
}
//*************************************//

//**********Main function**********//
int main(void)
{
	DDRD |= 0b11110000;
	// Pins 14-11 on LCD (D7-4) to PORTD7-4 on ATMega328 (pin 7-4 on Arduino)
	DDRB |= 0b00000011;
	// RS and EN on LCD (4, 6) to PORTB1,0 on ATMega328 (pin 9,8 on Arduino)
	lcd_init ();							
	while (1) print_names();		
	return 0;
}
//*********************************//
