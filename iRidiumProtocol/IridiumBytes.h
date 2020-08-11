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
#ifndef _IRIDIUM_BYTES_H_INCLUDED_
#define _IRIDIUM_BYTES_H_INCLUDED_

#include "IridiumTypes.h"

// Сериализация данных
u8* WriteByte(u8* in_pBuffer, u8 in_u8Bytes);
u8* WriteDirect(u8* in_pBuffer, void* in_pValue, size_t in_stSize);
u8* WriteReverse(u8* in_pBuffer, void* in_pValue, size_t in_stSize);

// Десериализация данных
u8* ReadByte(u8* in_pBuffer, u8& in_u8Bytes);
u8* ReadDirect(u8* in_pBuffer, void* in_pValue, size_t in_stSize);
u8* ReadReverse(u8* in_pBuffer, void* in_pValue, size_t in_stSize);

// Проверка на порядок байт
#if IRIDIUM_BYTE_ORDER == IRIDIUM_LITTLE_ENDIAN
// Остроконечный (Little-endian) порядок байт
#define WriteLE(...) WriteDirect(__VA_ARGS__)
#define WriteBE(...) WriteReverse(__VA_ARGS__)
#define ReadLE(...)  ReadDirect(__VA_ARGS__)
#define ReadBE(...)  ReadReverse(__VA_ARGS__)

#elif IRIDIUM_BYTE_ORDER == IRIDIUM_BIG_ENDIAN
// Сетевой (Big-ending) тупоконечный порядок байт
#define WriteLE(...) WriteReverse(__VA_ARGS__)
#define WriteBE(...) WriteDirect(__VA_ARGS__)
#define ReadLE(...)  ReadReverse(__VA_ARGS__)
#define ReadBE(...)  ReadDirect(__VA_ARGS__)

#else
#error "Byte order not defined!"
#endif

#if 0
bool BytesUnitTest();
#endif

#endif   // _IRIDIUM_BYTES_H_INCLUDED_
