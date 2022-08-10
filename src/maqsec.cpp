
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

#include <string.h>
#include <advancedSerial.h>

#include "maqsec.h"
#include "mq_sec.h"
#include "hardware.h"

//-------------------- GLOBAL VARIABLES ----------------------------------------
static short condicionsec[NMAX_CONDICIONESEC];   //Limitada al número máximo de CONDICIONES.
static int maqsecEstado, est_anterior;          //Estas son necesarias para otros ficheros.
MAQFLAGSEC maqsec_bits;

//-------------------- GLOBAL FUNCTIONS ----------------------------------------
static void maqsec_ResetFlag1A (void);
static int  maqsec_RevisionCondiciones (int Estado);

//------------------------------------------------------------------------------
// FUNCTION: void mqest_Principal (void)
// DESCRIPTION: Función que llama a los diferentes estados de la MAQUINA. Las
// funciones aquí descritas se llaman de forma concurrente, por lo que se está
// comprobando en segundo plano las entradas y salidas de la placa.          
// - PARAMETERS: T_actual = tiempo actual, Entradas = Las teclas pulsadas.
// - RETURNS:  Void 
//------------------------------------------------------------------------------

void maqsec_SECUNDARIA (unsigned long T_actual, int comando_LoRa, int boton_tecla, bool status_Bluetooth,
                        int tecla_FCEstirar, int tecla_FCRecoger) {
  //unsigned long T = 0;
    //Esto entraría en la parte de inicialización
  if (est_anterior != maqsecEstado) { //Activa o no el estado de primera vez a la hora de entrar a un estado.
      maqsec_ResetFlag1A ();
  } 
  est_anterior = maqsecEstado;      //Guardamos el estado actual para la próxima vez.
  switch (maqsecEstado) {           //Aquí se llaman todas funciones de los estados en relación al estado actual.
  case INICIAL:
    mqsec_EstINICIAL  (T_actual);     //Function of state: INICIAL.
    break;
  case REPOSO:
    mqsec_EstREPOSO (T_actual, comando_LoRa, boton_tecla);      
    break;
  case VINCULAR:
    mqsec_EstVINCULAR (T_actual, comando_LoRa, boton_tecla);     
    break;
  case LLENAR:
    mqprin_EstLLENAR          (T_actual, comando_LoRa, boton_tecla);      //Function of state: REPOSO.
    break;
  case ALARMAS:
    mqprin_EstALARMAS         (T_actual, comando_LoRa, boton_tecla);      //Function of state: REPOSO.
    break;
  case LLENAR_1A:
    mqprin_EstLLENAR_1A       (T_actual, comando_LoRa, boton_tecla);      //Function of state: REPOSO.
    break;
  case VACIAR:
    mqprin_EstVACIAR          (T_actual, comando_LoRa, boton_tecla);      //Function of state: REPOSO.
    break;
  case ESTIRARPERSIANA:
    mqprin_EstESTIRARPERSIANA (T_actual, comando_LoRa, boton_tecla, tecla_FCEstirar);//Function of state: ESTIRAR PERSIANA.
    break;
  case RECOGERPERSIANA:
    mqprin_EstRECOGERPERSIANA (T_actual, comando_LoRa, boton_tecla, tecla_FCRecoger);//Function of state: RECOGER PERSIANA.
    break;
  default:
      break;
  } 
  maqsecEstado = maqsec_RevisionCondiciones (maqsecEstado);    //Revisión de las condiciones en función del estado en el que se encuentra la máquina de estados.
  memset (condicionsec,0,sizeof(condicionsec));               //Resetea todas las condiciones de salto.
}

//------------------------------------------------------------------------------
// FUNCTION: static int mqest_RevisionCondiciones (int Estado)
// DESCRIPTION: This function is the one that really is the responsable to make 
// the jump between states.
// PARAMETERS: Actual state.
// RETURNS:  Next state.
//------------------------------------------------------------------------------
static int maqsec_RevisionCondiciones (int Estado) {
    if (*(condicionsec + CS0) == true) return REPOSO;
    if (*(condicionsec + CS1) == true) return VINCULAR;
    //if (*(condicionsec + CS2) == true) return PING;
    if (*(condicionsec + CS3) == true) return LLENAR;
    if (*(condicionsec + CS4) == true) return ALARMAS;
    if (*(condicionsec + CS5) == true) return LLENAR_1A;
    if (*(condicionsec + CS6) == true) return VACIAR;
    if (*(condicionsec + CS7) == true) return ESTIRARPERSIANA;
    if (*(condicionsec + CS8) == true) return RECOGERPERSIANA;
    //if (*(condicionsec + CS9) == true) return ;
    //if (*(condicionsec + CS10) == true) return ;
    return Estado;  //If it is not completed anything return the same state.
} 

//------------------------------------------------------------------------------
// FUNCTION: int mqest_CheckEstado (void)
// DESCRIPTION: This function is called from the out of file and return the
// actual state.
// PARAMETERS: Void.
// RETURNS:  Actual state.
//------------------------------------------------------------------------------
int maqest_CheckEstado (void) {
    return maqsecEstado;
}

//------------------------------------------------------------------------------
// FUNCTION: static void mqest_IniciacionVbles (void)
// DESCRIPTION: This function is the one that intiate the system.
// PARAMETERS: Void.
// RETURNS:  Void.
//------------------------------------------------------------------------------
void maqsec_IniciacionVbles (void) {
    memset(condicionsec,0,sizeof(condicionsec));//Inicia las condiciones a CERO.
    maqsec_ResetFlag1A ();                      //Inicia este flag. 
    maqsecEstado = INICIAL;                 //STATE 0 (Initial).
    est_anterior = -1;                          //It is a way to initial it.
} 

//------------------------------------------------------------------------------
// FUNCTION: void mqest_ActivarCondicion (UBYTE mensaje)
// DESCRIPTION: This function is the one that actives one condition in function
// of message.
// PARAMETERS: UBYTE mensaje.
// RETURNS:  Void.
//------------------------------------------------------------------------------
void maqsec_ActivarCondicion (int N_Condicion) {
    *(condicionsec + N_Condicion) = true;
}


//------------------------------------------------------------------------------
// static void mqest_ResetFlag1A (void)                                        -
//------------------------------------------------------------------------------
// Function that reset the flag the first time that intro into some state.     -
//------------------------------------------------------------------------------

void maqsec_ResetFlag1A (void) {
    maqsec_bits.flag_1A_vez = false;
}
//------------------------------------------------------------------------------
// static void mqest_SetFlag1A (void)                                          -
//------------------------------------------------------------------------------
// Function that set the flag the first time that intro into some state.       -
//------------------------------------------------------------------------------
void maqsec_SetFlag1A (void) {
    maqsec_bits.flag_1A_vez = true;
}
