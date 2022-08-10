//=============================================================================
//                       ARP
//                      (c) Copyright 2021
//=============================================================================
#ifndef _PROTOCOLO_H_
#define _PROTOCOLO_H_

  #define CRC_NULL  -1
  #define CRC_OK    0

  extern void   protocolo_Init (void);
  extern void   array_to_string(char array[], unsigned int len, char *buffer);
  extern int    array_to_ASCII (String input, int longitud, int index, char *mensaje_out);
  extern int    protocolo_Mensaje_PCBRELES (char *mensaje_, int long_mensaje, char *mensaje_local);
  extern int    protocolo_componer_Mensaje (char *mensaje_, int long_mensaje, char *mensaje_local);
  extern int    protocolo_Test_SN_RECEIVER (char *lora_sn_receiver);
  extern int    protocolo_Test_SN_SENDER (char *lora_sn_sender);
  extern void   protocolo_PRINTLN_STRING (char *mensaje);
  extern int    protocolo_DESCRIFRAR_Mensaje (String pakete, char *mensaje, int total_length, char *mensaje_recibido);
  extern void   protocolo_leer_SNSENDER (char *lora_sn_cabecera);
  extern void   protocolo_leer_SNRECEIVER (char *lora_sn_reles);

  extern float  protocolo_Test_NIVEL (void);
  extern float  protocolo_Test_MEDIA (void);
  extern float  protocolo_Test_DESVIACION (void);

#endif // _PROTOCOLO_H_
