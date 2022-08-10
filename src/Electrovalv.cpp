
#include <advancedSerial.h>

#include "Electrovalv.h"
#include "maqsec.h"
#include "hardware.h"

EVFLAGS evbits;
unsigned long timeout_led_boton = 0;
unsigned long timeout_nivel_LEDESP32, timeout_led_ESP32 = 0, timeouts_array_ESP32[20];
short nivel_LEDESP32, contador_pulsos_led_ESP32 = 0, n_pulsos_ESP32, niveles_array_ESP32[30];

unsigned long timeout_nivel_LED_BOTTON, timeout_led_BOTTON = 0, timeouts_array_BOTTON[20];
short nivel_LED_BOTTON, contador_pulsos_led_BOTTON = 0, n_pulsos_BOTTON, niveles_array_BOTTON[30];

void Electrovalv_EV_LLENADO (bool accion) {
    if (accion == EV_ON) {
        digitalWrite  (EV_LLENADO, EV_ON);
        evbits.ev_llenado = EV_ON;
    } else {
        digitalWrite  (EV_LLENADO, EV_OFF);
        evbits.ev_llenado = EV_OFF;
    }
}
//ESTA FUNCIÓN SERÍA PARA ACTIVAR UNA ELECTROVÁLVULA PARA ABRIR UNA TUBERÍA DE DESAGÜE.
void Electrovalv_EV_VACIADO (bool accion) {
    if (accion == EV_ON) {
        digitalWrite  (EV_VACIADO, EV_ON);
        evbits.ev_vaciado = EV_ON;
    } else {
        digitalWrite  (EV_VACIADO, EV_OFF);
        evbits.ev_vaciado = EV_OFF;
    }
}
//ESTA FUNCIÓN SERÍA PARA ACTIVAR UNA BOMBA DE VACIADO DE LA PISCINA.
void Electrovalv_EV_BOMBA_VAC (bool accion) {
    if (accion == EV_ON) {
        digitalWrite  (EV_BOMBA_VAC, EV_ON);
        evbits.ev_bomba_vac = EV_ON;
    } else {
        digitalWrite  (EV_BOMBA_VAC, EV_OFF);
        evbits.ev_bomba_vac = EV_OFF;
    }
}
void Electrovalv_EV_PERSIANA_ESTIRAR (bool accion) {
    if (accion == EV_ON) {
        digitalWrite  (EV_PERSIANA_ESTIRAR, EV_ON);
        evbits.ev_persiana_estirar = EV_ON;
    } else {
        digitalWrite  (EV_PERSIANA_ESTIRAR, EV_OFF);
        evbits.ev_persiana_estirar = EV_OFF;
    }
}
void Electrovalv_EV_PERSIANA_RECOGER (bool accion) {
    if (accion == EV_ON) {
        digitalWrite  (EV_PERSIANA_RECOGER, EV_ON);
        evbits.ev_persiana_recoger = EV_ON;
    } else {
        digitalWrite  (EV_PERSIANA_RECOGER, EV_OFF);
        evbits.ev_persiana_recoger = EV_OFF;
    }
}

bool Electrovalv_test (int evflag) {
    switch (evflag) {
        case EV_BITS_BOMBA:
            return evbits.ev_bomba;
            break;
        case EV_BITS_LLENADO:
            return evbits.ev_llenado;
            break;
        case EV_BITS_VACIADO:
            return evbits.ev_vaciado;
            break;
        case EV_BITS_BOMBA_VAC:
            return evbits.ev_bomba_vac;
            break;
        case EV_BITS_PERSINA_ESTIRAR:
            return evbits.ev_persiana_recoger;
            break;
        case EV_BITS_PERSINA_RECOGER:
            return evbits.ev_persiana_estirar;
            break;
        default:
            break;
    }
    return evbits.ev_persiana_estirar;
}
void Electrovalv_EV_LED_BOTTON (bool accion) {
    if (accion == EV_ON) {
        digitalWrite  (LED_BOTON, EV_ON);
        evbits.led_booton = EV_ON;
    } else {
        digitalWrite  (LED_BOTON, EV_OFF);
        evbits.led_booton = EV_OFF;
    }
}
void Electrovalv_LED_BLINKONOFF_BOTTON (unsigned long T_actual, int timeout_ON, int timeout_OFF) {
    if (evbits.led_booton == EV_ON) {   //Botón en ON. Esperar timeout para cambiar a OFF.
        if ((T_actual - timeout_led_boton) > timeout_ON) {
            timeout_led_boton = T_actual;   //Reseteo el timeout porque ya ha llegado a su tiempo.
            evbits.led_booton = EV_OFF;
            digitalWrite  (LED_BOTON, EV_OFF);
        }
    } else {
        if ((T_actual - timeout_led_boton) > timeout_OFF) {
            timeout_led_boton = T_actual;   //Reseteo el timeout porque ya ha llegado a su tiempo.
            evbits.led_booton = EV_ON;
            digitalWrite  (LED_BOTON, EV_ON);
        }
    }
}

short Electrovalv_Coger_Nivel_BOTTON (short contador_pulsos_led) {
    return niveles_array_BOTTON[contador_pulsos_led];
}
unsigned long Electrovalv_Coger_Timeout_BOTTON (short contador_pulsos_led) {
    return timeouts_array_BOTTON[contador_pulsos_led];
}
void Electrovalv_Poner_nivel_BOTTON (short nivel_BOTTON) {
    digitalWrite  (LED_BOTON, nivel_BOTTON);
}
void Electrovalv_LED_ONOFF_BOTTON (unsigned long T_actual) {

    if ((T_actual - timeout_led_BOTTON) > timeout_nivel_LED_BOTTON) {
        timeout_led_BOTTON = T_actual;
        if (contador_pulsos_led_BOTTON >= n_pulsos_BOTTON) {
            contador_pulsos_led_BOTTON = 0;
        }
        nivel_LED_BOTTON          = Electrovalv_Coger_Nivel_BOTTON (contador_pulsos_led_BOTTON);    
        timeout_nivel_LED_BOTTON  = Electrovalv_Coger_Timeout_BOTTON (contador_pulsos_led_BOTTON);
        Electrovalv_Poner_nivel_BOTTON (nivel_LED_BOTTON);
        contador_pulsos_led_BOTTON++;
    }
}

void Electrovalv_Setup_LED_ONOFF_BOTTON (int Estado, unsigned long T_actual) {
    contador_pulsos_led_BOTTON = 0;
    timeout_led_BOTTON = T_actual;
    memset (niveles_array_BOTTON, 0, sizeof(niveles_array_BOTTON));
    memset (timeouts_array_BOTTON, 0, sizeof(timeouts_array_BOTTON));
    switch (Estado) {
    case INICIAL:       //UN PULSO CONTINUO A 0.
        n_pulsos_BOTTON = 1;
        nivel_LED_BOTTON = niveles_array_BOTTON[0] = ON_LED32;
        timeouts_array_BOTTON[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LED_BOTTON = niveles_array_BOTTON[0];
        break;
    case REPOSO:        //UN PULSO ON.
        n_pulsos_BOTTON = 2;
        nivel_LED_BOTTON = niveles_array_BOTTON[0] = ON_LED32;
        timeouts_array_BOTTON[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LED_BOTTON = timeouts_array_BOTTON[0];
        niveles_array_BOTTON[1] = OFF_LED32;
        timeouts_array_BOTTON[1] = (unsigned long) TIMEOUT_LED32_OFF2S;
        break;
    case LLENAR:        //TRES PULSOS A ON.
    case LLENAR_1A:
        n_pulsos_BOTTON = 4;
        nivel_LED_BOTTON = niveles_array_BOTTON[0] = ON_LED32;
        timeouts_array_BOTTON[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LED_BOTTON = timeouts_array_BOTTON[0];
        niveles_array_BOTTON[1] = OFF_LED32;
        timeouts_array_BOTTON[1] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_BOTTON[2] = ON_LED32;
        timeouts_array_BOTTON[2] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_BOTTON[3] = OFF_LED32;
        timeouts_array_BOTTON[3] = (unsigned long) TIMEOUT_LED32_OFF2S;
        break;
    case VINCULAR:      //DOS PULSOS ON.
        n_pulsos_BOTTON = 6;
        nivel_LED_BOTTON = niveles_array_BOTTON[0] = ON_LED32;
        timeouts_array_BOTTON[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LEDESP32 = timeouts_array_BOTTON[0];
        niveles_array_BOTTON[1] = OFF_LED32;
        timeouts_array_BOTTON[1] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_BOTTON[2] = ON_LED32;
        timeouts_array_BOTTON[2] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_BOTTON[3] = OFF_LED32;
        timeouts_array_BOTTON[3] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_BOTTON[4] = ON_LED32;
        timeouts_array_BOTTON[4] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_BOTTON[5] = OFF_LED32;
        timeouts_array_BOTTON[5] = (unsigned long) TIMEOUT_LED32_OFF2S;
        break;
    case ALARMAS:       //DOS PULSOS A ON.
        n_pulsos_BOTTON = 2;
        nivel_LED_BOTTON = niveles_array_BOTTON[0] = ON_LED32;
        timeouts_array_BOTTON[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LEDESP32 = timeouts_array_BOTTON[0];
        niveles_array_BOTTON[1] = OFF_LED32;
        timeouts_array_BOTTON[1] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        break;
    case VACIAR:
        n_pulsos_BOTTON = 8;
        nivel_LED_BOTTON = niveles_array_BOTTON[0] = ON_LED32;
        timeouts_array_BOTTON[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LEDESP32 = timeouts_array_BOTTON[0];
        niveles_array_BOTTON[1] = OFF_LED32;
        timeouts_array_BOTTON[1] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_BOTTON[2] = ON_LED32;
        timeouts_array_BOTTON[2] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_BOTTON[3] = OFF_LED32;
        timeouts_array_BOTTON[3] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_BOTTON[4] = ON_LED32;
        timeouts_array_BOTTON[4] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_BOTTON[5] = OFF_LED32;
        timeouts_array_BOTTON[5] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_BOTTON[6] = ON_LED32;
        timeouts_array_BOTTON[6] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_BOTTON[7] = OFF_LED32;
        timeouts_array_BOTTON[7] = (unsigned long) TIMEOUT_LED32_OFF2S;
        break;
    default:
        break;
    }
}
/**********************************************************************************************
 * *** FUNCIONES: para el manejo del LED del ESP32
 * ***
 * *******************************************************************************************/
short Electrovalv_Coger_Nivel_ESP32 (short contador_pulsos_led_ESP32) {
    return niveles_array_ESP32[contador_pulsos_led_ESP32];
}
unsigned long Electrovalv_Coger_Timeout_ESP32 (short contador_pulsos_led_ESP32) {
    return timeouts_array_ESP32[contador_pulsos_led_ESP32];
}
void Electrovalv_Poner_nivel (short nivel) {
    digitalWrite  (led_sender, nivel);
}
void Electrovalv_LED_ONOFF_ESP32 (unsigned long T_actual) {

    if ((T_actual - timeout_led_ESP32) > timeout_nivel_LEDESP32) {
        timeout_led_ESP32 = T_actual;
        if (contador_pulsos_led_ESP32 >= n_pulsos_ESP32) {
            contador_pulsos_led_ESP32 = 0;
        }
        nivel_LEDESP32          = Electrovalv_Coger_Nivel_ESP32 (contador_pulsos_led_ESP32);    
        timeout_nivel_LEDESP32  = Electrovalv_Coger_Timeout_ESP32 (contador_pulsos_led_ESP32);
        Electrovalv_Poner_nivel (nivel_LEDESP32);
        contador_pulsos_led_ESP32++;
    }
}

void Electrovalv_Setup_LED_ONOFF_ESP32 (int Estado, unsigned long T_actual) {
    contador_pulsos_led_ESP32 = 0;
    timeout_led_ESP32 = T_actual;
    memset (niveles_array_ESP32, 0, sizeof(niveles_array_ESP32));
    memset (timeouts_array_ESP32, 0, sizeof(timeouts_array_ESP32));
    switch (Estado) {
    case INICIAL:       //UN PULSO CONTINUO A 0.
        n_pulsos_ESP32 = 1;
        nivel_LEDESP32 = niveles_array_ESP32[0] = ON_LED32;
        timeouts_array_ESP32[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LEDESP32 = timeouts_array_ESP32[0];
        break;
    case REPOSO:        //UN PULSO ON.
        n_pulsos_ESP32 = 2;
        nivel_LEDESP32 = niveles_array_ESP32[0] = ON_LED32;
        timeouts_array_ESP32[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LEDESP32 = timeouts_array_ESP32[0];
        niveles_array_ESP32[1] = OFF_LED32;
        timeouts_array_ESP32[1] = (unsigned long) TIMEOUT_LED32_OFF2S;
        break;
    case LLENAR:        //TRES PULSOS A ON.
    case LLENAR_1A:
        n_pulsos_ESP32 = 4;
        nivel_LEDESP32 = niveles_array_ESP32[0] = ON_LED32;
        timeouts_array_ESP32[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LEDESP32 = timeouts_array_ESP32[0];
        niveles_array_ESP32[1] = OFF_LED32;
        timeouts_array_ESP32[1] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_ESP32[2] = ON_LED32;
        timeouts_array_ESP32[2] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_ESP32[3] = OFF_LED32;
        timeouts_array_ESP32[3] = (unsigned long) TIMEOUT_LED32_OFF2S;
        break;
    case VINCULAR:      //DOS PULSOS ON.
        n_pulsos_ESP32 = 6;
        nivel_LEDESP32 = niveles_array_ESP32[0] = ON_LED32;
        timeouts_array_ESP32[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LEDESP32 = timeouts_array_ESP32[0];
        niveles_array_ESP32[1] = OFF_LED32;
        timeouts_array_ESP32[1] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_ESP32[2] = ON_LED32;
        timeouts_array_ESP32[2] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_ESP32[3] = OFF_LED32;
        timeouts_array_ESP32[3] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_ESP32[4] = ON_LED32;
        timeouts_array_ESP32[4] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_ESP32[5] = OFF_LED32;
        timeouts_array_ESP32[5] = (unsigned long) TIMEOUT_LED32_OFF2S;
        break;
    case ALARMAS:       //DOS PULSOS A ON.
        n_pulsos_ESP32 = 2;
        nivel_LEDESP32 = niveles_array_ESP32[0] = ON_LED32;
        timeouts_array_ESP32[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LEDESP32 = timeouts_array_ESP32[0];
        niveles_array_ESP32[1] = OFF_LED32;
        timeouts_array_ESP32[1] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        break;
    case VACIAR:
        n_pulsos_ESP32 = 8;
        nivel_LEDESP32 = niveles_array_ESP32[0] = ON_LED32;
        timeouts_array_ESP32[0] = (unsigned long) TIMEOUT_LED32_ON100MS;
        timeout_nivel_LEDESP32 = timeouts_array_ESP32[0];
        niveles_array_ESP32[1] = OFF_LED32;
        timeouts_array_ESP32[1] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_ESP32[2] = ON_LED32;
        timeouts_array_ESP32[2] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_ESP32[3] = OFF_LED32;
        timeouts_array_ESP32[3] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_ESP32[4] = ON_LED32;
        timeouts_array_ESP32[4] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_ESP32[5] = OFF_LED32;
        timeouts_array_ESP32[5] = (unsigned long) TIMEOUT_LED32_OFF200MS;
        niveles_array_ESP32[6] = ON_LED32;
        timeouts_array_ESP32[6] = (unsigned long) TIMEOUT_LED32_ON100MS;
        niveles_array_ESP32[7] = OFF_LED32;
        timeouts_array_ESP32[7] = (unsigned long) TIMEOUT_LED32_OFF2S;
        break;
    default:
        break;
    }
}