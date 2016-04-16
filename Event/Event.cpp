#include "Event.h"

namespace cru
{
    EventHandler& EventHandler::operator=(const EventHandler & other)
    {
        if (this != &other)
        {
            Destroy_();
            function_ = other.function_->Clone();
            object_ = other.object_;
        }
        return *this;
    }

    EventHandler & EventHandler::operator=(EventHandler && other)
    {
        if (this != &other)
        {
            Destroy_();
            function_ = other.function_;
            other.function_ = nullptr;
            object_ = other.object_;
        }
        return *this;
    }

    BeEventHandler::~BeEventHandler()
    {
        destruction_event.Raise();
    }

    void Event::EventHandlerMap::AddHandler(const EventHandler & handler)
    {
        if (!map_)
            map_ = new EventHandlerMap_;
        map_->emplace(handler.GetObject(), handler);
    }

    void Event::EventHandlerMap::AddHandler(EventHandler && handler)
    {
        if (!map_)
            map_ = new EventHandlerMap_;
        map_->emplace(handler.GetObject(), std::move(handler));
    }

    void Event::EventHandlerMap::RemoveHandler(BeEventHandler * object)
    {
        map_->erase(object);
        if (map_->empty())
        {
            delete map_;
            map_ = nullptr;
        }
    }

    void Event::EventHandlerMap::Handle() const
    {
        if (map_)
            for (auto& i : *map_)
                i.second();
    }

    Event::EventHandlerMap::~EventHandlerMap()
    {
        if (map_)
            delete map_;
    }
}
