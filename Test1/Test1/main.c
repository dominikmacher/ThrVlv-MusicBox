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


#define MUSICLENGTH 170

#define PWMDUTYCYCLE   5
#define BASELENGTH   1250 /* lenth of a full note in ms*/
#define INTERNOTEGAP   30
#define DEBOUNCELIMIT   500

#define STOP 0
#define PLAY 1


#define TRUE 1
#define FALSE 0
#define DEBUGGING TRUE


/* 140x uint16_t  -> 70 freq + notes */
static const uint16_t music[] PROGMEM = {
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
	0x0000,0x0004,
	0x0000,0x0004,
	0x0000,0x0004
};








void initTimer0(void);
void initTimer1(void);
void initButton(void);
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




volatile uint16_t elapsedDuration=0;
volatile uint8_t  currentInterNoteGap=0;

uint16_t musicPosition=0;

int main(void){

	uint16_t currentFrequency = 0;
	uint16_t currentDuration = 0;
	
	uint8_t state=STOP;
	uint16_t debounce=0;
	
	/* do initializations */
	initButton();
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
			debounce++;
		} else {
			debounce = 0;
		}
		if (state == STOP){
			if (debounce == DEBOUNCELIMIT){
				state = PLAY;
				debounce = 0;
				musicPosition = 0;
			}
		} else {
			if (debounce >= 2*DEBOUNCELIMIT){
				state = STOP;
				debounce = 0;
			}
		}
		
		if (state == PLAY){
			setDebuggingLed();
			/* if next note should be played */
			if(elapsedDuration == 0){
				/* if there are more notes to play */
				if(musicPosition >= MUSICLENGTH){
					/* we have finished the track - stop */
					state = STOP;
					setPWM(0);
				} else {
					/* fetch music date */
					currentFrequency = pgm_read_word(&music[2*musicPosition]);
					currentDuration =  pgm_read_word(&music[2*musicPosition+1]);
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


void initButton(void){
	/* set PortD3 as input with Pullup */
	PORTD |= (1<<PD3);
	DDRD &= ~(1<<PD3);
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
	if(elapsedDuration > 0){
		/* handle internote gap if needed */
		if(elapsedDuration == currentInterNoteGap){
			/* keep silent during gap*/
			setPWM( 0 );
		}
		/* progress time */
		--elapsedDuration;
	}
}

/* starts playback of a tone with given frequency, duration, and following inter-note-gap  */
void playToneBackend(uint16_t duration_ms, uint16_t frequency_hz, uint8_t internotegap_ms){
	/* set up tone */
	elapsedDuration = duration_ms + internotegap_ms;
	currentInterNoteGap = internotegap_ms;
	setPWM( frequency_hz );
	/* debugging */
	//toggleDebuggingLed();
}


