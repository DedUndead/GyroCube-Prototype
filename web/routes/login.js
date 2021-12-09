const express = require('express');
const mongoose = require('mongoose');
const passport = require('passport');

const router = express.Router();
const {ensureAuthenticated} = require('../config/auth');

// User model
const Users = require('../models/Users');

// Login page
router.get('/', async (req,res) => {
    res.render('homepage');
});

// Login handle
router.post('/', (req,res,next) => {
    passport.authenticate('local', {
        successRedirect: '/homepage',
        failureRedirect: '/',
        failureFlash: true
    })(req, res, next);
  });

router.get('/homepage', ensureAuthenticated, async (req,res) => {
    res.render('homepage');
});

router.get('/cube', async(req,res) => {
    res.render('cube');
})

router.get('/cube_1',(req,res) => {
    res.render('cube_1_settings');
})

// Logout handle
router.get('/logout', (req,res) => {
    req.logout();
    req.flash('success_msg', 'You are logged out');
    res.redirect('/');
});

router.get('*', (req,res) => {
    res.redirect('/');
});

module.exports = router;