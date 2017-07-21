/*
 * NewMain.c
 *
 * Created: 18.07.17 21:21:35
 *  Author: tendai
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#pragma GCC optimize 0

#define ON 1
#define OFF 0

typedef unsigned char byte;

void set_big_RGB_LED(byte red,byte green, byte blue)
{
	byte color = red|(green<<1)|(blue<<2);
	DDRB = 0x07;
	PORTB = (PORTB&0xF8);
	PORTB = (PORTB|color);
}

byte get_buttons(byte button)
{
	DDRA = (DDRA & 0xFC);
	switch(button)
	{
		case 1: return (PINA&0x01);
			break;
		case 2: return (PINA&0x02);
			break;
		default: return 2;
	}
}

int main(void)
{
	byte the_button;
	
	set_big_RGB_LED(OFF,ON,OFF);
	
	while (1)
	{
		the_button = get_buttons(1);
		if(the_button==1)
		{
			set_big_RGB_LED(OFF,ON,OFF);
		}
		else{
			set_big_RGB_LED(ON,OFF,OFF);
		}
	}
	
}
