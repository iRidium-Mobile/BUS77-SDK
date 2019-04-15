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
#ifndef _IRIDIUM_CRC16_H_INCLUDED_
#define _IRIDIUM_CRC16_H_INCLUDED_

#include "IridiumConfig.h"
#include "IridiumTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// Работа с Flash памятью
u16 GetCRC16Modbus(u16 in_u16Init, const u8* in_pBuffer, size_t in_stSize);
   
#ifdef __cplusplus
}
#endif

#endif  // _IRIDIUM_CRC16_H_INCLUDED_
