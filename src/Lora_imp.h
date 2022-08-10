//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2021
//=============================================================================
#ifndef _LORA_IMP_H_
#define _LORA_IMP_H_

 #define LONGITUD_MINIMA_MENSAJE_LORA 10+10+8+1+8   //37 - He puesto al mensaje al menos 1 byte.
  enum comandos_LoRa {
    LORA_NULL = -1,
    LORA_SERIALNUMBER_RECEIVER_OK,
    LORA_SERIALNUMBER_RECEIVER_NULL,
    LORA_SERIALNUMBER_RECEIVER_VALIDO,
    LORA_SERIALNUMBER_SENDER_OK,
    LORA_SERIALNUMBER_SENDER_NULL,
    LORA_SERIALNUMBER_SENDER_VALIDO,
    LORA_DESCIFRAR_OK,
    LORA_DESCIFRAR_NULL,
    LORA_LONGITUD_OK,
    LORA_LONGITUD_NULL,
    LORA_CRC_OK,
    LORA_CRC_NULL,
    LORA_PAYLOAD_OK,
    LORA_PAYLOAD_NULL,
    LORA_VINCULAR,
    LORA_PING,
    LORA_ACK,
    LORA_TEST_NIVELES,
    LORA_VINCULACION_FORZOSA,
    LORA_EXTENDER_TIMEOUT
  };

    extern void Lora_imp_setup (void);
    extern void LoRa_Serial_Number (void);
    extern void Lora_imp_loop (unsigned long T_actual);
    extern void Lora_imp_SendPacket (char *packet, int total_length);
    extern int Lora_imp_CheckPacket (void);
    extern void Lora_imp_Test_SNRECEIVER (void);
    extern void lora_imp_GRABAR_SNRECEIVER (char *sn_cabecera);
    extern void Lora_imp_RESET_SNSENDER (void);

#endif  //_LORA_IMP_H_
