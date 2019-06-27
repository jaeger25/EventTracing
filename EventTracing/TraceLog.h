#pragma once

#include "TraceLog.g.h"

namespace winrt::FourShot::EventTracing::implementation
{
    struct TraceLog : TraceLogT<TraceLog>
    {
        static IAsyncOperation<EventTracing::TraceLog> OpenAsync(hstring etlLogFilePath);

        TraceLog(const hstring& etlLogFilePath);

        event_token EventProcessed(TypedEventHandler<EventTracing::TraceLog, EventTracing::EventProcessedEventArgs> const& handler);
        void EventProcessed(event_token const& token) noexcept;

        uint32_t UnprocessedEventCount() const noexcept;
        DateTime BootTime() const noexcept;
        DateTime BootTimeUtc() const noexcept;
        DateTime StartTime() const noexcept;
        DateTime StartTimeUtc() const noexcept;
        DateTime EndTime() const noexcept;
        DateTime EndTimeUtc() const noexcept;

        IAsyncAction ProcessEventsAsync();

    private:
        static void __stdcall ProcessEventCallback(PEVENT_RECORD pEvent);

        void ReadTraceHeader(PTRACE_LOGFILE_HEADER pHeader) noexcept;
        void ProcessEvent(PEVENT_RECORD pEvent) noexcept;
        std::vector<std::pair<hstring, hstring>> ProcessEventProperty(PEVENT_RECORD pEvent, PTRACE_EVENT_INFO pEventInfo, ULONG propIndex, PBYTE* ppUserData, PBYTE pEndOfUserData);

        event<TypedEventHandler<EventTracing::TraceLog, EventTracing::EventProcessedEventArgs>> m_eventProcessedEvent;

        wil::unique_tracehandle m_trace;
        wil::unique_hmodule m_tdhLibHandle;
        DateTime m_bootTime;
        DateTime m_bootTimeUtc;
        DateTime m_startTime;
        DateTime m_startTimeUtc;
        DateTime m_endTime;
        DateTime m_endTimeUtc;
        ULONG m_pointerSize = 0;
        uint32_t m_unprocessedEventCount = 0;
        TIME_ZONE_INFORMATION m_timeZoneInfo = {};
        std::vector<char> m_eventBuffer;
        std::vector<char> m_propertyBuffer;
        std::vector<char> m_formattedDataBuffer;
    };
}

namespace winrt::FourShot::EventTracing::factory_implementation
{
    struct TraceLog : TraceLogT<TraceLog, implementation::TraceLog>
    {
    };
}
