# GyroCube: Prototype

Documentation in the root folder overviews the project.

1. Detailed information on cube's hardware/software architecture can be found here: [CLICK](https://github.com/DedUndead/GyroCube-Prototype/blob/master/cube/README.md)<br>
2. Detailed information on hub's hardware/software architecture can be found here: [CLICK](https://github.com/DedUndead/GyroCube-Prototype/blob/master/hub/README.md)<br>
3. Detailed information on web's architecture can be found here: [CLICK](https://github.com/DedUndead/GyroCube-Prototype/blob/master/web/README.md)<br>

Reading the documentation fully allows to understand the architectural decisions and the code better.

#### Developers and contributors:
<b>Pavel Arefyev</b>: Cube's HW/SW architecture, Hub's HW/SW architecture<br>
<b>Joel Kontas</b>: Cube's HW/SW architecture, Zigbee topology<br>
<b>Daniel Liberman</b>: Cube's HW/SW architecture, Web<br>
<b>Alex Franko</b>: Web<br>

# Concept
GyroCube is a Smart-Home accessory. The GyroCube is a multipurpose cube that can perform different functions based on the cube placing. 
Functionality is changed by simply flipping the cube on the table. GyroCube uses vibration modules and LEDs as the output method to notify the user of changes. 
Additionally, user has freedom and flexibility to map and configure side-function relations. This can be done through web interface or mobile application.

GyroCube consists of the main station and cubes that are in constant M2M communication. 
The main station acts as a gateway in personal area network, while the cubes are input/output devices. 
This allows the GyroCube environment to be scalable, as the user can connect as many cubes as he wants. 
The goal is to provide the customer with a delightful looking home accessory, which will also allow them to obtain helpful information in a simple, minimalistic way.

<p align="center"><img src="https://i.imgur.com/4AzbVe5.png" alt="Concept overview"></p>
<p align="center">Figure 1. Concept overview</p>

For instance, sides can be mapped to track the weather, humidity or notifications.
What's more, functions can be assigned to special gestures, such as shaking or tapping.
With the help of additional platforms, for example "If This, Then That", GyroCube can be extended to integrate into already existing smart home environment.

<p align="center"><img src="https://i.imgur.com/WeUtIFV.png" alt="Functionality sketch"></p>
<p align="center">Figure 2. Cube's functionality example</p>

# Project description
The current project presents a prototype for the idea. 
The main goal was to obtain general information about the technologies, that can potentially be used in the actual product and develop a proof of concept in a short period of time.
The conclusions are discussed in the last section of the documentation. Comparing to the original concept, the implementation has limited functionality.

<p align="center"><img src="https://i.imgur.com/GiuHPgV.jpg" alt="Prototype picture"></p>
<p align="center">Figure 3. Prototype picture</p>

### Features summary
*	One cube, one hub and web interface
*	6 configurable sides
*	6 predefined functions to choose from
*	Point-to-Point Zigbee topology
*	Addressable LEDs and Vibration motor outputs
*	Temperature/Humidity sensor, Accelerometer inputs
*	Open case for demonstration purposes
*	Software and interfaces abstraction written for ARM Cortex-M3 (hub) and ARM Cortex-M0+ (cubes)

### Overall architecture

The links in the first section provide additional information about technologies used, justification for component choices and user manuals.<br>

<p align="center"><img src="https://i.imgur.com/whUNJtT.png?1" alt="Project acrhitecture diagram"></p>
<p align="center">Figure 4. Overall project architecture</p>

### Predefined functionality

As stated in the overview, the prototype provides six functions for mapping. 
The main idea was to create a challenge of designing configurable and flexible software architecture.
The presence of 6 configurable functions allowed the team to discover various pitfalls and determine the possible improvements for the future product.

<p align="center">Table 1. Predefined functions description</p>
<div align="center">
  
| Function     | Default side | Description  | Configuration options | 
|--------------|--------------|--------------|-----------------------|
| Idle         | 1 (idx 0) | Does nothing | 
| Simple color | 2 (idx 1) | Displays color | Color |
| Temperature  | 3 (idx 2) | Displays temperature as a gradient from green to blue if current temperature is colder than target, and green to red if warmer  | Target temperature |
| Humidity     | 4 (idx 3) | Displays humidity as a gradient from green to red, where green is the target humidity | Target humidity | 
| Weather      | 5 (idx 4) | Displays color matching the current weather  
| Notification | 6 (idx 5) | Notifies user with vibration/color blink/both when notification arrives | Notification mode, color | 

</div>

The <b>idx</b> denotes index of the function, used in the code.

### Side numbering

Web interface and the cube itself utilizes 1-6 numbering when presented to the user.<br>
However, it is worth mentioning, that in the code they are indexed starting from 0.<br>
For instance, side 3 would have index 2.

Picture of the cube with numbers
  
### ZigBee PAN information

Cube and hub communicate within Personal Area Network using ZigBee protocol.<br>
XBee2 modules and XCTU configuration software were used in order to establish connection between devices.<br>
Both devices are configured to use AT mode for peer-to-peer communication.<br>

<b>PAN ID:</b> 0xFF009AD9F7F60000<br>
<b>Channel scan range:</b> 0x3A0C

To issue updates, the devices use simple string format with identifier characters: `iValueiValue`<br>
List of identifiers:
* s - side/setting, value is side index
* f - target function, value is function index
* c - target color, value is uint32_t in decimal format
* t - target value, value is integer in decimal format / temperature measurement
* w - weather update, value is weather array index* in decimal format (0 - 9)
* n - notification, value is notification mode index (0 - 2)
* h - humidity measurement<br>
\* Weather indexes can be obtained from the web documentation

Examples of messages:<br>
`s0f3c0t30` to cube - map side 1 to humidity function with target value 30<br>
`w0` to cube - weather changed to index 0<br>
`n` to cube - notification arrived<br>
`h30t25` to hub - humidity is 30%, temperature is 25 C
