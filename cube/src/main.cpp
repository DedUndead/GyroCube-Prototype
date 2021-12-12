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
#include "interface/XBee.h"
/* Macros and global variables */
#define BAUDRATE          250 * 1000
#define ACC_BUFFER_LENGTH 2
#define SDA_PIN           16
#define SCL_PIN           17
#define NEOPIXEL_PIN      5
#define MOTOR_PIN         15
#define NEOPIXEL_LEN      4
#define TICK_LENGTH       500
#define SAMPLING_RATE     2

static volatile bool tick(false);          // Flag determining state machine tick
static volatile bool sample_side(false);   // Flag determining if side should be sampled
static volatile uint8_t sampling_index(0); // Sampling index for accelerometer filtering
static volatile uint8_t tick_counter(0);   // Tick counter for sampling scheduling

/* Function definitions */
void record_accelerometer_sample(Accelerometer& accelerometer, uint8_t* buffer);
void handle_accelerometer_sample(Gyrocube& gyrocube, uint8_t* buffer, uint8_t& current_side);

/* Interrupt handlers */
bool irq_timer(repeating_timer *rt)
{   
    // Set state machine tick flag ready
    tick = true;

    // Set sample acceleration flag ready
    tick_counter++;
    if (tick_counter == SAMPLING_RATE) {
        sample_side = true;
        sampling_index = !sampling_index; 
        tick_counter = 0;
    }

    return true;
}

/* Main program body */
int main()
{
    /* Initialize hardware */
    stdio_init_all();

    /* Initialize periherals and interfaces */
    I2C i2c(i2c0, BAUDRATE, SDA_PIN, SCL_PIN);
    Hih6020 hih6020(&i2c);
    Accelerometer accelerometer(&i2c);
    NeoPixel leds(pio0, NEOPIXEL_PIN, NEOPIXEL_LEN);
    VibrationMotor motor(MOTOR_PIN);

    sleep_ms(100); // Current settling time

    /* Enable accelerometer */
    uint8_t current_side = accelerometer.get_side();
    uint8_t acc_sample_buffer[ACC_BUFFER_LENGTH] = { current_side, current_side };
    accelerometer.enable_in_fast_mode();

    /* Set up timer interrupts */
    repeating_timer timer;
    add_repeating_timer_ms(TICK_LENGTH, irq_timer, NULL, &timer);

    /* Initialize cube's state machine */
    Gyrocube gyrocube(&hih6020, &leds, &motor, 0, true);

    while (true) {
        if (sample_side) {
            record_accelerometer_sample(accelerometer, acc_sample_buffer);
            handle_accelerometer_sample(gyrocube, acc_sample_buffer, current_side);
            
            sample_side = false;
        }

        if (tick) {
            gyrocube.handle_state(Event(Event::eTick));
            
            tick = false;
        }
    }
}

/**
 * @brief Record sample if valid
 * @param accelerometer Pointer to accelerometer device
 * @param buffer        Filtering buffer pointer
 */
void record_accelerometer_sample(Accelerometer& accelerometer, uint8_t* buffer)
{
    uint8_t sample = accelerometer.get_side();
    if (sample != ORIENT_UNKNOWN) {
        buffer[sampling_index] = sample;
    }
}

/**
 * @brief Handle accelerometer incoming sample
 * If change in side is verified, send an event to state machine
 * @param buffer       Buffer containing samples
 * @param current_side Current side of the cube
 * @return uint8_t 
 */
void handle_accelerometer_sample(Gyrocube& gyrocube, uint8_t* buffer, uint8_t& current_side)
{
    if (buffer[0] == buffer[1] && buffer[1] != current_side) {
        current_side = buffer[0];
        printf("Current side %d\n", current_side);
        gyrocube.handle_state(Event(Event::eChange, current_side));
    }
}