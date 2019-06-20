#pragma once

namespace winrt::FourShot::EventTracing
{
    struct TdhWrapper
    {
        static
        TDHSTATUS
        TdhFormatProperty_Wrap(
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
            _Out_ PUSHORT UserDataConsumed);

        static
        TDHSTATUS
        TdhGetPropertySize_Wrap(
            _In_ HINSTANCE tdhLibHandle,
            _In_ PEVENT_RECORD pEvent,
            _In_ ULONG TdhContextCount,
            _In_reads_opt_(TdhContextCount) PTDH_CONTEXT pTdhContext,
            _In_ ULONG PropertyDataCount,
            _In_reads_(PropertyDataCount) PPROPERTY_DATA_DESCRIPTOR pPropertyData,
            _Out_ ULONG* pPropertySize);

        static
        TDHSTATUS
        TdhGetProperty_Wrap(
            _In_ HINSTANCE tdhLibHandle,
            _In_ PEVENT_RECORD pEvent,
            _In_ ULONG TdhContextCount,
            _In_reads_opt_(TdhContextCount) PTDH_CONTEXT pTdhContext,
            _In_ ULONG PropertyDataCount,
            _In_reads_(PropertyDataCount) PPROPERTY_DATA_DESCRIPTOR pPropertyData,
            _In_ ULONG BufferSize,
            _Out_writes_bytes_(BufferSize) PBYTE pBuffer);

        static
        TDHSTATUS
        TdhGetEventInformation_Wrap(
            _In_ HINSTANCE tdhLibHandle,
            _In_ PEVENT_RECORD Event,
            _In_ ULONG TdhContextCount,
            _In_reads_opt_(TdhContextCount) PTDH_CONTEXT TdhContext,
            _Out_writes_bytes_opt_(*BufferSize) PTRACE_EVENT_INFO Buffer,
            _Inout_ PULONG BufferSize);
    };
}
