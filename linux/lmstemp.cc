//
//  Copyright (c) 2000 by Leopold Toetsch <lt@toetsch.at>
//
//  Read temperature entries from /proc/sys/dev/sensors/*/*
//  and display actual and high temperature
//  if actual >= high, actual temp changes color to indicate alarm
//
//  File based on btrymeter.* by
//  Copyright (c) 1997 by Mike Romberg ( romberg@fsl.noaa.gov )
//
//  This file may be distributed under terms of the GPL
//
//
//
#include "lmstemp.h"
#include "xosview.h"
#include <fstream.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

static const char PROC_SENSORS[] = "/proc/sys/dev/sensors";

LmsTemp::LmsTemp( XOSView *parent, const char *filename, const char *label,
		 const char *caption)
  : FieldMeter( parent, 3, label, caption, 1, 1, 0 ){
    if(!checksensors(filename)) {
	cerr <<"Can not find file : " <<PROC_SENSORS <<"/*/" << filename <<endl;
    	parent_->done(1);
  }
    char *p;
    if ((p = strrchr(caption,'/')) != 0)
      _highest = atoi(p+1);
    else
      _highest = 100;
}

LmsTemp::~LmsTemp( void ){
}

/* this part is adapted from ProcMeter3.2 */
int  LmsTemp::checksensors(const char* filename)
{
  bool found = false;
  DIR *d1, *d2;
  struct dirent *ent1, *ent2;
  struct stat buf;

  d1=opendir(PROC_SENSORS);
  if(!d1)
    return false;
  else
    {
      char dirname[64];

      while(!found && (ent1=readdir(d1)))
	{
	  if(!strncmp(ent1->d_name,".", 1))
	    continue;
	  if(!strncmp(ent1->d_name,"..", 2))
	    continue;

	  snprintf(dirname, 64, "%s/%s",PROC_SENSORS ,ent1->d_name);
	  if(stat(dirname,&buf)==0 && S_ISDIR(buf.st_mode))
	    {
	      d2=opendir(dirname);
	      if(!d2)
		cerr << "The directory " <<dirname <<"exists but cannot be read.\n";
	      else
		{
		  while((ent2=readdir(d2)))
		    {

		      char f[80];
		      snprintf(f, 80, "%s/%s",dirname,ent2->d_name);
		      if(stat(f,&buf)!=0 || !S_ISREG(buf.st_mode))
			continue;

		      if(!strncmp(ent2->d_name, filename, strlen(filename)))
			{
			  strcpy(_filename, f);
			  found = true;
			  break;
			}
		    }
		  closedir(d2);
		}
	    }

	}
    }
  closedir(d1);
  return found;
}

void LmsTemp::checkResources( void ){
  FieldMeter::checkResources();

  setfieldcolor( 0, parent_->getResource( "lmstempActColor" ) );
  setfieldcolor( 1, parent_->getResource( "lmstempIdleColor") );
  setfieldcolor( 2, parent_->getResource( "lmstempHighColor" ) );

  priority_ = atoi (parent_->getResource( "lmstempPriority" ) );
  SetUsedFormat(parent_->getResource( "lmstempUsedFormat" ) );
}

void LmsTemp::checkevent( void ){
  getlmstemp();

  drawfields();
}

// Note:
// procentry looks like
// high low actual
//
// if actual >= high alarm is triggered, fan starts and high is set to
//   a higher value by BIOS
//   after fan cooled down the chips, high get's reset
// if  this happens display color of actual is set to HighColor
// this could be very machine depended
//
// a typical entry on my machine (Gericom Overdose 2 XXL, PIII 600) looks like:
//
// $ sensors
// max1617-i2c-0-4e
// Adapter: SMBus PIIX4 adapter at 1400
// Algorithm: Non-I2C SMBus adapter
// temp:       52 C (limit:   55 C, hysteresis:  -55 C)
// remote_temp:
//             56 C (limit:   90 C, hysteresis:  -55 C)
//
// after alarm limits are set to 60 / 127 respectively
// low/hysteresis looks broken ;-)
//

void LmsTemp::getlmstemp( void ){
  ifstream file( _filename );

  if ( !file ){
    cerr <<"Can not open file : " <<file <<endl;
    parent_->done(1);
    return;
  }

  int dummy, high;
  total_ = _highest;  // Max temp
  file >> high >> dummy >> fields_[0];
  fields_[1] = high - fields_[0];
  if(fields_[1] <= 0) {	// alarm
    fields_[1] = 0;
    setfieldcolor( 0, parent_->getResource( "lmstempHighColor" ) );
  }
  else
    setfieldcolor( 0, parent_->getResource( "lmstempActColor" ) );

  fields_[2] = total_ - fields_[1] - fields_[0];
  if(fields_[2] <= 0) {	// alarm, high was set above 100
    fields_[2] = 0;
    setfieldcolor( 0, parent_->getResource( "lmstempHighColor" ) );
  }
  else
    setfieldcolor( 0, parent_->getResource( "lmstempActColor" ) );
  setUsed (fields_[0], total_);
}