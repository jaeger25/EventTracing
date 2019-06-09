#pragma once

#include "TraceEvent.g.h"
#include "TraceEventHeader.h"

namespace winrt::FourShot::EventTracing::implementation
{
    struct TraceEvent : TraceEventT<TraceEvent>
    {
        TraceEvent(PEVENT_HEADER pEventHeader, PTRACE_EVENT_INFO pEventInfo, const TIME_ZONE_INFORMATION& timeZoneInfo, std::map<hstring, hstring>&& properties);

        hstring Name() const noexcept;
        hstring ProviderName() const noexcept;
        IMapView<hstring, hstring> Properties() const noexcept;
        EventTracing::TraceEventHeader Header() const noexcept;

    private:
        hstring m_name;
        hstring m_providerName;
        IMap<hstring, hstring> m_properties;
        EventTracing::TraceEventHeader m_eventHeader{ nullptr };
    };
}
