#include "SoftwareSerial.h"

int turn_around_counter = 0;
bool debug_movement = false;
bool debug_wing_sensors = false;

enum movement{left, forward, right, stopped, backwards};

enum movement last_movement = stopped;
enum movement current_movement = stopped;

bool robot_running = true;
int right_wing_counter = 0;

bool start_button = false;
bool first_run = true;

int direction_array[5];
int percent_direction_array[5];

int start_slow_counter = 0;
int right_wing_counter_not = 0;
boolean slow_zone = false;
bool slow_start_mark = false;

int speed_robot = 255;
int current_section = 0;

movement map_array[100];
int map_array_counter = 0;
bool read_curvature_mark = false;

SoftwareSerial MyBlue(0, 1); // RX | TX


void printMap(){
  for (int i = 0; i < map_array_counter; i++){
    MyBlue.print(map_array_counter);
//    MyBlue.print(map_array[i]);
    if (map_array[i] == 0){
      MyBlue.println("Left");
    } else if (map_array[i] == 1){
      MyBlue.println("Forward");
    } else if (map_array[i] == 2){
      MyBlue.println("Right");
    }
  }
}

void printDirArray(){
  MyBlue.print('[');
  for (int i = 0; i < 3; i++){
    MyBlue.print(direction_array[i]);
    if (i < 2){
      MyBlue.print(',');
    }
  }
  MyBlue.println(']');
}

void convertDirectionPercent(){
  double sum = direction_array[0] + direction_array[1] + direction_array[2];
  for (int i = 0; i < 3; i++){
    direction_array[i] = (int) direction_array[i] / sum * 100;
  }
}

movement getSection(int in_array[]) {
  if (in_array[0] > 50){
    return left;
  } else if (in_array[2] > 50){
    return right;
  } else {
    return forward;
  }  
}

void addDirectionArray(int direction_a){
  switch(direction_a){
    case forward:
    direction_array[1] = direction_array[1] + 1;
//    MyBlue.print('F');
    break;
    case left:
    direction_array[0] = direction_array[0] + 1;
//    MyBlue.print('L');
    break;
    case right:
    direction_array[2] = direction_array[2] + 1;
//    MyBlue.print('R');
    break;
  }
//  MyBlue.print(" ");

  // calculate and store percent array
  double sum = direction_array[0] + direction_array[1] + direction_array[2];
  for (int i = 0; i < 3; i++){
    percent_direction_array[i] = (int) direction_array[i] / sum * 100;
  }
}

void clearDirectionArray(){
  memset(direction_array,0,4);
}

void mappingCheckAndCalc(){
  // set the straight LED if straight is set more than 75%
  if (percent_direction_array[1] > 60){
    // set led
    PORTB |= (1<<2);
  } else {
    PORTB &= ~(1<<2);
  }
  // also don't want the sensor to fire a bunch of times when it reads one line
  //  have a bool that toggles on when it first reads the curvature mark, then off when it goes back to white
    // we are using sensor 1 and sensor 8 for start/finish/curve/slow detection
  // these are PF4 (ADC4) and PD4 (ADC8)
  
  // read ADC4
  ADMUX &= 0b11011111;
  ADMUX &= 0b11100000;
  ADMUX |= 0b00000100;
  ADCSRB = 0;
  ADCSRA |= (1<<ADSC);
  while(~ADCSRA&(1<<ADIF)){}
  int right_wing_sensor = ADCL;
  int temp = ADCH;
  ADMUX |= 0b00100000;

  // read ADC8
//  ADMUX &= !(1<<ADLAR);
//  
  ADMUX &= 0b11100000;
  ADMUX |= 0b00000000;
  ADCSRB |= (1<<MUX5);
  ADCSRA |= (1<<ADSC);
  while(~ADCSRA&(1<<ADIF)){}
  int left_wing_sensor = ADCH;
//  temp = ADCL;
//  ADMUX |= (1<<ADLAR);
//  

  if (debug_wing_sensors){
    MyBlue.write("Right Wing Sensor: ");
    MyBlue.print(right_wing_sensor,DEC);
    MyBlue.print("      ");
    MyBlue.write("Left Wing Sensor: ");
    MyBlue.print(left_wing_sensor,DEC);
    MyBlue.write("\n");
  }

// based on current percentages in direction array, adjust speed
  
//  MyBlue.println(left_wing_sensor);
  if (left_wing_sensor > 210 && read_curvature_mark == false){
    convertDirectionPercent();
    printDirArray();
    read_curvature_mark = true;
    map_array[map_array_counter] = getSection(percent_direction_array);
//    printMap();
    clearDirectionArray();
//    MyBlue.println(map_array[map_array_counter], DEC);
    map_array_counter++;
    
  } else if(left_wing_sensor < 50 && read_curvature_mark == true){
    read_curvature_mark = false;
  }

  if (right_wing_sensor > 254 && slow_start_mark == false){
    right_wing_counter++;
    if (right_wing_counter > 15){
      slow_start_mark = true;
      start_slow_counter++;
      if (start_slow_counter == 3){
        // start of slow zone
        slow_zone = true;
      } else if (start_slow_counter == 4){
        // end of slow zone;
        slow_zone = false;
      } else if (start_slow_counter == 9){
        // stop the robot
        //  robot_running = false;
        start_button = false;
        first_run = false;
        printMap();
      }
    }
  } else {
    if (right_wing_counter > 0){
      right_wing_counter--;
    }
  }
    if(right_wing_sensor < 240 && slow_start_mark == true){
      right_wing_counter_not++;
      if (right_wing_counter_not > 80){
        slow_start_mark = false;
        right_wing_counter_not = 0;
      }
    }
    
    }
  
  // check the wing sensor
  // if wing sensor above threshold
  //  call the get section, passing the array into it
  // append the value from get section to the map array
  // reset all relevant values
//}


void useMap(){
  // if the current section is straight
  if (map_array[map_array_counter] == 1){
    // set led
    PORTB |= (1<<2);
  } else {
    PORTB &= ~(1<<2);
  }
  // also don't want the sensor to fire a bunch of times when it reads one line
  //  have a bool that toggles on when it first reads the curvature mark, then off when it goes back to white
    // we are using sensor 1 and sensor 8 for start/finish/curve/slow detection
  // these are PF4 (ADC4) and PD4 (ADC8)
  
  // read ADC4
  ADMUX &= 0b11011111;
  ADMUX &= 0b11100000;
  ADMUX |= 0b00000100;
  ADCSRB = 0;
  ADCSRA |= (1<<ADSC);
  while(~ADCSRA&(1<<ADIF)){}
  int right_wing_sensor = ADCL;
  int temp = ADCH;
  ADMUX |= 0b00100000;

  // read ADC8
  ADMUX &= 0b11100000;
  ADMUX |= 0b00000000;
  ADCSRB |= (1<<MUX5);
  ADCSRA |= (1<<ADSC);
  while(~ADCSRA&(1<<ADIF)){}
  int left_wing_sensor = ADCH;  

  if (debug_wing_sensors){
    MyBlue.write("Right Wing Sensor: ");
    MyBlue.print(right_wing_sensor,DEC);
  }
  
  if (left_wing_sensor > 210 && read_curvature_mark == false){
    read_curvature_mark = true;
    map_array_counter++;
    
  } else if(left_wing_sensor < 50 && read_curvature_mark == true){
    read_curvature_mark = false;
  }

  if (right_wing_sensor > 254 && slow_start_mark == false){
    right_wing_counter++;
    if (right_wing_counter > 15){
      slow_start_mark = true;
      start_slow_counter++;
      if (start_slow_counter == 3){
        // start of slow zone
        slow_zone = true;
      } else if (start_slow_counter == 4){
        // end of slow zone;
        slow_zone = false;
      } else if (start_slow_counter == 9){
        // stop the robot
        //  robot_running = false;
        start_button = false;
        first_run = false;
        printMap();
      }
    }
  } else {
    if (right_wing_counter > 0){
      right_wing_counter--;
    }
  }
    if(right_wing_sensor < 240 && slow_start_mark == true){
      right_wing_counter_not++;
      if (right_wing_counter_not > 80){
        slow_start_mark = false;
        right_wing_counter_not = 0;
      }
    }
}

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

  // read ADC7
  ADMUX &= 0b11100000;
  ADMUX |= 0b00000111;
  ADCSRB = 0;
  ADCSRA |= (1<<ADSC);
  while(~ADCSRA&(1<<ADIF)){}
  int sensor_out_A = ADCH;

// now we are reading a black line, instead of a white line, so swap accordingly
      if (first_run){
        speed_robot = 220 - (100 * percent_direction_array[1] / 100);
        speed_robot = 220 - (100 * percent_direction_array[1] / 100);
      } else {
        // use map array to speed up/slow down
         current_section = map_array[map_array_counter];
         if (current_section == 1){
          speed_robot = 80;
         } else if (current_section == 0 || current_section == 2){
          speed_robot = 150;
         }
      }
      if (slow_zone){
        // if in slow zone, go slow
        speed_robot = 200;
      }

    if (sensor_out_A > 40 && sensor_out_B > 40){
      current_movement = forward;
      OCR0B = speed_robot;
      OCR0A = speed_robot;
      
//      if (debug_movement){
//        MyBlue.write("FORWARD\n");
//      }
    } else if (sensor_out_A < 40 && sensor_out_B < 40) {
      if (first_run){
        OCR0B = 255;
        OCR0A = 255;
        turn_around_counter++;
        current_movement = stopped;
      } else {
        if (current_section == 0){
          // going left, spin right wheel
          OCR0B = speed_robot;
          OCR0A = 255;
        } else if (current_section == 2){
          // going right, spin left wheel
          OCR0A = speed_robot;
          OCR0B = 255;
        }
      }
//      if (debug_movement){
//        MyBlue.write("STOP\n");
//      }
    }  else if (sensor_out_B < 40) {
      OCR0B = speed_robot;
      OCR0A = 255;
      current_movement = left;
//      if (debug_movement){
//        MyBlue.write("TURN LEFT\n");
//      }
    } else if (sensor_out_A < 40) {
      OCR0A = speed_robot;
      OCR0B = 255;
      current_movement = right;
//      if (debug_movement){
//        MyBlue.write("TURN RIGHT\n");
//      }
    }
    addDirectionArray(current_movement);
//    if (debug_movement){
//      print_movement();
//    }
}


void print_movement(){
  if (current_movement != last_movement){
    last_movement = current_movement;
    switch (current_movement){
      case forward:
      MyBlue.println("Forward");
      break;

      case left:
      MyBlue.println("Left");
      break;

      case right:
      MyBlue.println("Right");
      break;

      case stopped:
      MyBlue.println("Stop");
      break;

      case backwards:
      MyBlue.println("Backwards");
      break;
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



void checkStartButton(){
  while(!start_button){
    OCR0B = 255;
    OCR0A = 255;
    if (PINC&(1<<6)){
      if (first_run){
        start_button = true;
        clearDirectionArray();
        memset(map_array,0,100);
        map_array_counter = 0;
        right_wing_counter = 0;
        start_slow_counter = 0;
      } else {
        map_array_counter = 0;
        right_wing_counter = 0;
        start_slow_counter = 0;
        start_button = true;
      }
    }
  }
  
}




//void spinAround(){
//// //set the B port to output for motor forward
//  DDRB |= (1<<7);
//
//  // set the D port to output for motor forward
//  DDRD |= 1;
//
////  // set the B port to output for motor backward
////  DDRB |= (1<<6);
////  
//
//// // set the B port for motor backward
////  DDRB |= (1<<5);
//
//
////  DDRB |= (1<<6)|(1<<5);
//  PORTE |= (1<<6);
//  PORTB |= (0<<0);
//
////  TCCR1A |= (1<<7)|(1<<5)|1;
////  TCCR1B |= (1<<3);
////  TIMSK1 |= (1<<0); ////enable overflow interrupt
////  OCR1A = 255;
////  OCR1B = 0;
////  TCCR1B |= (1<<2);
//  
//  
//  
////  TCCR0A |= (1<<7)|(1<<5)|(1<<1)|1;
////  TCCR0A |= (1<<6)|(1<<4);
////  TCCR0B |= (1<<2);
//  OCR0A = 150;
//  OCR0B = 150;
//  
//  while(1){
//    
//  }
//  
//}




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

  // setup the C port to input switch 0:
  DDRC &= ~(1<<6);
  PORTC |= (1<<6);

  // set up the LED 3 for straight line stuff
  DDRB |= (1<<2);
  PORTB &= ~(1<<2);

  
  TCCR0A |= (1<<7)|(1<<5)|(1<<1)|1;
  TCCR0A |= (1<<6)|(1<<4);
  TCCR0B |= (1<<2);
  OCR0A = 150;
  OCR0B = 150;
  
  while(robot_running){
    basicLineFollowing();
    if (first_run){
      mappingCheckAndCalc();
    } else {
      useMap();
    }
    checkStartButton();
//  OCR0A = 214;
//  OCR0B = 214;
  }
  OCR0A = 255;
  OCR0B = 255;



  
}
