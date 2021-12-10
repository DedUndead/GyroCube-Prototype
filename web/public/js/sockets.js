// Initiate websockets
const socket = io("localhost:3000");

// I don't think there will be updates from hub
socket.on("update_from_hub", (data) => {
    
    // Handle mqtt timeout
    if(mqtt_oneshot == true){
        console.log("[*] calling success popup")
        mqtt_oneshot = false
        success_popup()
    }

    // Update dashboard if status is correct
    if(data.status == 1 || data.status == 0){
        // Save elements to variables

        // Update values
        // Update correct slider
    }

    // Show popups
    else if (data.status == -1 || data.status == -2) {
        error_popup(data.status)
    }

    // Refresh mqtt timeout
    mqtt_timeout()
});

/**
 * @function update_mqtt
 * @description Send new state of cube to the HUB
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

