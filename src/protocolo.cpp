/********************************************************************************
*  Fichero: maquina.c  Modulo que integra la estructura de la máquina de estados*
*********************************************************************************
*  Descripcion: En este módulo se definen las funciones de la máquina de estados*
*              usada por las diferentes máquinas en el programa.                *
*********************************************************************************
*  Creacion del módulo: 17 de Septiembre de 2017                                *
*  Last modification:                                                           *
*  Name of creator of module: Ángel Ropero                                      *
*  Software Arduino.                                                            *
*  Enlaza con otros ficheros: mqprinc.c                                         *
*  Company: IMPOREXO                                                            *
********************************************************************************/

#include <string.h>
#include <stdio.h>
#include <advancedSerial.h>

#include "hardware.h"
#include "datos.h"
#include "protocolo.h"
#include "crc.h"
#include "Lora_imp.h"

char sn_sender[DATOS_NUMERO_SERIE + 1], sn_receptioner[DATOS_NUMERO_SERIE + 1];
int longitud_mensaje = 0;
byte mensaje_plaintext[500], mensaje_cifrado[500];
int crc_mensaje;
char protocolo_nivel[10], protocolo_media[10], protocolo_des_tip[10];
bool flag_nivel = false, flag_media = false, flag_desviacion = false;
float nivel_int = 0, media_int = 0, d_t_int = 0;
char sn_receiver[] = "0000000000";

static int    protocolo_Extraer_LONGITUD (String pakete);
static int    protocolo_Comprobar_CRC (char *mensaje_recibido, int length_payload);
static int    protocolo_Extraer_PAYLOAD (String pakete, char *mensaje, int lenght_payload, int total_length);
static float  protocolo_Extraer_INT (char *token);
static void   protocolo_array_to_string(int original, char *buffer);

void protocolo_Init (void) {
  
}

int protocolo_componer_Mensaje (char *mensaje_, int long_mensaje, char *mensaje_local) {
  int indice = 0;
  //char mensaje_local[60];
  byte dato[4];

  //SN SENDER: LECTURA E INTRODUCCIÓN EN EL MENSAJE.
  protocolo_leer_SNSENDER (sn_sender);
  for (int i1 = 0; i1 < DATOS_NUMERO_SERIE; i1++) {
    mensaje_local[indice++] = sn_sender[i1];
  }
  //SN RECEIVER: LECTURA E INTRODUCCIÓN EN EL MENSAJE.
  protocolo_leer_SNRECEIVER (sn_receptioner);
  for (int i2 = 0; i2 < DATOS_NUMERO_SERIE; i2++) {
    mensaje_local[indice++] = sn_receptioner[i2];
  }
  //LONGITUD: CALCULO E INTRODUCCIÓN EN EL MENSAJE.
  dato[0] = (long_mensaje & 0xFF);           //LSB
  dato[1] = ((long_mensaje >> 8) & 0xFF);
  dato[2] = ((long_mensaje >> 16) & 0xFF);
  dato[3] = ((long_mensaje >> 24) & 0xFF);   //MSB
  mensaje_local[indice++] = (char) dato[0];
  mensaje_local[indice++] = (char) dato[1];
  mensaje_local[indice++] = (char) dato[2];
  mensaje_local[indice++] = (char) dato[3];
  //PAYLOAD: INTRODUCCIÓN EN EL MENSAJE.
  for (int i3 = 0; i3 < long_mensaje; i3++){
    mensaje_local[indice++] = mensaje_[i3];
  }
  //CRC: CALCULO.
  uint16_t crc = CRC_ChecksumFletcher16(mensaje_local, indice);
  dato[0] = (crc & 0xFF);           //LSB
  dato[1] = ((crc >> 8) & 0xFF);
  dato[2] = ((crc >> 16) & 0xFF);
  dato[3] = ((crc >> 24) & 0xFF);   //MSB
  //CRC: INTRODUCCIÓN EN EL MENSAJE.
  mensaje_local[indice++] = (char) dato[0];
  mensaje_local[indice++] = (char) dato[1];
  mensaje_local[indice++] = (char) dato[2];
  mensaje_local[indice++] = (char) dato[3];
  mensaje_local[indice] = 0;
  //Serial.println ();
  //Serial.print (F("+++++ EL CRC es: "));
  //Serial.print (dato[0], HEX);
  //Serial.print (dato[1], HEX);
  //Serial.print (dato[2], HEX);
  //Serial.print (dato[3], HEX);
  return indice;
}


void protocolo_leer_SNSENDER (char *sender) {
  String sender_local;
  int i;

  //memset (sender, 0, sizeof(sender));
  for (i = 0; i < DATOS_NUMERO_SERIE; i++) {
    sender[i] = datos_EEPROM_Read_BYTE (DAT_SN_SENDER + i);
  }
  sender[i] = 0;
}

void protocolo_leer_SNRECEIVER (char *receptioner) {
  int i;
  memset (receptioner, 0, sizeof(receptioner));
  //Serial.print (F("SNRECEPTIONER: "));
  for (i = 0; i < DATOS_NUMERO_SERIE; i++) {
    receptioner[i] = datos_EEPROM_Read_BYTE (DAT_SN_RECEIVER + i);
    if (receptioner[i] == 0) {
      return;
    //  receptioner[i] = '0';
    }
    //Serial.print ((char) receptioner[i]);
  }
  receptioner[i] = 0;
  //Serial.println ();
}
int protocolo_Test_SN_RECEIVER (char *lora_sn_reles) {
  char receiver_local[11];
  int i;

  for (i = 0; i < DATOS_NUMERO_SERIE; i++) {
    receiver_local[i] = datos_EEPROM_Read_BYTE (DAT_SN_SENDER + i);
  }
  receiver_local[i] = '\0';
  Serial.print (F("EEPROM SN_RELES: "));
  protocolo_PRINTLN_STRING (receiver_local);
  if (!strcmp(receiver_local, lora_sn_reles)) {  
    //Serial.println (F("SN_RECEIVER RECIBIDO == EEPROM SN_RELES"));
    return LORA_SERIALNUMBER_RECEIVER_OK;
  }
  Serial.println (F("RECEIVER diferente de SN_RELES EEPROM"));
  return LORA_SERIALNUMBER_RECEIVER_NULL;
}
int protocolo_Test_SN_SENDER (char *lora_sn_cabecera) {
  char receiver_local[11];
  int i;

  for (i = 0; i < DATOS_NUMERO_SERIE; i++) {
    receiver_local[i] = datos_EEPROM_Read_BYTE (DAT_SN_RECEIVER + i);
  }
  receiver_local[i] = '\0';
  Serial.print (F("EEPROM SN_CABECERA: "));
  Serial.println (receiver_local);
  //Serial.print (F("SN_CABECERA A COMPARAR: "));
  //protocolo_PRINTLN_STRING (lora_sn_cabecera);
  if (!strcmp(receiver_local, lora_sn_cabecera)) {  
    //Serial.println (F("SN_SENDER RECIBIDO == EEPROM SN_CABECERA"));
    return LORA_SERIALNUMBER_SENDER_OK;
  } else if (lora_sn_cabecera[0] == 'S' && lora_sn_cabecera[1] == 'M' && lora_sn_cabecera[2] == 'F' && lora_sn_cabecera[3] == 'C') {
    Serial.print (F("RECEIVER VALIDO SIN ASIGNAR: "));
    Serial.println (lora_sn_cabecera);
    return LORA_SERIALNUMBER_SENDER_VALIDO;
  }
  Serial.println (F("RECEIVER diferente de SN-CABECERA"));
  return LORA_SERIALNUMBER_SENDER_NULL;
}

void protocolo_PRINTLN_STRING (char *mensaje) {
  char aux_local[150];

  memset (aux_local, 0, sizeof(aux_local));
  for (int i = 0; i < sizeof(aux_local); i++) {
    aux_local[i] = (char) mensaje[i];
    if (mensaje[i] == 0) {
      break;
    }
    Serial.print ((char) aux_local[i]);
  }
  Serial.println ();
}


/*************************************************************************
 * *** FUNCIÓN: Sirve para descifrar el mensaje, sacando los campos de:
 * *** > LONGITUD. Este campo es utilizado internamente para saber cuantos caracteres de PAYLOAD existen en el mensaje.
 * *** > PAYLOAD. Este campo es el realmente importante.
 * *** > CRC. Este campo sirver para ver la integridad del mensaje.
 * ***********************************************************************/
int protocolo_DESCRIFRAR_Mensaje (String pakete, char *mensaje, int total_length, char *mensaje_recibido) {
  int longitud_LENGTH = 0, result_crc = CRC_NULL, result_payload = LORA_NULL;

  //Serial.print (F("MENSAJE A DESCRIFRAR: "));
  //Serial.println (pakete);
  longitud_LENGTH = protocolo_Extraer_LONGITUD (pakete);
  result_crc = protocolo_Comprobar_CRC (mensaje_recibido, longitud_LENGTH);
  if (result_crc != CRC_OK) return LORA_CRC_NULL;
  result_payload = protocolo_Extraer_PAYLOAD (pakete, mensaje, longitud_LENGTH, total_length);
  return result_payload;
}
int protocolo_Comprobar_CRC (char *mensaje_recibido, int length_payload) {
  char mensaje_crc[N_MAXIMO_CARACTERES_MENSAJE], dato_crc[5];
  int n, comienzo_CRC = 0, crc_aux;
  byte nib_LSB0, nib_LSB1, nib_LSB2, nib_LSB3;
  int temp[8];


  memset (mensaje_crc, 0, sizeof(mensaje_crc));
  //Serial.print (F("MENSAJE A PASAR POR CRC: "));
  n = array_to_ASCII (mensaje_recibido, (COMIENZO_PAYLOAD + length_payload), 0, mensaje_crc);
  //for (int i = 0; i < n; i++) {
  //  Serial.print (mensaje_crc[i]);
  //}
  //Serial.println ();
  uint16_t crc = CRC_ChecksumFletcher16(mensaje_crc, n);
  //Serial.print (F("CRC RECONSTRUIDO: "));
  //Serial.println (crc, HEX);
  comienzo_CRC = COMIENZO_PAYLOAD + length_payload;
  memset (dato_crc, 0, sizeof(dato_crc));
  n = array_to_ASCII (mensaje_recibido, 4, comienzo_CRC, dato_crc);
  nib_LSB0 = dato_crc[0];
  nib_LSB1 = dato_crc[1];
  nib_LSB2 = dato_crc[2];
  nib_LSB3 = dato_crc[3];
  temp[0] = (nib_LSB0 >= '0'&& nib_LSB0 <= '9') ? nib_LSB0 -= '0' : nib_LSB0 -= '7';
  temp[1] = (nib_LSB1 >= '0'&& nib_LSB1 <= '9') ? nib_LSB1 -= '0' : nib_LSB1 -= '7';
  temp[2] = (nib_LSB2 >= '0'&& nib_LSB2 <= '9') ? nib_LSB2 -= '0' : nib_LSB2 -= '7';
  temp[3] = (nib_LSB3 >= '0'&& nib_LSB3 <= '9') ? nib_LSB3 -= '0' : nib_LSB3 -= '7';
  crc_aux =  ((temp[0] << 0) & 0x000F) | 
             ((temp[1] << 4) & 0x00F0) | 
             ((temp[2] << 8) & 0x0F00) | 
             ((temp[3] << 12)& 0xF000);
  if (crc != crc_aux) return CRC_NULL;
  else                return CRC_OK;

}
int protocolo_Extraer_PAYLOAD (String pakete, char *mensaje, int lenght_payload, int total_length) {
  char *token;
  std::string aux_string;
  //int aux_int;

  array_to_ASCII (pakete, lenght_payload, COMIENZO_PAYLOAD, mensaje);
  //Serial.print (F("PAYLOAD EXTRAIDO: "));
  //Serial.println (mensaje);

  if (strstr (mensaje, "#VINCULAR*") != NULL) {
    return LORA_VINCULAR;
  } else if (strstr (mensaje, "#PING*") != NULL) {
    return LORA_PING;
  } else if (strstr (mensaje, "#ACK*") != NULL) {
    return LORA_ACK;
  } else {
    token = strtok (mensaje, "#");
    flag_nivel = false, flag_media = false, flag_desviacion = false;
    while (token != NULL) {
      float aux = protocolo_Extraer_INT (token);
      if (flag_nivel == true && flag_media == false && flag_desviacion == false) {
        nivel_int = aux;
        Serial.print (F("---- NIVEL = "));
        Serial.println (nivel_int);
      } else if (flag_nivel == true && flag_media == true && flag_desviacion == false) {
        media_int = aux;
        Serial.print (F("---- MEDIA = "));
        Serial.println (media_int);
      } else if (flag_nivel == true && flag_media == true && flag_desviacion == true) {
        d_t_int = aux;
        Serial.print (F("---- DESVIACIÓN = "));
        Serial.println (d_t_int);
      }
      token = strtok (NULL, "#");
    }
    if (flag_nivel == true && flag_media == true && flag_desviacion == true) {
      return LORA_PAYLOAD_OK;
    }
  }
  return LORA_NULL;
}
float protocolo_Extraer_INT (char *token) {
  char nivel_string[] = "NIVEL:", media_string[] = "MEDIA:", desv_tip_string[] = "D_T:";
  int length_nivel, length_media, length_desviacion;
  char *nivel, *media, *desv_tip;
  char nivel_str[10], media_str[10], desv_tip_str[10];

  //Serial.print (F("---- TOKEN: "));
  //Serial.println (token);
  length_nivel = strlen (nivel_string);
  nivel = strstr (token, nivel_string);
  if (nivel != NULL) {
    flag_nivel = true;
    memset (nivel_str, 0, sizeof(nivel_str));
    for (int i = length_nivel; i < strlen(nivel); i++) {
      nivel_str[i - length_nivel] = nivel[i];
    }
    return (atof(nivel_str));
  }
  length_media = strlen (media_string);
  media   = strstr (token, media_string);
  if (media != NULL) {
    flag_media = true;
    memset (media_str, 0, sizeof(media_str));
    for (int i = length_media; i < strlen(media); i++) {
      media_str[i - length_media] = media[i];
    }
    return (atof(media_str));
  }
  length_desviacion = strlen (desv_tip_string);
  desv_tip = strstr (token, desv_tip_string);
  if (desv_tip != NULL) {
    flag_desviacion = true;
    memset (desv_tip_str, 0, sizeof(desv_tip_str));
    for (int i = length_desviacion; i < strlen(desv_tip); i++) {
      desv_tip_str[i - length_desviacion] = desv_tip[i];
    }
    return (atof(desv_tip_str));
  }
  return 0;  
}
/*************************************************************************
 * *** FUNCIÓN: Sirve para extraer la LONGITUD del mensaje, dado que el número
 * *** llega en ASCII y hay que transformarlo a ENTERO.
 * ***********************************************************************/
int protocolo_Extraer_LONGITUD (String pakete) {
  int longitud_local;
  int temp[8];
  char aux_local[9];
  byte nib_LSB0, nib_LSB1, nib_LSB2, nib_LSB3;
  
  longitud_local = 0;
  memset (aux_local, 0, sizeof(aux_local));
  array_to_ASCII (pakete, 4, 20, aux_local);
  //Serial.print (F("LONGITUD EN CARACTERES: "));
  //Serial.println (aux_local);
  nib_LSB0 = aux_local[0];
  nib_LSB1 = aux_local[1];
  nib_LSB2 = aux_local[2];
  nib_LSB3 = aux_local[3];
  temp[0] = (nib_LSB0 >= '0'&& nib_LSB0 <= '9') ? nib_LSB0 -= '0' : nib_LSB0 -= '7';
  temp[1] = (nib_LSB1 >= '0'&& nib_LSB1 <= '9') ? nib_LSB1 -= '0' : nib_LSB1 -= '7';
  temp[2] = (nib_LSB2 >= '0'&& nib_LSB2 <= '9') ? nib_LSB2 -= '0' : nib_LSB2 -= '7';
  temp[3] = (nib_LSB3 >= '0'&& nib_LSB3 <= '9') ? nib_LSB3 -= '0' : nib_LSB3 -= '7';
  longitud_local =  ((temp[0] << 0) & 0x000F) | 
                    ((temp[1] << 4) & 0x00F0) | 
                    ((temp[2] << 8) & 0x0F00) | 
                    ((temp[3] << 12)& 0xF000);
  //Serial.print (F("LONGITUD DESCIFRADA: "));
  //Serial.println (longitud_local);
  return longitud_local;
}

void array_to_string(char array[], unsigned int len, char buffer[]) {
    for (unsigned int i = 0; i < len; i++) {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

int array_to_ASCII (String input, int longitud, int index, char *mensaje_out) {
  char temp[3], c;
  int n, aux_i;
  
  for (n = 0; n < longitud; n++) {
    byte nib_MSB = input[(2*(n+index))+0];
    byte nib_LSB = input[(2*(n+index))+1];
    temp[0] = (nib_MSB >= '0'&& nib_MSB <= '9') ? nib_MSB -= '0' : nib_MSB -= '7';
    temp[1] = (nib_LSB >= '0'&& nib_LSB <= '9') ? nib_LSB -= '0' : nib_LSB -= '7';
    aux_i = (temp[0] << 4) | temp[1];
    c = (char) (aux_i & 0x00FF);
    mensaje_out[n] = c;
  }
  return n;
  //Serial.print (F("LA CADENA ASCII: "));
  //Serial.println (mensaje_out);
}

int protocolo_Mensaje_PCBRELES (char *mensaje_, int long_mensaje, char *mensaje_local) {
  int indice = 0;
  char long_local[6];
  
  //SN SENDER: LECTURA E INTRODUCCIÓN EN EL MENSAJE.
  protocolo_leer_SNSENDER (sn_sender);
  for (int i1 = 0; i1 < DATOS_NUMERO_SERIE; i1++) {
    mensaje_local[indice++] = sn_sender[i1];
  }
  Serial.print (F("SN_SENDER: "));
  Serial.println (sn_sender);
  //SN RECEIVER: LECTURA E INTRODUCCIÓN EN EL MENSAJE.
  protocolo_leer_SNRECEIVER (sn_receptioner);
  if (strlen (sn_receptioner) == 0) {
    strcpy (sn_receptioner, sn_receiver);         //COPIA '0000000000' en el SN_RECEIVER del mensaje.
  }
  for (int i2 = 0; i2 < DATOS_NUMERO_SERIE; i2++) {
    mensaje_local[indice++] = sn_receptioner[i2];
  }
  Serial.print (F("SN_RECEIVER: "));
  Serial.println (sn_receptioner);
  //LONGITUD: CALCULO E INTRODUCCIÓN EN EL MENSAJE.
  protocolo_array_to_string (long_mensaje, long_local);
  //Serial.print (F("LONGITUD MENSAJE: "));
  //Serial.println (long_local);  
  for (int i2bis = 0; i2bis < 4; i2bis++) {
    mensaje_local[indice++] = long_local[i2bis];
  }
  //PAYLOAD: INTRODUCCIÓN EN EL MENSAJE.
  for (int i3 = 0; i3 < long_mensaje; i3++){
    mensaje_local[indice++] = mensaje_[i3];
  }
  Serial.print (F("PAYLOAD MENSAJE: "));
  Serial.println (mensaje_);  
  //CRC: CALCULO.
  //Serial.print (F("MENSAJE A PASAR POR CRC: "));
  //for (int i3bis = 0; i3bis <= indice; i3bis++) {
  //  Serial.print (mensaje_local[i3bis]);
  //}
  //Serial.println ();
  int crc = CRC_ChecksumFletcher16(mensaje_local, indice);
  protocolo_array_to_string (crc, long_local);
  //Serial.print (F("CRC MENSAJE: "));
  //Serial.println (long_local);  
  for (int i4 = 0; i4 < 4; i4++) {
    mensaje_local[indice++] = long_local[i4];
  }
  return indice;
}
void protocolo_array_to_string(int original, char *buffer) {
  
  byte nib1 = (original >> 0) & 0x0F;
  byte nib2 = (original >> 4) & 0x0F;
  byte nib3 = (original >> 8) & 0x0F;
  byte nib4 = (original >> 12)& 0x0F;
  buffer[0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
  buffer[1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  buffer[2] = nib3  < 0xA ? '0' + nib3  : 'A' + nib3  - 0xA;
  buffer[3] = nib4  < 0xA ? '0' + nib4  : 'A' + nib4   - 0xA;
  buffer[4] = '\0';
}

float protocolo_Test_NIVEL (void) {
  return nivel_int;
}
float protocolo_Test_MEDIA (void) {
  return media_int;
}
float protocolo_Test_DESVIACION (void) {
  return d_t_int;
}