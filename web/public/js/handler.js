// Add onchange listener to the main dashboard inputs
document.getElementById("color_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('color_side')
    let color = document.getElementById('color_target').value;
    console.log('[+] side: ' + side + ' color: ' + color)
    
    function_map[side] = 'lamp'
    map_color(side, color)

});

document.getElementById("temp_tracker_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('temp_side')
    let temp = + document.getElementById('temp_target').value;
    console.log('[+] side: ' + side + ' temp: ' + temp)
    
    function_map[side] = 'thermometer'
    map_temperature(side, temp)

});

document.getElementById("hum_tracker_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('hum_side')
    let hum = + document.getElementById('hum_target').value;
    console.log('[+] side: ' + side + ' hum: ' + hum)

    function_map[side] = 'humidboi'
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

    function_map[side] = 'notifier'
    map_notify(side, mode, color)

});

document.getElementById("weather_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('weather_side')
    console.log('[+] side: ' + side)
    
    function_map[side] = 'forecast'
    map_weather(side , 'Helsinki')

});

document.getElementById("idle_submit").addEventListener("click", (event) => {
    // Get form data
    let side = get_side_by_id('idle_side')
    console.log('[+] idle side: ' + side)

    function_map[side] = 'idler'
    map_idle(side)

});

let pinger = document.getElementById("pingpong")
pinger.style="cursor: pointer;"
pinger.addEventListener("click", (event) => {
    console.log('[+] PING')
    ping()
});

// Start first mqtt timeout
mqtt_timeout()