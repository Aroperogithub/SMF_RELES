//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2020
//=============================================================================
#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

  typedef struct _datosPuertoSer_Bluetooth {
    const char line[20]; //Total de datos reservados para el comando, hasta un total de 15 caracteres.
  } COMANDO_COLORES;

  #define B_CONNECTED    1
  #define B_DISCONNECTED 0

  #define BLUETOOTH_NOTIFICATION  5000
  #define BLUETOOTH_RESET         1000*20  //20s para la vigilancia de desconexiones.
  #define BLUETOOTH_NVECES_DESC   4
  
  enum comandos_bluetooth {
        MSB_CONFIGURACION = 1,    //1-CONFIGURACION.
        MSB_OTRO                  //LED_OFF
  };
  //extern void thingerio_setup (char *ssid, char *pass);
  extern void Bluetooth_setup (void);
  extern std::string Bluetooth_loop(unsigned long T_actual);
  extern bool Bluetooth_TestCONNECTED (unsigned long T_actual);
  extern void Bluetooth_Notification_CONFIGURACION (char *buffer_bluetooth);
  extern void Bluetooth_ActivarRESET_DESCONEXION (unsigned long T_actual);
  extern void Bluetooth_Notification (void);

#endif    //_BLUETOOTH_H_
  
