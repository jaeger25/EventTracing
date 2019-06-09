#include "pch.h"
#include "TraceEvent.h"
#include "TraceEvent.g.cpp"

namespace winrt::FourShot::EventTracing::implementation
{
    TraceEvent::TraceEvent(PEVENT_HEADER pEventHeader, PTRACE_EVENT_INFO pEventInfo, const TIME_ZONE_INFORMATION& timeZoneInfo, std::map<hstring, hstring>&& properties)
    {
        m_name = TEI_TASK_NAME(pEventInfo);
        m_providerName = TEI_PROVIDER_NAME(pEventInfo);
        m_properties = single_threaded_map(std::move(properties));
        m_eventHeader = make<TraceEventHeader>(pEventHeader, timeZoneInfo);
    }

    hstring TraceEvent::Name() const noexcept
    {
        return m_name;
    }

    hstring TraceEvent::ProviderName() const noexcept
    {
        return m_providerName;
    }

    EventTracing::TraceEventHeader TraceEvent::Header() const noexcept
    {
        return m_eventHeader;
    }

    IMapView<hstring, hstring> TraceEvent::Properties() const noexcept
    {
        return m_properties.GetView();
    }
}
