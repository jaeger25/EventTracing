#include "pch.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace FourShot::EventTracing;

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 2)
    {
        std::wcerr << L"Usage \"EventTracing.Console.exe C:\\Path\\To\\Trace.etl\"" << std::endl;
        return -1;
    }

    init_apartment();

    TraceLog trace = TraceLog::OpenAsync(argv[1]).get();
    trace.EventProcessed(([](const TraceLog& /*trace*/, const EventProcessedEventArgs& args)
        {
            std::wcout << args.Event().ProviderName().c_str() << L" : " << args.Event().Name().c_str() << std::endl;
            for (auto&& property : args.Event().Properties())
            {
                std::wcout << L"\t" << property.Key().c_str() << L" : " << property.Value().c_str() << std::endl;
            }
        }));

    trace.ProcessEventsAsync().get();
}
