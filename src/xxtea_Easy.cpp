/*

  Easy to Use example of xxtea-lib Cryptographic Library

  This example shows the calling convention for the various functions.

  For more information about this library please visit us at
  http://github.com/boseji/xxtea-lib

  Created by Abhijit Bose (boseji) on 04/03/16.
  Copyright 2016 - Under creative commons license 3.0:
        Attribution-ShareAlike CC BY-SA

  @version API 2.0.0
  @author boseji - salearj@hotmail.com

*/

#include <xxtea-lib.h>
#include <advancedSerial.h>

#include "hardware.h"
#include "xxtea_Easy.h"

//Text to Encrypt - ! Carefull no to more than 80 bytes ! - Or See `Limitations`. ARREGLADO, AMPLIADO A 500 en la librería <xxtea-lib.h>
// #define MAX_XXTEA_DATA8  500
String plaintext = F("En un lugar de la mancha de cuyo nombre no quiero acordarme, había un hidalgo llamado D. Quijote de espada y armadura...");

void xxtea_Easy_setup (void) {
  
  // Set the Password
  // Key for Encrypt - ! Carefull no to more than 16 bytes ! - Or See `Limitations`
  xxtea.setKey("XXXXXXIMPXXXXXX");

}

void xxtea_Easy_loop (void) {
  long TimeA, TimeB;

  // Perform Encryption on the Data
  Serial.print(F(" Encrypted Data: "));
  TimeA = micros();
  String result = xxtea.encrypt(plaintext);
  TimeB = micros();
  int longitud_result = result.length ();
  result.toLowerCase(); // (Optional)
  Serial.println(result);
  Serial.print (F("Longitud de la cadena cifrada: "));
  Serial.println (longitud_result);
  Serial.print (F("Tiempo de cifrado: "));
  Serial.println (TimeB - TimeA);
  

  // Perform Decryption
  Serial.print(F(" Decrypted Data: "));
  TimeA = micros();
  String Cadena_disfrada = xxtea.decrypt(result);
  TimeB = micros();
  Serial.println(Cadena_disfrada);
  int longitud_cadena = Cadena_disfrada.length ();
  Serial.print (F("Longitud de la cadena descifrada: "));
  Serial.println (longitud_cadena);
  Serial.print (F("Tiempo de descifrado: "));
  Serial.println (TimeB - TimeA);

}
