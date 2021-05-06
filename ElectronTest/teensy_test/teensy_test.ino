/* Analog Input Example, Teensyduino Tutorial #4
   http://www.pjrc.com/teensy/tutorial4.html

   After uploading this to your board, use Serial Monitor
   to view the message.  When Serial is selected from the
   Tools > USB Type menu, the correct serial port must be
   selected from the Tools > Serial Port AFTER Teensy is
   running this code.  Teensy only becomes a serial device
   while this code is running!  For non-Serial types,
   the Serial port is emulated, so no port needs to be
   selected.

   This example code is in the public domain.
*/

void setup()
{                
  Serial.begin(9600);
}

int val;
uint8_t data[160];

void loop()                     
{
  for(int i=0;i<160;i++){
    data[i] = random(0,255);
   // Serial.print(random(0,255),HEX);
  }

   Serial.write( (uint8_t*)data, sizeof(data) );

//  val++;
//  Serial.print("Loop: ");
//  Serial.println(val);
  delay(2500);
}
