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
#include "IridiumBytes.h"
#include "IridiumValue.h"

/**
   Сериализация байт в буфер
   на входе    :  in_pBuffer  - указатель на буфер куда нужно записать значение
                  in_u8Byte   - значение для записи в буфер
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteByte(u8* in_pBuffer, u8 in_u8Byte)
{
   *in_pBuffer++ = in_u8Byte;
   return in_pBuffer;
}

/**
   "Прямая" сериализация данных
   на входе    :  in_pBuffer  - указатель на буфер куда нужно записать значение
                  in_pValue   - указатель на записываемое значени
                  in_stSize   - размер данных
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteDirect(u8* in_pBuffer, void* in_pValue, size_t in_stSize)
{
   memcpy(in_pBuffer, in_pValue, in_stSize);
   return in_pBuffer + in_stSize;
}

/**
   "Обратная" сериализация данных
   на входе    :  in_pBuffer  - указатель на буфер куда нужно записать значение
                  in_pValue   - указатель на записываемое значени
                  in_stSize   - размер данных
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* WriteReverse(u8* in_pBuffer, void* in_pValue, size_t in_stSize)
{
   u8* l_pIn = (u8*)in_pValue + in_stSize - 1;
   for(size_t i = 0; i < in_stSize; i++)
      *in_pBuffer++ = *l_pIn--;

   return in_pBuffer;
}

/**
   Десериализация байт из буфера
   на входе    :  in_pBuffer  - указатель на буфер откуда нужно прочитать значения
                  in_rByte    - ссылка на переменную куда нужно прочитать значение
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadByte(u8* in_pBuffer, u8& in_rByte)
{
   in_rByte = *in_pBuffer++;
   return in_pBuffer;
}

/**
   "Прямая" десериализация данных
   на входе    :  in_pBuffer  - указатель на буфер откуда нужно прочитать значения
                  in_pValue   - указатель на значени куда нужно записать полученое значение
                  in_stSize   - размер данных
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadDirect(u8* in_pBuffer, void* in_pValue, size_t in_stSize)
{
   memcpy(in_pValue, in_pBuffer, in_stSize);
   return in_pBuffer + in_stSize;
}

/**
   "Обратная" десериализация данных
   на входе    :  in_pBuffer  - указатель на буфер откуда нужно прочитать значения
                  in_pValue   - указатель на значени куда нужно записать полученое значение
                  in_stSize   - размер данных
   на выходе   :  указатель на сдвинутый буфер с данными
*/
u8* ReadReverse(u8* in_pBuffer, void* in_pValue, size_t in_stSize)
{
   u8* l_pOut = (u8*)in_pValue + in_stSize - 1;
   for(size_t i = 0; i < in_stSize; i++)
      *l_pOut-- = *in_pBuffer++;

   return in_pBuffer;
}

#if 0
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
#endif
