#ifndef _C_UNIX_TIME_H_INCLUDED_
#define _C_UNIX_TIME_H_INCLUDED_

#include "IridiumTime.h"

// Преобразования времени
bool IridiumTimeToUnixTime(iridium_time_t& in_rIT, u32& out_rUT);
bool UnixTimeToIridiumTime(u32& in_rUT, iridium_time_t& out_IT);

#endif   // _C_UNIX_TIME_H_INCLUDED_
