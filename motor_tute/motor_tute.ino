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
#include "SoftwareSerial.h"
//#include <HardwareSerial.h>

int turn_around_counter = 0;
bool debug_movement = true;
bool debug_wing_sensors = true;

enum movement{forward, left, right, stopped, backwards};

enum movement last_movement = stopped;
enum movement current_movement = stopped;

SoftwareSerial MyBlue(0, 1); // RX | TX 
//HardwareSerial MyBlue(1);  
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND

void basicLineFollowing(){
    // basic line following
  // we are using sensor 3 and sensor 4 for line following (PF6 and PF7)
  //
  
//     read ADC6
    ADMUX &= 0b11100000;
    ADMUX |= 0b00000110;
//    ADMUX &= 0b11111011;
    ADCSRB = 0;
    ADCSRA |= (1<<ADSC);
    while(~ADCSRA&(1<<ADIF)){}
    int sensor_out_B = ADCH;
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
  int sensor_out_A = ADCH;
  //  if (sensor_out_B > 100){
  //    OCR0A = 200;
  //  } else {
  //    OCR0A = 255;
  //  }


  // we are using sensor 1 and sensor 8 for start/finish/curve/slow detection
  // these are PF4 (ADC4) and PD4 (ADC8)
  
  // read ADC4
  ADMUX &= 0b11100000;
  ADMUX |= 0b00000100;
  ADCSRB = 0;
  ADCSRA |= (1<<ADSC);
  while(~ADCSRA&(1<<ADIF)){}
  int left_wing_sensor = ADCH;

  // read ADC8
  ADMUX &= 0b11100000;
  ADMUX |= 0b00000000;
  ADCSRB |= (1<<MUX5);
  ADCSRA |= (1<<ADSC);
  while(~ADCSRA&(1<<ADIF)){}
  int right_wing_sensor = ADCH;

  if (debug_wing_sensors){
//    MyBlue.write("Left Wing Sensor: ");
//    MyBlue.print(sensor_out_A,DEC);
//    MyBlue.write("\n");
    MyBlue.write("Right Wing Sensor: ");
    MyBlue.print(right_wing_sensor,DEC);
    MyBlue.write("\n");
  }

  
// now we are reading a black line, instead of a white line, so swap accordingly
    if (sensor_out_A > 40 && sensor_out_B > 40){
      OCR0A = 200;
      OCR0B = 200;
      current_movement = forward;
//      if (debug_movement){
//        MyBlue.write("FORWARD\n");
//      }
    } else if (sensor_out_A < 40 && sensor_out_B < 40) {
      OCR0B = 255;
      OCR0A = 255;
      turn_around_counter++;
      current_movement = stopped;
//      if (debug_movement){
//        MyBlue.write("STOP\n");
//      }
    }  else if (sensor_out_B < 40) {
      OCR0B = 200;
      OCR0A = 255;
      current_movement = left;
//      if (debug_movement){
//        MyBlue.write("TURN LEFT\n");
//      }
    } else if (sensor_out_A < 40) {
      OCR0A = 200;
      OCR0B = 255;
      current_movement = right;
//      if (debug_movement){
//        MyBlue.write("TURN RIGHT\n");
//      }
    }
    print_movement();
}

void print_movement(){
  if (current_movement != last_movement){
    last_movement = current_movement;
    switch (current_movement){
      case forward:
      MyBlue.write("Forward");

      case left:
      MyBlue.write("Left");

      case right:
      MyBlue.write("Right");

      case stopped:
      MyBlue.write("Stop");

      case backwards:
      MyBlue.write("Backwards");
    }
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
//  MyBlue.begin(9600, SERIAL_8N1, 0, 1);
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
//    if(turn_around_counter >= 2000){
//      turnAround();
      while(1){
        basicLineFollowing();
      }
//    } else {
//      basicLineFollowing();
//    }
//    spinAround();
    
  }



  
}
