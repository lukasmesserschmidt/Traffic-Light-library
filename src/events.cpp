#include "events.h"

void EventManager::connect(EventName name, void (*callback)()) {
  events[static_cast<int>(name)].callback = callback;
}

void EventManager::disconnect(EventName name) {
  events[static_cast<int>(name)].callback = nullptr;
}

void EventManager::emit(EventName name) {
  void (*callback)() = events[static_cast<int>(name)].callback;
  if (callback != nullptr) {
    callback();
  }
}
