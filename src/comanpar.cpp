#include <advancedSerial.h>
#include <string.h>
#include <stdlib.h>
#include <Wire.h>

#include "datos.h"
#include "hardware.h"
#include "Bluetooth.h"
#include "comanres.h"
#include "comanpar.h"
#include "Lora_imp.h"

//#define DEBUG(a) Serial.println(a);

const COMANDO_PARAMETROS AR_Comand_PARA[] PROGMEM = {
      //MSB_PARA_SNEEPROM
      {"AR+SNEEPROM="},           //0
      //MSB_PARA_LLENADO_ON
      {"AR+LLENADO_ON="},        //1
      //MSB_PARA_N_LLENADOS
      {"AR+NLLENADOS="},        //2
      //MSB_PARA_ALTURAPOOL
      {"AR+ALTURA="},             //3
      //MSB_PARA_DESVIACION
      {"AR+DESVIACION="},         //4
      //MSB_PARA_MEDIDA_MAXLASER
      {"AR+MAXLASER="},           //5
      //MSB_PARA_PULSO_PERSIANA
      {"AR+PPERSIANA="},           //6
      //MSB_PARA_NIVEL_SECO,      
      {"AR+SECO="},               //7
      //MSB_PARA_NIVEL_OPTIMO,    
      {"AR+OPTIMO="},             //8 
      //MSB_PARA_NIVEL_SECO2,      
      {"AR+SECO2="},               //9
      //MSB_PARA_NIVEL_OPTIMO2,    
      {"AR+OPTIMO2="},             //10 
      //MSB_PARA_TIME_PISCINA,     
      {"AR+TIMEPISCINA="},         //11
      //MSB_PARA_TIME_PISCINA2,     
      {"AR+TIMEPISCINA2="},         //12
      //MSB_PARA_TIEMPO_LLENADO,     
      {"AR+TIEMPOLLENADO?"},      //13
      //MSB_PARA_TIME_LLENAR_1A,     
      {"AR+TIME1A="},             //14
      //MSB_PARA_SUPERFICIE,      
      {"AR+SUPERFICIE="},         //15
      //MSB_PARA_LLENAR_ON,       
      {"AR+LLENAR_ON="},          //16
      //MSB_PARA_VACIAR_ON,       
      {"AR+VACIAR_ON="},          //17
      //MSB_PARA_PERSIANA_ON,     
      {"AR+PERSIANA_ON="},        //18
      //MSB_PARA_DESVIACION2,     
      {"AR+DESVIACION2="},        //19
      //MSB_PARA_RESERVED_L2
      {"AR+RESERVED_L2="},        //20
      //MSB_PARA_SSID
      {"AR+SSID="},               //21
      //MSB_PARA_PASSWORD
      {"AR+PASSWRD="},            //22
      //MSB_PARA_USER_THI
      {"AR+USER="},               //23
      //MSB_PARA_DEVICE_THI
      {"AR+DEVICE="},             //24
      //MSB_PARA_CREDENTIALS_THI
      {"AR+CREDENTIALS="},        //25
      //MSB_PARA_CODIGOPOSTAL
      {"AR+CODIGOP="},            //26
      //MSB_PARA_SSID2
      {"AR+SSID2="},              //27
      //MSB_PARA_PASSWORD2
      {"AR+PASSWRD2="},           //28
      //MSB_PARA_CONFIGURACION
      {"AR+CONFIGU?"},            //29
      //MSB_PARA_RESET
      {"AR+RESETDAT"},            //30
      //MSB_PARA_READEEPROM
      {"AR+READEEPROM"},          //31
      //MSB_PARA_MEDIR
      {"AR+SHOWMEDIA"},           //32
      //MSB_PARA_REPOSO
      {"AR+REPOSO"},              //33
      //MSB_PARA_LLENAR
      {"AR+LLENAR"},              //34
      //MSB_PARA_PARARLL
      {"AR+PARARLL"},             //35
      //MSB_PARA_VACIAR
      {"AR+VACIAR"},              //36
      //MSB_PARA_PARARVACIAR
      {"AR+PARARVACIAR"},         //37
      //MSB_PARA_ESTIRARP
      {"AR+ESTIRARP"},            //38
      //MSB_PARA_RECOGERP
      {"AR+RECOGERP"},            //39
      //MSB_PARA_VINCULACION_FORZADA
      {"AR+VINCFORZ"},            //40
      //MSB_PARA_HELP
      {"AR+HELP"}                 //41
};

const COMANPAR_RESP comandpar_AR[] PROGMEM = {
      //MSG_COMANPAR_RESPUESTA_OUT
      {"Valor OUT:   "},
      //MSG_COMANPAR_OUT_MENOR
      {"DEBE SER < "},
      //MSG_COMANPAR_OUT_OPTIMO
      {"DEBE ESTAR 40 - 120"},
      //MSG_COMANPAR_RESPUESTA_DESV_TIPICA
      {"DEBE ESTAR 5 - 30"},
      //MSG_COMANPAR_RESPUESTA_TIEMPO
      {"DEBE ESTAR 0 - 15"},
      //MSG_COMANPAR_RESPUESTA_MAX
      {"MAXIMAS RESPUESTAS"}
};

int Comanpar_TratarComandoActiva (char *parametro, int comando, int tipo_datos) {
  const COMANDO_PARAMETROS *tmpptr = AR_Comand_PARA;
  char buffer_res[DATOS_COMANDO_PARAMETRO], buffer_int[DATOS_COMANDO_PARAMETRO];
  long dato_long;
  int longitud_parametro;

  unsigned long valor = 0;
  valor = (unsigned long) atol (parametro);         //La variable "valor" toma el parametro que se ha extraído del comando.
  longitud_parametro = strlen (parametro);
  if (longitud_parametro > DATOSPUERTOSERIE_PARA) {
    Serial.println ("-------PARÁMETRO DEMASIADO LARGO---------");
    return DATOS_ERROR_LONG;
  }
  memset (buffer_res, 0, sizeof(buffer_res));
  switch (comando) {
    case MSB_PARA_LLENADO_ON:
      datos_EEPROM_Write_INT ((int) valor, DAT_LLENADO_ON);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_N_LLENADOS:
      datos_EEPROM_Write_INT ((int) valor, DAT_N_LLENADOS);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_ALTURA:
      datos_EEPROM_Write_BYTE ((byte) valor, DAT_ALTURA);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_DESVIACION:
      datos_EEPROM_Write_BYTE ((byte) valor, DAT_DESVIACION);
      datos_EEPROM_Write_BYTE ((byte) valor, DAT_DESVIACION2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_MEDIDA_MAXLASER:
      datos_EEPROM_Write_INT ((int) valor, DAT_MEDIDA_MAXLASER);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_PULSO_PERSIANA:
      datos_EEPROM_Write_INT ((int) valor, DAT_PULSO_PERSIANA);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_NIVEL_SECO:
      dato_long = (long) datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO);
      if (dato_long >= valor) {
        Serial.println (F("ERROR: SECO NO PUEDE SER MENOR QUE ÓPTIMO"));
        return DATOS_ERROR_LONG;
      } else if (valor > EEMPROM_NIVEL_SECO_MAXIMO) {
        Serial.print (F("ERROR: SECO NO PUEDE SER MAYOR DE "));
        Serial.println (EEMPROM_NIVEL_SECO_MAXIMO);
        return DATOS_ERROR_LONG;
      }
      datos_EEPROM_Write_INT ((int) valor, DAT_NIVEL_SECO);
      datos_EEPROM_Write_INT ((int) valor, DAT_NIVEL_SECO_2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_NIVEL_OPTIMO:
      dato_long = (long) datos_EEPROM_Read_INT (DAT_NIVEL_SECO);
      if (dato_long < valor) {
        Serial.println (F("ERROR: ÓPTIMO NO PUEDE SER MAYOR QUE SECO"));
        return DATOS_ERROR_LONG;
      } else if (valor < EEMPROM_NIVEL_OPTIMO_MINIMO) {
        Serial.print (F("ERROR: ÓPTIMO NO PUEDE SER MENOR DE "));
        Serial.println (EEMPROM_NIVEL_OPTIMO_MINIMO);
        return DATOS_ERROR_LONG;
      }
      datos_EEPROM_Write_INT ((int) valor, DAT_NIVEL_OPTIMO);
      datos_EEPROM_Write_INT ((int) valor, DAT_NIVEL_OPTIMO_2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
/*************************************************/
/**********  SOLO PARA PRUEBAS *******************/
/**********      INICIO        *******************/
    /*case MSB_PARA_NIVEL_SECO2:
      datos_EEPROM_Write_INT ((int) valor, DAT_NIVEL_SECO_2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_NIVEL_OPTIMO2:
      datos_EEPROM_Write_INT ((int) valor, DAT_NIVEL_OPTIMO_2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_TIME_PISCINA2:
      datos_EEPROM_Write_INT ((int) valor, DAT_TIME_PISCINA_2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_DESVIACION2:
      datos_EEPROM_Write_BYTE ((byte) valor, DAT_DESVIACION2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;*/
/*************************************************/
/**********  SOLO PARA PRUEBAS *******************/
/**********      FIN           *******************/
    case MSB_PARA_TIME_PISCINA:
      datos_EEPROM_Write_INT ((int) valor, DAT_TIME_PISCINA);
      datos_EEPROM_Write_INT ((int) valor, DAT_TIME_PISCINA_2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_TIEMPO_LLENADO:
      memset (buffer_int, 0, sizeof(buffer_int));               //Se resetea el buffer.
      dato_long = (int) datos_EEPROM_Read_INT (DAT_TIEMPO_LLENADO);
      //sprintf(buffer_int, "%d", dato_long);
      printf(buffer_int, "%l", dato_long);
      Comanres_EnviarRespuestaComando (comando, buffer_int);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_TIME_LLENAR_1A:
      datos_EEPROM_Write_INT ((int) valor, DAT_TIME_LLENAR_1A);
      datos_EEPROM_Write_INT ((int) valor, DAT_TIME_LLENAR_1A_2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_SUPERFICIE:
      datos_EEPROM_Write_LONG (valor, DAT_SUPERFICIE);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_LLENAR_ON:
      datos_EEPROM_Write_BYTE ((byte) valor, DAT_LLENAR_ON);
      datos_EEPROM_Write_BYTE ((byte) valor, DAT_LLENAR_ON_2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_VACIAR_ON:
      datos_EEPROM_Write_BYTE ((byte) valor, DAT_VACIAR_ON);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_PERSIANA_ON:
      datos_EEPROM_Write_BYTE ((byte) valor, DAT_PERSIANAS_ON);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_RESERVED_L2:
      datos_EEPROM_Write_LONG (valor, DAT_RESERVED_L2);
      Comanres_EnviarRespuestaComando (comando, parametro);     //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_SSID:
      datos_Reset_Parcial_MemoriaEEPROM (DAT_SSID, (DAT_SSID + longitud_parametro + 1));       //Machaca la longitud del parámetro más 1 para el caracter 0.
      for (int n_datos = 0; n_datos < strlen (parametro); n_datos++) {
        datos_EEPROM_Write_BYTE (parametro[n_datos], DAT_SSID + n_datos);
      }
      Comanres_EnviarRespuestaComando (MSG_RESP_SSID, parametro);                             //MUESTRA los datos grabados con su comando correspondiente.
      //Wifipool_ControlarInit ();                                                                //INTENTO VOLVER A CONECTAR CON LA WiFi.
      break;
    case MSB_PARA_PASSWORD:
      //Serial.print ("MSB_RECEP_PASSWORD: ");
      datos_Reset_Parcial_MemoriaEEPROM (DAT_PASSWRD, (DAT_PASSWRD + longitud_parametro + 1));
      for (int n_datos = 0; n_datos < strlen (parametro); n_datos++) {
        datos_EEPROM_Write_BYTE (parametro[n_datos], DAT_PASSWRD + n_datos);
      }
      Comanres_EnviarRespuestaComando (MSG_RESP_PASSWRD, parametro);                            //MUESTRA los datos grabados con su comando correspondiente.
      //Wifipool_ControlarInit ();                                                                //INTENTO VOLVER A CONECTAR CON LA WiFi.
      break;
    case MSB_PARA_USER_THI:
      datos_Reset_Parcial_MemoriaEEPROM (DAT_USER_THINGERIO, (DAT_USER_THINGERIO + longitud_parametro + 1));
      for (int n_datos = 0; n_datos < strlen (parametro); n_datos++) {
        datos_EEPROM_Write_BYTE (parametro[n_datos], DAT_USER_THINGERIO + n_datos);
      }
      Comanres_EnviarRespuestaComando (comando, parametro);                             //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_DEVICE_THI:
      datos_Reset_Parcial_MemoriaEEPROM (DAT_DEVICE_ID_THINGERIO, (DAT_DEVICE_ID_THINGERIO + longitud_parametro + 1));
      for (int n_datos = 0; n_datos < strlen (parametro); n_datos++) {
        datos_EEPROM_Write_BYTE (parametro[n_datos], DAT_DEVICE_ID_THINGERIO + n_datos);
      }
      Comanres_EnviarRespuestaComando (comando, parametro);                             //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_CREDENTIALS_THI:
      datos_Reset_Parcial_MemoriaEEPROM (DAT_CREDENTIALS_THINGERIO, (DAT_CREDENTIALS_THINGERIO + longitud_parametro + 1));
      for (int n_datos = 0; n_datos < strlen (parametro); n_datos++) {
        datos_EEPROM_Write_BYTE (parametro[n_datos], DAT_CREDENTIALS_THINGERIO + n_datos);
      }
      Comanres_EnviarRespuestaComando (comando, parametro);                             //MUESTRA los datos grabados con su comando correspondiente.
      break;
    case MSB_PARA_CODIGOPOSTAL:
      datos_Reset_Parcial_MemoriaEEPROM (DAT_CODIGO_POSTAL, (DAT_CODIGO_POSTAL + longitud_parametro + 1));
      for (int n_datos = 0; n_datos < strlen (parametro); n_datos++) {
        datos_EEPROM_Write_BYTE (parametro[n_datos], DAT_CODIGO_POSTAL + n_datos);
      }
      Comanres_EnviarRespuestaComando (comando, parametro);                             //MUESTRA los datos grabados con su comando correspondiente.
      break;

    case MSB_PARA_SSID2:
      datos_Reset_Parcial_MemoriaEEPROM (DAT_SSID2, (DAT_SSID2 + longitud_parametro + 1));       //Machaca la longitud del parámetro más 1 para el caracter 0.
      for (int n_datos = 0; n_datos < strlen (parametro); n_datos++) {
        datos_EEPROM_Write_BYTE (parametro[n_datos], DAT_SSID2 + n_datos);
      }
      Comanres_EnviarRespuestaComando (MSG_RESP_SSID2, parametro);                             //MUESTRA los datos grabados con su comando correspondiente.
      //Wifipool_ControlarInit ();                                                                //INTENTO VOLVER A CONECTAR CON LA WiFi.
      break;
    case MSB_PARA_PASSWORD2:
      //Serial.print ("MSB_RECEP_PASSWORD: ");
      datos_Reset_Parcial_MemoriaEEPROM (DAT_PASSWRD2, (DAT_PASSWRD2 + longitud_parametro + 1));
      for (int n_datos = 0; n_datos < strlen (parametro); n_datos++) {
        datos_EEPROM_Write_BYTE (parametro[n_datos], DAT_PASSWRD2 + n_datos);
        //if (parametro[n_datos] == 0) break;
      }
      Comanres_EnviarRespuestaComando (MSG_RESP_PASSWRD2, parametro);                            //MUESTRA los datos grabados con su comando correspondiente.
      //Wifipool_ControlarInit ();                                                                //INTENTO VOLVER A CONECTAR CON LA WiFi.
      break;



    case MSB_PARA_CONFIGURACION:
      Comanres_MostrarEEPROM_CONFIG (tipo_datos);
      Serial.println (F("MOSTRADA LA INFORMACIÓN DE LA EEPROM."));
      break;
    case MSB_PARA_RESET:
      //datos_Reset_MemoriaEEPROM ();
      datos_Parametros_FabricaEEPROM ();                                              //PARAMETRIZA LA MEMORIA EEPROM.
      LoRa_Serial_Number ();
      Comanres_MostrarEEPROM_CONFIG (tipo_datos);
      Serial.println (F("MOSTRADA LA INFORMACIÓN DE LA EEPROM."));
      break;
    case MSB_PARA_READEEPROM:
      datos_EEPROM_Read_Memoria (DAT_SN_SENDER, DAT_MAX_DATOS);
      break;
    case MSB_PARA_MEDIR:
      //mqprin_ShowMediaUSB ();                           //Muestra una sola vez la Distancia y la Media.
      break;
    case MSB_PARA_REPOSO:
      return DATOS_REPOSO;
      break;
    case MSB_PARA_LLENAR:
      Serial.println (F("COMANDO LLENAR"));
      break;
    case MSB_PARA_PARARLL:
      Serial.println (F("COMANDO PARAR DE LLENAR"));
      break;
    case MSB_PARA_VACIAR:
      Serial.println (F("COMANDO VACIAR"));
      break;
    case MSB_PARA_PARARVACIAR:
      Serial.println (F("COMANDO PARAR DE VACIAR"));
      break;
    case MSB_PARA_ESTIRARP:
      Serial.println (F("COMANDO ESTIRAR PERSIANA"));
      break;
    case MSB_PARA_RECOGERP:
      Serial.println (F("COMANDO RECOGER PERSIANA"));
      break;
    case MSB_PARA_VINCULACION_FORZADA:
      Serial.println (F("COMANDO VINCULACIÓN FORZADA"));
      break;
    case MSB_PARA_HELP:
      for (byte n = MSB_PARA_LLENADO_ON; n <= MSB_PARA_HELP; n++) {
        memset (buffer_res, 0, sizeof(buffer_res));
        strcpy_P (buffer_res, (*(tmpptr + n)).line);
        Serial.println (buffer_res);
      }
      break;
    default:
      break;
  }
  return DATOS_CORRECTO;
}

/*************************************************************************************************************************
****  FUNCIÓN: int Comanpar_ConsultaDatosPSerie (char *parametro)                                                     ****
****  PARAMETROS: char *parametro. Puntero a una cadena de caracteres que será tratada más tarde para saber qué       ****
****  parametro se le ha pasado al comando.                                                                           ****
****  DEVUELVE: Devuelve un int que define como ha ido la función.                                                    ****
****      - DATOS_NOT_FOUND         0xFC                                                                              ****
****      - DATOS_AR                0xFD                                                                              ****
****      - DATOS_ERROR_LONG        0xFE                                                                              ****
****      - DATOS_CORRECTO          0xFF                                                                              ****
*************************************************************************************************************************/
int Comanpar_ConsultaDatosPSerie (char *parametro, int *comando, unsigned long T_actual) {
    const COMANDO_PARAMETROS *tmpptr = AR_Comand_PARA;
    char buffer_int[DATOS_COMANDO_PARAMETRO], buffer_res[DATOSPUERTOSERIE_PARA];
    std::string data_str ="";
    String data = "";
    
  memset (buffer_int, 0, sizeof(buffer_int));       //Reset del Buffer intermedio que se usa para la extracción del comando.
  memset (parametro, 0, DATOSPUERTOSERIE_PARA);     //Reset del buffer para tomar el número que acompaña al comando.
  comando[0] = 0xFF;                                //Ningun comando.

  data_str = Bluetooth_loop (T_actual);
  if (data_str.length () > 0) {
    if (data_str.length () >= DATOS_COMANDO_PARAMETRO) { //Esto es para cuando se recibe una cadena que no está dentro de los márgenes establecidos como normales (nº caracteres).
      Serial.print (F("ERROR LONGITUD: "));
      Serial.println (data_str.length ());
      return DATOS_ERROR_LONG;                    //Se ha recibido algo, aunque no es conocido y por tanto es un error de LONGITUD.
    }
    if (data_str == "AR") {
      Serial.println (F("OK"));
      return DATOS_AR;                            //Se ha recibido el comando AR y el número sería el 0.
    }
    for (int i = 0; i < data_str.length (); i++) {
      buffer_int[i] = data_str[i];                    //Se almacena el comando recibido en un buffer intermedio sacado del String para poder compararlo a posteriori.
    }
    Serial.print (F("------> COMANDO RECIBIDO: "));
    Serial.println (buffer_int);

    for (byte n = MSB_PARA_LLENADO_ON; n <= MSB_PARA_HELP; n++) {    //Se verifica el comando desde LIMITE INSTANTANEO hasta el comando HELP.
      memset (buffer_res, 0, sizeof(buffer_res));
      strcpy_P (buffer_res, (*(tmpptr + n)).line);
      if (strstr (buffer_int, buffer_res)) {      //Si el comando ha sido localizado vamos a extraerle el parámetro.
        comando[0] = n;
        Serial.print (F("COMANDO IDENTIFICADO: "));
        Serial.println (buffer_res);
        int n_datos = strlen(buffer_res);
        Serial.print (F("N_CARACTERES DEL COMANDO: "));
        Serial.println (n_datos);
        strcpy (parametro, &buffer_int[n_datos]);
        Serial.print (F("PARAMETRO: "));
        Serial.println (parametro);
        //AQUÍ SERÍA DONDE HABRÍA QUE ENVIAR EL OK DE RESPUESTA BLUETOOTH. ESPERAR A LAS PRUEBAS EN REAL.
        return DATOS_CORRECTO_BLUETOOTH;
      }
    }
  }
  if (Serial.available()) {                         //Hay datos en el puerto serie.
      //String data = Serial.readStringUntil('\n');
      data = Serial.readStringUntil('\n');
      Serial.print (F("COMANDO RECIBIDO: "));
      Serial.println (data);                      //Muestra lo almacenado en "data".
      //Serial.print (F("N_DATOS RECIBIDOS: "));
      //Serial.println (data.length());
      //data[data.length ()] = 0;                 //Le pone un cero para tener una cadena completa.
      //Serial.print (F("COMANDO RECIBIDO 2: "));
      //Serial.println (data);                      //Muestra lo almacenado en "data".
      if (data.length () >= DATOS_COMANDO_PARAMETRO) { //Esto es para cuando se recibe una cadena que no está dentro de los márgenes establecidos como normales (nº caracteres).
        Serial.print (F("ERROR LONG: "));
        Serial.println (data.length ());
        return DATOS_ERROR_LONG;                    //Se ha recibido algo, aunque no es conocido y por tanto es un error de LONGITUD.
      }
      if (data == "AR") {
        Serial.println (F("OK"));
        return DATOS_AR;                            //Se ha recibido el comando AR y el número sería el 0.
      }
      for (int i = 0; i < data.length (); i++) {
        buffer_int[i] = data[i];                    //Se almacena el comando recibido en un buffer intermedio sacado del String para poder compararlo a posteriori.
      }
      for (byte n = MSB_PARA_LLENADO_ON; n <= MSB_PARA_HELP; n++) {    //Se verifica el comando desde LIMITE INSTANTANEO hasta el comando HELP.
        memset (buffer_res, 0, sizeof(buffer_res));
        strcpy_P (buffer_res, (*(tmpptr + n)).line);
        if (strstr (buffer_int, buffer_res)) {      //Si el comando ha sido localizado vamos a extraerle el parámetro.
          comando[0] = n;
          Serial.print (F("COMANDO IDENTIFICADO: "));
          Serial.println (buffer_res);
          int n_datos = strlen(buffer_res);
          Serial.print (F("N_CARACTERES DEL COMANDO: "));
          Serial.println (n_datos);
          strcpy (parametro, &buffer_int[n_datos]);
          Serial.print (F("PARAMETRO: "));
          Serial.println (parametro);
          return DATOS_CORRECTO;
        }
      }
      Serial.println (F("ERROR: NOT FOUND"));
      return DATOS_NOT_FOUND;
  }
   return DATOS_NOT_FOUND;
}
