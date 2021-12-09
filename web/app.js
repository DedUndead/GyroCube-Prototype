const express = require('express');
const mongoose = require('mongoose');
const flash = require('connect-flash')
const session = require('express-session');
const passport = require('passport');
const mongoStore = require('connect-mongo');
const app = express();

// Database keys
const db = require('./config/keys').MongoURI;

// Connection to the database's collections
const Users = require('./models/Users');

// Connects to the mongoDB
mongoose.connect(db, { useNewUrlParser: true })
    .then(() => console.log("MongoDB connected"))
    .catch(err => console.log(err));


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

app.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
})