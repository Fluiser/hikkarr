#include "SysAPI.h"

#include <util/util.hpp>
#ifdef WIN32
#include <Windows.h>
#include <Psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <fstream>
#include <string>
#endif

namespace SysAPI {

#ifdef WIN32
    void get_sys_info(sys_info& value)
    {
        PROCESS_MEMORY_COUNTERS pc;
        MEMORYSTATUSEX memstat;

        ZeroMem(pc);
        ZeroMem(memstat);

        memstat.dwLength = sizeof(memstat);
        pc.cb = sizeof(pc);

        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pc, sizeof(pc));
        GlobalMemoryStatusEx(&memstat);

        value.totalmem = memstat.ullTotalPhys;
        value.freemem = memstat.ullAvailPhys;
        value.usagemem = pc.WorkingSetSize;

        {
            FILETIME creationTime, exitTime, kernelTime, userTime;
            double percentProcessorTime = 0.0;

            if (GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime))
            {
                FILETIME currentTime, elapsedTime;
                GetSystemTimeAsFileTime(&currentTime) ;
                if (CompareFileTime(&creationTime, &currentTime) &&  CompareFileTime(&currentTime, &exitTime))
                {
                    elapsedTime.dwLowDateTime = currentTime.dwLowDateTime - creationTime.dwLowDateTime;
                    elapsedTime.dwHighDateTime = currentTime.dwHighDateTime - creationTime.dwHighDateTime;

                    ULONGLONG elapsedTimeInMs = (elapsedTime.dwLowDateTime + elapsedTime.dwHighDateTime * 4294967296lu) / 10000;

                    ULONGLONG kernelTimeMs = (kernelTime.dwLowDateTime + kernelTime.dwHighDateTime * 4294967296lu) / 10000;
                    ULONGLONG userTimeMs = (userTime.dwLowDateTime + userTime.dwHighDateTime * 4294967296lu) / 10000;

                    value.cpu_usage = (kernelTimeMs + userTimeMs) * 100.0f / elapsedTimeInMs;
                    return;
                }
            }
        }
        value.cpu_usage = -100;
    }
#else
    void get_sys_info(sys_info& value)
    {
        struct rusage usage;
        struct sysinfo sys_info;

        getrusage(RUSAGE_SELF, &usage);
        sysinfo(&sys_info);

        value.usagemem = (usage.ru_maxrss*1024);
        value.freemem = (sys_info.freeram);
        value.totalmem = (sys_info.totalram);


        value.cpu_usage = (float(usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) +
                           float(usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) * 1e-6f);

    }
#endif
}
