#include <esp_system.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/timer.h>

#define PIN_BOTON  0   // Pin del botón
#define PIN_LED    2   // Pin del LED

hw_timer_t *temporizador1 = NULL;  // Temporizador para calcular el tiempo del botón

hw_timer_t *temporizador2 = NULL;  // Temporizador para el parpadeo

volatile bool botonPresionado = false;
volatile uint32_t tiempoPresionado = 0;
volatile uint32_t tiempoRestante = 0;
volatile bool parpadeando = false;
TaskHandle_t tareaLedHandle = NULL;

 // ISR para detectar botón presionado

void IRAM_ATTR botonISR() {

if (digitalRead(PIN_BOTON) == LOW) {
    timerWrite(temporizador1, 0);
    timerStart(temporizador1);
    botonPresionado = true;
    digitalWrite(PIN_LED, HIGH);

    } else {

    timerStop(temporizador1);
    tiempoPresionado = timerReadMicros(temporizador1) / 1000;
    tiempoRestante = tiempoPresionado;
    botonPresionado = false;
    digitalWrite(PIN_LED, LOW);
    parpadeando = true;
    xTaskNotifyFromISR(tareaLedHandle, 1, eSetValueWithOverwrite, NULL);
    }
   }
}

// Manejador de parpadeo del LED con temporizador

 void IRAM_ATTR temporizador2ISR() {
      if (parpadeando && tiempoRestante > 0) {
        digitalWrite(PIN_LED, !digitalRead(PIN_LED));
        tiempoRestante -= 100;

    } else {

        parpadeando = false;
        digitalWrite(PIN_LED, LOW);
        timerStop(temporizador2);
    }
}

// Función de parpadeo de LED

 void tareaLed(void *parametro) {
      while (1) {
        uint32_t notificacion;
        if (xTaskNotifyWait(0, 0, &notificacion, portMAX_DELAY) == pdTRUE) {
            if (parpadeando) {
                timerWrite(temporizador2, 0);
                timerAlarmWrite(temporizador2, 100000, true);
                timerStart(temporizador2);
            }
        }
}

void setup() {

    pinMode(PIN_BOTON, INPUT_PULLUP);
    pinMode(PIN_LED, OUTPUT);
    attachInterrupt(PIN_BOTON, botonISR, CHANGE);

            temporizador1 = timerBegin(0, 80, true);
            timerStop(temporizador1);

                 temporizador2 = timerBegin(1, 80, true);
                 timerAttachInterrupt(temporizador2, &temporizador2ISR, true);
                 timerAlarmEnable(temporizador2);

    xTaskCreate(tareaLed, "parpadeo LED", 1024, NULL, 1, &tareaLedHandle);
}
