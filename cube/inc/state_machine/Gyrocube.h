#ifndef GYROCUBE_H
#define GYROCUBE_H


#include "state_machine/Event.h"
#include "sensor/Hih6020.h"
#include "actuator/NeoPixel.h"

#define N_SIDES                6
#define VIBRATION_DURATION     400
#define N_WEATHER_COLORS       10

#define CONNECTION_COLOR       0x0000ff
#define I2C_ERROR_COLOR        0xff0000

#define TEMP_MAX_DIFFERENCE    10
#define HUMID_MAX_DIFFERENCE   10

class Gyrocube;

typedef void (Gyrocube::*state_ptr)(const Event &);

typedef struct side_settings {
    uint8_t function;
    uint32_t color;
    int target;
} side_settings;

typedef struct machine_state {
    int humidity;
    int temperature;
} machine_state;

/**
 * @brief Abstraction of gyrocube system in form of a state machine
 * State machine UML can be viewed from the documentation
 */
class Gyrocube {
public:
    Gyrocube(
        Hih6020* sensor_,
        NeoPixel* leds_,
        VibrationMotor* motor_,
        uint8_t side,
        bool standalone = false
    );
    void handle_state(const Event& e);
    void update_settings(uint8_t side, side_settings new_setting);
    void update_weather(uint8_t new_color_index);
    machine_state get_info();
private:
    // States and state handlers
    state_ptr current_state;
    void set_state(state_ptr new_state);
    void startup(const Event& e);
    void state_idle(const Event& e);
    void state_lamp(const Event& e);
    void state_temp(const Event& e);
    void state_humid(const Event& e);
    void state_weather(const Event& e);
    void state_notification(const Event& e);

    // Support functions
    void clear();
    void display_temperature();
    void display_humidity();
    void notify();
    void vibrate();
    void appear(uint32_t color);
    void set_state(uint8_t state_index);
    void update_measurements();

    // Support variables
    int timer;                       // Timer for filtering tick events
    bool update_required;            // Flag identifies external changes
    bool function_changed;           // Flag identifes change of the function
    uint8_t current_side;            // Current cube's placement
    side_settings settings[N_SIDES]; // Setting individual for each side
    machine_state state;             // Current measurements and errors of the cube

    // Weather color array used by weather functions
    uint32_t weather_colors[N_WEATHER_COLORS];
    uint8_t weather_color_index;

    // Sensors and actuators
    Hih6020* sensor;
    NeoPixel* leds;
    VibrationMotor* motor;

    // State pointer array to quickly call needed state based on side's setting
    void (Gyrocube::*functional_states[N_SIDES])(const Event&);

};


#endif