#include <Wire.h>
#include <Time.h>

#include "hardware.h"
#include "maqsec.h"
#include "mqsec_aux.h"
#include "esp_system.h"
#include "Lora_imp.h"
#include "datos.h"
#include "protocolo.h"
#include "AnalogInput.h"
#include "xxtea_Easy.h"
#include "teclas.h"
#include "Bluetooth.h"

unsigned long T_actual;
const int wdtTimeout = 5000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;


static void inicializacion_Pines (void);

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

void setup() {
  Serial.begin (115200);
  inicializacion_Pines ();
  datos_IniciarEEPROM ();
  //datos_Reset_MemoriaEEPROM ();
  datos_Parametrizar_MemoriaEEPROM ();
  maqsec_IniciacionVbles ();                        //Initias all variables of State Machine.
  Lora_imp_setup ();
  protocolo_Init ();
  AnalogInput_setup ();
  xxtea_Easy_setup ();
  T_actual = millis ();
  teclas_Iniciar_Variables_F (T_actual);
  Bluetooth_setup ();
  mqsec_aux_RESET_OUTLIERS ();
//WATCHDOG
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
}
 
void loop() {
  int comando_LoRa = LORA_NULL;
  int boton, boton_tecla;
  int fc_estirar, fc_recoger;
  int tecla_FCEstirar, tecla_FCRecoger;
  bool status_Bluetooth;  

  timerWrite(timer, 0);                             //reset timer (feed watchdog)
  T_actual = millis ();                             //Captura de los milisegundos transcurridos.
  boton = digitalRead (BOTON);
  boton_tecla = teclas_filtro_Teclado_FAVORITO (boton, T_actual);
  tecla_FCEstirar = digitalRead (FC_ESTIRAR);
  fc_estirar = teclas_ESTIRAR (tecla_FCEstirar, T_actual);
  tecla_FCRecoger = digitalRead (FC_RECOGER);
  fc_recoger = teclas_ESTIRAR (tecla_FCRecoger, T_actual);
  status_Bluetooth = Bluetooth_TestCONNECTED (T_actual);
  comando_LoRa = Lora_imp_CheckPacket ();
  maqsec_SECUNDARIA (T_actual, comando_LoRa, boton_tecla, status_Bluetooth, fc_estirar, fc_recoger);
  //Bluetooth_ActivarRESET_DESCONEXION (T_actual); 
}
void inicializacion_Pines (void) {
  pinMode       (BOTON,               INPUT);
  pinMode       (EV_LLENADO,          OUTPUT);
  digitalWrite  (EV_LLENADO,          NIVEL_BAJO);        //Pulso para apagar el FOCOIRIS.
  pinMode       (EV_VACIADO,          OUTPUT);
  digitalWrite  (EV_VACIADO,          NIVEL_BAJO);        //Pulso para apagar el FOCOIRIS.
  pinMode       (EV_PERSIANA_ESTIRAR, OUTPUT);
  digitalWrite  (EV_PERSIANA_ESTIRAR, NIVEL_BAJO);        //Pulso para apagar el FOCOIRIS.
  pinMode       (EV_PERSIANA_RECOGER, OUTPUT);
  digitalWrite  (EV_PERSIANA_RECOGER, NIVEL_BAJO);        //Pulso para apagar el FOCOIRIS.
  pinMode       (EV_BOMBA_VAC,        OUTPUT);
  digitalWrite  (EV_BOMBA_VAC,        NIVEL_BAJO);        //Pulso para apagar el FOCOIRIS.
  pinMode       (LED_BOTON,           OUTPUT);
  digitalWrite  (LED_BOTON,           LOW);               //Pulso para apagar el FOCOIRIS.
  pinMode       (led_sender,          OUTPUT);
  digitalWrite  (led_sender,          LOW);               //Pulso para apagar el FOCOIRIS.
  pinMode       (FC_ESTIRAR,          INPUT_PULLUP);
  pinMode       (FC_RECOGER,          INPUT_PULLUP);
}

 
