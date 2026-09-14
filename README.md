# esp32-voice-snake

Edge AI on an ESP32: gesture recognition from an IMU and keyword spotting from a
microphone, used to control a Snake game. No cloud, all inference runs on the
microcontroller.

## Hardware

| Part | Details |
|---|---|
| Board | ESP32 DevKit, chip ESP32-D0WD-V3 (dual-core 240 MHz, 320 KB RAM, 4 MB flash) |
| IMU | GY-521 module (sold as MPU6050, chip reports MPU6500, WHO_AM_I 0x70), I2C on SDA=23 SCL=22 |
| Microphone | INMP441 I2S MEMS mic (coming next) |
| Toolchain | PlatformIO, Arduino framework, esptool |

## Progress

- [x] Day 1: toolchain set up, blink + serial test flashed and verified at 115200 baud
- [x] Day 2: IMU register-level driver (no library), 6-axis CSV stream at 50 Hz
- [ ] Record gesture dataset on device
- [ ] Train and quantize gesture classifier, run on the ESP32
- [ ] I2S microphone capture
- [ ] Record dataset on device
- [ ] Train and quantize keyword-spotting model (int8)
- [ ] Run inference on the ESP32 with TensorFlow Lite Micro
- [ ] Trigger Snake game over serial

## Build numbers

| Stage | RAM used | Flash used |
|---|---|---|
| Blink + serial | 21,464 B (6.6%) | 269,133 B (20.5%) |
| IMU driver, 50 Hz CSV | 21,800 B (6.7%) | 287,337 B (21.9%) |

## Targets

| env | what it does |
|---|---|
| `blink` | LED + serial sanity test |
| `imu` | streams `ax,ay,az,gx,gy,gz` as CSV, accel in g, gyro in deg/s |
| `i2cscan` | finds which GPIO pair the IMU is wired to. Saved me when SDA was on the wrong row |

## Run it

```
pio run -e imu -t upload
pio device monitor
```

Stop the monitor with Ctrl+C before uploading. The flasher and the monitor
cannot share the USB port.
