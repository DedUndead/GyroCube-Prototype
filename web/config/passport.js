const LocalStrategy = require('passport-local').Strategy;
const mongoose = require('mongoose');
const bcrypt = require('bcrypt');

// User model
const Users = require('../models/Users');

const validate_email = (email) => {
    let re = new RegExp("[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?");
    return re.test((String(email).toLowerCase()));
}

const compatible_email = (email) => {
    const arr = email.split('.');

    for (let i = 0; i < (arr.length - 1); i++) {
        arr[i] = arr[i].charAt(0).toUpperCase() + arr[i].slice(1);
    }

    const str2 = arr.join('.');

    return str2;
}

module.exports = async function(passport) {
    await passport.use(
        new LocalStrategy({usernameField: 'email'},async (email, password, done) => {
                try {
                    if (validate_email(email) == true) {
                        email = compatible_email(email);
                    }
                    console.log(email);
                    const user = await Users.findOne({ email: email });
                    console.log(user);
                    if (!user) {
                        return done(null, false, { message: 'That email does not exist in the database!' });
                    }
                    bcrypt.compare(password, user.password, (err, isMatch) => {
                        if (err) throw err;
            

                    if (isMatch) {
                        return done(null, user);
                    }
                    else {
                        return done(null, false, { message: 'Password incorrect' });
                    }
                })}
                catch (err) {
                    console.log(err);
                }
            }));
    
        passport.serializeUser((user,done) => {
            done(null, user.id);
        });
        passport.deserializeUser((id, done) => {
            Users.findById(id, (err,user) =>{
                done(err, user);
            });
        });
};

