#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "actuator/ws2812.pio.h"
#include "actuator/NeoPixel.h"

/* @brief WS2812 Leds wrapper class
 * @param pio_hw  Pointer to hardware, pio0/pio1
 * @param pin     Target pio pin
 * @param length  Amount of leds in the circuit
 */
NeoPixel::NeoPixel(PIO pio_hw_, uint8_t pin_, uint8_t length_) : 
    pio_hw(pio_hw_),
    pin(pin_),
    length(length_)
{
    // Sets the insturctions on the pio hardware
    uint offset = pio_add_program(pio_hw, &ws2812_program);

    // Initialize the hardware
    ws2812_program_init(pio_hw, 0, offset, pin, 800000, false);

}

/* @brief Sets the color of the leds
 * Do nothing if format is incorrect
 * @param color_code Color code in html format
 */
void NeoPixel::fill(char* color_code)
{
    unsigned int red;
    unsigned int green;
    unsigned int blue;

    if (sscanf(color_code, "#%2x%2x%2x", &red, &green, &blue)) {
        NeoPixel::fill(red, green, blue);
    }
}

/* @brief Sets the color of the leds
 * @param red   Sets the intensity of the color red 
 * @param green Sets the intensity of the color green
 * @param blue  Sets the intensity of the color blue
 */
void NeoPixel::fill(uint8_t red, uint8_t green, uint8_t blue) 
{
    for(uint8_t i = 0; i < length; i++) {
        put_pixel(get_grb(red, green, blue));
    }
}

/* @brief Handles the led address shifting
 * @param pixel_grb Takes the color binary code
*/
void NeoPixel::put_pixel(uint32_t pixel_grb) 
{
    pio_sm_put_blocking(pio_hw, 0, pixel_grb << 8u);
}

/* @brief Handles the rgb to grb transformation
 * @param r Sets the intensity of the color red 
 * @param g Sets the intensity of the color green
 * @param b Sets the intensity of the color blue
 * @return  32 bit value that corresponds to grb color code
 */
uint32_t NeoPixel::get_grb(uint8_t red, uint8_t green, uint8_t blue) 
{
    return  ((uint32_t)(red) << 8)    | 
            ((uint32_t)(green) << 16) | 
            ((uint32_t)(blue));
}

/**
 * @brief Interpolate two RGB colors and fill the led sequence
 * Adopted from Stackoverflow by Carlson Barcellos, 2014 Jan 9
 * @param color1   Starting color
 * @param color2   Ending color
 * @param fraction Interpolation percent
 */
void NeoPixel::interpolate(uint32_t color1, uint32_t color2, float fraction)
{
    unsigned char r1 = (color1 >> 16) & 0xff;
    unsigned char r2 = (color2 >> 16) & 0xff;
    unsigned char g1 = (color1 >> 8) & 0xff;
    unsigned char g2 = (color2 >> 8) & 0xff;
    unsigned char b1 = color1 & 0xff;
    unsigned char b2 = color2 & 0xff;

    NeoPixel::fill(
        (uint8_t)((r2 - r1) * fraction + r1),
        (uint8_t)((g2 - g1) * fraction + g1),
        (uint8_t)((b2 - b1) * fraction + b1)
    );
}