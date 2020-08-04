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
#ifndef _C_OUT_BUFFER_H_INCLUDED_
#define _C_OUT_BUFFER_H_INCLUDED_

// включения
#include "IridiumTypes.h"
#include "IridiumValue.h"

// Буфер 
class COutBuffer
{
public:
   // Конструктор/деструктор
   COutBuffer();
   virtual ~COutBuffer();

   // Установка буфера
   void SetBuffer(void* in_pBuffer, size_t in_stSize);
   void* GetBuffer() const
      { return m_pBuffer; }

   // Получение данных пакета
   size_t Size() const                                      // Получение размера заполненного буфера
      { return m_pPtr - m_pBuffer; }
   size_t Free()                                            // Получение свободного пространства в буфере
      { return m_pEnd - m_pPtr; }

   void Clear()                                             // Очистка буфера
      { m_pPtr = m_pBuffer; }

   // Резервирование переменных
   u8* CreateAnchor(size_t in_stSize);                     // Резервирование указаного количества байт
   void SetAnchorLE(u8* in_pAnchor, void* in_pValue, size_t in_stSize);     // Запись в зарезервированное место указаного количества байт в LE порядке
   void SetAnchorBE(u8* in_pAnchor, void* in_pValue, size_t in_stSize);     // Запись в зарезервированное место указаного количества байт в BE порядке

   // 8 bit
   void AddU8(u8 in_u8Value);                               // Добавление 8 битного беззнакового значения
   void AddS8(s8 in_s8Value);                               // Добавление 8 битного знакового значения

   // 16 bit
   void AddU16LE(u16 in_u16Value);                          // Добавление 16 битного беззнакового значения LE
   void AddU16BE(u16 in_u16Value);                          // Добавление 16 битного беззнакового значения BE
   void AddS16LE(s16 in_s16Value);                          // Добавление 16 битного знакового значения LE
   void AddS16BE(s16 in_s16Value);                          // Добавление 16 битного знакового значения LE

   // 32 bit
   void AddU32LE(u32 in_u32Value);                          // Добавление 32 битного беззнакового значения LE
   void AddU32BE(u32 in_u32Value);                          // Добавление 32 битного беззнакового значения BE
   void AddS32LE(s32 in_s32Value);                          // Добавление 32 битного знакового значения LE
   void AddS32BE(s32 in_s32Value);                          // Добавление 32 битного знакового значения BE
   void AddF32LE(f32 in_f32Value);                          // Добавление 32 битного значения c плавающей запятой LE
   void AddF32BE(f32 in_f32Value);                          // Добавление 32 битного значения c плавающей запятой BE

   // 64 bit
   void AddU64LE(u64 in_u64Value);                          // Добавление 64 битного беззнакового значения LE
   void AddU64BE(u64 in_u64Value);                          // Добавление 64 битного беззнакового значения BE
   void AddS64LE(s64 in_s64Value);                          // Добавление 64 битного знакового значения LE
   void AddS64BE(s64 in_s64Value);                          // Добавление 64 битного знакового значения BE
   void AddF64LE(f64 in_f64Value);                          // Добавление 64 битного значения c плавающей запятой LE
   void AddF64BE(f64 in_f64Value);                          // Добавление 64 битного значения c плавающей запятой BE

   // Строки
   void AddString(const char* in_pszStr);                   // Добавить стринг (должен оканчиватся 0)

#if defined(IRIDIUM_MCU_AVR)
   void AddStringFromFlash(const char* in_pszStr);          // Добавить стринг из сегмента программы (должен оканчиватся 0)
#endif

   void AddStringEndless(const char* in_pszStr);            // Добавить стринг без добавления в конце 0
   bool AddStringWithU16Size(const char* in_pszStr);        // Добавить стринг без добавления в конце 0, с добавлением в начале двух байтного размера данных
   bool AddStringWithU8Size(const char* in_pszStr);         // Добавить стринг без добавления в конце 0, с добавлением в начале однобайтного размера данных

   // Данные
   void AddData(const void* in_pData, size_t in_stSize);    // Добавление данных

   // Добавление времени
   bool AddTime(iridium_time_t& in_rTime);                  // Добавление времени

   // Универсальное значение
   bool AddValue(u8 in_u8Type, universal_value_t& in_rValue, size_t& out_stPosition, size_t in_stMax);
   
   void Shift(size_t in_stMove);

   // Проверка успешности
   void SetError()
      { m_pPtr = m_pEnd + 1; }
   // Проверка успешности
   bool Result()
      { return (m_pPtr <= m_pEnd); }

#if 0
   bool UnitTest();
#endif

protected:
   u8*   m_pBuffer;                                // Указатель на начало буфера с данными
   u8*   m_pEnd;                                   // Указатель на конец буфера
   u8*   m_pPtr;                                   // Текущая позиция записи данных
};
#endif // _C_OUT_BUFFER_H_INCLUDED_
