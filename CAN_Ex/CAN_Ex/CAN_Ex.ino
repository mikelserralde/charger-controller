#include <FlexCAN_T4.h>
#include <isotp.h>
isotp<RX_BANKS_16, 512> tp; /* 16 slots for multi-ID support, at 512bytes buffer each payload rebuild */
isotp<RX_BANKS_16, 512> tp2;

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;//Car CAN
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can3;//Charger CAN

CAN_message_t msg;  //message to send over CAN - > tp.write
CAN_message_t Rmsg; //message to store CAN.read value

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
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print(" OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print(" LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" BUS: "); Serial.print(msg.bus);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
}

void setup() {
  //untouched send/receive example
  Serial.begin(115200); delay(400);
  Can2.begin();
  Can2.setClock(CLK_60MHz);
  Can2.setBaudRate(500000);
  Can2.setMaxMB(16);
  Can2.enableFIFO();
  Can2.enableFIFOInterrupt();
  Can2.onReceive(canSniff);
  tp.begin();
  tp.setWriteBus(&Can2); /* we write to this bus */
  tp.onReceive(myCallback); /* set callback */

//duplicated send/receive example
  Can3.begin();
  Can3.setClock(CLK_60MHz);
  Can3.setBaudRate(500000);
  Can3.setMaxMB(16);
  Can3.enableFIFO();
  Can3.enableFIFOInterrupt();
  Can3.onReceive(canSniff);
  tp2.begin();
  tp2.setWriteBus(&Can3); /* we write to this bus */
  tp2.onReceive(myCallback); /* set callback */
}

void loop() {

  
  //untouched send/receive example
  static uint32_t sendTimerCAR = millis();
  if ( millis() - sendTimerCAR > 4000 ) {
    Serial.println("Checking CAR CAN AAAAAA.......");
    uint8_t buf[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5 };
    const char b[] = "01413AAAAABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    ISOTP_data config;
    config.id = 0x666;
    config.flags.extended = 0; /* standard frame */
    config.separation_time = 10; /* time between back-to-back frames in millisec */
    tp.write(config, buf, sizeof(buf));
    tp.write(config, b, sizeof(b));
    sendTimerCAR = millis();
    //basic stuff I wrote to ignore tp man fuck that guy
//    msg.buf[0] = 1;
//    msg.id = 666;
//    Can2.write(msg);
  }


//duplicated from send/receive example

//  static uint32_t sendTimerCHARGER = millis();
//  if ( millis() - sendTimerCHARGER > 5000 ) {
//    Serial.println("Checking CHARGER CAN 1111111111.......");
//    uint8_t buf[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5 };
//    const char b[] = "01413AAAAABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
//    ISOTP_data config;
//    config.id = 0x111;
//    config.flags.extended = 0; /* standard frame */
//    config.separation_time = 10; /* time between back-to-back frames in millisec */
//    tp2.write(config, buf, sizeof(buf));
//    tp2.write(config, b, sizeof(b));
//    sendTimerCHARGER = millis();
////    msg.buf[0] = 2;
////    msg.id = 420;
////    Can3.write(msg);
//  }


//Untouched form dual receivers example

//  if(Can2.read(Rmsg))
//  {
//    Serial.print("CAN2 "); 
//    Serial.print("MB: "); Serial.print(Rmsg.mb);
//    Serial.print("  ID: 0x"); Serial.print(Rmsg.id, HEX );
//    Serial.print("  EXT: "); Serial.print(Rmsg.flags.extended );
//    Serial.print("  LEN: "); Serial.print(Rmsg.len);
//    Serial.print(" DATA: ");
//    for ( uint8_t i = 0; i < 8; i++ ) {
//      Serial.print(Rmsg.buf[i]); Serial.print(" ");
//    }
//    Serial.print("  TS: "); Serial.println(Rmsg.timestamp);
//  }
//  

//untouched dual receivers example

  if(Can3.read(Rmsg))
  {
    Serial.print("CAN3 "); 
    Serial.print("MB: "); Serial.print(Rmsg.mb);
    Serial.print("  ID: 0x"); Serial.print(Rmsg.id, HEX );
    Serial.print("  EXT: "); Serial.print(Rmsg.flags.extended );
    Serial.print("  LEN: "); Serial.print(Rmsg.len);
    Serial.print(" DATA: ");
    for ( uint8_t i = 0; i < 8; i++ ) {
      Serial.print(Rmsg.buf[i]); Serial.print(" ");
    }
    Serial.print("  TS: "); Serial.println(Rmsg.timestamp);
  }
}
