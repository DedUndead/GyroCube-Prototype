const https = require('https')

// WEATHER API FUNCTIONALITY

// Curr date in api format
function get_current_date(){
    return new Date().toISOString().
    replace(/T.+/, '').
    replace(/[-]+/g,'/')
}

// API location response
async function api_get_location(city) {

    let url = 'https://www.metaweather.com/api/location/search/?query=' + city;
    let retval = 'nil'

    return new Promise((resolve) => {
        https.get(url, response => {
            let body = '';
    
            response.on('data', chunk => {
                if (Buffer.byteLength(chunk) > 2) {
                    body += chunk;
                }
            });
    
            response.on('end', () => {
                if( body != '') {
                    retval = body;
                }
                resolve(retval)
            });
        })
    })
}

// Get woeid from API lcoation response
async function get_woeid(city) {

    let response = await api_get_location(city);
    if (response == 'nil') {
        console.log("City not found")
        return 'nil'
    }
    else {
        let parsed = JSON.parse(response)
        return parsed[0].woeid
    }
}

// api weather date response
async function api_get_weather_data(woeid) {
    let url = 'https://www.metaweather.com/api/location/' + woeid + '/' + get_current_date() + '/';
    let retval = 'nil'

    return new Promise((resolve) => {
        https.get(url, response => {
            let body = '';
    
            response.on('data', chunk => {
                if (Buffer.byteLength(chunk) > 2) {
                    body += chunk;
                }
            });
    
            response.on('end', () => {
                if( body != '') {
                    retval = body;
                }
                resolve(retval)
            });
        })
    })
}

// Get weather state from api weather date response
async function parse_weather_state(woeid) {
    let response = await api_get_weather_data(woeid)
    if (response == 'nil') {
        console.log("Weather state unavailable")
        return 'nil'
    }
    else {
        let parsed = JSON.parse(response)
        return [parsed[0].weather_state_name, parsed[0].weather_state_abbr]
    }
}

// Use previous helper functions to get current weather state in the specified city
async function get_current_weather_state(city) {
    let woeid = await get_woeid(city)
    let [weather_state, weather_state_abbr] = await parse_weather_state(woeid)

    console.log(weather_state)
    console.log(weather_state_abbr)
    return weather_state_abbr

}


// LED LAMP COLOR
function rgb_to_hex(red, green, blue) {

    if (red > 255)      { red = 255 }
    if (green > 255)    { green = 255 }
    if (blue > 255)     { blue = 255 }

    return '#' + red.toString(16)
        + green.toString(16)
        + blue.toString(16)
}

// FUNCTIONS TO MAP ON CUBE
function map_color(side, r, g, b) {
    let block = {
        side: side,
        func: 0,
        color: rgb_to_hex(r, g, b),
        target: 0
    }

    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

async function map_weather(side, city) {
    let state = await get_current_weather_state(city)
    let block = {
        side: side,
        func: 1,
        color: '#000000',
        target: state
    }

    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function map_humidity(side, target_humidity) {
    let block = {
        side: side,
        func: 2,
        color: '#000000',
        target: target_humidity
    }

    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function map_temperature(side, target_temperature) {
    let block = {
        side: side,
        func: 3,
        color: '#000000',
        target: target_temperature
    }

    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function map_idle(side) {
    let block = {
        side: side,
        func: 5,
        color: '#000000',
        target: 0 
    }
    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

function notify(side, type, r = 0, g = 0, b = 0) {
    let block

    if (type == 2 || type == 1) {
        block = {
            side: side,
            func: 4,
            color: rgb_to_hex(r, g, b),
            target: type
        }
    }
    else {
        block = {
            side: side,
            func: 4,
            color: '#000000',
            target: type
        }
    }

    console.log('[MQTT] Publishing: ' + JSON.stringify(block))
}

async function main () {
    // await get_current_weather_state('Oslo')
    // let x = rgb_to_hex(255, 255, 255)
    map_color(4, 128, 255, 64)

    // console.log(x)
}

main()