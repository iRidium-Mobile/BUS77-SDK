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
#include "CInBuffer.h"
#include "IridiumBytes.h"

#include <stdio.h>

/**
   Конструктор класса
   на входе    :  *
*/
CInBuffer::CInBuffer()
{
   Reset();
}

/**
   Деструктор класса
*/
CInBuffer::~CInBuffer()
{
}

/**
   Установка внешнего буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  in_stSize   - размер буфера
   на выходе   :  *
*/
void CInBuffer::SetBuffer(const void* in_pBuffer, size_t in_stSize)
{
   // Установка границ буфера
   m_pBuffer   = (u8*)in_pBuffer;
   m_pEnd      = m_pBuffer + in_stSize;
   m_pReadPtr  = m_pBuffer;
   m_pWritePtr = m_pEnd;
}

/**
   Сброс параметров буфера
   на входе    :  *
   на выходе   :  *
*/
void CInBuffer::Reset()
{
   m_pBuffer = NULL;
   m_pEnd = NULL;
   m_pReadPtr = NULL;
   m_pWritePtr = NULL;
}

/**
   Очистка буфера
   на входе    :  *
   на выходе   :  *
*/
void CInBuffer::Clear()
{
   m_pReadPtr  = m_pBuffer;
   m_pWritePtr = m_pBuffer;

   memset(m_pBuffer, 0, m_pEnd - m_pBuffer);
}

/**
   Пропустить указаное количество байт
   на входе    :  in_stSize  - количество пропускаемых байт
   на выходе   :  *
*/
void CInBuffer::Skip(size_t in_stSize)
{
   m_pReadPtr += in_stSize;
   if(m_pReadPtr > m_pEnd)
      m_pReadPtr = m_pEnd;
}

/**
   Сдвиг буфера, данные после позиции чтения копируются в начало буфера
   на входе    :  *
   на выходе   :  *
*/
void CInBuffer::Shift()
{
   // Сдвиг данных
   size_t l_stMove = Size();
   if(l_stMove)
      memmove(m_pBuffer, m_pReadPtr, l_stMove);

   m_pReadPtr = m_pBuffer;
   m_pWritePtr = m_pReadPtr + l_stMove;
}

/**
   Добавление байта в буфер
   на входе    :  in_u8Byte   - значение для добавления
   на выходе   :  true  - значение было добавлено
                  false - недостаточно места, значение не записано
*/
bool CInBuffer::AddByte(u8 in_u8Byte)
{
   bool l_bResult = false;
   if(m_pWritePtr < m_pEnd)
   {
      m_pWritePtr[0] = in_u8Byte;
      m_pWritePtr++;
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Добавление данных в буфер
   на входе    :  in_pBuffer  - указатель на буфер с данными
                  in_stSize   - размер данных
   на выходе   :  количество добавленных байт
*/
size_t CInBuffer::Add(const void* in_pBuffer, size_t in_stSize)
{
   // Проверка размера данных
   size_t l_stResult = Free();
   if(l_stResult > in_stSize)
      l_stResult = in_stSize;
   else
      l_stResult = 0;

   // Копирование данных
   memcpy(m_pWritePtr, in_pBuffer, l_stResult);
   m_pWritePtr += l_stResult;
   return l_stResult;
}

/**
   Получение 8 битного значения из потока
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetU8(u8& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 1) <= m_pWritePtr)
   {
      m_pReadPtr = ReadByte(m_pReadPtr, out_rValue);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 8 битного знакового значения из потока
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetS8(s8& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 1) <= m_pWritePtr)
   {
      m_pReadPtr = ReadByte(m_pReadPtr, (u8&)out_rValue);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 16 битного беззнакового значения из потока LE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetU16LE(u16& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 2) <= m_pWritePtr)
   {
      m_pReadPtr = ReadLE(m_pReadPtr, &out_rValue, 2);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 16 битного беззнакового значения из потока BE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetU16BE(u16& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 2) <= m_pWritePtr)
   {
      m_pReadPtr = ReadBE(m_pReadPtr, &out_rValue, 2);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 16 битного знакового значения из потока LE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetS16LE(s16& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 2) <= m_pWritePtr)
   {
      m_pReadPtr = ReadLE(m_pReadPtr, &out_rValue, 2);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 16 битного знакового значения из потока BE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetS16BE(s16& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 2) <= m_pWritePtr)
   {
      m_pReadPtr = ReadBE(m_pReadPtr, &out_rValue, 2);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 32 битного беззнакового значения из потока LE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetU32LE(u32& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 4) <= m_pWritePtr)
   {
      m_pReadPtr = ReadLE(m_pReadPtr, &out_rValue, 4);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 32 битного беззнакового значения из потока BE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetU32BE(u32& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 4) <= m_pWritePtr)
   {
      m_pReadPtr = ReadBE(m_pReadPtr, &out_rValue, 4);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 32 битного знакового значения из потока LE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetS32LE(s32& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 4) <= m_pWritePtr)
   {
      m_pReadPtr = ReadLE(m_pReadPtr, &out_rValue, 4);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 32 битного знакового значения из потока BE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetS32BE(s32& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 4) <= m_pWritePtr)
   {
      m_pReadPtr = ReadBE(m_pReadPtr, &out_rValue, 4);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 32 битного значения с плавающей запятой из потока LE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetF32LE(f32& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 4) <= m_pWritePtr)
   {
      m_pReadPtr = ReadLE(m_pReadPtr, &out_rValue, 4);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 32 битного значения с плавающей запятой из потока BE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetF32BE(f32& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 4) <= m_pWritePtr)
   {
      m_pReadPtr = ReadBE(m_pReadPtr, &out_rValue, 4);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 64 битного беззнакового значения из потока LE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetU64LE(u64& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 8) <= m_pWritePtr)
   {
      m_pReadPtr = ReadLE(m_pReadPtr, &out_rValue, 8);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 64 битного беззнакового значения из потока BE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetU64BE(u64& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 8) <= m_pWritePtr)
   {
      m_pReadPtr = ReadBE(m_pReadPtr, &out_rValue, 8);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 64 битного знакового значения из потока LE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetS64LE(s64& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 8) <= m_pWritePtr)
   {
      m_pReadPtr = ReadLE(m_pReadPtr, &out_rValue, 8);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 64 битного знакового значения из потока BE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetS64BE(s64& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 8) <= m_pWritePtr)
   {
      m_pReadPtr = ReadBE(m_pReadPtr, &out_rValue, 8);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 64 битного значения с плавающей запятой из потока LE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetF64LE(f64& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 8) <= m_pWritePtr)
   {
      m_pReadPtr = ReadLE(m_pReadPtr, &out_rValue, 8);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение 64 битного значения с плавающей запятой из потока BE
   на входе    :  out_rValue  - ссылка на перемунную куда нужно поместить результат
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetF64BE(f64& out_rValue)
{
   bool l_bResult = false;
   if((m_pReadPtr + 8) <= m_pWritePtr)
   {
      m_pReadPtr = ReadBE(m_pReadPtr, &out_rValue, 8);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение указателя на строку
   на входе    :  out_rString - ссылка на указатель куда нужно поместить указатель на строку
   на выходе   :  успешность
*/
bool CInBuffer::GetString(char*& out_rString)
{
   bool l_bResult = false;

   // Поиск конца строки в пределах полученных данных
   size_t l_stSize = 1;
   for(u8* l_pszPtr = m_pReadPtr; l_pszPtr < m_pWritePtr; l_pszPtr++, l_stSize++)
   {
      if(!*l_pszPtr)
      {
         out_rString = (char*)m_pReadPtr;
         m_pReadPtr += l_stSize;
         l_bResult = true;
         break;
      }
   }
   return l_bResult;
}

/**
   Заполнение данными указанного буфера со сдвигом позиции
   на входе    :  out_pBuffer - указатель на буфер который надо заполнить данными
                  in_stSize   - размер получаемых данных
   на выходе   :  успешность
*/
bool CInBuffer::FillData(u8* out_pBuffer, size_t in_stSize)
{
   bool l_bResult = false;
   if(out_pBuffer && in_stSize && (m_pReadPtr + in_stSize) <= m_pWritePtr)
   {
      memcpy(out_pBuffer, m_pReadPtr, in_stSize);
      m_pReadPtr += in_stSize;
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение универсального значения из потока
   на входе    :  out_rType   - ссылка на переменную куда нужно поместить тип значения
                  out_rValue  - ссылка на структуру куда нужно поместить значение
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetValue(u8& out_rType, universal_value_t& out_rValue)
{
   bool l_bResult = false;
   bool l_bValue = false;

   out_rType = IVT_NONE;
   memset(&out_rValue, 0, sizeof(universal_value_t));

   // Получение типа значения
   l_bResult = GetU8(out_rType);
   if(l_bResult)
   {
      // Проверка наличия данных
      l_bValue = (0 != (out_rType >> 7));
      // Отделение типа
      out_rType &= 0x7F;
      switch(out_rType)
      {
      // Неизвестный тип
      case IVT_NONE:
         break;
         
      // Битовое значение
      case IVT_BOOL:
         out_rValue.m_bValue = l_bValue;
         break;

      // Знаковое 8 битное значени
      case IVT_S8:
         if(l_bValue)
            l_bResult = GetS8(out_rValue.m_s8Value);
         break;

      // Беззнаковое 8 битное значени
      case IVT_U8:
         if(l_bValue)
            l_bResult = GetU8(out_rValue.m_u8Value);
         break;

      // Знаковое 16 битное значени
      case IVT_S16:
         if(l_bValue)
            l_bResult = GetS16LE(out_rValue.m_s16Value);
         break;

      // Беззнаковое 16 битное значени
      case IVT_U16:
         if(l_bValue)
            l_bResult = GetU16LE(out_rValue.m_u16Value);
         break;

      // Знаковое 32 битное значени
      case IVT_S32:
         if(l_bValue)
            l_bResult = GetS32LE(out_rValue.m_s32Value);
         break;

      // Беззнаковое 32 битное значени
      case IVT_U32:
         if(l_bValue)
            l_bResult = GetU32LE(out_rValue.m_u32Value);
         break;

      // 32 битное значени с плавающей запятой
      case IVT_F32:
         if(l_bValue)
            l_bResult = GetF32LE(out_rValue.m_f32Value);
         else
            out_rValue.m_f32Value = 0.0f;
         break;

      // Знаковое 64 битное число
      case IVT_S64:
         if(l_bValue)
            l_bResult = GetS64LE(out_rValue.m_s64Value);
         break;

      // Беззнаковое 64 битное число
      case IVT_U64:
         if(l_bValue)
            l_bResult = GetU64LE(out_rValue.m_u64Value);
         break;

      // 64 битное число с плавающей запятой
      case IVT_F64:
         if(l_bValue)
            l_bResult = GetF64LE(out_rValue.m_f64Value);
         else
            out_rValue.m_f64Value = 0.0;
         break;

      // Строка
      case IVT_STRING8:
         if(l_bValue)
         {
            u16 l_u16Size = 0;
            l_bResult = GetU16LE(l_u16Size);
            if(l_bResult)
            {
               // Заполнение параметров строки
               out_rValue.m_Array.m_stSize = l_u16Size;
               out_rValue.m_Array.m_pPtr = GetDataPtr();
               
#if defined(IRIDIUM_MCU_AVR)
               // Указание расположения типа памяти с данными
               out_rValue.m_Array.m_bMem = false;
#endif
            }
            Skip(l_u16Size);
         }
         break;

      // Время
      case IVT_TIME:
         if(l_bValue)
            l_bResult = GetTime(out_rValue.m_Time);
         break;

      // Массив
      case IVT_ARRAY_U8:
         if(l_bValue)
         {
            u16 l_u16Size = 0;
            l_bResult = GetU16LE(l_u16Size);
            if(l_bResult)
            {
               // Заполнение параметров строки
               out_rValue.m_Array.m_stSize = l_u16Size;
               out_rValue.m_Array.m_pPtr = GetDataPtr();

#if defined(IRIDIUM_MCU_AVR)
               // Указание расположения типа памяти с данными
               out_rValue.m_Array.m_bMem = false;
#endif
            }
            Skip(l_u16Size);
         }
         break;
      default:
         l_bResult = false;
         break;
      }
   }
   return l_bResult;
}

/**
   Получение времени
   на входе    :  out_rTime   - ссылка на переменную куда нужно поместить данные о времени
   на выходе   :  успешность получения значения
*/
bool CInBuffer::GetTime(iridium_time_t& out_rTime)
{
   bool l_bResult = true;
   u8 l_u8Flag = 0;
   u32 l_u32Data = 0;
   // Чтение данных флагов
   ReadByte(GetDataPtr(), l_u8Flag);
   // Извлечение флагов
   out_rTime.m_u8Flags = l_u8Flag & 0x3;
   // Проверка наличия данных даты
   if(out_rTime.m_u8Flags & IRIDIUM_TIME_FLAG_DATE)
   {
      // Чтение данных даты
      l_bResult = GetU32LE(l_u32Data);
      if(l_bResult)
      {
         // Получение года 14 бит
         out_rTime.m_u16Year = (l_u32Data >> 2) & 0x3FFF;
         // Получение месяца 4 бита
         out_rTime.m_u8Month = (l_u32Data >> 16) & 0xF;
         // Получение дня 5 бит
         out_rTime.m_u8Day = (l_u32Data >> 20) & 0x1F;
      }
   }
   // Проверка наличия данных времени
   if(out_rTime.m_u8Flags & IRIDIUM_TIME_FLAG_TIME)
   {
      // Чтение данных времени
      l_bResult = GetU32LE(l_u32Data);
      if(l_bResult)
      {
         // Получение часов 5 бит
         out_rTime.m_u8Hour = (l_u32Data >> 2) & 0x1F;
         // Получение минут 6 бит
         out_rTime.m_u8Minute = (l_u32Data >> 7) & 0x3F;
         // Получение секунд 6 бит
         out_rTime.m_u8Second = (l_u32Data >> 13) & 0x3F;
         // Получение миллисекунд 10 бит
         out_rTime.m_u16Milliseconds = (l_u32Data >> 19) & 0x3FF;
         // Получение дня недели 3 бита
         out_rTime.m_u8DayOfWeek = (l_u32Data >> 29) & 0x7;
      }
   }
   return l_bResult;
}

/*
bool CInBuffer::UnitTest()
{
   u32 l_u32Sum = 0;
   u8 l_aBuffer[512];
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

      // Добавление в буфер
      Add(l_aBuffer, l_pPtr - l_aBuffer);

      if(GetU8(l_u8Val) && l_u8Val == l_aU8[i])
         l_u32Sum--;
      if(GetS8(l_s8Val) && l_s8Val == l_aS8[i])
         l_u32Sum--;
      if(GetU16LE(l_u16Val) && l_u16Val == l_aU16[i])
         l_u32Sum--;
      if(GetU16BE(l_u16Val) && l_u16Val == l_aU16[i])
         l_u32Sum--;
      if(GetS16LE(l_s16Val) && l_s16Val == l_aS16[i])
         l_u32Sum--;
      if(GetS16BE(l_s16Val) && l_s16Val == l_aS16[i])
         l_u32Sum--;
      if(GetU32LE(l_u32Val) && l_u32Val == l_aU32[i])
         l_u32Sum--;
      if(GetU32BE(l_u32Val) && l_u32Val == l_aU32[i])
         l_u32Sum--;
      if(GetS32LE(l_s32Val) && l_s32Val == l_aS32[i])
         l_u32Sum--;
      if(GetS32BE(l_s32Val) && l_s32Val == l_aS32[i])
         l_u32Sum--;
      if(GetF32LE(l_f32Val) && l_f32Val == l_aF32[i])
         l_u32Sum--;
      if(GetF32BE(l_f32Val) && l_f32Val == l_aF32[i])
         l_u32Sum--;
      if(GetU64LE(l_u64Val) && l_u64Val == l_aU64[i])
         l_u32Sum--;
      if(GetU64BE(l_u64Val) && l_u64Val == l_aU64[i])
         l_u32Sum--;
      if(GetS64LE(l_s64Val) && l_s64Val == l_aS64[i])
         l_u32Sum--;
      if(GetS64BE(l_s64Val) && l_s64Val == l_aS64[i])
         l_u32Sum--;
      if(GetF64LE(l_f64Val) && l_f64Val == l_aF64[i])
         l_u32Sum--;
      if(GetF64BE(l_f64Val) && l_f64Val == l_aF64[i])
         l_u32Sum--;

      Shift();
      
      if(l_u32Sum)
         break;
   }
   return (l_u32Sum == 0);
}
*/
