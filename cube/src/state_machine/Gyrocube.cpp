#include <string.h>
#include <stdio.h>
#include "state_machine/Event.h"
#include "sensor/Hih6020.h"
#include "actuator/VibrationMotor.h"
#include "actuator/NeoPixel.h"
#include "state_machine/Gyrocube.h"

/**
 * @brief Construct a new Gyrocube:: Gyrocube object
 * @param sensor_    Pointer to humid/temp sensor interface
 * @param leds_      Pointer to leds actuator
 * @param side       Current cube's side
 * @param standalone State machine's mode.
 * Standalone mode allows user to use the cube seperately from zigbee topology
 */
Gyrocube::Gyrocube(
    Hih6020* sensor_, 
    NeoPixel* leds_,
    VibrationMotor* motor_, 
    uint8_t side, 
    bool standalone
) :
    update_required(false),
    timer(0),
    sensor(sensor_),
    leds(leds_),
    motor(motor_),
    functional_states { 
        &Gyrocube::state_idle,    &Gyrocube::state_lamp,
        &Gyrocube::state_temp,    &Gyrocube::state_humid,
        &Gyrocube::state_weather, &Gyrocube::state_notification
    }
{
    // TODO: Replace initial settings with file saved settings
    // Move to function
    for (uint8_t i = 0; i < N_SIDES; i++) {
        settings[i].function = i;
        strcpy(settings[i].color, "#ff0000");
        settings[i].target = 25;
    }
    
    // Set initial state according to state machine's mode
    current_side = side;
    if (!standalone) set_state(&Gyrocube::startup);
    else             set_state(settings[current_side].function);
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
 * @brief Overload of set_state function
 * Changes the state of state machine based on the state's index
 * @param state_index Index of the state
 */
void Gyrocube::set_state(uint8_t state_index)
{
    set_state(functional_states[state_index]);
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
 * @brief Update settings of the side
 * Following flags will be send to notify current state:
 * update_requited, function_changed
 * @param side        Target side
 * @param new_setting New settings
 */
void Gyrocube::update_settings(uint8_t side, side_settings new_setting)
{
    // See if current side function was altered
    if (side == current_side && new_setting.function != settings[side].function) {
        function_changed = true;
    }

    // Update settings
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
            // Fading effect on each tick while connecting
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
            current_side = e.value;
            set_state(settings[current_side].function);

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
            clear();

            break;
        case Event::eTick:
            if (function_changed) {
                set_state(settings[current_side].function);
            }

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            current_side = e.value;
            set_state(settings[current_side].function);

            break;
        default:
            break;
    }
}

void Gyrocube::state_lamp(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            clear();
            leds->fill(settings[current_side].color);

            break;
        case Event::eTick:
            if (function_changed) {
                set_state(settings[current_side].function);
            }

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
            current_side = e.value;
            set_state(settings[current_side].function);

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
            clear();
            
            break;
        case Event::eTick:
            if (function_changed) {
                set_state(settings[current_side].function);
            }

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            current_side = e.value;
            set_state(settings[current_side].function);
            
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
            clear();

            break;
        case Event::eTick:
            if (function_changed) {
                set_state(settings[current_side].function);
            }

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            current_side = e.value;
            set_state(settings[current_side].function);
            
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
            clear();

            break;
        case Event::eTick:
            if (function_changed) {
                set_state(settings[current_side].function);
            }

            if (update_required) {
                
            }

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            current_side = e.value;
            set_state(settings[current_side].function);
            
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
            clear();

            break;
        case Event::eTick:
            if (function_changed) {
                set_state(settings[current_side].function);
            }
        case Event::eNotify:
            notify();

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            current_side = e.value;
            set_state(settings[current_side].function);
            
            break;
        default:
            break; 
    }
}

/**
 * @brief Clear actuators from all current effects
 */
void Gyrocube::clear()
{
    update_required = false;
    function_changed = false;
}

/**
 * @brief Perform double vibration
 */
void Gyrocube::vibrate()
{
    motor->vibrate(VIBRATION_DURATION);
    sleep_ms(100);
    motor->vibrate(VIBRATION_DURATION);
}

/**
 * @brief Make a notification display
 * based on selected settings
 */
void Gyrocube::notify()
{
    uint8_t mode = settings[current_side].target;

    // TODO: make it better according to configuration
    if (mode == 0) {
        vibrate();
    }
    else if (mode == 1) {
        for (float i = 0; i <= 1; i += 0.1) {
            leds->interpolate(0x000000, 0xff0000, i);
            sleep_ms(50);
        }
    }
    else {
        for (float i = 0; i <= 1; i += 0.1) {
            leds->interpolate(0x000000, 0xff0000, i);
            sleep_ms(50);
        }
        vibrate();
    }
}