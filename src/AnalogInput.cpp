/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground through 220 ohm resistor

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInOutSerial
*/
#include <advancedSerial.h>

#include "AnalogInput.h"
#include "hardware.h"


int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

int p_nivel = 0, p_desviacion = 0;
int array_niveles[N_NIVELES], nivel = 0;
long media_llenado = 0;
float media = 0, desviacion_tipica = 255;
unsigned long timeout_lectura = 0;

static void AnalogInput_CalcularMEDIA (int medida, char mostrar);

void AnalogInput_setup (void) {
  // Stablish the resolution bits.
  analogReadResolution(12);
}

void AnalogInput_loop (unsigned long t_actual) {
  //float tension = 0;
  //float fondoEscala = 3.3;
  float aux = 0;

  if ((t_actual - timeout_lectura) > TIMEOUT_LECTURA_NIVEL) {
    timeout_lectura = t_actual;                         //Se resetea el TIMEOUT.
    analogReadResolution(12);                           // Stablish the resolution bits.
    sensorValue = analogRead(analogInPin);              // read the analog in value:
    outputValue = map(sensorValue, 0, 4095, 800, 70);   // map it to the range of the analog out:
    // change the analog out value:
    //analogWrite(analogOutPin, outputValue);
    // print the results to the Serial Monitor:
    Serial.print("sensor = ");
    Serial.print(sensorValue);
    //aux = (float)((3.3*sensorValue)/4096);
    //Serial.print("\t Auxilia r = ");
    //Serial.print(aux);
    //tension = (float)(800-((730*aux)/fondoEscala));
    //Serial.print("\t Tension = ");
    //Serial.print(tension);
    Serial.print("\t output = ");
    Serial.println(outputValue);
    AnalogInput_CalcularMEDIA (outputValue, 1);
  }
}


/************************************************************************************************
 * FUNCIÓN: void mqprin_CalcularNivel (int medida, char mostrar)
 * DESCRIPCIÓN: Se encarga de calcular la MEDIA y la DESVIACIÓN TÍPICA
 * ARGUMENTOS DE ENTRADA:
 *    - MEDIDA: Es el nivel instantaneo del agua.
 *    - MOSTRAR:
 *        > 0: No muestra nada.
 *        > 1: Muestra el nivel, la media y la desviación típica.
 *************************************************************************************************/
void AnalogInput_CalcularMEDIA (int medida, char mostrar) {
  float media_inter = 0, varianza_inter = 0, cuadrado = 0, resta = 0;
  
  if (p_nivel >= N_NIVELES) {                //El array de niveles será cíclico.
    p_nivel = 0;                      //Se resetea el puntero de niveles.
  }
  nivel = medida;
  array_niveles[p_nivel++] = medida;    //Se introduce en el array de niveles la medida recien tomada.
  Serial.print (F("ARRAY: "));
  for (int i = 0; i < N_NIVELES; i++) {
    Serial.print (array_niveles[i]);
    Serial.print (",");
  }
  Serial.println();
  
  if (mostrar == 1) {
    Serial.print (F("***--- DISTANCIA: "));
    Serial.println (medida);
  }
  for (int i = 0; i < N_NIVELES; i++) {
    media_inter += array_niveles[i];
  }
  media = media_inter / N_NIVELES;
  if (mostrar == 1) {
    Serial.print (F("Media: "));
    Serial.println (media);
  }
  for (int i = 0; i < N_NIVELES; i++) {
    resta = (float)array_niveles[i] - media;
    cuadrado = pow (resta, 2);
    varianza_inter += cuadrado;
  }
  float varianza = (float) varianza_inter / (float) (N_NIVELES - 1);   //ESTO ES PORQUE ES n-1.
  desviacion_tipica = sqrt (varianza);
  if (mostrar == 1) {
    Serial.print (F("Desviacion Tipica: "));
    Serial.println (desviacion_tipica);
  }
}

