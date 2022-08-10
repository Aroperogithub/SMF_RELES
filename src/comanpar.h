//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2018
//=============================================================================
#ifndef _COMANDPAR_H_
#define _COMANDPAR_H_

#define DATOSPUERTOSERIE_PARA   30    //Numero de datos del parámetro que acompaña al Comando.
#define DATOS_COMANDO_PARAMETRO 56    //Total de datos que tendría el buffer.

#define DATOS_REPOSO                0xFB
#define DATOS_NOT_FOUND             0xFC
#define DATOS_AR                    0xFD
#define DATOS_ERROR_LONG            0xFE
#define DATOS_CORRECTO              0xFF
#define DATOS_CORRECTO_BLUETOOTH    0x0F


  typedef struct _datosPuertoSer_Para {
    const char line[DATOS_COMANDO_PARAMETRO-DATOSPUERTOSERIE_PARA]; //Total de datos reservados para el comando, hasta un total de 15 caracteres.
  } COMANDO_PARAMETROS;
  enum PARA_msg_id {
      MSB_PARA_SNEEPROM,        //0
      MSB_PARA_LLENADO_ON,     //1    
      MSB_PARA_N_LLENADOS,      //2   
      MSB_PARA_ALTURA,          //3   
      MSB_PARA_DESVIACION,      //4   
      MSB_PARA_MEDIDA_MAXLASER, //5   
      MSB_PARA_PULSO_PERSIANA,   //6   
      MSB_PARA_NIVEL_SECO,      //7   
      MSB_PARA_NIVEL_OPTIMO,    //8   
      MSB_PARA_NIVEL_SECO2,      //9   
      MSB_PARA_NIVEL_OPTIMO2,    //10   
      MSB_PARA_TIME_PISCINA,    //11   
      MSB_PARA_TIME_PISCINA2,   //12   
      MSB_PARA_TIEMPO_LLENADO,  //13
      MSB_PARA_TIME_LLENAR_1A,  //14
      MSB_PARA_SUPERFICIE,      //15
      MSB_PARA_LLENAR_ON,       //16  Flag de activación de llenado por Bluetooth.
      MSB_PARA_VACIAR_ON,       //17  Flag de activación de vaciado por Bluetooth.
      MSB_PARA_PERSIANA_ON,     //18  Flag de activación de estirar persianas.
      MSB_PARA_DESVIACION2,     //19  NO USADO.
      MSB_PARA_RESERVED_L2,     //20
      MSB_PARA_SSID,            //21
      MSB_PARA_PASSWORD,        //22
      MSB_PARA_USER_THI,        //23
      MSB_PARA_DEVICE_THI,      //24
      MSB_PARA_CREDENTIALS_THI, //25
      MSB_PARA_CODIGOPOSTAL,    //26
      MSB_PARA_SSID2,           //27
      MSB_PARA_PASSWORD2,       //28
      MSB_PARA_CONFIGURACION,   //29
      MSB_PARA_RESET,           //30  
      MSB_PARA_READEEPROM,      //31
      MSB_PARA_MEDIR,           //32
      MSB_PARA_REPOSO,          //33
      MSB_PARA_LLENAR,          //34
      MSB_PARA_PARARLL,         //35
      MSB_PARA_VACIAR,          //36
      MSB_PARA_PARARVACIAR,     //37
      MSB_PARA_ESTIRARP,        //38
      MSB_PARA_RECOGERP,        //39
      MSB_PARA_VINCULACION_FORZADA,//40
      MSB_PARA_HELP             //41
  };

  typedef struct _respuestas_Comanpar {
    const char line[DATOS_COMANDO_PARAMETRO-DATOSPUERTOSERIE_PARA];
  } COMANPAR_RESP;
  enum COMANPAR_RESP_msg_id {
    MSG_COMANPAR_RESPUESTA_OUT,
    MSG_COMANPAR_OUT_MENOR,
    MSG_COMANPAR_OUT_OPTIMO,
    MSG_COMANPAR_RESPUESTA_DESV_TIPICA,
    MSG_COMANPAR_RESPUESTA_TIEMPO,
    MSG_COMANPAR_RESPUESTA_MAX
  };


  extern int Comanpar_ConsultaDatosPSerie (char *parametro, int *comando, unsigned long T_actual);
  extern int Comanpar_TratarComandoActiva (char *parametro, int comando, int tipo_datos);

#endif //_COMANDPAR_H_
