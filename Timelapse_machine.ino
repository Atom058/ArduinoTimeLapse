#include <Stepper.h>
#include <LiquidCrystal.h>

// Settings for Motor
const int stepsPerRevolution = 64;  // For the engine
const int oneRevolution = stepsPerRevolution * 32; //Approximate with gearing
const float stepResolution = (float) 360 / oneRevolution; //Good for finding out positioning
const int oneDegree = oneRevolution / 360; //Not REALLY one degree, but anyway.
const int motorSpeed = 10; //RPM of the motor, gives 10/64 RPM out (0,15RPM)

//Pins for crystal
const byte rs = 2;
const byte enable = 3;
const byte d4 = 4;
const byte d5 = 5;
const byte d6 = 6;
const byte d7 = 7;

//Pins for motor
const byte motorPin1 = 8;
const byte motorPin2 = 9;
const byte motorPin3 = 12;
const byte motorPin4 = 13;

// Pins that aren't connected to the library, but are still related to LCD
const byte contrast = 11;

//Other Pins
const byte buttonUp = A1;
const byte buttonDown = A2;
const byte buttonMenu = A3;
//Reset button - should be connected to a pnp to LCD/Motor and to Arduino

//Variables
float currentMotorPosition = 0;
String output = "";

// The stepper motor is geared, so it has 64 internal steps per
//   revolution, but that translates 64x32 steps on the engine.
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin2, motorPin3, motorPin4);

LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

void setup() {
  //Motor
  myStepper.setSpeed(motorSpeed);

  //LCD
  lcd.begin(16, 2);
  lcd.noAutoscroll();

}

unsigned long loopTime = 0;
bool timelapseRunning = false;

void loop() {

  if( intialContrastSet ){
    loopTime = millis();

    readButtons();
    updateDisplay();

    if( timelapseRunning ) {
      turnMotor();
    }
  } else {
    readButtons();
    updateDisplay();
  }
  
  /*
  lcd.setCursor(0, 0);
  lcd.print("Total degrees:");
  lcd.setCursor(0,1);
  output = String(currentMotorPosition);
  lcd.print(output);
  myStepper.step(oneDegree);
  delay(500);
  currentMotorPosition += oneDegree * stepResolution;
  */
  
}

unsigned long startButtonPress = 0;
void readButtons() {}

void updateDisplay() {}