#ifndef __kernel_h__
#define __kernel_h__

//
//  NetBSD port:
//  Copyright (c) 1995, 1996, 1997-2002 by Brian Grayson (bgrayson@netBSD.org)
//
//  This file was written by Brian Grayson for the NetBSD and xosview
//    projects.
//  This file may be distributed under terms of the GPL or of the BSD
//    license, whichever you choose.  The full license notices are
//    contained in the files COPYING.GPL and COPYING.BSD, which you
//    should have received.  If not, contact one of the xosview
//    authors for a copy.
//

#include "defines.h"

void
SetKernelName(const char* kernelName);

int
MACHGetCPUSpeed();

void
MACHPageInit();

void
MACHGetPageStats(uint64_t *meminfo, uint64_t *pageinfo);

void
MACHCPUInit();

void
MACHGetCPUTimes(uint64_t *timesArray, unsigned int cpu);

int
MACHNetInit();

void
MACHGetNetInOut(uint64_t *inbytes, uint64_t *outbytes, const char *netIface, bool ignored);

int
MACHSwapInit();

void
MACHGetSwapInfo(uint64_t *total, uint64_t *used);

int
MACHDiskInit();

uint64_t
MACHGetDiskXFerBytes(uint64_t *read_bytes, uint64_t *write_bytes);

int
MACHIntrInit();

int
MACHNumInts();

void
MACHGetIntrStats(uint64_t *intrCount, unsigned int *intrNbrs);

int
MACHCountCpus(void);

unsigned int
MACHGetCPUTemperature(float *temps, float *tjmax);

void
MACHGetSensor(const char *name, const char *valname, float *value);

bool
MACHHasBattery();

void
MACHGetBatteryInfo(int *remaining, unsigned int *state);


#endif
