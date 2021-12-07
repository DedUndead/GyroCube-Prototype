#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "interface/i2c.h"
#include "sensor/Hih6020.h"
#include "sensor/Accelerometer.h"
#include "actuator/NeoPixel.h"
#include "actuator/VibrationMotor.h"
#include "state_machine/Gyrocube.h"

/* Macros and global variables */
#define BAUDRATE         250 * 1000
#define SDA_PIN          0 
#define SCL_PIN          1
#define NEOPIXEL_PIN     2
#define MOTOR_PIN        3
#define NEOPIXEL_LEN     30
#define ACC_IRQ_PIN      16

static volatile bool cube_flipped(false);

/* Function definitions */

/* Interrupt handlers */
void gpio_irq_handler(uint gpio, uint32_t events)
{
    cube_flipped = true;
}

int main() {
    stdio_init_all();

    /* Initialize periherals and interfaces */
    I2C i2c(i2c0, BAUDRATE, SDA_PIN, SCL_PIN);
    Hih6020 hih6020(&i2c);
    Accelerometer accelerometer(&i2c);
    NeoPixel leds(pio0, NEOPIXEL_PIN, NEOPIXEL_LEN);
    VibrationMotor motor(MOTOR_PIN);

    /* Enable pin interrupts for accelerometer */
    gpio_set_irq_enabled_with_callback(
        ACC_IRQ_PIN,
        GPIO_IRQ_EDGE_RISE,
        true,
        gpio_irq_handler
    );

    sleep_ms(5000); // Debugging purpose !

    /* Configure accelerometer */
    if (accelerometer.enable_orientation_interrupt() != 0) {
        printf("Error on orientation configuration.\n");
    }
    if (accelerometer.enable_in_fast_mode() != 0) {
        printf("Error on accelerometer enabling.\n");
    }

    /* Initialize cube's state machine */
    Gyrocube gyrocube(&hih6020, &leds, &motor, 0, true);

    while (true) {
        if (cube_flipped) {
            int orientation = accelerometer.get_orientation();
            printf("New orientation value is %d\n", orientation);
        }
    }

    return 0;
}