#include "common/affinity.h"

#ifdef _WIN32

#include <windows.h>

bool pin_to_core(int core) {
    if (core < 0 || core >= 64) {
        return false;
    }
    const DWORD_PTR mask = static_cast<DWORD_PTR>(1) << core;
    return SetThreadAffinityMask(GetCurrentThread(), mask) != 0;
}

#else

#include <sched.h>

bool pin_to_core(int core) {
    if (core < 0 || core >= CPU_SETSIZE) {
        return false;
    }
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(core, &set);
    return sched_setaffinity(0, sizeof(set), &set) == 0;
}

#endif
