#include <stdio.h>
#include "pico/stdlib.h"
#include "interface/i2c.h"
#include "hardware/pio.h"
#include "sensor/Hih6020.h"
#include "actuator/NeoPixel.h"

#define BAUDRATE 250 * 1000
#define SDA_PIN 0 
#define SCL_PIN 1

int main() {
    stdio_init_all();
    
#if 1 // NeoPixel interface example
    NeoPixel leds(pio0, 0, 30);
    leds.fill(0, 255, 0);
    
    // Idle loop
    while (true) sleep_ms(1000);
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
}