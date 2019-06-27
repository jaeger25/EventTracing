#include "pch.h"
#include "DateTimeHelper.h"

namespace winrt::FourShot::EventTracing
{
    DateTime DateTimeHelper::ToDateTimeUtc(const LARGE_INTEGER& dateTimeInt) noexcept
    {
        FILETIME fileTime;
        fileTime.dwHighDateTime = dateTimeInt.HighPart;
        fileTime.dwLowDateTime = dateTimeInt.LowPart;

        return clock::from_file_time(fileTime);
    }

    DateTime DateTimeHelper::ToDateTimeLocal(const LARGE_INTEGER& dateTimeInt, const TIME_ZONE_INFORMATION& timeZoneInfo) noexcept
    {
        FILETIME fileTime;
        fileTime.dwHighDateTime = dateTimeInt.HighPart;
        fileTime.dwLowDateTime = dateTimeInt.LowPart;

        SYSTEMTIME sysTime;
        THROW_LAST_ERROR_IF(!FileTimeToSystemTime(&fileTime, &sysTime));

        SYSTEMTIME sysTimeLocal;
        THROW_LAST_ERROR_IF(!SystemTimeToTzSpecificLocalTime(&timeZoneInfo, &sysTime, &sysTimeLocal));

        FILETIME fileTimeLocal;
        THROW_LAST_ERROR_IF(!SystemTimeToFileTime(&sysTimeLocal, &fileTimeLocal));

        return clock::from_file_time(fileTimeLocal);
    }
}
