#include "activity_cycle.h"
#include <Arduino.h>
#include <stdint.h>

ActivityCycle::ActivityCycle()
    : state(ActivityCycleState::ACTIVE), active_time_ms(0), inactive_time_ms(0),
      last_time_ms(0) {
  // Initialize flags (all flags cleared by default)
  flags = 0;
}

bool ActivityCycle::is_enabled() { return (flags & (1 << FLAG_ENABLED)) != 0; }

bool ActivityCycle::has_state_changed() {
  bool result = (flags & (1 << FLAG_STATE_CHANGED)) != 0;
  flags &= ~(1 << FLAG_STATE_CHANGED);
  return result;
}

ActivityCycleState ActivityCycle::get_state() { return state; }

void ActivityCycle::set_times(unsigned long active_time_ms,
                              unsigned long inactive_time_ms) {
  this->active_time_ms = active_time_ms;
  this->inactive_time_ms = inactive_time_ms;
}

void ActivityCycle::enable() {
  flags = (1 << FLAG_ENABLED); // Set enabled, clear other flags
  state = ActivityCycleState::ACTIVE;
  last_time_ms = millis();
}

void ActivityCycle::disable() {
  flags &= ~(1 << FLAG_ENABLED);
  state = ActivityCycleState::ACTIVE; // Reset to default state
}

void ActivityCycle::update() {
  // Clear state changed flag
  flags &= ~(1 << FLAG_STATE_CHANGED);

  if (!is_enabled())
    return;

  unsigned long now = millis();
  unsigned long elapsed = now - last_time_ms;
  unsigned long target_duration =
      (state == ActivityCycleState::ACTIVE) ? active_time_ms : inactive_time_ms;

  // Check if current state duration has elapsed
  if (elapsed < target_duration) {
    return;
  }

  // Toggle state
  state = (state == ActivityCycleState::ACTIVE) ? ActivityCycleState::INACTIVE
                                                : ActivityCycleState::ACTIVE;
  flags |= (1 << FLAG_STATE_CHANGED);
  last_time_ms = now;
}