/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "esp_gap_ble_api.h"
#include "esp_err.h"

#include <advancedSerial.h>

#include "WiFi.h"
#include "Bluetooth.h"
#include "mq_sec.h"
#include "maqsec.h"
#include "hardware.h"
#include "datos.h"
#include "protocolo.h"
#include "mqsec_aux.h"


BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

float txValue = 0;
bool flag;
unsigned long t_inter_blue = 0, timeout_reset_bluetooth = 0;
int nveces_bluetooth_reset = 0;
int status_Blue = B_DISCONNECTED;
std::string rxValue;    // Could also make this a global var to access it in loop()
IPAddress IP_GLOBAL_BLUETOOTH;
int RSSI_global_bluetooth;


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
//#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID



#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
//#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

void Bluetooth_restartService (void);

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      //std::string rxValue = pCharacteristic->getValue();
      
      rxValue = pCharacteristic->getValue();
      /*if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }
        Serial.println();
        Serial.println("*********");
      }*/
    }
};
void remove_all_bonded_devices(void) {
  int dev_num = esp_ble_get_bond_device_num();
  esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
  esp_ble_get_bond_device_list(&dev_num, dev_list);
  for (int i = 0; i < dev_num; i++) {
      esp_ble_remove_bond_device(dev_list[i].bd_addr);
  }
  free(dev_list);
}

void Bluetooth_setup (void) {
  char nombreBLE[10];
  String nameBluetooth;

  memset (nombreBLE, 0, sizeof(nombreBLE));
  WiFi.mode(WIFI_MODE_STA);
  Serial.print (F("MAC ADDRESS: "));
  nameBluetooth = WiFi.macAddress();
  Serial.println (nameBluetooth);
  strcpy (nombreBLE, "BOR");
  nombreBLE[0] = 'S';
  nombreBLE[1] = 'M';
  nombreBLE[2] = 'F';
  nombreBLE[3] = nameBluetooth[9];
  nombreBLE[4] = nameBluetooth[10];
  nombreBLE[5] = nameBluetooth[12];
  nombreBLE[6] = nameBluetooth[13];
  nombreBLE[7] = nameBluetooth[15];
  nombreBLE[8] = nameBluetooth[16];
  Serial.print (F("NOMBRE DEL BLUETOOTH: "));
  Serial.println (nombreBLE);

  // Create the BLE Device
  BLEDevice::init(nombreBLE); // Give it a name
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL1, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL2, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL3, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL4, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL5, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL6, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL8, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
  //BORRA TODOS LOS DISPOSITIVOS VINCULADOS PREVIAMENTE.
  //Serial.println (F("Antes de borrar vinculados."));
  remove_all_bonded_devices();
  //Serial.println (F("DESPUÉS VINCULADOS"));
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pCharacteristic->setValue("Hello World");
  pService->start();

 // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}


std::string Bluetooth_loop(unsigned long T_actual) {
  //const COMANDO_COLORES *tmpptr = AR_Colores;
  std::string data_str ="";

  if (abs(T_actual - t_inter_blue) >= 100) {
    t_inter_blue = T_actual;
    if (deviceConnected) {
      if (rxValue.length() != 0) {
        data_str = rxValue;
      } 
      rxValue = "";         //Esto es para resetear el buffer de entrada/lectura de comandos.
    }
  }
  return data_str;
}

bool Bluetooth_TestCONNECTED (unsigned long T_actual) {

 // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println(F("********** BLUETOOTH DESCONECTADO ***********"));
      oldDeviceConnected = deviceConnected;
      nveces_bluetooth_reset++;               //Se recibe una desconexión de Bluetooth.
      //timeout_reset_bluetooth = T_actual;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      Serial.println (F("!!!!!!!!!! BLUETOOTH CONECTADO !!!!!!!!!!"));
      oldDeviceConnected = deviceConnected;
      nveces_bluetooth_reset = 0;             //Se resetea el contador.
  }
  if (!deviceConnected) {
    return B_DISCONNECTED;
  } else {
    return B_CONNECTED;
  }
}

void Bluetooth_Notification (void) {
  float temp_float;
  int temp_int, reg_alarmas, alarmas_aux;
  char buf[500], buff_aux[80], temp_aux[50];
  
  sprintf (buf, "+DATOS:");
  //NP - Nivel Piscina. Este es el nivel instantáneo de la piscina, lo que denominamos "Distancia" en los LOGS serie
  temp_int = (int) protocolo_Test_NIVEL ();
  sprintf(buff_aux, "#NP-%d*", temp_int);
  strcat (buf, buff_aux);
  //MP - Media Piscina. Esta es la media del histórico de las últimas 16 tomas de medidas.
  temp_float = mqsec_aux_MEDIA_RELES ();
  sprintf(buff_aux, "#MP-%.2f*", temp_float);
  strcat (buf, buff_aux);
  //DT - Desviación Típica. Este es el valor de la Desviación Típica teniendo en cuenta las últimas 16 medidas de NP.
  temp_float = mqsec_aux_DESVIACION_RELES ();
  sprintf(buff_aux, "#DT-%.2f*", temp_float);
  strcat (buf, buff_aux);
  //ES - ESTADO en el que se encuentra el dispositivo.
  temp_int = mq_sec_ESTADO ();
  sprintf (buff_aux, "#ES-");
  switch (temp_int) {
    case INICIAL:
      strcat(buff_aux, "INICIAL*");
      break;
    case REPOSO:
      strcat(buff_aux, "REPOSO*");
      break;
    case LLENAR:
      strcat(buff_aux, "LLENAR*");
      break;
    case ALARMAS:
      strcat(buff_aux, "ALARMAS*");
      break;
    case LLENAR_1A:
      strcat(buff_aux, "LLENAR_1A*");
      break;
    default:
      strcat(buff_aux, "DESCONOCIDO*");
      break;
  }
  strcat (buf, buff_aux);
  //AL - ALARMAS
  sprintf (buff_aux, "#AL-");
  reg_alarmas = mq_sec_ALARMAS_Bluetooth ();
  if (reg_alarmas == ALARMA_SIN_ALARMAS) {
    int temp_int = (int) datos_EEPROM_Read_BYTE (DAT_INCONGRUENCIA);
    if (temp_int != 0) {
      strcat (buff_aux, "DATOS EEPROM: ");
      sprintf(temp_aux, "%i*", temp_int);
      strcat (buff_aux, temp_aux);
    } else {
      strcat (buff_aux, "NO HAY ALARMAS*");
    }
  } else {
    alarmas_aux = reg_alarmas & ALARMA_INCONGRUENCIA;        
    if (alarmas_aux == ALARMA_INCONGRUENCIA) {
      strcat (buff_aux, "DATOS ERRONEOS EEPROM*");
    }
    alarmas_aux = reg_alarmas & ALARMA_INUNDACION;        
    if (alarmas_aux == ALARMA_INUNDACION) {
      strcat (buff_aux, "INUNDACION*");
    }
    alarmas_aux = reg_alarmas & ALARMA_MEDIDORES_FAIL;        
    if (alarmas_aux == ALARMA_MEDIDORES_FAIL) {
      strcat (buff_aux, "ERROR MEDIDOR*");
    }
    alarmas_aux = reg_alarmas & ALARMA_ULTRASONIDO;        
    if (alarmas_aux == ALARMA_ULTRASONIDO) {
      strcat (buff_aux, "ERROR ULTRASONIDO*");
    }
    alarmas_aux = reg_alarmas & ALARMA_CABEZAL;        
    if (alarmas_aux == ALARMA_CABEZAL) {
      strcat (buff_aux, "SIN RESPUESTA ULTRASONIDOS*");
    }
    alarmas_aux = reg_alarmas & ALARMA_NIVELES;        
    if (alarmas_aux == ALARMA_NIVELES) {
      strcat (buff_aux, "NIVEL EXCESIVO*");
    }
    alarmas_aux = reg_alarmas & ALARMA_LLENADO;        
    if (alarmas_aux == ALARMA_LLENADO) {
      strcat (buff_aux, "TIEMPO EXCESIVO DE LLENADO*");
    }
  }
  strcat (buf, buff_aux);
  //NO - Nivel OPTIMO. Este es el nivel óptimo programado por el usuario.
  temp_int = datos_EEPROM_Read_INT (DAT_NIVEL_OPTIMO);
  sprintf(buff_aux, "#NO-%d*", temp_int);
  strcat (buf, buff_aux);
  //NL - Nivel LLENADO. Este es el nivel de llenado programado por el usuario.
  temp_int = datos_EEPROM_Read_INT (DAT_NIVEL_SECO);
  sprintf(buff_aux, "#NL-%d*", temp_int);
  strcat (buf, buff_aux);
  //NU - Número de llenados realizados
  temp_int = datos_EEPROM_Read_INT (DAT_N_LLENADOS);
  sprintf(buff_aux, "#NU-%d*", temp_int);
  strcat (buf, buff_aux);
  //TL - Tiempo de llenado. Este es el tiempo de llenado programado por el usuario para determinar si hay 
  // alarma de tiempo excesivo de llenado.
  temp_int = datos_EEPROM_Read_INT (DAT_TIME_PISCINA);
  sprintf(buff_aux, "#TL-%d*", temp_int);
  strcat (buf, buff_aux);
  strcat (buf, "#SS-");
  memset (buff_aux, 0, sizeof(buff_aux));
  for (int n_datos = 0; n_datos <= (DAT_PASSWRD - DAT_SSID); n_datos++) {
    buff_aux[n_datos] = datos_EEPROM_Read_BYTE (DAT_SSID + n_datos);
    if (buff_aux[n_datos] == 0) break;
  }
  strcat (buf, buff_aux);
  strcat (buf, "*");
  strcat (buf, "#IP-");
  IPAddress ip = WiFi.localIP();
  sprintf(buff_aux, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  strcat (buf, buff_aux);
  strcat (buf, "*");
  strcat (buf, "#RS-");
  RSSI_global_bluetooth = (int) WiFi.RSSI();
  sprintf(buff_aux, "%d", RSSI_global_bluetooth);
  strcat (buf, buff_aux);
  strcat (buf, "*");
  if (deviceConnected) {
    pCharacteristic->setValue(buf);
    pCharacteristic->notify();
    Serial.println (buf);
    /*+DATOS:#NP-nnnnn*#MP-nnnnn*#DT-nn,nn*#ES-nnnnnnn(<=10caracteres)*#AL-nnnnnn(<=30caracteres)
    *#NO-nnnnn*#NL-nnnnn*#NU-nn*#TL-nnnnn(<=5caracteres*#SSnnnnnnnn(<=20caracteres)-
    *#IPnnn.nnn.nnn.nnn-*#RS-nnn*////*/
  }
}

void Bluetooth_Notification_CONFIGURACION (char *buffer_bluetooth) {
  pCharacteristic->setValue(buffer_bluetooth);
  pCharacteristic->notify();
}

void Bluetooth_ActivarRESET_DESCONEXION (unsigned long T_actual) {
  if (nveces_bluetooth_reset == 0) {
    timeout_reset_bluetooth = T_actual;
    //Serial.println (F("--- RESET TIMEOUT BLUETOOTH ---"));
  } else {
    if (nveces_bluetooth_reset >= BLUETOOTH_NVECES_DESC && (T_actual - timeout_reset_bluetooth) < BLUETOOTH_RESET) {
      //Esto se leería: Si se ha desconectado BLUETOOTH_NVECES_DESC (4) veces o más en menos de BLUETOOTH_RESET (20s)
      Serial.println (F("¡¡¡¡¡¡¡¡ - EL EQUIPO SE RESETEA POR BLUETOOTH PERDIDO - !!!!!!!!"));
      while (1);
    }
  }
}
