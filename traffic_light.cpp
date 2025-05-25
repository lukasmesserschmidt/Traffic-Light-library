#include "traffic_light.h"

#include <Arduino.h>

#define INVALID_PIN 0
#define DEFECT_THRESHOLD 1000

// constructor
TrafficLight::TrafficLight(int red_pin, int yellow_pin, int green_pin)
    : light_pins{red_pin, yellow_pin, green_pin},
      test_pins{INVALID_PIN, INVALID_PIN, INVALID_PIN},
      intact_lights{true, true, true},
      pattern{false, false, false},
      cycle(),
      activity_cycle(),
      event_manager() {
    pinMode(red_pin, OUTPUT);
    pinMode(yellow_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
}

// getters
bool TrafficLight::is_cycle_enabled() {
    return cycle.is_enabled();
}

bool TrafficLight::is_activity_cycle_enabled() {
    return activity_cycle.is_enabled();
}

bool* TrafficLight::get_current_pattern() {
    return pattern;
}

// setters
void TrafficLight::set_test_pin(int index, int pin) {
    test_pins[index] = pin;
    pinMode(pin, INPUT);
}

void TrafficLight::set_test_pins(int red_pin, int yellow_pin, int green_pin) {
    set_test_pin(0, red_pin);
    set_test_pin(1, yellow_pin);
    set_test_pin(2, green_pin);
}

void TrafficLight::set_pattern(bool red_light, bool yellow_light, bool green_light) {
    pattern[0] = red_light;
    pattern[1] = yellow_light;
    pattern[2] = green_light;
}

void TrafficLight::set_cycle_repetitions_limit(unsigned long repetitions_limit) {
    cycle.set_repetitions_limit(repetitions_limit);
}

void TrafficLight::set_cycle_phases(Phase* phases, int phase_count) {
    cycle.set_phases(phases, phase_count);
}

void TrafficLight::set_activity_cycle_times(unsigned long active_time_ms, unsigned long inactive_time_ms) {
    activity_cycle.set_times(active_time_ms, inactive_time_ms);
}

// controls
void TrafficLight::enable_cycle() {
    cycle.enable();
}

void TrafficLight::disable_cycle() {
    cycle.disable();
    set_pattern(false, false, false);
}

void TrafficLight::enable_activity_cycle() {
    activity_cycle.enable();
}

void TrafficLight::disable_activity_cycle() {
    activity_cycle.disable();
}

// events
void TrafficLight::register_event(EventName name, void (*callback)()) {
    event_manager.connect(name, callback);
}

void TrafficLight::unregister_event(EventName name) {
    event_manager.disconnect(name);
}

// update
void TrafficLight::update() {
    if (activity_cycle.is_enabled()) {
        // update activity cycle
        activity_cycle.update();

        // enable/disable cycle and emit events
        if (activity_cycle.has_state_changed()) {
            if (activity_cycle.get_state() == ActivityCycleState::ACTIVE) {
                enable_cycle();
                event_manager.emit(EventName::ACTIVITY_CYCLE_TO_ACTIVE);
            } else if (activity_cycle.get_state() == ActivityCycleState::INACTIVE) {
                disable_cycle();
                event_manager.emit(EventName::ACTIVITY_CYCLE_TO_INACTIVE);
            }

            event_manager.emit(EventName::ACTIVITY_CYCLE_STATE_CHANGED);
        }
    }

    if (cycle.is_enabled() && cycle.get_phase_count() > 0) {
        // update cycle
        cycle.update();

        // update pattern
        for (int i = 0; i < 3; i++) {
            pattern[i] = cycle.get_current_phase()->pattern[i];
        }

        // emit events
        if (cycle.has_phase_changed()) {
            event_manager.emit(EventName::CYCLE_PHASE_CHANGED);
        }
        if (cycle.has_restarted()) {
            event_manager.emit(EventName::CYCLE_RESTARTED);
        }
        if (cycle.has_reached_repetitions_limit()) {
            event_manager.emit(EventName::CYCLE_REACHED_REPETITIONS_LIMIT);
        }
    }

    // power lights
    for (int i = 0; i < 3; i++) {
        digitalWrite(light_pins[i], pattern[i] ? HIGH : LOW);
    }

    // test pins
    test_light_pins();
}

void TrafficLight::test_light_pins() {
    EventName event_names[] = {
        EventName::RED_LIGHT_DEFECT,
        EventName::YELLOW_LIGHT_DEFECT,
        EventName::GREEN_LIGHT_DEFECT,
    };

    for (int i = 0; i < 3; i++) {
        if (test_pins[i] != INVALID_PIN && analogRead(test_pins[i]) > DEFECT_THRESHOLD) {
            if (!intact_lights[i]) continue;

            intact_lights[i] = false;
            event_manager.emit(event_names[i]);
        }
    }
}