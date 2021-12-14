#ifndef VIBRATIONMOTOR_H
#define VIBRATIONMOTOR_H


#include "pico/stdlib.h"

/**
 * @brief Vibration motor interface
 * Provides simple functions to issue pulses to the motor
 */
class VibrationMotor {
public:
    VibrationMotor(uint8_t motor_pin_);
    void vibrate(int duration);
private:
    static uint8_t motor_pin;

    static repeating_timer timer;
    static int64_t alarm_handler(alarm_id_t id, void *user_data);
};


#endif