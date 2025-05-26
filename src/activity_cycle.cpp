#include "activity_cycle.h"

#include <Arduino.h>

ActivityCycle::ActivityCycle()
    : enabled(false),
      state(ActivityCycleState::ACTIVE),
      state_changed(false),
      active_time_ms(0),
      inactive_time_ms(0),
      last_time_ms(0) {}

bool ActivityCycle::is_enabled() {
    return enabled;
}

bool ActivityCycle::has_state_changed() {
    bool result = state_changed;
    state_changed = false;
    return result;
}

ActivityCycleState ActivityCycle::get_state() {
    return state;
}

void ActivityCycle::set_times(unsigned long active_time_ms, unsigned long inactive_time_ms) {
    this->active_time_ms = active_time_ms;
    this->inactive_time_ms = inactive_time_ms;
}

void ActivityCycle::enable() {
    enabled = true;
    state_changed = false;
    state = ActivityCycleState::ACTIVE;
    last_time_ms = millis();
}

void ActivityCycle::disable() {
    enabled = false;
    state = ActivityCycleState::ACTIVE;
}

void ActivityCycle::update() {
    state_changed = false;

    if (!enabled) return;

    unsigned long now = millis();
    unsigned long elapsed = now - last_time_ms;

    ActivityCycleState previous_state = state;

    // update state
    if (state == ActivityCycleState::INACTIVE && elapsed >= inactive_time_ms) {
        state = ActivityCycleState::ACTIVE;
    } else if (state == ActivityCycleState::ACTIVE && elapsed >= active_time_ms) {
        state = ActivityCycleState::INACTIVE;
    }

    // state has changed
    if (previous_state != state) {
        state_changed = true;
        last_time_ms = now;
    }
}