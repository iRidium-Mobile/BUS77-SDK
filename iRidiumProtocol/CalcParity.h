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
#ifndef _CALC_PARITY_H_INCLUDED_
#define _CALC_PARITY_H_INCLUDED_

// Включения
#include "IridiumTypes.h"

// Получение четности байта
inline bool GetParity(u8 in_u8Byte)
{
   in_u8Byte = (in_u8Byte >> 4) ^ (in_u8Byte & 0xF);
   in_u8Byte = (in_u8Byte >> 2) ^ (in_u8Byte & 0x3);
   in_u8Byte = (in_u8Byte >> 1) ^ (in_u8Byte & 0x1);
   return (0 != in_u8Byte);
}
#endif   // _CALC_PARITY_H_INCLUDED_

