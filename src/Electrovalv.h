//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2020
//=============================================================================
#ifndef _ELECTROVALV_H_
#define _ELECTROVALV_H_

#include "hardware.h"

    #define BLINK_ALARMA_LLENADO        1000
    #define BLINK_ALARMA_INUNDACION     2000
    #define BLINK_ALARMA_LASER          500

    #define TIMEOUT_ON_REPOSO           100
    #define TIMEOUT_OFF_REPOSO          5000

    #define TIMEOUT_ON_LLENAR           2000
    #define TIMEOUT_OFF_LLENAR          2000
    
    #define TIMEOUT_ON_LLENAR1A         4000    
    #define TIMEOUT_OFF_LLENAR1A        4000
    
    #define TIMEOUT_ON_TELL             250     //TIEMPO EXCESIVO DE LLENADO
    #define TIMEOUT_OFF_TELL            250
    
    #define TIMEOUT_ON_INUNDACION       250     //INUNDACIÓN
    #define TIMEOUT_OFF_INUNDACION      2000
    
    #define TIMEOUT_ON_LASER_MAL        500     //LASER EN MAL ESTADO
    #define TIMEOUT_OFF_LASER_MAL       500
    
    #define TIMEOUT_ON_INCONGRUENCIA    1000    //INCONGRUENCIA DE DATOS. SIEMPRE ENCENDIDO.
    #define TIMEOUT_OFF_INCONGRUENCIA   0

    #define TIMEOUT_ON_ESTIRARPERSIANA  5000     //LASER EN MAL ESTADO
    #define TIMEOUT_OFF_ESTIRARPERSIANA 100
    
    #define TIMEOUT_ON_RECOGERPERSIANA  5000     //LASER EN MAL ESTADO
    #define TIMEOUT_OFF_RECOGERPERSIANA 500

    #define TIMEOUT_ON_CABEZAL          100     //LASER EN MAL ESTADO
    #define TIMEOUT_OFF_CABEZAL         100

    #define TIMEOUT_LED32_OFF2S         5000
    #define TIMEOUT_LED32_OFF200MS      200
    #define TIMEOUT_LED32_ON100MS       100
    #define ON_LED32                    EV_ON
    #define OFF_LED32                   EV_OFF

    typedef volatile struct _EVFLAGS {
        unsigned nulled2                : 1; //     7 - RESERVED.                                                            
        unsigned led_booton             : 1; //     6 - RESERVED.                                    
        unsigned ev_persiana_recoger    : 1; //     5 - RESERVED.
        unsigned ev_persiana_estirar    : 1; //     4 - RESERVED.
        unsigned ev_bomba_vac           : 1; //     3 - RESERVED.
        unsigned ev_vaciado             : 1; //     2 - Flag para definir el estado de la Electroválvula del vaciado.                 
        unsigned ev_llenado             : 1; //     1 - Flag para definir el estado de la Electroválvula del llenado.
        unsigned ev_bomba               : 1; //     0 - Flag para definir el estado de la Electroválvula para la BOMBA.
    } __attribute__((packed)) EVFLAGS;

    enum EV_BITS {
        EV_BITS_BOMBA,              //0
        EV_BITS_LLENADO,            //1
        EV_BITS_VACIADO,            //2
        EV_BITS_BOMBA_VAC,          //3
        EV_BITS_PERSINA_ESTIRAR,    //4
        EV_BITS_PERSINA_RECOGER     //5
    };
    extern void Electrovalv_EV_LLENADO (bool accion);
    extern void Electrovalv_EV_VACIADO (bool accion);
    extern void Electrovalv_EV_BOMBA_VAC (bool accion);
    extern void Electrovalv_EV_PERSIANA_ESTIRAR (bool accion);
    extern void Electrovalv_EV_PERSIANA_RECOGER (bool accion);
    extern bool Electrovalv_test (int evflag);
    extern void Electrovalv_EV_LED_BOTTON (bool accion);
    extern void Electrovalv_LED_BLINKONOFF_BOTTON (unsigned long T_actual, int timeout_ON, int timeout_OFF);
    extern void Electrovalv_Setup_LED_ONOFF_BOTTON (int Estado, unsigned long T_actual);
    extern void Electrovalv_LED_ONOFF_BOTTON (unsigned long T_actual);

#endif //_ELECTROVALV_H_
