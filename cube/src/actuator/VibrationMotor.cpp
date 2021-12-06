#include "actuator/VibrationMotor.h"
#include "hardware/gpio.h"

// TODO (stretch): Make vibration interface pwm driven
VibrationMotor::VibrationMotor(uint8_t pin_) : pin(pin_)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

/**
 * @brief Issue pwm pulses with defined length
 * @param duration           Duration of vibration
 * @param vibrtion_intensity Intensity of vibration
 */
void VibrationMotor::vibrate(int duration)
{
    gpio_put(pin, true);
    sleep_ms(duration);
    gpio_put(pin, false);
}