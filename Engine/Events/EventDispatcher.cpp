#include "../Events/EventDispatcher.h"

using namespace CodeMonkeys::Engine::Events;


void EventDispatcher::register_event(std::string event_name, std::function<void()> handler) {
    this->event_handlers[event_name] = handler;
}

void EventDispatcher::report_event(std::string event_name) {
    this->event_queue.push(event_name);
}

void EventDispatcher::dispatch() {
    while (!this->event_queue.empty())
    {
        std::string event_name = event_queue.front();
        event_queue.pop();

        if (event_handlers.count(event_name) == 0)
        {
            throw "unhandled game event: " + event_name;
        }

        std::function<void()> handler = event_handlers[event_name];
        handler();
    }
}
