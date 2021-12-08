#include "actuator/VibrationMotor.h"
#include "hardware/gpio.h"

// Static member to handle alarm interrupt encapsulated in the class
uint8_t VibrationMotor::motor_pin;

VibrationMotor::VibrationMotor(uint8_t motor_pin_)
{
    motor_pin = motor_pin_;

    gpio_init(motor_pin);
    gpio_set_dir(motor_pin, GPIO_OUT);
}

/**
 * @brief Issue pwm pulses with defined length
 * @param duration           Duration of vibration in ms
 * @param vibrtion_intensity Intensity of vibration
 */
void VibrationMotor::vibrate(int duration)
{
    gpio_put(motor_pin, true);
    // Handle waiting in the background
    add_alarm_in_ms(duration, alarm_handler, NULL, false);
}

int64_t VibrationMotor::alarm_handler(alarm_id_t id, void *user_data)
{
    gpio_put(motor_pin, false);
    return 0;
}