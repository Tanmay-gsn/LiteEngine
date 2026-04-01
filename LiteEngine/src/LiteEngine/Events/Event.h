#pragma once
#include "hzpch.h"
#include "LiteEngine/Core/Core.h"

// Make sure spdlog can use operator<< for custom types
#include "spdlog/fmt/ostr.h"

namespace LiteEngine {

    // -------------------------------
    // Event Type Enumeration
    // -------------------------------
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    // -------------------------------
    // Event Category Flags
    // -------------------------------
    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = 1 << 0,
        EventCategoryInput = 1 << 1,
        EventCategoryKeyboard = 1 << 2,
        EventCategoryMouse = 1 << 3,
        EventCategoryMouseButton = 1 << 4
    };

    // -------------------------------
    // Macros for Event Classes
    // -------------------------------
#define EVENT_CLASS_TYPE(type) \
        static EventType GetStaticType() { return EventType::type; } \
        virtual EventType GetEventType() const override { return GetStaticType(); } \
        virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
        virtual int GetCategoryFlags() const override { return category; }

    // -------------------------------
    // Base Event Class
    // -------------------------------
    class LITEENGINE_API Event
    {
        friend class EventDispatcher;
    public:
        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        inline bool IsInCategory(EventCategory category) const
        {
            return GetCategoryFlags() & category;
        }

        bool IsHandled() const { return Handled; }
        void SetHandled(bool value) { Handled = value; }

    protected:
        bool Handled = false;
    };

    // -------------------------------
    // Event Dispatcher
    // -------------------------------
    class EventDispatcher
    {
        template<typename T>
        using EventFn = std::function<bool(T&)>;

    public:
        EventDispatcher(Event& event)
            : m_Event(event) {
        }

        template<typename T>
        bool Dispatch(EventFn<T> func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled = func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };

    // -------------------------------
    // Stream Operator for Logging
    // -------------------------------
    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }

} // namespace LiteEngine
