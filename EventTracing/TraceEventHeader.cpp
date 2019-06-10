#include "pch.h"
#include "TraceEventHeader.h"
#include "TraceEventHeader.g.cpp"

#include "DateTimeHelper.h"

namespace winrt::FourShot::EventTracing::implementation
{
    TraceEventHeader::TraceEventHeader(PEVENT_HEADER pEventHeader, const TIME_ZONE_INFORMATION& timeZoneInfo)
    {
        m_id = pEventHeader->EventDescriptor.Id;
        m_activityId = pEventHeader->ActivityId;
        m_processId = pEventHeader->ProcessId;
        m_threadId = pEventHeader->ThreadId;
        m_opCode = static_cast<TraceEventOpCode>(pEventHeader->EventDescriptor.Opcode);
        m_level = static_cast<TraceEventLevel>(pEventHeader->EventDescriptor.Level);
        m_timestampUtc = DateTimeHelper::ToDateTimeUtc(pEventHeader->TimeStamp);
        m_timestamp = DateTimeHelper::ToDateTimeLocal(pEventHeader->TimeStamp, timeZoneInfo);
    }

    uint16_t TraceEventHeader::Id() const noexcept
    {
        return m_id;
    }

    guid TraceEventHeader::ActivityId() const noexcept
    {
        return m_activityId;
    }

    TraceEventOpCode TraceEventHeader::OpCode() const noexcept
    {
        return m_opCode;
    }

    TraceEventLevel TraceEventHeader::Level() const noexcept
    {
        return m_level;
    }

    uint32_t TraceEventHeader::ProcessId() const noexcept
    {
        return m_processId;
    }

    uint32_t TraceEventHeader::ThreadId() const noexcept
    {
        return m_threadId;
    }

    DateTime TraceEventHeader::Timestamp() const noexcept
    {
        return m_timestamp;
    }

    DateTime TraceEventHeader::TimestampUtc() const noexcept
    {
        return m_timestampUtc;
    }
}
