#include <string.h>
#include "state_machine/Event.h"
#include "sensor/Hih6020.h"
#include "actuator/NeoPixel.h"
#include "state_machine/Gyrocube.h"

Gyrocube::Gyrocube(Hih6020* sensor_, NeoPixel* leds_) :
    update_required(false), timer(0), sensor(sensor_), leds(leds_),
    functional_states { 
        &Gyrocube::state_idle,    &Gyrocube::state_lamp,
        &Gyrocube::state_temp,    &Gyrocube::state_humid,
        &Gyrocube::state_weather, &Gyrocube::state_notification
    }
{
    set_state(&Gyrocube::startup);

    // TODO: Replace initial settings with file saved settings
    // Move to function
    for (uint8_t i = 0; i < N_SIDES; i++) {
        settings[i].function = i;
        strcpy(settings[i].color, "#ff0000");
        settings[i].target = 25;
    }

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

void Gyrocube::update_settings(uint8_t side, side_settings new_setting)
{
    settings[side] = new_setting;
    update_required = true;
}

/**
 * @brief Startup event
 * Waits for the hub's ACK
 */
void Gyrocube::startup(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            break;
        case Event::eTick:
            if (timer == 0) {
                for (int i = 10; i < 250; i += 10) {
                    leds->fill(0, 0, i);
                    sleep_ms(50);
                }
                timer++;
            }
            else {
                leds->fill(0, 0, 0);
                timer = 0;
            }

            break;
        case Event::eNotify:
            set_state(e.value);

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
            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            set_state(e.value);

            break;
        default:
            break;
    }
}

void Gyrocube::state_lamp(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            leds->fill(settings[current_side].color);

            break;
        case Event::eTick:
            // Change color if update in settings is present
            if (update_required) {
                leds->fill(settings[current_side].color);
                update_required = false;
            }

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            set_state(e.value);

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
            break;
        case Event::eTick:
            // Display temperature gradient
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            set_state(e.value);
            
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
            break;
        case Event::eTick:
            // Display humidity gradient
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            set_state(e.value);
            
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
            leds->fill("#ffff00");

            break;
        case Event::eTick:
            if (update_required) {
                
            }

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            set_state(e.value);
            
            break;
        default:
            break;
    }
}

/**
 * @brief Notification state
 * Displays notification with motor and leds
 */
void Gyrocube::state_notification(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            break;
        case Event::eNotify:
            // Notify based on settings

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            set_state(e.value);
            
            break;
        default:
            break; 
    }
}

/**
 * @brief Overload of set_state function
 * Changes the state of state machine based on the new cube's side
 * Updates current side of the cube
 * @param side_index Index of the side the cube was placed on
 */
void Gyrocube::set_state(uint8_t side_index)
{
    current_side = side_index;
    set_state(functional_states[current_side]);
}

/**
 * @brief Clear actuators from all current effects
 */
void Gyrocube::clear()
{
    leds->fill("#000000");
}