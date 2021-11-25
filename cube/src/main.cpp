#include <stdio.h>
#include "pico/stdlib.h"
#include "interface/i2c.h"
#include "sensor/Hih6020.h"

#define BAUDRATE 250 * 1000
#define SDA_PIN 0 
#define SCL_PIN 1

int main() {
    stdio_init_all();
    
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
}