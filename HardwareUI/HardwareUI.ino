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

//Menu Stuff
#define DEFAULT_CHARGE_VOLTAGE  4.2
#define DEFAULT_CHARGE_CURRENT  1.1
#define Startup_Menu            0
#define Custom_Or_Default       1
#define Set_Voltage             2
#define Set_Current             3
#define Ask_to_Start            4
#define Charge_Menu1            5
#define Charge_Menu2            6
#define Menu_Error              15

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
  LCD_Next_Menu(current_menu, DEFAULT_CHARGE_VOLTAGE, DEFAULT_CHARGE_CURRENT, 0);

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
  long newPosL = KNOB_Turn_Check_L();
  long newPosR = KNOB_Turn_Check_R();
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

//If left pressed return 1
//If right pressed return 2
//If both pressed return 3
int KNOB_Buttons_Check() {
  int pressed = 0;
  
  // Update the Bounce instances :
  L_debounce.update();
  R_debounce.update();

  // LEFT BUTTON PRESSED
  if (L_debounce.rose()) {
    KNOB_Button_L_Pressed();
    pressed += 1;
  }
  
  // RIGHT BUTTON PRESSED
  if(R_debounce.rose()) {
    KNOB_Button_R_Pressed();
    pressed += 2;
  }

  return pressed;
}

void KNOB_Button_L_Pressed(){
  if(current_menu != 0) {
      current_menu--;
    } else {
      current_menu = 3;
    }
    LCD_Next_Menu(current_menu, DEFAULT_CHARGE_VOLTAGE, DEFAULT_CHARGE_CURRENT, 0);

    return;
}

void KNOB_Button_R_Pressed(){
  if(current_menu != 3) {
      current_menu++;
    } else {
      current_menu = 0;
    }
    LCD_Next_Menu(current_menu, DEFAULT_CHARGE_VOLTAGE, DEFAULT_CHARGE_CURRENT, 0);

    return;
}


long KNOB_Turn_Check_L() {
  
  // ENCODER STUFF
  long newLeft;
  newLeft = knobLeft.read();
  if (newLeft != positionLeft){
    Serial.print("Left = ");
    Serial.print(newLeft);
    Serial.println();
    return newLeft;
  }

  else return -999;

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


long KNOB_Turn_Check_R() {
  
  // ENCODER STUFF
  long newRight;

  newRight = knobRight.read();
  if(newRight != positionRight) {

    Serial.print(", Right = ");
    Serial.print(newRight);
    Serial.println();
    return newRight;
  }

  else return -999;
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

template <typename T> void LCD_Next_Menu(T n, double Charging_Voltage, double Charging_Current, double Charging_Time) {
  T line1,line2,line3,line4;
  current_menu = n;

  switch(n){
    case Startup_Menu:
      line1 = " Missouri S&T ";
      line2 = "Formula Electric";
      line3 = "Press Any Button";
      line4 = "To Continue";
      break;
    case Custom_Or_Default:
      line1 = " Charge at Custom ";
      line2 = "or Default Voltage? ";
      line3 = " ";
      line4 = "Custom       Default";
      break;
    case Set_Voltage:
      line1 = " Set Voltage: ";
      line2 = Charging_Voltage;
      line3 = " ";
      line4 = "+5           +0.1";
      break;
    case Set_Current:
      line1 = " Set Current: ";
      line2 = Charging_Current;
      line3 = " ";
      line4 = "+1           +0.1";
      break;
    case Ask_to_Start:
      line1 = "Start Charging At:";
      line2 = Charging_Voltage;
      line3 = "Screen Number:";
      line4 = "Yes          NO";
      break;
    case Charge_Menu1:
      line1 = "Charge Time:";
      line2 = "CURRENT_TIME_IDK"; // Charge time lol
      line3 = " ";
      line4 = "Press Both to Stop";
      break;
    case Charge_Menu2:
      line1 = "Charging to:";
      line2 = Charging_Voltage;
      line3 = Charging_Current; //current idk
      line4 = "Press Both to Stop";
      break;
    default:
      line1 = " Missouri S&T ";
      line2 = "Formula Electric";
      line3 = "Screen Number:";
      line4 = "LCD Error";
  }

  LCD_Set_Menu(line1,line2,line3,line4);

  return;
}
template <typename T> 
void LCD_Set_Menu(T line1, T line2, T line3, T line4) {
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
