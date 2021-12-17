# GyroCube: Hub Architecture

The hub is not encapsulated into the case.<br>
It is an LPC1549 development board with WI-FI shield provided by Metropolia University of Applied Sciences.<br>
RF Module is connected directly to the pin headers of the board.

Picture of a hub

Hub acts as a bridge between LAN and PAN, connecting web interface and cubes together.

Project structures are generated using MCUXpresso IDE.
Source project is inside **hub_main** folder, while other projects are hardware libraries provided by NXP for ARM-Cortex M3.

# Hardware architecture

### Main attachments

<p align="center"><img src="https://i.imgur.com/SYuSDM2.png" alt="Functionality sketch"></p>
<p align="center">Figure 1. Hardware architecture overview </p>

### List of components

Table 1. List of components

| Component         | Name            | Datasheet                                                                                         |  
|-------------------|-----------------|---------------------------------------------------------------------------------------------------|
| MCU               | LPC1549         | [Link](https://www.nxp.com/docs/en/data-sheet/LPC15XX.pdf)                                        |
| RF Module         | Xbee 2          | [Link](http://www.farnell.com/datasheets/27606.pdf)                                               |
| Wi-Fi Module      | ESP8266         | [Link](https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf) |
