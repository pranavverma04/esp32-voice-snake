// I2C pin finder, pass 2: SDA fixed on 23, find SCL with a full pin reset between tries.
#include <Arduino.h>
#include <Wire.h>
#include "driver/gpio.h"

static const int PINS[] = {21, 22, 19, 18, 23, 5, 4, 2, 15, 25, 26, 27, 14, 12, 13, 32, 33};
static const int NPINS = sizeof(PINS) / sizeof(PINS[0]);
static const int SDA_PIN = 23;

static void resetAll() {
  Wire.end();
  for (int i = 0; i < NPINS; i++) { gpio_reset_pin((gpio_num_t)PINS[i]); pinMode(PINS[i], INPUT); }
  delay(20);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("SCL search with SDA=23");
  for (int j = 0; j < NPINS; j++) {
    if (PINS[j] == SDA_PIN) continue;
    resetAll();
    Wire.begin(SDA_PIN, PINS[j], 100000);
    Wire.beginTransmission(0x68);
    if (Wire.endTransmission() == 0) Serial.printf("  FOUND 0x68 with SDA=23 SCL=GPIO%d\n", PINS[j]);
  }
  resetAll();
  Serial.println("Pin search done.");
}

void loop() { delay(5000); }
