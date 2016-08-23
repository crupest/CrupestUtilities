#pragma once
#include <utility>
#include <list>
#include <functional>

namespace cru
{
    template<typename >
    class Event;

    template<typename R, typename... Args>
    class Event<R(Args...)>
    {
    public:
        using EventHandler = std::function<R(Args...)>;

        Event() = default;
        Event(const Event&) = delete;
        Event(Event&&) = delete;
        Event& operator = (const Event&) = delete;
        Event& operator = (Event&&) = delete;
        ~Event() = default;

        template<typename... Args>
        void AddHandler(Args&& args) { handlers_.emplace_back(std::forward<Args>(args)...); }
        void ClearHandlers() { handlers_.clear(); }

        R operator()(Args&&... args) const;
        void operator+=(const EventHandler& handler);
        void operator+=(EventHandler&& handler);
    private:
        std::list<EventHandler> handlers_;
    };

    template<typename R, typename ...Args>
    R Event<R(Args)>::operator()(Args && ...args) const
    {
        for (auto& i : handlers_)
            i(std::forward<Args>(args)...);
    }

    template<typename R, typename ...Args>
    inline void Event<R(Args...)>::operator+=(const EventHandler & handler)
    {
        handlers_.push_back(handler);
    }

    template<typename R, typename ...Args>
    inline void Event<R(Args...)>::operator+=(EventHandler && handler)
    {
        handlers_.push_back(std::move(handler));
    }
}
