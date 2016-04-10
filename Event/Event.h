#pragma once
#include <set>
#include <map>
#include <type_traits>

#include "Function.h"

#define EnableEventMap(senderClass, eventClass) public Event<senderClass>, public cru::internal_event_::EventHandlerMap_<senderClass, eventClass>

namespace cru
{
    //forward declarations
    template<typename Event_>
    int SendEvent(Event_&);

    namespace internal_event_
    {
        template<typename Sender_, typename Event_>
        class EventHandlerMap_;
    }



    template<typename Event_>
    class EventHandler
    {
        friend internal_event_::EventHandlerMap_<typename Event_::Sender, Event_>;
    public:
        using EventClass = Event_;
        using Sender = typename EventClass::Sender;
        using FunctionType = Function<void(EventClass&)>;

        EventHandler() = default;
        template<typename... Args>
        EventHandler(Sender* specified_sender, Args&&... function);
        EventHandler(EventHandler&& other);
        EventHandler& operator = (EventHandler&& other);
        ~EventHandler();

        bool GetState() const { return listening_state_; }
        Sender* GetSender() const { return specified_sender_; }
        FunctionType GetFunction() const { return function_; }

        void TurnOn();
        void TurnOff();
        void Set(Sender* specified_sender);
        template<typename... Args>
        void Set(Args&&... function);

    private:
        void HandleEvent(EventClass& event_) const { function_(event_); }

        Sender* specified_sender_ = nullptr;
        bool listening_state_ = false;
        FunctionType function_;
    };

    template<typename Event_>
    template<typename ...Args>
    EventHandler<Event_>::EventHandler(Sender * specified_sender, Args && ...function)
        : specified_sender_(specified_sender), function_(std::forward<Args>(function)...)
    {
        TurnOn();
    }

    template<typename Event_>
    template<typename ...Args>
    void EventHandler<Event_>::Set(Args && ...function)
    {
        function_ = FunctionType(std::forward<Args>(function)...);
    }

    template<typename Event_>
    EventHandler<Event_>::EventHandler(EventHandler && other)
        : specified_sender_(other.specified_sender_), listening_state_(other.listening_state_), function_(std::move(other.function_))
    {
        if (listening_state_)
        {
            EventClass::RemoveHandlerFromMap(&other, false);
            EventClass::AddHandlerToMap(this);
        }
    }

    template<typename Event_>
    EventHandler<Event_> & EventHandler<Event_>::operator=(EventHandler && other)
    {
        if (this != &other)
        {
            TurnOff();
            specified_sender_ = other.specified_sender_;
            function_ = std::move(other.function_);
            listening_state_ = other.listening_state_;
            if (listening_state_)
            {
                EventClass::RemoveHandlerFromMap(&other, false);
                EventClass::AddHandlerToMap(this);
            }
        }
        return *this;
    }

    template<typename Event_>
    EventHandler<Event_>::~EventHandler()
    {
        TurnOff();
    }

    template<typename Event_>
    void EventHandler<Event_>::TurnOn()
    {
        if (!listening_state_)
        {
            EventClass::AddHandlerToMap(this);
            listening_state_ = true;
        }
    }

    template<typename Event_>
    inline void EventHandler<Event_>::TurnOff()
    {
        if (listening_state_)
        {
            EventClass::RemoveHandlerFromMap(this);
            listening_state_ = false;
        }
    }

    template<typename Event_>
    void EventHandler<Event_>::Set(Sender * specified_sender)
    {
        TurnOff();
        specified_sender_ = specified_sender;
        TurnOn();
    }



    template<typename Sender_>
    class Event
    {
    public:
        using Sender = Sender_;
        Event(Sender* sender) : sender_(sender) { }
        Sender* GetSender() const { return sender_; }
    private:
        Sender* sender_;
    };

    namespace internal_event_
    {
        template<typename SpecificEvent, typename Sender>
        constexpr bool baseOnEvent_()
        {
            return std::is_base_of<Event<Sender>, SpecificEvent>::value;
        }


        template<typename Sender_, typename Event_>
        class EventHandlerMap_
        {
            friend class cru::EventHandler<Event_>;
            friend int cru::SendEvent(Event_&);
        private:
            using EventClass = Event_;
            using SenderPrivate_ = Sender_;
            using Handler = EventHandler<EventClass>;

            static int HandleEvent(EventClass& event_);
            static int HandleEvent_helper_(EventClass& event_, SenderPrivate_* sender);
            static void AddHandlerToMap(Handler* handler);
            static void RemoveHandlerFromMap(Handler* handler, bool clean = true);

            static std::map<SenderPrivate_*, std::set<Handler*>> event_handler_map_;
        };

        template<typename Sender_, typename Event_>
        std::map<Sender_*, std::set<EventHandler<Event_>*>> EventHandlerMap_<Sender_, Event_>::event_handler_map_;


        template<typename Sender_, typename Event_>
        int EventHandlerMap_<Sender_, Event_>::HandleEvent(EventClass & event)
        {
            int result = 0;
            result += HandleEvent_helper_(event, nullptr);
            if (event.GetSender())
                result += HandleEvent_helper_(event, event.GetSender());
            return result;
        }

        template<typename Sender_, typename Event_>
        int EventHandlerMap_<Sender_, Event_>::HandleEvent_helper_(EventClass & event, SenderPrivate_ * sender)
        {
            int result = 0;
            auto handlers = event_handler_map_.find(sender);
            if (handlers != event_handler_map_.end())
                for (auto i : handlers->second)
                {
                    i->HandleEvent(event);
                    result++;
                }
            return result;
        }

        template<typename Sender_, typename Event_>
        void EventHandlerMap_<Sender_, Event_>::AddHandlerToMap(Handler * handler)
        {
            event_handler_map_[handler->GetSender()].insert(handler);
        }

        template<typename Sender_, typename Event_>
        void EventHandlerMap_<Sender_, Event_>::RemoveHandlerFromMap(Handler * handler, bool clean)
        {
            event_handler_map_[handler->GetSender()].erase(handler);
            if (clean)
                if (event_handler_map_[handler->GetSender()].empty())
                    event_handler_map_.erase(handler->GetSender());
        }
    }

    template<typename Event_>
    int SendEvent(Event_& event)
    {
        return Event_::HandleEvent(event);
    }

    template<typename Event_, typename... Args>
    int SendEvent(Args&&... args)
    {
        Event_ event(std::forward<Args>(args)...);
        return SendEvent(event);
    }
}
