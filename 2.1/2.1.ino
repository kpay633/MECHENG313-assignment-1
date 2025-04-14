#include<avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>

#define THRESHOLD 3
#define TH_V THRESHOLD*1024/5

volatile uint16_t adc_result;

int main(void) {
  //setup:
  cli();
  DDRB |= (1 << DDB5);//onboard LED PB5 output. 
  DDRC &= ~(1 << DDC1); //set A1 as input (arduino pc1)

  Serial.begin(9600);

  ADMUX |= (1 << MUX0); //using ADC1
  ADMUX &= ~((1 << REFS0)|(1 << REFS1)); //AREF
  ADCSRA |= (1 << ADEN)|(1 << ADIE); //adc enable and int enable.
  ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0); //division factor 128
  //ADCSRB |= (1 << ADTS2)|(1 << ADTS0); //for TC1, this is setting trigger source
  //this enables auto trigger source. i want adc to trigger ISR to change state to either >2v or <2v depending on the reading. so every time an adc reading is completed it will go to the isr and update the adc value. 

  
  DDRC &= ~(1 << DDC1); //set pin A1 as input. (later read PINC1)
  TCCR1B |= (1 << WGM12); //timer1 in CTC mode. 
  TCCR1B |= (1 << CS12)|(1 << CS10); //timer1 prescaler 1024.
  OCR1A = 15625; //timer1 raches this CV at 1000ms hence OF int.

  TIMSK1 |= (1 << OCIE1A); //output compare match A int enable.
  sei();

  TCNT1 = 0;
  ADCSRA |= (1 << ADSC); //this is to start a conversion. idk if we want it here. 

  while(1) {

      if (adc_result >= TH_V) {
        if (TCNT1 == 3906){PORTB |= (1 << PORTB5);}
        if (TCNT1 == 7813){PORTB &= ~(1 << PORTB5);}
        if (TCNT1 == 11719){PORTB |= (1 << PORTB5);}
        if (TCNT1 == 15624){PORTB &= ~(1 << PORTB5);}        
      }
      if (adc_result < TH_V) {
        if (TCNT1 == 3906){PORTB |= (1 << PORTB5);}
        if (TCNT1 == 7813){PORTB &= ~(1 << PORTB5);}       
      }
  }

}

ISR(TIMER1_COMPA_vect) {
  //this occurs every sec. read ADC here.
  ADCSRA |= (1 << ADSC); //this is to start a conversion. idk if we want it here. 
  //Serial.print("balls\n");
}

ISR(ADC_vect) {
  adc_result = ADC;
  Serial.print(adc_result);
}
