#include "state_machine/Event.h"
#include "sensor/Hih6020.h"
#include "actuator/NeoPixel.h"
#include "state_machine/Gyrocube.h"

Gyrocube::Gyrocube(Hih6020* sensor_, NeoPixel* leds_) :
    update_required(false), timer(0), sensor(sensor_), leds(leds_)
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

/**
 * @brief Change current state machine state
 * Call enter event after change
 * @param new_state Pointer to state
 */
void Gyrocube::set_state(state_ptr new_state)
{
    current_state = new_state;
    (this->*current_state)(Event(Event::eEnter));
}

/**
 * @brief Send event to current state
 * @param e Event object
 */
void Gyrocube::handle_state(const Event& e)
{
    (this->*current_state)(e);
}

/**
 * @brief Startup event
 * Waits for the hub's ACK
 */
void Gyrocube::startup(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            // Clear previous effects

            break;
        case Event::eTick:
            if (timer == 0) {
                // Gradient blue
                timer++;
            }
            else {
                // Leds off
                timer = 0;
            }

            break;
        case Event::eNotify:
            // Go to execution state

            break;
        default:
            break;
    }
}

/**
 * @brief Idle state
 * Does nothing but waits for crucial changes
 */
void Gyrocube::state_idle(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            // Clear previous effects

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            // Go to execution state

            break;
        default:
            break;
    }
}

void Gyrocube::state_lamp(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            // Clear previous effects

            break;
        case Event::eTick:
            if (update_required) {
                leds->fill(settings[current_side].color);
            }

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            // Go to execution state
            
            break;
        default:
            break;
    }
}

/**
 * @brief Temperature state
 * Display temperature with LEDs
 */
void Gyrocube::state_temp(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            // Clear previous effects

            break;
        case Event::eTick:
            if (sensor->read_temperature() != settings[current_side].target) {
                // Display temperature gradient
            }
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            // Go to execution state
            
            break;
        default:
            break;
    }
}

/**
 * @brief Humidity state
 * Display humidity with LEDs
 */
void Gyrocube::state_humid(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            // Clear previous effects

            break;
        case Event::eTick:
            if (sensor->read_humidity() != settings[current_side].target) {
                // Display temperature gradient
            }
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            // Go to execution state
            
            break;
        default:
            break;
    }
}

/**
 * @brief Weather state
 * Display weather with LEDs
 */
void Gyrocube::state_weather(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            // Clear previous effects

            break;
        case Event::eNotify:
            // Change weather based on notification value

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            // Go to execution state
            
            break;
        default:
            break;
    }
}

void Gyrocube::state_notification(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            // Clear previous effects

            break;
        case Event::eNotify:
            // Notify based on settings

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            // Go to execution state
            
            break;
        default:
            break; 
}