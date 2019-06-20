#include "pch.h"
#include "TdhHelper.h"
#include "TdhWrapper.h"

namespace winrt::FourShot::EventTracing
{
    USHORT TdhHelper::GetPropertyLength(HINSTANCE tdhLibHandle, PEVENT_RECORD pEvent, PTRACE_EVENT_INFO pEventInfo, ULONG propertyInfoIndex, PBYTE pUserData)
    {
        PROPERTY_DATA_DESCRIPTOR propertyDataDescriptor = {};
        const EVENT_PROPERTY_INFO& propertyInfo = pEventInfo->EventPropertyInfoArray[propertyInfoIndex];

        // If the property is a binary blob and is defined in a manifest, the property can 
        // specify the blob's size or it can point to another property that defines the 
        // blob's size. The PropertyParamLength flag tells you where the blob's size is defined.
        if ((propertyInfo.Flags & PropertyParamLength) == PropertyParamLength)
        {
            USHORT lengthPropertyIndex = propertyInfo.lengthPropertyIndex;

            propertyDataDescriptor.ArrayIndex = ULONG_MAX;
            propertyDataDescriptor.PropertyName = reinterpret_cast<ULONGLONG>(
                (reinterpret_cast<PBYTE>(pEventInfo) + pEventInfo->EventPropertyInfoArray[lengthPropertyIndex].NameOffset));

            ULONG propertySize = 0;
            THROW_IF_WIN32_ERROR(TdhWrapper::TdhGetPropertySize_Wrap(tdhLibHandle, pEvent, 0, nullptr, 1, &propertyDataDescriptor, &propertySize));

            DWORD propertyLength = 0;
            THROW_IF_WIN32_ERROR(TdhWrapper::TdhGetProperty_Wrap(tdhLibHandle, pEvent, 0, nullptr, 1, &propertyDataDescriptor, propertySize, reinterpret_cast<PBYTE>(&propertyLength)));

            return static_cast<USHORT>(propertyLength);
        }
        else
        {
            if (propertyInfo.length > 0)
            {
                return propertyInfo.length;
            }
            else
            {
                // If the property is a binary blob and is defined in a MOF class, the extension
                // qualifier is used to determine the size of the blob. However, if the extension 
                // is IPAddrV6, you must set the PropertyLength variable yourself because the 
                // EVENT_PROPERTY_INFO.length field will be zero.

                if (TDH_INTYPE_BINARY == propertyInfo.nonStructType.InType &&
                    TDH_OUTTYPE_IPV6 == propertyInfo.nonStructType.OutType)
                {
                    return sizeof(IN6_ADDR);
                }
                else if (TDH_INTYPE_UNICODESTRING == propertyInfo.nonStructType.InType ||
                    TDH_INTYPE_ANSISTRING == propertyInfo.nonStructType.InType ||
                    (propertyInfo.Flags & PropertyStruct) == PropertyStruct)
                {
                    return propertyInfo.length;
                }
                else if (TDH_INTYPE_SID == propertyInfo.nonStructType.InType)
                {
                    //  typedef struct _SID {
                    //      UCHAR  Revision;
                    //      UCHAR  SubAuthorityCount;
                    //      SID_IDENTIFIER_AUTHORITY  IdentifierAuthority;
                    //      ULONG  SubAuthority[ANYSIZE_ARRAY];
                    //  } SID, *PISID;

                    constexpr ULONGLONG MinSidSize = sizeof(SID) - sizeof(ULONG);
                    UCHAR subAuthCount = pUserData[1];
                    return MinSidSize + subAuthCount * sizeof(ULONG);
                }
                else
                {
                    THROW_WIN32(ERROR_EVT_INVALID_EVENT_DATA);
                }
            }
        }
    }

    USHORT TdhHelper::GetArraySize(HINSTANCE tdhLibHandle, PEVENT_RECORD pEvent, PTRACE_EVENT_INFO pInfo, ULONG propertyInfoIndex)
    {
        const EVENT_PROPERTY_INFO& propInfo = pInfo->EventPropertyInfoArray[propertyInfoIndex];

        if ((propInfo.Flags & PropertyParamCount) == PropertyParamCount)
        {
            PROPERTY_DATA_DESCRIPTOR dataDescriptor = {};
            dataDescriptor.ArrayIndex = ULONG_MAX;
            dataDescriptor.PropertyName = reinterpret_cast<ULONGLONG>(
                reinterpret_cast<PBYTE>(pInfo) + pInfo->EventPropertyInfoArray[propInfo.countPropertyIndex].NameOffset);

            DWORD propertySize = 0;
            THROW_IF_WIN32_ERROR(TdhWrapper::TdhGetPropertySize_Wrap(tdhLibHandle, pEvent, 0, nullptr, 1, &dataDescriptor, &propertySize));

            DWORD count = 0;
            THROW_IF_WIN32_ERROR(TdhWrapper::TdhGetProperty_Wrap(tdhLibHandle, pEvent, 0, nullptr, 1, &dataDescriptor, propertySize, reinterpret_cast<PBYTE>(&count)));
            return static_cast<USHORT>(count);
        }
        else
        {
            return propInfo.count;
        }
    }
}
