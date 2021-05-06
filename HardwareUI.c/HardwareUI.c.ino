#include <circular_buffer.h>
#include <FlexCAN_T4.h>
#include <imxrt_flexcan.h>
#include <isotp.h>
#include <isotp_server.h>
#include <kinetis_flexcan.h>

// HARDWARE USER CONTROL COD

// LIBRARIES
#include <Encoder.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Bounce2.h>
#include "HardwareUI.h"

// NOTES:
// AVOID USING PINS WITH LEDs ATTACHED



void KNOB_Buttons_Init(Bounce &L_debounce, Bounce &R_debounce){
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
int KNOB_Buttons_Check(Bounce &L_debounce, Bounce &R_debounce) {
  int pressed = 0;
  
  // Update the Bounce instances :
  L_debounce.update();
  R_debounce.update();


  // LEFT BUTTON RELEASED
  if (L_debounce.rose()) {
    pressed --;
  }

  //LEFT BUTTON DEPRESSED
  if(L_debounce.fell()){
    pressed ++;
  }
  
  // RIGHT BUTTON RELEASED
  if(R_debounce.rose()) {
    pressed -= 2;
  }

  //RIGHT BUTTON DEPRESSED
  if(R_debounce.fell()){
    pressed +=2;
  }


  
  return pressed;
}


long KNOB_Turn_Check_L(Encoder &knobLeft) {
  
  // ENCODER STUFF
  long newLeft;
  newLeft = knobLeft.read();

  return newLeft;
}


long KNOB_Turn_Check_R(Encoder &knobRight) {
  
  // ENCODER STUFF
  long newRight;

  newRight = knobRight.read();

  return newRight;
}

void LCD_Init(LiquidCrystal &lcd){
  // SETUP LCD SIZE
  lcd.begin(LCD_COLUMNS, LCD_ROWS);

  // SETUP LCD BACKLIGHT PINS
  pinMode(LCD_RED, OUTPUT);
  pinMode(LCD_GREEN, OUTPUT);
  pinMode(LCD_BLUE, OUTPUT);

  // INITIAL LCD BRIGHTNESS
  int brightness = 255;
  // INITIAL LCD BACKLIGHT COLOR
  LCD_setBacklight(255,255,255, brightness);

  return;
}

void LCD_setBacklight(uint8_t r, uint8_t g, uint8_t b, int brightness) {
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

void LCD_Next_Menu(int n, double Charging_Voltage, double Charging_Current, double Charging_Time, LiquidCrystal &lcd) {

  String line1,line2,line3,line4;

  switch(n){
    case Startup_Menu:
      line1 = "    Missouri S&T    ";
      line2 = "  Formula Electric  ";
      line3 = "  Press Any Button  ";
      line4 = "    To Continue:    ";
      break;
    case Custom_Or_Default:
      line1 = "  Charge at Custom  ";
      line2 = " or Default Voltage?";
      line3 = " ";
      line4 = "Custom       Default";
      break;
    case Set_Voltage:
      line1 = "    Set Voltage:    ";
      line2 = String(Charging_Voltage) + " V";
      line3 = "                    ";
      line4 = "+5              +0.1";
      break;
    case Set_Current:
      line1 = "    Set Current:    ";
      line2 = String(Charging_Current) + " A";
      line3 = "                    ";
      line4 = "+1              +0.1";
      break;
    case Ask_to_Start:
      line1 = " Start Charging At: ";
      line2 = "     " + String(Charging_Voltage) + " V";
      line3 = "     " + String(Charging_Current) + " A";;
      line4 = "NO               YES";
      break;
    case Charge_Menu1:


      line1 = String((int(Charging_Time) / 1000)%60);
      line3 = String((int(Charging_Time) / (60*1000))%60);
      line4 = String(int((Charging_Time / (60*60*1000))));

      line2 = "      " + line4 + ":" + line3 + ":" + line1;
    
      line1 = "   Charge Time:   ";
      line3 = "                  ";
      line4 = "Press Both to Stop";
      break;
      
    case Charge_Menu2:
      line1 = "   Charging to:   ";
      line2 = "     " + String(Charging_Voltage) + " V";
      line3 = "     " + String(Charging_Current) + " A";
      line4 = "Press Both to Stop";
      break;

      
    default:
      line1 = "   Missouri S&T   ";
      line2 = " Formula Electric ";
      line3 = "  Screen Number:  ";
      line4 = "    LCD Error     ";
      break;
  }


  LCD_Set_Menu(lcd,line1,line2,line3,line4);

  return;
}


void LCD_Set_Menu(LiquidCrystal &lcd, String line1, String line2, String line3, String line4) {
  
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
