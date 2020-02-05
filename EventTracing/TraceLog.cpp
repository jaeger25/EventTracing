#include "pch.h"
#include "TraceLog.h"
#include "TraceLog.g.cpp"

#include "EventProcessedEventArgs.h"
#include "TraceEvent.h"
#include "TdhHelper.h"
#include "DateTimeHelper.h"
#include "TdhWrapper.h"

using namespace std::chrono_literals;

namespace winrt::FourShot::EventTracing::implementation
{
    TraceLog::TraceLog(const hstring& etlLogFilePath)
    {
        m_tdhLibHandle.reset(LoadLibrary(L"tdh.dll"));
        THROW_LAST_ERROR_IF(m_tdhLibHandle.get() == INVALID_HANDLE_VALUE);

        EVENT_TRACE_LOGFILE trace = {};
        trace.LogFileName = const_cast<wchar_t*>(etlLogFilePath.data());
        trace.EventRecordCallback = static_cast<PEVENT_RECORD_CALLBACK>(ProcessEventCallback);
        trace.ProcessTraceMode = PROCESS_TRACE_MODE_EVENT_RECORD;
        trace.Context = this;

        m_trace.reset(OpenTrace(&trace));
        THROW_LAST_ERROR_IF(m_trace.get() == INVALID_PROCESSTRACE_HANDLE);

        ReadTraceHeader(&trace.LogfileHeader);
    }

    event_token TraceLog::EventProcessed(TypedEventHandler<EventTracing::TraceLog, EventTracing::EventProcessedEventArgs> const& handler)
    {
        return m_eventProcessedEvent.add(handler);
    }

    void TraceLog::EventProcessed(event_token const& token) noexcept
    {
        m_eventProcessedEvent.remove(token);
    }

    uint32_t TraceLog::UnprocessedEventCount() const noexcept
    {
        return m_unprocessedEventCount;
    }

    DateTime TraceLog::BootTime() const noexcept
    {
        return m_bootTime;
    }

    DateTime TraceLog::BootTimeUtc() const noexcept
    {
        return m_bootTimeUtc;
    }

    DateTime TraceLog::StartTime() const noexcept
    {
        return m_startTime;
    }

    DateTime TraceLog::StartTimeUtc() const noexcept
    {
        return m_startTimeUtc;
    }

    DateTime TraceLog::EndTime() const noexcept
    {
        return m_endTime;
    }

    DateTime TraceLog::EndTimeUtc() const noexcept
    {
        return m_endTimeUtc;
    }

    IAsyncAction TraceLog::ProcessEventsAsync()
    {
        auto spThis = get_strong();
        co_await resume_background();

        // ETW events are limited to 64KB. Double the property buffer to
        // allow for room for formatted properties
        m_eventBuffer.reserve(USHRT_MAX);
        m_propertyBuffer.reserve(USHRT_MAX);
        m_formattedDataBuffer.reserve(2 * USHRT_MAX);

        THROW_IF_WIN32_ERROR(::ProcessTrace(m_trace.addressof(), 1, nullptr, nullptr));

        m_eventBuffer.shrink_to_fit();
        m_propertyBuffer.shrink_to_fit();
        m_formattedDataBuffer.shrink_to_fit();
    }

    void TraceLog::ReadTraceHeader(PTRACE_LOGFILE_HEADER pHeader) noexcept
    {
        m_pointerSize = pHeader->PointerSize;
        m_timeZoneInfo = pHeader->TimeZone;

        // Use pHeader to access all fields prior to LoggerName.
        // Adjust pHeader based on the pointer size to access
        // all fields after LogFileName. This is required only if
        // you are consuming events on an architecture that is 
        // different from architecture used to write the events.
        if (pHeader->PointerSize != sizeof(PVOID))
        {
            pHeader = reinterpret_cast<PTRACE_LOGFILE_HEADER>(
                reinterpret_cast<PUCHAR>(pHeader) +
                2 * (pHeader->PointerSize - sizeof(PVOID)));
        }

        try
        {
            m_bootTimeUtc = DateTimeHelper::ToDateTimeUtc(pHeader->BootTime);
            m_bootTime = DateTimeHelper::ToDateTimeLocal(pHeader->BootTime, m_timeZoneInfo);
        }
        CATCH_LOG();
    }

    void TraceLog::ProcessEvent(PEVENT_RECORD pEvent) noexcept
    {
        try
        {
            if (IsEqualGUID(pEvent->EventHeader.ProviderId, EventTraceGuid) &&
                pEvent->EventHeader.EventDescriptor.Opcode == EVENT_TRACE_TYPE_INFO)
            {
                return;
            }

            ULONG bufferSize = static_cast<ULONG>(m_eventBuffer.capacity());
            PTRACE_EVENT_INFO pEventInfo = reinterpret_cast<PTRACE_EVENT_INFO>(m_eventBuffer.data());
            TDHSTATUS status = TdhWrapper::TdhGetEventInformation_Wrap(m_tdhLibHandle.get(), pEvent, 0, nullptr, pEventInfo, &bufferSize);
            if (status == ERROR_NOT_FOUND)
            {
                m_unprocessedEventCount++;
                return;
            }
            THROW_IF_WIN32_ERROR(status);

            if (pEventInfo->DecodingSource != DECODING_SOURCE::DecodingSourceTlg)
            {
                m_unprocessedEventCount++;
                return;
            }

            PBYTE pUserData = static_cast<PBYTE>(pEvent->UserData);
            PBYTE pEndOfUserData = static_cast<PBYTE>(pEvent->UserData) + pEvent->UserDataLength;

            // Metadata for all the top-level properties come before
            // structure member properties in the  property information array.

            std::map<hstring, hstring> properties;
            for (ULONG i = 0; i < pEventInfo->TopLevelPropertyCount; i++)
            {
                auto propertyPairs = ProcessEventProperty(pEvent, pEventInfo, i, &pUserData, pEndOfUserData);
                for (auto&& pair : propertyPairs)
                {
                    properties.emplace(std::move(pair));
                }
            }

            auto traceEvent = winrt::make<TraceEvent>(&pEvent->EventHeader, pEventInfo, m_timeZoneInfo, std::move(properties));
            auto eventHeader = traceEvent.Header();
            if (m_startTimeUtc == DateTime{})
            {
                m_startTime = eventHeader.Timestamp();
                m_startTimeUtc = eventHeader.TimestampUtc();
            }

            m_endTime = eventHeader.Timestamp();
            m_endTimeUtc = eventHeader.TimestampUtc();

            auto args = winrt::make<EventProcessedEventArgs>(std::move(traceEvent));
            m_eventProcessedEvent(*this, args);
        }
        catch (...)
        {
            LOG_CAUGHT_EXCEPTION();

            m_unprocessedEventCount++;
        }
    }

    std::vector<std::pair<hstring, hstring>> TraceLog::ProcessEventProperty(PEVENT_RECORD pEvent, PTRACE_EVENT_INFO pEventInfo, ULONG propIndex, PBYTE* ppUserData, PBYTE pEndOfUserData)
    {
        EVENT_PROPERTY_INFO& propInfo = pEventInfo->EventPropertyInfoArray[propIndex];

        hstring propName = TEI_PROPERTY_NAME(pEventInfo, &propInfo);
        USHORT propLength = TdhHelper::GetPropertyLength(m_tdhLibHandle.get(), pEvent, pEventInfo, propIndex, *ppUserData);

        // Get the size of the array if the property is an array
        USHORT arraySize = TdhHelper::GetArraySize(m_tdhLibHandle.get(), pEvent, pEventInfo, propIndex);
        std::vector<std::pair<hstring, hstring>> propertyPairs;

        for (USHORT i = 0; i < arraySize; i++)
        {
            // If the property is a structure, enumerate the members of the structure.
            if ((propInfo.Flags & PropertyStruct) == PropertyStruct)
            {
                USHORT lastMemberIndex = propInfo.structType.StructStartIndex +
                    propInfo.structType.NumOfStructMembers;

                for (USHORT j = propInfo.structType.StructStartIndex; j < lastMemberIndex; j++)
                {
                    auto structPropertyPairs = ProcessEventProperty(pEvent, pEventInfo, j, ppUserData, pEndOfUserData);
                    for (auto&& structPair : structPropertyPairs)
                    {
                        hstring structPropName =
                            arraySize == 1 ? propName + L"." + structPair.first :
                            propName + L"[" + i + L"]" + L"." + structPair.first;

                        propertyPairs.emplace_back(propName + L"." + structPair.first, std::move(structPair.second));
                    }
                }
            }
            else
            {
                if (i == 0)
                {
                    propertyPairs.emplace_back(std::move(propName), L"");
                }
                if (propLength == 0)
                {
                    break;
                }

                PWSTR pFormattedData = reinterpret_cast<LPWSTR>(m_formattedDataBuffer.data());
                ULONG formattedDataBufferSize = static_cast<ULONG>(m_formattedDataBuffer.capacity());
                USHORT userDataConsumed;
                THROW_IF_WIN32_ERROR(TdhWrapper::TdhFormatProperty_Wrap(
                    m_tdhLibHandle.get(),
                    pEventInfo,
                    nullptr,
                    m_pointerSize,
                    propInfo.nonStructType.InType,
                    propInfo.nonStructType.OutType,
                    propLength,
                    static_cast<USHORT>(pEndOfUserData - *ppUserData),
                    *ppUserData,
                    &formattedDataBufferSize,
                    pFormattedData,
                    &userDataConsumed));

                if (*pFormattedData != L'\0')
                {
                    propertyPairs[0].second = propertyPairs[0].second + pFormattedData;
                }
                *ppUserData += userDataConsumed;

                if (i + 1 < arraySize)
                {
                    propertyPairs[0].second = propertyPairs[0].second + L", ";
                }
            }
        }

        return propertyPairs;
    }

    /*static*/ IAsyncOperation<EventTracing::TraceLog> TraceLog::OpenAsync(hstring etlLogFilePath)
    {
        co_await resume_background();

        co_return winrt::make<TraceLog>(etlLogFilePath);
    }

    /*static*/ void TraceLog::ProcessEventCallback(PEVENT_RECORD pEvent)
    {
        TraceLog* pEventTrace = reinterpret_cast<TraceLog*>(pEvent->UserContext);
        pEventTrace->ProcessEvent(pEvent);
    }
}
