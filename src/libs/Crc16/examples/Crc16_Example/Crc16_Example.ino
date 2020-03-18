#include <Crc16.h>
//Crc 16 library (XModem)
Crc16 crc; 

void setup()
{
    Serial.begin(9600); 
    Serial.println("CRC-16 bit test program");
    Serial.println("=======================");
  
}

void loop()
{
  //calculate crc incrementally  
                  
  byte data[] = {192,192, 5, 41, 2, 23, 1, 1, 105, 0, 0};

  unsigned short value;
 
  //DNP3
  value = crc.DNP3(data,0,11);
  Serial.print("DNP3 crc = 0x");    
  Serial.println(value, HEX);

  while(true);
}
