/*** Includes ***/
#include <string.h>
#include <stdlib.h>
#include <advancedSerial.h>

#include "mqsec_aux.h"
#include "mq_sec.h"
#include "maqsec.h"
#include "datos.h"
#include "hardware.h"
#include "comanpar.h"
#include "Electrovalv.h"
#include "Lora_imp.h"
#include "protocolo.h"

extern DATEEPROM maqDatosEEPROM;
extern int reg_alarmas;
int num_reset_laser = 0;
int array_niveles_RELES[N_NIVELES], contador_niveles, contador_outliers = 0;
int media_outlier_buena, media_outlier_erronea;
//float desviacion_niveles;

float nivel_reles, media_reles, desviacion_reles;
float media_reles_def, desviacion_reles_def;


void mqsec_aux_Reset_SNRECEIVER (void) {
  Serial.println (F("ATENCIÓN!!! -> BORRADO DEL SN_RECEIVER DE LA EEPROM."));
  for (int i = 0; i < DATOS_NUMERO_SERIE; i++) {
    datos_EEPROM_Write_BYTE (0, DAT_SN_RECEIVER + i);   //GRABA EN LA EEPROM, EN LA POSICIÓN RECEIVER, EL SN-SENDER (CABECERA).
  }
}

int mqsec_aux_Chech_SN_PCBRELES (void) {
  byte valor_EEPROM[11];

  memset (valor_EEPROM, 0, sizeof(valor_EEPROM));
  Serial.print (F("El S/N de la PCB_CABEZA es: "));
  for (int i = 0; i < DATOS_NUMERO_SERIE; i++) {
    valor_EEPROM[i] = datos_EEPROM_Read_BYTE (DAT_SN_RECEIVER + i);
    Serial.print ((char) valor_EEPROM[i]);
  }
  Serial.println ();
  if (valor_EEPROM[0] != 'S' || valor_EEPROM[1] != 'M' || valor_EEPROM[2] != 'F' || valor_EEPROM[3] != 'C') {
    Serial.println (F("¡¡¡¡¡NO HAY S/N PCB RELES!!!!!"));
    return 0;
  } else {
    return 1;
  }
}
void mqsed_aux_Enviar_COMANDO_LORA (int Comando_Enviar) {
  char comando[N_MAXIMO_CARACTERES_MENSAJE];
  char mensaje_local[N_MAXIMO_CARACTERES_MENSAJE];

  memset (comando, 0, sizeof(comando));
  memset (mensaje_local, 0, sizeof(mensaje_local));
  switch (Comando_Enviar) {
  case LORA_ACK:
    strcpy (comando, "#ACK*");
    break;
  case LORA_PING:
    strcpy (comando, "#PING*");
    break;
  case LORA_VINCULAR:
    strcpy (comando, "#VINCULAR*");
    break;
  case LORA_TEST_NIVELES:
    strcpy (comando, "#TESTNIVELES*");
    break;
  case LORA_VINCULACION_FORZOSA:
    strcpy (comando, "#VINC_FORZADA*");
    break;
  case LORA_EXTENDER_TIMEOUT:
    strcpy (comando, "#EXT_TIMEOUT*");
    break;
  default:
    return;
    break;
  }
  int total_lenght = protocolo_Mensaje_PCBRELES (comando, strlen(comando), mensaje_local);
  Lora_imp_SendPacket (mensaje_local, total_lenght);
}

void mqsec_LeerEEPROM (byte tipo_datos, byte mostrar) {
  //char buffer_int[35];

  maqDatosEEPROM.desviacion             = datos_EEPROM_Read_BYTE (DAT_DESVIACION);
  maqDatosEEPROM.llenado_ON             = datos_EEPROM_Read_INT (DAT_LLENADO_ON);
  maqDatosEEPROM.N_llenados             = datos_EEPROM_Read_INT (DAT_N_LLENADOS);
  maqDatosEEPROM.altura_pool            = datos_EEPROM_Read_BYTE (DAT_ALTURA);
  maqDatosEEPROM.nivel_optimo           = datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO);
  maqDatosEEPROM.nivel_seco             = datos_EEPROM_Read_INT (DAT_NIVEL_SECO);
  maqDatosEEPROM.time_llenar_1A         = datos_EEPROM_Read_INT (DAT_TIME_LLENAR_1A);
  maqDatosEEPROM.time_piscina           = datos_EEPROM_Read_INT (DAT_TIME_PISCINA);
  maqDatosEEPROM.medida_maxlaser        = datos_EEPROM_Read_BYTE (DAT_MEDIDA_MAXLASER);
  maqDatosEEPROM.pulso_persiana         = datos_EEPROM_Read_INT (DAT_PULSO_PERSIANA);
  maqDatosEEPROM.superficie_pool        = (long) datos_EEPROM_Read_LONG (DAT_SUPERFICIE);
  if (tipo_datos == DATOS_SOLO_NIVELES) {             //Esto es simplemente para ahorrar tiempo en la lectura de los datos de la EEPROM.
    return;
  }
  /*DATOS STRING: SSID, PASSWORD, USER, DEVICE, CREDENTIALS*/
  memset (maqDatosEEPROM.ssid, 0, sizeof(maqDatosEEPROM.ssid));
  for (int n_datos = 0; n_datos <= (DAT_PASSWRD - DAT_SSID); n_datos++) {
    maqDatosEEPROM.ssid[n_datos] = datos_EEPROM_Read_BYTE (DAT_SSID + n_datos);
    if (maqDatosEEPROM.ssid[n_datos] == 0) break;
  }
  memset (maqDatosEEPROM.passwrd, 0, sizeof(maqDatosEEPROM.passwrd));
  for (int n_datos = 0; n_datos <= (DAT_USER_THINGERIO - DAT_PASSWRD); n_datos++) {
    maqDatosEEPROM.passwrd[n_datos] = datos_EEPROM_Read_BYTE (DAT_PASSWRD + n_datos);
    if (maqDatosEEPROM.passwrd[n_datos] == 0) break;
  }
  memset (maqDatosEEPROM.user_thingerio, 0, sizeof(maqDatosEEPROM.user_thingerio));
  for (int n_datos = 0; n_datos <= (DAT_DEVICE_ID_THINGERIO - DAT_USER_THINGERIO); n_datos++) {
    maqDatosEEPROM.user_thingerio[n_datos] = datos_EEPROM_Read_BYTE (DAT_USER_THINGERIO + n_datos);
    if (maqDatosEEPROM.user_thingerio[n_datos] == 0) break;
  }
  memset (maqDatosEEPROM.device_thingerio, 0, sizeof(maqDatosEEPROM.device_thingerio));
  for (int n_datos = 0; n_datos <= (DAT_CREDENTIALS_THINGERIO - DAT_DEVICE_ID_THINGERIO); n_datos++) {
    maqDatosEEPROM.device_thingerio[n_datos] = datos_EEPROM_Read_BYTE (DAT_DEVICE_ID_THINGERIO + n_datos);
    if (maqDatosEEPROM.device_thingerio[n_datos] == 0) break;
  }
  memset (maqDatosEEPROM.credentials_thingerio, 0, sizeof(maqDatosEEPROM.credentials_thingerio));
  for (int n_datos = 0; n_datos <= (DAT_CODIGO_POSTAL - DAT_CREDENTIALS_THINGERIO); n_datos++) {
    maqDatosEEPROM.credentials_thingerio[n_datos] = datos_EEPROM_Read_BYTE (DAT_CREDENTIALS_THINGERIO + n_datos);
    if (maqDatosEEPROM.credentials_thingerio[n_datos] == 0) break;
  }
  memset (maqDatosEEPROM.codigo_postal, 0, sizeof(maqDatosEEPROM.codigo_postal));
  for (int n_datos = 0; n_datos <= ((DAT_SSID2 - 1) - DAT_CODIGO_POSTAL); n_datos++) {
    maqDatosEEPROM.codigo_postal[n_datos] = datos_EEPROM_Read_BYTE (DAT_CODIGO_POSTAL + n_datos);
    if (maqDatosEEPROM.codigo_postal[n_datos] == 0) break;
  }
  memset (maqDatosEEPROM.ssid2, 0, sizeof(maqDatosEEPROM.ssid2));
  for (int n_datos = 0; n_datos <= ((DAT_PASSWRD2 - 1) - DAT_SSID2); n_datos++) {
    maqDatosEEPROM.ssid2[n_datos] = datos_EEPROM_Read_BYTE (DAT_SSID2 + n_datos);
    if (maqDatosEEPROM.ssid2[n_datos] == 0) break;
  }
  memset (maqDatosEEPROM.passwrd2, 0, sizeof(maqDatosEEPROM.passwrd2));
  for (int n_datos = 0; n_datos <= ((DAT_MAX_DATOS - 1) - DAT_PASSWRD2); n_datos++) {
    maqDatosEEPROM.passwrd2[n_datos] = datos_EEPROM_Read_BYTE (DAT_PASSWRD2 + n_datos);
    if (maqDatosEEPROM.passwrd2[n_datos] == 0) break;
  }
  //Serial.println (buffer_int);
  if (mostrar == MOSTRAR_DATOS) {
    Serial.print (F("DESVIACIÓN: "));
    Serial.println (maqDatosEEPROM.desviacion);
    Serial.print (F("NIVEL OPTIMO: "));
    Serial.println (maqDatosEEPROM.nivel_optimo);
    Serial.print (F("NIVEL SECO: "));
    Serial.println (maqDatosEEPROM.nivel_seco);
    Serial.print (F("NIVEL MAX-LASER: "));
    Serial.println (maqDatosEEPROM.medida_maxlaser);
    Serial.print (F("NIVEL PULSO PERSIANA: "));
    Serial.println (maqDatosEEPROM.pulso_persiana);
    Serial.print (F("WIFI SSID: "));
    Serial.println (maqDatosEEPROM.ssid);
    Serial.print (F("WIFI PASSWORD: "));
    Serial.println (maqDatosEEPROM.passwrd);
    Serial.print (F("WIFI SSID2: "));
    Serial.println (maqDatosEEPROM.ssid2);
    Serial.print (F("WIFI PASSWORD2: "));
    Serial.println (maqDatosEEPROM.passwrd2);
    Serial.print (F("USER THINGER: "));
    Serial.println (maqDatosEEPROM.user_thingerio);
    Serial.print (F("DEVICE THINGER: "));
    Serial.println (maqDatosEEPROM.device_thingerio);
    Serial.print (F("CREDENTIALS THINGER: "));
    Serial.println (maqDatosEEPROM.credentials_thingerio);
    Serial.print (F("CODIGO POSTAL: "));
    Serial.println (maqDatosEEPROM.codigo_postal);
  }
}

void mqsec_Gestionar_ALARMAS (unsigned long T_actual) {
  int reg_alarmas_aux = reg_alarmas & ALARMA_NIVELES;
  if (reg_alarmas_aux == ALARMA_NIVELES) {
    mqsec_LeerEEPROM (DATOS_SOLO_NIVELES, NO_MOSTRAR_DATOS);
    if (maqDatosEEPROM.nivel_optimo < maqDatosEEPROM.nivel_seco) {
      reg_alarmas &= ALARMA_NIVELES_C1;      //bit 3 del registro. Se deja que sea el estado de REPOSO quien verifique las alarmas y envíe el dispositivo a tal estado.
      Serial.println (F("--- ATENCIÓN --- LOS NIVELES DE OPTIMO Y SECO ESTÁN CORRECTOS."));
      return;
    }
    Serial.println (F("--- ALARMA: NIVELES OPTIMO Y SECO CRUZADOS. ---"));
  }
  reg_alarmas_aux = reg_alarmas & ALARMA_CABEZAL;
  if (reg_alarmas_aux == ALARMA_CABEZAL) {
    Serial.println (F("--- ALARMA: NO SE RECIBEN COMANDOS DESDE LA PCB_CABEZAL. ---"));
  }
  reg_alarmas_aux = reg_alarmas & ALARMA_INUNDACION;
  if (reg_alarmas_aux == ALARMA_INUNDACION) {
    Serial.println (F("--- ALARMA: INUNDACIÓN. ---"));
  }
  reg_alarmas_aux = reg_alarmas & ALARMA_LLENADO;
  if (reg_alarmas_aux == ALARMA_LLENADO) {
    int time_piscina_aux = maqDatosEEPROM.time_piscina;     //Guardamos una copia de lo que había antes.
    maqDatosEEPROM.time_piscina = datos_EEPROM_Read_INT (DAT_TIME_PISCINA);  //Volvemos a leer la variable de la EEPROM.
    if (maqDatosEEPROM.time_piscina > time_piscina_aux) {   //Comparamos si ha cambiado a un valor mayor.
      reg_alarmas &= ALARMA_LLENADO_C1;        //Esto es para anular la alarma.
      return;                                               //Salimos para comprobar si hay otra alarma y si no,
    }                                                       // se irá al estado de REPOSO.
    Serial.print (F("--- ALARMA: LLENADO. TRAS "));
    Serial.print (maqDatosEEPROM.time_piscina);
    Serial.println (F(" minutos, LA MEDIA DEL NIVEL NO HA DISMINUIDO NADA. REVISAR SISTEMA DE LLENADO."));
    
  }
  reg_alarmas_aux = reg_alarmas & ALARMA_MEDIDORES_FAIL;
  if (reg_alarmas_aux == ALARMA_MEDIDORES_FAIL) {
    Serial.print (F("--- ALARMA: FALLO DE ULTRASONIDO: "));
    Serial.println(num_reset_laser + 1);
    //vl53l0x_setup ();                   
    num_reset_laser++;
    if (num_reset_laser >= 10) {
      //while (1);                        //ATENCIÓN!!!! RESETEA EL EQUIPO PORQUE NO SE RECUPERA.
    }
  }
  reg_alarmas_aux = reg_alarmas & ALARMA_ULTRASONIDO;
  if (reg_alarmas_aux == ALARMA_ULTRASONIDO) {
    //Serial.print (F("--- ALARMA: FALLO DE ULTRASONIDO SALTOS: "));
    //Serial.println(num_reset_laser + 1);
    //vl53l0x_setup ();                   
    num_reset_laser++;
    if (num_reset_laser >= 10) {
      //while (1);                        //ATENCIÓN!!!! RESETEA EL EQUIPO PORQUE NO SE RECUPERA.
    }
  }
}

//void mqsec_aux_Setup_OUTLIER (void) {
//  memset (array_niveles_RELES, 0, sizeof (array_niveles_RELES));
//  contador_outliers = 0;
//  contador_niveles = 0;
//}

void mqsec_aux_Calcular_MEDIA_DEFINITIVA (void) {
  float media_aux, resta, cuadrado, varianza_inter;
  
  media_aux = 0;
  for (int i = 0; i < N_NIVELES; i++) {
    media_aux += array_niveles_RELES[i];
  }
  media_reles_def = media_aux / N_NIVELES;
  for (int i = 0; i < N_NIVELES; i++) {
    resta = (float)array_niveles_RELES[i] - media_reles;
    cuadrado = pow (resta, 2);
    varianza_inter += cuadrado;
  }
  float varianza = (float) varianza_inter / (float) (N_NIVELES - 1);   //ESTO ES PORQUE ES n-1.
  desviacion_reles_def = sqrt (varianza);
}


int mqsec_aux_Comprobar_OUTLIER (void) {

  nivel_reles       = protocolo_Test_NIVEL ();
  media_reles       = protocolo_Test_MEDIA ();
  desviacion_reles  = protocolo_Test_DESVIACION ();
//ESTO ES PARA COMPROBAR QUE EL ARRAY DE NIVELES DE LA PCB_RELES TODAVÍA NO HA SIDO RELLENADO.
  //if (array_niveles_RELES[0] == 0 && array_niveles_RELES[8] == 0 && array_niveles_RELES[14] == 0) {
  //  for (int i = 0; i < N_NIVELES; i++) {
  //    array_niveles_RELES[i] = media_reles;
  //  }
  //}
  //mqsec_aux_Calcular_MEDIA_DEFINITIVA ();
  if (desviacion_reles > DESVIACION_CRITICA) {
//AQUÍ PROBABLEMENTE HABRÍA QUE DAR UNA ALARMA YA.
    mqsec_aux_Set_flag_outliers ();
    contador_outliers++;
    Serial.print (F("/////+++++ CONTABILIZANDO OUTLIERS: "));
    Serial.println (contador_outliers);
    Serial.print (F("////******NIVEL: "));
    Serial.print (nivel_reles);
    Serial.print (F("\tMEDIA: "));
    Serial.print (media_reles);
    Serial.print (F("\t\tDESVIACIÓN: "));
    Serial.println (desviacion_reles);
    if (contador_outliers == 1) {
      media_outlier_erronea = nivel_reles;
      Serial.print (F("//////***** SE TOMA MEDIDA ERRONEA: "));
      Serial.println (media_outlier_erronea);
      Serial.println ();
    }
    if (contador_outliers >= N_OUTLIERS_CRITICOS) {
      //Serial.println (F("/////***** ALARMA ULTRASONIDO LECTURAS ERRÓNEAS *****//////"));
      return OUTLIER_ALARMA; 
    }
  } else {
    mqsec_aux_Reset_flag_outliers ();
    Serial.println ();
    Serial.print (F("/////+++++ CONTABILIZANDO OUTLIERS: "));
    Serial.println (contador_outliers);
    Serial.print (F("¡¡¡¡¡*****----- MEDIA_OUTLIER_ERRONEA: "));
    Serial.println (media_outlier_erronea);
    Serial.print (F("¡¡¡¡¡*****----- MEDIA_OUTLIER_BUENA: "));
    Serial.println (media_outlier_buena);
    Serial.println ();
    if (contador_outliers == 0) {
      media_outlier_buena = media_reles;    //Se guarda solamente si el contador de OUTLIERS es igual a cero por que no se ha recibido nada.
      Serial.print (F("//////***** SE TOMA MEDIDA BUENA: "));
      Serial.println (media_outlier_buena);
      Serial.println ();
      media_outlier_erronea = 0;
      Serial.print (F("//////***** SE RESETEA MEDIDA ERRONEA: "));
      Serial.println (media_outlier_erronea);
      Serial.println ();
    }
    if (media_reles <= (media_outlier_erronea + TOLERANCIA_OUTLIER) && media_reles >= (media_outlier_erronea - TOLERANCIA_OUTLIER)) {    
//ESTE SERÍA EL CASO EN EL QUE LA DESVIACIÓN ESTÁ POR DEBAJO DE LA CRITICA, el contadorde outliers > 0 y el valor de la media_reles está cercana a la "media_outlier_erronea"
      Serial.print (F("¡¡¡¡¡ MEDIA_OUTLIER_ERRONEA: "));
      Serial.println (media_outlier_erronea);
      Serial.println (F("¡¡¡¡¡-------------------- RESET CABEZAL ----------------!!!!!"));
      return OUTLIER_RESET_CABEZAL;
    } 
    //SE DA LA CIRCUNSTANCIA DE QUE LA DESVIACIÓN ES MENOR QUE LA CRITICA Y QUE EL VALOR DE MEDIA RECOGIDO ESTÁ ENTRE EL VALOR BUENO +/-20.
    if (media_reles <= (media_outlier_buena + TOLERANCIA_OUTLIER) && media_reles >= (media_outlier_buena - TOLERANCIA_OUTLIER)) {
      contador_outliers = 0;
      //media_outlier_erronea = media_outlier_buena = 0;
      Serial.print (F("¡¡¡¡¡ CONTADOR_OULIER: "));
      Serial.println (contador_outliers);
    }
    //AQUÍ SE INTRODUCE TODAS LAS MEDIAS EN EL ARRAY CORRESPONDIENTE.
    //if (contador_niveles >= N_NIVELES) {
    //  contador_niveles = 0;
    //}
    //array_niveles_RELES[contador_niveles++] = media_reles;
  }
  return OUTLIER_OK;
}
void mqsec_aux_RESET_OUTLIERS (void) {
  contador_outliers = 0;
  media_outlier_erronea = 0;
  media_outlier_buena = 0;
}
int mqsec_aux_ASK_Contador_ouliers (void) {
  return contador_outliers;
}
int mqsec_aux_ASK_MEDIAErronea_ouliers (void) {
  return media_outlier_erronea;
}
int mqsec_aux_ASK_MEDIABuena_ouliers (void) {
  return media_outlier_buena;
}
int mqsec_aux_NIVEL_RELES (void) {
  return nivel_reles;
}
float mqsec_aux_MEDIA_RELES (void) {
  return media_reles;
}
float mqsec_aux_DESVIACION_RELES (void) {
  return desviacion_reles;
}
