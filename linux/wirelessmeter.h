//
//  Copyright (c) 1997 by Mike Romberg ( romberg@fsl.noaa.gov )
//
//  This file may be distributed under terms of the GPL
//
//
// $Id: btrymeter.h,v 1.1 1997/02/26 23:44:50 mromberg Exp $
//
#ifndef _WIRELESSMETER_H_
#define _WIRELESSMETER_H_

#include "fieldmetergraph.h"
#include <string>

static const char WLFILENAME[] = "/proc/net/wireless";


class WirelessMeter : public FieldMeterGraph {
public:
  WirelessMeter( XOSView *parent, int ID = 0, const char *wlID = "WL");
  ~WirelessMeter( void );

  const char *name( void ) const { return "WirelessMeter"; }
  void checkevent( void );

  void checkResources( void );
  static int countdevices(void);
  static const char *wirelessStr(int num);

protected:
  void getpwrinfo( void );

private:
  unsigned long _poorqualcol, _fairqualcol, _goodqualcol;
  int _lastquality, _number;
  std::string _devname;

};


#endif
