const express = require('express');
const mongoose = require('mongoose');
const flash = require('connect-flash')
const session = require('express-session');
const passport = require('passport');
const mongoStore = require('connect-mongo');
const mqtt = require('async-mqtt');
const https = require('https')
const { Server } = require("socket.io");
const { createServer } = require("http");
const app = express();

// Socket set.up
const httpServer = createServer(app);
const io = new Server(httpServer);

// Database keys
const db = require('./config/keys').MongoURI;

// Connection to the database's collections
const Users = require('./models/Users');

// Connects to the mongoDB
mongoose.connect(db, { useNewUrlParser: true })
    .then(() => console.log("MongoDB connected"))
    .catch(err => console.log(err));


// MQTT Config
const mqtt_client = mqtt.connect('mqtt://18.198.188.151:21883')

// Handle connectino to mqtt
mqtt_client.on('connect', function () {
    mqtt_client.subscribe('/gyro/hub', function (err) {
        if (err) {
            console.log("error encountered")
            mqtt_client.reconnect()
        }
        else {
            console.log("Well hello there 8)")
        }
    })
})

// Handle new mqtt message
mqtt_client.on('message', async function (topic, message) {
    let input = JSON.parse(message)
    console.log("Received: " + JSON.stringify(input))
    if  (input.side != -1) {
        io.emit("update_from_cubus", input);
    }
});

// Dictionary for mapping weather to codes
let weather_dict = {
    "sn":   0,
    "sl":   1,
    "h":    2,
    "t":    3,
    "hr":   4,
    "lr":   5,
    "s":    6,
    "hc":   7,
    "lc":   8,
    "c":    9,
}

/**
 * @function get_current_date
 * @description Get current date and modify it so it's in the correct format
 * @return Current date in the format yyyy/mm/dd
 **/
 function get_current_date(){
    return new Date().toISOString().
    replace(/T.+/, '').
    replace(/[-]+/g,'/')
}

/**
 * @function api_get_location
 * @description Convert city name to location data
 * @param city City for which the location data will be returned
 * @return location data
 **/
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

/**
 * @function get_woeid
 * @description Use location data from the specified city to get the woeid value
 * @param city City for which the woeid will be returned
 * @return woeid value
 **/
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

/**
 * @function api_get_weather_data
 * @description Get weather data from a specified location
 * @param woeid Woeid specifying the location
 * @return weather data
 **/
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

/**
 * @function parse_weather_state
 * @description Use weather data from the api_get_weather_data function and parse the weather state fields
 * @param woeid Woeid specifying the location
 * @return Current weather and the codename for the weather
 **/
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

/**
 * @function get_current_weather_state
 * @description Combine the other API functions into one which returns weather based on specified city
 * @param city City used to fetch weather data
 * @return Current state of weather and the abbreviation code of the weather
 **/
 async function get_current_weather_state(city) {
    let woeid = await get_woeid(city)
    let [weather_state, weather_state_abbr] = await parse_weather_state(woeid)

    return [weather_state, weather_state_abbr]
}

// WebSocket conf
io.on("connection", (socket) => {
    console.log("New connection: " + socket.id);

    // Handle cube updates from client
    socket.on('update_cube_side', function (data) {
        mqtt_client.publish("/gyro/web", JSON.stringify(data))
        console.log("[+] Side update from client: " + JSON.stringify(data))
    });

    // Handle weather updates from client
    socket.on('update_weather', async function (data) {
        console.log("[+] Updating Cube and Client...")

        // Get data from API
        let weather_data = await get_current_weather_state('helsinki')

        // Save data used in the web ui
        let weather_full = weather_data[0]

        // Map the weather abbreviation 
        let weather_short = {
            weather: weather_dict[weather_data[1]]
        }

        // Debug output
        console.log('[+] weather: ' + weather_full + ' weather_short: ' + weather_short.weather)
        console.log('[+] Weather updated from client ' + JSON.stringify(weather_short))
        
        // Update client and Cube
        io.emit("weather_update", weather_full)
        mqtt_client.publish("/gyro/web", JSON.stringify(weather_short))
    });

    socket.on('notif', async function (data) {
        console.log("[+] Notify cube...")
        let notification = {
            notif: 1
        }
        console.log('[+] Notif sent from ' + JSON.stringify(notification))
        mqtt_client.publish("/gyro/web", JSON.stringify(notification))
    });

});

// Pasport config
require('./config/passport')(passport);

// EJS
app.use(express.static('./public/'));
app.set('view engine', 'ejs');

// Session
app.use(session({ 
    secret: 'secret',
    resave: true,
    saveUninitialized: true,
    store: mongoStore.create({ mongoUrl: db })
}));

// Passport middleware
app.use(passport.initialize());
app.use(passport.session());

// Connect flash
app.use(flash());

// Global vars
app.use((req, res, next) => {
    res.locals.success_msg = req.flash('success_msg');
    res.locals.error_msg = req.flash('error_msg');
    res.locals.error = req.flash('error');
    next();
});

app.use('/', require('./routes/login'));

const PORT = 3000; 

httpServer.listen(PORT, () => {
    console.log(`server running on port ${PORT}`)
})