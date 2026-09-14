# esp32-voice-snake

On-device keyword spotting on an ESP32, used to control a Snake game by voice.
No cloud, all inference runs on the microcontroller.

## Hardware

| Part | Details |
|---|---|
| Board | ESP32 DevKit, chip ESP32-D0WD-V3 (dual-core 240 MHz, 320 KB RAM, 4 MB flash) |
| Microphone | INMP441 I2S MEMS mic (coming next) |
| Toolchain | PlatformIO, Arduino framework, esptool |

## Progress

- [x] Day 1: toolchain set up, blink + serial test flashed and verified at 115200 baud
- [ ] I2S microphone capture
- [ ] Record dataset on device
- [ ] Train and quantize keyword-spotting model (int8)
- [ ] Run inference on the ESP32 with TensorFlow Lite Micro
- [ ] Trigger Snake game over serial

## Build numbers

| Stage | RAM used | Flash used |
|---|---|---|
| Blink + serial | 21,464 B (6.6%) | 269,133 B (20.5%) |

## Run it

```
pio run -t upload
pio device monitor
```

Stop the monitor with Ctrl+C before uploading. The flasher and the monitor
cannot share the USB port.
