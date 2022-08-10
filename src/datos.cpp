/********************************************************************************
*  Fichero: datos.c  Modulo que integra la estructura de datos del programa     *
*********************************************************************************
*  Descripcion: En este módulo se definen las funciones que manejan la EEPROM   *
*              usada por las diferentes funciones en el programa.               *
*********************************************************************************
*  Creacion del módulo: 30 de Septiembre de 2017                                *
*  Last modification:                                                           *
*  Name of creator of module: Ángel Ropero                                      *
*  Software Arduino.                                                            *
*  Enlaza con otros ficheros: mqprinc.c                                         *
*  Company: IMPOREXO                                                            *
********************************************************************************/
#include <advancedSerial.h>
#include <EEPROM.h>     //Incluye la librería de memoria EEPROM de la placa de ARDUINO UNO.
#include <string.h>
#include <stdlib.h>
#include <Wire.h>

#include "datos.h"
#include "hardware.h"

//#define DEBUG(a) Serial.println(a);

//DEFINICIÓN DE VARIABLES GLOBALES

//DEFINICIÓN DE FUNCIONES LOCALES AL MÓDULO

int datos_IniciarEEPROM (void) {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println(F("Failed to initialise EEPROM")); 
    return EEPROM_KO;
  }
  Serial.println (F("EEPROM Initialized properly"));
  return EEPROM_OK;
}

unsigned int datos_EEPROM_Read (int tipo) {
  //int datos = 0;
  
  int datos = (int) (((byte)EEPROM.read(tipo) << 8) | (byte)EEPROM.read(tipo + 1));
  //Serial.print ("TIPO-R: ");
  //Serial.println (tipo);
  //Serial.print ("DATOS MSB-R: ");
  //Serial.println ((byte) (EEPROM.read(tipo)), HEX);
  //Serial.print ("DATOS LSB-R: ");
  //Serial.println ((byte) (EEPROM.read(tipo + 1)), HEX);
  return datos;
}

void datos_EEPROM_Read_Memoria (int inicioEEPROM, int finalEEPROM)  {
  byte dato;

  Serial.println ("Datos EEPROM: ");
  for (int i = inicioEEPROM; i <= finalEEPROM; i++) {
    dato = 0;
    dato = (byte) EEPROM.read(i);
    Serial.print (i);
    Serial.print (": ");
    Serial.print (dato, HEX);
    Serial.print (" ");
    if ((i % 4) == 0) Serial.println ();
  }
  Serial.println ();
}


void datos_Reset_MemoriaEEPROM (void) {
  EEPROM.write (DAT_MAX_DATOS, 0x00);
  EEPROM.commit();
  Serial.println (F("--- MEMORIA RESETEADA - DAT_MAX_DATOS = 0x00. ---"));
}
void datos_Reset_Parcial_MemoriaEEPROM (int INI_DATOS, int FIN_DATOS) {
  Serial.print ("PARCIAL MEM EEPROM: ");
  for (int i = INI_DATOS; i < FIN_DATOS; i++) {
    Serial.print (i);
    Serial.print (", ");
    EEPROM.write(i, 0x00);
  }
  Serial.println ();
  EEPROM.commit();
}

/**********************************************************************************
 * FUNCIONES DE ESCRITURA/LECTURA DE MEMORIA POR TIPOS DE DATOS: BYTE, INT, LONG
 **********************************************************************************/
byte datos_EEPROM_Read_BYTE (int tipo) {
  byte dato = 0;

  dato = (byte) EEPROM.read(tipo);
  return dato;
}
void datos_EEPROM_Write_BYTE (byte datos, int tipo) {
  EEPROM.write (tipo, datos);
  EEPROM.commit();
}
int datos_EEPROM_Read_INT (int num) {
  int unitId = 0;

  unitId = (int)EEPROM.read(num + 0) << 8 | (int)EEPROM.read(num + 1);
  return unitId;
}

void datos_EEPROM_Write_INT (int datos, int tipo) {
  //Serial.print ("TIPO-W: ");
  //Serial.println (tipo);
  //Serial.print ("DATOS MSB-W: ");
  //Serial.println ((byte) (datos >> 8), HEX);
  //Serial.print ("DATOS LSB-W: ");
  //Serial.println ((byte) datos, HEX);
  EEPROM.write (tipo, (byte) (datos >> 8));
  EEPROM.write (tipo + 1,(byte) datos);
  EEPROM.commit();
}
long datos_EEPROM_Read_LONG (int num) {
  long unitId = 0;

  unitId = (long)EEPROM.read(num + 0) << 24 | (long)EEPROM.read(num + 1) << 16 | (long)EEPROM.read(num + 2) << 8 | (long)EEPROM.read(num + 3);
  return unitId;
}
void datos_EEPROM_Write_LONG (long datos, int tipo) {
    EEPROM.write(tipo + 0, (byte) (datos >> 24));
    EEPROM.write(tipo + 1, (byte) (datos >> 16));
    EEPROM.write(tipo + 2, (byte) (datos >> 8));
    EEPROM.write(tipo + 3, (byte) datos);
    EEPROM.commit();
}

void datos_Parametrizar_MemoriaEEPROM (void) {
  if (EEPROM.read (DAT_MAX_DATOS) == 0xAA) {
    Serial.println ("Memoria EEPROM ya parametrizada");
    return;
  }
  Serial.println ("Parametrizacion Memoria EEPROM");
  for (int i = DAT_SN_RECEIVER; i < DAT_MAX_DATOS; i++) {
    EEPROM.write(i, 0x00);
  }
  datos_EEPROM_Write_BYTE ((byte) EEMPROM_DESVIACION_MIN,       DAT_DESVIACION);
  datos_EEPROM_Write_BYTE ((byte) EEMPROM_DESVIACION_MIN,       DAT_DESVIACION2);
  datos_EEPROM_Write_INT ((int)   EEMPROM_NIVEL_SECO,           DAT_NIVEL_SECO);
  datos_EEPROM_Write_INT ((int)   EEMPROM_NIVEL_SECO,           DAT_NIVEL_SECO_2);
  datos_EEPROM_Write_INT ((int)   EEMPROM_NIVEL_OPTIMO,         DAT_NIVEL_OPTIMO);
  datos_EEPROM_Write_INT ((int)   EEMPROM_NIVEL_OPTIMO,         DAT_NIVEL_OPTIMO_2);
  datos_EEPROM_Write_INT ((int)   EEMPROM_TIMEPISCINA,          DAT_TIME_PISCINA);
  datos_EEPROM_Write_INT ((int)   EEMPROM_TIMEPISCINA,          DAT_TIME_PISCINA_2);
  datos_EEPROM_Write_INT ((int)   EEMPROM_NUMLLENADOS,          DAT_N_LLENADOS);
  datos_EEPROM_Write_INT ((int)   EEMPROM_TIME_1A,              DAT_TIME_LLENAR_1A);
  datos_EEPROM_Write_INT ((int)   EEMPROM_TIME_1A,              DAT_TIME_LLENAR_1A_2);
  datos_EEPROM_Write_INT ((int)   EEMPROM_TIME_PULSO_PERSIANA,  DAT_PULSO_PERSIANA);
  #ifdef SMARTFILL_PREMIUM
    datos_EEPROM_Write_BYTE ((byte) 1, DAT_LLENAR_ON);
    datos_EEPROM_Write_BYTE ((byte) 1, DAT_VACIAR_ON);
    datos_EEPROM_Write_BYTE ((byte) 1, DAT_PERSIANAS_ON);
    datos_EEPROM_Write_BYTE ((byte) 1, DAT_LLENAR_ON_2);
  #endif
  #ifdef SMARTFILL_BASIC
    datos_EEPROM_Write_BYTE ((byte) 1, DAT_LLENAR_ON);
    datos_EEPROM_Write_BYTE ((byte) 1, DAT_LLENAR_ON_2);
    datos_EEPROM_Write_BYTE ((byte) 0, DAT_VACIAR_ON);
    datos_EEPROM_Write_BYTE ((byte) 0, DAT_PERSIANAS_ON);
  #endif
  EEPROM.write (DAT_MAX_DATOS, 0xAA);
  EEPROM.commit();
}
void datos_Parametros_FabricaEEPROM (void) {
  Serial.println ("Parametrizacion FORZADA: EEPROM");
  EEPROM.write(DAT_MAX_DATOS, 0x00);
  EEPROM.commit();
  delay(10);
  datos_Parametrizar_MemoriaEEPROM ();
}

int datos_Comprobar_Incongruencia_de_datos (void) {
  int temp_seco = 0, temp_seco2 = 0, temp_optimo = 0, temp_optimo2 = 0;
  int temp1 = 0, temp2 = 0;
  int temp_incon = 0;

  temp_incon =  (int) datos_EEPROM_Read_BYTE (DAT_INCONGRUENCIA); //Contador de las veces que se han detectado INCONGRUENCIAS en los datos.
  /******* ZONA 1: INICIO - PROBADA ******/
  temp_seco = datos_EEPROM_Read_INT (DAT_NIVEL_SECO);
  temp_optimo = datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO);
  if (temp_seco <= temp_optimo) {
    temp_incon = DATOS_ALARMA_INCONGRUENCIA;        //Esto es CRÍTICO, por lo que se pone directamente al máximo del nº de veces permitidas.
    datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
    if (temp_incon >= DATOS_ALARMA_INCONGRUENCIA) { //Si el contador de INCONGRUECIAS es mayor que el Máximo permitido...
      Serial.println (F("---EEPROM: Nº INCONGRUENCIAS SUPERADAS -> SECO < OPTIMO."));
      return DATOS_EEPROM_ALARMA;                   // se sale de la función para ir al estado de alarmas.
    }
  }
  /******* ZONA 1: FIN - PROBADA******/
  /******* ZONA 2: INICIO-> SECO - PROBADA******/
  temp_seco = datos_EEPROM_Read_INT (DAT_NIVEL_SECO);
  temp_seco2 = datos_EEPROM_Read_INT (DAT_NIVEL_SECO_2);
  if (temp_seco != temp_seco2) {   //Si se detectan que son diferentes es porque algo lo ha alterado.
    Serial.println (F("---EEPROM: Error NIVEL SECO. 1a vez---"));
    temp_incon++;         //Se aumenta en 1 el contador de INCONGRUENCIAS.
    if (temp_incon >= DATOS_ALARMA_INCONGRUENCIA) { //Si el contador de INCONGRUECIAS es mayor que el Máximo permitido...
      Serial.println (F("---EEPROM: Nº INCONGRUENCIAS SUPERADAS -> SECO."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;                   // se sale de la función para ir al estado de alarmas.
    } else if (temp_seco == 0) {    //Si el 1 es el que se ha borrado, le sustituimos por el 2.
      datos_EEPROM_Write_INT ((int) temp_seco2, DAT_NIVEL_SECO);
      Serial.println (F("---EEPROM: SECO = 0."));
    } else if (temp_seco2 == 0) {    //Si el 2 es el que se ha borrado, le sustituimos por el 1.
      datos_EEPROM_Write_INT ((int) temp_seco, DAT_NIVEL_SECO_2);
      Serial.println (F("---EEPROM: SECO_2 = 0."));
    } else {                        //Este caso sería aquel en el que se leerían datos diferentes sin ser 0. ¿A ver a cual hago caso?
      Serial.println (F("---EEPROM: SECO != SECO_2 && != 0."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;                   // se sale de la función para ir al estado de alarmas.
    }
    //VOLVER A LEER A VER SI SE HA ALMACENADO CORRECTAMENTE.
    temp_seco = datos_EEPROM_Read_INT (DAT_NIVEL_SECO);
    temp_seco2 = datos_EEPROM_Read_INT (DAT_NIVEL_SECO_2);
    if (temp_seco != temp_seco2) {   //Si se detectan que son diferentes es porque NO SE ESTÁN GRABANDO CORRECTAMENTE y alguna posición de la memoria tiene algún problema.
      Serial.println (F("---EEPROM: SECO != SECO_2. 2a vez---"));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    }
    datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
  }
  /******* ZONA 2: FIN -> SECO - PROBADA******/
  /******* ZONA 3: INICIO -> OPTIMO - PROBADA******/
  temp_optimo = datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO);
  temp_optimo2 = datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO_2);
  if (temp_optimo != temp_optimo2) {
    Serial.println (F("---EEPROM: Error NIVEL OPTIMO. 1a vez---"));
    temp_incon++;
    if (temp_incon >= DATOS_ALARMA_INCONGRUENCIA) {
      Serial.println (F("---EEPROM: Nº INCONGRUENCIAS SUPERADAS -> OPTIMO."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    } else if (temp_optimo == 0) {    //Si el 1 es el que se ha borrado, le sustituimos por el 2.
      datos_EEPROM_Write_INT ((int) temp_optimo2, DAT_NIVEL_OPTIMO);
      Serial.println (F("---EEPROM: OPTIMO = 0."));
    } else if (temp_optimo2 == 0) {    //Si el 2 es el que se ha borrado, le sustituimos por el 1.
      datos_EEPROM_Write_INT ((int) temp_optimo, DAT_NIVEL_OPTIMO_2);
      Serial.println (F("---EEPROM: OPTIMO2 = 0."));
    } else {                        //Este caso sería aquel en el que se leerían datos diferentes sin ser 0.
      Serial.println (F("---EEPROM: OPTIMO != OPTIMO_2 && != 0."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;                   // se sale de la función para ir al estado de alarmas.
    }
    //VOLVER A LEER A VER SI SE HA ALMACENADO CORRECTAMENTE.
    temp_optimo = datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO);
    temp_optimo2 = datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO_2);
    if (temp_optimo != temp_optimo2) {   //Si se detectan que son diferentes es porque NO SE ESTÁN GRABANDO CORRECTAMENTE y alguna posición de la memoria tiene algún problema.
      Serial.println (F("---EEPROM: OPTIMO != OPTIMO_2. 2a vez---"));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    }
    datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA);
  }
  /******* ZONA 3: FIN -> OPTIMO - PROBADA******/
  /******* ZONA 4: INICIO -> DAT_TIME_PISCINA ******/
  temp1 = datos_EEPROM_Read_INT (DAT_TIME_PISCINA);
  temp2 = datos_EEPROM_Read_INT (DAT_TIME_PISCINA_2);
  if (temp1 != temp2) {
    Serial.println (F("---EEPROM: Error DAT_TIME_PISCINA. 1a vez---"));
    temp_incon++;
    if (temp_incon >= DATOS_ALARMA_INCONGRUENCIA) {
      Serial.println (F("---EEPROM: Nº INCONGRUENCIAS SUPERADAS -> TIME_PISCINA."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    } else if (temp1 == 0) {    //Si el 1 es el que se ha borrado, le sustituimos por el 2.
      Serial.println (F("---EEPROM: DAT_TIME_PISCINA = 0."));
      datos_EEPROM_Write_INT ((int) temp2, DAT_TIME_PISCINA);
    } else if (temp2 == 0) {    //Si el 2 es el que se ha borrado, le sustituimos por el 1.
      Serial.println (F("---EEPROM: DAT_TIME_PISCINA_2 = 0."));
      datos_EEPROM_Write_INT ((int) temp1, DAT_TIME_PISCINA_2);
    } else {                        //Este caso sería aquel en el que se leerían datos diferentes sin ser 0.
      Serial.println (F("---EEPROM: DAT_TIME_PISCINA != DAT_TIME_PISCINA_2 && != 0."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;                   // se sale de la función para ir al estado de alarmas.
    }
    //VOLVER A LEER A VER SI SE HA ALMACENADO CORRECTAMENTE.
    temp1 = datos_EEPROM_Read_INT (DAT_TIME_PISCINA);
    temp2 = datos_EEPROM_Read_INT (DAT_TIME_PISCINA_2);
    if (temp1 != temp2) {   //Si se detectan que son diferentes es porque NO SE ESTÁN GRABANDO CORRECTAMENTE y alguna posición de la memoria tiene algún problema.
      Serial.println (F("---EEPROM: Error DAT_TIME_PISCINA. 2a vez---"));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    }
    datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA);
  }
  /******* ZONA 4: FIN -> DAT_TIME_PISCINA ******/
  /******* ZONA 5: INICIO ******/
  temp1 = datos_EEPROM_Read_INT (DAT_TIME_LLENAR_1A);
  temp2 = datos_EEPROM_Read_INT (DAT_TIME_LLENAR_1A_2);
  if (temp1 != temp2) {
    Serial.println (F("---EEPROM: Error DAT_TIME_LLENAR_1A. 1a vez---"));
    temp_incon++;
    if (temp_incon >= DATOS_ALARMA_INCONGRUENCIA) {
      Serial.println (F("---EEPROM: Nº INCONGRUENCIAS SUPERADAS -> TIME_LLENAR_1A."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    } else if (temp1 == 0) {    //Si el 1 es el que se ha borrado, le sustituimos por el 2.
      Serial.println (F("---EEPROM: DAT_TIME_LLENAR_1A = 0."));
      datos_EEPROM_Write_INT ((int) temp2, DAT_TIME_LLENAR_1A);
    } else if (temp2 == 0) {    //Si el 2 es el que se ha borrado, le sustituimos por el 1.
      Serial.println (F("---EEPROM: DAT_TIME_LLENAR_1A_2 = 0."));
      datos_EEPROM_Write_INT ((int) temp1, DAT_TIME_LLENAR_1A_2);
    } else {                        //Este caso sería aquel en el que se leerían datos diferentes sin ser 0.
      Serial.println (F("---EEPROM: DAT_TIME_LLENAR_1A != DAT_TIME_LLENAR_1A_2 && != 0."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;                   // se sale de la función para ir al estado de alarmas.
    }
    temp1 = datos_EEPROM_Read_INT (DAT_TIME_LLENAR_1A);
    temp2 = datos_EEPROM_Read_INT (DAT_TIME_LLENAR_1A_2);
    if (temp1 != temp2) {
      Serial.println (F("---EEPROM: Error DAT_TIME_LLENAR_1A. 2a vez---"));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    }
    datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA);
  }
  /******* ZONA 5: FIN ******/
  /******* ZONA 6: INICIO ******/
  temp1 =  (int) datos_EEPROM_Read_BYTE (DAT_LLENAR_ON);
  if (temp1 != 1) {
    Serial.println (F("---EEPROM: Error DAT_LLENAR_ON. 1a vez---"));
    temp_incon++;
    if (temp_incon >= DATOS_ALARMA_INCONGRUENCIA) {
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    } else {    //Lo cargamos con el valor 1.
      datos_EEPROM_Write_BYTE ((byte) 1, DAT_LLENAR_ON);
    }
    temp1 =  (int) datos_EEPROM_Read_BYTE (DAT_LLENAR_ON);
    if (temp1 != 1) {
      Serial.println (F("---EEPROM: Error DAT_LLENAR_ON. 2a vez---"));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    }
  }
  /******* ZONA 6: FIN -> LLENAR - PROBADA******/
  /******* ZONA 7: INICIO -> DESVIACIÓN - PROBADA******/
  temp_optimo   = datos_EEPROM_Read_BYTE (DAT_DESVIACION);
  temp_optimo2  = datos_EEPROM_Read_BYTE (DAT_DESVIACION2);
  if (temp_optimo != temp_optimo2) {
    Serial.println (F("---EEPROM: Error DESVIACIÓN. 1a vez---"));
    temp_incon++;
    if (temp_incon >= DATOS_ALARMA_INCONGRUENCIA) {
      Serial.println (F("---EEPROM: Nº INCONGRUENCIAS SUPERADAS -> DESVIACIÓN."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    } else if (temp_optimo == 0) {    //Si el 1 es el que se ha borrado, le sustituimos por el 2.
      datos_EEPROM_Write_BYTE ((byte) temp_optimo2, DAT_DESVIACION);
      Serial.println (F("---EEPROM: DESVIACIÓN = 0."));
    } else if (temp_optimo2 == 0) {    //Si el 2 es el que se ha borrado, le sustituimos por el 1.
      datos_EEPROM_Write_BYTE ((byte) temp_optimo, DAT_DESVIACION2);
      Serial.println (F("---EEPROM: DESVIACIÓN = 0."));
    } else {                        //Este caso sería aquel en el que se leerían datos diferentes sin ser 0.
      Serial.println (F("---EEPROM: DESVIACIÓN != DESVIACIÓN2 && != 0."));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;                   // se sale de la función para ir al estado de alarmas.
    }
    //VOLVER A LEER A VER SI SE HA ALMACENADO CORRECTAMENTE.
    temp_optimo   = datos_EEPROM_Read_BYTE (DAT_DESVIACION);
    temp_optimo2  = datos_EEPROM_Read_BYTE (DAT_DESVIACION2);
    if (temp_optimo != temp_optimo2) {   //Si se detectan que son diferentes es porque NO SE ESTÁN GRABANDO CORRECTAMENTE y alguna posición de la memoria tiene algún problema.
      Serial.println (F("---EEPROM: DESVIACIÓN != DESVIACIÓN. 2a vez---"));
      temp_incon = DATOS_ALARMA_INCONGRUENCIA;
      datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA); // y se graba el contador.
      return DATOS_EEPROM_ALARMA;
    }
    datos_EEPROM_Write_BYTE ((byte) temp_incon, DAT_INCONGRUENCIA);
  }
  /******* ZONA 7: FIN ******/
  return DATOS_EEPROM_BIEN;
}
