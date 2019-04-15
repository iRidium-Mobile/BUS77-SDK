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
#ifndef _IRIDIUM_TYPES_H_INCLUDED_
#define _IRIDIUM_TYPES_H_INCLUDED_

// Включения
#include "IridiumPlatform.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#ifndef IRIDIUM_WINDOWS_PLATFORM
  #include <inttypes.h>
#endif

#ifndef __cplusplus
typedef uint8_t bool;
#endif

// Определение типов
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef const char cc;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

// time_t на некоторых платформах может быть равен s32 и он может переполниться
typedef time_t    irtime_t;                        // переопределение времени. пока в коде остается time_t, но в идеале перейти полностью
typedef irtime_t  iridium_sec;                     // время в секундах. храним его в time_t, хватит примерно на 68 лет
typedef u64       iridium_ms;                      // время в миллисекундах
typedef time_t    irdelta_ms;                      // разница во времени в миллисекундах. используется для малых величин

#endif   // _IRIDIUM_TYPES_H_INCLUDED_
