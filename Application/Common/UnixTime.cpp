#include "UnixTime.h"

#define  _TBIAS_DAYS    ((70 * (u32)365) + 17)
#define  _TBIAS_SECS    (_TBIAS_DAYS * (u32)86400)
#define  _TBIAS_YEAR    1900
#define  MONTAB(year)   ((((year) & 03) || ((year) == 0)) ? mos : lmos)

const s16   lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
const s16   mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define	Daysto32(year, mon)	(((year - 1) / 4) + MONTAB(year)[mon])
/**
   Преобразование времени в unix время
   на входе    :  in_rIT   - ссылка на данные времени
                  out_rUT  - ссылка на переменную куда нужно поместить значение
   на выходе   :  успешность преобразования
*/
bool IridiumTimeToUnixTime(iridium_time_t& in_rIT, u32& out_rUT)
{
   bool l_bResult = false;

   s32   days;
   u32   secs;
   s32   mon, year;

	// Calculate number of days.
	mon   = in_rIT.m_u8Month - 1;
	year  = in_rIT.m_u16Year - _TBIAS_YEAR;
	days  = Daysto32(year, mon) - 1;
	days  += 365 * year;
	days  += in_rIT.m_u8Day;
	days  -= _TBIAS_DAYS;

	// Calculate number of seconds.
	secs  = 3600 * in_rIT.m_u8Hour;
	secs  += 60 * in_rIT.m_u8Minute;
	secs  += in_rIT.m_u8Second;

	secs += (days * (u32)86400);

   out_rUT = secs;
	return l_bResult;
}

/**
   Преобразование unix время в временя
   на входе    :  in_rUT   - ссылка на данные времени
                  out_rIT  - ссылка на переменную куда нужно поместить значение
   на выходе   :  успешность преобразования
*/
bool UnixTimeToIridiumTime(u32& in_rUT, iridium_time_t& out_IT)
{
   bool l_bResult = false;

   u32   secs;
   s32   days;
   s32   mon;
   s32   year;
   s32   i;
   const s16*  pm;

   secs = in_rUT;
   days = _TBIAS_DAYS;

		/* days, hour, min, sec */
	days += secs / 86400;
   secs = secs % 86400;
	out_IT.m_u8Hour = secs / 3600;
   secs %= 3600;
	out_IT.m_u8Minute = secs / 60;
   out_IT.m_u8Second = secs % 60;

		/* determine year */
	for(year = days / 365; days < (i = Daysto32(year, 0) + 365*year); )
   {
      --year;
   }
	days -= i;
	out_IT.m_u16Year = year + _TBIAS_YEAR;

		/* determine month */
	pm = MONTAB(year);
	for (mon = 12; days < pm[--mon]; );
	out_IT.m_u8Month = mon + 1;
	out_IT.m_u8Day = days - pm[mon] + 1;

   return l_bResult;
}
