//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2018
//=============================================================================
#ifndef _COMANDRES_H_
#define _COMANDRES_H_

#define DATOSPUERTOSERIE_RESP   30



  typedef struct _datosPuertoSer_Resp {
    const char line[DATOSPUERTOSERIE_RESP];
  } COMANDO_RESPUESTA;
  enum RESP_msg_id {
      MSG_RESP_NUMSERIEEPROM,   //0   NUMERO DE SERIE DEL DISPOSITIVO. NO USADO.
      MSB_RESP_LLENADO_ON,      //1   PARA SABER SI SE ESTABA LLENANDO DESPUÉS DE UN RESET O BOMBA ON.
      MSB_RESP_N_LLENADOS,      //2   NÚMERO DE LLENADOS DESDE QUE SE CONECTÓ EL DISPOSITIVO.
      MSB_RESP_ALTURA,          //3   ALTURA DE LA PISCINA
      MSB_RESP_DESVIACION,      //4   CONFIGURACIÓN DEL VALOR DE LA DESVIACIÓN.
      MSB_RESP_MEDIDA_MAXLASER, //5   NO USADO.
      MSB_RESP_PULSO_PERSIANA,  //6   NO USADO.
      MSB_RESP_NIVEL_SECO,      //7   CONFIGURACIÓN DEL NIVEL SECO O DE LLENADO.
      MSB_RESP_NIVEL_OPTIMO,    //8   CONFIGURACIÓN DEL NIVEL OPTIMO O DE PARAR DE LLENAR.
      MSB_RESP_NIVEL_SECO2,     //9   CONFIGURACIÓN DEL NIVEL SECO O DE LLENADO.
      MSB_RESP_NIVEL_OPTIMO2,   //10   CONFIGURACIÓN DEL NIVEL OPTIMO O DE PARAR DE LLENAR.
      MSB_RESP_TIME_PISCINA,    //11   Tiempo de llenado programado por el usuario para determinar si hay alarma de tiempo excesivo de llenado.
      MSB_RESP_TIME_PISCINA2,   //12   Tiempo de llenado programado por el usuario para determinar si hay alarma de tiempo excesivo de llenado.
      MSB_RESP_TIEMPO_LLENADO,  //13  Tiempo de llenado del último llenado.
      MSB_RESP_TIME_LLENAR_1A,  //14  Tiempo de llenado para LLENAR COMPLETAMENTE la piscina.
      MSB_RESP_SUPERFICIE,      //15  SUPERFICIE de la piscina.
      MSB_RESP_LLENAR_ON,       //16  Flag de activación de llenado por Bluetooth.
      MSB_RESP_VACIAR_ON,       //17  Flag de activación de vaciado por Bluetooth.
      MSB_RESP_PERSIANA_ON,     //18  Flag de activación de estirar persianas.
      MSB_RESP_DESVIACION2,     //19  NO USADO.
      MSB_RESP_RESERVED_L2,     //20  NO USADO.
      MSG_RESP_SSID,            //21  CONFIGURACIÓN DE LA SSI DE LA WIFI.
      MSG_RESP_PASSWRD,         //22  CONFIGURACIÓN DE LA PASSWRD DE LA WIFI.
      MSB_RESP_USER_THI,        //23  NO USADO.
      MSB_RESP_DEVICE_THI,      //24  NO USADO.
      MSB_RESP_CREDENTIALS_THI, //25  NO USADO.
      MSB_RESP_CODIGOPOSTAL,    //26  CODIGO POSTAL.
      MSG_RESP_SSID2,           //27  CONFIGURACIÓN DE LA SSI DE UNA 2ª WIFI.
      MSG_RESP_PASSWRD2,        //28  CONFIGURACIÓN DE LA PASSWRD DE UNA 2ª WIFI.
      MSG_RESP_INCONGRUENCIA,   //29  MUESTRA LA INFORMACIÓN DEL Nº DE INCONGRUENCIAS LOCALIZADAS.
      MSG_RESP_FIRMWARE         //30  VERSIÓN FIRMWARE DEL DISPOSITIVO.
  };

  extern void Comanres_EnviarRespuestaComando (int comando, char *parametro);
  extern void Comanres_MostrarEEPROM_CONFIG (int tipo_datos);

#endif //_COMANDRES_H_
