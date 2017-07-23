/*
 * NewMain.c
 *
 * Created: 13.07.2017 23:03:12
 *  Author: Tendai Rondof
 */ 

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "Mocca_Mini.h"
 #include <avr/pgmspace.h>							// Ermöglicht die Platzierung von "static const" im Code-Segment, statt im RAM.
 // Definition mit "PROGMEM", Lesen mit "pgm_read_byte, pgm_read_ptr"
 
 #pragma GCC optimize 0								// Optimierung ausschalten, damit das Debugging möglich ist


 // Das sind die Funktionen, die genutzt werden können.
 void init_Mocca_Mini(void);
 void lcd_init(void);
 void clear_lcd(void);
 void write_zahl(u8 x_pos, u8 y_pos, u32 zahl_v, u8 s_vk, u8 s_nk, u8 komma);
 void write_text(u8 y_pos, u8 x_pos, const char* str_ptr);
 void wait_1ms(u32 delay);
 void delay_nop(u8 time);
 void set_RGB_LED(u16 rot,u16 gruen,u16 blau);
 u16  get_ADC_Channel(u8 channel);

 // Hier sind globale Variablen definiert, die also überall bekannt und gültig sind.
 extern u32 tick_1ms;
 extern u16 DIP_Switch, Taste_1, Taste_2, Taste_3, Taste_4;
 extern u8  write_RGB_LCD_activ;
 extern u16 RGB_rot, RGB_gruen, RGB_blau;

 u8 std, min, sec, sec_neu;

 u8* crate_code()
 {
	clear_lcd();
	write_text(0,0,PSTR("\x1B""                  \x1A"""));
 	write_text(1,0,PSTR("create Code:"));
 	write_text(3,18,PSTR("OK"));

	u8 zahl = 0;
	static u8 code[3];
	u8 x_pos = 0;
	u8 index = 0;
	write_zahl(2,x_pos,zahl,2,0,0);
	while (1)
	{
		if(Taste_4 == 1)
		{
			x_pos += 3;
			code[index] = zahl;
			index++;
			zahl = 0;
			
			if (index > 2)
			{
				break;
			}
			write_zahl(2,x_pos,zahl,2,0,0);
			write_text(2,10,PSTR("Applyed"));
			wait_1ms(500);
			write_text(2,10,PSTR("       "));
		}
		if (Taste_3 == 1)
		{
			zahl++;
			if(zahl>15){
				zahl = 0;
			}
			write_zahl(2,x_pos,zahl,2,0,0);
			wait_1ms(500);
		}
		if (Taste_1 == 1)
		{
			zahl--;
			if(zahl>15){
				zahl = 15;
			}
			write_zahl(2,x_pos,zahl,2,0,0);
			wait_1ms(500);
		}
	}
	clear_lcd();
	write_text(1,0,PSTR("    Code created    "));
	write_zahl(2,6,code[0],2,0,0);
	write_zahl(2,10,code[1],2,0,0);
	write_zahl(2,14,code[2],2,0,0);

	return code;
 }

 u8 start_game(u8 code[3])
 {
	clear_lcd();
	u16 Poti = get_ADC_Channel(7);
	Poti = Poti>>6;
	u16 color = 0x0007;
	u16 crackt = 0x0000;
	set_RGB_LED(color,0,0);
	u8 i = 0;
	u8 place = 8;

	write_text(3,0,PSTR("       try to crack\x10"));
	write_text(1,2,PSTR("code: ..,..,..  "));

	while(i<3)
	{
		u8 is_in_while = 0;
		while (is_in_while == 0)
		{
			u16 Poti = get_ADC_Channel(7);
			Poti = Poti>>6;

			if (Poti==code[i])
			{
				set_RGB_LED(0,0x0007,0);
				if (Taste_4 == 1)
				{
					crackt = crackt<<1;
					crackt++;
					color = 0x0007-crackt;
					is_in_while = 1;
					write_zahl(1,place,code[i],2,0,0);
					place+=3;
					i++;
					wait_1ms(500);
				}
			}
			else
			{
				set_RGB_LED(color,crackt,0);
			}
			if (Taste_4 == 1 && Poti != code[i])
			{
				set_RGB_LED(0,0,0);
				return 0;
			}
		}
	}
	set_RGB_LED(0,0,0);
	return 1;
	 
}

void code_cracker_game()
{
	clear_lcd();
	u8 *code_array;
	write_text(0,0,PSTR("\x11"" crate the code "));
	write_text(3,13,PSTR(" back \x10"));
	wait_1ms(500);

	while (1)
	{
		if (Taste_1 == 1)
		{
			code_array = crate_code();
			clear_lcd();
			write_text(0,0,PSTR("\x11"" crate the code "));
			write_text(3,0,PSTR("\x11"" start   \xDA""   back \x10"));
			wait_1ms(500);
		}
		if (Taste_2 == 1)
		{
			u8 game_result = start_game(code_array);
			if (game_result == 1)
			{
				clear_lcd();
				write_text(1,0,PSTR("      YOU WIN      "));
				wait_1ms(10000);
				clear_lcd();
				write_text(0,0,PSTR("\x11"" crate the code "));
				write_text(3,0,PSTR("\x11"" start   \xDA""   back \x10"));

			} 
			else
			{
				clear_lcd();
				write_text(1,0,PSTR("      YOU LOSE     "));
				wait_1ms(10000);
				clear_lcd();
				write_text(0,0,PSTR("\x11"" crate the code "));
				write_text(3,0,PSTR("\x11"" start   \xDA""   back \x10"));
			}
		}
		if (Taste_4 == 1)
		{
			clear_lcd();
			wait_1ms(100);
			break;
		}
	}
}

u8 start_fighter_game()
{
	clear_lcd();

	u16 life_points = 0x001F;
	u16 losted_points = 0;

	//player vars
	u8 player_x_pos = 3;
	u8 player_y_pos = 1;
	u8 player_save_time = 0;
	u8 cool_down = 5;

	//target vars
	u8 respawn = 0;
	u8 target_y_pos = 15;
	u8 target_x_pos = 19;
	u8 move_cool_down = 5;
	u8 target_y_poses[20];
	for (u8 j = 0;j<20;j++)
	{
		target_y_poses[j] = 15;
	}
	u8 target_x_poses[20];
	for (u8 j = 0;j<20;j++)
	{
		target_x_poses[j] = 19;
	}

	//other game vars
	u8 ind = 0;
	u8 spawn_points[35] = {1,0,2,1,1,0,3,2,3,3,2,0,2,3,0,1,2,3,1,3,2,1,0,0,3,0,1,2,3,0,3,2,1,0,1};
	u16 fps = 40;

	set_RGB_LED(losted_points,0,life_points);

	while (life_points>0)
	{
		//create a new enemy
		if (respawn<1)
		{
			//check game end
			if (ind==35)
			{
				clear_lcd();
				write_text(1,6,PSTR("YOU WIN"));
				wait_1ms(2000);
				return 1;
			}
			target_y_pos = spawn_points[ind];
			ind++;

			for (u8 j = 0;j<20;j++)
			{
				if (target_y_poses[j] == 15)
				{
					target_y_poses[j] = target_y_pos;
					target_x_poses[j] = target_x_pos;
					break;
				}
			}

			respawn = 20;
		}
		//move enemy
		if(move_cool_down == 0)
		{
			for (u8 i = 0; i<20;i++)
			{
				if (target_y_poses[i]<4)
				{
					write_text(target_y_poses[i],target_x_poses[i],PSTR(" "));
					target_x_poses[i] = target_x_poses[i]-1;
					write_text(target_y_poses[i],target_x_poses[i],PSTR("\x11"""));
					move_cool_down = 5;
				}
				if (target_x_poses[i]<=1)
				{
					write_text(target_y_poses[i],target_x_poses[i],PSTR(" "));
					target_y_poses[i] = 15;
				}
				//damage und display life points leds
				if (target_x_poses[i] == player_x_pos && target_y_poses[i] == player_y_pos && player_save_time==0)
				{
					write_text(player_y_pos,player_x_pos-1,PSTR(" \xB2"""));
					life_points = life_points>>1;
					losted_points = 0x001F-life_points;
					set_RGB_LED(losted_points,0,life_points);
					player_save_time = 20;
					wait_1ms(150);
				}
			}
			move_cool_down = 5;
		}

		//display the player
		write_text(player_y_pos,player_x_pos,PSTR("\x10"""));

		//move the player one up
		if (Taste_3 == 1 && cool_down==0)
		{
			write_text(player_y_pos,player_x_pos,PSTR(" "));
			player_y_pos--;

			if (player_y_pos>4)
			{
				player_y_pos = 3;
			}
			cool_down = 5;
		}
		//move the player one down
		if (Taste_4 == 1 && cool_down==0)
		{
			write_text(player_y_pos,player_x_pos,PSTR(" "));
			player_y_pos++;

			if (player_y_pos>4)
			{
				player_y_pos = 0;
			}
			cool_down = 5;
		}
		//controll speed limiter
		if (cool_down>0)
		{
			cool_down--;
		}
		//enemy move speed limiter
		if (move_cool_down>0)
		{
			move_cool_down--;
		}
		//player save time after a collision 
		if (player_save_time>0)
		{
			player_save_time--;
		}
		respawn--;
		wait_1ms(fps);
	}
	clear_lcd();
	write_text(1,6,PSTR("YOU LOSE"));
	wait_1ms(2000);
	return 0;
}


 int main (void){

	// Definition von Variablen, die nur im "main" bekannt sind.
	

	init_Mocca_Mini();
	lcd_init();

	write_text(0,0,PSTR("\x11"" code cracker "));
	write_text(3,0,PSTR("\x11"" fighter game "));

	while (1)
	{
		if (Taste_1 == 1)
		{
			code_cracker_game();
			clear_lcd();
			write_text(0,0,PSTR("\x11"" code cracker "));
			write_text(3,0,PSTR("\x11"" fighter game "));
		}
		if (Taste_2 == 1)
		{
			start_fighter_game();
			clear_lcd();
			write_text(0,0,PSTR("\x11"" code cracker "));
			write_text(3,0,PSTR("\x11"" fighter game "));
			set_RGB_LED(0,0,0);
		}
	}
	
 }