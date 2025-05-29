#include "traffic_light.h"

#include <Arduino.h>

// constructor
TrafficLight::TrafficLight(int red_pin, int yellow_pin, int green_pin)
    : light_pins{red_pin, yellow_pin, green_pin},
      test_pins{INVALID_PIN, INVALID_PIN, INVALID_PIN},
      intact_lights{true, true, true}, pattern{false, false, false}, cycle(),
      activity_cycle(), event_manager() {
  // Initialize light pins
  for (int i = 0; i < NUM_LIGHTS; i++) {
    pinMode(light_pins[i], OUTPUT);
    digitalWrite(light_pins[i], LOW);
  }
}

// getters
bool TrafficLight::is_cycle_enabled() { return cycle.is_enabled(); }

bool TrafficLight::is_activity_cycle_enabled() {
  return activity_cycle.is_enabled();
}

bool *TrafficLight::get_pattern() { return pattern; }

// setters
void TrafficLight::set_test_pin(int index, int pin) {
  if (index < 0 || index >= NUM_LIGHTS) {
    return; // Invalid index
  }

  // Set new pin
  test_pins[index] = pin;

  // Configure new pin if valid
  if (pin != INVALID_PIN) {
    pinMode(pin, INPUT);
  }
}

void TrafficLight::set_test_pins(int red_pin, int yellow_pin, int green_pin) {
  set_test_pin(0, red_pin);
  set_test_pin(1, yellow_pin);
  set_test_pin(2, green_pin);
}

void TrafficLight::set_pattern(bool red_light, bool yellow_light,
                               bool green_light) {
  pattern[0] = red_light;
  pattern[1] = yellow_light;
  pattern[2] = green_light;
}

void TrafficLight::set_cycle_repetitions_limit(
    unsigned long repetitions_limit) {
  cycle.set_repetitions_limit(repetitions_limit);
}

void TrafficLight::set_cycle_phases(Phase *phases, int phase_count) {
  cycle.set_phases(phases, phase_count);
}

void TrafficLight::set_activity_cycle_times(unsigned long active_time_ms,
                                            unsigned long inactive_time_ms) {
  activity_cycle.set_times(active_time_ms, inactive_time_ms);
}

void TrafficLight::set_auto_lights_off(bool enabled) {
  auto_lights_off = enabled;
}

void TrafficLight::set_auto_recovery(bool enabled) {
  auto_recovery_enabled = enabled;
}

// controls
void TrafficLight::enable_cycle() {
  cycle.enable();
  Phase *phase = cycle.get_phase();
  if (phase != nullptr) {
    set_pattern(phase->pattern[0], phase->pattern[1], phase->pattern[2]);
  }
}

void TrafficLight::disable_cycle() {
  cycle.disable();
  if (auto_lights_off) {
    set_pattern(false, false, false);
  }
}

void TrafficLight::enable_activity_cycle() { activity_cycle.enable(); }

void TrafficLight::disable_activity_cycle() { activity_cycle.disable(); }

// events
void TrafficLight::register_event(EventName name, void (*callback)()) {
  event_manager.connect(name, callback);
}

void TrafficLight::unregister_event(EventName name) {
  event_manager.disconnect(name);
}

// update
void TrafficLight::update() {
  // Update activity cycle
  activity_cycle.update();
  if (activity_cycle.has_state_changed()) {
    on_activity_cycle_state_changed();
  }

  // Update cycle
  cycle.update();

  // Check for cycle events
  if (cycle.has_phase_changed()) {
    on_cycle_phase_changed();
  }
  if (cycle.has_finished()) {
    on_cycle_finished();
  }
  if (cycle.has_reached_repetitions_limit()) {
    on_cycle_reached_repetitions_limit();
  }

  // power lights
  for (int i = 0; i < 3; i++) {
    digitalWrite(light_pins[i], pattern[i] ? HIGH : LOW);
  }

  // Test for defects if any test pin is configured
  for (int i = 0; i < NUM_LIGHTS; i++) {
    if (test_pins[i] != INVALID_PIN) {
      test_for_defekt_lights();
      break;
    }
  }
}

void TrafficLight::on_activity_cycle_state_changed() {
  ActivityCycleState state = activity_cycle.get_state();

  // enable/disable cycle and emit events
  if (state == ActivityCycleState::ACTIVE) {
    enable_cycle();
    event_manager.emit(EventName::ACTIVITY_CYCLE_TO_ACTIVE);
  } else if (state == ActivityCycleState::INACTIVE) {
    disable_cycle();
    event_manager.emit(EventName::ACTIVITY_CYCLE_TO_INACTIVE);
  }

  event_manager.emit(EventName::ACTIVITY_CYCLE_STATE_CHANGED);
}

void TrafficLight::on_cycle_phase_changed() {
  Phase *phase = cycle.get_phase();

  if (phase == nullptr)
    return;

  set_pattern(phase->pattern[0], phase->pattern[1], phase->pattern[2]);

  event_manager.emit(EventName::CYCLE_PHASE_CHANGED);
}

void TrafficLight::on_cycle_finished() {
  event_manager.emit(EventName::CYCLE_FINISHED);
}

void TrafficLight::on_cycle_reached_repetitions_limit() {
  if (auto_lights_off) {
    set_pattern(false, false, false);
  }
  event_manager.emit(EventName::CYCLE_REACHED_REPETITIONS_LIMIT);
}

void TrafficLight::test_for_defekt_lights() {
  static const EventName defect_event_names[] = {EventName::RED_LIGHT_DEFECT,
                                                 EventName::YELLOW_LIGHT_DEFECT,
                                                 EventName::GREEN_LIGHT_DEFECT};
  static const EventName recovered_event_names[] = {
      EventName::RED_LIGHT_RECOVERED, EventName::YELLOW_LIGHT_RECOVERED,
      EventName::GREEN_LIGHT_RECOVERED};

  static unsigned long last_test_time = 0;
  const unsigned long test_interval = 100; // Test every 100ms
  unsigned long now = millis();
  unsigned long elapsed = (now - last_test_time);

  // Throttle the test frequency
  if (elapsed < test_interval) {
    return;
  }
  last_test_time = now;

  for (int i = 0; i < NUM_LIGHTS; i++) {
    if (test_pins[i] == INVALID_PIN) {
      continue; // No test pin for this light
    }

    // Only test lights that are supposed to be on
    if (!pattern[i]) {
      continue;
    }

    int reading = analogRead(test_pins[i]);
    bool is_defect = (reading > DEFECT_THRESHOLD);

    if (is_defect && intact_lights[i]) {
      // Light just became defective
      intact_lights[i] = false;
      event_manager.emit(defect_event_names[i]);
    } else if (!is_defect && !intact_lights[i] && auto_recovery_enabled) {
      // Light was defective but is now functioning and auto-recovery is
      // enabled
      intact_lights[i] = true;
      event_manager.emit(recovered_event_names[i]);
    }
  }
}