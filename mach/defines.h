#ifndef __defines_h__
#define __defines_h__

#include <sys/param.h>
#include <cstdint>   /* for uint64_t */

/* The MACH variant. */
#if defined ( __APPLE__ ) && defined ( __MACH__ )
#define XOSVIEW_MACH
#endif

#if !( defined(XOSVIEW_MACH) )
#error "Unsupported MACH variant."
#endif

/* Helper defines for battery meter. */
#define XOSVIEW_BATT_NONE         0
#define XOSVIEW_BATT_CHARGING     1
#define XOSVIEW_BATT_DISCHARGING  2
#define XOSVIEW_BATT_FULL         4
#define XOSVIEW_BATT_LOW          8
#define XOSVIEW_BATT_CRITICAL    16


#endif
