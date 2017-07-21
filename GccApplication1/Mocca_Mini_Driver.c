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


#define LCD_Refresh_Time				200			// Zeitintervall für LCD-Ausgaben:  200 x 1ms = 200ms 


u32 volatile tick_1ms_cnt		= 0;
u32 tick_1ms								= 0;
u16 LCD_Refresh_Time_cnt		= LCD_Refresh_Time;
u16 DIP_Switch, Taste_1, Taste_2, Taste_3, Taste_4;
 u8  write_RGB_LCD_activ = 0;
u16 RGB_rot, RGB_gruen, RGB_blau;


void wait_1ms(u32 delay)
{
	if (delay != 0) tick_1ms_cnt = delay; else tick_1ms_cnt = 1;
	while(tick_1ms_cnt != 1);
}

void delay_nop(u8 time)				// time = 1 ==> Delay = 2.08us
															// time > 1 ==> Delay = time * 0.62us
{ u8 i;
	for(i=0; i<time; i++) __asm__ __volatile__ ("nop");
}



#pragma GCC optimize 2						// Optimitation-Level nicht verstellen, weil sonst das Timing der LCD-Schnittstelle nicht mehr stimmen würde!!!

// Siganlnamen für Daten und Clock
#define LCD_SOD_0					(PORTA &= ~0x10)	//PA.4: LCD SOD = 0  (Serial Out Data)
#define LCD_SOD_1					(PORTA |=  0x10)	//PA.4: LCD SOD = 1
#define LCD_SCLK_0				(PORTA &= ~0x40)	//PA.6: LCD SCLK = 0 (Serial CLOCK)
#define LCD_SCLK_1				(PORTA |=  0x40)	//PA.6: LCD SCLK = 1

void write_lcd_f(u8 rs, u8 wert)
{	unsigned char i;
	
	write_RGB_LCD_activ = 1;
	// LCD: EDIP204	
	//Manche Befehle müssen mehrfach ausgeführt werden, damit die minimale Pulslänge von xxx ns eingehalten wird. (bei 8MHz)

	//Synchronisierung: Clock-Signal 5x toggeln
	LCD_SOD_1;																			// Während nachfolgend 5 Synch-Bits gesendet werden muss SOD = 1 sein.
	LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;LCD_SCLK_1;		// Pulsbreite je 500ns (Keine Schleife verwenden, da sonst Laufzeit unnötig grösser wird.)
	LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;LCD_SCLK_1;
	LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;LCD_SCLK_1;
	LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;LCD_SCLK_1;
	LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;
	
	//R/W: 1=Read, 0=Write
	LCD_SOD_0;																									// R/W = 0
	LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;													// R/W-Bit senden
	
	//RS Register Selection: 0=Command, 1=Data
	if (rs == 'C') LCD_SOD_0; else LCD_SOD_1; 
	LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;													// RS-Bit senden

	//End-Marke 0
	LCD_SOD_0;
	LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;													// END-Bit senden
	
	for (i = 0; i < 4; i++)																			// Daten-Bit 0-3 senden
	{ LCD_SCLK_0;
		if (wert & 0x01) LCD_SOD_1; else LCD_SOD_0;
		wert = wert >> 1;
		LCD_SCLK_0;	LCD_SCLK_1;
	}

	LCD_SOD_0;																									// 4x "0" senden
	for (i = 0; i < 4; i++) {LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;}
	
	for (i = 0; i < 4; i++)																			// Daten-Bit 4-7 senden
	{ LCD_SCLK_0;
		if (wert & 0x01) LCD_SOD_1; else LCD_SOD_0;
		wert = wert >> 1;
		LCD_SCLK_0;	LCD_SCLK_1;
	}

	LCD_SOD_0;																									// 4x "0" senden
	for (i = 0; i < 4; i++)	{LCD_SCLK_0;LCD_SCLK_0; LCD_SCLK_1;}

	// Write-Befehl auf 50us verlängern, damit minimale Execution-Time 39us/43us eingehalten ist.
	for (i = 0; i < 20; i++) LCD_SOD_1;
	write_RGB_LCD_activ = 0;
}

/**********************************************************************************
  write_text
 
  Text an xy-Position ausgeben (auf integriertem LCD-Display)
  y_pos:   Zeile-Nummer (0..3)
  x_pos:   Spalte-Nummer (0..19) (Zeichenposition auf Zeile)
  str_ptr: Adresse des zu schreibenden Textes
***********************************************************************************/
void write_text(u8 y_pos, u8 x_pos, const char* str_ptr)
{	u8 wert, str_p = 0;
	
	x_pos += y_pos * 0x20;																					// Position auf LCD berechnen 4x20 Zeichen
	write_lcd_f ('C',x_pos | 0x80);																	// LCD-Cursor auf gewünsschte Adresse setzen = Position auf LCD
	wert = pgm_read_byte(&str_ptr[str_p++]);
	while(wert != 0)
	{	write_lcd_f ('D',wert);																				// Daten in obige Adresse schreiben
		wert = pgm_read_byte(&str_ptr[str_p++]);
	}
}

/**********************************************************************************\
* write_text_RAM
*
* Text an xy-Position ausgeben (auf integriertem LCD-Display)
* y_pos:   Zeile-Nummer (0..3)
* x_pos:   Spalte-Nummer (0..19) (Zeichenposition auf Zeile)
* str_ptr: Adresse des zu schreibenden Textes
\**********************************************************************************/
void write_text_ram(u8 y_pos, u8 x_pos, const char* str_ptr)
{	u8 str_p = 0;
	
	x_pos += y_pos * 0x20;																					// Position auf LCD berechnen 4x20 Zeichen
	write_lcd_f ('C',x_pos | 0x80);																	// LCD-Cursor auf gewünsschte Adresse setzen = Position auf LCD
	while (str_ptr[str_p]) write_lcd_f ('D',str_ptr[str_p++]);			// Daten in obige Adresse schreiben
}


/**********************************************************************************\
* write_zahl
*
* Zahl an xy-Position ausgeben (auf integriertem LCD-Display)
* y_pos:   Zeile-Nummer (0..3)
* x_pos:   Spalte-Nummer (0..19) (Zeichenposition auf Zeile)
* zahl_v:  Zahl (0...4'294'967'296)
* s_vk:
* s_nk:
* komma:  0 = kein Komma
*         1 = vor der      letzten Ziffer das Komma setzen (Zahl=Zahl/10)
*         2 = vor der zweitletzten Ziffer das Komma setzen (Zahl=Zahl/100)
*         3 = vor der drittletzten Ziffer das Komma setzen (Zahl=Zahl/1'000)
*         9 = vor der viertletzten Ziffer das Komma setzen (Zahl=Zahl/10'000)
\**********************************************************************************/
void write_zahl(u8 x_pos, u8 y_pos, u32 zahl_v, u8 s_vk, u8 s_nk, u8 komma)
{
	u32		zehner;
	char	send_buffer[12];
	u8		i, pos, pos_t, nullen_loeschen = 1;

	//Umwandlung in die einzelnen Stellen-Zahlen 1er, 10er, 100er, ... 1'000'000'000er
	//zahl_v = 1234567890;

	if(s_vk >= 100) 
	{ s_vk -= 100;
		nullen_loeschen = 0;
	}
	if(s_vk > 10) s_vk = 10;
	
	if (s_nk > komma) s_nk = komma;									// unmöglicher Fall: mehr Nachkommastellen als Komma überhaupt geschoben werden 
	if (s_vk + s_nk > 10) s_nk = 10 - s_vk;					// unsinniger  Fall: zu viele Stellen
	
	zehner		= 10;
	send_buffer[11] = (zahl_v % 10) + 48;
	i = 10;
	do
	{ send_buffer[i] = ( (zahl_v / zehner) % 10) + 48;
		zehner *= 10;
	} while(i--);

	//Vor-Kommastellen kopieren
	pos = 0;
	pos_t = 12-komma-s_vk;
	//if(s_vk == 0) pos_t
	for (i = 0; i < s_vk; i++)
	{ send_buffer[pos++] = send_buffer[pos_t++];
	}
	if (s_nk > 0)
	{ send_buffer[pos++] = '.';

		//Nach-Kommastellen kopieren
		pos_t = 12-komma;
		for (i = 0; i < s_nk; i++) send_buffer[pos++] = send_buffer[pos_t++];
	}
	send_buffer[pos] = 0;    //Endmarke des Strings setzen

	if(nullen_loeschen)
	{ //Vorangehende Nullen löschen		(xyz)
	  i = 0;
	  while ((send_buffer[i] == 48) && (i < s_vk-1)) send_buffer[i++] = 32;
	}
	
	write_text_ram(x_pos, y_pos, send_buffer);
}


/**********************************************************************************\
* lcd_init
*
* Initialisierung des integrierten LCD-Displays
\**********************************************************************************/

void lcd_init(void)
{	u8 i;
	
	DDRA |= 0x10;													// Port PA4 auf Output setzen LCD-Outputs SOD
	DDRA |= 0x40;													// Port PA6 auf Output setzen LCD-Outputs SCLK

	for(i=0; i<50; i++) delay_nop(120);   // 10ms (50x 0.2us) warten, bis LCD gestartet ist (Power-Up)
	
	write_lcd_f('C',0x34);								// set 8-Bit-Interface RE = 1
	write_lcd_f('C',0x34);								// Nochmals, denn einige LCD starten sonst nicht korrekt.
	write_lcd_f('C',0x09);								// 4-Zeilen-Modus, 5-Dot Font-Breite
	write_lcd_f('C',0x30);								// set 8-Bit-Interface RE = 0
	write_lcd_f('C',0x0C);								// Display ON, Cursor OFF
	write_lcd_f('C',0x01);								// Clear Display
	for(i=0; i<8; i++) delay_nop(120);		// 1.6ms (8x 0.2us) warten, bis LCD gelöscht ist 
	
	write_lcd_f('C',0x07);								// Entry Mode
}


/**********************************************************************************\
* clear_lcd
*
* Löscht die Anzeige auf dem LCD-Display
\**********************************************************************************/

void clear_lcd(void)
{ u8 i;
	write_lcd_f('C',0x01);      //Clear Display
	for(i=0; i<8; i++) delay_nop(120);		// 1.6ms (8x 0.2us) warten, bis LCD gelöscht ist 
}


u8 get_Taste_1(void)		
{
	if(LED_1_Input)							// Falls der Ausgang auf 1 gesetzt ist, diesen auf 0 setzen,
	{ LED_1_OFF;								// um Eingangskapazität des Ports zu leeren, sonst würde am Eingang anschliessend immer eine 1 gelesen.
		DDRA  &= ~0x01;						// Port als Eingang definieren: Taste 1
		LED_1_ON;									// Ausgang wieder auf 1 zurücksetzen. ( Wird erst aktiv, wen Port wieder als Ausgang definiert ist.)
	}
	else DDRA  &= ~0x01;				// Port als Eingang definieren: Taste 1
	__asm__ __volatile__ ("nop");	// Warten, bis Eingang im Fall logisch 1 Pegel erreicht hat.
	__asm__ __volatile__ ("nop");
	__asm__ __volatile__ ("nop");

	if(Taste_1)	
	{ DDRA  |=  0x01;						// Port wieder als Ausgang definieren: Taste 1 auf Mocca Mini					 
		return(1);
	}
	else
	{ DDRA  |=  0x01;						// Port wieder als Ausgang definieren: Taste 1 auf Mocca Mini					 
		return(0);
	}	
}


u8 get_Taste_2(void)		
{
	if(LED_2_Input)							// Falls der Ausgang auf 1 gesetzt ist, diesen auf 0 setzen,
	{ LED_2_OFF;								// um Eingangskapazität des Ports zu leeren, sonst würde am Eingang anschliessend immer eine 1 gelesen.
		DDRA  &= ~0x02;						// Port als Eingang definieren: Taste 2
		LED_2_ON;									// Ausgang wieder auf 1 zurücksetzen. ( Wird erst aktiv, wen Port wieder als Ausgang definiert ist.)
	}
	else DDRA  &= ~0x02;				// Port als Eingang definieren: Taste 2
	if(Taste_2)	
	{ DDRA  |=  0x02;						// Port wieder als Ausgang definieren: Taste 2 auf Mocca Mini					 
		return(1);
	}
	else
	{ DDRA  |=  0x02;						// Port wieder als Ausgang definieren: Taste 2 auf Mocca Mini					 
		return(0);
	}	
}



u16 get_ADC_Channel(u8 channel)	
{	u16 AD_Result;
	u8  i;
	
	// A/D-Wandler Initialisieren
	ADMUXA = channel;		// select Channel 
	
	if(channel == 12) ADMUXB = 0x01;			// Wenn interner Temperatur-Sensor gewählt wird, muss die "Internal 1.1V Reference" selektiert sein.
						   else ADMUXB = 0x00;			// Vcc als Referenz = 5V
	// ADMUXB = 0x01;			// Internal 1.1V Reference
	// ADMUXB = 0x02;			// Internal 2.2V Reference
	// ADMUXB = 0x03;			// Internal 4.096V reference
	
	if(channel == 12) for(i=0; i<8; i++) delay_nop(120);		// 1.6ms (8x 0.2us) warten, bis interne Temperatur gemessen werden kann
	
	
	ADCSRA |=  (1 << ADEN) | (1 << ADSC);			// A/D-Wandler Enable, Start, Single Mode
	while(ADCSRA & (1 << ADSC));							// Warten, bis Messung fertig ist.
	
	AD_Result = ADCL + (ADCH << 8);			//	ADCL zuerst lesen und dann erst ADCH !!!
	
	return(AD_Result);
}

#pragma GCC optimize 2						// Optimitation-Level nicht verstellen, weil sonst das Timing der LCD-Schnittstelle nicht mehr stimmen würde!!!

void set_RGB_LED(u16 rot,u16 gruen,u16 blau)
{
	u8 i;
	u16 bit_muster;
	
	RGB_rot = rot;
	RGB_gruen = gruen;
	RGB_blau = blau;
	DDRA  |=  0x04;						// Port wieder als Ausgang definieren
	
	write_RGB_LCD_activ = 1;
	RGB_SHCP_0;
	RGB_OE_1;
	bit_muster =  0x8000;
	for(i=0; i<16; i++)
	{ if(blau  & bit_muster) RGB_DS_1; else RGB_DS_0;
		RGB_SHCP_1; RGB_SHCP_0;
		if(gruen & bit_muster) RGB_DS_1; else RGB_DS_0;
		RGB_SHCP_1; RGB_SHCP_0;
		if(rot   & bit_muster) RGB_DS_1; else RGB_DS_0;
		RGB_SHCP_1; RGB_SHCP_0;
		bit_muster >>= 1;
	}
	RGB_STCP_0;
	RGB_STCP_1;								// Daten ins "storage register" übernehmen
	write_RGB_LCD_activ = 0;
}


void init_Mocca_Mini(void)  //xyz ganze Funktion
{
		// CPU-Clock auf Maximum stellen

	CLKPR	= 0x00;			// System-Takt = 8MHz
	
	//OSCCAL0 = 102;		// 6.6.3 OSCCAL0 – Oscillator Calibration Register, Figure 26-77. Internal 8MHz RC Oscillator Frequency vs. OSCCAL0
	
	//OSCTCAL0A = 1;
	//OSCTCAL0B = 1;

	
	// Strom-Reduktion durch Abschaltung von:
	//PRR 	 |= (1 << PRADC);			// Analog to Digital Converter
	//PRR 	 |= (1 << PRUSI);			// SPI
	//ACSR	 |= (1 << ACD);				// Analog Comparator
	

	// Initialisierung des ADC's
	PRR 	 &= ~(1 << PRADC);			// Stromsparmodus des ADC's ausschalten
	ADCSRA |=  (1 << ADEN) | (1 << ADSC) | (0 << ADATE);			// A/D-Wandler Enable, Start, !Auto-Trigger
	
	//ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (0 << ADPS2);		// Clock = CPU /   8 ==> ADC_Clock =   1MHz ==> Conversen-Time =  13us  
	//ADCSRA |= (0 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);		// Clock = CPU /  64 ==> ADC_Clock = 125kHz ==> Conversen-Time = 104us  
	//ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);		// Clock = CPU / 128 ==> ADC_Clock = 125kHz ==> Conversen-Time = 208us  
	//ADCSRB |= (0 << ADTS0) | (0 << ADTS1) | (0 << ADTS2);		// Free Running mode
	
	// PWM für LED-Dimmer initialisieren
	DDRB			|= (1 << PB2);													// PB2 = Output
	TCCR0A		|= (1 << COM0A1) | (1 << WGM00);				// phase correct PWM mode A
	TCCR0A		|= (1 << COM0B1) | (1 << WGM00);				// phase correct PWM mode B
	
	// Ausgangs-Multiplexer einstellen
//	TOCPMSA1	&= ~((1 << TOCC4S1) | (1 << TOCC4S0));	// 00: Output MUX: 8Bit-PWM Timer/Counter0 OC0B auf Pin PB5 - Pin8 - TOCC4 schalten
//	TOCPMCOE	|=   (1 << TOCC4OE);										// Output enable TOCC4

//	TOCPMSA1	&= ~((1 << TOCC7S1) | (1 << TOCC7S0));	// 00: Output MUX: 8Bit-PWM Timer/Counter0 OC0A auf Pin PB2 - Pin5 - TOCC7 schalten
//	TOCPMCOE	|=   (1 << TOCC7OE);										// Output enable TOCC7
	
	OCR0A			 =   0;									// initial PWM pulse width A
	OCR0B			 =   10;								// initial PWM pulse width B
	// Bei System-Takt = 1MHz gilt:
	//TCCR0B |= (0 << CS00) | (1 << CS01);		// PWM-Grundtakt = 254Hz: clock source = CLK/8, start PWM (125kHz => 8us, 8us*255 *2 = 4.08ms => 254Hz)
	//TCCR0B |= (1 << CS00) | (0 << CS01);		// PWM-Grundtakt = 1.96kHz: clock source = CLK  , start PWM (  1MHz => 1us, 1us*255 *2 = 0.51ms => 1.96kHz)
	
	// Bei System-Takt = 8MHz gilt:
	TCCR0B |= (1 << CS00) | (1 << CS01);		// PWM-Grundtakt = 245Hz: clock source = CLK/64  , start PWM ( 125kHz => 8us, 8us*255 *2 = 4.08ms => 245Hz)
	//TCCR0B |= (0 << CS00) | (1 << CS01);		// PWM-Grundtakt = 1.96kHz: clock source = CLK/8  , start PWM ( 1MHz =>   1us,   1us*255 *2 = 0.51ms => 1.96kHz)
	//TCCR0B |= (1 << CS00) | (0 << CS01);		// PWM-Grundtakt = 15.68kHz: clock source = CLK  , start PWM (  8MHz => 125ns, 125ns*255 *2 = 0.63.75us => 15.68kHz)
	
	// 16-Bit-Timer für 1ms-Ticker
	GTCCR  |= (1 << TSM) | (1 << PSR  );  //Timer anhalten, Prescaler Reset
	TCNT1	= 0x1FF;														// 12.12.6 TCNTnH and TCNTnL – Timer/Counter
	TCCR1A |= (0 << COM1A1) | (0 << COM1A1);	// Normal Operation
	TCCR1A |= (0 <<  WGM11) | (0 <<  WGM10);	// CTC-Mode
	TCCR1B |= (0 <<  WGM13) | (1 <<  WGM12);	// CTC-Mode
	//TCCR1B |= (0 <<   CS12) | (0 <<   CS11) | (1 <<   CS10);	// Clock select CLK/1: 1'000'000Hz ==> f = 1'000'000Hz ==> t = 1us
	TCCR1B |= (0 <<   CS12) | (1 <<   CS11) | (0 <<   CS10);	// Clock select CLK/8: 8'000'000Hz ==> f = 1'000'000Hz ==> t = 1us
	TCCR1C |= (0 <<  FOC1A);					// no Output Compare
	OCR1A   = 1000-1;									// 1us * 1000 = 1ms
	TIMSK1 |= (1 << OCIE1A);					// Interrupt Enable
	GTCCR  &= ~(1 << TSM);						// Timer starten
	
	GIMSK  |= (1 << PCIE0);			// General Interrupt Mask Register für PCINT[7:0] any change
	PCMSK0 |= (1 << 2);					// Interrupt für SPI-Eingang setzen (Pin 11/PA2/PCINT2)

	sei();										// Global Interrupts aktivieren
	
	// Ports als EIN- oder AUSGANG initialisieren
	
	// Ausgänge
	//PUEA  &= ~0x03;			// PORTA Bit 0-1: Pull-Up Widerstand ausschalten
	DDRA  |=  0x03;			// LED 1 und 2 auf Mocca Mini
	PORTA &= ~0x03;			// LED 1 und 2 ausschalten
	DDRA  |=  0x02;			// Ausgang PA.1 Pin 12:		RGB storage register clock
	
	DDRA  |=  0x04;			// Ausgang PA.2 Pin 11:		RGB shift register clock
	DDRA  |=  0x20;			// freier I/O
	

	// Eingänge
	
	//DDRA  &= ~0x04;			// Eingang PA2 Pin 11:		Switch vom LCD-RGB-SWITCH einlesen
	DDRA  &= ~0x08;			// Eingang PA3 Pin 10:		A/D-Channel 3 Umgebungs-Temperatur
	DDRA  &= ~0x80;			// Eingang PA7 Pin  6:		A/D-Channel 7 Potentiometer
	
		
	/* Ausgänge für LCD werden im Treiber konfiguriert
	DDRA |= 0x10;								// Port PA4 auf Output setzen LCD-Outputs SOD
	DDRA |= 0x40;								// Port PA6 auf Output setzen LCD-Outputs SCLK	*/

	DDRA  |=  0x04;
	DDRB  |=  0x07;			// RGB-LED
	
	set_RGB_LED(0,0,0);
}


void read_SWITCH(void)
{
	u8  i;
	u16 bit_muster, bit_dip_nr, switch_wert;
	
	// Die korrekten Werte wieder ins RGB-Schieberegster schreiben, damit beim nächsten RGB_STCP-Puls (verursacht durch die korrekten Daten übernomen werden.
	RGB_SHCP_0;
	RGB_OE_1;
	bit_muster =  0x8000;
	for(i=0; i<16; i++)
	{ if(RGB_blau  & bit_muster) RGB_DS_1; else RGB_DS_0;
		RGB_SHCP_1; RGB_SHCP_0;
		if(RGB_gruen & bit_muster) RGB_DS_1; else RGB_DS_0;
		RGB_SHCP_1; RGB_SHCP_0;
		if(RGB_rot   & bit_muster) RGB_DS_1; else RGB_DS_0;
		RGB_SHCP_1; RGB_SHCP_0;
		bit_muster >>= 1;
	}
	RGB_STCP_0;
	RGB_STCP_1;								// Daten ins "storage register" übernehmen
	
	
	RGB_SHCP_0;							// um Eingangskapazität des Ports zu leeren, sonst würde am Eingang anschliessend immer eine 1 gelesen.
	DDRA  &= ~0x04;						// Port als Eingang definieren
	
	SWICH_P_Load_0;						// parallel load: paralleles Einlesen aller Eingänge (LOW-Pegel)
	SWICH_P_Load_1;						// serial shift : Ab jetzt werden die einzelnen Bits seriell herausgelesen. (HIGH-Pegel)
	
	switch_wert = 0;
	bit_muster = 1;
	SWITCH_CLK_0;						
	for(i=0; i<16; i++)
	{ 
		if(SWITCH_QH) switch_wert |= bit_muster;
		bit_muster = bit_muster << 1;
			
		SWITCH_CLK_1;
		SWITCH_CLK_0;
	}
	DDRA  |=  0x04;						// Port wieder als Ausgang definieren

	DIP_Switch = 0;
	bit_muster =   1;
	bit_dip_nr = 512;
	for(i=0; i<10; i++)
	{ if(switch_wert & bit_muster) DIP_Switch |= bit_dip_nr;
		bit_muster <<= 1;
		bit_dip_nr >>= 1;
	}
	if(switch_wert & 1024) Taste_1 = 1; else Taste_1 = 0;
	if(switch_wert & 2048) Taste_2 = 1; else Taste_2 = 0;
	if(switch_wert & 4096) Taste_3 = 1; else Taste_3 = 0;
	if(switch_wert & 8192) Taste_4 = 1; else Taste_4 = 0;

}


#pragma GCC optimize 0


// Interrupt-Funktion, die auslöst, wenn SPI-Eingang sich verändert (Pin 11/PA2/PCINT2)
ISR (PCINT0_vect) // Interrupt für Inputs PCINT[7:0]
{
}


ISR (TIMER1_COMPA_vect) // Dieser Timer-Interrupt löst im 1ms-Takt aus.
{	
	
	tick_1ms++;
	
	if(tick_1ms_cnt > 0) tick_1ms_cnt--;										// Wird für 1ms-Tick benötigt
	if(LCD_Refresh_Time_cnt > 0) LCD_Refresh_Time_cnt--;
	
	if (write_RGB_LCD_activ == 0) read_SWITCH();		// liesst alle DIP-Switch, Tasten und Optionen-Jumper ein und speichert die Zustaände in globalen Variablen ab.
	
}



