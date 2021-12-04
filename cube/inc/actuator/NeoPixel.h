#ifndef NEOPIXEL_H
#define NEOPIXEL_H


#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "actuator/ws2812.pio.h"

class NeoPixel {
public:
    NeoPixel(PIO pio_hw_, uint8_t pin_, uint8_t length_);
    void fill(char* color_code);
    void fill(uint8_t red, uint8_t green, uint8_t blue);
    void gradient();
private:
    void put_pixel(uint32_t pixel_grb);
    uint32_t get_grb(uint8_t red, uint8_t green, uint8_t blue);
    PIO pio_hw;
    uint8_t pin;
    uint8_t length;
};


#endif