


typedef unsigned char			 u8;
typedef unsigned int			u16;
typedef unsigned long int u32;

// Port-Definitionen
// LED 1 und 2 auf Mocca Mini
#define LED_1_ON						(PORTA |=  0x01)	//PA.0: LED_1 = EIN
#define LED_1_OFF						(PORTA &= ~0x01)	//PA.0: LED_1 = AUS
#define LED_1_TOGGLE				(PORTA ^=  0x01)	//PA.0: LED_1 = toggle
#define LED_1_Input					(PORTA &   0x01)	//PA.0: LED_1 (Kann zum Rücklesen des LED-Zustandes gebraucht werden.)

#define LED_2_ON						(PORTA |=  0x02)	//PA.1: LED_2 = EIN
#define LED_2_OFF						(PORTA &= ~0x02)	//PA.1: LED_2 = AUS
#define LED_2_TOGGLE				(PORTA ^=  0x02)	//PA.1: LED_2 = toggle
#define LED_2_Input					(PORTA &   0x02)	//PA.1: LED_2 (Kann zum Rücklesen des LED-Zustandes gebraucht werden.)

#define LED_1_2_ON					(PORTA |=  0x03)	//PA.0 und PA.1: LED_1 und LED_2 = EIN
#define LED_1_2_OFF					(PORTA &= ~0x03)	//PA.0 und PA.1: LED_1 und LED_2 = AUS

#define Taste_1_Mocca				(PINA  &  0x01)		//PA.0: Taste 1
#define Taste_2_Mocca				(PINA  &  0x02)		//PA.1: Taste 2

// RGB-LED auf Mocca Mini
#define LED_ROT_ON					(PORTB |=  0x01)	//PB.0: LED_ROT = EIN
#define LED_ROT_OFF					(PORTB &= ~0x01)	//PB.0: LED_ROT = AUS
#define LED_ROT_TOGGLE			(PORTB ^=  0x01)	//PB.0: LED_ROT = toggle
#define LED_GRUEN_ON				(PORTB |=  0x02)	//PB.1: LED_ROT = EIN
#define LED_GRUEN_OFF				(PORTB &= ~0x02)	//PB.1: LED_ROT = AUS
#define LED_GRUEN_TOGGLE		(PORTB ^=  0x02)	//PB.1: LED_ROT = toggle
#define LED_BLAU_ON					(PORTB |=  0x04)	//PB.2: LED_ROT = EIN
#define LED_BLAU_OFF				(PORTB &= ~0x04)	//PB.2: LED_ROT = AUS
#define LED_BLAU_TOGGLE			(PORTB ^=  0x04)	//PB.2: LED_ROT = toggle
#define LED_RGB_OFF					(PORTB &= ~0x07)	//PB.2: LED_RGB = AUS

// Erweiterungsboard LCD-RGB-SWICH
#define SWITCH_QH						(PINA  &   0x04)	//PA.2: Switch vom LCD-RGB-SWITCH
#define SWICH_P_Load_1			(PORTA |=  0x02)	//PA.4: Switch parallel load = 1
#define SWICH_P_Load_0			(PORTA &= ~0x02)	//PA.4: Switch parallel load = 0
#define SWITCH_CLK_1				(PORTA |=  0x10)	//PA.1: Clock = 1
#define SWITCH_CLK_0				(PORTA &= ~0x10)	//PA.1: Clock = 0

#define RGB_OE_0						(PORTA |=  0x01)	//PA.0: !Output Enable = 1
#define RGB_OE_1 						(PORTA &= ~0x01)	//PA.0: !Output Enable = 0
#define RGB_STCP_1					(PORTA |=  0x02)	//PA.1: storage register clock = 1
#define RGB_STCP_0					(PORTA &= ~0x02)	//PA.1: storage register clock = 0
#define RGB_DS_1						(PORTA |=  0x10)	//PA.4: serial data = 1
#define RGB_DS_0						(PORTA &= ~0x10)	//PA.4: serial data = 0
#define RGB_SHCP_1					(PORTA |=  0x04)	//PA.2: shift register clock = 1
#define RGB_SHCP_0					(PORTA &= ~0x04)	//PA.2: shift register clock = 0

// LCD: Siganlnamen für Daten und Clock (sind in der Treiberdatei definiert)
//#define LCD_SOD_0					(PORTA &= ~0x10)	//PA.4: LCD SOD = 0  (Serial Out Data)
//#define LCD_SOD_1					(PORTA |=  0x10)	//PA.4: LCD SOD = 1
//#define LCD_SCLK_0				(PORTA &= ~0x40)	//PA.6: LCD SCLK = 0 (Serial CLOCK)
//#define LCD_SCLK_1				(PORTA |=  0x40)	//PA.6: LCD SCLK = 1


#define PA5_EIN							(PORTA |=  0x20)	//PA.5: Test
#define PA5_AUS							(PORTA &= ~0x20)	//PA.5: Test