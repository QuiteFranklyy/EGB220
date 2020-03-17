#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile float MS_L = 0.5;
volatile float MS_R = 0.5;
volatile float old_MS_L = 0.5;
volatile float old_MS_R = 0.5;
#define max_speed 0.8

ISR(TIMER0_OVF_vect){ //interrupt called when timer0 overflows
  
  if(MS_L != old_MS_L){
    OCR0A = (MS_L*max_speed*255);
    OCR0B = 128;
    old_MS_L = MS_L;
  }
  
}


ISR(TIMER1_OVF_vect){ //interrupt called when timer1 overflows
  
  if(MS_R != old_MS_R){
    OCR1A = 0;
    OCR1B = 0;
    old_MS_R = MS_R;
  }
  
}

void timer0_init(){

  //set up timer0 to output fast PWM, 256 prescaler, clear on compare match, set on top
  
  DDRB |= (1<<7);
  DDRD |= (1<<0);
  TCCR0A |= (1<<7)|(1<<5)|(1<<1)|1;
  TIMSK0 |= (1<<0); //enable overflow interrupt
  OCR0A = 255; // 255 or 0 doessnt seem to change anything
  OCR0B = 0;
  TCCR0B |= (1<<2);
  
}

void timer1_init(){

  //set up timer1 to output fast PWM (8-bit), 256 prescaler, clear on compare match, set on top
  
  DDRB |= (1<<6)|(1<<5);
  TCCR1A |= (1<<7)|(1<<5)|1;
  TCCR1B |= (1<<3);
  TIMSK1 |= (1<<0); ////enable overflow interrupt
  OCR1A = 255;
  OCR1B = 0;
  TCCR1B |= (1<<2);
  
}
 
int main(void)
{
  // //Exercise 1
    // // connect led to pin PC0
    // DDRE |= (1 << 6);
 
    // // set up timer with prescaler = 1024
    // TCCR0B |= (1 << CS02)|(1 << CS00);
 
    // // initialize counter
    // TCNT0 = 0;
 
    // // loop forever
    // while(1)
    // {
    //     // check if the timer count reaches 124
    //     if (TCNT0 >= 250)
    //     {
    //         PORTE ^= (1 << 6);    // toggles the led
    //         TCNT0 = 0;            // reset counter
    //     }
    // }


    // // Exercise 2
    // int increment = 10;

  // DDRB |= (1<<7);  //OC0A - Initialise as Output

    // TCCR0A |= (1<<7)|(1<<5)|(1<<1)|1;
    // TCCR0B |= (1<<2);
  // OCR0A = 50;

  // while(1){
    
  //  OCR0A += increment; //increase speed

  //  if( OCR0A > 250){ //if speed is greater than max, reset it to 20%
  //    OCR0A = 50;
  //  }
  //  _delay_ms(500); //wait half a second so the speed change is noticeable
  // } 

  // Exercise 3

    sei(); //enable interrupts
  timer0_init(); //set up timers
  //timer1_init();

  //Initialise LED
  // DDRE |= (1<<6);

  while(1){
    //Toggle Light
    // PORTE ^= (1<<6);

    //Change Motor Speeds
    MS_L += 0.1;
    MS_R += 0.1;
    //Ensure Motor speeds don't get larger than 1
    if(MS_R == 1){
      MS_R = 0.4;
      MS_L = 0.4;
    }
    
    _delay_ms(500); //wait half a second so the speed change is noticeable

  }

  return 0;
}