#pragma once

namespace winrt::FourShot::EventTracing
{
    struct DateTimeHelper
    {
        static DateTime ToDateTimeUtc(const LARGE_INTEGER& dateTimeInt) noexcept;
        static DateTime ToDateTimeLocal(const LARGE_INTEGER& dateTimeInt, const TIME_ZONE_INFORMATION& timeZoneInfo) noexcept;
    };
}
