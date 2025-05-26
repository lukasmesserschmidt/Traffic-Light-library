# Arduino Traffic Light Library

A minimal Arduino library designed for simulating a standard three-light traffic light (red, yellow, green). This library aims for ease of use in educational and prototyping contexts.

## Features

- **Individual Light Control:** Directly set the state (on/off) of each traffic light.
- **Configurable Cycles:** Define sequences of light patterns (phases) with specified durations to create automated traffic light behaviors.
- **Activity Cycles:** Implement an overarching active/inactive state for the traffic light, allowing it to operate or remain off for defined periods.
- **Defect Detection:** Monitor light functionality using analog test pins to detect and report light defects.
- **Event System:** Utilize a comprehensive event manager to connect callback functions to various traffic light occurrences, such as phase changes, cycle finishes, or light defects.

## Installation

To install this library in your Arduino IDE:

1.  Download the ZIP file of this repository.
2.  In the Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library...`.
3.  Navigate to and select the downloaded ZIP file.

## Usage

### Basic Traffic Light Control

Initialize a `TrafficLight` object by providing the digital pins connected to the red, yellow, and green LEDs.

```cpp
#include <TrafficLight.h>

TrafficLight trafficLight(13, 12, 11); // Red, Yellow, Green pins

void setup() {
  // Set a custom pattern (e.g., all lights on)
  trafficLight.set_pattern(true, true, true);
  trafficLight.update(); // Apply the pattern
}

void loop() {
  // Your main loop code
}
```

### Running a Traffic Cycle

Define an array of `Phase` structs, where each phase specifies a light pattern and its duration.

```cpp
#include <TrafficLight.h>

// Define phases
Phase phases[] = {
  {{true, false, false}, 3000}, // Red light for 3 seconds
  {{true, true, false}, 1000},  // Red and Yellow for 1 second
  {{false, false, true}, 5000}, // Green light for 5 seconds
  {{false, true, false}, 2000}  // Yellow light for 2 seconds
};

int phase_count = sizeof(phases) / sizeof(phases[0]);

TrafficLight trafficLight(13, 12, 11);

void setup() {
  trafficLight.set_cycle_phases(phases, phase_count); // Set the defined phases
  trafficLight.enable_cycle(); // Enable the traffic cycle
}

void loop() {
  trafficLight.update(); // Update the traffic light state
}
```

### Implementing an Activity Cycle

Set the active and inactive durations for the `ActivityCycle`. When enabled, the traffic light will automatically switch between active (running its cycle) and inactive (all lights off) states.

```cpp
#include <TrafficLight.h>

Phase phases[] = {
  {{true, false, false}, 3000},
  {{false, false, true}, 5000}
};

int phase_count = sizeof(phases) / sizeof(phases[0]);

TrafficLight trafficLight(13, 12, 11);

void setup() {
  trafficLight.set_cycle_phases(phases, phase_count);
  trafficLight.set_activity_cycle_times(10000, 5000); // Active for 10s, Inactive for 5s
  trafficLight.enable_activity_cycle(); // Enable the activity cycle
}

void loop() {
  trafficLight.update();
}
```

### Handling Events

Register callback functions to respond to various events, such as a phase change in the cycle or a light defect.

> **Note:** Each event can be associated with only one callback function. If you register another callback for the same event, it will overwrite the previously registered one.

```cpp
#include <TrafficLight.h>

TrafficLight trafficLight(13, 12, 11);

Phase phases[] = {
  {{true, false, false}, 3000},
  {{false, false, true}, 5000}
};

int phase_count = sizeof(phases) / sizeof(phases[0]);

void onCyclePhaseChanged() {
  Serial.println("Traffic cycle phase changed!");
}

void onRedLightDefect() {
  Serial.println("Red light defect detected!");
}

void setup() {
  Serial.begin(9600);
  trafficLight.set_cycle_phases(phases, phase_count);
  trafficLight.enable_cycle();

  // Set test pin for red light (e.g., analog pin A0)
  trafficLight.set_test_pin(0, A0); // Index 0 for red light

  // Register event callbacks
  trafficLight.register_event(EventName::CYCLE_PHASE_CHANGED, onCyclePhaseChanged); //
  trafficLight.register_event(EventName::RED_LIGHT_DEFECT, onRedLightDefect); //
}

void loop() {
  trafficLight.update();
}
```

### Limiting Cycle Repetitions

You can set a limit on how many times the traffic light cycle will repeat.

```cpp
#include <TrafficLight.h>

TrafficLight trafficLight(13, 12, 11);

Phase phases[] = {
  {{true, false, false}, 1000},
  {{false, false, true}, 1000}
};

int phase_count = sizeof(phases) / sizeof(phases[0]);

void setup() {
  trafficLight.set_cycle_phases(phases, phase_count);
  trafficLight.set_cycle_repetitions_limit(3); // Cycle will run 3 times
  trafficLight.enable_cycle();
}

void loop() {
  trafficLight.update();
}
```
