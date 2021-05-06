#include <FlexCAN_T4.h>
#include <isotp.h>
#include <Metro.h>
isotp<RX_BANKS_16, 512> tp; /* 16 slots for multi-ID support, at 512bytes buffer each payload rebuild */
isotp<RX_BANKS_16, 512> tp2;

//FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;//Car CAN
//FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can3;//Charger CAN
//
//CAN_message_t msg;  //message to send over CAN - > tp.write
//
//CAN_message_t Rmsg; //message to store CAN.read value

//Metro ChargerHeartbeat(1000);


int msgcnt=0;





//untouched send/receive example
void myCallback(const ISOTP_data &config, const uint8_t *buf) {
  Serial.print("ID: ");
  Serial.print(config.id, HEX);
  Serial.print("\tLEN: ");
  Serial.print(config.len);
  Serial.print("\tFINAL ARRAY: ");
  for ( int i = 0; i < config.len; i++ ) {
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  } Serial.println();
}
//untouched send/receive example
void canSniff(const CAN_message_t &msg) {

    switch(msg.id){

    case 0x10:
      ReadAMSStatus(FaultMessage, Contactors, MaxCellTemp, MinCellTemp, StateofCharge, msg);
//      Serial.print("   FaultMessage ");
//      Serial.print(FaultMessage, HEX);
//      Serial.print("   Contactors ");
//      Serial.print(Contactors, DEC);
//      Serial.print("   MaxCellTemp ");
//      Serial.print(MaxCellTemp, DEC);
//      Serial.print("   MinCellTemp ");
//      Serial.println(MinCellTemp, DEC);
      break;

    case 0x11:
      ReadAMSVoltages(PackVoltage, VehicleVoltage, MaxCellVoltage, MinCellVoltage, msg);
//      Serial.print("   PackVoltage");
//      Serial.print(PackVoltage, DEC);
//      Serial.print("   VehicleVoltage: ");
//      Serial.print(VehicleVoltage, DEC);
//      Serial.print("   MaxCellVoltage ");
//      Serial.print(MaxCellVoltage, DEC);
//      Serial.print("   MinCellVoltage ");
//      Serial.println(MinCellVoltage, DEC);
      break;

    case 0x12: 
      ReadAMSCurrents(PackCurrent, DischargeCurrentLimit, ChargeCurrentLimit, PeakCurrent, msg);
//      Serial.print("   PackCurrent ");
//      Serial.print(PackCurrent, DEC);
//      Serial.print("   DischargeCurrentLimit ");
//      Serial.print(DischargeCurrentLimit, DEC);
//      Serial.print("   ChargeCurrentLimit ");
//      Serial.print(ChargeCurrentLimit, DEC);
//      Serial.print("   PeakCurrent ");
//      Serial.println(PeakCurrent, DEC);
      
      break;

    case 0x50:
      ReadCellVoltandTemp(buff, msg);
//      for (int i = 0; i < 245; i++)
//      {
//        Serial.print(buff[i], HEX);
//      }
//      Serial.println();
      break;

    default:
      Serial.println("WTF");
      break;
    }
}

//Updates values of current, voltage, and charger status variables based on msg from Battery Charger
//current and voltage variables are 10 times what they really are
// Ex Current = 582 -- > 58.2 A
// Ex Voltage = 3201 --> 320.1 V
void ReadChargerMsg(uint16_t &current, uint16_t &voltage, uint8_t &ChargerStatus, CAN_message_t &msg)
{
  //read in voltage high byte then low byte
  voltage = msg.buf[0];
  voltage = voltage << 8;
  voltage |= msg.buf[1];

  //read in current high byte then low byte
  current = msg.buf[2];
  current = current << 8;
  current |= msg.buf[3];

  ChargerStatus = msg.buf[4];
}


//formats a message to send max allowable voltage / current, ChargeControl = 1 --> start charging, 0 --> stop charging
void SendChargerMsg(uint16_t &current, uint16_t &voltage, bool ChargeControl, CAN_message_t &msg)
{

  msg.id = 0x1806E5F4;
  //msg.id = 0xF4E50618;
  msg.buf[0] = (uint8_t)(voltage >> 8);
  msg.buf[1] = (uint8_t)(voltage);
  msg.buf[2] = (uint8_t)(current >> 8);
  msg.buf[3] = (uint8_t)(current);

  if (ChargeControl == 1) msg.buf[4] = 0x00; //Start Charging
  if (ChargeControl == 0) msg.buf[4] = 0xFF; //Stop Charging
  
}

//Contactors: if a 1 means closed: 100 contact+ closed, 010 contact - closed, 001 contact precharge closed, 111 all closed etc.
void ReadAMSStatus(uint8_t &FaultMessage, uint8_t &Contactors, uint16_t &MaxCellTemp, uint16_t &MinCellTemp, uint16_t StateofCharge, const CAN_message_t &msg)
{
  FaultMessage = msg.buf[0];
  Contactors = ((msg.buf[1] >> 4) & ( 0x07));

  MaxCellTemp = msg.buf[2];
  MaxCellTemp = MaxCellTemp << 8;
  MaxCellTemp |= msg.buf[3];

  MinCellTemp = msg.buf[4];
  MinCellTemp = MinCellTemp << 8;
  MinCellTemp |= msg.buf[5];

  StateofCharge = msg.buf[6];
  StateofCharge = StateofCharge << 8;
  StateofCharge |= msg.buf[7];
}


void ReadAMSVoltages(uint16_t &PackVoltage, uint16_t &VehicleVoltage, uint16_t &MaxCellVoltage, uint16_t &MinCellVoltage, const CAN_message_t &msg)
{
  PackVoltage = msg.buf[0];
  PackVoltage = PackVoltage << 8;
  PackVoltage |= msg.buf[1];

  VehicleVoltage = msg.buf[2];
  VehicleVoltage = VehicleVoltage << 8;
  VehicleVoltage |= msg.buf[3];

  MaxCellVoltage = msg.buf[4];
  MaxCellVoltage = MaxCellVoltage << 8;
  MaxCellVoltage |= msg.buf[5];

  MinCellVoltage = msg.buf[6];
  MinCellVoltage = MinCellVoltage << 8;
  MinCellVoltage |= msg.buf[7];
}


void ReadAMSCurrents(uint16_t &PackCurrent, uint16_t &DischargeCurrentLimit, uint16_t &ChargeCurrentLimit, uint16_t &PeakCurrent, const CAN_message_t &msg)
{
  PackCurrent = msg.buf[0];
  PackCurrent = PackCurrent << 8;
  PackCurrent |= msg.buf[1];

  DischargeCurrentLimit = msg.buf[2];
  DischargeCurrentLimit = DischargeCurrentLimit << 8;
  DischargeCurrentLimit |= msg.buf[3];

  ChargeCurrentLimit = msg.buf[4];
  ChargeCurrentLimit = ChargeCurrentLimit << 8;
  ChargeCurrentLimit |= msg.buf[5];

  PeakCurrent = msg.buf[6];
  PeakCurrent = PeakCurrent << 8;
  PeakCurrent |= msg.buf[7];
}


bool ReadCellVoltandTemp(uint8_t * buff, const CAN_message_t &msg)
{
  for (int j = 0; j < 7; j++)
  {
    buff[ j + msg.buf[7]*7] = msg.buf[j];
  }

//  Serial.println("SEQ NUM");
//  Serial.println(msg.buf[7], DEC);
}




//void setup() {
//  //untouched send/receive example
//  Serial.begin(9600); delay(400);
//  Can2.begin();
//  Can2.setClock(CLK_60MHz);
//  Can2.setBaudRate(500000);
//  Can2.setMaxMB(16);
//  Can2.setMBFilter(ACCEPT_ALL);
//  Can2.enableFIFO();
//  Can2.enableFIFOInterrupt();
//  Can2.onReceive(canSniff);
////  tp.begin();
////  tp.setWriteBus(&Can2); /* we write to this bus */
////  tp.onReceive(myCallback); /* set callback */
//
////duplicated send/receive example
//  Can3.begin();
//  Can3.setClock(CLK_60MHz);
//  Can3.setBaudRate(500000);
//  Can3.setMaxMB(16);
//
//  Can3.setMBFilter(ACCEPT_ALL);
//  Can3.enableFIFO();
//  Can3.enableFIFOInterrupt();
//  Can3.onReceive(canSniff);
////  tp2.begin();
////  tp2.setWriteBus(&Can3); /* we write to this bus */
////  tp2.onReceive(myCallback); /* set callback */
//
//
////TargetCurrent = 0;
////TargetVoltage = 0;
////ChargeControl = 0;
//
//}
//
//void loop() {
//
//  
//  if ( ChargerHeartbeat.check() ) 
//  {
//    SendChargerMsg(TargetCurrent, TargetVoltage, ChargeControl, msg);
//    Can2.write(msg);
//    //Serial.println("sending don't charge...");
//    //canSniff(msg);
//    ChargerHeartbeat.reset();
//    for (int i = 0; i < 2; i++)
//    {
//      Serial.print(buff[i]);
//    }
////    Serial.print( FaultMessage, HEX);
////    Serial.print( Contactors, HEX );
////    Serial.print( MaxCellTemp, HEX );
////    Serial.print( MinCellTemp, HEX );
////    Serial.print( ChargeControl, HEX );
////    Serial.print( PackVoltage, HEX );
////    Serial.print( VehicleVoltage, HEX );
////    Serial.print( MaxCellVoltage, HEX );
////    Serial.print( MinCellVoltage, HEX );
////    Serial.print( PackCurrent, HEX );
////    Serial.print( DischargeCurrentLimit, HEX );
////    Serial.print( ChargeCurrentLimit, HEX );
////    Serial.print( PeakCurrent, HEX);
//  }
//






//}
