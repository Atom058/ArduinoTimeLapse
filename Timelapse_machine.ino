#include <Stepper.h>
#include <LiquidCrystal.h>


//Pins
    //Pins for crystal
    const int rs = 2;
    const int enable = 3;
    const int d4 = 4;
    const int d5 = 5;
    const int d6 = 6;
    const int d7 = 7;

    //Pins for motor
    const int motorPin1 = 8;
    const int motorPin2 = 9;
    const int motorPin3 = 12;
    const int motorPin4 = 13;

    // Pins that aren't connected to the library, but are still related to LCD
    const int backlightPin = 10;
    const int contrastPin = 11;

    //Other Pins
    const int buttonUp = A1;
    const int buttonDown = A2;
    const int buttonMenu = A3;
    //Reset button - should be connected to a pnp to LCD/Motor and to Arduino

//Motor
    // Settings for Motor
    const int stepsPerRevolution = 64;    // For the engine
    const int oneRevolution = stepsPerRevolution * 32; //Approximate with gearing
    const float stepResolution = (float) 360 / oneRevolution; //Good for finding out positioning
    const int oneDegree = oneRevolution / 360; //Not REALLY one degree, but anyway.
    const int motorSpeed = 10; //RPM of the motor, gives 10/64 RPM out (0,15RPM)

    // The stepper motor is geared, so it has 64 internal translated to 64x32 steps out.
    Stepper myStepper(stepsPerRevolution, motorPin1, motorPin2, motorPin3, motorPin4);

//LCD
LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

//Variables
    unsigned long loopTime = 0;
    unsigned long lastPressTime = 0;
    int debounceTime = 200;

    //Values for settings
    int contrast = 40; //TODO Calibrate initial value
    int angle = 0;
    int time = 0;

    float currentMotorPosition = 0;

    bool timelapseRunning = false;
    bool initialContrastSet = false;

//State recording

    //Screens
    const int angleScreen = 0; 
    const int timeScreen = 1;
    const int contrastScreen = 2;
    const int startItem = 3; //ONLY for currentSelection
    const int firstItem = angleScreen;
    const int lastItem = startItem;

    const int menuScreen = 100; //ONLY for currentScreen
    const int timelapseScreen = 200; //Not included as of yet

    char* menuItems[] = {
        "Angle",
        "Time",
        "Screen",
        "Start"
    };

    //Keeps track of which screen we are on
    int currentScreen = menuScreen; //initiate on menu screen
    int currentSelection = firstItem; //Initiate on First item

    //Buttons
    const int up = 1;
    const int down = 2;
    const int menu = 3;

    int previousButton = 0;
    bool buttonRepeated = false;

    bool buttonUpOn = false;
    bool buttonDownOn = false;
    bool buttonMenuOn = false;


void setup() {
    //Motor
    myStepper.setSpeed(motorSpeed);

    //LCD
    lcd.begin(16, 2);
    lcd.noAutoscroll();

    //Contrast pin
    analogWrite(contrastPin, contrast);
    //LED Backlighting pin
    analogWrite(backlightPin, 200);

    Serial.begin(9600);

}


void loop() {

    digitalWrite(enable, LOW);
    delay(10);

    Serial.println(" ");
    Serial.println( "New loop has begun" );
    Serial.print( "Contrast is set to: "); Serial.println( contrast );
    Serial.print( "Angle is set to: "); Serial.println( angle );
    Serial.print( "Time is set to: "); Serial.println( time );
    
    loopTime = millis();
    readButtons();

    if( !initialContrastSet ){

        Serial.print( "InitialContrast set: " ); Serial.println( initialContrastSet );

        if( buttonMenuOn ) {
            //If the "ok" button is pressed, the contrast is OK
            initialContrastSet = true;
        } else {
            contrastScreenLogic();
        }

    } else if( !timelapseRunning ) {

        //These weird if cases are nescessary because of switch/case implementation
        if( currentScreen == menuScreen ){
            menuScreenLogic();
        } else if( currentScreen == timeScreen ){
            timeScreenLogic();
        } else if( currentScreen == contrastScreen ){
            contrastScreenLogic();
        } else if( currentScreen == angleScreen ){
            angleScreenLogic();
        } else {
            currentScreen = menuScreen;
        }

    } else {
        lcd.clear();
        lcd.setCursor( 0, 0 );
        lcd.noCursor();
        lcd.print("Engine on");
    }
    
}

/*
     Function to read all the input buttons. This is done to avoid
         calling digitalRead mutliple times for each instance where the
         variable is needed. Furthermore, to avoid inconsitant behaviour,
         it is beneficial to share the button state over the entire loop.
*/
void readButtons() {

    if( lastPressTime + debounceTime < loopTime ) {
        //There must be some time between presses to avoid button bounce

        buttonUpOn = false;
        buttonDownOn = false;
        buttonMenuOn = false;

        if( digitalRead(buttonUp) == HIGH ){

            buttonUpOn = true;
            Serial.println("Button UP pressed");

            buttonRepeated = ( previousButton == up );

            previousButton = up;
            lastPressTime = loopTime;

        } else if( digitalRead(buttonDown) == HIGH ){

            buttonDownOn = true;
            Serial.println("Button DOWN pressed");

            buttonRepeated = ( previousButton == down );

            previousButton = down;
            lastPressTime = loopTime;

        } else if( digitalRead(buttonMenu) == HIGH ){

            buttonMenuOn = true;
            Serial.println("Button MENU pressed");

            buttonRepeated = ( previousButton == menu );

            previousButton = menu;
            lastPressTime = loopTime;

        } else {

            previousButton = 0;
            buttonRepeated = false;

        }

        if( buttonRepeated ){
            Serial.println("Button was repeated");
        }

    }

}

void renderScreenWithText( char text[], int currentValue, int maxValue ){

    Serial.println("Render text logic");
    Serial.print( "  Text received: " );Serial.println(text);

    lcd.noCursor();
    lcd.clear();

    //Print the first line
    lcd.setCursor( 0, 0 );
    lcd.print( text );

        
    //Print the second line
    lcd.setCursor( 0, 1 );
    lcd.print( currentValue, DEC); lcd.print("/"); lcd.print(maxValue, DEC );
    
}

void menuScreenLogic() {

    lcd.cursor();
    lcd.clear();

    if( !buttonRepeated ){

        if( buttonUpOn ){

            if( ++currentSelection > sizeof(menuItems) ){
                currentSelection == firstItem;
            }

        } else if( buttonDownOn ){

            if( --currentSelection < 0 ){
                currentSelection == lastItem;
            }

        } else if( buttonMenuOn ){
            
            //If the button is pressed, we can go back to the loop
            if( currentScreen == startItem ){
                //Starts the timelapse
                timelapseRunning = true;
            } else {
                //Switches screens
                currentScreen = currentSelection;
            }

            return;

        }

    }

    //Print menu choices
    for( int item; item < 4; item++ ) {

        if( sizeof(menuItems[item]) < 6 ){

            //Integer MAGIC!
            lcd.setCursor( ( 9 * item % 2 ), item/2 );
            lcd.print(menuItems[item]);

        } else {

            for( int i = 0; i < 6; i++ ){

                //Integer MAGIC!
                lcd.setCursor( ( 9 * item % 2 ) + i, item/2 );                
                lcd.write(menuItems[item][i]);

            }

        }

    }

    //Set updated cursor position
    if( currentSelection == angleScreen ){
        lcd.setCursor( 0, 0 );
    } else if( currentSelection == timeScreen ){
        lcd.setCursor( 9, 0 );
    } else if( currentSelection == contrastScreen ){
        lcd.setCursor( 0, 1 );
    } else if( currentSelection == startItem ){
        lcd.setCursor( 9, 1 );
    } else {
        //Default case, something has gone terribly wrong!
        currentSelection = firstItem;
        lcd.setCursor( 0, 0 );
    }

}


void contrastScreenLogic() {

    Serial.println("contrast logic entered");

    if( buttonUpOn && contrast < 255 ){
        contrast += 1;
    } else if( buttonDownOn && contrast > 0 ) {
        contrast -= 1;
    }

    analogWrite( contrastPin, contrast );
    
    if( buttonMenuOn && !buttonRepeated ){

        currentScreen = menuScreen;
        return;

    } 

    renderScreenWithText("Screen contrast", contrast, 255);
    
}


void timeScreenLogic() {

    if( buttonUpOn && time < 720 ){
        time += 1;
    } else if( buttonDownOn && time > 0 ) {
        time -= 1;
    }
    
    if( buttonMenuOn && !buttonRepeated ){

        currentScreen = menuScreen;
        return;

    } 

    renderScreenWithText("Desired Angle", time, 720);

}


void angleScreenLogic() {

    if( buttonUpOn && angle < 720 ){
        angle += 1;
    } else if( buttonDownOn && angle > 0 ) {
        angle -= 1;
    }
    
    if( buttonMenuOn && !buttonRepeated ){

        currentScreen = menuScreen;
        return;

    } 

    renderScreenWithText("Desired Angle", angle, 720);

}
