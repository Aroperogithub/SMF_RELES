#include <advancedSerial.h>
#include <string.h>
#include <stdlib.h>
#include <Wire.h>

#include "datos.h"
#include "hardware.h"
#include "Comanpar.h"
#include "Comanres.h"
#include "Bluetooth.h"
#include "version.h"

//#define DEBUG(a) Serial.println(a);

const COMANDO_RESPUESTA AR_Comand_RESP[] PROGMEM = {
      //MSG_RESP_NUMSERIEEPROM    //0
      {"+SNEEPROM: "},          
      //MSB_RESP_LLENADO_ON       //1
      {"+LLENADO_ON: "},
      //MSB_RESP_N_LLENADOS       //2
      {"+N_LLENADOS: "},
      //MSB_RESP_ALTURA           //3
      {"+ALTURA: "},
      //MSB_RESP_DESVIACION       //4
      {"+DESVIACION: "},
      //MSB_RESP_MEDIDA_MAXLASER  //5
      {"+MAXLASER: "},
      //MSB_RESP_PULSO_PERSIANA   //6
      {"+PPERSIANA: "},
      //MSB_RESP_NIVEL_SECO,      //7
      {"+SECO: "},
      //MSB_RESP_NIVEL_OPTIMO,    //8
      {"+OPTIMO: "},
      //MSB_RESP_NIVEL_SECO2,     //9
      {"+SECO2: "},
      //MSB_RESP_NIVEL_OPTIMO2,   //10
      {"+OPTIMO2: "},
      //MSB_RESP_TIME_PISCINA,    //11
      {"+TIMEPISCINA: "},
      //MSB_RESP_TIME_PISCINA2,   //12
      {"+TIMEPISCINA2: "},
      //MSB_RESP_TIEMPO_LLENADO,  //13
      {"+TIEMPOLLENADO: "},
      //DAT_TIME_LLENAR_1A        //14
      {"+TIME1A: "},
      //MSB_RESP_SUPERFICIE,      //15
      {"+SUPERFICIE: "},
      //MSB_RESP_LLENAR_ON        //16
      {"+LLENAR_ON: "},
      //MSB_RESP_VACIAR_ON        //17
      {"+VACIAR_ON: "}, 
      //MSB_RESP_PERSIANA_ON      //18
      {"+PERSIANA_ON: "},
      //MSB_RESP_DESVIACION2      //19
      {"+DESVIACION2: "},
      //MSB_RESP_RESERVED_L2      //20
      {"+RESERVED_L2: "},
      //MSG_RESP_SSID             //21
      {"+SSID: "}, 
      //MSG_RESP_PASSWRD          //22
      {"+PASSWRD: "}, 
      //MSB_RESP_USER_THI         //23
      {"+USER: "},
      //MSB_RESP_DEVICE_THI       //24  
      {"+DEVICE: "},
      //MSB_RESP_CREDENTIALS_THI  //25  
      {"+CREDENTIALS: "},
      //MSB_RESP_CODIGOPOSTAL     //26
      {"+CODIGOP: "},
      //MSG_RESP_SSID2            //27
      {"+SSID2: "}, 
      //MSG_RESP_PASSWRD2         //28
      {"+PASSWRD2: "},
      //MSG_RESP_INCONGRUENCIA    //29
      {"+INCONGRU: "},
      //MSG_RESP_FIRMWARE         //30
      {"+FIRMWARE: "} 
};

void Comanres_EnviarRespuestaComando (int comando, char *parametro) {
  char buffer_res[DATOS_COMANDO_PARAMETRO];
  const COMANDO_RESPUESTA *tmpptr_envio = AR_Comand_RESP;

  Serial.println ("OK");
  memset (buffer_res, 0, sizeof(buffer_res));
  strcpy_P (buffer_res, (*(tmpptr_envio + comando)).line);
  strcat (buffer_res, parametro);
  Serial.println (buffer_res);

}

void Comanres_MostrarEEPROM_CONFIG (int tipo_datos) {
  const COMANDO_RESPUESTA *tmpptr_envio = AR_Comand_RESP;
  char buffer_res[35], buffer_int[50], buffer_bluetooth[500], buffer_aux[50];
  long dato_long;
  int temp_int;
  //unsigned long valor;

  //mqprinc_ResetTIMEOUT_LASER ();
  Serial.println();
  if (tipo_datos == DATOS_CORRECTO_BLUETOOTH) {
    memset (buffer_bluetooth, 0, sizeof(buffer_bluetooth));
    sprintf (buffer_bluetooth, "+CONFIGU=");
    //strcat (buffer_bluetooth, "#LL-1*#VA-1*#PE-1*");
  }
  for (int n = MSB_RESP_LLENADO_ON; n <= MSG_RESP_FIRMWARE; n++) {
  //for (int n = MSB_RESP_N_LLENADOS; n <= MSB_RESP_SUPERFICIE; n++) {
    memset (buffer_int, 0, sizeof(buffer_int));
    memset (buffer_res, 0, sizeof(buffer_res));
    switch (n) {
      case MSB_RESP_LLENADO_ON:
        temp_int = datos_EEPROM_Read_INT (DAT_LLENADO_ON);
        dato_long = (long) temp_int;
        //sprintf(buffer_aux, "#RE-%d*", temp_int);
        //strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_N_LLENADOS:
        temp_int = datos_EEPROM_Read_INT (DAT_N_LLENADOS);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#NLL-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_ALTURA:
        temp_int = (int) datos_EEPROM_Read_BYTE (DAT_ALTURA);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#AP-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_DESVIACION:
        temp_int = (int) datos_EEPROM_Read_BYTE (DAT_DESVIACION);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#DT-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_DESVIACION2:
        temp_int = (long) datos_EEPROM_Read_BYTE (DAT_DESVIACION2);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#R1-%ld*", dato_long);
        strcat (buffer_bluetooth, buffer_aux);
        break;
      case MSB_RESP_MEDIDA_MAXLASER:
        temp_int = datos_EEPROM_Read_INT (DAT_MEDIDA_MAXLASER);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#MXL-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_PULSO_PERSIANA:
        temp_int = datos_EEPROM_Read_INT (DAT_PULSO_PERSIANA);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#MXU-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_NIVEL_SECO:
        temp_int = datos_EEPROM_Read_INT (DAT_NIVEL_SECO);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#NL-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_NIVEL_OPTIMO:
        temp_int = datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#NO-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_NIVEL_SECO2:
        temp_int = datos_EEPROM_Read_INT (DAT_NIVEL_SECO_2);
        dato_long = (long) temp_int;
        break;
      case MSB_RESP_NIVEL_OPTIMO2:
        temp_int = datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO_2);
        dato_long = (long) temp_int;
        break;
      case MSB_RESP_TIME_PISCINA:
        temp_int = datos_EEPROM_Read_INT (DAT_TIME_PISCINA);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#TL-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_TIME_PISCINA2:
        temp_int = datos_EEPROM_Read_INT (DAT_TIME_PISCINA_2);
        dato_long = (long) temp_int;
        break;
      case MSB_RESP_TIEMPO_LLENADO:
        temp_int = datos_EEPROM_Read_INT (DAT_TIEMPO_LLENADO);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#RE-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_TIME_LLENAR_1A:
        temp_int = datos_EEPROM_Read_INT (DAT_TIME_LLENAR_1A);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#T1A-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_SUPERFICIE:
        dato_long = (long) datos_EEPROM_Read_LONG (DAT_SUPERFICIE);
        sprintf(buffer_aux, "#SP-%ld*", dato_long);
        strcat (buffer_bluetooth, buffer_aux);
        break;
      case MSB_RESP_LLENAR_ON:
        temp_int = (int) datos_EEPROM_Read_BYTE (DAT_LLENAR_ON);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#LL-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_VACIAR_ON:
        temp_int = (int) datos_EEPROM_Read_BYTE (DAT_VACIAR_ON);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#VA-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSB_RESP_PERSIANA_ON:
        temp_int = (int) datos_EEPROM_Read_BYTE (DAT_PERSIANAS_ON);
        dato_long = (long) temp_int;
        sprintf(buffer_aux, "#PE-%d*", temp_int);
        strcat (buffer_bluetooth, buffer_aux);        
        break;
      case MSG_RESP_INCONGRUENCIA:
        temp_int = (int) datos_EEPROM_Read_BYTE (DAT_INCONGRUENCIA);
        dato_long = (long) temp_int;
        break;
      case MSB_RESP_RESERVED_L2:
        dato_long = (long) datos_EEPROM_Read_LONG (DAT_RESERVED_L2);
        sprintf(buffer_aux, "#R2-%ld*", dato_long);
        strcat (buffer_bluetooth, buffer_aux);
        break;
      case MSG_RESP_SSID:
        for (int n_datos = 0; n_datos <= (DAT_PASSWRD - DAT_SSID); n_datos++) {
          buffer_int[n_datos] = datos_EEPROM_Read_BYTE (DAT_SSID + n_datos);
          if(buffer_int[n_datos] == 0) break;
        }
        strcat (buffer_bluetooth, "#SS1-");    //SSID DE LA WiFi 1.
        strcat (buffer_bluetooth, buffer_int);
        strcat (buffer_bluetooth, "*");    //SSID DE LA WiFi 1.
        break;
      case MSG_RESP_PASSWRD:
        for (int n_datos = 0; n_datos <= (DAT_USER_THINGERIO - DAT_PASSWRD); n_datos++) {
          buffer_int[n_datos] = datos_EEPROM_Read_BYTE (DAT_PASSWRD + n_datos);
          if(buffer_int[n_datos] == 0) break;
        }
        strcat (buffer_bluetooth, "#PW1-");    //SSID DE LA WiFi 1.
        strcat (buffer_bluetooth, buffer_int);
        strcat (buffer_bluetooth, "*");    //SSID DE LA WiFi 1.
        break;
      case MSB_RESP_USER_THI:
        for (int n_datos = 0; n_datos <= (DAT_DEVICE_ID_THINGERIO - DAT_USER_THINGERIO); n_datos++) {
          buffer_int[n_datos] = datos_EEPROM_Read_BYTE (DAT_USER_THINGERIO + n_datos);
          if(buffer_int[n_datos] == 0) break;
        }
        strcat (buffer_bluetooth, "#UC-");    //SSID DE LA WiFi 1.
        strcat (buffer_bluetooth, buffer_int);
        strcat (buffer_bluetooth, "*");    //SSID DE LA WiFi 1.
        break;
      case MSB_RESP_DEVICE_THI:
        for (int n_datos = 0; n_datos <= (DAT_CREDENTIALS_THINGERIO - DAT_DEVICE_ID_THINGERIO); n_datos++) {
          buffer_int[n_datos] = datos_EEPROM_Read_BYTE (DAT_DEVICE_ID_THINGERIO + n_datos);
          if(buffer_int[n_datos] == 0) break;
        }
        strcat (buffer_bluetooth, "#DC-");    //SSID DE LA WiFi 1.
        strcat (buffer_bluetooth, buffer_int);
        strcat (buffer_bluetooth, "*");    //SSID DE LA WiFi 1.
        break;
      case MSB_RESP_CREDENTIALS_THI:
        for (int n_datos = 0; n_datos <= (DAT_CODIGO_POSTAL - DAT_CREDENTIALS_THINGERIO); n_datos++) {
          buffer_int[n_datos] = datos_EEPROM_Read_BYTE (DAT_CREDENTIALS_THINGERIO + n_datos);
          if(buffer_int[n_datos] == 0) break;
        }
        strcat (buffer_bluetooth, "#TC-");    //SSID DE LA WiFi 1.
        strcat (buffer_bluetooth, buffer_int);
        strcat (buffer_bluetooth, "*");    //SSID DE LA WiFi 1.
        break;
      case MSB_RESP_CODIGOPOSTAL:
        for (int n_datos = 0; n_datos <= (DAT_SSID2 - DAT_CODIGO_POSTAL); n_datos++) {
          buffer_int[n_datos] = datos_EEPROM_Read_BYTE (DAT_CODIGO_POSTAL + n_datos);
          if(buffer_int[n_datos] == 0) break;
        }
        strcat (buffer_bluetooth, "#CP-");    //SSID DE LA WiFi 1.
        strcat (buffer_bluetooth, buffer_int);
        strcat (buffer_bluetooth, "*");    //SSID DE LA WiFi 1.
        break;
      case MSG_RESP_SSID2:
        for (int n_datos = 0; n_datos <= (DAT_PASSWRD2 - DAT_SSID2); n_datos++) {
          buffer_int[n_datos] = datos_EEPROM_Read_BYTE (DAT_SSID2 + n_datos);
          if(buffer_int[n_datos] == 0) break;
        }
        strcat (buffer_bluetooth, "#SS2-");    //SSID DE LA WiFi 1.
        strcat (buffer_bluetooth, buffer_int);
        strcat (buffer_bluetooth, "*");    //SSID DE LA WiFi 1.
        break;
      case MSG_RESP_PASSWRD2:
        for (int n_datos = 0; n_datos <= (DAT_MAX_DATOS - DAT_PASSWRD2); n_datos++) {
          buffer_int[n_datos] = datos_EEPROM_Read_BYTE (DAT_PASSWRD2 + n_datos);
          if(buffer_int[n_datos] == 0) break;
        }
        strcat (buffer_bluetooth, "#PW2-");    //SSID DE LA WiFi 1.
        strcat (buffer_bluetooth, buffer_int);
        strcat (buffer_bluetooth, "*");    //SSID DE LA WiFi 1.
        break;
      case MSG_RESP_FIRMWARE:
        strcpy (buffer_int, VERSION_SHORT);
        strcat (buffer_bluetooth, "#FW-");
        strcat (buffer_bluetooth, VERSION_SHORT);
        strcat (buffer_bluetooth, "*");    //.
        break;
      default:
        break;
    }
    strcpy_P (buffer_res, (*(tmpptr_envio + n)).line);
    Serial.print (buffer_res);
    if ((n >= MSB_RESP_LLENADO_ON && n <= MSB_RESP_RESERVED_L2) || n == MSG_RESP_INCONGRUENCIA) {
      Serial.println (dato_long);
    } else {
      Serial.println (buffer_int);
    }
  }
  if (tipo_datos == DATOS_CORRECTO_BLUETOOTH) {
    Serial.println ();
    Serial.println (buffer_bluetooth);
    Bluetooth_Notification_CONFIGURACION (buffer_bluetooth);
  }
}
