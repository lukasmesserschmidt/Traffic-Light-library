#ifndef EVENTS_H
#define EVENTS_H

#define EVENT_LIST                                                             \
  EVENT(RED_LIGHT_DEFECT)                                                      \
  EVENT(YELLOW_LIGHT_DEFECT)                                                   \
  EVENT(GREEN_LIGHT_DEFECT)                                                    \
  EVENT(RED_LIGHT_RECOVERED)                                                   \
  EVENT(YELLOW_LIGHT_RECOVERED)                                                \
  EVENT(GREEN_LIGHT_RECOVERED)                                                 \
  EVENT(CYCLE_PHASE_CHANGED)                                                   \
  EVENT(CYCLE_FINISHED)                                                        \
  EVENT(CYCLE_REACHED_REPETITIONS_LIMIT)                                       \
  EVENT(ACTIVITY_CYCLE_STATE_CHANGED)                                          \
  EVENT(ACTIVITY_CYCLE_TO_ACTIVE)                                              \
  EVENT(ACTIVITY_CYCLE_TO_INACTIVE)

enum class EventName {
#define EVENT(name) name,
  EVENT_LIST
#undef EVENT
      COUNT
};

struct Event {
  EventName name;
  void (*callback)();
};

class EventManager {
public:
  /**
   * Connects a callback function to an event
   * @param name The name of the event
   * @param callback The callback
   */
  void connect(EventName name, void (*callback)());

  /**
   * Disconnects a callback function from an event
   * @param name The name of the event
   */
  void disconnect(EventName name);

  /**
   * Triggers the callback function of an event
   * @param name The name of the event
   */
  void emit(EventName name);

private:
  Event events[static_cast<int>(EventName::COUNT)] = {
#define EVENT(name) {EventName::name, nullptr},
      EVENT_LIST
#undef EVENT
  };
};

#endif