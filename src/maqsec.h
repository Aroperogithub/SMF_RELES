//=============================================================================
//                       ARP
//                      (c) Copyright 2021
//=============================================================================
#ifndef _MAQUINASEC_H_
#define _MAQUINASEC_H_

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


typedef volatile struct _MAQFLAGSEC {
    unsigned bit7             : 1; // BIT 7 - UNUSED
    unsigned bit6             : 1; //     6 - UNUSED
    unsigned bit5             : 1; //     5 - UNUSED
    unsigned bit4             : 1; //     4 - UNUSED
    unsigned bit3             : 1; //     3 - UNUSED
    unsigned bit2             : 1; //     2 - UNUSED
    unsigned flag_reenviar    : 1; //     1 - UNUSED
    unsigned flag_1A_vez      : 1; //     0 - UNUSED
} __attribute__((packed)) MAQFLAGSEC;

enum estados_SEC {
    INICIAL,
    REPOSO,
    VINCULAR,
    LLENAR,
    ALARMAS,
    LLENAR_1A,
    VACIAR,
    ESTIRARPERSIANA,
    RECOGERPERSIANA,
    SALIDA_MQSEC
};

#define NO_CUMPLE       0
#define CUMPLE          1

enum  condiciones_sec {
  CS0,
  CS1,
  CS2,
  CS3,
  CS4,
  CS5,
  CS6,
  CS7,
  CS8,
  //CS9,
  //CS10,
  NMAX_CONDICIONESEC   
};
    extern void maqsec_SetFlag1A (void);
    extern void maqsec_ActivarCondicion (int N_Condicion);
    extern void maqsec_IniciacionVbles (void);
    extern void maqsec_SECUNDARIA (unsigned long T_actual, int comando_LoRa, int boton_tecla, bool status_Bluetooth, 
                                    int tecla_FCEstirar, int tecla_FCRecoger);
    extern int  maqest_CheckEstado (void);
    extern void maqsec_IniciacionVbles (void);


#endif // _MAQUINASEC_H_
