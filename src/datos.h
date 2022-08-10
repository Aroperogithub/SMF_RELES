//=============================================================================
//                       IMPOREXO
//                      (c) Copyright 2017
//=============================================================================
#ifndef _DATOS_H_
#define _DATOS_H_

#include "hardware.h"

  #define EEPROM_SIZE                 512
  #define EEPROM_OK                   0
  #define EEPROM_KO                   -1

  //DATOS EN LA EEPROM
  #define DAT_SN_SENDER               0           //9 BYTES PARA EL NÚMERO DE SERIE DE ESTE DISPOSITIVO.
  #define DAT_SN_RECEIVER             10          //5 PUBLISHERs o MOVILES.
  
  #define DAT_LLENADO_ON              16+4    //Posiciones 16. INT = 65535. PARA SABER SI SE ESTABA LLENANDO DESPUÉS DE UN RESET O BOMBA ON.
  #define DAT_N_LLENADOS              18+4    //Posiciones 18. INT = 65535. CONTABILIZA EL NÚMERO DE LLENADOS QUE LLEVA.
  #define DAT_ALTURA                  20+4    //Posiciones 20. BYTE = 255.
  #define DAT_DESVIACION              21+4    //Posiciones 21. BYTE = 255.
  #define DAT_MEDIDA_MAXLASER         22+4    //Posiciones 22 y 23. INT = 65.535.
  #define DAT_PULSO_PERSIANA          24+4    //Posiciones 24 y 25. INT = 65.535.
  #define DAT_NIVEL_SECO              26+4    //Posiciones 26 y 27. INT = 65.535.
  #define DAT_NIVEL_OPTIMO            28+4    //Posiciones 28 y 29. INT = 65.535.
  #define DAT_TIME_PISCINA            30+4    //Posiciones 30 y 31. INT = 65.535.
  #define DAT_TIEMPO_LLENADO          32+4    //Posiciones 32 y 33. INT = 65.535.
  #define DAT_TIME_LLENAR_1A          34+4    //Posiciones 34 y 35. INT = 65.535.
  #define DAT_SUPERFICIE              36+4    //Posiciones 36, 37, 38, 39. LONG 4 bytes.
  #define DAT_LLENAR_ON               40+4    //Posiciones 40. 1 BYTE = 255.
  #define DAT_VACIAR_ON               41+4    //Posiciones 41, 1 BYTE = 255.
  #define DAT_PERSIANAS_ON            42+4    //Posiciones 42, 1 BYTE = 255.
  #define DAT_DESVIACION2             43+4    //Posiciones 43, 1 BYTE = 255.
  #define DAT_RESERVED_L2             44+4    //Posiciones 44, 45, 46, 47. LONG 4 bytes.
  #define DAT_SSID                    48+4    //Posiciones 48 a 81. 33 bytes, Cadena de caracteres.
  #define DAT_PASSWRD                 82+4    //Posiciones 82 a 115. 33 bytes, Cadena de caracteres.
  #define DAT_USER_THINGERIO          116+4   //Posiciones 116 a 149. 33 bytes, cadena de caracteres. usuario ""
  #define DAT_DEVICE_ID_THINGERIO     150+4   //Posiciones 150 a 183. 33 bytes, cadena de caracteres. device_Id ""
  #define DAT_CREDENTIALS_THINGERIO   184+4   //Posiciones 184 a 217. 33 bytes, cadena de caracteres. device_credentials ""
  #define DAT_CODIGO_POSTAL           218+4   //Posiciones 218 a 224.  6 bytes, cadena de caracteres. codigo_postal ""
  #define DAT_SSID2                   225+4   //Posiciones 225 a 258. 33 bytes, cadena de caracteres. SSID de la WiFi 2.
  #define DAT_PASSWRD2                259+4   //Posiciones 259 a 292. 33 bytes, cadena de caracteres. PASSWORD de la WiFi 2.

  #define DAT_NIVEL_SECO_2              293+4    //Posiciones 26 y 27. INT = 65.535.
  #define DAT_NIVEL_OPTIMO_2            295+4    //Posiciones 28 y 29. INT = 65.535.
  #define DAT_TIME_PISCINA_2            297+4    //Posiciones 30 y 31. INT = 65.535.
  #define DAT_TIME_LLENAR_1A_2          299+4    //Posiciones 34 y 35. INT = 65.535.
  #define DAT_LLENAR_ON_2               301+4    //Posiciones 40. 1 BYTE = 255.
  #define DAT_INCONGRUENCIA             302+4    //Cuenta el número de veces que se ha dado la incongruencia.
  #define DAT_SN_SENDER_INCONG          303+4           //9 BYTES PARA EL NÚMERO DE SERIE DE ESTE DISPOSITIVO.
  #define DAT_SN_RECEIVER_INCONG        303+4+10          //5 PUBLISHERs o MOVILES.
  #define DAT_MAX_DATOS                 303+4+10+10

#define DATOS_EEPROM_MAL                -1
#define DATOS_EEPROM_BIEN               0
#define DATOS_EEPROM_ALARMA             1
#define DATOS_ALARMA_INCONGRUENCIA      3 //Nº DE VECES DE HABER 3 INCONGRUENCIAS SEGUIDAS.

#define DATOS_PSSI                      0xFD
#define DATOS_PSNO                      0xFE
#define DATOS_PSWI                      0xFF


//#define NUMERO_COMANDO        51    //50 caracteres tiene el total del comando (comando + caracteres), 20 caracteres para el comando, lo que deja 30 caracteres para el dato que le acompaña.

#ifdef  SMART_FILL_v5
  #define EEMPROM_DESVIACION_MIN      15
  #define EEMPROM_NIVEL_SECO          330
  #define EEMPROM_NIVEL_OPTIMO        150 
  #define EEMPROM_NIVEL_OPTIMO_MINIMO 70
  #define EEMPROM_NIVEL_SECO_MAXIMO   800
  #define EEMPROM_TIMEPISCINA         15
  #define EEMPROM_NUMLLENADOS         0
  #define EEMPROM_TIME_1A             100
  #define EEMPROM_TIME_PULSO_PERSIANA 5
#endif

  #define DATOS_TODOS           0
  #define DATOS_SOLO_NIVELES    1

//=============================================================================
// Public data declarations
//=============================================================================
//typedef volatile struct _DATEEPROM {
typedef struct _DATEEPROM {
      int llenado_ON;
      int N_llenados;
      byte altura_pool;             //Se almacenará en cm, es decir de 0 a 255cm (que equivale a 25,5m).
      byte desviacion;
      int  medida_maxlaser;         //Variable para albergar la medida máxima definida para el LASER.
      int  pulso_persiana;          //Variable para albergar la medida máxima definida para el ULTRASONIDO.
      int  nivel_seco;              //NIVEL a partir del cual se inicia el LLENADO de la Piscina.
      int  nivel_optimo;            //NIVEL a partir del cual se considera a la Piscina como suficientemente llena.
      int  time_piscina;            //Este es el tiempo que se configura para que el nivel cambie durante el llenado, tras el cual se sale del estado de llenar.
                                    // Dependerá de cada Piscina y del caudal del agua.
      int  tiempo_llenado;          //Este es el tiempo de llenado que se guarda del último llenado.
      int  time_llenar_1A;          //Este es el tiempo que se configura para alcanzar el nivel OPTIMO en el primer llenado de la piscina.
      long superficie_pool;         //Se almacenará en m2 hasta dos decimales: 0 - 21.474.836,47m2.
      byte llenar_on;               //Flag de configuración de llenado por bluetooth.
      byte vaciar_on;               //Flag de configuración de vaciado por Bluetooth.
      byte persiana_on;             //Flag de configuración de Persina activa.
      byte reserved_b1;             //Flag reservado tipo BYTE.
      long reserved_l2;             //Memoria reservada tipo LONG.
      char ssid[33];                
      char passwrd[33];
      char user_thingerio[34];
      char device_thingerio[34];
      char credentials_thingerio[34];
      char codigo_postal[7];
      char ssid2[33];
      char passwrd2[33];
} __attribute__((packed)) DATEEPROM;

  //LA DISPOSICIÓN DE TODOS LOS DATOS DE LA EEPROM ESTÁ EN EL FICHERO HARDWARE.H, donde tiene más sentido, ya que dependerá del equipo en sí y no de las rutinas del módulo Datos.cpp/h.

  extern int datos_IniciarEEPROM (void);
  extern unsigned int datos_EEPROM_Read (int tipo);
  extern void datos_EEPROM_Read_Memoria (int inicioEEPROM, int finalEEPROM);
  extern void datos_Parametrizar_MemoriaEEPROM (void);
  extern void datos_Reset_MemoriaEEPROM (void);

  extern byte datos_EEPROM_Read_BYTE (int tipo);
  extern void datos_EEPROM_Write_BYTE (byte datos, int tipo);
  extern int  datos_EEPROM_Read_INT (int num);
  extern void datos_EEPROM_Write_INT (int datos, int tipo);
  extern long datos_EEPROM_Read_LONG (int num);
  extern void datos_EEPROM_Write_LONG (long datos, int tipo);
  extern void datos_Parametros_FabricaEEPROM (void);
  extern void datos_Reset_Parcial_MemoriaEEPROM (int INI_DATOS, int FIN_DATOS);
  extern int  datos_Comprobar_Incongruencia_de_datos (void);
#endif // _DATOS_H_
