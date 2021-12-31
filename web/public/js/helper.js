// Global variables needed to control the popup timers
var popup_timer
var popup_duration = 3000

// Global variables needed to control the mqtt timeout timer
var mqtt_timer
var mqtt_duration = 10000
var mqtt_oneshot = true

// Global weather update timer variables
var weather_timer
var weather_interval = 10 * 60000   // 10 min interval

// Default weather location
var weather_location = 'helsinki'

// Default mapping of functions to sides
var function_map = {
    0: 'Idle',
    1: 'Simple color',
    2: 'Temp tracker',
    3: 'Humidity tracker',
    4: 'Weather',
    5: 'Notifications'    
}

/**
 * @function parse_cube_update
 * @description Create an object which is used to update the web UI
 * @param data data received from the cube
 * @return no return
 **/
function parse_cube_update(data) {
    let parsed = {
        side: data.side + 1,
        func: function_map[data.side],
        temp: data.temp,
        humi: data.humid,
        colr: 'tba',
    }
    return parsed
}

/**
 * @function error_popup
 * @description Creates a red popup when displaying an error message or mqtt inactivity
 * @param code 0 -> mqtt inactivity, 1 -> weather error
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

    if (code == 0){
        // Set mqtt error styling
        popup_container.className = "flex flex-row justify-center bg-red-100 absolute inset-x-0 top-0"
        popup.innerHTML = "No mqtt data for a while now :("
    }
    else if (code == 1){
        // Set mqtt error styling
        popup_container.className = "flex flex-row justify-center bg-red-100 absolute inset-x-0 top-0"
        popup.innerHTML = "Location not found :("
        
        // Start popup timer
        popup_timer = setTimeout(function () {
            document.getElementById("popup").remove()
            document.getElementById("popup_container").remove()
        }, popup_duration) 
    }
}

/**
 * @function success_popup
 * @description Creates a green popup when receiving mqtt data after a pause and updates the data like the weather or function mappings
 * @return no return
 **/
function success_popup(){

    // Update weather
    update_weather()
    clearInterval(weather_timer)
    weather_timer = setInterval(function () {
        update_weather
    }, weather_interval) 

    // Update function mappings to default
    function_map = {
        0: 'Idle',
        1: 'Simple color',
        2: 'Temp tracker',
        3: 'Humidity tracker',
        4: 'Weather',
        5: 'Notifications'    
    }

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
    }, popup_duration) 
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
        error_popup(0)
    }, mqtt_duration)
}

/**
 * @function get_side_by_id
 * @description Get selected side from the settings interface
 * @param element_id id of the element from which we want to extract the data
 * @return side value
 **/
 function get_side_by_id(element_id) {
    let element = document.getElementById(element_id) 
    let side = + element.options[element.selectedIndex].value 
    return side - 1
}

const colorLerp = (color1, color2, percentage) => {
    const [rStart, gStart, bStart] = [color1 & 0xff0000, color1 & 0xff00, color1 & 0xff] 
    const [rDiff, gDiff, bDiff] = [
      (color2 & 0xff0000) - rStart,
      (color2 & 0xff00) - gStart,
      (color2 & 0xff) - bStart] 
  
    return '#' + (Math.floor(rStart + rDiff * percentage) & 0xff0000
      | Math.floor(gStart + gDiff * percentage) & 0xff00
      | Math.floor(bStart + bDiff * percentage) & 0xff).toString(16).padStart(6, '0') 
  } 