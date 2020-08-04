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
#ifndef _IRIDIUM_VALUE_H_INCLUDED_
#define _IRIDIUM_VALUE_H_INCLUDED_

#include "IridiumTypes.h"
#include "IridiumTime.h"

// Типы данных
enum eIridiumValueType
{
   IVT_NONE = 0,
   IVT_BOOL,
   IVT_S8,
   IVT_U8,
   IVT_S16,
   IVT_U16,
   IVT_S32,
   IVT_U32,
   IVT_F32,
   IVT_S64,
   IVT_U64,
   IVT_F64,
   IVT_STRING8,
   IVT_POINTER,                                    // Данный тип не используется, удалять нельзя, можно только на его место поместить новый тип
   IVT_TIME,
   IVT_ARRAY_U8,

   IVT_COUNT
};

// Универсальный тип
typedef union universal_value_s
{
   u64            m_u64Value;                      // IVT_U64 примечение: данный тип расположен здесь, для обеспечения статической инициализации. Если перенести, не будет работать правильно
   bool           m_bValue;                        // IVT_BOOL
   u8             m_u8Value;                       // IVT_U8
   s8             m_s8Value;                       // IVT_S8
   u16            m_u16Value;                      // IVT_U16
   s16            m_s16Value;                      // IVT_S16
   u32            m_u32Value;                      // IVT_U32
   s32            m_s32Value;                      // IVT_S32
   f32            m_f32Value;                      // IVT_F32
   s64            m_s64Value;                      // IVT_S64
   f64            m_f64Value;                      // IVT_F64
   iridium_time_t m_Time;                          // IVT_TIME
   struct                                          // IVT_STRING8/IVT_ARRAY_U8 примечение: данные тип нельзя инициализировать статически
   {
      size_t      m_stSize;                        // Размер строки или массива
      void*       m_pPtr;                          // Указатель на строку или массив
      
#if defined(IRIDIUM_MCU_AVR)
      bool        m_bMem;                          // Определение в каком типе памяти расположены данные
#endif
   } m_Array;
} universal_value_t;
#endif // _IRIDIUM_VALUE_H_INCLUDED_
