//
//  NetBSD port:
//  Copyright (c) 1995, 1996, 1997-2002 by Brian Grayson (bgrayson@netBSD.org)
//
//  This file was written by Brian Grayson for the NetBSD and xosview
//    projects.
//  This file contains code from the NetBSD project, which is covered
//    by the standard BSD license.
//  Dummy device ignore code by : David Cuka (dcuka@intgp1.ih.att.com)
//  The OpenBSD interrupt meter code was written by Oleg Safiullin
//    (form@vs.itam.nsc.ru).
//  This file may be distributed under terms of the GPL or of the BSD
//    license, whichever you choose.  The full license notices are
//    contained in the files COPYING.GPL and COPYING.BSD, which you
//    should have received.  If not, contact one of the xosview
//    authors for a copy.
//

#include "kernel.h"

// Who knows how much of this stuff is even needed
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <nlist.h>
#include <limits.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_var.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/vmmeter.h>

// For sysctl and other MACH stuff
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/vm_map.h>
#include <mach/processor_info.h>
#include <mach/mach_host.h>
#include <mach/vm_statistics.h>


// Need to get rid of this
void MACHInit() {
    // This is never called...
}

// Need to get rid of this
void SetKernelName(const char* kernelName) {
    // wtf? whats the function
}

// Get CPU Speed from sysctl
// TODO: Sadly this is not the dynamic speed yet...
int MACHGetCPUSpeed() {
    int64_t cpuSpeed = 0;
    size_t len = sizeof(cpuSpeed);
    sysctlbyname("hw.cpufrequency", &cpuSpeed, &len, NULL, 0);
    
    // Convert from Hz to MHz
    int smallSpeed = (int)(cpuSpeed / 1000000);
    return smallSpeed;
}

// Need to get rid of this
void MACHPageInit() {

}

// /usr/include/mach/vm_statistics.h
// Numbers don't quite add up but its reasonably close for now
// http://stackoverflow.com/questions/14789672/why-does-host-statistics64-return-inconsistent-results
// "ACT/INACT/WRD/FREE"
// "IN/OUT/IDLE"
void MACHGetPageStats(uint64_t *meminfo, uint64_t *pageinfo) {
    if (meminfo != NULL) {
        vm_statistics64 vmInfo;
        natural_t memCount = HOST_VM_INFO64_COUNT;
        kern_return_t err_ret = host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vmInfo, &memCount);
        if (err_ret == KERN_SUCCESS) {
            meminfo[0] = vmInfo.active_count;
            meminfo[1] = vmInfo.inactive_count;
            meminfo[2] = vmInfo.wire_count;
            meminfo[3] = vmInfo.free_count;
            meminfo[4] = getpagesize();
        }
        else {
            err(EX_OSERR, "MACHGetPageStats() meminfo: host_statistics64() read failed.");
        }
    }
    else if (pageinfo != NULL) {
        vm_statistics64 vmInfo;
        natural_t memCount = HOST_VM_INFO64_COUNT;
        kern_return_t err_ret = host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vmInfo, &memCount);
        if (err_ret == KERN_SUCCESS) {
            pageinfo[0] = vmInfo.pageins;
            pageinfo[1] = vmInfo.pageouts;
        }
        else {
            err(EX_OSERR, "MACHGetPageStats() pageinfo: host_statistics64() read failed.");
        }
    }
    else {
        err(EX_OSERR, "MACHGetPageStats() null pointers passed for meminfo and page info.");
    }
}

// Need to get rid of this
void MACHCPUInit() {

}

// Good SO on this:
// http://stackoverflow.com/questions/6785069/get-cpu-percent-usage
// No interrupt monitoring available
// "USR/NICE/SYS/FREE"
void MACHGetCPUTimes(uint64_t *timesArray, unsigned int cpu) {
    // Some mach variables
    processor_info_array_t cpuInfo;
    mach_msg_type_number_t numCpuInfo;
    natural_t cpuCount = 0;
    
    // Call host_processor_info to get a struct with the cpu info
    kern_return_t err_ret = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &cpuCount, &cpuInfo, &numCpuInfo);
    
    if (err_ret == KERN_SUCCESS) {
        if (cpu < cpuCount) {
            timesArray[0] = cpuInfo[(CPU_STATE_MAX * cpu) + CPU_STATE_USER];
            timesArray[1] = cpuInfo[(CPU_STATE_MAX * cpu) + CPU_STATE_NICE];
            timesArray[2] = cpuInfo[(CPU_STATE_MAX * cpu) + CPU_STATE_SYSTEM];
            timesArray[3] = cpuInfo[(CPU_STATE_MAX * cpu) + CPU_STATE_IDLE];
        }
        else {
            err(EX_OSERR, "MACHGetCPUTimes() cpu parameter outside valid cpu range.");
        }
    }
    else {
        err(EX_OSERR, "MACHGetCPUTimes() host_processor_info() read failed.");
    }
    
    // Deallocate cpuInfo
    size_t cpuInfoSize = sizeof(integer_t) * numCpuInfo;
    vm_deallocate(mach_task_self(), (vm_address_t)cpuInfo, cpuInfoSize);
}

int MACHNetInit() {
    return 0;
}

void MACHGetNetInOut(uint64_t *inbytes, uint64_t *outbytes, const char *netIface, bool ignored) {

}

// Need to get rid of this
int MACHSwapInit() {
    return 0;
}

// Get the swap info from sysctl
void MACHGetSwapInfo(uint64_t *total, uint64_t *used) {
    xsw_usage swapInfo;
    size_t len = sizeof(swapInfo);
    sysctlbyname("vm.swapusage", &swapInfo, &len, NULL, 0);
    *total = swapInfo.xsu_total;
    *used = swapInfo.xsu_used;
}

// Need to get rid of this
int MACHDiskInit() {
    return 1;
}

uint64_t MACHGetDiskXFerBytes(uint64_t *read_bytes, uint64_t *write_bytes) {
    return 0;
}

int MACHIntrInit() {
    return 0;
}

int MACHNumInts() {
    return 0;
}

void MACHGetIntrStats(uint64_t *intrCount, unsigned int *intrNbrs) {

}

// Get total number of cpus from sysctl
// This should probably be done once
// unless to dynamically show parked cores
int MACHCountCpus(void) {
    int cpuCount = 0;
    size_t len = sizeof(cpuCount);
    sysctlbyname("hw.logicalcpu_max", &cpuCount, &len, NULL, 0);
    return cpuCount;
}

unsigned int MACHGetCPUTemperature(float *temps, float *tjmax) {
    return 0;
}

void MACHGetSensor(const char *name, const char *valname, float *value) {

}

bool MACHHasBattery() {
    return false;
}

void MACHGetBatteryInfo(int *remaining, unsigned int *state) {

}