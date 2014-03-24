//
//  Initial port performed by Greg Onufer (exodus@cheers.bungi.com)
//
#include "cpumeter.h"
#include "xosview.h"
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>


CPUMeter::CPUMeter(XOSView *parent, kstat_ctl_t *_kc, int cpuid)
	: FieldMeterGraph(parent, CPU_STATES, cpuStr(cpuid), "USER/SYS/WAIT/IDLE")
{
	kc = _kc;
	aggregate = ( cpuid == 0 );
	for (int i = 0 ; i < 2 ; i++)
		for (int j = 0 ; j < CPU_STATES ; j++)
			cputime_[i][j] = 0;
	cpuindex_ = 0;

	if (!aggregate) {
		int j = 0;
		for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next) {
			if (strncmp(ksp->ks_name, "cpu_stat", 8) == 0) {
				j++;
				if (j == cpuid)
					break;
			}
		}
	}
}

CPUMeter::~CPUMeter(void)
{
}

void CPUMeter::checkResources(void)
{
	FieldMeterGraph::checkResources();

	setfieldcolor(0, parent_->getResource("cpuUserColor"));
	setfieldcolor(1, parent_->getResource("cpuSystemColor"));
	setfieldcolor(2, parent_->getResource("cpuInterruptColor"));
	setfieldcolor(3, parent_->getResource("cpuFreeColor"));
	priority_ = atoi(parent_->getResource("cpuPriority"));
	dodecay_ = parent_->isResourceTrue("cpuDecay");
	useGraph_ = parent_->isResourceTrue("cpuGraph");
	SetUsedFormat(parent_->getResource("cpuUsedFormat"));
}

void CPUMeter::checkevent(void)
{
	getcputime();
	drawfields();
}

void CPUMeter::getcputime(void)
{
	total_ = 0;
	cpu_stat_t cs;

	if (aggregate) {
		bzero(cputime_[cpuindex_], CPU_STATES * sizeof(cputime_[cpuindex_][0]));
		for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next) {
			if (strncmp(ksp->ks_name, "cpu_stat", 8))
				continue;
			if (kstat_read(kc, ksp, &cs) == -1) {
				parent_->done(1);
				return;
			}
			cputime_[cpuindex_][0] += cs.cpu_sysinfo.cpu[CPU_USER];
			cputime_[cpuindex_][1] += cs.cpu_sysinfo.cpu[CPU_KERNEL];
			cputime_[cpuindex_][2] += cs.cpu_sysinfo.cpu[CPU_WAIT];
			cputime_[cpuindex_][3] += cs.cpu_sysinfo.cpu[CPU_IDLE];
		}
	}
	else {
		if (kstat_read(kc, ksp, &cs) == -1) {
			parent_->done(1);
			return;
		}
		cputime_[cpuindex_][0] = cs.cpu_sysinfo.cpu[CPU_USER];
		cputime_[cpuindex_][1] = cs.cpu_sysinfo.cpu[CPU_KERNEL];
		cputime_[cpuindex_][2] = cs.cpu_sysinfo.cpu[CPU_WAIT];
		cputime_[cpuindex_][3] = cs.cpu_sysinfo.cpu[CPU_IDLE];
	}

	int oldindex = (cpuindex_ + 1) % 2;
	for (int i = 0 ; i < CPU_STATES ; i++) {
		fields_[i] = cputime_[cpuindex_][i] - cputime_[oldindex][i];
		total_ += fields_[i];
	}
	cpuindex_ = (cpuindex_ + 1) % 2;

	if (total_)
		setUsed(total_ - fields_[3], total_);
}

const char *CPUMeter::cpuStr(int num)
{
	static char buffer[8] = "CPU";
	if (num > 0)
		snprintf(buffer + 3, 4, "%d", num - 1);
	buffer[7] = '\0';
	return buffer;
}
