
#ifndef CAN_EX_H
#define CAN_EX_H

#include <FlexCAN_T4.h>
#include <isotp.h>
#include <Metro.h>



//untouched send/receive example
void myCallback(const ISOTP_data &config, const uint8_t *buf);

//untouched send/receive example
void canSniff(const CAN_message_t &msg);

//Updates values of current, voltage, and charger status variables based on msg from Battery Charger
//current and voltage variables are 10 times what they really are
// Ex Current = 582 -- > 58.2 A
// Ex Voltage = 3201 --> 320.1 V
void ReadChargerMsg(uint16_t &current, uint16_t &voltage, uint8_t &ChargerStatus, CAN_message_t &msg);


//formats a message to send max allowable voltage / current, ChargeControl = 1 --> start charging, 0 --> stop charging
void SendChargerMsg(uint16_t &current, uint16_t &voltage, bool ChargeControl, CAN_message_t &msg);


//Contactors: if a 1 means closed: 100 contact+ closed, 010 contact - closed, 001 contact precharge closed, 111 all closed etc.
void ReadAMSStatus(uint8_t &FaultMessage, uint8_t &Contactors, uint16_t &MaxCellTemp, uint16_t &MinCellTemp, uint16_t StateofCharge, const CAN_message_t &msg);


void ReadAMSVoltages(uint16_t &PackVoltage, uint16_t &VehicleVoltage, uint16_t &MaxCellVoltage, uint16_t &MinCellVoltage, const CAN_message_t &msg);


void ReadAMSCurrents(uint16_t &PackCurrent, uint16_t &DischargeCurrentLimit, uint16_t &ChargeCurrentLimit, uint16_t &PeakCurrent, const CAN_message_t &msg);


void ReadCellVoltandTemp(uint8_t * buff, const CAN_message_t &msg);






#endif
