#pragma once

namespace winrt::FourShot::EventTracing
{
    struct TdhHelper
    {
        // Get the length of the property data. For MOF-based events, the size is inferred from the data type
        // of the property. For manifest-based events, the property can specify the size of the property value
        // using the length attribute. The length attribue can specify the size directly or specify the name 
        // of another property in the event data that contains the size. If the property does not include the 
        // length attribute, the size is inferred from the data type. The length will be zero for variable
        // length, null-terminated strings and structures.
        static USHORT GetPropertyLength(HINSTANCE tdhLibHandle, PEVENT_RECORD pEvent, PTRACE_EVENT_INFO pEventInfo, ULONG propertyInfoIndex, PBYTE pUserData);

        // Get the size of the array. For MOF-based events, the size is specified in the declaration or using 
        // the MAX qualifier. For manifest-based events, the property can specify the size of the array
        // using the count attribute. The count attribue can specify the size directly or specify the name 
        // of another property in the event data that contains the size.
        static USHORT GetArraySize(HINSTANCE tdhLibHandle, PEVENT_RECORD pEvent, PTRACE_EVENT_INFO pInfo, ULONG propertyInfoIndex);
    };
}
