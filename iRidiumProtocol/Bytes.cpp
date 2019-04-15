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
#include "Bytes.h"
#include "IridiumValue.h"

/**
   Запись беззнакового 8 битного значения в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_u8Value  - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteU8(u8* in_pBuffer, u8 in_u8Value)
{
   in_pBuffer[0] = in_u8Value;
   return in_pBuffer + 1;
}

/**
   Запись знакового 8 битного значения в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_s8Value  - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteS8(u8* in_pBuffer, s8 in_s8Value)
{
   in_pBuffer[0] = in_s8Value;
   return in_pBuffer + 1;
}

/**
   Запись беззнакового 16 битного значения, в LE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_u16Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteU16LE(u8* in_pBuffer, u16 in_u16Value)
{
   in_pBuffer[0] = in_u16Value & 0xFF;
   in_pBuffer[1] = (in_u16Value >> 8) & 0xFF;
   return in_pBuffer + 2;
}

/**
   Запись беззнакового 16 битного значения, в BE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_u16Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteU16BE(u8* in_pBuffer, u16 in_u16Value)
{
   in_pBuffer[0] = (in_u16Value >> 8) & 0xFF;
   in_pBuffer[1] = in_u16Value & 0xFF;
   return in_pBuffer + 2;
}

/**
   Запись знакового 16 битного значения, в LE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_s16Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteS16LE(u8* in_pBuffer, s16 in_s16Value)
{
   in_pBuffer[0] = in_s16Value & 0xFF;
   in_pBuffer[1] = (in_s16Value >> 8) & 0xFF;
   return in_pBuffer + 2;
}

/**
   Запись знакового 16 битного значения, в BE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_s16Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteS16BE(u8* in_pBuffer, s16 in_s16Value)
{
   in_pBuffer[0] = (in_s16Value >> 8) & 0xFF;
   in_pBuffer[1] = in_s16Value & 0xFF;
   return in_pBuffer + 2;
}

/**
   Запись беззнакового 32 битного значения, в LE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_u32Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteU32LE(u8* in_pBuffer, u32 in_u32Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy(in_pBuffer, (void*)&in_u32Value, 4);
#else
   in_pBuffer[0] = in_u32Value & 0xFF;
   in_pBuffer[1] = (in_u32Value >> 8) & 0xFF;
   in_pBuffer[2] = (in_u32Value >> 16) & 0xFF;
   in_pBuffer[3] = (in_u32Value >> 24) & 0xFF;
#endif
   return in_pBuffer + 4;
}

/**
   Запись беззнакового 32 битного значения, в BE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_u32Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteU32BE(u8* in_pBuffer, u32 in_u32Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pIn = (u8*)&in_u32Value;
   in_pBuffer[0] = l_pIn[3];
   in_pBuffer[1] = l_pIn[2]; 
   in_pBuffer[2] = l_pIn[1]; 
   in_pBuffer[3] = l_pIn[0]; 
#else
   in_pBuffer[0] = (in_u32Value >> 24) & 0xFF;
   in_pBuffer[1] = (in_u32Value >> 16) & 0xFF;
   in_pBuffer[2] = (in_u32Value >> 8) & 0xFF;
   in_pBuffer[3] = in_u32Value & 0xFF;
#endif
   return in_pBuffer + 4;
}

/**
   Запись знакового 32 битного значения, в LE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_s32Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteS32LE(u8* in_pBuffer, s32 in_s32Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy(in_pBuffer, (void*)&in_s32Value, 4);
#else
   in_pBuffer[0] = in_s32Value & 0xFF;
   in_pBuffer[1] = (in_s32Value >> 8) & 0xFF;
   in_pBuffer[2] = (in_s32Value >> 16) & 0xFF;
   in_pBuffer[3] = (in_s32Value >> 24) & 0xFF;
#endif
   return in_pBuffer + 4;
}

/**
   Запись знакового 32 битного значения, в BE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_s32Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteS32BE(u8* in_pBuffer, s32 in_s32Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pIn = (u8*)&in_s32Value;
   in_pBuffer[0] = l_pIn[3];
   in_pBuffer[1] = l_pIn[2];
   in_pBuffer[2] = l_pIn[1];
   in_pBuffer[3] = l_pIn[0];
#else
   in_pBuffer[0] = (in_s32Value >> 24) & 0xFF;
   in_pBuffer[1] = (in_s32Value >> 16) & 0xFF;
   in_pBuffer[2] = (in_s32Value >> 8) & 0xFF;
   in_pBuffer[3] = in_s32Value & 0xFF;
#endif
   return in_pBuffer + 4;
}

/**
   Запись 32 битного значения с плавающей запятой, в LE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_f32Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteF32LE(u8* in_pBuffer, f32 in_f32Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy(in_pBuffer, (void*)&in_f32Value, 4);
#else
   universal_value_t l_sVal;
   l_sVal.m_f32Value = in_f32Value;
   in_pBuffer[0] = l_sVal.m_u32Value & 0xFF;
   in_pBuffer[1] = (l_sVal.m_u32Value >> 8) & 0xFF;
   in_pBuffer[2] = (l_sVal.m_u32Value >> 16) & 0xFF;
   in_pBuffer[3] = (l_sVal.m_u32Value >> 24) & 0xFF;
#endif
   return in_pBuffer + 4;
}

/**
   Запись 32 битного значения с плавающей запятой, в BE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_f32Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteF32BE(u8* in_pBuffer, f32 in_f32Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pIn = (u8*)&in_f32Value;
   in_pBuffer[0] = l_pIn[3];
   in_pBuffer[1] = l_pIn[2];
   in_pBuffer[2] = l_pIn[1];
   in_pBuffer[3] = l_pIn[0];
#else
   universal_value_t l_sVal;
   l_sVal.m_f32Value = in_f32Value;
   in_pBuffer[0] = (l_sVal.m_u32Value >> 24) & 0xFF;
   in_pBuffer[1] = (l_sVal.m_u32Value >> 16) & 0xFF;
   in_pBuffer[2] = (l_sVal.m_u32Value >> 8) & 0xFF;
   in_pBuffer[3] = l_sVal.m_u32Value & 0xFF;
#endif
   return in_pBuffer + 4;
}

/**
   Запись беззнакового 64 битного значения, в LE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_u64Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteU64LE(u8* in_pBuffer, u64 in_u64Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy(in_pBuffer, (void*)&in_u64Value, 8);
#else
   in_pBuffer[0] = in_u64Value & 0xFF;
   in_pBuffer[1] = (in_u64Value >> 8) & 0xFF;
   in_pBuffer[2] = (in_u64Value >> 16) & 0xFF;
   in_pBuffer[3] = (in_u64Value >> 24) & 0xFF;
   in_pBuffer[4] = (in_u64Value >> 32) & 0xFF;
   in_pBuffer[5] = (in_u64Value >> 40) & 0xFF;
   in_pBuffer[6] = (in_u64Value >> 48) & 0xFF;
   in_pBuffer[7] = (in_u64Value >> 56) & 0xFF;
#endif
   return in_pBuffer + 8;
}

/**
   Запись беззнакового 64 битного значения, в BE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_u64Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteU64BE(u8* in_pBuffer, u64 in_u64Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pIn = (u8*)&in_u64Value;
   in_pBuffer[0] = l_pIn[7];
   in_pBuffer[1] = l_pIn[6];
   in_pBuffer[2] = l_pIn[5];
   in_pBuffer[3] = l_pIn[4];
   in_pBuffer[4] = l_pIn[3];
   in_pBuffer[5] = l_pIn[2];
   in_pBuffer[6] = l_pIn[1];
   in_pBuffer[7] = l_pIn[0];
#else
   in_pBuffer[0] = (in_u64Value >> 56) & 0xFF;
   in_pBuffer[1] = (in_u64Value >> 48) & 0xFF;
   in_pBuffer[2] = (in_u64Value >> 40) & 0xFF;
   in_pBuffer[3] = (in_u64Value >> 32) & 0xFF;
   in_pBuffer[4] = (in_u64Value >> 24) & 0xFF;
   in_pBuffer[5] = (in_u64Value >> 16) & 0xFF;
   in_pBuffer[6] = (in_u64Value >> 8) & 0xFF;
   in_pBuffer[7] = in_u64Value & 0xFF;
#endif
   return in_pBuffer + 8;
}

/**
   Запись знакового 64 битного значения, в LE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_s64Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteS64LE(u8* in_pBuffer, s64 in_s64Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy(in_pBuffer, (void*)&in_s64Value, 8);
#else
   in_pBuffer[0] = in_s64Value & 0xFF;
   in_pBuffer[1] = (in_s64Value >> 8) & 0xFF;
   in_pBuffer[2] = (in_s64Value >> 16) & 0xFF;
   in_pBuffer[3] = (in_s64Value >> 24) & 0xFF;
   in_pBuffer[4] = (in_s64Value >> 32) & 0xFF;
   in_pBuffer[5] = (in_s64Value >> 40) & 0xFF;
   in_pBuffer[6] = (in_s64Value >> 48) & 0xFF;
   in_pBuffer[7] = (in_s64Value >> 56) & 0xFF;
#endif
   return in_pBuffer + 8;
}

/**
   Запись знакового 64 битного значения, в BE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteS64BE(u8* in_pBuffer, s64 in_s64Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pIn = (u8*)&in_s64Value;
   in_pBuffer[0] = l_pIn[7];
   in_pBuffer[1] = l_pIn[6];
   in_pBuffer[2] = l_pIn[5];
   in_pBuffer[3] = l_pIn[4];
   in_pBuffer[4] = l_pIn[3];
   in_pBuffer[5] = l_pIn[2];
   in_pBuffer[6] = l_pIn[1];
   in_pBuffer[7] = l_pIn[0];
#else
   in_pBuffer[0] = (in_s64Value >> 56) & 0xFF;
   in_pBuffer[1] = (in_s64Value >> 48) & 0xFF;
   in_pBuffer[2] = (in_s64Value >> 40) & 0xFF;
   in_pBuffer[3] = (in_s64Value >> 32) & 0xFF;
   in_pBuffer[4] = (in_s64Value >> 24) & 0xFF;
   in_pBuffer[5] = (in_s64Value >> 16) & 0xFF;
   in_pBuffer[6] = (in_s64Value >> 8) & 0xFF;
   in_pBuffer[7] = in_s64Value & 0xFF;
#endif
   return in_pBuffer + 8;
}

/**
   Запись 64 битного значения с плавающей запятой, в LE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_f64Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteF64LE(u8* in_pBuffer, f64 in_f64Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy(in_pBuffer, (void*)&in_f64Value, 8);
#else
   universal_value_t l_sVal;
   l_sVal.m_f64Value = in_f64Value;
   in_pBuffer[0] = l_sVal.m_u64Value & 0xFF;
   in_pBuffer[1] = (l_sVal.m_u64Value >> 8) & 0xFF;
   in_pBuffer[2] = (l_sVal.m_u64Value >> 16) & 0xFF;
   in_pBuffer[3] = (l_sVal.m_u64Value >> 24) & 0xFF;
   in_pBuffer[4] = (l_sVal.m_u64Value >> 32) & 0xFF;
   in_pBuffer[5] = (l_sVal.m_u64Value >> 40) & 0xFF;
   in_pBuffer[6] = (l_sVal.m_u64Value >> 48) & 0xFF;
   in_pBuffer[7] = (l_sVal.m_u64Value >> 56) & 0xFF;
#endif
   return in_pBuffer + 8;
}

/**
   Запись 64 битного значения с плавающей запятой, в BE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_f64Value - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteF64BE(u8* in_pBuffer, f64 in_f64Value)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pIn = (u8*)&in_f64Value;
   in_pBuffer[0] = l_pIn[7];
   in_pBuffer[1] = l_pIn[6];
   in_pBuffer[2] = l_pIn[5];
   in_pBuffer[3] = l_pIn[4];
   in_pBuffer[4] = l_pIn[3];
   in_pBuffer[5] = l_pIn[2];
   in_pBuffer[6] = l_pIn[1];
   in_pBuffer[7] = l_pIn[0];
#else
   universal_value_t l_sVal;
   l_sVal.m_f64Value = in_f64Value;
   in_pBuffer[0] = (l_sVal.m_u64Value >> 56) & 0xFF;
   in_pBuffer[1] = (l_sVal.m_u64Value >> 48) & 0xFF;
   in_pBuffer[2] = (l_sVal.m_u64Value >> 40) & 0xFF;
   in_pBuffer[3] = (l_sVal.m_u64Value >> 32) & 0xFF;
   in_pBuffer[4] = (l_sVal.m_u64Value >> 24) & 0xFF;
   in_pBuffer[5] = (l_sVal.m_u64Value >> 16) & 0xFF;
   in_pBuffer[6] = (l_sVal.m_u64Value >> 8) & 0xFF;
   in_pBuffer[7] = l_sVal.m_u64Value & 0xFF;
#endif
   return in_pBuffer + 8;
}

/**
   Запись size_t значения, в BE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_stValue - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
   примечание  :  размер size_t в зависимости от компилятора может быть 4 байта для 32 битных систем
                  или 8 байт для 64 битных.
*/
u8* WriteSTBE(u8* in_pBuffer, size_t in_stValue)
{
   if(sizeof(size_t) == sizeof(u64))
      WriteU64BE(in_pBuffer, (u64)in_stValue);
   else
      WriteU32BE(in_pBuffer, (u32)in_stValue);

   return in_pBuffer + sizeof(size_t);
}

/**
   Запись size_t значения, в LE последовательности байт, в буфер
   на входе    :  in_pBuffer  - указатель на буфер
                  in_stValue - значение для записи
   на выходе   :  указатель на сдвинутый буфер с данными
   примечание  :  размер size_t в зависимости от компилятора может быть 4 байта для 32 битных систем
                  или 8 байт для 64 битных.
*/
u8* WriteSTLE(u8* in_pBuffer, size_t in_stValue)
{
   if(sizeof(size_t) == sizeof(u64))
      WriteU64LE(in_pBuffer, in_stValue);
   else
      WriteU32LE(in_pBuffer, in_stValue);

   return in_pBuffer + sizeof(size_t);
}

/**
   Чтение беззнакового 8 битного значения из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadU8(u8* in_pBuffer, u8& out_rValue)
{
   out_rValue = in_pBuffer[0];
   return in_pBuffer + 1;
}

/**
   Чтение знакового 8 битного значения из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadS8(u8* in_pBuffer, s8& out_rValue)
{
   out_rValue = in_pBuffer[0];
   return in_pBuffer + 1;
}

/**
   Чтение беззнакового 16 битного значения, в LE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadU16LE(u8* in_pBuffer, u16& out_rValue)
{
   out_rValue = in_pBuffer[0] | in_pBuffer[1] << 8;
   return in_pBuffer + 2;
}

/**
   Чтение беззнакового 16 битного значения, в BE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadU16BE(u8* in_pBuffer, u16& out_rValue)
{
   out_rValue = in_pBuffer[1] | in_pBuffer[0] << 8;
   return in_pBuffer + 2;
}

/**
   Чтение знакового 16 битного значения, в LE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadS16LE(u8* in_pBuffer, s16& out_rValue)
{
   out_rValue = in_pBuffer[0] | in_pBuffer[1] << 8;
   return in_pBuffer + 2;
}

/**
   Чтение знакового 16 битного значения, в BE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadS16BE(u8* in_pBuffer, s16& out_rValue)
{
   out_rValue = in_pBuffer[1] | in_pBuffer[0] << 8;
   return in_pBuffer + 2;
}

/**
   Чтение беззнакового 32 битного значения, в LE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadU32LE(u8* in_pBuffer, u32& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy((void*)&out_rValue, in_pBuffer, 4);
#else
   out_rValue = in_pBuffer[0] | in_pBuffer[1] << 8 | in_pBuffer[2] << 16 | in_pBuffer[3] << 24;
#endif
   return in_pBuffer + 4;
}

/**
   Чтение беззнакового 32 битного значения, в BE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadU32BE(u8* in_pBuffer, u32& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pOut = (u8*)&out_rValue;
   l_pOut[0] = in_pBuffer[3];
   l_pOut[1] = in_pBuffer[2];
   l_pOut[2] = in_pBuffer[1];
   l_pOut[3] = in_pBuffer[0];
#else
   out_rValue = in_pBuffer[3] | in_pBuffer[2] << 8 | in_pBuffer[1] << 16 | in_pBuffer[0] << 24;
#endif
   return in_pBuffer + 4;
}

/**
   Чтение знакового 32 битного значения, в LE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadS32LE(u8* in_pBuffer, s32& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy((void*)&out_rValue, in_pBuffer, 4);
#else
   out_rValue = in_pBuffer[0] | in_pBuffer[1] << 8 | in_pBuffer[2] << 16 | in_pBuffer[3] << 24;
#endif
   return in_pBuffer + 4;
}

/**
   Чтение знакового 32 битного значения, в BE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadS32BE(u8* in_pBuffer, s32& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pOut = (u8*)&out_rValue;
   l_pOut[0] = in_pBuffer[3];
   l_pOut[1] = in_pBuffer[2];
   l_pOut[2] = in_pBuffer[1];
   l_pOut[3] = in_pBuffer[0];
#else
   out_rValue = in_pBuffer[3] | in_pBuffer[2] << 8 | in_pBuffer[1] << 16 | in_pBuffer[0] << 24;
#endif
   return in_pBuffer + 4;
}

/**
   Чтение 32 битного значения с плавающей запятой, в LE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadF32LE(u8* in_pBuffer, f32& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy((void*)&out_rValue, in_pBuffer, 4);
#else
   universal_value_t l_sVal;
   l_sVal.m_u32Value = in_pBuffer[0] | in_pBuffer[1] << 8 | in_pBuffer[2] << 16 | in_pBuffer[3] << 24;
   out_rValue = l_sVal.m_f32Value;
#endif
   return in_pBuffer + 4;
}

/**
   Чтение 32 битного значения с плавающей запятой, в BE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadF32BE(u8* in_pBuffer, f32& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pOut = (u8*)&out_rValue;
   l_pOut[0] = in_pBuffer[3];
   l_pOut[1] = in_pBuffer[2];
   l_pOut[2] = in_pBuffer[1];
   l_pOut[3] = in_pBuffer[0];
#else
   universal_value_t l_sVal;
   l_sVal.m_u32Value = in_pBuffer[3] | in_pBuffer[2] << 8 | in_pBuffer[1] << 16 | in_pBuffer[0] << 24;
   out_rValue = l_sVal.m_f32Value;
#endif
   return in_pBuffer + 4;
}

/**
   Чтение беззнакового 64 битного значения, в LE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadU64LE(u8* in_pBuffer, u64& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy((void*)&out_rValue, in_pBuffer, 8);
#else
   out_rValue =   (u64)in_pBuffer[0]       |
                  (u64)in_pBuffer[1] << 8  |
                  (u64)in_pBuffer[2] << 16 |
                  (u64)in_pBuffer[3] << 24 |
                  (u64)in_pBuffer[4] << 32 |
                  (u64)in_pBuffer[5] << 40 |
                  (u64)in_pBuffer[6] << 48 |
                  (u64)in_pBuffer[7] << 56;
#endif
   return in_pBuffer + 8;
}

/**
   Чтение беззнакового 64 битного значения, в BE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadU64BE(u8* in_pBuffer, u64& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pOut = (u8*)&out_rValue;
   l_pOut[0] = in_pBuffer[7];
   l_pOut[1] = in_pBuffer[6];
   l_pOut[2] = in_pBuffer[5];
   l_pOut[3] = in_pBuffer[4];
   l_pOut[4] = in_pBuffer[3];
   l_pOut[5] = in_pBuffer[2];
   l_pOut[6] = in_pBuffer[1];
   l_pOut[7] = in_pBuffer[0];
#else
   out_rValue =   (u64)in_pBuffer[7]       |
                  (u64)in_pBuffer[6] << 8  |
                  (u64)in_pBuffer[5] << 16 |
                  (u64)in_pBuffer[4] << 24 |
                  (u64)in_pBuffer[3] << 32 |
                  (u64)in_pBuffer[2] << 40 |
                  (u64)in_pBuffer[1] << 48 |
                  (u64)in_pBuffer[0] << 56;
#endif
   return in_pBuffer + 8;
}

/**
   Чтение знакового 64 битного значения, в LE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadS64LE(u8* in_pBuffer, s64& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy((void*)&out_rValue, in_pBuffer, 8);
#else
   out_rValue =   (u64)in_pBuffer[0]       |
                  (u64)in_pBuffer[1] << 8  |
                  (u64)in_pBuffer[2] << 16 |
                  (u64)in_pBuffer[3] << 24 |
                  (u64)in_pBuffer[4] << 32 |
                  (u64)in_pBuffer[5] << 40 |
                  (u64)in_pBuffer[6] << 48 |
                  (u64)in_pBuffer[7] << 56;
#endif
   return in_pBuffer + 8;
}

/**
   Чтение знакового 64 битного значения, в BE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadS64BE(u8* in_pBuffer, s64& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pOut = (u8*)&out_rValue;
   l_pOut[0] = in_pBuffer[7];
   l_pOut[1] = in_pBuffer[6];
   l_pOut[2] = in_pBuffer[5];
   l_pOut[3] = in_pBuffer[4];
   l_pOut[4] = in_pBuffer[3];
   l_pOut[5] = in_pBuffer[2];
   l_pOut[6] = in_pBuffer[1];
   l_pOut[7] = in_pBuffer[0];
#else
   out_rValue =   (u64)in_pBuffer[7]       |
                  (u64)in_pBuffer[6] << 8  |
                  (u64)in_pBuffer[5] << 16 |
                  (u64)in_pBuffer[4] << 24 |
                  (u64)in_pBuffer[3] << 32 |
                  (u64)in_pBuffer[2] << 40 |
                  (u64)in_pBuffer[1] << 48 |
                  (u64)in_pBuffer[0] << 56;
#endif
   return in_pBuffer + 8;
}

/**
   Чтение 64 битного значения с плавающей запятой, в LE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadF64LE(u8* in_pBuffer, f64& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   memcpy((void*)&out_rValue, in_pBuffer, 8);
#else
   universal_value_t l_sVal;
   l_sVal.m_u64Value =  (u64)in_pBuffer[0]       |
                        (u64)in_pBuffer[1] << 8  |
                        (u64)in_pBuffer[2] << 16 |
                        (u64)in_pBuffer[3] << 24 |
                        (u64)in_pBuffer[4] << 32 |
                        (u64)in_pBuffer[5] << 40 |
                        (u64)in_pBuffer[6] << 48 |
                        (u64)in_pBuffer[7] << 56;

   out_rValue = l_sVal.m_f64Value;
#endif
   return in_pBuffer + 8;
}

/**
   Чтение 64 битного значения с плавающей запятой, в BE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadF64BE(u8* in_pBuffer, f64& out_rValue)
{
#if defined(IRIDIUM_AVR_PLATFORM)
   u8* l_pOut = (u8*)&out_rValue;
   l_pOut[0] = in_pBuffer[7];
   l_pOut[1] = in_pBuffer[6];
   l_pOut[2] = in_pBuffer[5];
   l_pOut[3] = in_pBuffer[4];
   l_pOut[4] = in_pBuffer[3];
   l_pOut[5] = in_pBuffer[2];
   l_pOut[6] = in_pBuffer[1];
   l_pOut[7] = in_pBuffer[0];
#else
   universal_value_t l_sVal;
   l_sVal.m_u64Value =  (u64)in_pBuffer[7]       |
                        (u64)in_pBuffer[6] << 8  |
                        (u64)in_pBuffer[5] << 16 |
                        (u64)in_pBuffer[4] << 24 |
                        (u64)in_pBuffer[3] << 32 |
                        (u64)in_pBuffer[2] << 40 |
                        (u64)in_pBuffer[1] << 48 |
                        (u64)in_pBuffer[0] << 56;

   out_rValue = l_sVal.m_f64Value;
#endif
   return in_pBuffer + 8;
}

/**
   Чтение size_t значения, в BE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
   примечание  :  размер size_t в зависимости от компилятора может быть 4 байта для 32 битных систем
                  или 8 байт для 64 битных.
*/
u8* ReadSTBE(u8* in_pBuffer, size_t& out_rValue)
{
   if(sizeof(size_t) == sizeof(u64))
      ReadU64BE(in_pBuffer, (u64&)out_rValue);
   else
      ReadU32BE(in_pBuffer, (u32&)out_rValue);

   return in_pBuffer + sizeof(size_t);
}

/**
   Чтение size_t значения, в LE последовательности байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  out_rValue  - ссылка на переменную куда нужно поместить значение
   на выходе   :  указатель на сдвинутый буфер с данными
   примечание  :  размер size_t в зависимости от компилятора может быть 4 байта для 32 битных систем
                  или 8 байт для 64 битных.
*/
u8* ReadSTLE(u8* in_pBuffer, size_t& out_rValue)
{
   if(sizeof(size_t) == sizeof(u64))
      ReadU64LE(in_pBuffer, (u64&)out_rValue);
   else
      ReadU32LE(in_pBuffer, (u32&)out_rValue);

   return in_pBuffer + sizeof(size_t);
}

/*
bool BytesUnitTest()
{
   u32 l_u32Sum = 0;
   u8 l_aBuffer[1024];
   u8* l_pPtr = l_aBuffer;

   u8 l_u8Val = 0;
   s8 l_s8Val = 0;
   u16 l_u16Val = 0;
   s16 l_s16Val = 0;
   u32 l_u32Val = 0;
   s32 l_s32Val = 0;
   f32 l_f32Val = 0.0f;
   u64 l_u64Val = 0;
   s64 l_s64Val = 0;
   f64 l_f64Val = 0.0;
   const char l_szBuf[5] = { 0, 0, 0, 0, 0 };

   u8 l_aU8[8]    = {   0,    1,  255,  244, -1, -60, -78, -99 };
   s8 l_aS8[8]    = { -11,  -22, -127, -100, -1, -63, 111, -12 };
   u16 l_aU16[8]  = {   0, 0x1234,  0x4321,  -1, -16555, -6000, 7896, -10099 };
   s16 l_aS16[8]  = { -1144, -3222, -25400, -1200, 9887, 6653, 1211, -1882 };
   u32 l_aU32[8]  = {   0, 0x12345678,  0x43214321,  -1, -16555, -6000, 7896, -10099 };
   s32 l_aS32[8]  = { -11448877, -388222, -2533400, -12200, 928287, 636543, 1555211, -133882 };
   f32 l_aF32[8]  = { -114488.75626f, -388222.88367f, -233400.524f, -12200.423f, 928287.12345f, 63654.5243f, 155511.6244f, -133882.5434f };
   u64 l_aU64[8]  = {   0, 0x1234567887654321,  0x2143218765423465,  -1, -17365366555, -60222020, 782926, -122220099 };
   s64 l_aS64[8]  = { -1124242828277, -382822222, -251111313400, -122222020, 922822287, 6336353433, 1545542411, -1335585852 };
   f64 l_aF64[8]  = { -11448456786448.75626, -3882665554433422.88367, -2533400.524, -12200.423, 928287.1544332345, 636543.5243, 1555211.65244, -133882.5434 };

   for(u32 i = 0; i < 8; i++)
   {
      l_u32Sum = 0;
      l_pPtr = l_aBuffer;
      l_pPtr = WriteU8(l_pPtr, l_aU8[i]);
      l_u32Sum++;
      l_pPtr = WriteS8(l_pPtr, l_aS8[i]);
      l_u32Sum++;
      l_pPtr = WriteU16LE(l_pPtr, l_aU16[i]);
      l_u32Sum++;
      l_pPtr = WriteU16BE(l_pPtr, l_aU16[i]);
      l_u32Sum++;
      l_pPtr = WriteS16LE(l_pPtr, l_aS16[i]);
      l_u32Sum++;
      l_pPtr = WriteS16BE(l_pPtr, l_aS16[i]);
      l_u32Sum++;
      l_pPtr = WriteU32LE(l_pPtr, l_aU32[i]);
      l_u32Sum++;
      l_pPtr = WriteU32BE(l_pPtr, l_aU32[i]);
      l_u32Sum++;
      l_pPtr = WriteS32LE(l_pPtr, l_aS32[i]);
      l_u32Sum++;
      l_pPtr = WriteS32BE(l_pPtr, l_aS32[i]);
      l_u32Sum++;
      l_pPtr = WriteF32LE(l_pPtr, l_aF32[i]);
      l_u32Sum++;
      l_pPtr = WriteF32BE(l_pPtr, l_aF32[i]);
      l_u32Sum++;
      l_pPtr = WriteU64LE(l_pPtr, l_aU64[i]);
      l_u32Sum++;
      l_pPtr = WriteU64BE(l_pPtr, l_aU64[i]);
      l_u32Sum++;
      l_pPtr = WriteS64LE(l_pPtr, l_aS64[i]);
      l_u32Sum++;
      l_pPtr = WriteS64BE(l_pPtr, l_aS64[i]);
      l_u32Sum++;
      l_pPtr = WriteF64LE(l_pPtr, l_aF64[i]);
      l_u32Sum++;
      l_pPtr = WriteF64BE(l_pPtr, l_aF64[i]);
      l_u32Sum++;

      l_pPtr = l_aBuffer;
      l_pPtr = ReadU8(l_pPtr, l_u8Val);
      if(l_u8Val == l_aU8[i])
         l_u32Sum--;
      l_pPtr = ReadS8(l_pPtr, l_s8Val);
      if(l_s8Val == l_aS8[i])
         l_u32Sum--;
      l_pPtr = ReadU16LE(l_pPtr, l_u16Val);
      if(l_u16Val == l_aU16[i])
         l_u32Sum--;
      l_pPtr = ReadU16BE(l_pPtr, l_u16Val);
      if(l_u16Val == l_aU16[i])
         l_u32Sum--;
      l_pPtr = ReadS16LE(l_pPtr, l_s16Val);
      if(l_s16Val == l_aS16[i])
         l_u32Sum--;
      l_pPtr = ReadS16BE(l_pPtr, l_s16Val);
      if(l_s16Val == l_aS16[i])
         l_u32Sum--;
      l_pPtr = ReadU32LE(l_pPtr, l_u32Val);
      if(l_u32Val == l_aU32[i])
         l_u32Sum--;
      l_pPtr = ReadU32BE(l_pPtr, l_u32Val);
      if(l_u32Val == l_aU32[i])
         l_u32Sum--;
      l_pPtr = ReadS32LE(l_pPtr, l_s32Val);
      if(l_s32Val == l_aS32[i])
         l_u32Sum--;
      l_pPtr = ReadS32BE(l_pPtr, l_s32Val);
      if(l_s32Val == l_aS32[i])
         l_u32Sum--;
      l_pPtr = ReadF32LE(l_pPtr, l_f32Val);
      if(l_f32Val == l_aF32[i])
         l_u32Sum--;
      l_pPtr = ReadF32BE(l_pPtr, l_f32Val);
      if(l_f32Val == l_aF32[i])
         l_u32Sum--;
      l_pPtr = ReadU64LE(l_pPtr, l_u64Val);
      if(l_u64Val == l_aU64[i])
         l_u32Sum--;
      l_pPtr = ReadU64BE(l_pPtr, l_u64Val);
      if(l_u64Val == l_aU64[i])
         l_u32Sum--;
      l_pPtr = ReadS64LE(l_pPtr, l_s64Val);
      if(l_s64Val == l_aS64[i])
         l_u32Sum--;
      l_pPtr = ReadS64BE(l_pPtr, l_s64Val);
      if(l_s64Val == l_aS64[i])
         l_u32Sum--;
      l_pPtr = ReadF64LE(l_pPtr, l_f64Val);
      if(l_f64Val == l_aF64[i])
         l_u32Sum--;
      l_pPtr = ReadF64BE(l_pPtr, l_f64Val);
      if(l_f64Val == l_aF64[i])
         l_u32Sum--;

      if(l_u32Sum)
         break;
   }
   return (l_u32Sum == 0);
}
*/
