# GyroCube: Cube Architecture

The prototype resides in a 3d printed open case in a shape of a cube with 2 sides missing.<br>
The open case design was developed to quickly present the hardware organization and main components during the demo.<br>
The presense of 4 solid black 4mm-wide plastic walls allows the cube to be stable on each side.<br>
The USB connector of the microcontroller is available on the bottom of the main side, allowing the cube to be powered and still operate comfortably on most of the orientations.

The soldering breadboard that has the "body" of the device is elevated to 4mm from the bottom. Accelerometer, temperature sensor, motor, connectors for RF module and LEDs lie on the breadboard. RF module is placed on one of the perpendicular sides.

Dimensions of the cube are 80x80x80 mm.

Picture of the cube

As stated above, the prototype must be always powered.<br>
In case the network connection goes down, the device is capable to operate in standalone mode with previously configured settings or defaults.<br>

Cube is controlled by flipping. The active side is the one facing upwards.<br>
The default functions are described in general documentation section (root source folder).<br>
To reset the device, replug the cube.

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

##### MCU - Paspberry Pi Pico
The cube required a small microcontroller that would fit inside together with other components, potentially allowing for a proper light and weight distribution.
RPi Pico is a great candidate, as long as it is based on a popular processor - ARM Cortex-M0+, meaning it won't be difficult to find a replacement during the production phase.
Additionally, Pico is a decent MCU for bare metal C/C++ programming, having wide range of hardware libraries publicly available. 
Pinout flexibility comes handy when it comes to designing the architecture for a small device. Special PIO hardware functionality gives an opportunity to integrate NeoPixel addressable LEDs with simplicity.

##### RF Module - XBee 2
XBee2 modules can be configured to operate with different wireless protocols using XCTU software. Sending/Receiving data and configuration of the device are handled via UART, meaning it doesn't present a difficult programming challenge to establish the communication. ZigBee was chosen to be the target PAN protocol, because of its scalability, self-management and power consumption. The module on the cube is configured to operate in AT mode, endpoint.

##### Accelerometer - I2C-ACC-8700
The name "GyroCube" comes from the fact, that gyroscope is  meant to be used in the device. However, for the prototype no special gestures were planned, so the accelerometer was enough to track the sides and changes.<br>
I2C-ACC-8700 is a breakout board for FXOS8700CQ accelerometer/magnetometer chip. FXOS8700CQ provides wide functionality and application cases. Fast read mode configuration was utilized in this project. What's more, the chip has several embedded functions, including the orientation detection and other algorithms, that would require complicated processing. Some of the functions were tried out and put on hold, but FXOS8700CQ will definitely be used in later stages of the projects.

##### Temperature/Humidity sensor - HIH6020-021-001
Honeywell sensors are of the highest quality. One of the main features is very low power consumpion, that is achieved with auto-sleep mode when no data is requested. Because the architecture was designed to use I2C interface for all sensors, HIH6020 works on the same bus with the accelerometer, occupying less pins on the microcontroller. The resolution is decent for home-held device.

##### Addressable LEDs - NeoPixel (WS2812b)
One of the hardest challenges of the concept is to provide even light distribution to all of the sides. NeoPixel LEDs are very bright, considering its low power consumption, therefore they were considered as the main candidate during design phase.Even though none of the LEDs have different colors at the same time in the prototype, the features of addressing individual strip pieces were researched for further use in the project.

#####  Vibration Motor - C1026B002F
Simple vibration motor placed on one of the cube sides. 9000 rpm is a speed that is within mobile phone vibration range that most people are used to.

# Software architecture

The software provides an abstraction layers for all the peripherals, state machine for handling function and side changing and main body for issuing events and managing communication.

The code is written with the help of Pico SDK hardware libraries. Documentation and examples of Pico SDK usage are found in official [documentation](https://raspberrypi.github.io/pico-sdk-doxygen/).

<b>Note:</b> In order to build the source code provided in the repository, Pico SDK path must be added to PICO_SDK_PATH cmake build/config environmental variables.

<p align="center"><img src="https://i.imgur.com/uRmOFMd.png" alt="Concept overview"></p>
<p align="center">Figure 3. Software architecture overview </p>

### Main body

Main body performs accelerometer measurements, issues event ticks and polls for ZigBee data periodically.<br>
Periodicity is implemeted using hardware timers.

Table 2. Main body functionality description

| Function                                           | Periodicity              |
|----------------------------------------------------|--------------------------|
| Accelerometer measurements (current side tracking) | Sample / 1000ms          |
| State machine tick                                 | Tick / 500 ms            |
| Send measurements and current side to hub          | Packet / 1000ms          |
| Listen to Zigbee data                              | Polling with 10 ms delay |

Accelerometer measurements are checked with a simple running filter. The change of side is considered to be valid only if two consecutive samples represent the same measurement. When the cube is flipped, main body issues <b>eChange</b> event with new side value.

State machine tick issues <b>eTick</b> event.

Zigbee data analysis performs different action depending on the message. If change in setting is requested, state machine's member function for setting changed is called passing the data fetched from the message. Weather change acts in the similar way. The notification message issues <b>eNotify</b> event to the state machine.

### State machine

State machine source code is available in src/state_machine/Gyrocube.cpp

State machine consists of seven states: startup state and six states for each induvidual function.<br>

Settings are stored in the private array:<br>
```
typedef struct side_settings {
    uint8_t function;
    uint32_t color;
    int target;
} side_settings;
side_settings settings[N_SIDES];
```
Function denotes index of the current function mapped to the side (in accordance to indexes in general documentation).<br>
Color denotes the color used for the function if applicable.<br>
Target denotes the target value for the function if applicable (for example target humidity).

All "functional" states (states that describe one of the predefined functions) are stored in the member function pointer array:<br>

```
void (Gyrocube::*functional_states[N_SIDES])(const Event&);
functional_states { 
    &Gyrocube::state_idle,    &Gyrocube::state_lamp,
    &Gyrocube::state_temp,    &Gyrocube::state_humid,
    &Gyrocube::state_weather, &Gyrocube::state_notification
}
```

As long as the user can map any function to any side, this set up allows to get into the required state for the side that the cube was placed on.
Here is the example of such state switching:
```
case Event::eChange:
    current_side = e.value;
    set_state(settings[current_side].function);

    break;
```

<p align="center"><img src="https://i.imgur.com/tEXbYmk.png" alt="Concept overview"></p>
<p align="center">Figure 4. State machine UML sketch </p>

State machine can be initialized in a standalone mode, skipping the startup states and requests for reconnection, or in zigbee mode. In latter option, if no acknowledgement is received for a long time, the state machine goes back to startup state, where it smoothly blinks blue until the network is restored.<br>
<b>Note:</b> In current implementation no acknowledgement is present, therefore the machine is initialized with standalone option.

All the states check on tick if the settings were updated, adjusting the execution or switching to another state:
```
case Event::eTick:
    update_measurements();

    if (function_changed) {
        function_changed = false;
        set_state(settings[current_side].function);
    }

    // Change color if update in settings is present
    if (update_required) {
        leds->fill(settings[current_side].color);
        update_required = false;
    }

    break;
```
On each tick event, state machine makes new humidity/temperature measurements that will be later send to hub.<br>
On entrance event, all the support flags are being cleared.<br>
On exit event, actuators are brought back to off state.
