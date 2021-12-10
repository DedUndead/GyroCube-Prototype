
// Global variables needed to control the popup timers
var popup_timer
var popup_duration = 3000

//Global variables needed to control the mqtt timeout timer
var mqtt_timer
var mqtt_duration = 10000
var mqtt_oneshot = true

// PROLLY NO ERROR HANDLING
/**
 * @function error_popup
 * @description Creates a red popup when displaying an error message or mqtt inactivity
 * @param {Integer} code specifies error message
 * @return no return
 **/
function error_popup(code){
    let popup_container
    let popup

    // Clear timer for popups
    clearTimeout(popup_timer)

    // Create elements if they don't exist
    if(document.getElementById("popup") == null) {
        popup_container = document.createElement("div")
        popup_container.id = "popup_container"

        popup = document.createElement("p")
        popup.id = "popup"

        document.getElementById("main_content").appendChild(popup_container)
        popup_container.appendChild(popup)
    }

    // If they do, save them to var
    else {
        popup = document.getElementById("popup")
        popup_container = document.getElementById("popup_container") 
    }

    // Set general class styling
    popup_container.className = "flex flex-row justify-center bg-red-100 absolute inset-x-0 top-0"
    popup.innerHTML = "No mqtt data for a while now :("
    
}

/**
 * @function success_popup
 * @description Creates a green popup when receiving mqtt data after a pause
 * @return no return
 **/
function success_popup(){
    let popup_container
    let popup

    // Clear timer for popups
    clearTimeout(popup_timer)

    // Create elements if they don't exist
    if(document.getElementById("popup") == null) {
        popup_container = document.createElement("div")
        popup_container.id = "popup_container"

        popup = document.createElement("p")
        popup.id = "popup"

        document.getElementById("main_content").appendChild(popup_container)
        popup_container.appendChild(popup)
    }

    // If they do, save them to var
    else {
        popup = document.getElementById("popup")
        popup_container = document.getElementById("popup_container")
    }

    // Set content and styling
    document.getElementById("popup").innerHTML = "Mqtt data received ! :)"
    document.getElementById("popup_container").className = "flex flex-row justify-center bg-green-100 absolute inset-x-0 top-0"

    // Start popup timer
    popup_timer = setTimeout(function () {
        document.getElementById("popup").remove()
        document.getElementById("popup_container").remove()
    }, popup_duration);
}

/**
 * @function mqtt_timeout
 * @description Starts a timer which creates a red popup notifying user of no mqtt data.
 * @return no return
 **/
function mqtt_timeout(){
    clearTimeout(mqtt_timer)

    mqtt_timer = setTimeout(function () {
        mqtt_oneshot = true
        error_popup(-3)
    }, mqtt_duration)
}


// Helper functions
function get_side_by_id(element_id) {
    let element = document.getElementById(element_id);
    let side = element.options[element.selectedIndex].value;
    return + side
}


// Add onchange listener to the main dashboard inputs
document.getElementById("color_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('color_side')
    let color = document.getElementById('color_target').value;
    console.log('[+] side: ' + side + ' color: ' + color)

    // Do something with it

});

document.getElementById("temp_tracker_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('temp_side')
    let temp = + document.getElementById('temp_target').value;
    console.log('[+] side: ' + side + ' temp: ' + temp)

    // Do something with it

});

document.getElementById("hum_tracker_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('hum_side')
    let hum = + document.getElementById('hum_target').value;
    console.log('[+] side: ' + side + ' hum: ' + hum)

    // Do something with it

});

document.getElementById("notif_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('notif_side')
    let mode = + document.querySelector('.form-radio:checked').value;
    let color = '#000000';
    if (mode != 0) {
        color = document.getElementById('notif_color_target').value;
    }
    console.log('[+] side: ' + side + ' mode: ' + mode + ' color: ' + color )

    // Do something with it

});

document.getElementById("weather_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('weather_side')
    console.log('[+] side: ' + side)
    // Do something with it

});

document.getElementById("idle_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('idle_side')
    console.log('[+] side: ' + side)

    // Do something with it

});

// Start first mqtt timeout
// mqtt_timeout()