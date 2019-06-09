#include "pch.h"
#include "EventProcessedEventArgs.h"
#include "EventProcessedEventArgs.g.cpp"

namespace winrt::FourShot::EventTracing::implementation
{
    EventProcessedEventArgs::EventProcessedEventArgs(TraceEvent&& event) noexcept :
        m_event(event)
    {
    }

    TraceEvent EventProcessedEventArgs::Event() const noexcept
    {
        return m_event;
    }
}
