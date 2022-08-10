#include <SPI.h>
#include <LoRa.h>
#include <advancedSerial.h>

#include "WiFi.h"
#include "Lora_imp.h"
#include "datos.h"
#include "hardware.h"
#include "protocolo.h"
#include "mq_sec.h"
#include "maqsec.h"

int counter = 0;
unsigned long timeout_LoRaSender = 0;
char nombreBLE[DATOS_NUMERO_SERIE +  1];
bool flag_comando_vincular = false;
char lora_sn_sender[11], lora_sn_receiver[11];


static int Lora_imp_Desempaquetar_Mensaje (String pakete, int longitud_loc);
static int Lora_imp_Extraer_SERIALNUMBERS_PCBRELES (String pakete, int longitud_loc, char *sn_cabecera);
//static int Lora_imp_Extraer_CRC (String pakete);
static void Lora_imp_Cambiar_String_Char (String pakete, char *mensaje_recibido, int total_length);

void Lora_imp_setup (void) {
  Serial.println(F("LoRa Sender"));
  
  LoRa.setPins (NSS, RESET,DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println(F("Starting LoRa failed!"));
    //while (1);
  }
  //Seleccionar potencia de transmisión (2dB - 20dB). ATENCIÓN!!! que en algunos países no admiten 20.
  // Habría que mirar las leyes nacionales al respecto. De todas formas hacer pruebas.
  LoRa.setTxPower (20);     
  LoRa_Serial_Number ();
}


void Lora_imp_loop (unsigned long T_actual) {
  //char mensaje_[60] = "Angel Ropero";
  char mensaje_[60] = "En un lugar de la Mancha de cuyo nombre no quiero acordarme";
  char mensaje_protocolo[500];
  int longitud_mensaje = 0, length_sender = 0;
  char str[150] = "";

  if ((T_actual - timeout_LoRaSender) > TIMEOUT_SEND_LORA) {
    longitud_mensaje = strlen (mensaje_);
    length_sender = protocolo_componer_Mensaje (mensaje_, longitud_mensaje, mensaje_protocolo);
    Serial.print (F("*****MENSAJE PROTOCOLO: "));
    //for (int i4 = 0; i4 < length_sender; i4++) {
    //  Serial.print (mensaje_protocolo[i4], HEX);
    //}
    array_to_string(mensaje_protocolo, length_sender, str);
    Serial.print (str);
    Serial.println ();
    timeout_LoRaSender = T_actual;
    Serial.print(F("Sending packet: "));
    Serial.println(counter);
    // send packet
    LoRa.beginPacket();
    //LoRa.print("hello ");
    LoRa.print(str);
    //LoRa.print(counter);
    LoRa.endPacket();
    //digitalWrite (led_sender, HIGH);
    //delay (100);
    //digitalWrite (led_sender, LOW);
    counter++;
  }
}
void Lora_imp_SendPacket (char *packet, int total_length) {
  char str[N_MAXIMO_CARACTERES_MENSAJE];

  memset (str, 0, sizeof(str));
  Serial.print(F("Sending packet: "));
  //Serial.println (packet);
  array_to_string(packet, total_length, str);
  Serial.println (str);
  // send packet
  LoRa.beginPacket();
  LoRa.print(str);
  LoRa.endPacket();
  //digitalWrite (led_sender, HIGH);
  //delay (100);
  //digitalWrite (led_sender, LOW);
}

//RECEIVER
int Lora_imp_CheckPacket (void) {
  int comando_LoRa = LORA_NULL;
  String rxValue = ""; 
  //char nameBluetooth[DATOS_NUMERO_SERIE + 1];
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    rxValue = LoRa.readString();
    Serial.print (F("Received packet: "));
    Serial.print (rxValue);
    Serial.print (F(", with RSSI "));
    Serial.println (LoRa.packetRssi());
    Serial.print(F("Longitud del paquete1: "));
    Serial.println (rxValue.length());
    //digitalWrite (led_sender, HIGH);
    //delay (100);
    //digitalWrite (led_sender, LOW);
    if (rxValue.length() != 0) {
      comando_LoRa = Lora_imp_Desempaquetar_Mensaje (rxValue, rxValue.length());
      return comando_LoRa;
    }
  }
  return LORA_NULL;
}
int Lora_imp_Desempaquetar_Mensaje (String pakete, int longitud_loc) {
  int result;
  char sn_cabecera[DATOS_NUMERO_SERIE + 1];
  char mensaje_vuelta[N_MAXIMO_CARACTERES_MENSAJE];
  char mensaje_recibido[N_MAXIMO_CARACTERES_MENSAJE];
  // Nota: La función strtok es una función estándar de la librería de 
  //       C++, pero no es un miembro clase de String, por lo que no se
  //       puede usar con objetos String. Hay que utilizar la función
  //       miembro c_str() para poder acceder al puntero de char del
  //       objeto String.
  //s = strtok(campos.c_str(), "#");
  
  //PROCESO DE DESEMPAQUETAR EL COMANDO RECIBIDO.
  memset (mensaje_recibido, 0, sizeof(mensaje_recibido));
  Lora_imp_Cambiar_String_Char (pakete, mensaje_recibido, longitud_loc);
  if (longitud_loc < LONGITUD_MINIMA_MENSAJE_LORA) return LORA_LONGITUD_NULL;
  memset (sn_cabecera, 0, sizeof (sn_cabecera));
  result = Lora_imp_Extraer_SERIALNUMBERS_PCBRELES (pakete, (DATOS_NUMERO_SERIE*2), sn_cabecera);
  if (result != LORA_SERIALNUMBER_SENDER_OK && result != LORA_SERIALNUMBER_SENDER_VALIDO) {
    Serial.println (F("---- SALGO DE DESEMPAQUETAR EL MENSAJE PORQUE EL SN_RELES NO ES OK O NO ES VALIDO -----"));
    return LORA_SERIALNUMBER_RECEIVER_NULL;
  }
  memset (mensaje_vuelta, 0, sizeof(mensaje_vuelta));
  result = protocolo_DESCRIFRAR_Mensaje (pakete, mensaje_vuelta, longitud_loc, mensaje_recibido);
  if (result == LORA_PAYLOAD_OK){
    Serial.println (F("*************************************"));
    Serial.println (F("********** LORA_PAYLOAD_OK **********"));
    Serial.println (F("*************************************"));
  } else if (result == LORA_VINCULAR) {
    Serial.println (F("******************************"));
    Serial.println (F("********** VINCULAR **********"));
    Serial.println (F("******************************"));
  } else if (result == LORA_PING) {
    Serial.println (F("**************************"));
    Serial.println (F("********** PING **********"));
    Serial.println (F("**************************"));
  } else if (result == LORA_ACK) {
    if (mq_sec_ESTADO () == VINCULAR) lora_imp_GRABAR_SNRECEIVER (sn_cabecera);
    Serial.println (F("*************************"));
    Serial.println (F("********** ACK **********"));
    Serial.println (F("*************************"));
  } else {
    Serial.println (F("*********************************"));
    Serial.println (F("********** DESCONOCIDO **********"));
    Serial.println (F("*********************************"));
  }

  return result;
}

void Lora_imp_Cambiar_String_Char (String pakete, char *mensaje_recibido, int total_length) {

  for (int i = 0; i < total_length; i++) {
    mensaje_recibido[i] = pakete[i];
  }
  //Serial.print (F("MENSAJE TRANSFORMADO: "));
  //Serial.println (mensaje_recibido);
}

int Lora_imp_Extraer_SERIALNUMBERS_PCBRELES (String pakete, int longitud_loc, char *sn_cabecera) {
  int result_cabecera = LORA_SERIALNUMBER_SENDER_NULL, result_reles = LORA_SERIALNUMBER_RECEIVER_NULL;

  array_to_ASCII (pakete, 10, 0, sn_cabecera);          //Lee el SN_CABECERA que es el SENDER.
  array_to_ASCII (pakete, 10, 10, lora_sn_receiver);    //Lee el SN_RELES que es el RECEIVER.
  //AHORA HAY QUE COMPROBAR QUE EL SN_RECEIVER QUE SOY YO, PORQUE SOY EL DE RELÉS, COINCIDE CON MI NUMERO DE SERIE.
  result_reles = protocolo_Test_SN_RECEIVER (lora_sn_receiver);
  //Serial.print (F("SN_SENDER-CABECERA(REMOTO): "));
  //protocolo_PRINTLN_STRING (sn_cabecera);        //SIMPLEMENTE PARA MOSTRARLO EN ASCII.
  //Serial.print (F("SN_RECEIVER-RELES(MIO): "));
  //protocolo_PRINTLN_STRING (lora_sn_receiver);      //SIMPLEMENTE PARA MOSTRARLO EN ASCII.
  //AQUI SE COMPRUEBA SI EL SN_SENDER QUE SE RECIBE ES EL ALMACENADO EN LA EEPROM EN LA POSICIÓN DEL RECEIVER.
  result_cabecera = protocolo_Test_SN_SENDER (sn_cabecera);
  if (result_reles != LORA_SERIALNUMBER_RECEIVER_OK) {
    Serial.println (F("----- SALGO DE COMPROBAR NUMEROS DE SERIE PORQUE EL SN_RELES NO ES CORRECTO -----"));
    return LORA_SERIALNUMBER_RECEIVER_NULL;
  }
  return (result_cabecera);
}
void Lora_imp_RESET_SNSENDER (void) {
  memset (lora_sn_sender, 0, sizeof(lora_sn_sender));   //Esto es para que no se quede ningun SN que hubiera de antes o el que acaba de llegar.
}
void lora_imp_GRABAR_SNRECEIVER (char *sn_cabecera) {
  Serial.print (F("ATENCIÓN!!! -> SE HA GRABADO EL SN_CABECERA EN LA EEPROM: "));
  for (int i = 0; i < DATOS_NUMERO_SERIE; i++) {
    datos_EEPROM_Write_BYTE (sn_cabecera[i], DAT_SN_RECEIVER + i);   //GRABA EN LA EEPROM, EN LA POSICIÓN RECEIVER, EL SN-SENDER (CABECERA).
    Serial.print (sn_cabecera[i]);
  }
  Serial.println ();
}

void Lora_imp_Test_SNRECEIVER (void) {

  Serial.print (F("SN_RECEIVER ALMACENADO TEMPORALMENTE: "));
  Serial.println (lora_sn_receiver);

}

void LoRa_Serial_Number (void) {
  byte valor_EEPROM[4];
  String nameBluetooth;

  memset (nombreBLE, 0, sizeof(nombreBLE));
  WiFi.mode(WIFI_MODE_STA);
  Serial.print (F("MAC ADDRESS: "));
  nameBluetooth = WiFi.macAddress();
  Serial.println (nameBluetooth);
  nombreBLE[0] = 'S';
  nombreBLE[1] = 'M';
  nombreBLE[2] = 'F';
  nombreBLE[3] = 'R';
  nombreBLE[4] = nameBluetooth[9];
  nombreBLE[5] = nameBluetooth[10];
  nombreBLE[6] = nameBluetooth[12];
  nombreBLE[7] = nameBluetooth[13];
  nombreBLE[8] = nameBluetooth[15];
  nombreBLE[9] = nameBluetooth[16];
  Serial.print (F("SERIAL NUMBER: "));
  Serial.println (nombreBLE);

  Serial.print (F("VALOR NUMERO DE SERIE: "));
  for (int i = 0; i < DATOS_NUMERO_SERIE; i++) {
    valor_EEPROM[0] = datos_EEPROM_Read_BYTE (DAT_SN_SENDER + i);
    Serial.print ((char) valor_EEPROM[0]);
  }
  Serial.println ();
  valor_EEPROM[0] = datos_EEPROM_Read_BYTE (DAT_SN_SENDER);
  valor_EEPROM[1] = datos_EEPROM_Read_BYTE (DAT_SN_SENDER + 1);
  valor_EEPROM[2] = datos_EEPROM_Read_BYTE (DAT_SN_SENDER + 2);
  valor_EEPROM[3] = datos_EEPROM_Read_BYTE (DAT_SN_SENDER + 3);
  if (valor_EEPROM[0] != 'S' || valor_EEPROM[1] != 'M' || valor_EEPROM[2] != 'F' || valor_EEPROM[3] != 'R') {
    Serial.println (F("¡¡¡¡¡GRABAR NUMERO DE SERIE!!!!!"));
    for (int i = 0; i < DATOS_NUMERO_SERIE; i++) {
      datos_EEPROM_Write_BYTE (nombreBLE[i], DAT_SN_SENDER + i);
    }
  } else {
    Serial.println (F("¡¡¡¡¡NO SE GRABA NOMBRE BLUETOOTH!!!!!"));
  }
}

//int Lora_imp_Extraer_CRC (String pakete) {
//  return LORA_CRC_OK;
//}
