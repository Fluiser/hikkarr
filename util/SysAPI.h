#ifndef SYSAPI_H
#define SYSAPI_H
#include <stdint.h>

namespace SysAPI {

    typedef struct {
        uint64_t totalmem;
        uint64_t usagemem;
        uint64_t freemem;

        // int cpu_cores;
        int cpu_usage;
    } sys_info;

    void get_sys_info(sys_info& value);

}

#endif // SYSAPI_H
