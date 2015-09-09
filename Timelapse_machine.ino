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
const byte motor_pin1 = 8;
const byte motor_pin2 = 9;
const byte motor_pin3 = 12;
const byte motor_pin4 = 13;

// Pins that aren't connected to the library, but are still related to LCD
const byte backlight = 10;
const byte contrast = 11;

//Other Pins - Probably not bytes?
const byte button_up = A1;
const byte button_down = A2;
const byte button_menu = A3;
//Reset button - should be connected to a pnp to LCD/Motor and to Arduino

//Variables
float current_degree = 0;
int backlight_level = 128;
boolean UP = true;
boolean DOWN = false;
boolean oscillation_direction = UP;
String output = "";

// The stepper motor is geared, so it has 64 internal steps per
//   revolution, but that translates 64x32 steps on the engine.
Stepper myStepper(stepsPerRevolution, motor_pin1, motor_pin2, motor_pin3, motor_pin4);

LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

void setup() {
  //Motor
  myStepper.setSpeed(motorSpeed);

  //LCD
  lcd.begin(16, 2);
  lcd.noAutoscroll();
  pinMode(backlight, OUTPUT);
  analogWrite(backlight, backlight_level);
}

void loop() {
  
  lcd.setCursor(0, 0);
  lcd.print("Total degrees:");
  lcd.setCursor(0,1);
  output = String(current_degree);
  lcd.print(output);
  myStepper.step(oneDegree);
  delay(500);
  current_degree += oneDegree * stepResolution;
  

  byte stepping_amount = 25;
  //For Backlighting correction
  if( oscillation_direction == UP ) {
    backlight_level += stepping_amount;
    if( backlight_level > 255 ){
      backlight_level = 255;
      oscillation_direction = DOWN;
    }
  } else {
    backlight_level -= stepping_amount;
    if( backlight_level < 0 ){
      backlight_level = 0;
      oscillation_direction = UP;
    }
  }
  
  analogWrite(backlight, backlight_level); 
}

