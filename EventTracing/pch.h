#pragma once

#define NOMINMAX

#include <map>

#include <initguid.h>

#include <wil\cppwinrt.h>

#include <unknwn.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <tdh.h>
#include <in6addr.h>

#include <wil\result_macros.h>
#include <wil\result_originate.h>
#include <wil\resource.h>

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
}

namespace wil
{
    typedef unique_any<TRACEHANDLE, decltype(&::CloseTrace), ::CloseTrace> unique_tracehandle;
}
