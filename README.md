# GyroCube: Prototype

### Concept
GyroCube is a Smart-Home accessory. The GyroCube is a multipurpose cube that can perform different functions based on the cube placing. 
Functionality is changed by simply flipping the cube on the table. GyroCube uses vibration modules and LEDs as the output method to notify the user of changes. 
Additionally, user has freedom and flexibility to map and configure side-function relations. This can be done through web interface or mobile application.

GyroCube consists of the main station and cubes that are in constant M2M communication. 
The main station acts as a gateway in personal area network, while the cubes are input/output devices. 
This allows the GyroCube environment to be scalable, as the user can connect as many cubes as he wants. 
The goal is to provide the customer with a delightful looking home accessory, which will also allow them to obtain helpful information in a simple, minimalistic way.

<p align="center"><img src="https://i.imgur.com/4AzbVe5.png" alt="alternatetext"></p>
<p align="center">Figure 1. Concept overview</p>

For instance, sides can be mapped to track the weather, humidity or notifications.
What's more, functions can be assigned to special gestures, such as shaking or tapping.
With help of additional platforms, for example "If This, Then That", GyroCube can be extended to integrate into already existing smart home environment.

<p align="center"><img src="https://i.imgur.com/WeUtIFV.png" alt="alternatetext"></p>
<p align="center">Figure 2. Cube's functionality example</p>

### Project description
The current project presents a prototype for the idea. 
The main goal was to obtain general information about the technologies, that can potentially be used in the actual product and develop a proof of concept in a short period of time.
The conclusions are discussed in the last section of the documentation. Comparing to the original concept, the implementation has limited functionality.

Picture of the project.

#### Features summary
*	One cube, one hub and web interface
*	6 configurable side
*	6 predefined functions to choose from
*	Point-to-Point Zigbee topology
*	Addressable LEDs and Vibration motor outputs
*	Temperature/Humidity sensor, Accelerometer inputs
*	Open case for demonstration purposes
*	Software and interfaces abstraction written for ARM Cortex-M3 (hub) and ARM Cortex-M0+ (cubes)

#### Overall architecture

Detailed information on hub's hardware/software architecture can be found here:<br>
Detailed information on cube's hardware/software architecture can be found here:<br>
Detailed information on web's architecture can be found here:<br>

The links above provide additional information about technologies used, jastification for components choices and user mannuals.
Data formats for communications are also described there.

<p align="center"><img src="https://i.imgur.com/AKczasR.png" alt="alternatetext"></p>
<p align="center">Figure 3. Overall project architecture</p>

