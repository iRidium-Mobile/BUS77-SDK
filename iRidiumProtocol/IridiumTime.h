/*******************************************************************************
 * Copyright (c) 2013-2019 iRidi Ltd. www.iridi.com
 *
 * Все права зарегистрированы. Эта программа и сопровождающие материалы доступны
 * на условиях Eclipse Public License v2.0 и Eclipse Distribution License v1.0,
 * которая сопровождает это распространение. 
 *
 * Текст Eclipse Public License доступен по ссылке
 *    http://www.eclipse.org/legal/epl-v20.html
 * Текст Eclipse Distribution License доступн по ссылке
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Участники:
 *    Марат Гилязетдинов, Сергей Королёв  - первая версия
 *******************************************************************************/
#ifndef _IRIDIUM_TIME_H_INCLUDED_
#define _IRIDIUM_TIME_H_INCLUDED_

#include "IridiumTypes.h"

#define IRIDIUM_TIME_FLAG_DATE   1
#define IRIDIUM_TIME_FLAG_TIME   2

// Cтруктура для описания текущего времени
typedef struct iridium_time_s
{
   u8    m_u8Flags;                                // Флаги
   u16   m_u16Year;                                // Год
   u8    m_u8Month;                                // Месяц
   u8    m_u8DayOfWeek;                            // День недели
   u8    m_u8Day;                                  // День
   u8    m_u8Hour;                                 // Час
   u8    m_u8Minute;                               // Минуты
   u8    m_u8Second;                               // Секунды
   u16   m_u16Milliseconds;                        // Миллисекунды
} iridium_time_t;
#endif // _IRIDIUM_TIME_H_INCLUDED_
