#include <advancedSerial.h>

#include "crc.h"


uint16_t CRC_ChecksumFletcher16(char *data, size_t count ) {
   uint16_t sum1 = 0;
   uint16_t sum2 = 0;

   for(size_t index = 0; index < count; ++index ) {
    sum1 = sum1 + (uint8_t)data[index];
    while (sum1>>8) 
      sum1 = (sum1 & 0xF) + (sum1 >> 8);
   
    sum2 = sum2 + sum1;
    while (sum2>>8)
      sum2 = (sum2 & 0xF) + (sum2 >> 8);

    //Serial.print (F("CRC Dato: "));
    //Serial.print (data[index], HEX);
    //Serial.print (F(", \tindex: "));
    //Serial.print (index);
    //Serial.print (F(", \tCRC: "));
    //Serial.println (((sum2 << 8) | sum1), HEX);
   }
   return (sum2 << 8) | sum1;
}
