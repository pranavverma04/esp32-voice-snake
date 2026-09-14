// MPU6050 / MPU6500 register-level driver on ESP32 (no library).
// Streams ax,ay,az,gx,gy,gz as CSV at 50 Hz. Accel in g, gyro in deg/s.
// Wiring: VCC->3V3, GND->GND, SCL->GPIO22, SDA->GPIO23
//
// Robust to a loose wire: if an I2C read fails, nothing is printed and the
// driver re-initialises the sensor when it comes back. Only numeric lines are
// ever printed, so the Edge Impulse data forwarder never sees junk.

#include <Arduino.h>
#include <Wire.h>

static const uint8_t MPU_ADDR      = 0x68;  // AD0 low
static const uint8_t REG_PWR_MGMT1 = 0x6B;
static const uint8_t REG_WHO_AM_I  = 0x75;
static const uint8_t REG_ACCEL_OUT = 0x3B;  // 14 bytes: accel(6) temp(2) gyro(6)

static const int SDA_PIN = 23;
static const int SCL_PIN = 22;
static const int SAMPLE_HZ = 50;

static bool imuReady = false;

static bool writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

static bool readRegs(uint8_t reg, uint8_t *buf, uint8_t n) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(MPU_ADDR, n) != n) return false;
  for (uint8_t i = 0; i < n; i++) buf[i] = Wire.read();
  return true;
}

// Returns true if the sensor answered with a known ID and woke up.
static bool imuInit() {
  uint8_t id = 0;
  if (!readRegs(REG_WHO_AM_I, &id, 1)) return false;
  // 0x68 = MPU6050, 0x70 = MPU6500, 0x71 = MPU9250: same accel/gyro registers
  if (id != 0x68 && id != 0x70 && id != 0x71) return false;
  if (!writeReg(REG_PWR_MGMT1, 0x00)) return false;   // wake, internal clock
  delay(50);
  return true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);
  Wire.setTimeOut(5);        // ms; fail fast when a wire is loose
  delay(100);
  imuReady = imuInit();
}

void loop() {
  static uint32_t next = 0;
  static uint32_t lastRetry = 0;
  uint32_t now = millis();

  if (!imuReady) {
    if (now - lastRetry >= 500) {   // try to reconnect twice a second
      lastRetry = now;
      imuReady = imuInit();
    }
    return;
  }

  if (now < next) return;
  next = now + 1000 / SAMPLE_HZ;

  uint8_t buf[14];
  if (!readRegs(REG_ACCEL_OUT, buf, 14)) {
    imuReady = false;             // wire dropped: go quiet, retry in loop()
    return;
  }

  int16_t raw[7];
  for (int i = 0; i < 7; i++) raw[i] = (int16_t)((buf[2 * i] << 8) | buf[2 * i + 1]);

  // Default ranges: accel +-2 g -> 16384 LSB/g, gyro +-250 dps -> 131 LSB/dps
  float ax = raw[0] / 16384.0f, ay = raw[1] / 16384.0f, az = raw[2] / 16384.0f;
  float gx = raw[4] / 131.0f,   gy = raw[5] / 131.0f,   gz = raw[6] / 131.0f;

  Serial.printf("%.3f,%.3f,%.3f,%.2f,%.2f,%.2f\n", ax, ay, az, gx, gy, gz);
}
