#include<avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>

#define BUFFER_SIZE 1024 //10 bit buffer

volatile uint8_t latest_sample;
volatile bool new_sample_ready = false;

uint8_t buffer[BUFFER_SIZE]; //circular buffer
uint16_t head = 0; //write pointer
uint16_t tail = 0; //read pointer


int main(void) {
  //setup:
  cli(); //clear local int
  Serial.begin(9600);

  DDRB |= (1 << DDB5); //onboard LED PB5 output. 

  DDRC &= ~(1 << DDC1); //set A1 as input (arduino pc1); need to read at 200Sa/s (sample evergy 5ms)
  
  //use ADC1, set Vref to ext 2.5V, left adjust ADC, use only 8-bit res, prescale 128
  ADMUX |= (1 << MUX0); //analog chan selection: ADC1
  ADMUX &= ~((1 << REFS0)|(1 << REFS1)); //AREF so need to supply 2.5V to AREF in hardware
  ADMUX |= (1 << ADLAR);  // left adjust
  ADCSRA |= (1 << ADEN)|(1 << ADIE); //adc enable and int enable.
  ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0); //division factor 128
  
  TCCR1B |= (1 << WGM12); //timer1 in CTC mode
  TCCR1B |= (1 << CS11) | (1 << CS10); // timer1 prescaler 64
  OCR1A = 1249; //timer1 reached this CV at 5ms hence OF int. 
  //OCR1A = 499999;
  TIMSK1 |= (1 << OCIE1A); //output compare match A int enable.

  sei(); 

  TCNT1 = 0;

  while(1) {
    //program will be getting a new adc_result every 5ms. need to store this adc_result in a ring buffer. 
    //also need to produce a digital output every time it acquires an adc sample as proof of sample rate
    if (new_sample_ready) {
      new_sample_ready = false;
      buffer[head] = latest_sample;
      head = (head + 1) % BUFFER_SIZE;
      PORTB ^= (1 << PORTB5);
    }
  } 
}

ISR(TIMER1_COMPA_vect) { //this occurs every 200ms. read ADC here
  ADCSRA |= (1 << ADSC); //start conversion. this triggers ISR.
  //Serial.print("hi");
}

ISR(ADC_vect) {
  latest_sample = ADCH;
  //Serial.print(latest_sample);
  new_sample_ready = true;
}
