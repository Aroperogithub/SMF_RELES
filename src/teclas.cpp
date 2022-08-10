/********************************************************************************
*  Fichero: Teclas.c Modulo que integra el manejo de las teclas.                *
*********************************************************************************
*  Descripcion: En este módulo se gestionan todas las teclas del dispositivo    *
*********************************************************************************
*  Creacion del módulo: 07 de Octubre de 2017                                   *
*  Last modification:                                                           *
*  Name of creator of module: Ángel Ropero                                      *
*  Software Arduino.                                                            *
*  Enlaza con otros ficheros: mqprinc.c                                         *
*  Company: IMPOREXO                                                            *
********************************************************************************/
#include <advancedSerial.h>

#include "teclas.h"
#include "hardware.h"


//-------------------- GLOBAL VARIABLES ----------------------------------------
int Tecla;
unsigned long time_intermedio, time_tecla;
int Flag_1aVez_Tecla, Flag_1aVez_Timeout, Flag_1aVez_Cambio; 

int Tecla_F;
unsigned long time_tecla_F;
int Flag_1aVez_Tecla_F, Flag_1aVez_Timeout_F, Flag_1aVez_Cambio_F;

unsigned long timeout_ESTIRAR = 0;
bool Flag_1aVez_ESTIRAR = false;
unsigned long timeout_RECOGER = 0;
bool Flag_1aVez_RECOGER = false;

unsigned long time_test;
int contador_test = 0;
//-------------------- GLOBAL FUNCTIONS ----------------------------------------

/*void teclas_Iniciar_Variables (unsigned long TIME_actual) {
  Flag_1aVez_Tecla = false;
  Flag_1aVez_Timeout = false;
  Flag_1aVez_Cambio = false;
  time_tecla = TIME_actual;
  Tecla = BOTON_OFF;
}*/
void teclas_Iniciar_Variables_F (unsigned long TIME_actual) {
  Flag_1aVez_Tecla_F = false;
  Flag_1aVez_Timeout_F = false;
  Flag_1aVez_Cambio_F = false;
  time_tecla_F = TIME_actual;
  Tecla_F = BOTON_OFF;
}
int teclas_filtro_Teclado_FAVORITO (int teclas, unsigned long T_actual) {
  if (teclas == 0 && Flag_1aVez_Cambio_F == false) {                    //TECLA PULSADA.
    //Serial.print ("TECLA PULSADA: ");
    //Serial.println (T_actual - time_tecla_F);
    if (Flag_1aVez_Timeout_F == false) {  //TECLA PULSADA por primera vez.
      time_tecla_F = T_actual;            //Se captura el momento en el que se detecta la tecla por primera vez.
      Flag_1aVez_Timeout_F = true;        //Se almacena que ya se capturado el tiempo de pulsación.
      return BOTON_OFF;                      //Se sale sin hacer nada.
    } else {                            //Se ha pulsado tecla pero no es la primera vez, luego ya tengo el tiempo pasado desde primera pulsación.
      if ((T_actual - time_tecla_F) < TIMEOUT_FILTRO) {   //Si el tiempo transcurrido es menor que el del filtro, podría ser un glitch.
        return BOTON_OFF;
      } else {
        if ((T_actual - time_tecla_F) < TIMEOUT_MENSAJE) {    //El tiempo transcurrido es mayor que el del FILTRO pero menor que el del MENSAJE.
          Flag_1aVez_Tecla_F = true;                          //Activo el flag de activación de tecla.
          return BOTON_OFF;
        } else {
          if ((T_actual - time_tecla_F) < TIMEOUT_CAMBIO) {   //El tiempo transcurrido es mayor que el del MENSAJE pero menor que el del CAMBIO.
            return BOTON_MENSAJE;
          } else {
            Flag_1aVez_Cambio_F = true;
            time_tecla_F = T_actual;
            Serial.println ("BOTÓN 3 segundos");
            return BOTON_CAMBIO;
          }
        }
      }
    }
  } else if (teclas != 0) {                              //NO HAY TECLA PULSADA.
    if (Flag_1aVez_Cambio_F == true) {    //Si proviene de APAGAR/ENCENDER.
      teclas_Iniciar_Variables_F (T_actual);      //Se inician todas las variables.
      return BOTON_OFF;                      //No se hace nada.
    } else if (Flag_1aVez_Tecla_F == true) {   //No hay tecla pulsada, No viene de un TIMEOUT_CAMBIO, pero si de un TIMEOUT_FILTRO.
        Serial.println ("BOTÓN PULSADO");
        teclas_Iniciar_Variables_F (T_actual);    //Se inician todas las variables.
        return BOTON_ON;          //Se superó el TIMEOUT_FILTRO del filtro de tecla y hay que cambiar de color.
    }
    teclas_Iniciar_Variables_F (T_actual);    //Se inician todas las variables.
    return BOTON_OFF;
  }
  return BOTON_OFF;
}

int teclas_ESTIRAR (int teclas, unsigned long T_actual) {
  if (teclas == DETECCION_BOTON) {            //Se ha detectado BOMBA = ON.
    if (Flag_1aVez_ESTIRAR == false) {          //Primera vez después de BOMBA = ON.
      Flag_1aVez_ESTIRAR = true;                //Se setea el flag.
      timeout_ESTIRAR = T_actual;               //Se rescata el Timeout Actual.
    } else {                                  //Segunda y siguientes BOMBA = ON.
      if ((T_actual - timeout_ESTIRAR) >= TIMEOUT_FINALCARRERA) { //Si timeout de BOMBA mayor de esos milisegundos.
        Serial.println (F("FINAL CARRERA ESTIRAR"));
        return BOTON_ON;
      }
    }
  } else {                                  //Se ha detectado BOMBA = OFF.
    Flag_1aVez_ESTIRAR = false;               //Se resetea el flag.
  }
  return BOTON_OFF;
}
int teclas_RECOGER (int teclas, unsigned long T_actual) {
  if (teclas == DETECCION_BOTON) {            //Se ha detectado BOMBA = ON.
    if (Flag_1aVez_RECOGER == false) {          //Primera vez después de BOMBA = ON.
      Flag_1aVez_RECOGER = true;                //Se setea el flag.
      timeout_RECOGER = T_actual;               //Se rescata el Timeout Actual.
    } else {                                  //Segunda y siguientes BOMBA = ON.
      if ((T_actual - timeout_RECOGER) >= TIMEOUT_FINALCARRERA) { //Si timeout de BOMBA mayor de esos milisegundos.
        Serial.println (F("FINAL CARRERA RECOGER"));
        return BOTON_ON;
      }
    }
  } else {                                  //Se ha detectado BOMBA = OFF.
    Flag_1aVez_RECOGER = false;               //Se resetea el flag.
  }
  return BOTON_OFF;
}

/*
int teclas_filtro_Teclado_Timeout (int teclas, unsigned long T_actual) {
  
  if (teclas == 0) {                    //TECLA PULSADA.
    if (Flag_1aVez_Tecla == false) {  //TECLA PULSADA por primera vez.
      time_tecla = T_actual;            //Se captura el momento en el que se detecta la tecla por primera vez.
      Flag_1aVez_Tecla = true;        //Se almacena que ya se capturado el tiempo de pulsación.
      return BOTON_OFF;                      //Se sale sin hacer nada.
    } else {                            //Se ha pulsado tecla pero no es la primera vez, luego ya tengo el tiempo pasado desde primera pulsación.
      if (abs(T_actual - time_tecla) < TIMEOUT_FILTRO) {   //Si el tiempo transcurrido es menor que el del filtro, podría ser un glitch.
        return BOTON_OFF;
      } else {
        Flag_1aVez_Timeout = true;                          //Activo el flag de activación de tecla.
        return BOTON_OFF;
      }
    }
  } else if (teclas != 0 && Flag_1aVez_Timeout == true) {                              //NO HAY TECLA PULSADA.
    //Serial.println (F("ACTIVAR TECLA"));
    teclas_Iniciar_Variables (T_actual);    //Se inician todas las variables.
    return BOTON_ON;          //Se superó el TIMEOUT_FILTRO del filtro de tecla y hay que cambiar de color.
  }
  teclas_Iniciar_Variables (T_actual);    //Se inician todas las variables.
  return BOTON_OFF;
}*/


