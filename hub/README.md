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

### Justification for components choice

##### MCU - LPC1549

LPC1549 is a development board supported by NXP. The hardware libraries and examples allow to quickly prototype the bridge software that the project needed. Variety of peripherals help to achieve the target as well, for instance using RGB LED as an indicator for different errors/events.

##### RF Module - XBee 2
XBee2 modules can be configured to operate with different wireless protocols using XCTU software. Sending/Receiving data and configuration of the device are handled via UART, meaning it doesn't present a difficult programming challenge to establish the communication. ZigBee was chosen to be the target PAN protocol, because of its scalability, self-management and power consumption. Hub is a device that works as a gateway for all devices in PAN. The module on the hub is configured to operate in AT mode, coordinator. 

##### Wi-Fi Module - ESP8266
ESP8266 shield provided by Metropolia University of Applied Sciences is a handy extension for the board. MQTT protocol is being used to communicate with the web, and the team members have already worked with ESP8266 and MQTT arduino-ported library for this hardware. This is another factor that helped to quickly implement the ideas in the code.

# Software architecture

The software provides abstraction layers for MQTT and UART interfaces. Main body of the program is constantly polling for ZigBee data. MQTT interface takes a callback function as a message handler, so no explicit polling is present.

### MQTT data format

The hub expects the data to arrive as a JSON string.<br>
There are several different messages that can be sent to the hub.<br>

Settings update:
```js
{
  side: 0-5,          // Target side to update
  func: 0-5           // Function to map
  color: uint32_t     // Color in decimal format
  target: int         // Target in decimal format
}
```

Weather update:
```js
{
  weather: 0-9        // New weather index
}
```

Notification:
```js
{
  notif: true
}
```

The hub sends only one type of JSON string, containing the measurements that arrived via ZigBee:
```js
{
  side: 0-5,           // Current side
  humidity: 0-100      // Current humidity, %
  temperature: -40-120 // Current temperature, C
}
```

### Bridging

Bridging is the term the project uses to describe reforwarding of the messages from PAN to LAN and vice versa.

Because the cube does not utilize any JSON parsers and implies lighter processing, the idea behind bridging is simplifying JSON strings into string with identifiers, that are described in the general documentaion page.

Examples of bridging:

```
{ side: 1, func: 5, color: 0, target: 2 } --> s1f5c0t2
```
```
s0h20t21 --> { side: 0, humidity: 20, temperature: 21 }
```

### Data samples filtering

Because of the strict UART timing and additional interference, some data samples arrive invalid. Therefore, hub performs simple sanity check for a data sample. In case the measurement seems to be invalid, the cube's side value is replaces with -1.<br>

```c++
if (sscanf(sample_str, "s%dh%dt%d", &current_side, &humidity, &temperature) != 3 ||
  temperature < MIN_TEMP || current_side > MAX_SIDE) {
  current_side = -1;
  humidity = -1;
  temperature = -1;
}
```

The reason the invalid messages are not dropped, is a potential oppotunity to use it for statistic. For instance, keeping track of interference peaks or calculating data loss percentage.

# Credit

UART abstraction class: Keijo Länsikunnas, Metropolia University of Applied Sciences<br>
JSON parser: [nhlohmann](https://github.com/nlohmann/json)
