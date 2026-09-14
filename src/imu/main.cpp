// MPU6050 / MPU6500 register-level test on ESP32 (no library).
// Prints ax,ay,az,gx,gy,gz as CSV at 50 Hz. Accel in g, gyro in deg/s.
// Wiring: VCC->3V3, GND->GND, SCL->GPIO22, SDA->GPIO23

#include <Arduino.h>
#include <Wire.h>

static const uint8_t MPU_ADDR      = 0x68;  // AD0 low
static const uint8_t REG_PWR_MGMT1 = 0x6B;
static const uint8_t REG_WHO_AM_I  = 0x75;
static const uint8_t REG_ACCEL_OUT = 0x3B;  // 14 bytes: accel(6) temp(2) gyro(6)

static const int SAMPLE_HZ = 50;

static void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

static uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)1);
  return Wire.read();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(23, 22);        // SDA, SCL
  Wire.setClock(400000);

  delay(100);
  // WHO_AM_I: 0x68 = MPU6050, 0x70 = MPU6500, 0x71 = MPU9250 (same accel/gyro registers)
  uint8_t id = readReg(REG_WHO_AM_I);
  if (id != 0x68 && id != 0x70 && id != 0x71) {
    Serial.printf("IMU not found, WHO_AM_I=0x%02X. Check wiring.\n", id);
    while (true) delay(1000);
  }
  writeReg(REG_PWR_MGMT1, 0x00);   // wake up, internal 8 MHz clock
  delay(50);
  // No header line: the Edge Impulse data forwarder expects numeric CSV only.
}

void loop() {
  static uint32_t next = 0;
  uint32_t now = millis();
  if (now < next) return;
  next = now + 1000 / SAMPLE_HZ;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(REG_ACCEL_OUT);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)14);

  int16_t raw[7];
  for (int i = 0; i < 7; i++) {
    raw[i] = (int16_t)((Wire.read() << 8) | Wire.read());
  }
  // Default ranges: accel +-2 g -> 16384 LSB/g, gyro +-250 dps -> 131 LSB/dps
  float ax = raw[0] / 16384.0f, ay = raw[1] / 16384.0f, az = raw[2] / 16384.0f;
  float gx = raw[4] / 131.0f,   gy = raw[5] / 131.0f,   gz = raw[6] / 131.0f;

  Serial.printf("%.3f,%.3f,%.3f,%.2f,%.2f,%.2f\n", ax, ay, az, gx, gy, gz);
}
