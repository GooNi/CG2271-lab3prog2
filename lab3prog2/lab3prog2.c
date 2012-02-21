/*
 * lab3prog2.c
 *
 * Created: 18/2/2012 12:13:44 PM
 *  Author: Desmond Wee
 */ 

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <math.h>
#include <avr/interrupt.h>

// val is from ADC
int remap(int val, int min_val, int max_val)
{
	double num = val-min_val;
	double dom = max_val-min_val;
	
	int result = (num/dom)*255;

	if(result<0)		//If ADC value is below min value
		return 0;
	
	if(result>255)		//If ADC value is above max value
		return 255;
	
	return result;
}

void tone(int input)
{
	//i rep the number of wave within 1 sec
	int freq; 
	double period, division;
	
	division = input/255.0;
	
	freq = division*400+100;   //100 is the least freq, 500 is the highest freq
	period = 1.0/freq;			  //eg. 0.01s
	
	double delay = period/2*pow(10,3); 
	
	for(int i=0; i<freq; i++)
	{
		// Write a 1 to digital pin 13 (PINB 5)
		PINB|= 0b00100000;
		_delay_ms(delay);
		//Delay is half the period (in millisecond)
	
		// Write a 0 to digital pin 13 (PINB 5)
		PINB &= 0b11011111;
		_delay_ms(delay);
		//Delay is half the period (in millisecond)
	}
}

unsigned adcval, constrained;

ISR(ADC_vect)
{
	unsigned loval, hival;
	
	// Read the result from the registers
	// Read ADC value
	loval=ADCL;
	hival=ADCH;
	adcval = hival*256+loval;
	
	// Re-start the conversion
	ADCSRA |= 0b01000000;
}

int main(void)
{
	//Set GPIO Pins
	DDRB|=0b00100000;  //PIN 13 output to buzzer
	
	//PRR to ADC (0 to ON)
	PRR &= 0b11111110;
	
	//Enable ADC, don't start conversion, enable ADIE (ADC interrupt)
	//and set ADPS2-0 to 111 to set prescalar value of 128
	ADCSRA|=0b10001111;
	
	//Set Ref Voltage (Vcc) and Channel Mux (Chn 1)
	ADMUX |= 0b01000001;
	
	//Start ADC conversion by writing a 1 to ADSC bit.
	ADCSRA |= 0b01000000;
	
	while(1)
	{
		//0-255 steps for tone strength 
	    constrained = remap(adcval,619,890);
		
		//Output tone )
		tone(constrained);
	}	
}