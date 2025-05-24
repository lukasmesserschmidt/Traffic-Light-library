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

   public:
    TrafficLight(int red_pin, int yellow_pin, int green_pin);

    bool is_cycle_enabled();
    bool is_activity_cycle_enabled();
    bool* get_current_pattern();

    void set_test_pin(int index, int pin);
    void set_test_pins(int red_pin, int yellow_pin, int green_pin);
    void set_pattern(bool red_light, bool yellow_light, bool green_light);
    void set_cycle_repetitions_limit(unsigned long repetitions_limit = 0);
    void set_cycle_phases(Phase* phases, int phase_count);
    void set_activity_cycle_times(unsigned long active_time_ms, unsigned long inactive_time_ms);

    void enable_cycle();
    void disable_cycle();
    void enable_activity_cycle();
    void disable_activity_cycle();

    void register_event(EventName name, void (*callback)());
    void unregister_event(EventName name);

    void update();
    void test_light_pins();
};

#endif