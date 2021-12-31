# Description
GyroCube is a Smart-Home accessory. The GyroCube is a multipurpose cube that can perform different functions based on the cube placing. 
The functionality is changed by simply flipping the cube on the table. The GyroCube features are Smart lamp, temperature-/ humidity measurement, weather forecast, and email notifications. 
The user can modify the cube these features using the web user interface. 
The web interface consists of setting configurations for the cube, the features that can be adjusted are:
-	Smart lamp
-	Temp tracker
-	Humidity
-	Notifications
-	Weather
-	Idle
Each face will consist of its own unique options that the user could change. 

# Technology stack
<b>Frontend</b>
*	Tailwind
*	EJS
*	ThreeJS

<b>Backend</b>
*	NodeJS
*	MQTT
*	SocketIO

<b>Modules</b>
| Module	| Description				      		 	 |
| ------------- | :--------------------------------------------------------------|			      
| async-mqtt 	| JavaScript client library for MQTT protocol 		 	 |
| EJS		| Template language to allow JavaScript in HTML markup   	 |
| socket.io 	| handles real time bidirectional event-based	         	 |
| nodemon  	| module to assist NodeJS application development	 	 |
| express 	| Minimal and flexible Node.js web application framework 	 |
| ThreeJS 	| Creating and displaying animated 3D models based on WebGL	 |
| Tailwind 	| CSS framework to build UI				         |


# User Manual
The web interface currently consists of two main pages:
-	Landing page – first figure
-	Cube settings page – second figure

<p align="center"><img src="https://i.imgur.com/Bd2FRYd.png" width=800 height=600 alt="Landing page"></p>
<p align="center">Figure 1. Landing page</p>

<p align="center"><img src="https://i.imgur.com/lDc07yR.png" width=800 height=600 alt="Cube settings page"></p>
<p align="center">Figure 2. Cube settings page</p>


When the user enters the web interface, he will be interacted with the landing page. The landing page consists of a description of the project, along with a toolbar ‘Home’ and ‘Settings’. The Home tab will redirect the user back to the landing page, whereas the settings tab will lead to the cube settings. 
Settings
Opening the ‘Settings’ tab it will open a drop-down menu that currently has only one cube. By clicking on “Cube” it will redirect the user to the cube settings configuration.
Then choose the feature that you want to change from the functionality options, the options are: 
-	Idle
-	Simple color
-	Temp tracker
-	Humidity tracker
-	Notifications
-	Weather forecast

It’s important to note that each of these features have the possibility to choose a side that the setting will apply on. 
In addition, by pressing submit, you’ll submit the new configuration to the cube.

<b>Idle</b><br>
The idle feature is a feature whereas the name states that side will be idle. The idea of that feature is for later development, where the cube potentially has a wireless charger option. So, that side will be fixed into idle mode.

<b>Simple color</b><br>
The simple color as a smart lamp, where you can configure the side of operation in the cube and the color the LEDs will operate.

<b>Temp tracker</b><br>
The temperature tracker is used as a temperature measurement, where you configure the side of operation of the temperature tracker and your ideal temperature target. 

<b>Humidity tracker</b><br>
The humidity tracker is used as a humidity measurement, where you configure the side of operation of the humidity tracker and your ideal humidity target.

<b>Notifications</b><br>
The notifications feature is used as a baseline for connecting that feature to email or to stocks platform to get notifications on certain changes. However, the current notifications are choosing a side of operation, and the way of notifying which are:
-	Vibration
-	Lights
-	Both

In addition, the color of the lights when a notification is received. To check the functionality press PINGPONG. 

<b>Weather forecast</b><br>
The weather forecasts feature is used by setting a city where you want to check that locations’ weather. By setting a city and submitting the side where you want that feature to operate the cube will shine according to the weather condition.

In addition to changing the features of the cube, it’s possible to get live feed from the cubes’ current settings, such as:
-	The current side that the cube is operating
-	Current function on that side
-	Temperature surrounding the cube
-	Humidity surrounding the cube
-	Current weather on the chosen city – Helsinki is the default city

# Usage
The GyroCubes’ faces functionality is configurable by user-based features that can be set up from the web interface. By submitting the new settings, the web interface will send the new configuration via web socket from the client side which is the web interface to the server side. 
Web sockets enables the real-time transfer of data between the client side to the server side of the web application and vice-versa.  
As mentioned earlier, the web interface sends the new configurations via web sockets. 
We use the web socket to also pass the current stats that the GyroCube displays to display it on the client side. In addition, the web socket is used to check the notification feature functionality by “pinging” the GyroCube.
Once the new configuration settings arrive to the server side it gets sent over MQTT to the HUB. A brief description of MQTT, MQTT is a network protocol that handles data transmission over TCP/IP. The HUB handles the data transmission between the web application to the GyroCube. 

The data format sent to the hub is as follow:

```javascript
{
        side: # Face to be updated,
        func: # Feature to operate,
        color: # Color of operation,
        target: # Target value of the feature 
}
```
The format that is received from the HUB:
```javascript
{
        side: # The number the face operating,
        func: # Feature operataing,
        temp: # Current temperature value in the cube,
        humid: # Current humidity percentage in the cube
}
```

The process is broken down on the following figures:
<p align="center"><img src="https://i.imgur.com/BWRAK5G.png" width=500 height=400 alt="Submit data flow diagram"></p>
<p align="center">Figure 3. Submit data flow</p>

<p align="center"><img src="https://i.imgur.com/sVSoQUQ.png" width=500 height=400 alt="Receiving data flow diagram"></p>
<p align="center">Figure 4. Receiving data flow</p>

<p align="center"><img src="https://i.imgur.com/U4LVdYo.png" width=500 height=400 alt="Communication flow diagram"></p>
<p align="center">Figure 5. Communication flow</p>

