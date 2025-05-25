#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "activity_cycle.h"
#include "cycle.h"
#include "events.h"

class TrafficLight {
   private:
    int light_pins[3];
    int test_pins[3];
    bool intact_lights[3];
    bool pattern[3];
    Cycle cycle;
    ActivityCycle activity_cycle;
    EventManager event_manager;

    /**
     * Test the light pins by checking their analog readings.
     * Updates the intact_lights array based on the readings.
     */
    void test_light_pins();

   public:
    /**
     * Constructor for the TrafficLight class.
     * @param red_pin The pin connected to the red light.
     * @param yellow_pin The pin connected to the yellow light.
     * @param green_pin The pin connected to the green light.
     */
    TrafficLight(int red_pin, int yellow_pin, int green_pin);

    /**
     * Checks if the cycle is enabled.
     * @return True if the cycle is enabled, false otherwise.
     */
    bool is_cycle_enabled();

    /**
     * Checks if the activity cycle is enabled.
     * @return True if the activity cycle is enabled, false otherwise.
     */
    bool is_activity_cycle_enabled();

    /**
     * Gets the current pattern of the traffic light.
     * @return The current pattern represented as a boolean array.
     */
    bool* get_current_pattern();

    /**
     * Sets the test pin for a specific light.
     * @param index The index of the light (0: red, 1: yellow, 2: green).
     * @param pin The pin connected to the light.
     */
    void set_test_pin(int index, int pin);

    /**
     * Sets the test pins for all lights.
     * @param red_pin The pin connected to the red light.
     * @param yellow_pin The pin connected to the yellow light.
     * @param green_pin The pin connected to the green light.
     */
    void set_test_pins(int red_pin, int yellow_pin, int green_pin);

    /**
     * Sets the pattern for the traffic light.
     * @param red_light The state of the red light.
     * @param yellow_light The state of the yellow light.
     * @param green_light The state of the green light.
     */
    void set_pattern(bool red_light, bool yellow_light, bool green_light);

    /**
     * Sets the repetitions limit for the cycle.
     * @param repetitions_limit The number of repetitions to run the cycle before stopping.
     */
    void set_cycle_repetitions_limit(unsigned long repetitions_limit = 0);

    /**
     * Sets the phases for the cycle.
     * @param phases An array of Phase objects representing the sequence of phases.
     * @param phase_count The number of phases in the sequence.
     */
    void set_cycle_phases(Phase* phases, int phase_count);

    /**
     * Sets the times for the active and inactive states of the activity cycle.
     * @param active_time_ms The time in milliseconds for the active state.
     * @param inactive_time_ms The time in milliseconds for the inactive state.
     */
    void set_activity_cycle_times(unsigned long active_time_ms, unsigned long inactive_time_ms);

    /**
     * Enables the cycle.
     */
    void enable_cycle();

    /**
     * Disables the cycle.
     */
    void disable_cycle();

    /**
     * Enables the activity cycle.
     */
    void enable_activity_cycle();

    /**
     * Disables the activity cycle.
     */
    void disable_activity_cycle();

    /**
     * Registers an event callback for a specific event.
     * @param name The name of the event.
     * @param callback The function to be called when the event is emitted.
     */
    void register_event(EventName name, void (*callback)());

    /**
     * Unregisters an event callback for a specific event.
     * @param name The name of the event.
     */
    void unregister_event(EventName name);

    /**
     * Updates the state of the traffic light, checking for light defects and updating the activity cycle.
     */
    void update();
};

#endif