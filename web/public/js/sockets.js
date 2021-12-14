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

    // {
    //     side: current_side
    //     temp: temperature
    //     humid: humid
    // }
    // UPDATE UI


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

// FUNCTIONS TO MAP ON CUBE
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
    socket.emit("notif", block)
    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}



// Leaving updates for db alone for now
/**
 * @function update_userdb
 * @description Emits user action data to be updated in the db to the server
 * @param {String} type_ String specifying action type
 * @param {String} desc_ String specifying action description
 * @param {Integer} val_ New value of the updated element
 * @return no return
 **/
// function update_userdb(type_, desc_, val_){
// 	let user_data = {
// 		type: type_,
// 		description: desc_,
// 		value: val_
// 	};
// 	socket.emit("update_userdb", user_data);
// }

