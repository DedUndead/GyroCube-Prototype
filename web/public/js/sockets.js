// Initiate websockets
const socket = io("localhost:3000");

// I don't think there will be updates from hub
socket.on("update_from_cubus", (data) => {
    // Handle mqtt timeout
    if(mqtt_oneshot == true){
        console.log("[*] calling success popup")
        mqtt_oneshot = false
        success_popup()
    }

    let parsed = parse_cube_update(data)
    console.log('[+] DATA: ' + JSON.stringify(parsed))

    document.getElementById('cube_side').innerText = 'Current side: ' + parsed.side
    document.getElementById('current_func').innerText = 'Active function: ' + parsed.func
    document.getElementById('current_tmp').innerText = 'Temperature: ' + parsed.temp
    document.getElementById('current_humidity').innerText = 'Humidity: ' + parsed.humi
    document.getElementById('current_color').innerText = 'Color: ' + 'tba';

    // Refresh mqtt timeout
    mqtt_timeout()
});

socket.on("weather_update", (data) => {
    console.log("[+] Weather update: " + data)
});

/**
 * @function update_mqtt
 * @description Send a function update to the cube
 * @return no return
 **/
function update_mqtt(side_index, function_index, function_settings){
	
    let s = side_index;
    let fi = function_index;
    let fs = function_settings;

	let cube_state_update = {
		side: s,
        function: fi,
        settings: fs
	}
	socket.emit("update_cube_side", cube_state_update)
}

// Mapping functions and updates
function map_color(side, color) {
    let block = {
        side: side,
        func: 1,
        color: parseInt(color.substring(1), 16),
        target: 0
    }
    socket.emit("update_cube_side", block)
    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function map_weather(side, city) {
    let block = {
        side: side,
        func: 4,
        color: 0,
        target: 0
    }
    socket.emit("update_cube_side", block)
    socket.emit("update_weather", null)
    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function map_humidity(side, target_humidity) {
    let block = {
        side: side,
        func: 3,
        color: 0,
        target: target_humidity
    }
    socket.emit("update_cube_side", block)
    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function map_temperature(side, target_temperature) {
    let block = {
        side: side,
        func: 2,
        color: 0,
        target: target_temperature
    }
    socket.emit("update_cube_side", block)
    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function map_idle(side) {
    let block = {
        side: side,
        func: 0,
        color: 0,
        target: 0 
    }
    socket.emit("update_cube_side", block)
    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function map_notify(side, type, color) {
    let block

    if (type == 2 || type == 1) {
        block = {
            side: side,
            func: 5,
            color: parseInt(color.substring(1), 16),
            target: type
        }
    }
    else {
        block = {
            side: side,
            func: 5,
            color: 0,
            target: type
        }
    }
    socket.emit("update_cube_side", block)
    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function ping() {
    socket.emit("notif", null)
}

function update_weather() {
    socket.emit("update_weather", null)
}
