#ifndef _C_DEVICE_INDICATOR_H_INCLUDED_
#define _C_DEVICE_INDICATOR_H_INCLUDED_

#include "CIndicator.h"

class CDeviceIndicator : public CIndicator
{
public:
   // Перегруженные методы
   virtual u32 GetTime();
   virtual void Set(u8 in_u8Color, bool in_bState);
};

#endif   // _C_DEVICE_INDICATOR_H_INCLUDED_
