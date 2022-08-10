//=============================================================================
//                       ARP
//                      (c) Copyright 2022
//=============================================================================
#ifndef _MQSEC_AUX_H_
#define _MQSEC_AUX_H_

  #define MOSTRAR_DATOS         0
  #define NO_MOSTRAR_DATOS      1
  //#define N_NIVELES             16
  #define DESVIACION_CRITICA    38
  #define N_OUTLIERS_CRITICOS   5
  #define TOLERANCIA_OUTLIER    20

  #define OUTLIER_OK             0
  #define OUTLIER_ALARMA        -1
  #define OUTLIER_RESET_CABEZAL -2

    extern void mqsec_aux_Reset_SNRECEIVER (void);
    extern int  mqsec_aux_Chech_SN_PCBRELES (void);
    extern void mqsed_aux_Enviar_COMANDO_LORA (int Comando_Enviar);
    extern void mqsec_LeerEEPROM (byte tipo_datos, byte mostrar);
    extern void mqsec_Gestionar_ALARMAS (unsigned long T_actual);
    //extern void mqsec_aux_Setup_OUTLIER (void);
    extern void mqsec_aux_RESET_OUTLIERS (void);
    extern int mqsec_aux_Comprobar_OUTLIER (void);
    extern int mqsec_aux_NIVEL_RELES (void);
    extern float mqsec_aux_MEDIA_RELES (void);
    extern float mqsec_aux_DESVIACION_RELES (void);
    extern int mqsec_aux_ASK_Contador_ouliers (void);
    extern int mqsec_aux_ASK_MEDIAErronea_ouliers (void);
    extern int mqsec_aux_ASK_MEDIABuena_ouliers (void);


#endif  //_MQSEC_AUX_H_
