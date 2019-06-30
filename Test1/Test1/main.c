/*
* Test1.c
*
* Created: 02.06.2019 20:08:46
* Author : koe
*/


#define F_CPU       (16000000UL)

#include <util/delay.h>
#include <util/atomic.h>

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "note_frequencies.h"

#define PWMDUTYCYCLE   5
#define BASELENGTH   1250 /* lenth of a full note in ms*/
#define INTERNOTEGAP   30
#define DEBOUNCELIMIT   500

#define STOP 0
#define PLAY 1

#define TRUE 1
#define FALSE 0
#define DEBUGGING TRUE

#define SONGS 6
#define SONG_A 0
#define SONG_B 1
#define SONG_C 2
#define SONG_D 3
#define SONG_E 4
#define SONG_F 5

static const uint16_t music[SONGS][400] PROGMEM = {

	/*** A-Team theme song ***/
	{	52, // array length
		NOTE_PAUSE,0x0004,
		NOTE_PAUSE,0x0004,
		NOTE_F5,0x0006,
		NOTE_C5,0x0003,
		NOTE_F5,0x000F,
		NOTE_B4,0x0003,
		NOTE_C5,0x0006,
		NOTE_F4,0x000C,
		NOTE_A4,0x0001,
		NOTE_C5,0x0001,
		NOTE_F5,0x0003,
		NOTE_C5,0x0003,
		NOTE_G5,0x0003,
		NOTE_F5,0x000F,
		NOTE_DIS5,0x0004,
		NOTE_D5,0x0001,
		NOTE_C5,0x0001,
		NOTE_B4,0x0004,
		NOTE_C5,0x000C,
		NOTE_F5,0x0004,
		NOTE_F5,0x0001,
		NOTE_C5,0x0003,
		NOTE_F5,0x000F,
		NOTE_A4,0x0003,
		NOTE_B4,0x0006,
		NOTE_C5,0x0003,
		NOTE_F4,0x000C,
		NOTE_GIS4,0x0004,
		NOTE_B4,0x0001,
		NOTE_CIS5,0x0003,
		NOTE_DIS5,0x0012,
		NOTE_PAUSE,0x0004,
		NOTE_DIS5,0x0003,
		NOTE_D5,0x0006,
		NOTE_B4,0x0003,
		NOTE_DIS5,0x0006,
		NOTE_D5,0x0006,
		NOTE_A4,0x0004,
		NOTE_B4,0x0001,
		NOTE_C5,0x0003,
		NOTE_F5,0x000F,
		NOTE_C5,0x0003,
		NOTE_B4,0x0006,
		NOTE_F4,0x0003,
		NOTE_C5,0x0006,
		NOTE_B4,0x0006,
		NOTE_B4,0x0003,
		NOTE_A4,0x0003,
		NOTE_F4,0x0003,
		NOTE_E4,0x0003,
		NOTE_F4,0x000C,
		NOTE_PAUSE,0x0004
	},
	
	/*** Imperial March ***/
	{
		73, // array length
		NOTE_PAUSE,0x0004,
		NOTE_PAUSE,0x0004,
		NOTE_G4,0x0008,
		NOTE_G4,0x0008,
		NOTE_G4,0x0008,
		NOTE_DIS4,0x0006,
		NOTE_B4,0x0002,
		NOTE_G4,0x0008,
		NOTE_DIS4,0x0006,
		NOTE_B4,0x0002,
		NOTE_G4,0x0010,
		NOTE_D5,0x0008,
		NOTE_D5,0x0008,
		NOTE_D5,0x0008,
		NOTE_DIS5,0x0006,
		NOTE_B4,0x0002,
		NOTE_FIS4,0x0008,
		NOTE_DIS4,0x0006,
		NOTE_B4,0x0002,
		NOTE_G4,0x0010,
		NOTE_G5,0x0008,
		NOTE_G4,0x0006,
		NOTE_G4,0x0002,
		NOTE_G5,0x0008,
		NOTE_FIS5,0x0006,
		NOTE_F5,0x0002,
		NOTE_E5,0x0002,
		NOTE_DIS5,0x0002,
		NOTE_E5,0x0004,
		NOTE_PAUSE,0x0004,
		NOTE_GIS4,0x0004,
		NOTE_CIS5,0x0008,
		NOTE_C5,0x0006,
		NOTE_H4,0x0002,
		NOTE_B4,0x0002,
		NOTE_A4,0x0002,
		NOTE_B4,0x0002,
		NOTE_PAUSE,0x0004,
		NOTE_DIS4,0x0004,
		NOTE_FIS4,0x0008,
		NOTE_DIS4,0x0006,
		NOTE_FIS4,0x0002,
		NOTE_B4,0x0008,
		NOTE_G4,0x0006,
		NOTE_B4,0x0002,
		NOTE_D5,0x0010,
		NOTE_G5,0x0008,
		NOTE_G4,0x0006,
		NOTE_G4,0x0002,
		NOTE_G5,0x0008,
		NOTE_FIS5,0x0006,
		NOTE_F5,0x0002,
		NOTE_E5,0x0002,
		NOTE_DIS5,0x0002,
		NOTE_E5,0x0004,
		NOTE_PAUSE,0x0004,
		NOTE_GIS4,0x0004,
		NOTE_CIS5,0x0008,
		NOTE_C5,0x0006,
		NOTE_H4,0x0002,
		NOTE_B4,0x0002,
		NOTE_A4,0x0002,
		NOTE_B4,0x0002,
		NOTE_PAUSE,0x0004,
		NOTE_DIS4,0x0004,
		NOTE_FIS4,0x0008,
		NOTE_DIS4,0x0006,
		NOTE_B4,0x0002,
		NOTE_G4,0x0008,
		NOTE_DIS4,0x0006,
		NOTE_B4,0x0002,
		NOTE_G4,0x0010,
		NOTE_PAUSE,0x0004
	},
	
	/*** Game of Thrones ***/
	{
		60, // array length
		NOTE_PAUSE,0x0004,
		NOTE_PAUSE,0x0008,
		NOTE_G4,0x000C,
		NOTE_C4,0x000C,
		NOTE_DIS4,0x0002,
		NOTE_F4,0x0002,
		NOTE_G4,0x0008,
		NOTE_C4,0x0008,
		NOTE_DIS4,0x0002,
		NOTE_F4,0x0002,
		NOTE_D4,0x0024,
		NOTE_PAUSE,0x0004,
		NOTE_F4,0x000C,
		NOTE_B3,0x000C,
		NOTE_DIS4,0x0002,
		NOTE_D4,0x0002,
		NOTE_F4,0x0008,
		NOTE_B3,0x0008,
		NOTE_DIS4,0x0002,
		NOTE_D4,0x0002,
		NOTE_C4,0x0024,
		NOTE_PAUSE,0x0004,
		NOTE_G5,0x000C,
		NOTE_C5,0x000C,
		NOTE_DIS5,0x0002,
		NOTE_F5,0x0002,
		NOTE_G5,0x0008,
		NOTE_C5,0x0008,
		NOTE_DIS5,0x0002,
		NOTE_F5,0x0002,
		NOTE_D5,0x0024,
		NOTE_PAUSE,0x0004,
		NOTE_F5,0x000C,
		NOTE_B4,0x000C,
		NOTE_DIS5,0x0002,
		NOTE_D5,0x0002,
		NOTE_F5,0x0008,
		NOTE_B4,0x0008,
		NOTE_DIS5,0x0002,
		NOTE_D5,0x0002,
		NOTE_C5,0x0024,
		NOTE_PAUSE,0x0004,
		NOTE_G5,0x000C,
		NOTE_C5,0x000C,
		NOTE_DIS5,0x0002,
		NOTE_F5,0x0002,
		NOTE_G5,0x0008,
		NOTE_C5,0x0008,
		NOTE_DIS5,0x0002,
		NOTE_F5,0x0002,
		NOTE_D5,0x0024,
		NOTE_PAUSE,0x0004,
		NOTE_F5,0x000C,
		NOTE_B4,0x000C,
		NOTE_D5,0x0008,
		NOTE_DIS5,0x0004,
		NOTE_D5,0x0008,
		NOTE_B4,0x0004,
		NOTE_C5,0x0024,
		NOTE_PAUSE,0x0008
	},
	
	/*** Time to say Goodbye ***/
	{
		114, // array length
		NOTE_PAUSE,0x0008,
		NOTE_PAUSE,0x0008,
		NOTE_D4,0x000E,
		NOTE_G4,0x001C,
		NOTE_FIS4,0x0007,
		NOTE_A4,0x0007,
		NOTE_G4,0x0031,
		NOTE_A4,0x0007,
		NOTE_H4,0x0004,
		NOTE_H4,0x0005,
		NOTE_H4,0x0005,
		NOTE_H4,0x001C,
		NOTE_A4,0x0004,
		NOTE_C5,0x0005,
		NOTE_H4,0x0005,
		NOTE_G4,0x0015,
		NOTE_G5,0x0007,
		NOTE_FIS5,0x0004,
		NOTE_D5,0x0005,
		NOTE_G5,0x0005,
		NOTE_FIS5,0x0004,
		NOTE_D5,0x0005,
		NOTE_H4,0x0005,
		NOTE_D5,0x001C,
		NOTE_A4,0x0004,
		NOTE_C5,0x0005,
		NOTE_H4,0x0005,
		NOTE_A4,0x0004,
		NOTE_G4,0x0005,
		NOTE_FIS4,0x0005,
		NOTE_D4,0x000E,
		NOTE_G4,0x001C,
		NOTE_FIS4,0x0007,
		NOTE_A4,0x0007,
		NOTE_G4,0x0031,
		NOTE_A4,0x0007,
		NOTE_H4,0x0004,
		NOTE_H4,0x0005,
		NOTE_H4,0x0005,
		NOTE_H4,0x001C,
		NOTE_A4,0x0004,
		NOTE_C5,0x0005,
		NOTE_H4,0x0005,
		NOTE_G4,0x0015,
		NOTE_G5,0x0007,
		NOTE_FIS5,0x0004,
		NOTE_D5,0x0005,
		NOTE_G5,0x0005,
		NOTE_FIS5,0x0004,
		NOTE_D5,0x0005,
		NOTE_H4,0x0005,
		NOTE_D5,0x001C,
		NOTE_A4,0x0004,
		NOTE_C5,0x0005,
		NOTE_H4,0x0005,
		NOTE_A4,0x0004,
		NOTE_G4,0x0005,
		NOTE_FIS4,0x0005,
		NOTE_E4,0x0007,
		NOTE_A4,0x0007,
		NOTE_A4,0x001C,
		NOTE_GIS4,0x0007,
		NOTE_H4,0x0007,
		NOTE_A4,0x0031,
		NOTE_H4,0x0007,
		NOTE_CIS5,0x0004,
		NOTE_CIS5,0x0005,
		NOTE_CIS5,0x0005,
		NOTE_CIS5,0x001C,
		NOTE_H4,0x0004,
		NOTE_D5,0x0005,
		NOTE_CIS5,0x0005,
		NOTE_A4,0x0015,
		NOTE_A5,0x0007,
		NOTE_GIS5,0x0004,
		NOTE_E5,0x0005,
		NOTE_A5,0x0005,
		NOTE_GIS5,0x0004,
		NOTE_E5,0x0005,
		NOTE_CIS5,0x0005,
		NOTE_E5,0x001C,
		NOTE_H4,0x0004,
		NOTE_D5,0x0005,
		NOTE_CIS5,0x0005,
		NOTE_H4,0x0004,
		NOTE_A4,0x0005,
		NOTE_GIS4,0x0005,
		NOTE_E4,0x0007,
		NOTE_A4,0x0007,
		NOTE_A4,0x001C,
		NOTE_GIS4,0x0007,
		NOTE_H4,0x0007,
		NOTE_A4,0x0031,
		NOTE_H4,0x0007,
		NOTE_CIS5,0x0004,
		NOTE_CIS5,0x0005,
		NOTE_CIS5,0x0005,
		NOTE_CIS5,0x001C,
		NOTE_H4,0x0004,
		NOTE_D5,0x0005,
		NOTE_CIS5,0x0005,
		NOTE_A4,0x0015,
		NOTE_A5,0x0007,
		NOTE_GIS5,0x0004,
		NOTE_E5,0x0005,
		NOTE_A5,0x0005,
		NOTE_GIS5,0x0004,
		NOTE_E5,0x0005,
		NOTE_CIS5,0x0005,
		NOTE_E5,0x001C,
		NOTE_FIS5,0x000E,
		NOTE_GIS5,0x000E,
		NOTE_A5,0x002A,
		NOTE_PAUSE,0x0008	
	},
	
	/*** Fuchsgrabenpolka ***/
	{
		138, // array length
		NOTE_PAUSE,0x0003,
		NOTE_PAUSE,0x0008,
		NOTE_F4,0x0009,		// erster Teil
		NOTE_A4,0x0003,
		NOTE_B4,0x0009,
		NOTE_A4,0x0003,
		NOTE_F4,0x0015,
		NOTE_PAUSE,0x0003,
		NOTE_F4,0x0009,
		NOTE_A4,0x0003,
		NOTE_B4,0x0003,
		NOTE_A4,0x0003,
		NOTE_G4,0x0003,
		NOTE_F4,0x0003,
		NOTE_E4,0x0015,
		NOTE_PAUSE,0x0003,
		NOTE_E4,0x0003,
		NOTE_DIS4,0x0003,
		NOTE_E4,0x0003,
		NOTE_F4,0x0003,
		NOTE_G4,0x0003,
		NOTE_FIS4,0x0003,
		NOTE_G4,0x0003,
		NOTE_A4,0x0003,
		NOTE_C5,0x000F,
		NOTE_PAUSE,0x0003,
		NOTE_B4,0x0003,
		NOTE_A4,0x0003,
		NOTE_G4,0x000F,
		NOTE_PAUSE,0x0003,
		NOTE_A4,0x0003,
		NOTE_B4,0x0003,
		NOTE_A4,0x000F,
		NOTE_PAUSE,0x0003,
		NOTE_C5,0x0006,
		NOTE_F4,0x0009,
		NOTE_A4,0x0003,
		NOTE_B4,0x0009,
		NOTE_A4,0x0003,
		NOTE_F4,0x0015,
		NOTE_PAUSE,0x0003,
		NOTE_F4,0x0009,
		NOTE_G4,0x0003,
		NOTE_A4,0x0003,
		NOTE_F4,0x0003,
		NOTE_G4,0x0003,
		NOTE_A4,0x0003,
		NOTE_B4,0x000F,
		NOTE_PAUSE,0x0003,
		NOTE_D5,0x0003,
		NOTE_CIS5,0x0003,
		NOTE_E5,0x000F,
		NOTE_D5,0x0003,
		NOTE_B4,0x0005,
		NOTE_D5,0x0001,
		NOTE_A4,0x000F,
		NOTE_PAUSE,0x0003,
		NOTE_G4,0x0003,
		NOTE_F4,0x0003,
		NOTE_E4,0x0003,
		NOTE_PAUSE,0x0003,
		NOTE_D5,0x0003,
		NOTE_C5,0x0006,
		NOTE_E4,0x0003,
		NOTE_A4,0x0003,
		NOTE_G4,0x0003,
		NOTE_F4,0x000F,
		NOTE_PAUSE,0x0003,
		NOTE_F4,0x0003,
		NOTE_PAUSE,0x0003,
		NOTE_F4,0x0001,		// Trio
		NOTE_G4,0x0001,
		NOTE_A4,0x0001,
		NOTE_B4,0x0003,
		NOTE_PAUSE,0x0003,
		NOTE_G4,0x0001,
		NOTE_PAUSE,0x0002,
		NOTE_DIS4,0x0001,
		NOTE_PAUSE,0x0002,
		NOTE_B3,0x0001,
		NOTE_PAUSE,0x0002,
		NOTE_D4,0x0003,
		NOTE_DIS4,0x0003,
		NOTE_E4,0x0003,
		NOTE_F4,0x0012,
		NOTE_DIS4,0x0006,
		NOTE_D4,0x0006,
		NOTE_C4,0x0006,
		NOTE_DIS4,0x0006,
		NOTE_D4,0x0006,
		NOTE_B3,0x0012,
		NOTE_G3,0x0006,
		NOTE_F3,0x000F,
		NOTE_PAUSE,0x0003,
		NOTE_DIS4,0x0003,
		NOTE_D4,0x0003,
		NOTE_H3,0x0006,
		NOTE_C4,0x0009,
		NOTE_PAUSE,0x0002,
		NOTE_F3,0x0006,
		NOTE_A3,0x0006,
		NOTE_C4,0x0006,
		NOTE_F4,0x0006,
		NOTE_DIS4,0x0006,
		NOTE_CIS4,0x0006,
		NOTE_D4,0x000C,
		NOTE_DIS4,0x0006,
		NOTE_D4,0x000F,
		NOTE_D5,0x0003,
		NOTE_DIS5,0x0003,
		NOTE_E5,0x0003,
		NOTE_F5,0x0012,
		NOTE_DIS5,0x0006,
		NOTE_D5,0x0006,
		NOTE_C5,0x0006,
		NOTE_DIS5,0x0006,
		NOTE_D5,0x0006,
		NOTE_B4,0x0012,
		NOTE_G4,0x0006,
		NOTE_F4,0x000F,
		NOTE_PAUSE,0x0003,
		NOTE_DIS5,0x0003,
		NOTE_D5,0x0003,
		NOTE_H4,0x0006,
		NOTE_C5,0x0009,
		NOTE_PAUSE,0x0002,
		NOTE_F4,0x0006,
		NOTE_A4,0x0006,
		NOTE_C5,0x0006,
		NOTE_F5,0x0006,
		NOTE_DIS5,0x0006,
		NOTE_D5,0x000C,
		NOTE_C5,0x0009,
		NOTE_A4,0x0003,
		NOTE_B4,0x000F,
		NOTE_PAUSE,0x0003,
		NOTE_B4,0x0003,
		NOTE_PAUSE,0x0008
	},
	
	/*** Donauwalzer ***/
	{	173, // array length
		0x0000,0x0004,
		0x0000,0x0004,
		0x0126,0x0004,
		0x0172,0x0004,
		0x01B8,0x0004,
		0x01B8,0x0008,
		0x0370,0x0004,
		0x0370,0x0004,
		0x0000,0x0004,
		0x02E4,0x0004,
		0x02E4,0x0004,
		0x0000,0x0004,
		0x0126,0x0004,
		0x0126,0x0004,
		0x0172,0x0004,
		0x01B8,0x0004,
		0x01B8,0x0008,
		0x0370,0x0004,
		0x0370,0x0004,
		0x0000,0x0004,
		0x0310,0x0004,
		0x0310,0x0004,
		0x0000,0x0004,
		0x0115,0x0004,
		0x0115,0x0004,
		0x014A,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0008,
		0x03DC,0x0004,
		0x03DC,0x0004,
		0x0000,0x0004,
		0x0310,0x0004,
		0x0310,0x0004,
		0x0000,0x0004,
		0x0115,0x0004,
		0x0115,0x0004,
		0x014A,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0008,
		0x03DC,0x0004,
		0x03DC,0x0004,
		0x0000,0x0004,
		0x02E4,0x0004,
		0x02E4,0x0004,
		0x0000,0x0004,
		0x0126,0x0004,
		0x0126,0x0004,
		0x0172,0x0004,
		0x01B8,0x0004,
		0x024B,0x0008,
		0x0497,0x0004,
		0x0497,0x0004,
		0x0000,0x0004,
		0x0370,0x0004,
		0x0370,0x0004,
		0x0000,0x0004,
		0x0126,0x0004,
		0x0126,0x0004,
		0x0172,0x0004,
		0x01B8,0x0004,
		0x024B,0x0008,
		0x0497,0x0004,
		0x0497,0x0004,
		0x0000,0x0004,
		0x03DC,0x0004,
		0x03DC,0x0004,
		0x0000,0x0004,
		0x014A,0x0004,
		0x014A,0x0004,
		0x0188,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0010,
		0x019F,0x0004,
		0x01B8,0x0004,
		0x02E4,0x0010,
		0x024B,0x0004,
		0x0172,0x0004,
		0x0172,0x0008,
		0x014A,0x0004,
		0x01EE,0x0008,
		0x01B8,0x0004,
		0x0126,0x0004,
		0x0000,0x0008,
		0x024B,0x0004,
		0x022A,0x0004,
		0x022A,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0004,
		0x0000,0x0004,
		0x01EE,0x0004,
		0x01D2,0x0004,
		0x01D2,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0004,
		0x0000,0x0004,
		0x014A,0x0004,
		0x014A,0x0004,
		0x0172,0x0008,
		0x014A,0x0004,
		0x0000,0x0004,
		0x014A,0x0004,
		0x014A,0x0004,
		0x01EE,0x0008,
		0x01B8,0x0004,
		0x0000,0x0004,
		0x024B,0x0004,
		0x022A,0x0004,
		0x022A,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0004,
		0x0000,0x0004,
		0x01EE,0x0004,
		0x022A,0x0004,
		0x0293,0x0004,
		0x024B,0x0004,
		0x024B,0x0004,
		0x0000,0x0004,
		0x019F,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0008,
		0x01B8,0x0004,
		0x019F,0x0008,
		0x0172,0x0004,
		0x0172,0x0004,
		0x0000,0x0004,
		0x014A,0x0004,
		0x01B8,0x0004,
		0x024B,0x0004,
		0x022A,0x0004,
		0x022A,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0004,
		0x0000,0x0004,
		0x01EE,0x0004,
		0x01D2,0x0004,
		0x01D2,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0004,
		0x0000,0x0004,
		0x014A,0x0004,
		0x014A,0x0004,
		0x0172,0x0008,
		0x014A,0x0004,
		0x0000,0x0004,
		0x014A,0x0004,
		0x014A,0x0004,
		0x01EE,0x0008,
		0x01B8,0x0004,
		0x0000,0x0004,
		0x024B,0x0004,
		0x022A,0x0004,
		0x022A,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0004,
		0x0000,0x0004,
		0x01EE,0x0004,
		0x022A,0x0004,
		0x0293,0x0004,
		0x024B,0x0004,
		0x024B,0x0004,
		0x0000,0x0004,
		0x019F,0x0004,
		0x01EE,0x0004,
		0x01EE,0x0008,
		0x01B8,0x0004,
		0x019F,0x0008,
		0x0172,0x0004,
		0x0172,0x0004,
		0x0000,0x0004,
		0x014A,0x0004,
		0x01B8,0x0004,
		0x0000,0x0004,
		0x0000,0x0004
	}
};



void initTimer0(void);
void initTimer1(void);
void initButtons(void);
void initDebugging(void);

void playToneBackend(uint16_t duration_ms, uint16_t frequency_hz, uint8_t internotegap_ms);

void setDebuggingOutput(){
	#if DEBUGGING == TRUE
	DDRB|=(1<<PB5);
	#endif
	
}
void toggleDebuggingLed(){
	#if DEBUGGING == TRUE
	PORTB^=(1<<PB5);
	#endif
}

void setDebuggingLed(){
	#if DEBUGGING == TRUE
	PORTB|=(1<<PB5);
	#endif
}

void clearDebuggingLed(){
	#if DEBUGGING == TRUE
	PORTB&=~(1<<PB5);
	#endif
}


uint16_t note2duration(uint16_t theNote){
	return (uint16_t)((BASELENGTH/16) * theNote);
}



uint16_t calcTOP(uint16_t frequency){
	/* we can not handle frequencies below 120Hz*/
	if (frequency < 120){
		frequency = 120;
	}
	return(F_CPU / 2 / frequency);
}

uint16_t calcOCVal(uint16_t frequency){
	return( calcTOP(frequency)/100*PWMDUTYCYCLE );
}

void setPWM(uint16_t frequency){
	
	/* we can not handle frequencies below 120Hz*/
	if (frequency < 120){
		
		//disable pwm output
		TCCR1A &= ~( 1<<COM1A1 | 1<<COM1A0);
		
	} else {
		
		OCR1A = calcOCVal(frequency);
		ICR1 = calcTOP(frequency);
		
		TCCR1A |=  1<<COM1A1 ;
		
	}
}

/* variables */




volatile uint16_t remainingDuration=0;
volatile uint8_t  currentInterNoteGap=0;

uint16_t musicPosition=0;

int main(void){

	uint16_t songSize = 0;
	uint16_t currentFrequency = 0;
	uint16_t currentDuration = 0;
	
	uint8_t state=STOP;
	uint8_t stateSong=SONG_A;
	uint16_t debounceBtnOnOff=0;
	uint16_t debounceBtnSong=0;
	
	/* do initializations */
	initButtons();
	initTimer0();
	initTimer1();
	
	#if DEBUGGING == TRUE
	initDebugging();
	#endif
	
	/*  enable interrupts */
	sei();
	
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();

	/* main work loop */
	while(TRUE){
		/* check if state change is needed (pressed button)*/		
		if ((PIND & (1<<PD3)) == 0){
			debounceBtnOnOff++;
			} else {
			debounceBtnOnOff = 0;
		}
		if ((PIND & (1<<PD4)) == 0){
			debounceBtnSong++;
			} else {
			debounceBtnSong = 0;
		}
		
		if (state == STOP){
			if (debounceBtnOnOff == DEBOUNCELIMIT){
				state = PLAY;
				debounceBtnOnOff = 0;
				musicPosition = 0;
			}
		} else {	// state == PLAY
			if (debounceBtnOnOff >= 2*DEBOUNCELIMIT) {
				state = STOP;
				debounceBtnOnOff = 0;
				debounceBtnSong = 0;
			}
			else if (debounceBtnSong >= DEBOUNCELIMIT) {
				musicPosition = 0;
				debounceBtnSong = 0;
				stateSong += 1;
				if (stateSong==SONGS) {
					stateSong = 0;
				}
			}
		}
		
		if (state == PLAY){
			setDebuggingLed();
			/* if next note should be played */
			if(remainingDuration == 0){
				
				/* fetch music date */
				switch (stateSong)
				{
					case SONG_A:
						currentFrequency = pgm_read_word(&music[SONG_A][2*musicPosition+1]);
						currentDuration =  pgm_read_word(&music[SONG_A][2*musicPosition+2]);
						songSize = music[SONG_A][0];
						break;
					case SONG_B:
						currentFrequency = pgm_read_word(&music[SONG_B][2*musicPosition+1]);
						currentDuration =  pgm_read_word(&music[SONG_B][2*musicPosition+2]);
						songSize = music[SONG_B][0];
						break;
					case SONG_C:
						currentFrequency = pgm_read_word(&music[SONG_C][2*musicPosition+1]);
						currentDuration =  pgm_read_word(&music[SONG_C][2*musicPosition+2]);
						songSize = music[SONG_C][0];
						break;
					case SONG_D:
						currentFrequency = pgm_read_word(&music[SONG_D][2*musicPosition+1]);
						currentDuration =  pgm_read_word(&music[SONG_D][2*musicPosition+2]);
						songSize = music[SONG_D][0];
						break;
					case SONG_E:
						currentFrequency = pgm_read_word(&music[SONG_E][2*musicPosition+1]);
						currentDuration =  pgm_read_word(&music[SONG_E][2*musicPosition+2]);
						songSize = music[SONG_E][0];
						break;
					case SONG_F:
						currentFrequency = pgm_read_word(&music[SONG_F][2*musicPosition+1]);
						currentDuration =  pgm_read_word(&music[SONG_F][2*musicPosition+2]);
						songSize = music[SONG_F][0];
						break;
				}
				
				
				/* if there are more notes to play */
				if(musicPosition >= songSize){ //music[stateSong][0]){
					/* we have finished the track - stop */
					state = STOP;
					setPWM(0);
				} else {
					/* play tone */
					playToneBackend(note2duration(currentDuration), currentFrequency, INTERNOTEGAP);
					musicPosition++;
				}
			}
		}else{
			clearDebuggingLed();
			setPWM(0);
		}
		/* Set CPU to sleep mode. */
		sleep_cpu();
	}
	return 0;
}


void initTimer0(void){
	/* set up time progressing timer (1ms interrupt frequency) */
	TCCR0A  = 0x02; //CTC Mode
	OCR0A   = F_CPU/64/1000-1; //top for CTC mode
	TIMSK0 |= (1<<OCIE0A);
	TCCR0B  = 0x03; //run with PS=64
}

void initTimer1(void){
	/* set up pwm pin */
	DDRB |= (1<<PB1);
	/* set up the hardware pwm timer */
	TCCR1A=0x80;
	TCCR1B=0x10;
	/*turn off pwm*/
	setPWM(0);
	/* start the timer. no ps */
	TCCR1B=0x11;
}


void initButtons(void){
	/* set PortD3 + PortD4 as input with Pullup */
	PORTD |= (1<<PD3);
	DDRD &= ~(1<<PD3);
	PORTD |= (1<<PD4);
	DDRD &= ~(1<<PD4);
}

void initDebugging(void){
	clearDebuggingLed();
	setDebuggingOutput();
}

uint16_t isrcount=0;

/* Blocking timer interrupt progressing the time */
/* is called every millisecond */
ISR(TIMER0_COMPA_vect, ISR_BLOCK){
	/* check if we have something to do */
	if(remainingDuration > 0){
		/* handle internote gap if needed */
		if(remainingDuration == currentInterNoteGap){
			/* keep silent during gap*/
			setPWM( 0 );
		}
		/* progress time */
		--remainingDuration;
	}
}

/* starts playback of a tone with given frequency, duration, and following inter-note-gap  */
void playToneBackend(uint16_t duration_ms, uint16_t frequency_hz, uint8_t internotegap_ms){
	/* set up tone */
	remainingDuration = duration_ms + internotegap_ms;
	currentInterNoteGap = internotegap_ms;
	setPWM( frequency_hz );
	/* debugging */
	//toggleDebuggingLed();
}


