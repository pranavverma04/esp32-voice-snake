// I2C bus scanner: lists every device address that responds on SDA=21, SCL=22.
#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
}

void loop() {
  int found = 0;
  Serial.println("Scanning I2C bus...");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  device at 0x%02X\n", addr);
      found++;
    }
  }
  Serial.printf("%d device(s) found\n\n", found);
  delay(2000);
}
