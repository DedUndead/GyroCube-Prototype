# GyroCube: Prototype

### Concept
GyroCube is a Smart-Home accessory. The GyroCube is a multipurpose cube that can perform different functions based on the cube placing. 
Functionality is changed by simply flipping the cube on the table. GyroCube uses vibration modules and LEDs as the output method to notify the user of changes. 
Additionally, user has freedom and flexibility to map and configure side-function relations. This can be done through web interface or mobile application.

GyroCube consists of the main station and cubes that will be in constant M2M communication. 
The main station acts as a gateway in personal area network, while the cubes act as input/output devices. 
This allows the GyroCube environment to be scalable, as the user can connect as many cubes as he wants. 
The goal is to provide the customer with a delightful looking home accessory, which will also allow them to obtain helpful information in a simple, minimalistic way.

### Project description
The current project presents a prototype for the idea. 
The main goal was to obtain general information about the technologies, that can potentially be used in the actual product and develop a proof of concept in a short period of time.
The conclusions are discussed in the last section of the documentation. Comparing to the original concept, the implementation has limited functionality.

### Features summary
*	One cube, one hub and web interface
*	6 configurable side
*	6 predefined functions 
*	Point-to-Point Zigbee topology
*	Addressable LEDs and Vibration motor outputs
*	Temperature/Humidity sensor, Accelerometer inputs
*	Open case for demonstration purposes
*	Software and interfaces abstraction written for ARM Cortex-M3 (hub) and ARM Cortex-M0+ (cubes)

