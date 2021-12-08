/** 
 * This file provides basic examples of interface usage
 * written by Gyrocube team
 * 07 December 2021
 */
#if 0

#include <stdio.h>
#include "pico/stdlib.h"
#include "interface/i2c.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "sensor/Hih6020.h"
#include "sensor/Accelerometer.h"
#include "actuator/NeoPixel.h"
#include "actuator/VibrationMotor.h"

#define BAUDRATE 250 * 1000
#define SDA_PIN 0 
#define SCL_PIN 1

int main() {
    stdio_init_all();

#if 0 // NeoPixel interface example
    NeoPixel leds(pio0, 0, 30);
    
    while (true) {
        // "Appearing" effect
        for (int i = 10; i < 250; i += 10) {
            leds.fill(0, 0, i);
            sleep_ms(50);
        }
        sleep_ms(1000);
        leds.fill(0, 0, 0);
        sleep_ms(1000);

        // Gradient effect
        for (float i = 0; i <= 1; i += 0.1) {
            leds.interpolate(0x00ff00, 0xff0000, i);
            sleep_ms(50);
        }
        sleep_ms(1000);
        leds.fill(0, 0, 0);
        sleep_ms(1000);
    }
#endif

#if 0 // I2C and temp/humid interface examples
    I2C i2c(i2c0, BAUDRATE, SDA_PIN, SCL_PIN);
    Hih6020 temp_humid_sensor(&i2c);

    int temp = 0, humid = 0;

    sleep_ms(10000);

    printf("Starting the program..\n");
    while (true) {
        temp = temp_humid_sensor.read_temperature();
        printf("Temperature: %d\n", temp);
        humid = temp_humid_sensor.read_humidity();
        printf("Humid: %d\n", humid);

        sleep_ms(2000);
    }   
#endif

#if 0 // Vibration motor interface usage example
    VibrationMotor motor(0);
    while (true) {
        motor.vibrate(500);
        sleep_ms(3000);
    }
#endif

#if 0 // Accelerometer interface usage example
    I2C i2c(i2c0, BAUDRATE, SDA_PIN, SCL_PIN);
    Accelerometer accelerometer(&i2c);
    acc_measurements acc_buffer;

    sleep_ms(3000);

    printf("Enabling accelerometer.\n");
    accelerometer.enable_in_fast_mode();

    while (true) {
        // Read into acc buffer
        accelerometer.read(acc_buffer);
        printf("X: %d\nY: %d\nZ: %d\n\n", acc_buffer.x, acc_buffer.y, acc_buffer.z);

        sleep_ms(50);
    }
#endif
}
#endif

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
#define TICK_LENGTH      500
#define SAMPLING_RATE    5

static volatile bool tick(false);
static volatile bool sample_side(false);
static volatile uint8_t tick_counter(0);

/* Function definitions */

/* Interrupt handlers */
bool irq_timer(repeating_timer *rt)
{   
    // Set state machine tick flag ready
    tick = true;

    // Set sample acceleration flad ready
    if (tick_counter < SAMPLING_RATE) {
        tick_counter++;
    }
    else {
        sample_side = true;
        tick_counter = 0;
    }

    return true;
}

int main() {
    stdio_init_all();

    /* Initialize periherals and interfaces */
    I2C i2c(i2c0, BAUDRATE, SDA_PIN, SCL_PIN);
    Hih6020 hih6020(&i2c);
    Accelerometer accelerometer(&i2c);
    NeoPixel leds(pio0, NEOPIXEL_PIN, NEOPIXEL_LEN);
    VibrationMotor motor(MOTOR_PIN);

    sleep_ms(100); // Current settling time

    /* Enable accelerometer */
    acc_measurements acceleration;
    accelerometer.enable_in_fast_mode();

    /* Initialize cube's state machine */
    Gyrocube gyrocube(&hih6020, &leds, &motor, 0, true);

    /* Set up timer interrupts */
    repeating_timer timer;
    add_repeating_timer_ms(TICK_LENGTH, irq_timer, NULL, &timer);

    while (true) {
        if (sample_side) {
            sample_side = false;
            accelerometer.read(acceleration);
            printf("X:%d\n Y:%d\n Z:%d\n\n");
            //analyze_measurement();
        }

        if (tick) {
            tick = false;
            gyrocube.handle_state(Event::eTick);
        }
    }

    return 0;
}