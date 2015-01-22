#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>

typedef uint32_t	u32;
typedef uint16_t	u16;
typedef uint8_t		u8;
typedef int32_t		s32;
typedef int16_t		s16;
typedef int8_t		s8;

#define DACP OCR1A
#define DACN OCR1B

__attribute__((naked)) int main(void)
{
	// Configure ports
	DDRB  = 0b00010010; // OC0AB are L/R PWM output. Drive unused pins low (helps noise).
	PORTB = 0b11100100; // No activate pull-ups

	// ADC
	ADMUX  = 0b11010011; // PB3, AVcc, 10 bit, 2.56VRef
	ADCSRA = 0b11000100; // CPU/16 sampling rate; Sampling freq: 16 MHz / 16 / 13 = 78 kHz
	//             ^ - interrupt enable
	// Power reduction
	PRR   = 0b00000110; // Clock-gate USI and Counter0.
	MCUCR = 0b00100000;


	// PWM
	//OCR1C = 255;
	TCCR1 = 0b01100010; // non-inv PWM, 32MHz / 512 = 62 kHz
	GTCCR = 0b01100000;
	TIMSK = 0b100;
	PLLCSR = 0b110;

	sei();

	while(1) {
		sleep_cpu();
	}
}


// 50% duty cycle at idle. Slightly less efficient, but better sounding.
u8 time;
u16 pos;
u16 neg;

ISR(TIMER1_OVF_vect) {

	if(1 & ++time) {
		u16 adc		= ADC;
		s16 val1	= (adc >> 1) - 256;
		u8 val2		= (u8)(adc & 1);

		pos = 256 + val1 + val2;
		neg = 256 - val1;

		if(val1 >= 254)
			pos = 510;

		if(val1 <= -255)
			neg = 510;

		if(pos > 255) {
			DACP = 255;
			pos -= 255;
		} else {
			DACP = pos;
			pos = 0;
		}

		if(neg > 255) {
			DACN = 255;
			neg -= 255;
		} else {
			DACN = neg;
			neg = 0;
		}

	} else {
		DACP = pos;
		DACN = neg;
	}
}



