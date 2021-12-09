/*  
* CREATES THE DATA STRUCTURE REQUIREMENTS FOR USER DATA
* CREATES COLLECTION UNDER THE NAME 'users'
* CREATES COLLECTION MODEL ACCORDING TO THE DATA SCHEMA
* USED FOR USER AUTHENTICATION
*/

const mongoose = require('mongoose');

const UserSchema = new mongoose.Schema({
    member: {
        type: String,
        require: true
    },
    email: {
        type: String,
        require: true
    },
    password: {
        type: String,
        require: true
    }
});

UserSchema.set('collection', 'Users');

const Users = mongoose.model('Users', UserSchema); 

module.exports = Users;