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

//#include <softwareserial.h>

int turn_around_counter = 0;
#include "SoftwareSerial.h"
SoftwareSerial MyBlue(0, 1); // RX | TX   
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND

void basicLineFollowing(){
    // basic line following
  
//     read ADC11
    ADMUX &= 0b11100000;
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


// now we are reading a black line, instead of a white line, so swap accordingly
    if (sensor_out_A > 40 && sensor_out_B > 40){
      OCR0A = 200;
      OCR0B = 200;
      MyBlue.write("FORWARD\n");
    } else if (sensor_out_A < 40 && sensor_out_B < 40) {
      OCR0B = 255;
      OCR0A = 255;
      turn_around_counter++;
      MyBlue.write("STOP\n");
    }  else if (sensor_out_B < 40) {
      OCR0B = 200;
      OCR0A = 255;
      MyBlue.write("TURN LEFT\n");
    } else if (sensor_out_A < 40) {
      OCR0A = 200;
      OCR0B = 255;
      MyBlue.write("TURN RIGHT\n");
    }
}

void turnAround(){
  bool foundLine = false;
  

  while(!foundLine){
    // read ADC11
    ADMUX &= 0b11100000;
    ADMUX |= 0b00000011;
    ADMUX &= 0b11111011;
    ADCSRB |= (1<<5);
    ADCSRA |= (1<<ADSC);
    while(~ADCSRA&(1<<ADIF)){}
    int sensor_out_A = ADCH;
  
    // read ADC7
    ADMUX |= 0b00000111;
    ADCSRB = 0;
    ADCSRA |= (1<<ADSC);
    while(~ADCSRA&(1<<ADIF)){}
    int sensor_out_B = ADCH;
    if (sensor_out_B > 50){
      foundLine = true;
    }
    PORTE |= (1<<6);
    OCR0B = 210;
    OCR0A = 210;
  }
  PORTE &= 0b10111111;
  PORTB &= 0b11111110;
}




void spinAround(){
// //set the B port to output for motor forward
  DDRB |= (1<<7);

  // set the D port to output for motor forward
  DDRD |= 1;

//  // set the B port to output for motor backward
//  DDRB |= (1<<6);
//  

// // set the B port for motor backward
//  DDRB |= (1<<5);


//  DDRB |= (1<<6)|(1<<5);
PORTE |= (1<<6);
PORTB |= (0<<0);

//  TCCR1A |= (1<<7)|(1<<5)|1;
//  TCCR1B |= (1<<3);
//  TIMSK1 |= (1<<0); ////enable overflow interrupt
//  OCR1A = 255;
//  OCR1B = 0;
//  TCCR1B |= (1<<2);
  
  
  
//  TCCR0A |= (1<<7)|(1<<5)|(1<<1)|1;
//  TCCR0A |= (1<<6)|(1<<4);
//  TCCR0B |= (1<<2);
  OCR0A = 150;
  OCR0B = 150;
  
  while(1){
    
  }
  
}




int main(void){
  MyBlue.begin(9600);
  // setup the two middle sensors for input
  ADMUX |= (1<<REFS1)|(1<<REFS0)|(1<<ADLAR);
  ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
  ADCSRB = 0;
  
  //set the B port to output for motor forward
  DDRB |= (1<<7);

//  // set the B port to output for motor backward
//  DDRB |= (1<<6);
//  
  // set the D port to output for motor forward
  DDRD |= 1;

//  TCCR1A |= (1<<7)|(1<<5)|(1<<1)|1;
//  TCCR1A |= (1<<6)|(1<<4);
//  TCCR1B |= (1<<2);
  
  TCCR0A |= (1<<7)|(1<<5)|(1<<1)|1;
  TCCR0A |= (1<<6)|(1<<4);
  TCCR0B |= (1<<2);
  OCR0A = 150;
  OCR0B = 150;
//  OCR1B = 150;


  while(1){
    if(turn_around_counter >= 2000){
      turnAround();
      while(1){
        basicLineFollowing();
      }
    } else {
      basicLineFollowing();
    }
//    spinAround();
    
  }



  
}
