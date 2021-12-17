# GyroCube: Cube Architecture

Documentation related to Cube device as a part of GyroCube environment.

# Hardware architecture

### Main attachments

Cube is equipped with accelerometer, humidity/temperature sensor, RF module, vibration motor, addressable LEDs and ARM Cortex-M0+ based MCU.

<p align="center"><img src="https://i.imgur.com/OeaccgP.png" alt="Concept overview"></p>
<p align="center">Figure 1. Hardware architecture overview</p>

The schematics below describes the hardware architecture of the cube's setup.<br>
Click to view the image in full resolution.

<p align="center"><img src="https://i.imgur.com/ZMvgTZd.png" alt="Concept overview"></p>
<p align="center">Figure 2. Cube schematics </p>

### List of components

Table 1. List of components

| Component         | Name            | Datasheet                                                                                              |  
|-------------------|-----------------|--------------------------------------------------------------------------------------------------------|
| MCU               | RPi Pico        | [Link](https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf)                                     |
| RF Module         | Xbee 2          | [Link](http://www.farnell.com/datasheets/27606.pdf)                                                    |
| Accelerometer     | I2C-ACC-8700    | [Link](https://www.gravitech.us/i2c3liacand3.html)                                                     |
| Temp/Humid sensor | HIH6020-021-001 | [Link](https://www.mouser.fi/datasheet/2/187/hih6000-datasheet-009073-7-en-1140713.pdf)                |
| Addressable LEDs  | WS2812b         | [Link](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)                                           |
| Vibration Motor   | C1026B002F      | [Link](https://www.mouser.fi/datasheet/2/321/28821-Flat-Coin-Vibration-Motor-Documentation-369707.pdf) |

### Justification for components choice


