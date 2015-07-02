#ifndef _DEFINES_H
#define _DEFINES_H

#define CALL_FAILED(i) ((i) < 0)
#define align_mem(addr, align) (((addr) + ((align) - 1)) & ~((align) - 1))
#define lerp(value, from_max, to_max) ((((value*10) * (to_max*10))/(from_max*10))/10)

#if defined(_WIN32)
#define ALIGN(i)

#elif defined(PLATFORM_PSP2)
#include <psp2/moduleinfo.h>
#include <psp2/kernel/memorymgr.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/display.h>
#include <psp2/gxm.h>
#include <sys/time.h>

#define ALIGN(i) __attribute((aligned(i)))
#ifndef abs
#define abs(x) ((x) < 0 ? (-(x)) : (x))
#endif

#endif

#endif
