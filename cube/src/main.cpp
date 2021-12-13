#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "interface/i2c.h"
#include "interface/XBee.h"
#include "sensor/Hih6020.h"
#include "sensor/Accelerometer.h"
#include "actuator/NeoPixel.h"
#include "actuator/VibrationMotor.h"
#include "state_machine/Gyrocube.h"

/* Macros and global variables */
#define BAUDRATE          250 * 1000
#define SDA_PIN           16
#define SCL_PIN           17
#define NEOPIXEL_PIN      5
#define MOTOR_PIN         15
#define NEOPIXEL_LEN      4
#define ZIGBEE_TX_PIN     8
#define ZIGBEE_RX_PIN     9
#define ZIGBEE_CST_PIN    10
#define ZIGBEE_WAKEUP_PIN 11
#define ZIGBEE_STATUS_PIN 12

#define TICK_LENGTH       500
#define SAMPLING_RATE     2
#define ACC_BUFFER_LENGTH 2
#define T_CURRENT_SETTLE  100
#define ZIGBEE_BUFFER_LEN 100

static volatile bool tick(false);          // Flag determining state machine tick
static volatile bool sample_side(false);   // Flag determining if side should be sampled
static volatile bool zigbee_send(false);   // Flag determining if machine's info should be fowarded
static volatile uint8_t sampling_index(0); // Sampling index for accelerometer filtering
static volatile uint8_t tick_counter(0);   // Tick counter for sampling scheduling

/* Function definitions */
void record_accelerometer_sample(Accelerometer& accelerometer, uint8_t* buffer);
void handle_accelerometer_sample(Gyrocube& gyrocube, uint8_t* buffer, uint8_t& current_side);
void update_settings(Gyrocube& gyrocube, char* zigbee_buffer);
void update_weather(Gyrocube& gyrocube, char* zigbee_buffer);
void forward_measurements(Gyrocube& gyrocube, uint8_t side, XBee& zigbee);

/* Interrupt handlers */
bool irq_timer(repeating_timer *rt)
{   
    // Set state machine tick flag ready
    tick = true;

    // Set other flag ready
    tick_counter++;
    if (tick_counter == SAMPLING_RATE) {
        sample_side = true;
        zigbee_send = true;

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

    XBee zigbee(
        uart1, ZIGBEE_RX_PIN, ZIGBEE_TX_PIN, ZIGBEE_CST_PIN, 
        ZIGBEE_WAKEUP_PIN, ZIGBEE_STATUS_PIN
    );
    char zigbee_buffer[ZIGBEE_BUFFER_LEN];

    sleep_ms(T_CURRENT_SETTLE); // Current settling time

    /* Enable accelerometer */
    uint8_t current_side = accelerometer.get_side();
    uint8_t acc_sample_buffer[ACC_BUFFER_LENGTH] = { current_side, current_side };
    accelerometer.enable_in_fast_mode();

    /* Initialize cube's state machine */
    Gyrocube gyrocube(&hih6020, &leds, &motor, current_side, true);
    uint8_t new_weather_index;

    /* Set up timer interrupts */
    repeating_timer timer;
    add_repeating_timer_ms(TICK_LENGTH, irq_timer, NULL, &timer);
  
    while (true) {
        // Accelerometer periodic sampling
        if (sample_side) {
            record_accelerometer_sample(accelerometer, acc_sample_buffer);
            handle_accelerometer_sample(gyrocube, acc_sample_buffer, current_side);
            
            sample_side = false;
        }

        // State machine tick
        if (tick) {
            gyrocube.handle_state(Event(Event::eTick));
            
            tick = false;
        }

        // Machine status update -> to hub
        if (zigbee_send) {
            //forward_measurements(gyrocube, current_side, zigbee);

            zigbee_send = false;
        }

        // New data from hub
        if (zigbee.get_data((uint8_t *)zigbee_buffer, ZIGBEE_BUFFER_LEN) > 0) {
            zigbee.send_data(zigbee_buffer);
            
            // Notification arrived
            if (zigbee_buffer[0] == 'n') {
                gyrocube.handle_state(Event(Event::eNotify));
            }
            // Weather update arived
            else if (zigbee_buffer[0] == 'w') {
                update_weather(gyrocube, zigbee_buffer);
            }
            // Settings update arrived
            else if (zigbee_buffer[0] == 's') {
                update_settings(gyrocube, zigbee_buffer);
            }

            strcpy(zigbee_buffer, ""); // Clear buffer
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
 * @param gyrocube     Target cube's state machine
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

/**
 * @brief Update settings of the side from zigbee buffer
 * @param gyrocube      Target cube's state machine
 * @param zigbee_buffer Zigbee buffer
 */
void update_settings(Gyrocube& gyrocube, char* zigbee_buffer)
{
    side_settings new_settings;
    uint8_t side_to_update;

    sscanf(
        zigbee_buffer, "s%d %d %d %d", 
        &side_to_update,
        &new_settings.function, 
        &new_settings.color, 
        &new_settings.target
    );

    gyrocube.update_settings(side_to_update, new_settings);
}

/**
 * @brief Update weather in the cube's settings from zigbee buffer
 * @param gyrocube      Target cube's state machine
 * @param zigbee_buffer Zigbee buffer
 */
void update_weather(Gyrocube& gyrocube, char* zigbee_buffer)
{
    uint8_t new_weather_index;

    sscanf(zigbee_buffer, "w%d", &new_weather_index);

    gyrocube.update_weather(new_weather_index);
}

/**
 * @brief Forward machine state to hub using zigbee interface
 * @param gyrocube      Target cube's state machine
 * @param side          Current cube's side
 * @param zigbee        Zigbee interface
 */
void forward_measurements(Gyrocube& gyrocube, uint8_t side, XBee& zigbee)
{
    char message[ZIGBEE_BUFFER_LEN];

    machine_state measurements = { 0, 0 };
    measurements = gyrocube.get_info();

    sprintf(
        message, "%d %d %d", 
        side, 
        measurements.humidity, 
        measurements.temperature
    );

    zigbee.send_data(message);
}