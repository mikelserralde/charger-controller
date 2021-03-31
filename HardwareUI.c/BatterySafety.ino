

/*
 * STATE MACHINE
 * 
 * 
 * 
 * 
 * TO DEFINE: 
 *  CAN MESSAGES:
 *    START_CHARGING, STOP_CHARGING
 */
 
//#include "CAN.h"
#include <Metro.h>
#include "HardwareUI.h"
#include <LiquidCrystal.h>

#define NEG_CONTACTOR_CTRL   36
#define POS_CONTACTOR_CTRL   37

#define NEG_CONTACTOR_SENSE     40
#define POS_CONTACTOR_SENSE     41
#define CONTACTOR_OFF_3v3       18
#define CONTACTOR_ON_3v3        19
#define IMD_FAULT               20
#define AMS_FAULT               21
#define SC_STATE_3v3            23

//Error_Msg
#define CONTACTOR_OPEN          1
#define CONTACTOR_CLOSED        2
#define PRECHARGE_TIMEOUT       3
#define CHARGING_STATE_ERROR    4

//Charge_State_g
#define Not_Charging            1
#define Start_Precharging       2
#define Precharging             3
#define Start_Charging          4
#define Charging                5
#define Charging_Shutdown       6
#define Error                   7

//Menu_State_g
#define Startup_Menu            0
#define Custom_Or_Default       1
#define Set_Voltage             2
#define Set_Current             3
#define Ask_to_Start            4
#define Charging_Menu1          5
#define Charging_Menu2          6
#define Menu_Error              15

#define DEFAULT_CHARGE_VOLTAGE  4.2
#define DEFAULT_CHARGE_CURRENT  1.1
#define DEFAULT_CHARGE_TIME     11.1
#define knob_increment          50
#define Left_Volt_Increment     5
#define Right_Volt_Increment    0.1
#define Left_Amp_Increment      1
#define Right_Amp_Increment     0.1



int Charge_State_g = Not_Charging;
int Menu_State_g = 0;

int Button_Pressed = 0;

double Charging_Voltage = DEFAULT_CHARGE_VOLTAGE;
double Charging_Current = DEFAULT_CHARGE_CURRENT;

Metro EndPrecharging(1500);
Metro ChargeMenuFlip(3000);
char Error_Msg = 0;

// INITIALIZE ENCODERS
Encoder knobLeft(KNOB_L_PA, KNOB_L_PB);
Encoder knobRight(KNOB_R_PA, KNOB_R_PB);

// ENCODER VALUES
long positionLeft  = -999;
long positionRight = -999;

// INTIALIZE BOUNCE OBJECTS FOR ENCODER PUSHBUTTONS
Bounce L_debounce = Bounce(); 
Bounce R_debounce = Bounce(); 

// INITIALIZE LCD
LiquidCrystal lcd(LCD_P1,LCD_P2,LCD_P3,LCD_P4,LCD_P5,LCD_P6);
// LCD BRIGHTNESS (0-255)
int brightness = 255;


void setup() {
  pinMode(NEG_CONTACTOR_CTRL, OUTPUT);
  pinMode(POS_CONTACTOR_CTRL, OUTPUT);

  pinMode(AMS_FAULT, INPUT);
  pinMode(IMD_FAULT, INPUT);
  pinMode(POS_CONTACTOR_SENSE, INPUT);
  pinMode(NEG_CONTACTOR_SENSE, INPUT);
  pinMode(CONTACTOR_OFF_3v3, INPUT);
  pinMode(CONTACTOR_ON_3v3, INPUT);
  pinMode(SC_STATE_3v3, INPUT);

  // INITIALIZE LCD
  LCD_Init(lcd);

  // INITIALIZE ENCODER BUTTONS + DEBOUNCE 
  KNOB_Buttons_Init(L_debounce, R_debounce);

  // SET DEFAULT STARTUP MENU
  LCD_Next_Menu(Menu_State_g, DEFAULT_CHARGE_VOLTAGE, DEFAULT_CHARGE_CURRENT, DEFAULT_CHARGE_TIME, lcd);

}

void loop() {
  // Always Check Faults --> GPIO INTERRUPT
  if(!SC_STATE_3v3 || !AMS_FAULT || !IMD_FAULT)
    {
      Charge_State_g = Not_Charging; //possibly a charge shutdown state
      digitalWrite(NEG_CONTACTOR_CTRL, LOW);
      digitalWrite(POS_CONTACTOR_CTRL, LOW);
    }

   
  //CHARGE STATE 
  //
  //
  //
  //
   switch (Charge_State_g)
   {
    case Not_Charging:
      if(digitalRead(POS_CONTACTOR_SENSE) || digitalRead(NEG_CONTACTOR_SENSE) || digitalRead(CONTACTOR_OFF_3v3) || !digitalRead(CONTACTOR_ON_3v3))
      {
        Error_Msg = CONTACTOR_CLOSED;
        Charge_State_g = Error;
      }

      break;

    case Start_Precharging:
    
      //How exactly will this work? Global command variable? Change state outside of this function? 
      //if (CAN_Receive() = START_CHARGING)
      digitalWrite(NEG_CONTACTOR_CTRL, HIGH);
      digitalWrite(POS_CONTACTOR_CTRL, HIGH);
      Charge_State_g = Precharging;
      EndPrecharging.reset();
      //START PRECHARGING TIMEOUT TIMER HERE

      break;

    case Precharging:
      
      if (digitalRead(POS_CONTACTOR_SENSE) && digitalRead(NEG_CONTACTOR_SENSE) && digitalRead(CONTACTOR_OFF_3v3) && !digitalRead(CONTACTOR_ON_3v3))
      {
        Charge_State_g = Charging;
      }
      else if (EndPrecharging.check())
      {
        Error_Msg = PRECHARGE_TIMEOUT;
        Charge_State_g = Error;
      }

      break;

    case Charging:
    
      if (!digitalRead(POS_CONTACTOR_SENSE) || !digitalRead(NEG_CONTACTOR_SENSE) || !digitalRead(CONTACTOR_OFF_3v3) || digitalRead(CONTACTOR_ON_3v3))
      {
        digitalWrite(NEG_CONTACTOR_CTRL, LOW);
        digitalWrite(POS_CONTACTOR_CTRL, LOW);
        Error_Msg = CONTACTOR_OPEN;
        Charge_State_g = Error;
      }

      break;

    case Charging_Shutdown:

      digitalWrite(NEG_CONTACTOR_CTRL, LOW);
      digitalWrite(POS_CONTACTOR_CTRL, LOW);
      Charge_State_g = Not_Charging;

      break;

    case Error:
      //switch case with error messages?
      break;

    default:
      Charge_State_g = Error;
      Error_Msg = CHARGING_STATE_ERROR;
      break;
   }


      
  //MENU STATE 
  //
  //
  //
  //
  Button_Pressed = KNOB_Buttons_Check(L_debounce, R_debounce);

  long newPosLeft = KNOB_Turn_Check_L(knobLeft);
  long newPosRight = KNOB_Turn_Check_R(knobRight);

  switch (Menu_State_g)
    {
      case Startup_Menu:
        if (Button_Pressed > 0)
          {
            Menu_State_g = Custom_Or_Default;
          }

          break;

      case Custom_Or_Default:
        if (Button_Pressed == 1)
        {
          Menu_State_g = Set_Voltage;
        }

        else if (Button_Pressed == 2)
        {
          Menu_State_g = Ask_to_Start;
          
        }

        else if (Button_Pressed == 3)
        {
          //Should we display something when this happens?
          Charge_State_g = Charging_Shutdown;
          Menu_State_g = Startup_Menu;
        }

        break;

      case Set_Voltage:
        //Asssuming turn clockwise increases position and voltage
        //Left knob
        if(newPosLeft > knob_increment)
        {
          knobLeft.write(0);
          Charging_Voltage += Left_Volt_Increment;
        }
        else if(newPosLeft < (0-knob_increment))
        {
          knobLeft.write(0);
          Charging_Voltage -= Left_Volt_Increment;
        }


          //Right KNOB
        if(newPosRight > knob_increment)
        {
          knobRight.write(0);
          Charging_Voltage += Right_Volt_Increment;
        }
        else if(newPosLeft < (0-knob_increment))
        {
          knobLeft.write(0);
          Charging_Voltage -= Right_Volt_Increment;
        }

        
        if (Button_Pressed == 2)
        {
          Menu_State_g = Set_Current;
        }

        if (Button_Pressed == 3)
        {
          Menu_State_g = Startup_Menu;
          Charge_State_g = Charging_Shutdown;
        }

        break;

      case Set_Current:
        //Asssuming turn clockwise increases position and voltage
        //Left knob
        if(newPosLeft > knob_increment)
        {
          knobLeft.write(0);
          Charging_Current += Left_Amp_Increment;
        }
        else if(newPosLeft < (0-knob_increment))
        {
          knobLeft.write(0);
          Charging_Current -= Left_Amp_Increment;
        }


          //Right KNOB
        if(newPosRight > knob_increment)
        {
          knobRight.write(0);
          Charging_Current += Right_Amp_Increment;
        }
        else if(newPosLeft < (0-knob_increment))
        {
          knobLeft.write(0);
          Charging_Current -= Right_Amp_Increment;
        }

        
        if (Button_Pressed == 2)
        {
          Menu_State_g = Ask_to_Start;
        }

        if (Button_Pressed == 3)
        {
          Menu_State_g = Startup_Menu;
          Charge_State_g = Charging_Shutdown;
        }

        break;

      case Ask_to_Start:
        if (Button_Pressed == 1)
        {
          Menu_State_g = Custom_Or_Default;
        }
        else if (Button_Pressed == 2)
        {
          Menu_State_g = Charging_Menu1;
          ChargeMenuFlip.reset();   //reset timer to change charge menus
          Charge_State_g = Precharging;
        }
        else if (Button_Pressed == 3)
        {
          Menu_State_g = Startup_Menu;
          Charge_State_g = Charging_Shutdown;
        }

        break;

      case Charging_Menu1:
        if (Button_Pressed == 3)
        {
          Menu_State_g = Startup_Menu;
          Charge_State_g = Charging_Shutdown;
        }

        if (ChargeMenuFlip.check())
        {
          Menu_State_g = Charge_Menu2;
          ChargeMenuFlip.reset();
        }

        break;

      case Charging_Menu2:
        if (Button_Pressed == 3)
        {
          Menu_State_g = Startup_Menu;
          Charge_State_g = Charging_Shutdown;
        }

        if (ChargeMenuFlip.check())
        {
          Menu_State_g = Charge_Menu1;
          ChargeMenuFlip.reset();
        }

        break;

      default:
        //Nothing?
        break;
    }
  
}
