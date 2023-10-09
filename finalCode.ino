#include <Servo.h> // import servo library

// define pin values of components
int redLEDPin = 3;
int greenLEDPin = 5;
int blueLEDPin = 6;
int proximityPin = 4;
int pushButtonPin = 10;
int LDRPin = 0;
int servoPin = 9;

Servo boomGate; // create object for servo
int boomGatePeriod = 3000; // define suitable time for boom gate movement

// define unique timing parameters (ID: 25127224)
int A = 3000;
int B = 6000;
int C = 2000;

// define system states
int state = 0; // overall state
int LEDState = 255; // LED brightness state
bool buttonPressed = false; // button state
int LDRValue;
int serialState = 0; // serial monitor state

// define timers for timed servo and LED events
unsigned long boomGateTimer;
unsigned long LEDTimer;

void setup() {

  Serial.begin(9600); // initialize serial communication

  // define pin modes for components
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(proximityPin, INPUT);
  pinMode(pushButtonPin, INPUT);
  
  boomGate.attach(servoPin); // attach servo to servo object 
  Serial.println();
  Serial.println("Welcome to the Login Portal, please enter the password to access the Admin Menu."); // password command

}

void loop() {
 
  if(serialState == 0) {

    if(Serial.available() > 0) {
      String password = Serial.readString();
      password.trim();
      if(password == "password") {
        Serial.println("Welcome to the Admin Menu. Which timing parameter (A, B or C) would you like to change?");
        serialState = 1;
      }
      else {
        Serial.println("Incorrect password, please try again!");
      }
    }
  }

  if(serialState == 1) {

    if(Serial.available() > 0) {
      String parameter = Serial.readString();
      parameter.trim();

      if(parameter == "A") {
        Serial.println("What value in milliseconds do you want change A to?");
        serialState = 2;
      }

      else if(parameter == "B") {
        Serial.println("What value in milliseconds do you want change B to?");
        serialState = 3;
      }

      else if(parameter == "C") {
        Serial.println("What value in milliseconds do you want change C to?");
        serialState = 4;
      }

      else if(parameter == "exit") {
        Serial.println("Welcome to the Login Portal, please enter the password to access the Admin Menu.");
        serialState = 0;
      }

      else {
        Serial.println("Please try again! Make sure the timing parameter is capitalized (A, B or C)");
      }
    }
  }

  if(serialState == 2) {

    if(Serial.available() > 0) {
      int newParameter = Serial.parseInt();
      A = newParameter;
      Serial.println("The timing of A has been changed to " + String(newParameter) + " milliseconds.");
      Serial.println("Welcome to the Admin Menu. Which timing parameter (A, B or C) would you like to change?");
      serialState = 1;
    }
  }

  if(serialState == 3) {

    if(Serial.available() > 0) {
      int newParameter = Serial.parseInt();
      B = newParameter;
      Serial.println("The timing of B has been changed to " + String(newParameter) + " milliseconds.");
      Serial.println("Welcome to the Admin Menu. Which timing parameter (A, B or C) would you like to change?");
      serialState = 1;
    }
  }

  if(serialState == 4) {

    if(Serial.available() > 0) {
      int newParameter = Serial.parseInt();
      C = newParameter;
      Serial.println("The timing of C has been changed to " + String(newParameter) + " milliseconds.");
      Serial.println("Welcome to the Admin Menu. Which timing parameter (A, B or C) would you like to change?");
      serialState = 1;
    }
  }

  LDRValue = analogRead(LDRPin);
  int brightness = map(LDRValue, 0, 600, 0, 255);

  // if proximity returns a reading
  if(digitalRead(proximityPin) == 0) {
    // if in state 0
    if(state == 0) {
      // go to state 1
      state = 1;
    }
    // else (if in state 1, 2, 3, 4, 5, 6, 7)
      // do nothing
  }
  // else (if proximity returns no reading)
    // do nothing

  // if button is pushed
  if(digitalRead(pushButtonPin) == HIGH) {
    // if in state 0
    if(state == 0) {
      // buttonPressed set to true
      buttonPressed = true;
    }
    // else (if in state 1, 2, 3, 4, 5, 6, 7)
      // do nothing
  }
  // else (if button is not pushed)
    // do nothing

  // if buttonPressed is true
  if(buttonPressed == true) {
    // if in state 0
    if(state == 0) {
      // go to state 5
        state = 5;
    }
    // else (if in state 1, 2, 3, 4, 5, 6, 7)
      // do nothing 
  }
  // else (if buttonPressed is false)
    // do nothing

  switch(state) {
    
    // if in state 0
    case 0: {
      // boom gate to close position
      boomGate.write(0);
      // solid red LED
      solidLEDs(redLEDPin, brightness);
      // restart timer
      boomGateTimer = millis();
      LEDTimer = millis();
      break;
    }
    
    // if in state 1
    case 1: {
      // boom gate moves to open position and flashing red LED
      unsigned long servoProgress = millis() - boomGateTimer;
      if (servoProgress <= boomGatePeriod) {
        long servoAngle = map(servoProgress, 0, boomGatePeriod, 0, 90);
        boomGate.write(servoAngle);
        // flashing red LED 
        flashLEDs(redLEDPin, 5, brightness);
      }
      // else (if 3000 milliseconds has passed)
      else {
        // reset LED
        solidLEDs(redLEDPin, 0);
        // restart timer
        boomGateTimer = millis();
        LEDTimer = millis();
        // move to state 2
        state = 2;
      }

      // if button is pushed
      if(digitalRead(pushButtonPin) == HIGH) {
        // buttonPressed set to true
        buttonPressed = true;
      }

      break;
    }

    // if in state 2
    case 2: {
      // boom gate remains open
      boomGate.write(90);
      // solid green LED
      solidLEDs(greenLEDPin, brightness);

      // if proximity reading returns no reading
      if(digitalRead(proximityPin) == 1) {
        // go to state 3
        boomGateTimer = millis();
        state = 3;
      }

      // if button is pushed
      if(digitalRead(pushButtonPin) == HIGH) {
        // buttonPressed set to true
        buttonPressed = true;
      }

      break;
    }
    
    // if in state 3
    case 3: {
      // if C milliseconds hasn't passed
      if(millis() - boomGateTimer <= C) {
        // boom gate remains open
        boomGate.write(90); 
        // flash green LED
        flashLEDs(greenLEDPin, 5, brightness);
      }
      // else (if C milliseconds has passed)
      else {
        // go to state 4
        state = 4;
        // reset LEDs
        solidLEDs(greenLEDPin, 0);
        // restart timer
        boomGateTimer = millis();
      }

      // if button is pushed
      if(digitalRead(pushButtonPin) == HIGH) {
        // buttonPressed set to true
        buttonPressed = true;
      }

      break;
    }

    // if in state 4
    case 4: {
      // boom gate moves to close position and flashing red LED
      unsigned long servoProgress = millis() - boomGateTimer;
      if (servoProgress <= boomGatePeriod) {
        long servoAngle = map(servoProgress, 0, boomGatePeriod, 90, 0);
        boomGate.write(servoAngle);
        // flashing red LED 
        flashLEDs(redLEDPin, 5, brightness);
      }
      // else (if 3000 milliseconds has passed)
      else {
        // restart timer
        boomGateTimer = millis();
        LEDTimer = millis();
        // move to state 0
        state = 0;
      }

      // if button is pushed
      if(digitalRead(pushButtonPin) == HIGH) {
        // buttonPressed set to true
        buttonPressed = true;
      }

      break;
    }

    // if in state 5
    case 5: {
      // if A milliseconds hasn't passed
      if (millis() - LEDTimer <= A) {
        // solid red LED
        solidLEDs(redLEDPin, brightness);
      }
      //else (if A milliseconds has passed)
      else {
        // turn off red LED
        solidLEDs(redLEDPin, 0);        
        // reset timer
        LEDTimer = millis();
        boomGateTimer = millis();
        // move to state 6
        state = 6;
      }

      // if proximity sensor returns a reading
      if(digitalRead(proximityPin) == 0) {
        // buttonPressed set to true
        state = 0;
        LEDTimer = millis();
        boomGateTimer = millis();
      }

      break;
    }

    case 6: {
      // if (2/3)*B [4] milliseconds hasn't passed
      if(millis() - LEDTimer <= (2*B)/3) {
        // solid blue LED
        solidLEDs(blueLEDPin, brightness);
      }
      
      // else (if (2/3)*B [4] milliseconds has passed)
      else {
        LEDTimer = millis();
        boomGateTimer = millis();
        // move to state 7
        state = 7;
      }
      break;
    }

    case 7: {
      // if (1/3)*B [2] milliseconds hasn't passed
      if(millis() - boomGateTimer <= (B)/3) {
        // flashing blue LED
        flashLEDs(blueLEDPin, 5, brightness);
      }
      // else ((1/3)*B [2] milliseconds has passed)
      else {
        LEDTimer = millis();
        boomGateTimer = millis();
        // reset LEDs
        solidLEDs(blueLEDPin, 0);  
        // move to state 0
        state = 8;
      }
      break;
    }

    case 8: {
      // if 1000 milliseconds has passed
      if(millis() - boomGateTimer <= 1000) {
        // solid red LED
        solidLEDs(redLEDPin, brightness);
      }
      // else (2000 milliseconds has passed)
      else {
        // reset timer and buttonPressed
        LEDTimer = millis();
        boomGateTimer = millis();
        buttonPressed = false;
        // move to state 0
        state = 0;
      }
      break;
    }

  }

}

void solidLEDs(int RGBColour, int brightness) {
  analogWrite(RGBColour, brightness);
}

void flashLEDs(int RGBPin, int hertz, int brightness) {
  int halfPeriod = (1000/(2*hertz));
  if(LEDState == 255) {
    if( (millis() - LEDTimer) >= halfPeriod) {   
      LEDState = 0;
      LEDTimer = millis();
    }
  } 

  else {   
    if( (millis() - LEDTimer) >= halfPeriod) {     
      LEDState = 255;
      LEDTimer = millis();
    }
  }

  if(LEDState == 255) {
    analogWrite(RGBPin, brightness);
  }
  else {
    analogWrite(RGBPin, 0);
  }
  
}