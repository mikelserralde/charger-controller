/*
Contains stuff about UART
*/

char UART_Init(Serial mySerial, long baud)
{
  Serial.begin(9600);
  while(!Serial);   //wait for serial to connect timeout?
  mySerial.begin(baud);
  //mySerial_p->begin(baud)  //Maybe pass serial obj as pointer?
}


char UART_Transmit(Serial mySerial, char data)
{
  
  if (Serial.available())
    return mySerial.write(data);  //returns number of bytes written
  else
    return SERIAL_UNAVAILABLE
}


char UART_Receive(Serial mySerial)
{
  
  if (mySerial.available())
    return mySerial.read();  //returns number of bytes written
  else
    return SERIAL_UNAVAILABLE //Different error code
}
