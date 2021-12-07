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

#if 0 // I2C and sensor interface examples
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

#if 1 // Accelerometer interface usage example
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