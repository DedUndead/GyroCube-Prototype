// Add onchange listener to the main dashboard inputs
document.getElementById("color_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('color_side')
    let color = document.getElementById('color_target').value;
    console.log('[+] side: ' + side + ' color: ' + color)

    map_color(side, color)

});

document.getElementById("temp_tracker_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('temp_side')
    let temp = + document.getElementById('temp_target').value;
    console.log('[+] side: ' + side + ' temp: ' + temp)

    map_temperature(side, temp)

});

document.getElementById("hum_tracker_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('hum_side')
    let hum = + document.getElementById('hum_target').value;
    console.log('[+] side: ' + side + ' hum: ' + hum)

    map_humidity(side, hum)

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

    map_notify(side, mode, color)

});

document.getElementById("weather_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('weather_side')
    console.log('[+] side: ' + side)
    
    map_weather(side , 'Helsinki')

});

document.getElementById("idle_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('idle_side')
    console.log('[+] side: ' + side)

    map_idle(side)

});

// Start first mqtt timeout
// mqtt_timeout()