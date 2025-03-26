#include <avr/interrupt.h>

volatile uint8_t adc_result;
void pulse(uint8_t);


int main(void) {
  //setup:
  cli();
  DDRB |= (1 << DDB3); //set PORTB3 as an output pin.
  DDRB |= (1 << DDB5);//onboard LED PB5 output. 

  ADMUX |= (1 << MUX0); //using ADC1
  ADMUX |= (1 << REFS0);
  ADCSRA |= (1 << ADEN)|(1 << ADIE); //adc enable and int enable.
  ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0); //division factor 128
  ADCSRB |= (); //this enables auto trigger source. i want adc to trigger ISR to change state to either >2v or <2v depending on the reading. so every time an adc reading is completed it will go to the isr and update the adc value. 
  
  
  DDRC &= ~(1 << DDC1); //set pin A1 as input. (later read PINC1)
  TCCR1B |= (1 << WGM12); //timer1 in CTC mode. 
  TCCR1B |= (1 << CS12)|(1 << CS10); //timer1 prescaler 1024.
  TIMSK0 |= (1 << OCIE0A); //output compare match A int enable.
  OCR1A = 15625; //timer1 reaches this CV at 1000ms hence OF int. 
  sei();

  while(1) {
    asm("nop"); 
  }
}

ISR(TIMER1_COMPA_vect) {
  //this occurs every sec. read ADC here.
  
}

ISR(ADC_vect) {
  adc_result = ADC;
}

void pulse(uint8_t n) {
  while (n > 0) {
    
  }
}
