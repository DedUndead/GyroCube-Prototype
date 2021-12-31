const express = require('express');

const router = express.Router();

// Landing page
router.get('/', async (req,res) => {
    res.render('homepage');
});

// Cube settings
router.get('/cube_settings',(req,res) => {
    res.render('cube_settings');
})

router.get('*', (req,res) => {
    res.redirect('/');
});

module.exports = router;