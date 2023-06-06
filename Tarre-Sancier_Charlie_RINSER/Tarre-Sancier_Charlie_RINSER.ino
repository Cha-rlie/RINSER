/*
Name: Charlie Tarre-Sancier
Date: 1st of June, 2023
Project: RINSER (Renewable Ink Nib Super Efficient Revolution)
Input: Colour Sensor, Water Flow Sensor, On and Off Switch
Processing: It will activate the cleaner until the water coming out of the pen nib clean
Output: Water Pump (moves water through to clean the pen nib), Motor, Multi-Colour RGB LED
Code Adapted From: 2 different non-specific sites (see reference list)
*/

// import necessary libraries
#include <Servo.h>

// Create boolean variables that have simplified names
const boolean ON = HIGH;
const boolean OFF = LOW;

// Define the button pin
int buttonPin = 4;

// Create a servo object
Servo microServo;

// Define the microservo pin
int microServoPin = 3;

// Define the water pump pin
int waterPumpPin = 8;

// Define the A1 as connecting to the water flow sensor
#define WATERFLOWSENSOR A1

// Define variables needed surrounding water flow sensor calculations
int X;
int Y;
float TIME = 0;
float FREQUENCY = 0;
float WATER = 0;
float TOTAL = 0;

// Define an array with all the colour sensor pins
int colourSensor[] = {11, 10, 9, 6, 2};

// Setting up an array with all three digital pins of the RGB LED
// 13 = redPin, 12 = greenPin, 7 = bluePin
int LEDRGB[] = {13,  12, 7};

// Create constants that can be called upon to display colours
const boolean RED[] = {ON, OFF, OFF};
const boolean GREEN[] = {OFF, ON, OFF};
const boolean BLUE[] = {OFF, OFF, ON};
const boolean YELLOW[] = {ON, ON, OFF};
const boolean CYAN[] = {OFF, ON , ON};
const boolean MAGENTA[] = {ON, OFF, ON};
const boolean WHITE[] = {ON, ON, ON};
const boolean BLACK[] = {OFF, OFF, OFF};

// Define the variable that can stop the whole if necessary
int stillCleaning = 1;

// Define the variable that checks for a safety risk
int issueIdentified = 0;

// Define the variable that keeps tracked of whether the cleaning process is done or not
int cleaningComplete = 0;

void setup() {
  // put your setup code here, to run once:

  // Set the RGB LED pins to be output
  for (int i=0; i<3; i++) {
    pinMode(LEDRGB[i], OUTPUT);
  }

  // Set all the colour sensor pins to their respective modes
  for (int i=0; i<4; i++) {
    pinMode(colourSensor[i], OUTPUT);
  }

  digitalWrite(colourSensor[0], HIGH);
  digitalWrite(colourSensor[1], LOW);

  pinMode(colourSensor[4], INPUT);

  // Sets the digital pin the button is connected to as an input pin
  pinMode(buttonPin, INPUT);

  // Sets the water flow sensor to input
  pinMode(WATERFLOWSENSOR, INPUT);

  // Sets the water pump pin to output
  pinMode(waterPumpPin, OUTPUT);

  // Attach the microservo pin to the servo object
  microServo.attach(microServoPin);

  // Start the pump for a bit
  digitalWrite(waterPumpPin, HIGH);
  delay(5000);
  
  // Start the Serial, which communicates to the Arduino for debugging purposes
  Serial.begin(9600);

  microServo.write(0);

}

void loop() {
  // put your main code here, to run repeatedly:

  // This means that if for whatever reason the system has to disactivate, the looping code will stop running
  powerButtonUpdate();
  if (!stillCleaning == 1) {
    setLEDColour(LEDRGB, RED);   
    digitalWrite(waterPumpPin, LOW);
  }
  else {
    checkForLeaks();
    identifyColour();
    emptyWaterContainer();
    digitalWrite(waterPumpPin, HIGH);    
    Serial.print(stillCleaning);
    Serial.print(issueIdentified);
    Serial.print(cleaningComplete);

  }
  

}

// Define the fucntion that can change the colour of the multi-colour RGB LED
// Define a function to set the LED to any colour
void setLEDColour(int* led, const boolean* color ) {

  boolean colorToBeSet[] = {color[0], color[1], color[2]};

  for (int i=0; i<3; i++) {
    digitalWrite(led[i], color[i]);
  }
} 

// Define the function that checks to see if the player touched the power button
void powerButtonUpdate() {
  // Do the following if the button is pressed
  if (digitalRead(buttonPin) == HIGH) {
    // Swap over the stillCleaning value (e.g. if the system is running it will now stop running)
    // Only turn the system back on if there are no issues and the cleaning has not already finished
    if (stillCleaning == 0 && issueIdentified == 0 && cleaningComplete == 0) {
      stillCleaning = 1;
      setLEDColour(LEDRGB, GREEN);
      }
    else if (stillCleaning == 1) {
      stillCleaning = 0;
      setLEDColour(LEDRGB, RED);
    }
  }
}

// Define the function that uses the colour sensor to check if the water is clean
void identifyColour() {
  // Indentify the red frequency
  digitalWrite(colourSensor[2],LOW);
  digitalWrite(colourSensor[3],LOW);
  long red_frequency = pulseIn(colourSensor[4], LOW);
  long red = map(red_frequency, 25,72,255,0);

  // Identify the green frequency
  digitalWrite(colourSensor[2], HIGH);
  digitalWrite(colourSensor[3], HIGH);
  long green_frequency = pulseIn(colourSensor[4], LOW);
  long green = map(green_frequency, 30,90,255,0);

  // Identify the blue frequency
  digitalWrite(colourSensor[2], LOW);
  digitalWrite(colourSensor[3], HIGH);
  long blue_frequency = pulseIn(colourSensor[4], LOW);
  long blue = map(blue_frequency, 25,70,255,0);

  // If the colour sensor cannot see any colours, then that must mean the pen nib is clean
  // Hence it will deactive the machine and let the microservo open
  if (red > 100 || green > 100 || blue > 100) {
    cleaningComplete = 0;
    stillCleaning = 0;
  }

}


// Define the function that uses the water flow sensor to check for leaks
void checkForLeaks() {
  X = pulseIn(WATERFLOWSENSOR, HIGH);
  Y = pulseIn(WATERFLOWSENSOR, LOW);
  TIME = X + Y;
  FREQUENCY = 1000000/TIME;
  WATER = FREQUENCY/7.5;
  int WATERPASSED = WATER/60;

  if (WATERPASSED == 0) {
    issueIdentified = 1;
    stillCleaning = 0;     
    setLEDColour(LEDRGB, RED);
  }
}

// Define the function that will change the microservo position to release the water
void emptyWaterContainer() {
  // If the pen nib is cleaned and there are no issues, the microsevo can rotate to drop the water
  if (cleaningComplete == 1 && issueIdentified == 0) {
    microServo.write(180);
    setLEDColour(LEDRGB, BLUE);
  }
}




