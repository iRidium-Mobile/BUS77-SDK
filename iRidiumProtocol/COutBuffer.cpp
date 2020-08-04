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
#include "COutBuffer.h"
#include "IridiumBytes.h"

#if defined(IRIDIUM_MCU_AVR)
#include <avr/pgmspace.h>
#endif

#define IRIDIUM_VALUE_RESERVE 5                    // Зарезервированый размер в исходящем буфере для строк

/**
   Конструктор класса
   на входе    :  *
*/
COutBuffer::COutBuffer()
{
   m_pBuffer   = NULL;
   m_pEnd      = NULL;
   m_pPtr      = NULL;
}

/**
   Деструктор класса
*/
COutBuffer::~COutBuffer()
{
}

/**
   Установка внешнего буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  in_stSize   - размер буфера
   на выходе   :  *
*/
void COutBuffer::SetBuffer(void* in_pBuffer, size_t in_stSize)
{
   m_pBuffer = (u8*)in_pBuffer;
   m_pEnd = m_pBuffer + in_stSize;

   Clear();
}

/**
   Резервирование указаного количества байт
   на входе    :  in_stSize   - размер зарезервированной области
   на выходе   :  указатель на зарезервированную область, если NULL произошла ошибка
*/
u8* COutBuffer::CreateAnchor(size_t in_stSize)
{
   u8* l_pResult = NULL;
   if((m_pPtr + in_stSize) <= m_pEnd)
   {
      l_pResult = m_pPtr;
      m_pPtr += in_stSize;
   } else
      SetError();
   return l_pResult;
}

/**
   Заполнение зарезервированной области значением в LE последовательности
   на входе    :  in_pAnchor  - указатель на зарезервированную область
                  in_pValue   - указатель на значение
                  in_stSize   - размер записываемой области
   на выходе   :  *
*/
void COutBuffer::SetAnchorLE(u8* in_pAnchor, void* in_pValue, size_t in_stSize)
{
   if(in_pAnchor)
      WriteLE(in_pAnchor, in_pValue, in_stSize);
}

/**
   Заполнение зарезервированной области значением в BE последовательности
   на входе    :  in_pAnchor  - указатель на зарезервированную область
                  in_pValue   - указатель на значение
                  in_stSize   - размер записываемой области
   на выходе   :  *
*/
void COutBuffer::SetAnchorBE(u8* in_pAnchor, void* in_pValue, size_t in_stSize)
{
   if(in_pAnchor)
      WriteBE(in_pAnchor, in_pValue, in_stSize);
}

/**
   Добавление 8 битного безнакового значения
   на входе    :  in_u8Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddU8(u8 in_u8Value)
{
   if((m_pPtr + 1) <= m_pEnd)
      m_pPtr = WriteByte(m_pPtr, in_u8Value);
   else
      SetError();
}

/**
   Добавление 8 битного знакового значения
   на входе    :  in_s8Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddS8(s8 in_s8Value)
{
   if((m_pPtr + 1) <= m_pEnd)
      m_pPtr = WriteByte(m_pPtr, in_s8Value);
   else
      SetError();
}

/**
   Добавление 16 битного беззнакового значения LE
   на входе    :  in_u16Value - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddU16LE(u16 in_u16Value)
{
   if((m_pPtr + 2) <= m_pEnd)
      m_pPtr = WriteLE(m_pPtr, &in_u16Value, 2);
   else
      SetError();
}

/**
   Добавление 16 битного безнакового значения BE
   на входе    :  in_u16Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddU16BE(u16 in_u16Value)
{
   if((m_pPtr + 2) <= m_pEnd)
      m_pPtr = WriteBE(m_pPtr, &in_u16Value, 2);
   else
      SetError();
}

/**
   Добавление 16 битного знакового значения LE
   на входе    :  in_s16Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddS16LE(s16 in_s16Value)
{
   if((m_pPtr + 2) <= m_pEnd)
      m_pPtr = WriteLE(m_pPtr, &in_s16Value, 2);
   else
      SetError();
}

/**
   Добавление 16 битного знакового значения BE
   на входе    :  in_s16Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddS16BE(s16 in_s16Value)
{
   if((m_pPtr + 2) <= m_pEnd)
      m_pPtr = WriteBE(m_pPtr, &in_s16Value, 2);
   else
      SetError();
}

/**
   Добавление 32 битного беззнакового значения LE
   на входе    :  in_u32Value - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddU32LE(u32 in_u32Value)
{
   if((m_pPtr + 4) <= m_pEnd)
      m_pPtr = WriteLE(m_pPtr, &in_u32Value, 4);
   else
      SetError();
}

/**
   Добавление 32 битного беззнакового значения BE
   на входе    :  in_u32Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddU32BE(u32 in_u32Value)
{
   if((m_pPtr + 4) <= m_pEnd)
      m_pPtr = WriteBE(m_pPtr, &in_u32Value, 4);
   else
      SetError();
}

/**
   Добавление 32 битного знакового значения LE
   на входе    :  in_s32Value - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddS32LE(s32 in_s32Value)
{
   if((m_pPtr + 4) <= m_pEnd)
      m_pPtr = WriteLE(m_pPtr, &in_s32Value, 4);
   else
      SetError();
}

/**
   Добавление 32 битного знакового значения BE
   на входе    :  in_s32Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddS32BE(s32 in_s32Value)
{
   if((m_pPtr + 4) <= m_pEnd)
      m_pPtr = WriteBE(m_pPtr, &in_s32Value, 4);
   else
      SetError();
}

/**
   Добавление 32 битного значения с плавающей запятой LE
   на входе    :  in_f32Value - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddF32LE(f32 in_f32Value)
{
   if((m_pPtr + 4) <= m_pEnd)
      m_pPtr = WriteLE(m_pPtr, &in_f32Value, 4);
   else
      SetError();
}

/**
   Добавление 32 битного значения с плавающей запятой BE
   на входе    :  in_f32Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddF32BE(f32 in_f32Value)
{
   if((m_pPtr + 4) <= m_pEnd)
      m_pPtr = WriteBE(m_pPtr, &in_f32Value, 4);
   else
      SetError();
}

/**
   Добавление 64 битного беззнакового значения LE
   на входе    :  in_u64Value - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddU64LE(u64 in_u64Value)
{
   if((m_pPtr + 8) <= m_pEnd)
      m_pPtr = WriteLE(m_pPtr, &in_u64Value, 8);
   else
      SetError();
}

/**
   Добавление 64 битного беззнакового значения BE
   на входе    :  in_u64Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddU64BE(u64 in_u64Value)
{
   if((m_pPtr + 8) <= m_pEnd)
      m_pPtr = WriteBE(m_pPtr, &in_u64Value, 8);
   else
      SetError();
}

/**
   Добавление 64 битного знакового значения LE
   на входе    :  in_s64Value - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddS64LE(s64 in_s64Value)
{
   if((m_pPtr + 8) <= m_pEnd)
      m_pPtr = WriteLE(m_pPtr, &in_s64Value, 8);
   else
      SetError();
}

/**
   Добавление 64 битного знакового значения BE
   на входе    :  in_s64Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddS64BE(s64 in_s64Value)
{
   if((m_pPtr + 8) <= m_pEnd)
      m_pPtr = WriteBE(m_pPtr, &in_s64Value, 8);
   else
      SetError();
}

/**
   Добавление 64 битного значения с плавающей запятой LE
   на входе    :  in_f64Value - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddF64LE(f64 in_f64Value)
{
   if((m_pPtr + 8) <= m_pEnd)
      m_pPtr = WriteLE(m_pPtr, &in_f64Value, 8);
   else
      SetError();
}

/**
   Добавление 64 битного значения с плавающей запятой BE
   на входе    :  in_f64Value  - значение для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddF64BE(f64 in_f64Value)
{
   if((m_pPtr + 8) <= m_pEnd)
      m_pPtr = WriteBE(m_pPtr, &in_f64Value, 8);
   else
      SetError();
}

/**
   Добавление стринга в пакет
   на входе    :  in_pszStr   - указатель на стринг
   на выходе   :  успешность добавления в буфер
   примечание  :  вместе со стрингом добавляется конец строки - 0
*/
void COutBuffer::AddString(const char* in_pszStr)
{
   // Добавление строки
   if(in_pszStr)
      AddData(in_pszStr, strlen(in_pszStr));
   // Добавление конца строки
   AddU8(0);
}

#if defined(IRIDIUM_MCU_AVR)

/**
   Добавление стринга в пакет из Flash памяти
   на входе    :  in_pszStr   - указатель на стринг
   на выходе   :  успешность добавления в буфер
   примечание  :  вместе со стрингом добавляется конец строки - 0
*/
void COutBuffer::AddStringFromFlash(const char* in_pszStr)
{
   // Вычисление длинны строки
   size_t l_stSize = in_pszStr ? strlen_P(in_pszStr) : 0;
   if(l_stSize)
   {
      if((m_pPtr + l_stSize) <= m_pEnd)
      {
         memcpy_P(m_pPtr, in_pszStr, l_stSize);
         m_pPtr += l_stSize;
      } else
         SetError();
   }
   AddU8(0);
}
#endif

/**
   Добавление стринга в пакет без добавления конца строки
   на входе    :  in_pszStr   - указатель на стринг
   на выходе   :  *
*/
void COutBuffer::AddStringEndless(const char* in_pszStr)
{
   // Проверка наличия данных
   if(in_pszStr)
      AddData(in_pszStr, strlen(in_pszStr));
}

/**
   Добавление байтовой последовательности в пакет
   на входе    :  in_pData    - указатель на данные
                  in_stSize   - количество байт для записи
   на выходе   :  успешность добавления в буфер
*/
void COutBuffer::AddData(const void* in_pData, size_t in_stSize)
{
   // Проверка на наличие данных
   if(in_pData && in_stSize)
   {
      // Проверка возможности добавления данных
      if((m_pPtr + in_stSize) <= m_pEnd)
      {
         memcpy(m_pPtr, in_pData, in_stSize);
         m_pPtr += in_stSize;
      } else
         SetError();
   }
}

/**
   Добавление времени в поток
   на входе    :  in_rTime - ссылка на время
   на выходе   :  успешность добавления в буфер
*/
bool COutBuffer::AddTime(iridium_time_t& in_rTime)
{
   u32 l_u32Data = 0;
   // Проверка наличия данных
   if(in_rTime.m_u8Flags)
   {
      // Проверка наличия данных даты
      if(in_rTime.m_u8Flags & IRIDIUM_TIME_FLAG_DATE)
      {
         l_u32Data = 0;
         // Добавление дня 5 бит
         l_u32Data |= (u32)(in_rTime.m_u8Day & 0x1F) << 20;
         // Добавление месяца 4 бита
         l_u32Data |= (u32)(in_rTime.m_u8Month & 0xF) << 16;
         // Добавление года 14 бит
         l_u32Data |= (u32)(in_rTime.m_u16Year & 0x3FFF) << 2;
         // Добавление флагов 2 бита
         l_u32Data |= (u32)in_rTime.m_u8Flags & 0x3;
         // Добавление данных даты в поток
         AddU32LE(l_u32Data);
      }
      // Проверка наличия данных времени
      if(in_rTime.m_u8Flags & IRIDIUM_TIME_FLAG_TIME)
      {
         l_u32Data = 0;
         // Добавление дня недели 3 бита
         l_u32Data |= (u32)(in_rTime.m_u8DayOfWeek & 0x7) << 29;
         // Добавление миллисекунд 10 бит
         l_u32Data |= (u32)(in_rTime.m_u16Milliseconds & 0x3FF) << 19;
         // Добавление секунд 6 бит
         l_u32Data |= (u32)(in_rTime.m_u8Second & 0x3F) << 13;
         // Добавление минут 6 бит
         l_u32Data |= (u32)(in_rTime.m_u8Minute & 0x3F) << 7;
         // Добавление часов 5 бит
         l_u32Data |= (u32)(in_rTime.m_u8Hour & 0x1F) << 2;
         // Добавление флагов 2 бита
         l_u32Data |= (u32)in_rTime.m_u8Flags & 0x3;
         // Добавление данных времени в поток
         AddU32LE(l_u32Data);
      } 
   }
   return Result();
}

/**
   Запись универсального значения в поток
   на входе    :  in_u8Type      - тип значения
                  in_rValue      - ссылка на универсальное значение которое нужно поместить в поток
                  out_stRemain   - ссылка на переменную с остатоком
                  in_stReserve   - размер зарезервированных данных
   на выходе   :  успешность добавления в буфер
   примечание  :  in_stPosition и in_stMax параметры которые используются только для значений с типами
                  IVT_STRING8 и IVT_ARRAY_U8
*/
bool COutBuffer::AddValue(u8 in_u8Type, universal_value_t& in_rValue, size_t& out_stRemain, size_t in_stReserve)
{
   bool l_bFlag = false;

   // Если значение строка или массив, а значение есть, инициализируем остаток
   if((in_u8Type == IVT_STRING8 || in_u8Type == IVT_ARRAY_U8) && in_rValue.m_Array.m_stSize && in_rValue.m_Array.m_pPtr)
   {
      // Проверка инициализации остатка
      if(out_stRemain == (size_t)-1)
         out_stRemain = in_rValue.m_Array.m_stSize;
   } else
      out_stRemain = 0;

   // Резервируем место для типа
   u8* l_pAnchor = CreateAnchor(1);
   if(l_pAnchor)
   {
      // Обработка типа
      switch(in_u8Type)
      {
      case IVT_NONE:
         break;
      case IVT_BOOL:
         l_bFlag |= in_rValue.m_bValue;
         break;
      case IVT_S8:
         if(in_rValue.m_s8Value)
         {
            l_bFlag = true;
            AddS8(in_rValue.m_s8Value);
         }
         break;
      case IVT_U8:
         if(in_rValue.m_u8Value)
         {
            l_bFlag = true;
            AddU8(in_rValue.m_u8Value);
         }
         break;
      case IVT_S16:
         if(in_rValue.m_s16Value)
         {
            l_bFlag = true;
            AddS16LE(in_rValue.m_s16Value);
         }
         break;
      case IVT_U16:
         if(in_rValue.m_u16Value)
         {
            l_bFlag = true;
            AddU16LE(in_rValue.m_u16Value);
         }
         break;
      case IVT_S32:
         if(in_rValue.m_s32Value)
         {
            l_bFlag = true;
            AddS32LE(in_rValue.m_s32Value);
         }
         break;
      case IVT_U32:
         if(in_rValue.m_u32Value)
         {
            l_bFlag = true;
            AddU32LE(in_rValue.m_u32Value);
         }
         break;
      case IVT_F32:
         if(in_rValue.m_f32Value != 0.0f)
         {
            l_bFlag = true;
            AddF32LE(in_rValue.m_f32Value);
         }
         break;
      case IVT_S64:
         if(in_rValue.m_s64Value)
         {
            l_bFlag = true;
            AddS64LE(in_rValue.m_s64Value);
         }
         break;
      case IVT_U64:
         if(in_rValue.m_u64Value)
         {
            l_bFlag = true;
            AddU64LE(in_rValue.m_u64Value);
         }
         break;
      case IVT_F64:
         if(in_rValue.m_f64Value != 0.0)
         {
            l_bFlag = true;
            AddF64LE(in_rValue.m_f64Value);
         }
         break;
      case IVT_STRING8:
         if(in_rValue.m_Array.m_stSize && in_rValue.m_Array.m_pPtr)
         {
            // Проверка наличия свободного места в буфере
            size_t l_stSize = Free();
            // Проверка влезет ли пакет в буфер с учетом зарезервированного места плюс размер строки и контрольной суммы
            if(l_stSize > (in_stReserve + IRIDIUM_VALUE_RESERVE))
            {
               // Вычисление позиции в строке
               size_t l_stPos = in_rValue.m_Array.m_stSize - out_stRemain;

               l_stSize -= (in_stReserve + IRIDIUM_VALUE_RESERVE);
               if(l_stSize > out_stRemain)
                  l_stSize = out_stRemain;

               // Добавление размера строки плюс 0 в конце
               AddU16LE(l_stSize + 1);
               // Добавление данных
#if defined(IRIDIUM_MCU_AVR)
               if(in_rValue.m_Array.m_bMem)
                  memcpy_P(m_pPtr, (const char*)in_rValue.m_Array.m_pPtr + l_stPos, l_stSize);
               else
                  memcpy(m_pPtr, (const char*)in_rValue.m_Array.m_pPtr + l_stPos, l_stSize);
#else
               memcpy(m_pPtr, (const char*)in_rValue.m_Array.m_pPtr + l_stPos, l_stSize);
#endif
               m_pPtr += l_stSize;
               // Добавление 0 в конец
               *m_pPtr++ = 0;
               // Сдвиг позиции
               out_stRemain -= l_stSize;
               // Отметим наличие данных
               l_bFlag = true;
            } else
               SetError();
               //l_bResult = false;
         }
         break;
      case IVT_TIME:
         // Проверка наличия данных
         if(in_rValue.m_Time.m_u8Flags)
         {
            l_bFlag = true;
            AddTime(in_rValue.m_Time);
         // Если даты или времени нет
         } else
            l_bFlag = false;
         break;
      case IVT_ARRAY_U8:
         if(in_rValue.m_Array.m_stSize && in_rValue.m_Array.m_pPtr)
         {
            // Проверка наличия свободного места в буфере
            size_t l_stSize = Free();
            // Проверка влезет ли пакет в буфер с учетом зарезервированного места плюс размер строки и контрольной суммы
            if(l_stSize > (in_stReserve + IRIDIUM_VALUE_RESERVE))
            {
               // Вычисление позиции в строке
               size_t l_stPos = in_rValue.m_Array.m_stSize - out_stRemain;

               l_stSize -= (in_stReserve + IRIDIUM_VALUE_RESERVE);
               if(l_stSize > out_stRemain)
                  l_stSize = out_stRemain;

               // Добавление размера строки
               AddU16LE(l_stSize);
               // Добавление данных
#if defined(IRIDIUM_MCU_AVR)
               if(in_rValue.m_Array.m_bMem)
                  memcpy_P(m_pPtr, (const char*)in_rValue.m_Array.m_pPtr + l_stPos, l_stSize);
               else
                  memcpy(m_pPtr, (const char*)in_rValue.m_Array.m_pPtr + l_stPos, l_stSize);
#else
               memcpy(m_pPtr, (const char*)in_rValue.m_Array.m_pPtr + l_stPos, l_stSize);
#endif
               m_pPtr += l_stSize;
               // Сдвиг позиции
               out_stRemain -= l_stSize;
               // Отметим наличие данных
               l_bFlag = true;
            } else
               SetError();
         }
         break;
      default:
         SetError();
         break;
      }
      // Запишем значение типа
      if(Result())
      {
         in_u8Type |= l_bFlag << 7;
         SetAnchorLE(l_pAnchor, &in_u8Type, 1);
      }
   }
   return Result();
}

/**
   Сдвиг буфера
   на входе    :  in_stMove   - размер сдвига
   на выходе   :  успешность сдвига
*/
void COutBuffer::Shift(size_t in_stMove)
{
   // Проверка на выход за пределы 
   size_t l_stSize = Size();
   if(l_stSize > in_stMove)
   {
      // Сдвиг буфера
      memmove(m_pBuffer, m_pBuffer + in_stMove, l_stSize - in_stMove);
      m_pPtr -= in_stMove;
   } else
      m_pPtr = m_pBuffer;
}

#if 0
bool COutStream::UnitTest()
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
   char l_szBuf[5] = { 0, 0, 0, 0, 0 };

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
      AddU8(l_aU8[i]);
      l_u32Sum++;
      AddS8(l_aS8[i]);
      l_u32Sum++;
      AddU16LE(l_aU16[i]);
      l_u32Sum++;
      AddU16BE(l_aU16[i]);
      l_u32Sum++;
      AddS16LE(l_aS16[i]);
      l_u32Sum++;
      AddS16BE(l_aS16[i]);
      l_u32Sum++;
      AddU32LE(l_aU32[i]);
      l_u32Sum++;
      AddU32BE(l_aU32[i]);
      l_u32Sum++;
      AddS32LE(l_aS32[i]);
      l_u32Sum++;
      AddS32BE(l_aS32[i]);
      l_u32Sum++;
      AddF32LE(l_aF32[i]);
      l_u32Sum++;
      AddF32BE(l_aF32[i]);
      l_u32Sum++;
      AddU64LE(l_aU64[i]);
      l_u32Sum++;
      AddU64BE(l_aU64[i]);
      l_u32Sum++;
      AddS64LE(l_aS64[i]);
      l_u32Sum++;
      AddS64BE(l_aS64[i]);
      l_u32Sum++;
      AddF64LE(l_aF64[i]);
      l_u32Sum++;
      AddF64BE(l_aF64[i]);
      l_u32Sum++;

      l_pPtr = GetPacketPtr();
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
      Clear();
   }
   return (l_u32Sum == 0);
}
#endif
