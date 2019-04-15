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
#include "CIridiumStreebog.h"

/**
   Конструктор класса
   на входе    :  *
*/
CIridiumStreebog::CIridiumStreebog()
{
}

/**
   Деструктор класса
*/
CIridiumStreebog::~CIridiumStreebog()
{
}

/**
   Сложение двух длинных чисел с помещением результата в третье число
   на входе    :  out_pDst - указатель на длинное число куда нужно поместить результат работы
                  in_pSrc  - указатель на первое длинное число
                  in_pAdd  - указатель не второй длинное число
   на выходе   :  *
*/
void CIridiumStreebog::AddMod512(u8* out_pDst, u8* in_pSrc, u8* in_pAdd)
{
   u16 l_u16Overrun = 0;

   for(u8 i = STREEBOG_BLOCK_SIZE; i-- > 0; )
   {
      l_u16Overrun = in_pSrc[i] + in_pAdd[i] + (l_u16Overrun >> 8);
      out_pDst[i] = (u8)l_u16Overrun;
   }
}

/**
   Добавление числа к длинному числу, с помещением результата во второе длинное число
   на входе    :  out_pDst - указатель на длинное число в которое нужно поместить результат
                  in_pSrc  - указатель на первое длинное число
                  in_u8Add - 8 битное число для сложения
   на выходе   :  *
   примечание  :  добавляемое число умножается на 8
*/
void CIridiumStreebog::AddMod512_u8(u8* out_pDst, u8* in_pSrc, u8 in_u8Add)
{
   u16 l_u16Add = in_u8Add * 8;
   for(u8 i = STREEBOG_BLOCK_SIZE; i-- > 0; )
   {
      l_u16Add += in_pSrc[i];
      out_pDst[i] = (u8)l_u16Add;
      l_u16Add >>= 8;
   }
}

/**
   Применение исключающего ИЛИ между двумя массивами, с записью в третий массив
   на входе    :  out_pDst - указатель на массив в который надо поместить результат
                  in_pA    - указатель на первый массив
                  in_pB    - указатель на второй массив
   на выходе   :  *
*/
void CIridiumStreebog::Xor512(u8* out_pDst, u8* in_pA, u8* in_pB)
{
   for(u8 i = 0; i < STREEBOG_BLOCK_SIZE; i++)
      out_pDst[i] = in_pA[i] ^ in_pB[i];
}

/**
   Трансформация значений вектора на значения из SBox массива
   на входе    :  out_pDst - указатель на вектор который надо трансформировать
   на выходе   :  *
*/
void CIridiumStreebog::S(u8* out_pDst)
{
   for(u8 i = 0; i < STREEBOG_BLOCK_SIZE; i++)
   {
#if defined(IRIDIUM_AVR_PLATFORM)
      out_pDst[i] = pgm_read_byte(&g_aSBox[out_pDst[i]]);
#else
      out_pDst[i] = g_aSBox[out_pDst[i]];
#endif
   }
}

/**
   P и L преобразование
   на входе    :  out_pDst - указатель на вектор который нужно преобразовать
   на выходе   :  *
*/
void CIridiumStreebog::LP(u8* out_pDst)
{
   u8 l_aVal[8];
   u8 l_aTmp[STREEBOG_BLOCK_SIZE];
   memcpy(l_aTmp, out_pDst, STREEBOG_BLOCK_SIZE);

   for(u8 i = 0; i < 8; i++)
   {
      memset(l_aVal, 0, sizeof(l_aVal));

      u8* l_pPtr = l_aTmp;
      u8* l_pTab = (u8*)g_aMatrixA;

      for(u8 j = 0; j < 8; j++)
      {
         u8 l_u8Mask = 0x80;
         for(u8 k = 0; k < 8; k++)
         {
            if(l_pPtr[i] & l_u8Mask)
            {
               for(u8 l = 0; l < 8; l++)
#if defined(IRIDIUM_AVR_PLATFORM)
                  l_aVal[l] ^= pgm_read_byte(&l_pTab[l]);
#else
                  l_aVal[l] ^= l_pTab[l];
#endif
            }
            l_u8Mask >>= 1;
            l_pTab += 8;
         }
         l_pPtr += 8;
      }

      for(u8 j = 0; j < 8; j++)
         out_pDst[i * 8 + j] = l_aVal[j];
   }
}

/**
   Эпсилон преобразование
   на входе    :  in_pDst  - указатель на массив куда нужно поместить результат преобразования
                  in_pK    - указатель на первое значение
                  in_pM    - указатель на второе значение
   на выходе   :  *
*/
void CIridiumStreebog::E(u8* out_pDst, u8* in_pK, u8* in_pM)
{
   u8 l_aK[STREEBOG_BLOCK_SIZE];
   memcpy(l_aK, in_pK, STREEBOG_BLOCK_SIZE);

   Xor512(out_pDst, l_aK, in_pM);

   u8* l_pPtr = (u8*)g_aIterationConstants;
   for(u8 i = 0; i < 12; i++)
   {
      S(out_pDst);
      LP(out_pDst);

      for(u8 j = 0; j < STREEBOG_BLOCK_SIZE; j++)
      {
#if defined(IRIDIUM_AVR_PLATFORM)
         l_aK[j] ^= pgm_read_byte(l_pPtr);
#else
         l_aK[j] ^= *l_pPtr;
#endif
         l_pPtr++;
      }

      S(l_aK);
      LP(l_aK);

      Xor512(out_pDst, l_aK, out_pDst);
   }
}

/**
   Функция сжатия с двумя массивами
   на входе    :  out_pDst - указатель на массив куда нужно поместить результат работы
                  in_pN    - указатель на первый массив
                  in_pM    - указатель на второй массив
   на выходе   :  *
   примечание  :  в случае если массив in_pN будет равен NULL, метод g_N работает как g_0
*/
void CIridiumStreebog::g_N(u8* out_pDst, u8* in_pN, u8* in_pM)
{
   u8 l_aHash[STREEBOG_BLOCK_SIZE];
   memcpy(l_aHash, out_pDst, STREEBOG_BLOCK_SIZE);

   // Проверка наличия массива N
   if(in_pN)
      Xor512(out_pDst, out_pDst, in_pN);

   S(out_pDst);
   LP(out_pDst);

   E(out_pDst, out_pDst, in_pM);

   Xor512(out_pDst, out_pDst, l_aHash);
   Xor512(out_pDst, out_pDst, in_pM);
}

/**
   Вычисление хэша для указанного буфера
   на входе    :  in_pBuffer     - указатель на буфер с данными для которых нужно вычислить хэш
                  in_stSize      - размер буфере для которого нужно вычислить хэш
                  out_pHash      - указатель на буфер куда нужно поместить вычисленный хэш
                  in_stHashSize  - размер буфера куда нужно поместить вычисленный хэш
                  in_eType       - тип хэша, 256 или 512 бит
   на выходе   :  размер полученного буфера, нулевое значение обозначает ошибку
*/
size_t CIridiumStreebog::Calc(const void* in_pBuffer, size_t in_stSize, u8* out_pHash, size_t in_stHashSize, eStreebogHashType in_eType)
{
   size_t l_stResult = 0;
   u8 l_aM[STREEBOG_BLOCK_SIZE];

   // Проверка входных параметров
   if(in_pBuffer && out_pHash)
   {
      // Проверка размеров хэш буфера
      if(((in_eType == SHT_HASH_256) && (in_stHashSize >= STREEBOG_HASH_256_BYTES)) ||
         ((in_eType == SHT_HASH_512) && (in_stHashSize >= STREEBOG_HASH_512_BYTES)))
      {
         // Инициализация массивов перед работой
         memset(out_pHash, in_eType, in_stHashSize);
         memset(m_aN, 0, STREEBOG_BLOCK_SIZE);
         memset(m_aE, 0, STREEBOG_BLOCK_SIZE);

         // Кодирование блоков по 64 байта
         while(in_stSize >= STREEBOG_BLOCK_SIZE)
         {
            memcpy(l_aM, (u8*)in_pBuffer + in_stSize - STREEBOG_BLOCK_SIZE, STREEBOG_BLOCK_SIZE);

            g_N(out_pHash, m_aN, l_aM);

            in_stSize -= STREEBOG_BLOCK_SIZE;

            AddMod512_u8(m_aN, m_aN, STREEBOG_BLOCK_SIZE);
            AddMod512(m_aE, m_aE, l_aM);
         }
         
         // Кодирование блока менее 64 байт
         u8 l_u8Padding = STREEBOG_BLOCK_SIZE - in_stSize;;
         if(l_u8Padding)
         {
            memset(l_aM, 0x00, l_u8Padding - 1);
            memset(l_aM + l_u8Padding - 1, 0x01, 1);
            memcpy(l_aM + l_u8Padding, in_pBuffer, in_stSize);
         }

         g_N(out_pHash, m_aN, l_aM);

         AddMod512_u8(m_aN, m_aN, in_stSize);
         AddMod512(m_aE, m_aE, l_aM);

         g_N(out_pHash, NULL, m_aN);
         g_N(out_pHash, NULL, m_aE);

         // Вернем размер вычисленного хэша
         l_stResult = in_eType ? STREEBOG_BLOCK_SIZE / 2 : STREEBOG_BLOCK_SIZE;
      }
   }
   return l_stResult;
}
