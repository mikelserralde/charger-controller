
// HARDWARE USER CONTROL COD

#ifndef hardwareUI_h
#define hardwareUI_h

// LIBRARIES
#include <Encoder.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Bounce2.h>

// NOTES:
// AVOID USING PINS WITH LEDs ATTACHED

// UNCOMMENT FOR TESTING
#define TESTING 0

// LEFT ENCODER PINS  Knob1 SW=24, BDT=25, ACLK=26
#define KNOB_L_PB 25
#define KNOB_L_PA 26
#define KNOB_L_BUTTON 24

// RIGHT ENCODER PINS  Knob2 SW=27, DT=28, CLK = 29
#define KNOB_R_PB 28
#define KNOB_R_PA 29
#define KNOB_R_BUTTON 27

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



void KNOB_Buttons_Init(Bounce &L_debounce, Bounce &R_debounce);


//If left pressed return 1
//If right pressed return 2
//If both pressed return 3
int KNOB_Buttons_Check(Bounce &L_debounce, Bounce &R_debounce);


long KNOB_Turn_Check_L(Encoder &knobLeft); 


long KNOB_Turn_Check_R(Encoder &knobRight); 

void LCD_Init(LiquidCrystal &lcd);

void LCD_setBacklight(uint8_t r, uint8_t g, uint8_t b, int brightness); 


void LCD_Next_Menu(int n, double Charging_Voltage, double Charging_Current, double Charging_Time, LiquidCrystal &lcd);


void LCD_Set_Menu(LiquidCrystal &lcd, String line1, String line2, String line3, String line4);


#endif
