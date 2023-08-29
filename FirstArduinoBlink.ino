#include <Arduino_FreeRTOS.h>
#include <semphr.h> // FreeRTOS Semaphore kütüphanesi eklendi

const int pin1 = 2;
const int pin2 = 3;
const int pin3 = 4;
const int pin4 = 5;
int flag = 0;

bool stopProgram = false;
SemaphoreHandle_t stopMutex;

void stopTask(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    if (Serial.available()) {
      char input = Serial.read();
      if (input == 's' || input == 'S') { // Eğer 's' veya 'S' tuşuna basıldıysa
        xSemaphoreTake(stopMutex, portMAX_DELAY);
        stopProgram = true;
        xSemaphoreGive(stopMutex); 
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100)); // Küçük bir gecikme ekleyerek sürekli kontrolü engelle
  }
}

void taskFunction(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    xSemaphoreTake(stopMutex, portMAX_DELAY);
    bool shouldStop = stopProgram;
    xSemaphoreGive(stopMutex);

    if (!shouldStop) {
      digitalWrite(pin1, HIGH);
      vTaskDelay(pdMS_TO_TICKS(500));
      digitalWrite(pin1, LOW);
      vTaskDelay(pdMS_TO_TICKS(100));

      digitalWrite(pin2, HIGH);
      vTaskDelay(pdMS_TO_TICKS(1000));
      digitalWrite(pin2, LOW);
      vTaskDelay(pdMS_TO_TICKS(100));

      digitalWrite(pin3, HIGH);
      vTaskDelay(pdMS_TO_TICKS(1500));
      digitalWrite(pin3, LOW);
      vTaskDelay(pdMS_TO_TICKS(100));

      digitalWrite(pin4, HIGH);
      vTaskDelay(pdMS_TO_TICKS(2000));
      digitalWrite(pin4, LOW);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}
void vLedTask(void *pvParameters) {
  while (1) {
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
      if (flag == 0) {
        flag = 1;
        digitalWrite(pin3, HIGH);
      } else {
        flag = 0;
        digitalWrite(pin3, LOW);
      }
    }
  }
}
void buttonInterrupt() {
  // Semaforu vererek pin3 ü tetikliyoruz
  xSemaphoreGiveFromISR(xSemaphore, NULL);
}
void setup() {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(4, INPUT);
  Serial.begin(9600);

  stopMutex = xSemaphoreCreateMutex();
  xSemaphore = xSemaphoreCreateBinary();
  xTaskCreate(taskFunction, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(stopTask, "StopTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  xTaskCreate(vLedTask, "LedTask", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

  // Interrupt işleyyiciyi bağlıyorum
  attachInterrupt(digitalPinToInterrupt(4), buttonInterrupt, FALLING);
  //attachInterrupt(digitalPinToInterrupt(pin), ISR, mode);
  vTaskStartScheduler();
}

void loop() {
}
