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
#ifndef _C_IN_BUFFER_H_INCLUDED_
#define _C_IN_BUFFER_H_INCLUDED_

#include "IridiumTypes.h"
#include "IridiumValue.h"

class CInBuffer
{
public:
   // Конструктор/деструктор
   CInBuffer();
   virtual ~CInBuffer();

   // Методы
   void SetBuffer(const void* in_pBuffer, size_t in_stSize);
   void* GetBuffer()
      { return m_pBuffer; }
   u8* GetDataPtr()                                // Получение текущего указателя на данные
      { return m_pReadPtr; }
   size_t Size() const                             // Получение размера данных
      { return m_pWritePtr - m_pReadPtr; }

   // Установка/получение позиции в буфере
   void Seek(size_t in_stPosition)
      { m_pReadPtr = m_pBuffer + in_stPosition; }
   size_t Tell()
      { return m_pReadPtr - m_pBuffer; }

   size_t Free()                                   // Получение доступного свободного пространства
      { return m_pEnd - m_pWritePtr; }
   size_t Used()                                   // Получение занятого пространства
      { return m_pWritePtr - m_pBuffer; }

   void Reset();                                   // Сброс параметров буфера
   virtual void Clear();                           // Очистка с обнулением данных
   void Skip(size_t in_stSize);                    // Пропустить указаное количество байт
   void Shift();                                   // Сдвиг буфера
   void Cut(size_t in_stStart, size_t in_stSize);  // Удаление фрагмента буфера

   // Добавление данных
   bool AddByte(u8 in_u8Byte);                     // Добавление одного байт
   size_t Add(const void* in_pBuffer, size_t in_stSize); // Добавление данных

   // 8 Bit
   bool GetU8(u8& in_rValue);                      // Получение 8 битного беззнакового значения
   bool GetS8(s8& in_rValue);                      // Получение 8 битного знакового значения

   // 16 Bit
   bool GetU16LE(u16& out_rValue);                 // Получение 16 битного беззнакового значения LE
   bool GetU16BE(u16& out_rValue);                 // Получение 16 битного беззнакового значения BE
   bool GetS16LE(s16& out_rValue);                 // Получение 16 битного знакового значения LE
   bool GetS16BE(s16& out_rValue);                 // Получение 16 битного знакового значения BE

   // 32 Bit
   bool GetU32LE(u32& out_rValue);                 // Получение 32 битного беззнакового значения LE
   bool GetU32BE(u32& out_rValue);                 // Получение 32 битного беззнакового значения BE
   bool GetS32LE(s32& out_rValue);                 // Получение 32 битного знакового значения LE
   bool GetS32BE(s32& out_rValue);                 // Получение 32 битного знакового значения BE
   bool GetF32LE(f32& out_rValue);                 // Получение 32 битного значения с плавающей запятой LE
   bool GetF32BE(f32& out_rValue);                 // Получение 32 битного значения с плавающей запятой BE

   // 64 bit
   bool GetU64LE(u64& out_rValue);                 // Получение 64 битного беззнакового значения LE
   bool GetU64BE(u64& out_rValue);                 // Получение 64 битного беззнакового значения BE
   bool GetS64LE(s64& out_rValue);                 // Получение 64 битного знакового значения LE
   bool GetS64BE(s64& out_rValue);                 // Получение 64 битного знакового значения BE
   bool GetF64LE(f64& out_rValue);                 // Получение 64 битного значения с плавающей запятой LE
   bool GetF64BE(f64& out_rValue);                 // Получение 64 битного значения с плавающей запятой BE

   // Строки
   bool GetString(char*& out_rString);             // Получение указателя на строку в буфере

   // Данные
   bool FillData(u8* in_pData, size_t in_stSize);  // Заполнение данными буфера со сдвигом позиции

   // Добавление времени
   bool GetTime(iridium_time_t& out_rTime);        // Получение времени

   // Универсальное значение
   bool GetValue(u8& out_rType, universal_value_t& out_rValue);

#if 0
   bool UnitTest();
#endif

protected:
   u8*   m_pBuffer;                                // Буфер с данными
   u8*   m_pEnd;                                   // Указатель на конец буфера
   u8*   m_pWritePtr;                              // Указатель на область записи данных
   u8*   m_pReadPtr;                               // Указатель на область чтения данных
};
#endif
