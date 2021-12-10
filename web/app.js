const express = require('express');
const mongoose = require('mongoose');
const flash = require('connect-flash')
const session = require('express-session');
const passport = require('passport');
const mongoStore = require('connect-mongo');
const https = require('https');
const mqtt = require('async-mqtt');
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

// WebSocket conf
io.on("connection", (socket) => {
    console.log("New connection: " + socket.id);

    // Handle updates from client
    socket.on('update_cube_side', function (data) {
        // Handle updates from cube
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