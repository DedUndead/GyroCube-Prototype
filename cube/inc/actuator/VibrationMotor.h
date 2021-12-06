#ifndef VIBRATIONMOTOR_H
#define VIBRATIONMOTOR_H


#include "pico/stdlib.h"

/**
 * @brief Vibration motor interface
 * Provides simple functions to issue pulses to the motor
 */
class VibrationMotor {
public:
    VibrationMotor(uint8_t pin_);
    void vibrate(int duration);
private:
    uint8_t pin;
};


#endif