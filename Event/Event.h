#pragma once
#include <utility>
#include <map>
#include <type_traits>

#define EnableEventHandler public cru::BeEventHandler

namespace cru
{
    class Event;
    class BeEventHandler;

    namespace internal_event_
    {
        class EventHandler_
        {
        public:
            virtual EventHandler_*  Clone()       const = 0;
            virtual void            Handle()      const = 0;
        };

        class FunctionPointer_ : public EventHandler_
        {
        public:
            using FunctionPointer = void(*)();

            FunctionPointer_(FunctionPointer pointer) : pointer_(pointer) { }

            FunctionPointer_*       Clone() const override { return new FunctionPointer_(pointer_); }
            void                    Handle() const override { return (*pointer_)(); }
        private:
            FunctionPointer pointer_;
        };

        template<typename C>
        class MemberFunctionPointer_ : public EventHandler_
        {
        public:
            using ObjectPointer = C*;
            typedef void(C::*MemberFunctionPointer)();

            MemberFunctionPointer_(ObjectPointer object, MemberFunctionPointer pointer)
                : object_(object), pointer_(pointer) { }

            MemberFunctionPointer_* Clone() const override { return new MemberFunctionPointer_(object_, pointer_); }
            void                    Handle() const override { return (object_->*pointer_)(); }
        private:
            ObjectPointer object_;
            MemberFunctionPointer pointer_;
        };

        template<typename C>
        class Functor_ : public EventHandler_
        {
        public:
            using ObjectPointer = C*;

            Functor_(ObjectPointer object) : object_(object) { }

            Functor_*               Clone() const override { return new Functor_(object_); }
            void                    Handle() const override { return (*object_)(); }
        private:
            ObjectPointer object_;
        };

        template<typename C>
        class Lambda_ : public EventHandler_
        {
        public:
            using LambdaClass = C;

            Lambda_(const LambdaClass& lambda) : lambda_(lambda) { }

            Lambda_*                Clone() const override { return new Lambda_(lambda_); }
            void                    Handle() const override { return lambda_(); }
        private:
            LambdaClass lambda_;
        };
    }


    class EventHandler
    {
    public:
        EventHandler() = default;
        EventHandler(void(*functionPointer)())
            : function_(new internal_event_::FunctionPointer_(functionPointer)) { }
        template<typename C>
        EventHandler(C* object, void(C::*functionPointer)())
            : function_(new internal_event_::MemberFunctionPointer_<C>(object, functionPointer))
        {
            static_assert(std::is_base_of<BeEventHandler, C>::value,
                "The class of the object isn't able to be EventHandler.");
            object_ = object;
        }
        template<typename C>
        EventHandler(C* object)
            : function_(new internal_event_::Functor_<C>(object))
        {
            static_assert(std::is_base_of<BeEventHandler, C>::value,
                "The class of the object isn't able to be EventHandler.");
            object_ = object;
        }
        template<typename Lambda>
        EventHandler(const Lambda& lambda)
            : function_(new internal_event_::Lambda_<Lambda>(lambda)) { }

        EventHandler(const EventHandler& other)
            : function_(other.function_->Clone()), object_(other.object_) { }
        EventHandler(EventHandler&& other)
            : function_(other.function_), object_(other.object_) { other.function_ = nullptr; }
        EventHandler& operator = (const EventHandler& other);
        EventHandler& operator = (EventHandler&& other);
        ~EventHandler() { Destroy_(); }

        BeEventHandler* GetObject() const
        {
            return object_;
        };

        void operator()() const
        {
            if (function_)
                function_->Handle();
        }

    private:
        void Destroy_() { if (function_) delete function_; }

        internal_event_::EventHandler_* function_ = nullptr;
        BeEventHandler*                 object_ = nullptr;
    };


    class Event
    {
    public:
        Event() = default;
        Event(const Event&) = delete;
        Event(Event&&) = delete;
        Event& operator = (const Event&) = delete;
        Event& operator = (Event&&) = delete;

        Event& operator += (const EventHandler& handler) { AddHandler(handler); return *this; }
        Event& operator += (EventHandler&& handler) { AddHandler(std::move(handler)); return *this; }

        template<typename... Args>
        void AddHandler(Args&&... args);
        void Raise() const { handlers_.Handle(); }

    private:
        class EventHandlerMap
        {
        public:
            EventHandlerMap() = default;
            ~EventHandlerMap();

            void AddHandler(const EventHandler&);
            void AddHandler(EventHandler&&);
            void RemoveHandler(BeEventHandler* object);

            void Handle() const;
        private:
            using EventHandlerMap_ = std::multimap<BeEventHandler*, EventHandler>;
            EventHandlerMap_* map_ = nullptr;
        };

        EventHandlerMap handlers_;
    };

    template<typename ...Args>
    void Event::AddHandler(Args && ...args)
    {
        EventHandler handler(std::forward<Args>(args)...);
        handlers_.AddHandler(std::move(handler));
        auto object = handler.GetObject();
        if (object)
            object->destruction_event += [this, object] { handlers_.RemoveHandler(object); };
    }

    class BeEventHandler
    {
    public:
        ~BeEventHandler();
    public:
        Event destruction_event;
    };

    inline void SendEvent(const Event& event)
    {
        return event.Raise();
    }
}
