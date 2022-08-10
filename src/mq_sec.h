//=============================================================================
//                       ARP
//                      (c) Copyright 2021
//=============================================================================
#ifndef _MQSEC_H_
#define _MQSEC_H_

/*******************************************************************************
*  Fichero: mqprinc.h       Módulo que integra la estructura de la máquina de  *
*                           estados.                                           *
********************************************************************************
*  Descripcion:  En este módulo se definen las funciones de la máquina de estados*
*   utilizadas después por las diferentes máquinas.                *
********************************************************************************
*  Creacion del modulo: 19 de Sep de 2017                                      *
*  Ultima modificacion:                                                        *
*  Nombre del creador del modulo:  Ángel Ropero                                *
*  Software Arduino                                                              *
*  Links con otros ficheros.                                                   *
*  Empresa. IMPOREXO                                                           *
*******************************************************************************/
#define N_MAXIMO 500
#define FONDO 0xFFFF

#define TIMEOUT_ESTADOS           2000
#define TIMEOUT_SEND_ACK          1000
#define TIMEOUT_PING              5*1000*60
#define TIMEOUT_REPETIR_COMANDOS  5125
#define TIMEOUT_RESET_CABEZAL     2*1000*60
#define TIMEOUT_RESET_CABEZAL_MAX 20*1000*60
#define TIMEOUT_RESET_OUTLIERS    60*1000*60
#define NMAX_REPETIR_COMANDOS     5
#define GAP_NIVEL_SECO_QUITAR_ALARMA  10

//int reg_alarmas;

typedef volatile struct _MQSEC_FLAGS {
    unsigned flag_reset_cabezal   : 1; // BIT 7 - UNUSED
    unsigned flag_PING            : 1; //     6 - UNUSED
    unsigned flag_apertura_1A     : 1; //     5 - UNUSED
    unsigned flag_ACK             : 1; //     4 - UNUSED
    unsigned flag_init_recepcion  : 1; //     3 - UNUSED
    unsigned flag_reset_outliers  : 1; //     2 - UNUSED
    unsigned flag_comando         : 1; //     1 - UNUSED
    unsigned flag_boton           : 1; //     0 - UNUSED
} __attribute__((packed)) MQSEC_FLAGS;

  #define TIMEOUT_ALARMAS_ON        3000
  #define TIMEOUT_INCONGRUENCIA     1000*60*60  //CADA 60 minutos (1 hora).
  #define MEDIDA_MARGEN_LASER       50
  #define ALARMA_SIN_ALARMAS        0x0000
  #define ALARMA_INUNDACION         0x0001
  #define ALARMA_MEDIDORES_FAIL     0x0002
  #define ALARMA_CABEZAL            0x0004
  #define ALARMA_NIVELES            0x0008
  #define ALARMA_LLENADO            0x0010
  #define ALARMA_EVAPORACION        0x0020
  #define ALARMA_INCONGRUENCIA      0x0040
  #define ALARMA_ULTRASONIDO        0x0080
  #define ALARMA_RESERVED4          0x0100
//------------------------------------------------
  #define ALARMA_INUNDACION_C1      0xFFFE
  #define ALARMA_MEDIDORES_FAIL_C1  0xFFFD
  #define ALARMA_CABEZAL_C1         0xFFFB
  #define ALARMA_NIVELES_C1         0xFFF7
  #define ALARMA_LLENADO_C1         0xFFEF
  #define ALARMA_EVAPORACION_C1     0xFFDF
  #define ALARMA_INCONGRUENCIA_C1   0xFFBF
  #define ALARMA_ULTRASONIDO_C1     0xFF7F
  #define ALARMA_RESERVED4_C1       0xFEFF

/*------------------- ESTADOS DE FUNCIONAMIENTO ---------------------*/


  extern short mqsec_EstINICIAL           (unsigned long T_actual);
  extern short mqsec_EstREPOSO            (unsigned long T_actual, int comando_LoRa, int boton_tecla);
  extern short mqsec_EstVINCULAR          (unsigned long T_actual, int comando_LoRa, int boton_tecla);
  extern short mqprin_EstLLENAR           (unsigned long T_actual, int comando_LoRa, int boton_tecla);
  extern short mqprin_EstALARMAS          (unsigned long T_actual, int comando_LoRa, int boton_tecla);
  extern short mqprin_EstLLENAR_1A        (unsigned long T_actual, int comando_LoRa, int boton_tecla);
  extern short mqprin_EstVACIAR           (unsigned long T_actual, int comando_LoRa, int boton_tecla);
  extern short mqprin_EstESTIRARPERSIANA  (unsigned long T_actual, int comando_LoRa, int boton_tecla, int tecla_FCEstirar);
  extern short mqprin_EstRECOGERPERSIANA  (unsigned long T_actual, int comando_LoRa, int boton_tecla, int tecla_FCRecoger);

  extern int mq_sec_ESTADO (void);
  extern int mq_sec_ALARMAS_Bluetooth (void);
  extern void mqsec_aux_Reset_flag_outliers (void);
  extern void mqsec_aux_Set_flag_outliers (void);


#endif //_MQSEC_H_
