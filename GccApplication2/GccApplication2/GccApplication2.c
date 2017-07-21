/*************************************************************************** 
    MSW-Winterthur Berufsfachschule, Zeughausstrasse 56, 8400 Winterthur     
    ---------------------------------------------------------------------    
    File        : sirius_driver.c
    Datum       : 1.4.2015
    Version			:	1.00
    Beschreibung: Treiber für ATMEL Mocca Mini Board
    Hardware    : MOCCA-Mini ATMEL ATTINY841
    Autor  			:	Ch. Riedel

****************************************************************************
    History
  		5.4.2015	: 1. Version erstellt	(ri)

****************************************************************************/

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


// ==============================================================================================================
// Hier beginnt das Hauptprogramm "main"
// --------------------------------------------------------------------------------------------------------------

int main (void)
{
	// Definition von Variablen, die nur im "main" bekannt sind.
	u16 Poti, Helligkeit;
	u16 i, speed;
	
	init_Mocca_Mini();				// Das Board wird hier initialisiert
	lcd_init();						// Das LCD muss ebenfalls initialisiert werden, um später genutz werden zu können
	clear_lcd();					// Das ganze LCD löschen

	while(1)						// Endlos-Schleife, die das ganze Programm enthält.
	{

		// Hier wird mit der dreifarbigen RGB-LED ein farbiges Blinken erzeugt
		write_text(0, 0,PSTR(" Farbblinken:        "));
		LED_RGB_OFF;					// Alle drei Farben ausschalten
		for (i=0; i<3; i++)				// Diese Schleife wird 3x durchlaufen und dann verlassen
		{
			LED_ROT_ON;					// rote LED einschalten
			wait_1ms(500);				// 500x 1ms warten
			LED_GRUEN_ON;				// grüne LED einschalten ==> Rot und Grün = Gelb
			wait_1ms(500);				// 500x 1ms warten
			LED_RGB_OFF;				// Alle drei Farben ausschalten
			LED_BLAU_ON;				// blaue LED einschalten
			wait_1ms(500);				// 500x 1ms warten
			LED_BLAU_OFF;				// blaue LED ausschalten
			wait_1ms(500);				// 500x 1ms warten
		}
		LED_GRUEN_ON;					// grüne LED einschalten ==> Rot und Grün = Gelb

		write_text(3, 0,PSTR("Taste S1 dr\x7E""cken.   "));
		while(Taste_1       == 0);				// Warten, bis die Taste 1 gedrückt wird.
		while(Taste_1       == 1);				// Warten, bis die Taste 1 auf dem Erweiterungsboard wieder losgelassen wurde.
		 
		// Hier wird ein Pin PA5 ein PWM-Signal erzeugt
		write_text(0, 0,PSTR(" PWM am Ausgang PA5:"));
		write_text(1,0, PSTR(" Poti  :"));		
		write_text(2,0, PSTR(" Licht :"));		

		write_text(3, 0,PSTR("Taste S2 dr\x7E""cken.  "));
		while(Taste_2 == 0)								// Schleife wir so lange wiederholt, bis Taste 2 auf dem Erweiterungsboard gedrückt wird
		{
			Poti		= get_ADC_Channel( 7);				//  Spannung des Potentiometers messen und das Resultat in die Variable "Poti"       schreiben
			Helligkeit  = get_ADC_Channel( 3);				//  Spannung des Lichtsensors messen und das Resultat in die Variable "Helligkeit" schreiben
			write_zahl(1, 9,Poti      ,4,0,0);
			write_zahl(2, 9,Helligkeit,4,0,0);
			PA5_EIN;										// PA5 einschalten
			wait_1ms(20);									// warten = HIGH-Phase
			PA5_AUS;										// PA5 einschalten
			wait_1ms(Helligkeit / 10);						// warten = LOW-Phase
		}
		while(Taste_2 == 1);							// Warten, bis Taste 2 auf dem Erweiterungsboard wieder losgelassen wurde.
		
		// Nur für RGB-Balken-Version
		write_text(0, 0,PSTR(" RGB-Helligkeit:    "));
		write_text(3, 0,PSTR("Taste S1 dr\x7E""cken.  "));
		while(Taste_1 == 0)								// Schleife wir so lange wiederholt, bis Taste S1 auf dem Erweiterungsboard gedrückt wird
		{
			// Der Analog/Digital-Wandler misst die analogen Spannungen
			Poti			= get_ADC_Channel( 7);				//  Spannung des Potentiometers messen und das Resultat in die Variable "Poti"       schreiben
			Helligkeit    	= get_ADC_Channel( 3);				//  Spannung des Lichtsensors   messen und das Resultat in die Variable "Helligkeit" schreiben
			write_zahl(1, 9,Poti      ,4,0,0);

			// Hier wird das Resultat der Helligkeit korrigiert		
			if (Helligkeit < 50) Helligkeit = 0; else Helligkeit = Helligkeit - 50;			// Wenn die Helligkeit kleiner als 50 ist, wird es auf 0 gesetzt, sonst um 50 verkleinert
			write_zahl(2, 9,Helligkeit,4,0,0);
			Helligkeit = Helligkeit / (880/16);
			Helligkeit = 1 << Helligkeit;

			if(Helligkeit < 2000)
			{ set_RGB_LED(0,Helligkeit,0xFFFF-Helligkeit);
			}
			else
			{ set_RGB_LED(Helligkeit,0,0xFFFF-Helligkeit);
			} 
		}
		while(Taste_1 == 1);		// Warten, bis Taste S1 auf dem Erweiterungsboard wieder losgelassen wurde.
			
		// Farbtest am RGB-Balken		
		write_text(0, 0,PSTR(" Farbtest Lauflicht:"));
		write_text(3, 0,PSTR("Taste S4 dru\x7E""cken.  "));
		set_RGB_LED(0xFFFF,0xFFFF,0xFFFF);					// Alle LED's Weiss
		while(Taste_4 == 0); while(Taste_4 != 0);			// warten, bis Taste 4 gedrückt und wieder losgelassen wurde.
		set_RGB_LED(0xFFFF,0,0);							// Alle LED's Rot
		while(Taste_4 == 0); while(Taste_4 != 0);			// warten, bis Taste 4 gedrückt und wieder losgelassen wurde.
		set_RGB_LED(0,0xFFFF,0);							// Alle LED's Grün		
		while(Taste_4 == 0); while(Taste_4 != 0);			// warten, bis Taste 4 gedrückt und wieder losgelassen wurde.
		set_RGB_LED(0,0,0xFFFF);							// Alle LED's Blau
		while(Taste_4 == 0); while(Taste_4 != 0);			// warten, bis Taste 4 gedrückt und wieder losgelassen wurde.
		
		
		while(Taste_4 == 0)
		{
			// Lauflicht der einzelnen Farben
			speed = 50;
			for(i=0; i<16; i++)
			{ set_RGB_LED(1 << i,0,0);
				wait_1ms(speed);
			}
			for(i=0; i<16; i++)
			{ set_RGB_LED(0,1 << i,0);
				wait_1ms(speed);
			}
			for(i=0; i<16; i++)
			{ set_RGB_LED(0,0,1 << i);
				wait_1ms(speed);
			}
			set_RGB_LED(0,0,0);
		}
		while(Taste_4 != 0);
	}

}