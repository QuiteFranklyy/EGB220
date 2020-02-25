// SENSOR ARRAY MAPPING
// sparkbob - spark - arduino - ARDUINO DOESN"T WORK
// S1 - PF4 - analog 4
// s2 - PF5 - analog 3
// S3 - PF6 - analog 2
// S4 - PF7 - analog 1
// S5 - PB4 - digital 8
// S6 - PD7 - digital 6 - activates when 4 is activated????
// S7 - PD6 - digital 12 - Outputs one of the thumb scrolls
// S8 - PD4 - digital 4  - Outputs one of the thumb scrolls

// actual assignments:
// from sensors 1 to 8
// A3, A2, A1, A0, 8, 7, 11, 6
// I'm not sure why, all the documentation goes against this. I found the last 3 by trial and error
// A5 and A4 are the thumbwheels that we'll remove anyway for our pcb design

// voltage decreases when light is reflected, so when the white line is seen, those sensors will have a lower value


int array[10] = {A3,A2,A1,A0,8,7,11,6,A5,A4};

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(9, OUTPUT);
  Serial.begin(115200);
  // setup all the sensor pins for input
  for (int i = 0; i < 10; i++){
    pinMode(array[i],INPUT);
  }
}




void loop() {
  // we want to read the values of each sensor and print it to the console
  for (int i = 0; i < 10; i++){
    Serial.print(array[i]);
    Serial.print(": ");
    Serial.println(analogRead(array[i]));
  }
  Serial.println();
  delay(200);
}
