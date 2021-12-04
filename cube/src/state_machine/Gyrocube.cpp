#include "state_machine/Event.h"
#include "sensor/Hih6020.h"
#include "actuator/NeoPixel.h"
#include "state_machine/Gyrocube.h"

Gyrocube::Gyrocube(Hih6020* sensor_, NeoPixel* leds_) :
    timer(0), sensor(sensor_), leds(leds_)
{
    set_state(&Gyrocube::startup);

    // TODO: Replace initial settings with file saved settings
    // Move to function
    settings[0] = { 0, "#000000", 0 };  // Default idle
    settings[1] = { 1, "#ff0000", 1 };  // Default lamp
    settings[2] = { 2, "#000000", 23 }; // Default temp
    settings[3] = { 3, "#ff0000", 25 }; // Default humid
    settings[4] = { 4, "#ffff00", 0 };  // Default weather
    settings[5] = { 5, "#ff0000", 2 };  // Default notification

    current_side = 0;
}

void Gyrocube::set_state(state_ptr new_state)
{
    current_state = new_state;
    (this->*current_state)(Event(Event::eEnter));
}

void Gyrocube::handle_state(const Event& e)
{
    (this->*current_state)(e);
}