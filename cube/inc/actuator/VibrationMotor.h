#ifndef VIBRATIONMOTOR_H
#define VIBRATIONMOTOR_H


#include "pico/stdlib.h"

/**
 * @brief Vibration motor interface
 * Provides simple functions to issue pwm pulses to the motor
 */
class VibrationMotor {
public:
    VibrationMotor(uint8_t pin);
    void vibrate(int duration, uint8_t vibration_intensity);
private:
    static void irq_pwm_handler();

    static uint slice_idx;
    static int counter;
    static uint8_t intensity;
    pwm_config config;
};


#endif