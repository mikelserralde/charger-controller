

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

//Charging_State_g
#define Not_Charging            1
#define Start_Precharging       2
#define Precharging             3
#define Start_Charging          4
#define Charging                5
#define Charging_Shutdown       6
#define Error                   7


char Charge_State_g = Not_Charging;
Metro EndPrecharge(1500);
char Error_Msg = 0;

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

}

void loop() {
  // Always Check Faults --> GPIO INTERRUPT
  if(!SC_STATE_3v3 || !AMS_FAULT || !IMD_FAULT)
    {
      Charge_State_g = Not_Charging; //possibly a charge shutdown state
      digitalWrite(NEG_CONTACTOR_CTRL, LOW);
      digitalWrite(POS_CONTACTOR_CTRL, LOW);
    }

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
      EndPrecharge.reset();
      //START PRECHARGING TIMEOUT TIMER HERE

      break;

    case Precharging:
      
      if (digitalRead(POS_CONTACTOR_SENSE) && digitalRead(NEG_CONTACTOR_SENSE) && digitalRead(CONTACTOR_OFF_3v3) && !digitalRead(CONTACTOR_ON_3v3))
      {
        Charge_State_g = Charging;
      }
      else if (EndPrecharge.check())
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
      
      break;

    default:
      Charge_State_g = Error;
      Error_Msg = CHARGING_STATE_ERROR;
      break;
   }
  

  
}
