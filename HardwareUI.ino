// HARDWARE USER CONTROL COD

// LIBRARIES
#include <Encoder.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Bounce2.h>

// NOTES:
// AVOID USING PINS WITH LEDs ATTACHED

// UNCOMMENT FOR TESTING
#define TESTING 0

// LEFT ENCODER PINS
#define KNOB_L_PB 16
#define KNOB_L_PA 17
#define KNOB_L_BUTTON 18

// RIGHT ENCODER PINS
#define KNOB_R_PB 19
#define KNOB_R_PA 20
#define KNOB_R_BUTTON 21

// TEENSY LED PIN - DO NOT USE FOR ANYTHING ELSE
#define LED_PIN 13

// LCD BACKLIGHT COLOR PINS (PWM)
#define LCD_RED 8
#define LCD_GREEN 9
#define LCD_BLUE 10

// LCD INTERFACE PINS
#define LCD_P1 2
#define LCD_P2 3
#define LCD_P3 4
#define LCD_P4 5
#define LCD_P5 6
#define LCD_P6 7

// LCD SIZE
#define LCD_ROWS 4
#define LCD_COLUMNS 20

// INITIALIZE ENCODERS
Encoder knobLeft(KNOB_L_PA, KNOB_L_PB);
Encoder knobRight(KNOB_R_PA, KNOB_R_PB);

// INTIALIZE BOUNCE OBJECTS FOR ENCODER PUSHBUTTONS
Bounce L_debounce = Bounce(); 
Bounce R_debounce = Bounce(); 

// INITIALIZE LCD
LiquidCrystal lcd(LCD_P1,LCD_P2,LCD_P3,LCD_P4,LCD_P5,LCD_P6);
// LCD BRIGHTNESS (0-255)
int brightness = 255;

// CURRENT MENU 
int current_menu = 0;

// ENCODER VALUES
long positionLeft  = -999;
long positionRight = -999;

void setup() {
  // INITIALIZE LCD
  LCD_Init();

  // INITIALIZE ENCODER BUTTONS + DEBOUNCE 
  KNOB_Buttons_Init();

  // SET DEFAULT STARTUP MENU
  LCD_Next_Menu(current_menu);

  // FOR TESTING ONLY
  #ifdef TESTING
        // SERIAL INITIALIZE
        Serial.begin(9600);
        Serial.println("TwoKnobs Encoder Test:");
        //Setup the LED
        //pinMode(LED_PIN,OUTPUT);
  #endif // TESTING

}

void loop() {
  KNOB_Buttons_Check();
  KNOB_Turn_Check();
}

void KNOB_Buttons_Init(){
    // SETUP UP KNOB PUSHBUTTONS
  // SETUP UP LEFT BUTTON WITH PULL-UP
  pinMode(KNOB_L_BUTTON,INPUT_PULLUP);
  // SETUP BOUNCE INSTANCE
  L_debounce.attach(KNOB_L_BUTTON);
  // SETUP DEBOUNCE INTERVAL IN ms
  L_debounce.interval(5);

  // SETUP UP RIGHT BUTTON WITH PULL-UP
  pinMode(KNOB_R_BUTTON,INPUT_PULLUP);
  // SETUP BOUNCE INSTANCE
  R_debounce.attach(KNOB_R_BUTTON);
  // SETUP DEBOUNCE INTERVAL IN ms
  R_debounce.interval(5);

  return;
}

// Return 0 if none, 1 if left, 2 if right
int KNOB_Buttons_Check() {
  int pressed = 0;
  
  // Update the Bounce instances :
  L_debounce.update();
  R_debounce.update();

  // LEFT BUTTON PRESSED
  if (L_debounce.rose()) {
    KNOB_Button_L_Pressed();
    pressed = 1;
  }
  
  // RIGHT BUTTON PRESSED
  if(R_debounce.rose()) {
    KNOB_Button_R_Pressed();
    pressed = 2;
  }

  return pressed;
}

void KNOB_Button_L_Pressed(){
  if(current_menu != 0) {
      current_menu--;
    } else {
      current_menu = 3;
    }
    LCD_Next_Menu(current_menu);

    return;
}

void KNOB_Button_R_Pressed(){
  if(current_menu != 3) {
      current_menu++;
    } else {
      current_menu = 0;
    }
    LCD_Next_Menu(current_menu);

    return;
}


void KNOB_Turn_Check() {
  
  // ENCODER STUFF
  long newLeft, newRight;
  newLeft = knobLeft.read();
  if (newLeft != positionLeft){
    Serial.print("Left = ");
    Serial.print(newLeft);
    Serial.println();
    positionLeft = newLeft;
  }

  newRight = knobRight.read();
  if(newRight != positionRight) {

    Serial.print(", Right = ");
    Serial.print(newRight);
    Serial.println();
    positionRight = newRight;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  #ifdef TESTING
    if (Serial.available()) {
      Serial.read();
      Serial.println("Reset both knobs to zero");
      knobLeft.write(0);
      knobRight.write(0);
    }
  #endif
}

void LCD_Init(){
  // SETUP LCD SIZE
  lcd.begin(LCD_COLUMNS, LCD_ROWS);

  // SETUP LCD BACKLIGHT PINS
  pinMode(LCD_RED, OUTPUT);
  pinMode(LCD_GREEN, OUTPUT);
  pinMode(LCD_BLUE, OUTPUT);

  // INITIAL LCD BRIGHTNESS
  brightness = 255;
  // INITIAL LCD BACKLIGHT COLOR
  LCD_setBacklight(255,255,255);

  return;
}

void LCD_setBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // normalize the red LED - its brighter than the rest!
  r = map(r, 0, 255, 0, 100);
  g = map(g, 0, 255, 0, 150);
 
  r = map(r, 0, 255, 0, brightness);
  g = map(g, 0, 255, 0, brightness);
  b = map(b, 0, 255, 0, brightness);
 
  // common anode so invert!
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);
//  Serial.print("R = "); Serial.print(r, DEC);
//  Serial.print(" G = "); Serial.print(g, DEC);
//  Serial.print(" B = "); Serial.println(b, DEC);
  analogWrite(LCD_RED, r);
  analogWrite(LCD_GREEN, g);
  analogWrite(LCD_BLUE, b);

  return;
}


void LCD_Next_Menu(int n) {
  String line1,line2,line3,line4;
  current_menu = n;

  switch(n){
    case 0:
      line1 = " Missouri S&T ";
      line2 = "Formula Electric";
      line3 = "Screen Number:";
      line4 = "ZERO";
      break;
    case 1:
      line1 = " Missouri S&T ";
      line2 = "Formula Electric";
      line3 = "Screen Number:";
      line4 = "ONE";
      break;
    case 2:
      line1 = " Missouri S&T ";
      line2 = "Formula Electric";
      line3 = "Screen Number:";
      line4 = "TWO";
      break;
    case 3:
      line1 = " Missouri S&T ";
      line2 = "Formula Electric";
      line3 = "Screen Number:";
      line4 = "THREE";
      break;
    default:
      line1 = " Missouri S&T ";
      line2 = "Formula Electric";
      line3 = "Screen Number:";
      line4 = "DEFAULT?";
  }

  LCD_Set_Menu(line1,line2,line3,line4);

  return;
}

void LCD_Set_Menu(String line1, String line2, String line3, String line4) {
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  lcd.setCursor(0,2);
  lcd.print(line3);
  lcd.setCursor(0,3);
  lcd.print(line4);
  
  return;
}
