#include "actuator/VibrationMotor.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

VibrationMotor::VibrationMotor(uint8_t pin)
{
    // Configure gpio and find required pwm internal timer index
    gpio_set_function(pin, GPIO_FUNC_PWM);
    slice_idx = pwm_gpio_to_slice_num(pin);

    // Configure interrupt
    pwm_clear_irq(slice_idx);
    pwm_set_irq_enabled(slice_idx, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, irq_pwm_handler);

    // Set pwm to approximately 1 ms period
    config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.9f);
    pwm_init(slice_idx, &config, false);
}

/**
 * @brief Issue pwm pulses with defined length
 * @param duration           Duration of vibration
 * @param vibrtion_intensity Intensity of vibration
 */
void VibrationMotor::vibrate(int duration, uint8_t vibration_intensity)
{
    counter = duration;
    intensity = vibration_intensity * 100
}

void VibrationMotor::irq_pwm_handler()
{
    pwm_clear_irq(slice_idx);

    if (counter > 0) {
        
        counter--;
    }
    else {

    }
    
    pwm_set_gpio_level(255)
}