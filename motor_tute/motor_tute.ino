// WE DOING TIMERS

// 
// led flashing
//int main(void){
//    // set led0 to output, have to set DDR to output
//  DDRE |= (1<<6);
//  
//  // set up timer with prescaler = 1024
//  TCCR0B |= (1<<CS02)|(1<<CS00);
//
//  // initialise counter
//  TCNT0 = 0;
//
//
//  while(1){
//    // put your main code here, to run repeatedly:
//    if (TCNT0 >= 250){
//      PORTE ^= (1<<6);
//      TCNT0 = 0;
//    } 
//  }
//}



int main(void){
  // setup the two middle sensors for input
  ADMUX |= (1<<REFS1)|(1<<REFS0)|(1<<ADLAR);
  ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
  ADCSRB = 0;
  
  //set the B port to output
  DDRB |= (1<<7);
  // set the D port to output
  DDRD |= 1;
  TCCR0A |= (1<<7)|(1<<5)|(1<<1)|1;
  TCCR0A |= (1<<6)|(1<<4);
  TCCR0B|= (1<<2);
  OCR0A = 150;
  OCR0B = 150;


  while(1){
//     read ADC11
//    ADMUX &= 0b11100000;
    ADMUX |= 0b00000011;
    ADMUX &= 0b11111011;
    ADCSRB |= (1<<5);
    ADCSRA |= (1<<ADSC);
    while(~ADCSRA&(1<<ADIF)){}
    int sensor_out_A = ADCH;
//    if (sensor_out_A >100){
//    OCR0B = 200;
//  } else {
//    OCR0B = 255;
//  }

  // read ADC7
  ADMUX |= 0b00000111;
  ADCSRB = 0;
  ADCSRA |= (1<<ADSC);
  while(~ADCSRA&(1<<ADIF)){}
    int sensor_out_B = ADCH;
  //  if (sensor_out_B > 100){
  //    OCR0A = 200;
  //  } else {
  //    OCR0A = 255;
  //  }
  
    if (sensor_out_A < 100 && sensor_out_B < 100){
      OCR0A = 200;
      OCR0B = 200;
    } else if (sensor_out_A > 100 && sensor_out_B > 100) {
      OCR0B = 255;
      OCR0A = 255;
    }  else if (sensor_out_B > 100) {
      OCR0B = 200;
      OCR0A = 255;
    } else if (sensor_out_A > 100) {
      OCR0A = 200;
      OCR0B = 255;
    }
  }



  
}
