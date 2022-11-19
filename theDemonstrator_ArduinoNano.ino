/*theDemonstrator LED sequencer
Intended to use with the Arduino Nano board.

V1.0 11/18/2022
by Brian P. Cox, FutureVision Research
Sketch is under Creative Commmons License Attribution-NonCommercial-ShareAlike 4.0 International
Circuit board design and theDemonstrator name copyright © Brian P. Cox and FutureVision Research, all rights reserved.
*/

/* Declare constants
We declare Arduino pin numbers as constants so we can change them in one spot, instead of making the change throughout our code
*/
const int FIRST_LED_PIN = 3, LAST_LED_PIN = 12; // Specifiy the first and last LED pins
const int MODE_BUTTONPIN = 2; // Specify the pin used for the button that will determine the sequencing mode. NOTE: Since pin 0 is used, do not press the mode button while programming the Arduino Micro via USB.
const int MODE_LED_PIN = 13; // Specify the pin used for the mode LED
const int ANALOG_SPEED_PIN = 0; // Specify the analog pin A0 for the voltage divider that will determine the sequencing speed
const int MIN_SPEED = 10; // Specify the minimum sequence speed in milliseconds
const int MAX_SPEED = 1000; // Specify the maximum sequence speed in milliseconds
const int MAX_MODES = 4; //Set the maximum number of modes we will rotate through (When this is set to zero, there is one mode)

// Declare variables
int voltageDivider = 0; //Used to store the result of the analog read from the voltage divider
int sequenceSpeed = MIN_SPEED; //Used to store the current sequencer interval in milliseconds
int modeButtonState = 0; //Used to store the result of button press. Button has pulldown resistor, so HIGH = button press
int counter; //used to keep track of counts in for loops
int currentMode = 1; //Used to keep track of the current mode for the LED sequencer
unsigned long lastInterruptTime = 0; //Used to keep track of time since the interrupt was last triggered

void setup() { //Setup function runs once
  Serial.begin(9600); //Set serial output to 9600 baud
  // Configure digital pins
  for (counter = FIRST_LED_PIN; counter <= LAST_LED_PIN; counter++) {  //Set all LED pins as outputs
    pinMode(counter,OUTPUT);
    delay(250); //Provides delay so we can see each pin change from float status to sink, which is the default mode for output
    } //End of counter
  // Configure settings for mode change button
  pinMode (MODE_BUTTONPIN, INPUT_PULLUP); //Set button pin as input and enable built in pull up resistor
  pinMode(MODE_LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTONPIN), modeChange, LOW); //Creates an Interrupt Service Routine that calles the function modeChange() each time the mode button is pressed.
} //End of setup function

void loop() { //Loop function runs over and over
  digitalWrite(MODE_LED_PIN, LOW); //Set MODE LED low in case there has been a mode change
  switch (currentMode){//Calls proper LED sequencer function based on the current mode
    case 1:
      classicSequencer();
      break;
    case 2:
      oneAfterAnother();
      break;
    case 3:
      pingPong();
      break;
    case 4:
      randomLED();
      break;
  }//End of switch
 }//End of loop function  

void modeChange(){ //Function called by Interrup Service Routine (ISR) to change the mode of the sequencer
 unsigned long interruptTime = millis(); //Reads the current time so it can be sued to debounce the mode button press
 if (interruptTime - lastInterruptTime > 200) { //Used to debounce the mode button press
  currentMode = ++currentMode; //increase the mode count by 1
    if (currentMode > MAX_MODES) {
      currentMode = 1;     
    }
  digitalWrite(MODE_LED_PIN, HIGH); //Light mode LED to indicate a mode change has occured.
  Serial.print("Changed to mode: ");
  Serial.println(currentMode);
  lastInterruptTime = interruptTime; //Keeps track of the last time the interrupt was triggered so it can be used to debounce the mode button press
  }
} //End of modeChange function

void classicSequencer(){ //Lignt one LED at a time from left to right
  for (counter = FIRST_LED_PIN; counter <= LAST_LED_PIN; ++counter) { //counter used to keep track of LED sequence
    getSequenceSpeed();
    digitalWrite(counter, HIGH);
    timeDelay(sequenceSpeed);
    digitalWrite(counter, LOW);
    }//End of counter
}//End of classicSequencer function
  
void oneAfterAnother(){ //Light one LED after another then turn each one off
    for(counter = FIRST_LED_PIN; counter <= LAST_LED_PIN; ++counter) { 
      getSequenceSpeed();
      digitalWrite(counter, HIGH);
      timeDelay(sequenceSpeed);   
    }
    for(counter = LAST_LED_PIN; counter >= FIRST_LED_PIN; --counter) {
      getSequenceSpeed();
      digitalWrite(counter, LOW);
      timeDelay(sequenceSpeed);   
    }         
}//End of oneAfterAnother function

void pingPong(){
  getSequenceSpeed();
  for(counter = FIRST_LED_PIN; counter <= LAST_LED_PIN; ++counter) {//Light one LED at a time from left to right
    digitalWrite(counter, HIGH);  // turn LED on
    timeDelay(sequenceSpeed);                    // pause to slow down
    digitalWrite(counter, LOW);   // turn LED off
  }

  getSequenceSpeed();
  for(counter = LAST_LED_PIN; counter >= FIRST_LED_PIN; --counter) { //Light one LED at a time from right to left
    digitalWrite(counter, HIGH);  // turn LED on
    timeDelay(sequenceSpeed);                    // pause to slow down
    digitalWrite(counter, LOW);   // turn LED off
  }
}//End of pingPong function

void randomLED(){
  getSequenceSpeed();
  counter = random(FIRST_LED_PIN,LAST_LED_PIN+1);  // pick a LED pin within the proper range
  digitalWrite(counter, HIGH);  // turn LED on
  timeDelay(sequenceSpeed);                    // pause to slow down
  digitalWrite(counter, LOW);   // turn LED off
}

int getSequenceSpeed(){//This function reads the value of the potentiometer and coverts it to a sequence speed range specified at the beginning of the sketch.
  sequenceSpeed = analogRead(ANALOG_SPEED_PIN); //Reads the value of the voltage divider and assigns it to the variable sequenceSpeed
  sequenceSpeed = map (sequenceSpeed, 0, 1024, MIN_SPEED, MAX_SPEED); //Converts the speed value to the range specified
  return sequenceSpeed;
}//End of getSequenceSpeed function

void timeDelay(int sequenceSpeed){//This function replaces the built-in Arduino function delay(). It provides a delay while still allowing interrupts to be triggered.
  unsigned long startMillis = millis();
  unsigned long currentMillis = millis(); 
  while (currentMillis - startMillis <= sequenceSpeed){
    currentMillis = millis();
  }
}//End of timeDelay function
