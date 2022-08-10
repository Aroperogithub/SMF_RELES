//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2017
//=============================================================================
#ifndef _HARDWARE_H_
#define _HARDWARE_H_

/********** TIPOLOGÍA DE PCB ***************/
#define SMART_FILL_v5
#define SMARTFILL_PREMIUM

  #define N_NIVELES                     16
  //DATOS
  #define DATOS_NUMERO_SERIE            10
  #define TIMEOUT_REPETICION_COMANDOS   5000
  #define COMANDOS_REPETIDOS            3
  //ANALOGINPUT
  #define TIMEOUT_LECTURA_NIVEL         5000
  //LORA_IMP
  #define TIMEOUT_SEND_LORA             10000
  #define TIMEOUT_GESTIONAR_NIVELES     15000
  #define TIMEOUT_ACK                   1000
  #define N_MAXIMO_CARACTERES_MENSAJE   300
  #define N_SERIALNUMBER                20
  #define COMIENZO_PAYLOAD              24    //20 SN_SENDER + 20 SN_RECEIVER + 8 LONGITUD

//PINES DE ENTRADA
  const int analogInPin =         A0;
  #define BOTON                   4
  #define NSS                     17
  #define RESET                   16
  #define DIO0                    15
  #define FC_ESTIRAR              33
  #define FC_RECOGER              32
  //PINES DE SALIDA
  #define EV_LLENADO              26  //EV5
  #define EV_VACIADO              27  //EV4
  #define EV_PERSIANA_ESTIRAR     14  //EV3
  #define EV_BOMBA_VAC            13  //EV1
  #define EV_PERSIANA_RECOGER     12  //EV2


  #define LED_BOTON               25

  #define led_sender              2

  #define NIVEL_BAJO  false
  #define NIVEL_ALTO  true
  //#define POSITIVE_LOGIC
  #define NEGATIVE_LOGIC

  #ifdef POSITIVE_LOGIC
      #define EV_ON   true
      #define EV_OFF  false
  #endif
  #ifdef NEGATIVE_LOGIC
      #define EV_ON   false
      #define EV_OFF  true
  #endif


#endif //_HARDWARE_H_
