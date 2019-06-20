#include "pch.h"
#include "TdhWrapper.h"

namespace winrt::FourShot::EventTracing
{
    TDHSTATUS
    TdhWrapper::TdhFormatProperty_Wrap(
        _In_ HINSTANCE tdhLibHandle,
        _In_ PTRACE_EVENT_INFO EventInfo,
        _In_opt_ PEVENT_MAP_INFO MapInfo,
        _In_ ULONG PointerSize,
        _In_ USHORT PropertyInType,
        _In_ USHORT PropertyOutType,
        _In_ USHORT PropertyLength,
        _In_ USHORT UserDataLength,
        _In_reads_bytes_(UserDataLength) PBYTE UserData,
        _Inout_ PULONG BufferSize,
        _Out_writes_bytes_opt_(*BufferSize) PWCHAR Buffer,
        _Out_ PUSHORT UserDataConsumed)
    {
        auto proc = (decltype(TdhFormatProperty)*)GetProcAddress(tdhLibHandle, "TdhFormatProperty");
        THROW_LAST_ERROR_IF_NULL(proc);

        return proc(EventInfo, MapInfo, PointerSize, PropertyInType, PropertyOutType, PropertyLength, UserDataLength, UserData, BufferSize, Buffer, UserDataConsumed);
    }

    TDHSTATUS
    TdhWrapper::TdhGetPropertySize_Wrap(
        _In_ HINSTANCE tdhLibHandle,
        _In_ PEVENT_RECORD pEvent,
        _In_ ULONG TdhContextCount,
        _In_reads_opt_(TdhContextCount) PTDH_CONTEXT pTdhContext,
        _In_ ULONG PropertyDataCount,
        _In_reads_(PropertyDataCount) PPROPERTY_DATA_DESCRIPTOR pPropertyData,
        _Out_ ULONG* pPropertySize)
    {
        auto proc = (decltype(TdhGetPropertySize)*)GetProcAddress(tdhLibHandle, "TdhGetPropertySize");
        THROW_LAST_ERROR_IF_NULL(proc);

        return proc(pEvent, TdhContextCount, pTdhContext, PropertyDataCount, pPropertyData, pPropertySize);
    }

    TDHSTATUS
    TdhWrapper::TdhGetProperty_Wrap(
        _In_ HINSTANCE tdhLibHandle,
        _In_ PEVENT_RECORD pEvent,
        _In_ ULONG TdhContextCount,
        _In_reads_opt_(TdhContextCount) PTDH_CONTEXT pTdhContext,
        _In_ ULONG PropertyDataCount,
        _In_reads_(PropertyDataCount) PPROPERTY_DATA_DESCRIPTOR pPropertyData,
        _In_ ULONG BufferSize,
        _Out_writes_bytes_(BufferSize) PBYTE pBuffer)
    {
        auto proc = (decltype(TdhGetProperty)*)GetProcAddress(tdhLibHandle, "TdhGetProperty");
        THROW_LAST_ERROR_IF_NULL(proc);

        return proc(pEvent, TdhContextCount, pTdhContext, PropertyDataCount, pPropertyData, BufferSize, pBuffer);
    }

    TDHSTATUS
    TdhWrapper::TdhGetEventInformation_Wrap(
        _In_ HINSTANCE tdhLibHandle,
        _In_ PEVENT_RECORD Event,
        _In_ ULONG TdhContextCount,
        _In_reads_opt_(TdhContextCount) PTDH_CONTEXT TdhContext,
        _Out_writes_bytes_opt_(*BufferSize) PTRACE_EVENT_INFO Buffer,
        _Inout_ PULONG BufferSize)
    {
        auto proc = (decltype(TdhGetEventInformation)*)GetProcAddress(tdhLibHandle, "TdhGetEventInformation");
        THROW_LAST_ERROR_IF_NULL(proc);

        return proc(Event, TdhContextCount, TdhContext, Buffer, BufferSize);
    }
}
