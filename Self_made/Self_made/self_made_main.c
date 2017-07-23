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

void wait_1ms(int delay)
{
	int tick_1ms_cnt;
	if (delay != 0) tick_1ms_cnt = delay; else tick_1ms_cnt = 1;
	while(tick_1ms_cnt != 1);
}

/* takes the parameters off time form each color and the runntime 
	all parameters are in mili seconds*/

void RGB_LED_PWM(byte red_pwm,byte green_pwm,byte blue_pwm,byte time_length)
{
	
	for(byte i =0;i<time_length;i++)
	{
		if ((i%red_pwm) == 0)
		{
			set_big_RGB_LED(ON,OFF,OFF);
		}
		if ((i%green_pwm) == 0)
		{
			set_big_RGB_LED(OFF,ON,OFF);
		}
		if ((i%blue_pwm) == 0)
		{
			set_big_RGB_LED(OFF,OFF,ON);
		}
		set_big_RGB_LED(OFF,OFF,OFF);
		wait_1ms(1);
	}
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
		else
		{
			RGB_LED_PWM(100,100,1,100000);
		}
		
	}
	
}
