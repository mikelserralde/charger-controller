/*
 * STATE MACHINE
 * 
 * 
 * 
 * 
 * TO DEFINE: 
 *  
 *  Contactor_Open (3.3V on accum)
    Contactor_Closed (3.3V on accum)
    Pos Contactor Sense - high if contactor closed
    Neg Contactor Sense
    SC After Interconnect Sense
    AMS Fault Sense - AMS_FAULT_SENSE
    IMD Fault Sense - IMD_FAULT_SENSE
  
    Pos Contactor Control - Pos_Contactor_Ctrl
    Neg Contactor Control - Neg_Contactor_Ctrl
    Pos Contactor Control (accum)?
    Neg Contactor Control (accum)?

    States: Not_Charging, Precharge, Charging, ??Charging Shutdown??, error state???
    Errors: CONTACTOR_CLOSED, CONTACTOR_OPEN, PRECHARGE_TIMEOUT,  
 */

#define GPT1_base _SFT_MEM8(0x401EC000

void setup() {

    char Charge_State_g = Not_Charging;
    GPT1_CNT = 0;

}

void loop() {
  // Always Check Faults --> GPIO INTERRUPT
  if(!SC_After_Interconnect || !AMS_FAULT_SENSE || !IMD_FAULT_SENSE)
    {
      Charge_State_g = Not_Charging //possibly a charge shutdown state
      Neg_Contactor_Ctrl = 0;
      Pos_Contactor_Ctrl = 0;
    }

   switch Charge_State_g:
   {
    case Not_Charging:
      if(Pos_Contactor_Sense || Neg_Contactor_Sense || Open_Contactor || !Closed_Contactor)
      {
        return CONTACTOR_CLOSED;
      }
      //How exactly will this work? Global command variable? Change state outside of this function? 
      if (CAN_Receive() = START_CHARGING)
      {
        Neg_Contactor_Ctrl = 1;
        Pos_Contactor_Ctrl = 1;
        Charge_State_g = Precharging;
        //START PRECHARGING TIMEOUT TIMER HERE
      }
      break;
    case Precharge:
      
      if (Pos_Contactor_Sense && Neg_Contactor_Sense && Open_Contactor && !Closed_Contactor)
      {
        Charge_State_g = Charging;
        //reset precharging timer
      }
      else if (TIMER_COUNT > 1500)
      {
        //reset precharging timer
        return PRECHARGE_TIMEOUT;
      }
       //How exactly will this work? Global command variable? Change state outside of this function? 
      else if (CAN_Receive() = STOP_CHARGING)
      {
        Neg_Contactor_Ctrl = 0;
        Pos_Contactor_Ctrl = 0;
        Charge_State_g = Not_Charging;
        //Reset precharging timer
      }
      
      break;

    case Charging:
      if (!Pos_Contactor_Sense || !Neg_Contactor_Sense || !Open_Contactor || Closed_Contactor)
      {
        Charging_State_g = Not_Charging;
        Neg_Contactor_Ctrl = 0;
        Pos_Contactor_Ctrl = 0;
        return CONTACTOR_OPEN;
      }

      //How exactly will this work? Global command variable? Change state outside of this function? 
      if (CAN_Receive() = STOP_CHARGING)
      {
        Neg_Contactor_Ctrl = 0;
        Pos_Contactor_Ctrl = 0;
        Charge_State_g = Not_Charging;
        // Checking that contactors are actually open right here, Not_Charging, or another state?
      }

      break;

    default:
      return CHARGING_STATE_ERROR;
      break;
   }
  

  
}
