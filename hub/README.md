# GyroCube: Hub Architecture

The hub is not encapsulated into the case.<br>
It is an LPC1549 development board with WI-FI shield provided by Metropolia University of Applied Sciences.<br>
RF Module is connected directly to the pin headers of the board.

Hub acts as a bridge between LAN and PAN, connecting web interface and cubes together.

Project structures are generated using MCUXpresso IDE.
Source project is inside **hub_main** folder, while other projects are hardware libraries provided by NXP for ARM-Cortex M3.
