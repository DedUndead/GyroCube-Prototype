#include <string.h>
#include <stdio.h>
#include <math.h>
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
    settings {
        { 0, 0x000000, 0 },
        { 1, 0xff00ff, 0 },
        { 2, 0x000000, 25 },
        { 3, 0x000000, 25 },
        { 4, 0x000000, 0 },
        { 5, 0xff0000, 2 }
    },
    sensor(sensor_),
    leds(leds_),
    motor(motor_),
    weather_colors {
        0xc0f6fb, 0xdddfdf, 0xdddfdf, 0xc2c2c2, 0xc2c2c2,
        0xdddfdf, 0xc0f6fb, 0xdddfdf, 0xf2f27a, 0xf0a144,
    },
    weather_color_index(0),
    functional_states { 
        &Gyrocube::state_idle,    &Gyrocube::state_lamp,
        &Gyrocube::state_temp,    &Gyrocube::state_humid,
        &Gyrocube::state_weather, &Gyrocube::state_notification
    }
{    
    // Set initial state according to state machine's mode
    current_side = side;
    if (!standalone) current_state = &Gyrocube::startup;
    else             current_state = functional_states[settings[current_side].function];
}

/**
 * @brief Change current state machine state
 * Call enter event after change
 * Delay for state transition is required to assure that
 * neopixel clock is syncronized
 * @param new_state Pointer to state
 */
void Gyrocube::set_state(state_ptr new_state)
{
    sleep_ms(10); // Delay for state transition
    (this->*current_state)(Event(Event::eExit));

    current_state = new_state;

    sleep_ms(10); // Delay for state transition
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
 * Delay for event transition is required to assure that
 * neopixel clock is syncronized
 * @param e Event object
 */
void Gyrocube::handle_state(const Event& e)
{
    sleep_ms(10); // Delay for event transition
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
 * @brief Update weather
 * @param new_color_index New color index 
 */
void Gyrocube::update_weather(uint8_t new_color_index)
{
    weather_color_index = new_color_index;
    update_required = true;
}

/**
 * @brief Get current machine measurement
 * @return machine_state Current machine measurements
 */
machine_state Gyrocube::get_info()
{
    return state;
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
            appear(CONNECTION_COLOR);

            break;
        case Event::eNotify:
            current_side = e.value;
            set_state(settings[current_side].function);

            break;
        case Event::eExit:
            leds->fill(0x000000);

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
            update_measurements();
            if (function_changed) {
                function_changed = false;
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
        case Event::eExit:
            leds->fill(0x000000);

            break;
        default:
            break;
    }
}

/**
 * @brief Lamp state
 * Display a certain color, user can configure
 */
void Gyrocube::state_lamp(const Event& e)
{
    switch (e.type) {
        case Event::eEnter:
            clear();
            leds->fill(settings[current_side].color);

            break;
        case Event::eTick:
            update_measurements();

            if (function_changed) {
                function_changed = false;
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
        case Event::eExit:
            leds->fill(0x000000);

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
            update_measurements();

            if (function_changed) {
                function_changed = false;
                set_state(settings[current_side].function);
            }

            display_temperature();

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            current_side = e.value;
            set_state(settings[current_side].function);
            
            break;
        case Event::eExit:
            leds->fill(0x000000);

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
            update_measurements();

            if (function_changed) {
                function_changed = false;
                set_state(settings[current_side].function);
            }

            display_humidity();

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            current_side = e.value;
            set_state(settings[current_side].function);
            
            break;
        case Event::eExit:
            leds->fill(0x000000);

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
            leds->fill(weather_colors[weather_color_index]);

            break;
        case Event::eTick:
            update_measurements();

            if (function_changed) {
                set_state(settings[current_side].function);
            }

            if (update_required) {
                leds->fill(weather_colors[weather_color_index]);
                update_required = false;
            }

            // DEBUGGING
            if (weather_color_index < N_WEATHER_COLORS) weather_color_index++;
            else weather_color_index = 0;
            update_weather(weather_color_index);

            break;
        case Event::eReconnect:
            set_state(&Gyrocube::startup);

            break;
        case Event::eChange:
            current_side = e.value;
            set_state(settings[current_side].function);
            
            break;
        case Event::eExit:
            leds->fill(0x000000);

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
            update_measurements();
            
            if (function_changed) {
                set_state(settings[current_side].function);
            }

            break;
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
        case Event::eExit:
            leds->fill(0x000000);

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
 * @brief Display temperature to the user using actuators
 * Display error color signal in case  of hw failure
 */
void Gyrocube::display_temperature()
{
    // Display error
    if (state.temperature == HIH_ERROR_STATUS) {
        appear(I2C_ERROR_COLOR);
        vibrate();
        return;
    }
    
    // Calculate difference value and make sure it doesnt exceed limit
    float difference = settings[current_side].target - state.temperature;
    if      (difference > TEMP_MAX_DIFFERENCE)      difference = TEMP_MAX_DIFFERENCE;
    else if (difference < -1 * TEMP_MAX_DIFFERENCE) difference = -1 * TEMP_MAX_DIFFERENCE;

    // Temperature matches target
    if (difference == 0) {
        leds->fill(0x00, 0xff, 0x00);
    }
    // Green->red gradient
    else if (difference < 0) {
        leds->interpolate(
            0x00ff00, 0xff0000, abs(difference) / TEMP_MAX_DIFFERENCE
        );
    }
    // Green->blue gradient
    else {
        leds->interpolate(
            0x00ff00, 0x0000ff, abs(difference) / TEMP_MAX_DIFFERENCE
        );
    }
}

/**
 * @brief Display humidity to the user using actuators
 * Display error color signal in case  of hw failure
 */
void Gyrocube::display_humidity()
{
    // Display error
    if (state.humidity == HIH_ERROR_STATUS) {
        appear(I2C_ERROR_COLOR);
        vibrate();
        return;
    }
    
    // Calculate difference value and make sure it doesnt exceed limit
    float difference = abs(settings[current_side].target - state.humidity);
    if (difference > HUMID_MAX_DIFFERENCE) difference = HUMID_MAX_DIFFERENCE;

    // Humidity matches target
    if (difference == 0) {
        leds->fill(0x00, 0xff, 0x00);
    }
    // Green->red gradient
    else {
        leds->interpolate(
            0x00ff00, 0xff0000, abs(difference) / HUMID_MAX_DIFFERENCE
        );
    }
}

/**
 * @brief Perform double vibration
 */
void Gyrocube::vibrate()
{
    motor->vibrate(VIBRATION_DURATION);
}

/**
 * @brief Make a notification display
 * based on selected settings
 */
void Gyrocube::notify()
{
    // Parse current settings
    uint8_t mode = settings[current_side].target;

    if (mode == 0) {
        vibrate();
    }
    else if (mode == 1) {
        appear(settings[current_side].color);
    }
    else {
        appear(settings[current_side].color);
        vibrate();
    }
}

/**
 * @brief Perform appearing effect of the color
 * @param color Target color
 */
void Gyrocube::appear(uint32_t color)
{
    for (float i = 0; i <= 1; i += 0.1) {
        leds->interpolate(0x000000, color, i);
        sleep_ms(30);
    }
    leds->fill(0x000000);
}

/**
 * @brief Update internal measurements of the machine
 */
void Gyrocube::update_measurements()
{
    // Fetch data once and use one reading to update buffer
    state.humidity = sensor->read_humidity();
    state.temperature = sensor->read_temperature(false);
}