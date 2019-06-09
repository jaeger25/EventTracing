#pragma once
#include "EventProcessedEventArgs.g.h"

namespace winrt::FourShot::EventTracing::implementation
{
    struct EventProcessedEventArgs : EventProcessedEventArgsT<EventProcessedEventArgs>
    {
        EventProcessedEventArgs(TraceEvent&& traceEvent) noexcept;

        TraceEvent Event() const noexcept;

    private:
        TraceEvent m_event;
    };
}
