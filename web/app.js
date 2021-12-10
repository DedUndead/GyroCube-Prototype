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
const mqtt_client = mqtt.connect('mqtt://127.0.0.1:9999')

mqtt_client.on('connect', function () {
    mqtt_client.subscribe('/cubus_testing', function (err) {
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
    // io.emit("stats", input);
});

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

    return [weather_state, weather_state_abbr]
}

// WebSocket conf
io.on("connection", (socket) => {
    console.log("New connection: " + socket.id);

    // Handle updates from client
    socket.on('update_cube_side', function (data) {
        mqtt_client.publish("update_from_client", data)
        console.log("[+] Side update from client: " + JSON.stringify(data))
    });

    // Handle updates from client
    socket.on('update_weather', async function (data) {
        console.log("[+] Updating Cube and Client...")

        // Get data from API
        let weather_data = await get_current_weather_state('helsinki')
        let weather_full = weather_data[0]
        let weather_short = {
            weather: weather_data[1]
        }
        console.log('[+] weather: ' + weather_full + ' weather_short: ' + weather_short.weather)
        
        // Update client and Cube
        io.emit("weather_update", weather_full)
        mqtt_client.publish("weather_update", weather_short)
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