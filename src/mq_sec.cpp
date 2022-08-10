/*******************************************************************************
   File   : mqprin.c  Module that integers the functions of machine states
********************************************************************************
   Description:In this module is called the functions of machine states.
********************************************************************************
   Creation of module: 25 of Apr of 2006
   Last modification:
   Name of creator of module: Ángel Ropero
   Software P2100
   Links with another files. mqprin.c.
   Company: HYC
*******************************************************************************/
/*** Includes ***/
#include <string.h>
#include <stdlib.h>
#include <advancedSerial.h>

#include "mq_sec.h"
#include "maqsec.h"
#include "datos.h"
#include "hardware.h"
#include "protocolo.h"
#include "Lora_imp.h"
#include "teclas.h"
#include "comanpar.h"
#include "Electrovalv.h"
#include "mqsec_aux.h"
#include "Bluetooth.h"
#include "AnalogInput.h"

#define VACIO     0

//-------------------- GLOBAL VARIABLES AL MODULO ------------------------------
extern MAQFLAGSEC maqsec_bits;
_MQSEC_FLAGS maqflags;
DATEEPROM maqDatosEEPROM;

unsigned long T_anterior_REPOSO, T_anterior_VINCULAR, T_anterior_PING, T_anterior_ACK, T_anterior_INICIAL;
unsigned long timeout_persiana, timeout_incongruencia, timeout_medida, timeout_llenar, timeout_IA_llenado = 0;
unsigned long timeout_ACK = 0, timeout_apertura_ev = 0, timeout_PING = 0, timeout_REPETIR_COMANDO = 0;
unsigned long timeout_RESET_CABEZAL = 0, timeout_incremental_reset_CABEZAL = TIMEOUT_RESET_CABEZAL, timeout_reset_vbles_outliers;
int Last_status = INICIAL, contador_comandos = 0, Last_command = LORA_NULL;
int reg_alarmas, contador_REPETIR_Comandos = 0;
int media_ultrasonidos, desviacion_ultrasonidos;
int media_llenado_AUX;

//-------------------- GLOBAL FUNCTIONS ----------------------------------------
static void mqsec_Iniciar_Flags (void);
static int mqprinc_GestionarDistancias (int estado_medidas);
//static void mqprinc_Gestionar_ALARMAS (unsigned long T_actual);
//static void mq_sec_Notification_Bluetooth (void);
//------------------------------------------------------------------------------
//------------------- STATES OF FUNCIONALITIES ---------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FUNCTION: short mqsec_EstINICIAL_SEC (void)
// DESCRIPTION: Function that executes the functionalities of INICIAL state.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqsec_EstINICIAL (unsigned long T_actual) {          //Este estado le dejamos ahí por si hiciera falta
  
  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    mqsec_Iniciar_Flags ();
    T_anterior_INICIAL = T_actual;
    Serial.println (F("ESTADO INICIAL"));
    Last_status = INICIAL;
    Electrovalv_EV_LED_BOTTON (NIVEL_BAJO);
    Electrovalv_EV_BOMBA_VAC (EV_ON);
    maqflags.flag_init_recepcion = true;
    mqsec_LeerEEPROM (DATOS_TODOS, MOSTRAR_DATOS); //LEE LOS PARÁMETROS DE LA EEPROM. 

  }
  //------------------------------------ PROCCESS --------------------------------
  int result = mqsec_aux_Chech_SN_PCBRELES ();
  if (result == 0)  maqsec_ActivarCondicion (CS1);   //Enviar al estado de VINCULAR.
  else              maqsec_ActivarCondicion (CS0);   //Enviar al estado de REPOSO.
  return 0;
}

//------------------------------------------------------------------------------
// FUNCTION: short mqsec_EstESPERA (void)
// DESCRIPTION: Function that executes the functionalities of REPOSO state.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqsec_EstREPOSO (unsigned long T_actual, int comando_LoRa, int boton_tecla) {
  char parametro[DATOSPUERTOSERIE_PARA], comando_respuesta[DATOSPUERTOSERIE_PARA];
  int  comando_EEPROM[2], accion_estado;
  //byte valor_EEPROM;

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    T_anterior_REPOSO = timeout_incongruencia = T_actual;
    Serial.println (F("ESTADO REPOSO: "));
    Last_status = REPOSO;
    maqflags.flag_boton = maqflags.flag_ACK = maqflags.flag_PING = maqsec_bits.flag_reenviar = false;
    timeout_medida = timeout_PING = T_actual;
    Electrovalv_EV_LLENADO (NIVEL_BAJO);
    Electrovalv_EV_VACIADO (NIVEL_BAJO);
    reg_alarmas = ALARMA_SIN_ALARMAS;
    contador_REPETIR_Comandos = 0;
    Electrovalv_Setup_LED_ONOFF_BOTTON (Last_status, T_actual);
    if (mqsec_aux_ASK_Contador_ouliers () != 0 || mqsec_aux_ASK_MEDIAErronea_ouliers () != 0 || mqsec_aux_ASK_MEDIABuena_ouliers () != 0) {
      timeout_reset_vbles_outliers = T_actual;
      Serial.println (F("SE ACTIVA TIMEOUT RESET VARIABLES DE OUTLIERS"));
      Serial.println ();
    }
    accion_estado = REPOSO;
  }
  //------------------------------------ PROCCESS --------------------------------
  Electrovalv_LED_ONOFF_BOTTON (T_actual);
/*****************************************************************/
/******* SE COMPRUEBA INCONGRUENCIA DE DATOS *********************/
/*****************************************************************/
  if ((T_actual - timeout_incongruencia) > TIMEOUT_INCONGRUENCIA) {
    timeout_incongruencia = T_actual;
    Serial.println (F("//// TEST INCONGRUENCIA DE DATOS EEPROM ////"));
    if (datos_Comprobar_Incongruencia_de_datos () == DATOS_EEPROM_ALARMA) {  //Se ha producido una Alarma de INCONGRUENCIA de datos.
      reg_alarmas |= ALARMA_INCONGRUENCIA;
    }
  }
  if ((T_actual - timeout_reset_vbles_outliers) > TIMEOUT_RESET_OUTLIERS && maqflags.flag_reset_outliers == false) {
    timeout_reset_vbles_outliers = T_actual;
    mqsec_aux_RESET_OUTLIERS ();
  }
/*****************************************************************************************************/
// AL ESTADO DE ALARMAS.
/*****************************************************************************************************/
  if (reg_alarmas != ALARMA_SIN_ALARMAS) {
    maqsec_ActivarCondicion (CS4);          //Lleva la máquina de estados al estado de ALARMAS.
  }
  //ESTO ES PARA LLEVAR AL EQUIPO AL ESTADO DE LLENAR.
  if (boton_tecla == BOTON_ON) {
    maqsec_ActivarCondicion (CS3);            //Al estado de LLENAR. Como si fuera automático.
    return 0;
  }
  //SE QUIERE INICIAR UNA VINCULACIÓN NUEVA por cambio de la PCB-CABECERA, POR LO QUE HAY QUE BORRAR EL SN_RECEIVER ANTERIOR.
  if (boton_tecla == BOTON_CAMBIO) {
    mqsec_aux_Reset_SNRECEIVER ();      //Se resetea el SN_RECEIVER en la EEPROM.
    maqsec_ActivarCondicion (CS1);      //Enviar al estado de VINCULAR.
  }
/*************************************************************************************************
**** TRATAMIENTO DEL ENVÍO DE LOS DIFERENTES COMANDOS LORA: LORA_TEST_NIVELES, LORA_PING, LORA_PING, 
**** LORA_EXTENDER_TIMEOUT *****
**************************************************************************************************/
//ESTO ES PARA EL CASO EN EL QUE SE HA INICIADO EL EQUIPO Y NO HA LLEGADO TODAVÍA NINGÚN PAYLOAD_OK
  if (maqflags.flag_init_recepcion == true && (T_actual - T_anterior_REPOSO) > TIMEOUT_SEND_LORA) {
    T_anterior_REPOSO = timeout_REPETIR_COMANDO = T_actual;
    Serial.println (F("///// SOLICITUD DE NIVELES POR INICIALIZACIÓN DE EQUIPO /////"));
    maqsec_bits.flag_reenviar = true;
    Last_command = LORA_TEST_NIVELES;
    mqsed_aux_Enviar_COMANDO_LORA (Last_command);
  }
//TRATAMIENTO DEL ENVÍO DE UN PING PARA SABER SI RESPONDE CON ACK PARA SABER SI ESTÁ VIVO
  if ((T_actual - timeout_PING) > TIMEOUT_PING) {
    timeout_PING = timeout_REPETIR_COMANDO = T_actual;
    Serial.println (F("////**** ENVIO DE UN PING ****/////"));
    maqsec_bits.flag_reenviar = true;
    Last_command = LORA_PING;
    mqsed_aux_Enviar_COMANDO_LORA (Last_command);
  }
  //ESTE CASO SERÍA EL QUE HA PASADO DE NUEVO EL TIMEOUT Y TODAVÍA NO SE HA RECIBIDO EL ACK
  //if ((T_actual - timeout_REPETIR_COMANDO) > TIMEOUT_REPETIR_COMANDOS && maqflags.flag_PING == true) {
  if ((T_actual - timeout_REPETIR_COMANDO) > TIMEOUT_REPETIR_COMANDOS && maqsec_bits.flag_reenviar == true) {
    contador_REPETIR_Comandos++;
    Serial.println ();
    Serial.print (F("****---- REPETIR EL COMANDO ANTERIOR: "));
    Serial.print (Last_command);
    Serial.print (F("\tCONTADOR_REPETIR: "));
    Serial.print (contador_REPETIR_Comandos);
    Serial.print (F(" ----****"));
    Serial.println ();
    timeout_REPETIR_COMANDO = T_actual;
    if (contador_REPETIR_Comandos >= NMAX_REPETIR_COMANDOS) {
      reg_alarmas |= ALARMA_CABEZAL;
      Serial.println (F("////**** ALARMA CABEZAL ****////"));
    }
    mqsed_aux_Enviar_COMANDO_LORA (Last_command);
  }
/************************************************************************************************
 * *** AQUÍ SE RECIBEN COMANDOS LORA Y SE REACCIONA.
 * ********************************************************************************************/
  if (comando_LoRa == LORA_ACK) {
    timeout_REPETIR_COMANDO = timeout_PING = T_actual;
    maqsec_bits.flag_reenviar = false;
    contador_REPETIR_Comandos = 0;
    Last_command = LORA_NULL;
  }
//ESTE COMANDO SE OCASIONA CUANDO SE RECIBE UN COMANDO LORA CUYO NÚMERO DE SERIE NO ES EL CORRECTO, POR LO QUE PODRÍA ESTAR COINCIDIENDO EN EL TIEMPO,
// ESTO OBLIBARÍA A RETRASARLO AL MENOS 1,5 SEGUNDOS.
  //ESTE COMANDO SE PUEDE RECIBIR COMO CONSECUENCIA DE LA RECEPCIÓN DE UN COMANDO #VINCULAR* EN LA PCB-CABECERA POR LA INSTALACIÓN
  // DE UN SMART FILL CERCANO (VECINO)
  if (comando_LoRa == LORA_PING || comando_LoRa == LORA_PAYLOAD_OK) {
    maqflags.flag_ACK = true;
    maqflags.flag_PING = false;
    contador_REPETIR_Comandos = 0;
    timeout_ACK = T_actual;
    timeout_PING = T_actual;
    timeout_REPETIR_COMANDO = T_actual;
    if (comando_LoRa == LORA_PAYLOAD_OK) {
      maqsec_bits.flag_reenviar = false;
      maqflags.flag_init_recepcion = false;
      //Last_command = LORA_NULL;
      Last_command = comando_LoRa;
    }
  }
  if (maqflags.flag_ACK == true && (T_actual - timeout_ACK) > TIMEOUT_ACK) {
    maqflags.flag_ACK = false;
    mqsed_aux_Enviar_COMANDO_LORA (LORA_ACK);
//ESTA SITUACIÓN SE DA PORQUE HA LLEGADO UN COMANDO DE PAYLOAD_OK Y ES NECESARIO GESTIONAR LAS DISTANCIAS DE INMEDIATO.
    if (Last_command == LORA_PAYLOAD_OK) {
      Last_command = LORA_NULL;
      accion_estado = mqprinc_GestionarDistancias (REPOSO);
      if (accion_estado == LLENAR) {
        maqsec_ActivarCondicion (CS3);          //Lleva la máquina de estados al estado de LLENADO.  
      } else if (accion_estado == ALARMAS) {
        maqsec_ActivarCondicion (CS4);          //Lleva la máquina de estados al estado de ALARMAS.  
      }
      Bluetooth_Notification ();
    }
  }
/*****************************************************************************************************/
//MEDICIÓN DE NIVELES Y GESTIÓN DEL LLENADO EN EL ESTADO DE REPOSO. CADA TIMEOUT_LASER segundos.
/*****************************************************************************************************/
  if ((T_actual - timeout_medida) > TIMEOUT_GESTIONAR_NIVELES) {
    timeout_medida = T_actual;              //Se resetea el timeout.
    accion_estado = mqprinc_GestionarDistancias (REPOSO);
    if (accion_estado == LLENAR) {
      maqsec_ActivarCondicion (CS3);          //Lleva la máquina de estados al estado de LLENADO.  
    } else if (accion_estado == ALARMAS) {
      maqsec_ActivarCondicion (CS4);          //Lleva la máquina de estados al estado de ALARMAS.  
    }
    Bluetooth_Notification ();
  }
/*****************************************************************************************************/
//TRATAMIENTO DE DATOS EN LA EEPROM.
/*****************************************************************************************************/
  mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, NO_MOSTRAR_DATOS);
  int datos = Comanpar_ConsultaDatosPSerie (parametro, comando_EEPROM,T_actual);
  switch (comando_EEPROM[0]) {
    case MSB_PARA_LLENAR:
      Serial.println (F("LLENAR - BLUETOOTH"));
      maqsec_ActivarCondicion (CS3);                      //Al estado de LLENAR.
      break;
    case MSB_PARA_PARARLL:
      Serial.println (F("PARAR DE LLENAR - BLUETOOTH"));
      break;
    case MSB_PARA_VACIAR:
      Serial.println (F("VACIAR - BLUETOOTH"));
      maqsec_ActivarCondicion (CS6);                      //Al estado de LLENAR.
      break;
    case MSB_PARA_PARARVACIAR:
      Serial.println (F("PARAR VACIAR - BLUETOOTH"));
      break;
    case MSB_PARA_ESTIRARP:
      //medida_laser_REPOSO = vl53l0x_Mostrar_Media ();
      //desviacion_REPOSO = vl53l0x_Mostrar_Desviacion ();
      if (media_ultrasonidos > maqDatosEEPROM.nivel_optimo && media_ultrasonidos < maqDatosEEPROM.nivel_seco && 
          desviacion_ultrasonidos <= maqDatosEEPROM.desviacion) {  
        Serial.println (F("ESTIRAR PERSINA - BLUETOOTH"));
        strcpy (comando_respuesta, "OK");
        maqsec_ActivarCondicion (CS7);                      //Al estado de LLENAR.
      } else {
        Serial.println (F("ESTIRAR PERSINA - NIVEL FUERA DE RANGO"));
        strcpy (comando_respuesta, "NOK");
      }
      Bluetooth_Notification_CONFIGURACION (comando_respuesta);
      break;
    case MSB_PARA_RECOGERP:
      //medida_laser_REPOSO = vl53l0x_Mostrar_Media ();
      //desviacion_REPOSO = vl53l0x_Mostrar_Desviacion ();
      if (media_ultrasonidos > maqDatosEEPROM.nivel_optimo && media_ultrasonidos < maqDatosEEPROM.nivel_seco && 
          desviacion_ultrasonidos <= maqDatosEEPROM.desviacion) {  
        Serial.println (F("RECOGER PERSINA - BLUETOOTH"));
        strcpy (comando_respuesta, "OK");
        maqsec_ActivarCondicion (CS8);                      //Al estado de LLENAR.
      } else {
        Serial.println (F("RECOGER PERSINA - NIVEL FUERA DE RANGO"));
        strcpy (comando_respuesta, "NOK");
      }
      Bluetooth_Notification_CONFIGURACION (comando_respuesta);
      break;
    case MSB_PARA_VINCULACION_FORZADA:
      Serial.println (F("////**** VINCULACIÓN FORZOSA ****/////"));
      timeout_REPETIR_COMANDO = T_actual;
      maqsec_bits.flag_reenviar = true;
      Last_command = LORA_VINCULACION_FORZOSA;
      mqsed_aux_Enviar_COMANDO_LORA (Last_command);
      break;
    default:
      break;
  }
  if (datos == DATOS_CORRECTO || datos == DATOS_CORRECTO_BLUETOOTH) {
    int command = Comanpar_TratarComandoActiva (parametro, comando_EEPROM[0], datos);
    if (command != DATOS_CORRECTO) {
      Serial.print (F("RESULTADO DEL COMANDO: "));
      Serial.println (command);
    }
  }
  return 0;
}



//------------------------------------------------------------------------------
// FUNCTION: short mqsec_EstCALLADO (void)
// DESCRIPTION: Function that executes the functionalities of REPOSO state.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqsec_EstVINCULAR (unsigned long T_actual, int comando_LoRa, int boton_tecla) {
  char parametro[DATOSPUERTOSERIE_PARA];//, comando_respuesta[DATOSPUERTOSERIE_PARA];
  int  comando_EEPROM[2];

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    T_anterior_VINCULAR = T_actual;
    Serial.println (F("ESTADO VINCULAR"));
    Last_status = VINCULAR;
    Electrovalv_Setup_LED_ONOFF_BOTTON (Last_status, T_actual);
  }
  //------------------------------------ PROCCESS --------------------------------
  Electrovalv_LED_ONOFF_BOTTON (T_actual);
  if ((T_actual - T_anterior_VINCULAR) >= TIMEOUT_REPETICION_COMANDOS) {
    T_anterior_VINCULAR = T_actual;            //Reset el TIMEOUT.
    mqsed_aux_Enviar_COMANDO_LORA (LORA_VINCULAR);
  }
  if (comando_LoRa == LORA_ACK) {
    maqsec_ActivarCondicion (CS0);      //Enviar al Estado de REPOSO porque se ha obtenido un ACK válido.
  }
  //ESTO ES SOLO PARA PRUEBAS, PARA BORRAR LA EEPROM Y NO TENER QUE CARGAR MÁS APLICACIONES
  // QUITAR EN LA VERSIÓN DEFINITIVA.
  if (boton_tecla == BOTON_CAMBIO) {
    mqsec_aux_Reset_SNRECEIVER ();
    T_anterior_VINCULAR = T_actual;
  }
  mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, NO_MOSTRAR_DATOS);
  int datos = Comanpar_ConsultaDatosPSerie (parametro, comando_EEPROM,T_actual);
  switch (comando_EEPROM[0]) {
    case MSB_PARA_VACIAR:
      Serial.println (F("VACIAR - BLUETOOTH"));
      maqsec_ActivarCondicion (CS6);                      //Al estado de VACIAR.
      break;
    case MSB_PARA_VINCULACION_FORZADA:
      mqsed_aux_Enviar_COMANDO_LORA (LORA_VINCULACION_FORZOSA);
      break;
    default:
      break;
  }
  if (datos == DATOS_CORRECTO || datos == DATOS_CORRECTO_BLUETOOTH) {
    int command = Comanpar_TratarComandoActiva (parametro, comando_EEPROM[0], datos);
    if (command != DATOS_CORRECTO) {
      Serial.print (F("RESULTADO DEL COMANDO: "));
      Serial.println (command);
    }
  }  
  return 0;
}

//------------------------------------------------------------------------------
// FUNCTION: short mqprin_EstLLENAR (unsigned long T_actual, int comando_LoRa, int boton_tecla)
// DESCRIPTION: Function that executes the functionalities of LLENAR state.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqprin_EstLLENAR           (unsigned long T_actual, int comando_LoRa, int boton_tecla) {
  char parametro[DATOSPUERTOSERIE_PARA];
  int  comando_EEPROM[2];
  unsigned long tiempo_llenado = 0;

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    Serial.println (F("ESTADO LLENAR"));
    Electrovalv_EV_LLENADO (NIVEL_ALTO);
    mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, MOSTRAR_DATOS); //LEE LOS PARÁMETROS DE LA EEPROM. 
    maqDatosEEPROM.N_llenados += 1;
    datos_EEPROM_Write_INT ((int) maqDatosEEPROM.N_llenados, DAT_N_LLENADOS);
    Last_status = LLENAR;
    timeout_llenar = timeout_IA_llenado = T_actual;
    Electrovalv_Setup_LED_ONOFF_BOTTON (Last_status, T_actual);
    media_llenado_AUX = (int) mqsec_aux_MEDIA_RELES ();
  }
  //------------------------------------ PROCCESS --------------------------------
  Electrovalv_LED_ONOFF_BOTTON (T_actual);
  //ESTE COMANDO SE PUEDE RECIBIR COMO CONSECUENCIA DE LA RECEPCIÓN DE UN COMANDO #VINCULAR* EN LA PCB-CABECERA POR LA INSTALACIÓN
  // DE UN SMART FILL CERCANO (VECINO)
  if (comando_LoRa == LORA_PING || comando_LoRa == LORA_PAYLOAD_OK) {
    maqflags.flag_ACK = true;
    timeout_ACK = T_actual;
    if (comando_LoRa == LORA_PAYLOAD_OK) maqflags.flag_init_recepcion = false;
  }
  if (maqflags.flag_ACK == true && (T_actual - timeout_ACK) > TIMEOUT_ACK) {
    maqflags.flag_ACK = false;
    mqsed_aux_Enviar_COMANDO_LORA (LORA_ACK);
  }
/*****************************************************************/
/******* SE COMPRUEBA INCONGRUENCIA DE DATOS *********************/
/*****************************************************************/
  if (datos_Comprobar_Incongruencia_de_datos () == DATOS_EEPROM_ALARMA) {  //Se ha producido una Alarma de INCONGRUENCIA de datos.
    reg_alarmas |= ALARMA_INCONGRUENCIA;
  }
/*****************************************************************/
/******* SE COMPRUEBAN TODAS LAS ALARMAS *************************/
/*****************************************************************/
  if (reg_alarmas != ALARMA_SIN_ALARMAS) {
    maqsec_ActivarCondicion (CS4);          //Lleva la máquina de estados al estado de ALARMAS.
  }
/**************************************************************************/
/******* SE PASA AL ESTADO DE REPOSO POR PULSAR BOTÓN *********************/
/**************************************************************************/
  if (boton_tecla == BOTON_ON) {
    tiempo_llenado = (T_actual - timeout_IA_llenado) / 60000;       //TIEMPO EN MINUTOS QUE HA TARDADO EN LLENAR CADA VEZ.
    Serial.print (F("--- EL TIEMPO DE LLENADO EN ESTA OCASIÓN HA SIDO DE: "));
    Serial.print (tiempo_llenado);
    Serial.println (F(" minutos ----"));
    datos_EEPROM_Write_INT ((int) tiempo_llenado, DAT_TIEMPO_LLENADO);
    Electrovalv_EV_LLENADO (NIVEL_BAJO);
    maqsec_ActivarCondicion (CS0);      //Vuelve al estado de REPOSO.
    return 0;
  }
  //SE QUIERE INICIAR UN LLENADO COMPLETO DESDE LA PISCINA VACÍA.
  if (boton_tecla == BOTON_CAMBIO) {
    maqsec_ActivarCondicion (CS5);      //Enviar al estado de LLENAR_1A.
    return 0;
  }

/**************************************************************************/
/******* SE COMPRUEBA EL TIEMPO MÁXIMO DE LLENADO *************************/
/**************************************************************************/
  maqDatosEEPROM.time_piscina = datos_EEPROM_Read_INT (DAT_TIME_PISCINA);    //Lo vuelvo a leer por si hubiera cambiado.
  if ((T_actual - timeout_llenar) > (maqDatosEEPROM.time_piscina * 60000)) {
    Serial.print (F("//// SE HA SUPERADO EL TIEMPO DE LLENADO: "));
    //Serial.print (maqDatosEEPROM.time_piscina * 60000/1000);
    Serial.print (maqDatosEEPROM.time_piscina);
    Serial.println (F(" minutos."));
    reg_alarmas |= ALARMA_LLENADO;   //ESTO OCURRE CUANDO DESPUÉS DE ESTAR X MINUTOS LLENANDO LA MEDIA NO HA DISMINUIDO.
    maqsec_ActivarCondicion (CS4);    //Lleva la máquina de estados al estado de ALARMAS.  
    return 0;
  }
/*****************************************************************************************************/
//MEDICIÓN DE NIVELES Y GESTIÓN DEL LLENADO EN EL ESTADO DE REPOSO. CADA TIMEOUT_LASER segundos.
/*****************************************************************************************************/
  if ((T_actual - timeout_medida) > TIMEOUT_GESTIONAR_NIVELES) {
    timeout_medida = T_actual;
    int accion_estado = mqprinc_GestionarDistancias (LLENAR);
    Bluetooth_Notification ();
    if (accion_estado == REPOSO) {
      tiempo_llenado = (T_actual - timeout_IA_llenado) / 60000;       //TIEMPO EN MINUTOS QUE HA TARDADO EN LLENAR CADA VEZ.
      Serial.print (F("--- EL TIEMPO DE LLENADO EN ESTA OCASIÓN HA SIDO DE: "));
      Serial.print (tiempo_llenado);
      Serial.println (F(" minutos ----"));
      datos_EEPROM_Write_INT ((int) tiempo_llenado, DAT_TIEMPO_LLENADO);
      maqsec_ActivarCondicion (CS0);    //Lleva la máquina de estados al estado de REPOSO.  
      return 0;
    } else if (accion_estado == ALARMAS) {
      maqsec_ActivarCondicion (CS4);    //Lleva la máquina de estados al estado de ALARMAS.  
      return 0;
    }
    Serial.print(F("Medida de llenado: "));
    Serial.print(media_llenado_AUX);
    Serial.print(F(" - Tiempo restante hasta tomar medida: "));
    Serial.print(((maqDatosEEPROM.time_piscina * 60000) - (T_actual - timeout_llenar)) / 60000);
    Serial.print(F(" minutos y "));
    Serial.print((int)(((maqDatosEEPROM.time_piscina * 60000) - (T_actual - timeout_llenar)) % 60000) / 1000);
    Serial.println(F(" segundos"));
  }
/*****************************************************************************************************/
//TRATAMIENTO DE DATOS EN LA EEPROM.
/*****************************************************************************************************/
  int datos = Comanpar_ConsultaDatosPSerie (parametro, comando_EEPROM, T_actual);
  if (datos == DATOS_CORRECTO || datos == DATOS_CORRECTO_BLUETOOTH) {
    int command = Comanpar_TratarComandoActiva (parametro, comando_EEPROM[0], datos);
    if (command != DATOS_CORRECTO) {
      Serial.print (F("RESULTADO DEL COMANDO: "));
      Serial.println (command);
    }
    if (comando_EEPROM[0] == MSB_PARA_PARARLL) {
      Electrovalv_EV_LLENADO (NIVEL_BAJO);      //SE CIERRA LA ELECTROVÁLVULA DE VACIADO.
      maqsec_ActivarCondicion (CS0);          //Lleva la máquina de estados al estado REPOSO.
    }
    mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, MOSTRAR_DATOS); //LEE LOS PARÁMETROS DE LA EEPROM. 
  }
  return 0;
}
//------------------------------------------------------------------------------
// FUNCTION: short mqprin_EstLLENAR_1A (void)
// DESCRIPTION: Function that executes the functionalities of LLENAR state by first time.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqprin_EstLLENAR_1A        (unsigned long T_actual, int comando_LoRa, int boton_tecla) {
  char parametro[DATOSPUERTOSERIE_PARA];
  int  comando_EEPROM[2];
  unsigned long tiempo_llenado = 0;

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    Serial.println (F("ESTADO LLENAR_1A"));
    Electrovalv_EV_LLENADO (NIVEL_ALTO);
    mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, MOSTRAR_DATOS); //LEE LOS PARÁMETROS DE LA EEPROM. 
    maqDatosEEPROM.N_llenados += 1;
    datos_EEPROM_Write_INT ((int) maqDatosEEPROM.N_llenados, DAT_N_LLENADOS);
    Last_status = LLENAR;
    timeout_llenar = timeout_IA_llenado = T_actual;
    Electrovalv_Setup_LED_ONOFF_BOTTON (Last_status, T_actual);
    media_llenado_AUX = (int) mqsec_aux_MEDIA_RELES ();
  }
  //------------------------------------ PROCCESS --------------------------------
  Electrovalv_LED_ONOFF_BOTTON (T_actual);
  //ESTE COMANDO SE PUEDE RECIBIR COMO CONSECUENCIA DE LA RECEPCIÓN DE UN COMANDO #VINCULAR* EN LA PCB-CABECERA POR LA INSTALACIÓN
  // DE UN SMART FILL CERCANO (VECINO)
  if (comando_LoRa == LORA_PING || comando_LoRa == LORA_PAYLOAD_OK) {
    maqflags.flag_ACK = true;
    timeout_ACK = T_actual;
    if (comando_LoRa == LORA_PAYLOAD_OK) maqflags.flag_init_recepcion = false;
  }
  if (maqflags.flag_ACK == true && (T_actual - timeout_ACK) > TIMEOUT_ACK) {
    maqflags.flag_ACK = false;
    mqsed_aux_Enviar_COMANDO_LORA (LORA_ACK);
  }
/*****************************************************************/
/******* SE COMPRUEBA INCONGRUENCIA DE DATOS *********************/
/*****************************************************************/
  if (datos_Comprobar_Incongruencia_de_datos () == DATOS_EEPROM_ALARMA) {  //Se ha producido una Alarma de INCONGRUENCIA de datos.
    reg_alarmas |= ALARMA_INCONGRUENCIA;
  }
/*****************************************************************/
/******* SE COMPRUEBAN TODAS LAS ALARMAS *************************/
/*****************************************************************/
  if (reg_alarmas != ALARMA_SIN_ALARMAS) {
    maqsec_ActivarCondicion (CS4);          //Lleva la máquina de estados al estado de ALARMAS.
  }
/**************************************************************************/
/******* SE PASA AL ESTADO DE REPOSO POR PULSAR BOTÓN *********************/
/**************************************************************************/
  if (boton_tecla == BOTON_ON) {
    tiempo_llenado = (T_actual - timeout_IA_llenado) / 60000;       //TIEMPO EN MINUTOS QUE HA TARDADO EN LLENAR CADA VEZ.
    Serial.print (F("--- EL TIEMPO DE LLENADO EN ESTA OCASIÓN HA SIDO DE: "));
    Serial.print (tiempo_llenado);
    Serial.println (F(" minutos ----"));
    datos_EEPROM_Write_INT ((int) tiempo_llenado, DAT_TIEMPO_LLENADO);
    Electrovalv_EV_LLENADO (NIVEL_BAJO);
    maqsec_ActivarCondicion (CS0);      //Vuelve al estado de REPOSO.
    return 0;
  }
/**************************************************************************/
/******* SE COMPRUEBA EL TIEMPO MÁXIMO DE LLENADO *************************/
/**************************************************************************/
  maqDatosEEPROM.time_piscina = datos_EEPROM_Read_INT (DAT_TIME_LLENAR_1A);    //Lo vuelvo a leer por si hubiera cambiado.
  if ((T_actual - timeout_llenar) > (maqDatosEEPROM.time_piscina * 60000)) {
    Serial.print (F("//// SE HA SUPERADO EL TIEMPO DE LLENADO: "));
    //Serial.print (maqDatosEEPROM.time_piscina * 60000/1000);
    Serial.print (maqDatosEEPROM.time_piscina);
    Serial.println (F(" minutos."));
    reg_alarmas |= ALARMA_LLENADO;   //ESTO OCURRE CUANDO DESPUÉS DE ESTAR X MINUTOS LLENANDO LA MEDIA NO HA DISMINUIDO.
    maqsec_ActivarCondicion (CS4);    //Lleva la máquina de estados al estado de ALARMAS.  
    return 0;
  }
/*****************************************************************************************************/
//MEDICIÓN DE NIVELES Y GESTIÓN DEL LLENADO EN EL ESTADO DE REPOSO. CADA TIMEOUT_LASER segundos.
/*****************************************************************************************************/
  if ((T_actual - timeout_medida) > TIMEOUT_GESTIONAR_NIVELES) {
    timeout_medida = T_actual;
    int accion_estado = mqprinc_GestionarDistancias (LLENAR_1A);
    Bluetooth_Notification ();
    if (accion_estado == REPOSO) {
      tiempo_llenado = (T_actual - timeout_IA_llenado) / 60000;       //TIEMPO EN MINUTOS QUE HA TARDADO EN LLENAR CADA VEZ.
      Serial.print (F("--- EL TIEMPO DE LLENADO EN ESTA OCASIÓN HA SIDO DE: "));
      Serial.print (tiempo_llenado);
      Serial.println (F(" minutos ----"));
      datos_EEPROM_Write_INT ((int) tiempo_llenado, DAT_TIEMPO_LLENADO);
      maqsec_ActivarCondicion (CS0);    //Lleva la máquina de estados al estado de REPOSO.  
      return 0;
    } else if (accion_estado == ALARMAS) {
      maqsec_ActivarCondicion (CS4);    //Lleva la máquina de estados al estado de ALARMAS.  
      return 0;
    }
    Serial.print(F("Medida de llenado: "));
    //Esto estaría mal, ya que hay que mostrar la primera media de la que partio.
    Serial.print(media_llenado_AUX);
    Serial.print(F(" - Tiempo restante hasta tomar medida: "));
    Serial.print(((maqDatosEEPROM.time_piscina * 60000) - (T_actual - timeout_llenar)) / 60000);
    Serial.print(F(" minutos y "));
    Serial.print((int)(((maqDatosEEPROM.time_piscina * 60000) - (T_actual - timeout_llenar)) % 60000) / 1000);
    Serial.println(F(" segundos"));
  }
/*****************************************************************************************************/
//TRATAMIENTO DE DATOS EN LA EEPROM.
/*****************************************************************************************************/
  int datos = Comanpar_ConsultaDatosPSerie (parametro, comando_EEPROM, T_actual);
  if (datos == DATOS_CORRECTO || datos == DATOS_CORRECTO_BLUETOOTH) {
    int command = Comanpar_TratarComandoActiva (parametro, comando_EEPROM[0], datos);
    if (command != DATOS_CORRECTO) {
      Serial.print (F("RESULTADO DEL COMANDO: "));
      Serial.println (command);
    }
    if (comando_EEPROM[0] == MSB_PARA_PARARLL) {
      Electrovalv_EV_LLENADO (NIVEL_BAJO);      //SE CIERRA LA ELECTROVÁLVULA DE VACIADO.
      maqsec_ActivarCondicion (CS0);          //Lleva la máquina de estados al estado REPOSO.
    }
    mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, MOSTRAR_DATOS); //LEE LOS PARÁMETROS DE LA EEPROM. 
  }
  return 0;
}
//------------------------------------------------------------------------------
// FUNCTION: short mqprin_EstALARMAS (void)
// DESCRIPTION: Function that executes the functionalities of ALARMAS's management.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqprin_EstALARMAS          (unsigned long T_actual, int comando_LoRa, int boton_tecla) {
  char parametro[DATOSPUERTOSERIE_PARA];
  int  comando_EEPROM[2], reg_alarmas_aux, accion_estado;

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    Serial.println (F("ESTADO ALARMAS"));
    Electrovalv_EV_LLENADO (NIVEL_BAJO);
    timeout_medida = timeout_apertura_ev = timeout_RESET_CABEZAL = T_actual;
    maqflags.flag_apertura_1A = false;
    Last_status = ALARMAS;
    Electrovalv_Setup_LED_ONOFF_BOTTON (Last_status, T_actual);
    reg_alarmas_aux = reg_alarmas & ALARMA_ULTRASONIDO;
    if (reg_alarmas_aux == ALARMA_ULTRASONIDO) {
      //timeout_incremental_reset_CABEZAL = TIMEOUT_RESET_CABEZAL; 
      maqflags.flag_reset_cabezal = false;
      //Serial.println (F("¡¡¡¡ RESET TIMEOUT_RESET_CABEZAL"));
      //QUITAR LA ALIMENTACIÓN DE LA PCB_CABEZAL.
    }
  }
  //------------------------------------ PROCCESS --------------------------------
  Electrovalv_LED_ONOFF_BOTTON (T_actual);
  if (reg_alarmas == ALARMA_SIN_ALARMAS) {
    maqsec_ActivarCondicion (CS0);          //Lleva la máquina de estados al estado REPOSO.
    return 0;
  }
/*****************************************************************************************************/
//***** AQUI SE COMPRUEBAN LOS COMANDOS RECIBIDOS Y SE RESPONDE A LA PCB_CABECERA*********************/
/*****************************************************************************************************/
  if (comando_LoRa == LORA_PING || comando_LoRa == LORA_PAYLOAD_OK) {
    maqflags.flag_ACK = true;
    timeout_ACK = T_actual;
    if (comando_LoRa == LORA_PAYLOAD_OK) maqflags.flag_init_recepcion = false;
    reg_alarmas &= ALARMA_CABEZAL_C1;
  }
  if (maqflags.flag_ACK == true && (T_actual - timeout_ACK) > TIMEOUT_ACK) {
    maqflags.flag_ACK = false;
    mqsed_aux_Enviar_COMANDO_LORA (LORA_ACK);
  }
/*****************************************************************************************************/
//***** SE PULSA EL BOTÓN PARA ANULAR LA ALARMA DE TIEMPO EXCESIVO DE LLENADO ************************/
/*****************************************************************************************************/
  if (boton_tecla == BOTON_ON && reg_alarmas == ALARMA_LLENADO) {
    Serial.println (F("SE ANULA MANUALMENTE LA ALARMA DE TIEMPO EXCESIVO!!!!"));
    maqsec_ActivarCondicion (CS0);          //Lleva la máquina de estados al estado REPOSO.
    reg_alarmas = reg_alarmas & ALARMA_LLENADO_C1;        //Esto es para anular la alarma.
    return 0;
  }
  if (reg_alarmas == ALARMA_LLENADO && mqsec_aux_MEDIA_RELES () < (maqDatosEEPROM.nivel_seco - (int)(maqDatosEEPROM.nivel_seco - maqDatosEEPROM.nivel_optimo)/2)) {
    reg_alarmas &= ALARMA_LLENADO_C1;
    return 0;
  }
/*****************************************************************************************************/
//MEDICIÓN DE NIVELES Y GESTIÓN DEL LLENADO EN EL ESTADO DE REPOSO. CADA TIMEOUT_LASER segundos.
/*****************************************************************************************************/
  if ((T_actual - timeout_medida) > TIMEOUT_GESTIONAR_NIVELES) {
    timeout_medida = T_actual;
    mqsec_Gestionar_ALARMAS (T_actual);                     //Función para gestionar los mensajes por ALARMAS.
    accion_estado = mqprinc_GestionarDistancias (ALARMAS);
    Bluetooth_Notification ();
  }
/*****************************************************************************************************/
//TRATAMIENTO DE DATOS EN LA EEPROM.
/*****************************************************************************************************/
  int datos = Comanpar_ConsultaDatosPSerie (parametro, comando_EEPROM, T_actual);
  if (datos == DATOS_CORRECTO || datos == DATOS_CORRECTO_BLUETOOTH) {
    int command = Comanpar_TratarComandoActiva (parametro, comando_EEPROM[0], datos);
    if (command != DATOS_CORRECTO) {
      Serial.print (F("RESULTADO DEL COMANDO: "));
      Serial.println (command);
    }
  }
/*****************************************************************************************************/
//**** AQUI ES DONDE SE MUESTRA LA CADENCIA DEL LED_BOTON PARA MOSTRAR EL TIPO DE ALARMA QUE ES*******/
/*****************************************************************************************************/
  //TESTEO DE INCONGRUENCIA DE DATOS.
  reg_alarmas_aux = reg_alarmas & ALARMA_INCONGRUENCIA;
  if (reg_alarmas_aux == ALARMA_INCONGRUENCIA) {
    //Electrovalv_LED_BLINKONOFF_BOTTON (T_actual, TIMEOUT_ON_INCONGRUENCIA, TIMEOUT_OFF_INCONGRUENCIA);
    return 0;
  }
  //TESTEO DE RESPUESTA DE LA PCB_CABEZAL.
  reg_alarmas_aux = reg_alarmas & ALARMA_CABEZAL;
  if (reg_alarmas_aux == ALARMA_CABEZAL) {
    //Electrovalv_LED_BLINKONOFF_BOTTON (T_actual, TIMEOUT_ON_CABEZAL, TIMEOUT_OFF_CABEZAL);
    return 0;
  }
  //TESTEO DE MEDIDA ERRONEA DE LASER.
  reg_alarmas_aux = reg_alarmas & ALARMA_MEDIDORES_FAIL;
  if (reg_alarmas_aux == ALARMA_MEDIDORES_FAIL) {
    //Electrovalv_LED_BLINKONOFF_BOTTON (T_actual, TIMEOUT_ON_LASER_MAL, TIMEOUT_OFF_LASER_MAL);
  }
  reg_alarmas_aux = reg_alarmas & ALARMA_ULTRASONIDO;
  if (reg_alarmas_aux == ALARMA_ULTRASONIDO) {
    if (mqsec_aux_MEDIA_RELES () < maqDatosEEPROM.nivel_seco) {
      reg_alarmas &= ALARMA_ULTRASONIDO_C1;
      reg_alarmas &= ALARMA_LLENADO_C1;
      Serial.print (F("------***** NIVEL SUPERIOR A SECO: "));
      Serial.println (mqsec_aux_MEDIA_RELES ());
      Serial.println ();
      Serial.print (F("------***** SE RESETEAN TODOS LOS PARAMETROS DE LOS OUTLIERS ******-------"));
      mqsec_aux_RESET_OUTLIERS ();
      timeout_RESET_CABEZAL = T_actual;
    }
//AQUI SE VA A QUITAR LA ALIMENTACIÓN Y SE INCREMENTA EL TIMEOUT EN 1 SEGUNDO.
    if (accion_estado == OUTLIER_RESET_CABEZAL) {
      Electrovalv_EV_BOMBA_VAC (EV_OFF);
      if (maqflags.flag_reset_cabezal == false) {
        maqflags.flag_reset_cabezal = true;
        timeout_RESET_CABEZAL = T_actual;
        timeout_incremental_reset_CABEZAL += (unsigned long) (1000 * 60);
        if (timeout_incremental_reset_CABEZAL >= TIMEOUT_RESET_CABEZAL_MAX) {
          timeout_incremental_reset_CABEZAL = TIMEOUT_RESET_CABEZAL_MAX;  //DE AQUÍ YA NO PASA.
        }
//ESTO ES SIMPLEMENTE PARA VISUALIZAR COMO INCREMENTA EL TIMEOUT QUE REGULA EL APAGADO.
        Serial.println ();
        Serial.println (F("------***** Electrovalv_EV_BOMBA_VAC (EV_ON) *****------"));
        Serial.println (F("¡¡¡¡¡ QUITAR--- ALIMENTACIÓN PCB_CABEZAL"));
        Serial.println ();
        Serial.print (F("¡¡¡¡¡ TIMEOUT_RESET_CABEZAL: "));
        Serial.println (timeout_incremental_reset_CABEZAL);
        //VUELVE A QUITAR ALIMENTACIÓN EN LA PCB_CABEZAL.
      }
    }
    if ((T_actual - timeout_RESET_CABEZAL) >= timeout_incremental_reset_CABEZAL && maqflags.flag_reset_cabezal == true) {
      timeout_RESET_CABEZAL = T_actual;
      maqflags.flag_reset_cabezal = false;
//VUELVE A PONER ALIMENTACIÓN EN LA PCB_CABEZAL
      Electrovalv_EV_BOMBA_VAC (EV_ON);
      Serial.println ();
      Serial.println (F("------***** Electrovalv_EV_BOMBA_VAC (EV_OFF) *****------"));
      Serial.println (F("¡¡¡¡¡ PONER+++ ALIMENTACIÓN PCB_CABEZAL"));
      Serial.println ();
      //mqsec_aux_RESET_OUTLIERS ();
      maqsec_ActivarCondicion (CS0);          //Lleva la máquina de estados al estado REPOSO.
      reg_alarmas = ALARMA_SIN_ALARMAS;
      return 0;
    }
  }
  //TESTEO DE TIEMPO EXCESIVO DE LLENADO.
  reg_alarmas_aux = reg_alarmas & ALARMA_LLENADO;
  if (reg_alarmas_aux == ALARMA_LLENADO) {
    //Electrovalv_LED_BLINKONOFF_BOTTON (T_actual, TIMEOUT_ON_TELL, TIMEOUT_OFF_TELL);
  }
  //CONTROL DE LA ALARMA DE INUNDACIÓN.
  reg_alarmas_aux = reg_alarmas & ALARMA_INUNDACION;
  if (reg_alarmas_aux == ALARMA_INUNDACION) {
    //Electrovalv_LED_BLINKONOFF_BOTTON (T_actual, TIMEOUT_ON_INUNDACION, TIMEOUT_OFF_INUNDACION);
    if (maqflags.flag_apertura_1A == false) {
      timeout_apertura_ev = T_actual;
      maqflags.flag_apertura_1A = true;
      Serial.println (F("--- ESTADO ALARMAS: ALARMA DE INUNDACIÓN - ON. ---"));
      Electrovalv_EV_VACIADO    (NIVEL_ALTO);                                //PRIMERO SE ABRE LA ELECTROVÁLVULA DE VACIADO.
    }
  //SI HA DEJADO DE INUNDARSE, HAY QUE PARAR TODO Y VOLVER AL ESTADO NORMAL DE FUNCIONAMIENTO. QUIZÁS QUEDE OTRA ALARMA.
  } else {
    Electrovalv_EV_VACIADO    (NIVEL_BAJO);
    maqflags.flag_apertura_1A = false;
    timeout_apertura_ev = T_actual;
  }
  return 0;
}
//------------------------------------------------------------------------------
// FUNCTION: short mqprin_EstVACIAR (void)
// DESCRIPTION: Function that executes the functionalities of VACIAR management.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqprin_EstVACIAR           (unsigned long T_actual, int comando_LoRa, int boton_tecla) {
  char parametro[DATOSPUERTOSERIE_PARA];
  int  comando_EEPROM[2];

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    Serial.println (F("ESTADO VACIAR"));
    Last_status = VACIAR;
    Electrovalv_EV_VACIADO (NIVEL_ALTO);    //SE ABRE LA ELECTROVÁLVULA DE VACIADO.
    Electrovalv_Setup_LED_ONOFF_BOTTON (Last_status, T_actual);
  }
  //------------------------------------ PROCCESS --------------------------------
  Electrovalv_LED_ONOFF_BOTTON (T_actual);
/*****************************************************************************************************/
/**** ESTO ES PARA NO VACIAR MÁS DEL NIVEL OPTIMO. AL MENOS POR EL SMART FILL. SI ALGUIEN QUIERE *****/
/**** VACIAR LA PISCINA QUE UTILICE OTRO MÉTODO, PERO NO ESTE EQUIPO. ********************************/
/*****************************************************************************************************/
  media_ultrasonidos      = (int) mqsec_aux_MEDIA_RELES ();
  if (media_ultrasonidos > maqDatosEEPROM.nivel_optimo) {  
    Serial.println ();
    Serial.println (F("***** ----- SE HA LLEGADO AL NIVEL OPTIMO EN EL PROCESO DE VACIADO ----****"));
    Serial.println ();
    Electrovalv_EV_VACIADO (NIVEL_BAJO);    //SE CIERRA LA ELECTROVÁLVULA DE VACIADO.
    maqsec_ActivarCondicion (CS0);            //Al estado de REPOSO.
    return 0;
  }
/*****************************************************************************************************/
  //ESTO ES PARA LLEVAR AL EQUIPO AL ESTADO DE REPOSO.
  if (boton_tecla == BOTON_ON) {
    Electrovalv_EV_VACIADO (NIVEL_BAJO);    //SE CIERRA LA ELECTROVÁLVULA DE VACIADO.
    maqsec_ActivarCondicion (CS0);            //Al estado de REPOSO.
    return 0;
  }

/*****************************************************************************************************/
//TRATAMIENTO DE DATOS EN LA EEPROM.
/*****************************************************************************************************/
  int datos = Comanpar_ConsultaDatosPSerie (parametro, comando_EEPROM, T_actual);
  if (datos == DATOS_CORRECTO || datos == DATOS_CORRECTO_BLUETOOTH) {
    int command = Comanpar_TratarComandoActiva (parametro, comando_EEPROM[0], datos);
    if (command != DATOS_CORRECTO) {
      Serial.print (F("RESULTADO DEL COMANDO: "));
      Serial.println (command);
    }
    if (comando_EEPROM[0] == MSB_PARA_PARARVACIAR) {
      Electrovalv_EV_VACIADO (NIVEL_BAJO);      //SE CIERRA LA ELECTROVÁLVULA DE VACIADO.
      maqsec_ActivarCondicion (CS0);          //Lleva la máquina de estados al estado REPOSO.
    }
    mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, MOSTRAR_DATOS); //LEE LOS PARÁMETROS DE LA EEPROM. 
  }
  return 0;
}

//------------------------------------------------------------------------------
// FUNCTION: short mqprin_EstESTIRARPERSIANA (void)
// DESCRIPTION: Function that executes the functionalities of ESTIRARPERSIANA management.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqprin_EstESTIRARPERSIANA  (unsigned long T_actual, int comando_LoRa, int boton_tecla, int tecla_FCEstirar) {
  char parametro[DATOSPUERTOSERIE_PARA];
  int  comando_EEPROM[2];

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    Serial.println (F("ESTADO ESTIRAR PERSIANA"));
    Last_status = ESTIRARPERSIANA;
    timeout_persiana = T_actual;
    maqDatosEEPROM.pulso_persiana = datos_EEPROM_Read_INT (DAT_PULSO_PERSIANA);
    Serial.print (F("Tiempo del Pulso ESTIRAR: "));
    Serial.print (maqDatosEEPROM.pulso_persiana);
    Serial.println (F(" segundos."));
    Electrovalv_EV_PERSIANA_ESTIRAR (NIVEL_ALTO);
  }
/**************** FALTA MIRAR LOS NIVELES *****************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
  //------------------------------------ PROCCESS --------------------------------
  if ((T_actual - timeout_persiana) > (maqDatosEEPROM.pulso_persiana * 1000) ||
      tecla_FCEstirar == BOTON_ON) {
    maqsec_ActivarCondicion (CS0);          //Lleva la máquina de estados al estado REPOSO.
    Electrovalv_EV_PERSIANA_ESTIRAR (NIVEL_BAJO);
    Electrovalv_EV_LED_BOTTON (NIVEL_BAJO);
  }
//TRATAMIENTO DE DATOS EN LA EEPROM.
/*****************************************************************************************************/
  int datos = Comanpar_ConsultaDatosPSerie (parametro, comando_EEPROM, T_actual);
  if (datos == DATOS_CORRECTO || datos == DATOS_CORRECTO_BLUETOOTH) {
    int command = Comanpar_TratarComandoActiva (parametro, comando_EEPROM[0], datos);
    if (command != DATOS_CORRECTO) {
      Serial.print (F("RESULTADO DEL COMANDO: "));
      Serial.println (command);
    }
    if (comando_EEPROM[0] == MSB_PARA_RECOGERP) {
      maqsec_ActivarCondicion (CS8);          //Lleva la máquina de estados al estado RECOGER_PERSIANA.
    }
    mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, MOSTRAR_DATOS); //LEE LOS PARÁMETROS DE LA EEPROM. 
  }
  return 0;
}
//------------------------------------------------------------------------------
// FUNCTION: short mqprin_EstRECOGERPERSIANA (void)
// DESCRIPTION: Function that executes the functionalities of RECOGERPERIANA management.
// PARAMETERS: Void.
// RETURNS:  0.
//------------------------------------------------------------------------------
short mqprin_EstRECOGERPERSIANA  (unsigned long T_actual, int comando_LoRa, int boton_tecla, int tecla_FCRecoger) {
  char parametro[DATOSPUERTOSERIE_PARA];
  int  comando_EEPROM[2];

  if (maqsec_bits.flag_1A_vez == false) {   //First time that is entried to state.
    maqsec_SetFlag1A ();               //Set the flag of first time.
    Serial.println (F("ESTADO RECOGER PERSIANA"));
    Last_status = RECOGERPERSIANA;
    timeout_persiana = T_actual;
    maqDatosEEPROM.pulso_persiana = datos_EEPROM_Read_INT (DAT_PULSO_PERSIANA);
    Serial.print (F("Tiempo del Pulso RECOGER: "));
    Serial.print (maqDatosEEPROM.pulso_persiana);
    Serial.println (F(" segundos."));
    Electrovalv_EV_PERSIANA_RECOGER (NIVEL_ALTO);
  }
/**************** FALTA MIRAR LOS NIVELES *****************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
  //------------------------------------ PROCCESS --------------------------------
  if ((T_actual - timeout_persiana) > (maqDatosEEPROM.pulso_persiana * 1000) ||
      tecla_FCRecoger == BOTON_ON) {
    maqsec_ActivarCondicion (CS0);          //Lleva la máquina de estados al estado REPOSO.
    Electrovalv_EV_PERSIANA_RECOGER (NIVEL_BAJO);
    Electrovalv_EV_LED_BOTTON (NIVEL_BAJO);
  }
//TRATAMIENTO DE DATOS EN LA EEPROM.
/*****************************************************************************************************/
  int datos = Comanpar_ConsultaDatosPSerie (parametro, comando_EEPROM, T_actual);
  if (datos == DATOS_CORRECTO || datos == DATOS_CORRECTO_BLUETOOTH) {
    int command = Comanpar_TratarComandoActiva (parametro, comando_EEPROM[0], datos);
    if (command != DATOS_CORRECTO) {
      Serial.print (F("RESULTADO DEL COMANDO: "));
      Serial.println (command);
    }
    if (comando_EEPROM[0] == MSB_PARA_ESTIRARP) {
      maqsec_ActivarCondicion (CS7);          //Lleva la máquina de estados al estado RECOGER_PERSIANA.
    }
    mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, MOSTRAR_DATOS); //LEE LOS PARÁMETROS DE LA EEPROM. 
  }
  return 0;
}

void mqsec_Iniciar_Flags (void) {
  maqflags.flag_comando = false;
  maqflags.flag_boton = false;
}

int mq_sec_ESTADO (void) {
  return Last_status;
}

int mqprinc_GestionarDistancias (int estado_medidas) {
  int diferencia_alturas = 0, reg_alarmas_aux, reg_alarmas_aux2, result;

  result = mqsec_aux_Comprobar_OUTLIER ();
  if (result < 0) {
    reg_alarmas |= ALARMA_ULTRASONIDO;
    Serial.println ();
    Serial.println (F("*****////// ¡¡¡¡ ALARMA DE ULTRASONIDOS: ON !!!!! *****/////"));
    Serial.println ();
    //return SALIDA_MQSEC;
    return result;
  } else {
    //Serial.println ();
    //Serial.println (F("*****////// ¡¡¡¡ ALARMA DE ULTRASONIDOS: OFF !!!!! *****/////"));
    //Serial.println ();
    reg_alarmas &= ALARMA_ULTRASONIDO_C1;
  }
  media_ultrasonidos      = (int) mqsec_aux_MEDIA_RELES ();
  desviacion_ultrasonidos = (int) mqsec_aux_DESVIACION_RELES ();
  switch (estado_medidas) {
  case REPOSO:
    Serial.print (F("/**** REPOSO.-"));
    break;
  case LLENAR:
    Serial.print (F("/**** LLENAR.-"));
    break;
  case LLENAR_1A:
    Serial.print (F("/**** LLENAR_1A.-"));
    break;
  case ALARMAS:
    Serial.print (F("/**** ALARMAS.-"));
    break;
  default:
    Serial.print (F("/**** ESTADO.- "));
    Serial.print (estado_medidas);
    break;
  }
  Serial.print (F("\t****--- MEDIA: "));
  Serial.print (media_ultrasonidos);
  Serial.print (F("\t\t****--- DESVIACIÓN: "));
  Serial.println (desviacion_ultrasonidos);
  mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, NO_MOSTRAR_DATOS);
  //Serial.println();
  Serial.print (F("\t\t****--- NIVEL OPTIMO: "));
  Serial.print (maqDatosEEPROM.nivel_optimo);
  Serial.print (F("\t****--- DESVIACIÓN: "));
  Serial.print (maqDatosEEPROM.desviacion);
  Serial.print (F("\t****--- NIVEL SECO: "));
  Serial.println (maqDatosEEPROM.nivel_seco);
  Serial.println();  
  if (media_ultrasonidos <= MINIMA_MEDIDA_ULTRASONIDOS || media_ultrasonidos >= MAXIMA_MEDIDA_ULTRASONIDOS) {
    reg_alarmas |= ALARMA_MEDIDORES_FAIL;
    return SALIDA_MQSEC;
  } else {
    reg_alarmas &= ALARMA_MEDIDORES_FAIL_C1;
  }
  switch (estado_medidas) {
    case REPOSO:
      if (desviacion_ultrasonidos < maqDatosEEPROM.desviacion) {     //El agua está quieta, no hay nadie bañándose y se podría empezar a llenar la Piscina.
        if (media_ultrasonidos >= maqDatosEEPROM.nivel_seco) {  
          return LLENAR;
        }
      } else if (media_ultrasonidos >= maqDatosEEPROM.nivel_seco) {
        Serial.println (F("DESVIACIÓN ATÍPICA. ESPERANDO PARA LLENAR."));  
      }
      if (media_ultrasonidos < maqDatosEEPROM.nivel_optimo) {
        diferencia_alturas = maqDatosEEPROM.nivel_optimo - media_ultrasonidos;  //Calculamos la diferencia.
        Serial.println (F("¡¡¡ATENCIÓN!!! - NIVEL OPTIMO SUPERADO."));
        Serial.print (F("Diferencia de alturas: "));
        Serial.println (diferencia_alturas);
        if (diferencia_alturas >= MEDIDA_MARGEN_LASER && desviacion_ultrasonidos < maqDatosEEPROM.desviacion) {  //MEDIDA_MARGEN_LASER >= 50.
          Serial.println (F("////// INUNDACIÓN!!!! /////"));
          reg_alarmas |= ALARMA_INUNDACION;             //INUNDACIÓN!!!! bit 0 del registro de ALARMAS.
        }
      }
      //NO HACER NADA, limitarse a medir y mostrar. De momento.
      return SALIDA_MQSEC;
      break;
    case LLENAR_1A:
    case LLENAR:     //En este estado voy a tener en cuenta las dos medidas por si acaso.
      if (desviacion_ultrasonidos >= maqDatosEEPROM.desviacion ) {     //El agua NO está quieta y podría haber alguien bañándose. Hay que parar de llenar.
        Serial.println (F("DESVIACIÓN ATÍPICA. PARAR DE LLENAR."));
        return REPOSO;
      } else {
        if (media_ultrasonidos <= maqDatosEEPROM.nivel_optimo) {
            Serial.println (F("NIVEL OPTIMO ALCANZADO. PARAR DE LLENAR."));
            return REPOSO;
        }
      }
      return SALIDA_MQSEC;
      break;
    case ALARMAS:
      //strandtest_REDCOLOR ();
      //ESTO ES PARA NO TENER EN CUENTA LOS NIVELES CUANDO EL LASER ESTÁ MAL.
      reg_alarmas_aux = reg_alarmas & ALARMA_MEDIDORES_FAIL;
      reg_alarmas_aux2 = reg_alarmas & ALARMA_ULTRASONIDO;
      if (reg_alarmas_aux == ALARMA_MEDIDORES_FAIL || reg_alarmas_aux2 == ALARMA_ULTRASONIDO) {
        Serial.println (F("¡¡¡¡¡NO SE COMPRUEBAN NIVELES POR ESTAR EL ULTRASONIDOS MAL!!!!"));
        return SALIDA_MQSEC;
      }
      //Si la medida del laser ha superado el nivel OPTIMO es porque de alguna manera se ha vaciado la piscina.
      if (media_ultrasonidos > (maqDatosEEPROM.nivel_optimo - (MEDIDA_MARGEN_LASER/2))) {  
        reg_alarmas &= ALARMA_INUNDACION_C1;              //SE ANULA EL BIT DE ALARMA DE INUNDACIÓN CUANDO SE SUPERA EL NIVEL OPTIMO.
      }
      //Si la medida del laser es menor que el nivel OPTIMO es porque de alguna manera se ha llenado la piscina.
      if (media_ultrasonidos <= maqDatosEEPROM.nivel_optimo) {  
        reg_alarmas &= ALARMA_LLENADO_C1;                 //SE ANULA EL BIT DE ALARMA DE TIEMPO DE LLENADO EXCESIVO CUANDO SE SUPERA EL NIVEL OPTIMO.
        diferencia_alturas = maqDatosEEPROM.nivel_optimo - media_ultrasonidos;  //Calculamos la diferencia.
        Serial.println (F("¡¡¡ATENCIÓN!!! - NIVEL OPTIMO SUPERADO."));
        Serial.print (F("Diferencia de alturas: "));
        Serial.println (diferencia_alturas);
        if (diferencia_alturas >= MEDIDA_MARGEN_LASER && desviacion_ultrasonidos < maqDatosEEPROM.desviacion) {  //MEDIDA_MARGEN_LASER >= 50.
            reg_alarmas |= ALARMA_INUNDACION;             //INUNDACIÓN!!!! bit 0 del registro de ALARMAS.
        }
      }
      return SALIDA_MQSEC;
      break;
    default:
      return SALIDA_MQSEC;
      break;
  }
  return SALIDA_MQSEC;
}

int mq_sec_ALARMAS_Bluetooth (void) {
  return reg_alarmas;
}

void mqsec_aux_Reset_flag_outliers (void) {
  maqflags.flag_reset_outliers = false;
}
void mqsec_aux_Set_flag_outliers (void) {
  maqflags.flag_reset_outliers = true;
}
