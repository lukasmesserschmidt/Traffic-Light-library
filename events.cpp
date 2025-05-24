#include "events.h"

void EventManager::connect(EventName name, void (*callback)()) {
    events[static_cast<int>(name)].callback = callback;
}

void EventManager::disconnect(EventName name) {
    events[static_cast<int>(name)].callback = nullptr;
}

void EventManager::emit(EventName name) {
    if (events[static_cast<int>(name)].callback == nullptr) return;
    events[static_cast<int>(name)].callback();
}
