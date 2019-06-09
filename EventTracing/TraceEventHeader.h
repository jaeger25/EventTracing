#pragma once
#include "TraceEventHeader.g.h"

namespace winrt::FourShot::EventTracing::implementation
{
    struct TraceEventHeader : TraceEventHeaderT<TraceEventHeader>
    {
        TraceEventHeader(PEVENT_HEADER pEventHeader, const TIME_ZONE_INFORMATION& timeZoneInfo);

        winrt::guid ActivityId() const noexcept;
        TraceEventOpCode OpCode() const noexcept;
        TraceEventLevel Level() const noexcept;
        uint32_t ProcessId() const noexcept;
        uint32_t ThreadId() const noexcept;
        Windows::Foundation::DateTime Timestamp() const noexcept;
        Windows::Foundation::DateTime TimestampUtc() const noexcept;

    private:
        guid m_activityId;
        hstring m_providerName;
        DateTime m_timestamp;
        DateTime m_timestampUtc;
        TraceEventOpCode m_opCode;
        TraceEventLevel m_level;
        uint32_t m_processId;
        uint32_t m_threadId;
    };
}
