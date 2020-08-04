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
 *    Максим Тишков - исходная версия 
 *    Марат Гилязетдинов, Сергей Королёв  - первая версия
 *******************************************************************************/
/**
   Реализация класса для работы с блочным шифром "Кузнечик"
   код основан на коде Максима Тишкова (maxamar@mail.ru)
   https://github.com/MaXaMaR/kuznezhik
*/
#include "CIridiumCipherGrasshopper.h"
#include "time.h"
#include "IridiumBytes.h"

#define ACCESS_128_VALUE_8(key, part) (key->m_au8[part])
#define ACCESS_128_VALUE_16(key, part) (key->m_au16[part])
#define ACCESS_128_VALUE_32(key, part) (key->m_au32[part])
#define ACCESS_128_VALUE_64(key, part) (key->m_au64[part])

#if(BITS == 8)
#define ACCESS_128_VALUE ACCESS_128_VALUE_8
#elif(BITS == 16)
#define ACCESS_128_VALUE ACCESS_128_VALUE_16
#elif(BITS == 32)
#define ACCESS_128_VALUE ACCESS_128_VALUE_32
#elif(BITS == 64)
#define ACCESS_128_VALUE ACCESS_128_VALUE_64
#endif

/**
   Обнуление блока
   на входе    :  in_pDst  - указатель на блок данных
   на выходе   :  *
*/
void zero128(block_128_bit_t* in_pDst)
{
   memset(in_pDst, 0, sizeof(block_128_bit_t));
}

/**
   Копирование блока
   на входе    :  in_pDst  - указатель куда нужно скопировать блок
                  in_pSrc  - указатель откуда копировать блок
   на выходе   :  *
*/
void copy128(block_128_bit_t* in_pDst, const block_128_bit_t* in_pSrc)
{
   memcpy(in_pDst, in_pSrc, sizeof(block_128_bit_t));
}

/**
   Применение операции исключающего ИЛИ к блоку
   на входе    :  in_pDst  - указатель на блок приемник
                  in_pXor  - указатель на блок который нужно наложить
   на выходе   :  *
*/
void xor128(block_128_bit_t* in_pDst, const block_128_bit_t* in_pXor)
{
   for(u8 i = 0; i < BIT_PARTS; i++)
      ACCESS_128_VALUE(in_pDst, i) ^= ACCESS_128_VALUE(in_pXor, i);
}

/**
   Копирование блока и наложение на него операции исключающего ИЛИ
   на входе    :  in_pDst  - указатель куда нужно скопировать блок
                  in_pSrc  - указатель на блок источник
                  in_pXor  - указатель на блок который нужно наложить
   на выходе   :  *
*/
void plus128(block_128_bit_t* in_pDst, const block_128_bit_t* in_pSrc, const block_128_bit_t* in_pXor)
{
   copy128(in_pDst, in_pSrc);
   xor128(in_pDst, in_pXor);
}

/**
   Табличная замена данных
   на входе    :  in_pDst     - указатель на заменяемый блок
                  in_pArray   - указатель на таблицу замены
   на выходе   :  *
*/
void convert128(block_128_bit_t* in_pDst, const u8* in_pArray)
{
   for(u8 i = 0; i < MAX_BIT_PARTS; i++)
#if defined(IRIDIUM_MCU_AVR)
      ACCESS_128_VALUE_8(in_pDst, i) = pgm_read_byte(&in_pArray[ACCESS_128_VALUE_8(in_pDst, i)]);
#else
      ACCESS_128_VALUE_8(in_pDst, i) = in_pArray[ACCESS_128_VALUE_8(in_pDst, i)];
#endif
}

#if(USE_TABLES)

// result & x must be different
void plus128multi(block_128_bit_t* in_pDst, const block_128_bit_t* in_pX, const block_128_bit_t in_Array[][256])
{
   zero128(in_pDst);
   for(u8 i = 0; i < MAX_BIT_PARTS; i++)
      xor128(in_pDst, &in_Array[i][ACCESS_128_VALUE_8(in_pX, i)]);
}

void append128multi(block_128_bit_t* in_pDst, block_128_bit_t* in_pX, const block_128_bit_t in_Array[][256])
{
   plus128multi(in_pDst, in_pX, in_Array);
   copy128(in_pX, in_pDst);
}

// Флаг инициализации таблиц
bool             CIridiumCipherGrasshopper::m_bInitTable = false;
// Статические таблицы для упрощения расчетов шифрования и дешифрования
block_128_bit_t  CIridiumCipherGrasshopper::m_aPILEnc128[MAX_BIT_PARTS][256];
block_128_bit_t  CIridiumCipherGrasshopper::m_aLDec128[MAX_BIT_PARTS][256];
block_128_bit_t  CIridiumCipherGrasshopper::m_aPILDec128[MAX_BIT_PARTS][256];
#endif

/**
   Выполнение операции p(x) = x^8 + x^7 + x^6 + x + 1
   на входе    :  x  - 
                  y  - 
   на выходе   :  результат операции
*/
u8 CIridiumCipherGrasshopper::MulGF256(u8 in_u8X, u8 in_u8Y)
{
   u8 z = 0;

   while(in_u8Y > 0)
   {
      if(in_u8Y & 1)
         z ^= in_u8X;
      in_u8X = (in_u8X << 1) ^ (in_u8X & 0x80 ? 0xC3 : 0x00);
      in_u8Y >>= 1;
   }

   return z;
}

/**
   Операция линейного преобразования
   на входе    :  in_pBlock   - указатель на блок для которого производится линейное преобразование
   на выходе   :  *
*/
void CIridiumCipherGrasshopper::L(block_128_bit_t* in_pBlock)
{
   // 16 раундов
   for(u8 j = 0; j < 16; j++)
   {
      u8 x = in_pBlock->m_au8[15];
      for(s8 i = 14; i >= 0; i--)
      {
         in_pBlock->m_au8[i + 1] = in_pBlock->m_au8[i];
#if defined(IRIDIUM_MCU_AVR)
         x ^= MulGF256(in_pBlock->m_au8[i], pgm_read_byte(&g_aGrasshopperLVec[i]));
#else
         x ^= MulGF256(in_pBlock->m_au8[i], g_aGrasshopperLVec[i]);
#endif
      }
      in_pBlock->m_au8[0] = x;
   }
}

/**
   Операция инверсного линейного преобразования
   на входе    :  in_pBlock   - указатель на блок для которого производится инверсное линейное преобразование
   на выходе   :  *
*/
void CIridiumCipherGrasshopper::InverseL(block_128_bit_t* in_pBlock)
{
   // 16 раундов
   for(u8 j = 0; j < 16; j++)
   {
      u8 x = in_pBlock->m_au8[0];
      for(u8 i = 0; i < 15; i++)
      {
         in_pBlock->m_au8[i] = in_pBlock->m_au8[i + 1];	
#if defined(IRIDIUM_MCU_AVR)
         x ^= MulGF256(in_pBlock->m_au8[i], pgm_read_byte(&g_aGrasshopperLVec[i]));
#else
         x ^= MulGF256(in_pBlock->m_au8[i], g_aGrasshopperLVec[i]);
#endif
      }
      in_pBlock->m_au8[15] = x;
   }
}

/**
   Инициализация шифрования
   на входе    :  out_pCTX - указатель на контекст который надо инициализировать
                  in_pKey  - указатель на массив данных ключа шифрования
   на выходе   :  *
*/
void CIridiumCipherGrasshopper::EncryptInit(grasshopper_context_t* out_pCTX, const u8* in_pKey)
{
   block_128_bit_t c, x, y, z;

   for(u8 i = 0; i < 16; i++)
   {
      // this will be have to changed for little-endian systems
      x.m_au8[i] = in_pKey[i];
      y.m_au8[i] = in_pKey[i + 16];
   }

   copy128(&out_pCTX->m_aKeys[0], &x);
   copy128(&out_pCTX->m_aKeys[1], &y);

   for(u8 i = 1; i <= 32; i++)
   {
      // C Value
      zero128(&c);
      c.m_au8[15] = i;		// load round in lsb
      L(&c);

      plus128(&z, &x, &c);
      convert128(&z, g_aGrasshoperPI);
      L(&z);
      xor128(&z, &y);

      copy128(&y, &x);
      copy128(&x, &z);

      if((i & 7) == 0)
      {
         u8 k = i >> 2;
         copy128(&out_pCTX->m_aKeys[k], &x);
         copy128(&out_pCTX->m_aKeys[k + 1], &y);
      }
   }
}

/**
   Инициализация дешифрования
   на входе    :  out_pCTX - указатель на контекст который надо инициализировать
                  in_pKey  - указатель на массив данных ключа дешифрования
   на выходе   :  *
*/
void CIridiumCipherGrasshopper::DecryptInit(grasshopper_context_t* out_pCTX, const u8* in_pKey)
{
    EncryptInit(out_pCTX, in_pKey);
    
#if(USE_TABLES)
    for(u8 i = 1; i < 10; i++)
        InverseL(&out_pCTX->m_aKeys[i]);
#endif
}

/**
   Кодирование блока
   на входе    :  in_pCTX     - указатель на контекст кодера
                  in_pBlock   - указатель на кодируемый блок
   на выходе   :  *
*/
void CIridiumCipherGrasshopper::EncryptBlock(grasshopper_context_t* in_pCTX, block_128_bit_t* in_pBlock)
{
#if(USE_TABLES)
   block_128_bit_t l_Buffer;
#endif

   for(u8 i = 0; i < 9; i++)
   {
      xor128(in_pBlock, &in_pCTX->m_aKeys[i]);
#if(USE_TABLES)
      append128multi(&l_Buffer, in_pBlock, m_aPILEnc128);
#else
      convert128(in_pBlock, g_aGrasshoperPI);
      L(in_pBlock);
#endif
   }
    
   xor128(in_pBlock, &in_pCTX->m_aKeys[9]);
}

/**
   Декодирование блока
   на входе    :  in_pCTX     - указатель на контекст декодера
                  in_pBlock   - указатель на декодируемый блок
   на выходе   :  *
*/
void CIridiumCipherGrasshopper::DecryptBlock(grasshopper_context_t* in_pCTX, block_128_bit_t* in_pBlock)
{
#if(USE_TABLES)

   block_128_bit_t l_Buffer;
   append128multi(&l_Buffer, in_pBlock, m_aLDec128);

   for(s8 i = 9; i > 1; i--)
   {
      xor128(in_pBlock, &in_pCTX->m_aKeys[i]);
      append128multi(&l_Buffer, in_pBlock, m_aPILDec128);
   }

   xor128(in_pBlock, &in_pCTX->m_aKeys[1]);
   convert128(in_pBlock, g_aGrasshopperPIInv);
   xor128(in_pBlock, &in_pCTX->m_aKeys[0]);

#else

   xor128(in_pBlock, &in_pCTX->m_aKeys[9]);

   for(s8 i = 8; i >= 0; i--)
   {
      InverseL(in_pBlock);
      convert128(in_pBlock, g_aGrasshopperPIInv);
      xor128(in_pBlock, &in_pCTX->m_aKeys[i]);
   }

#endif
}

/**
   Конструктор класса
   на входе    :  *
*/
CIridiumCipherGrasshopper::CIridiumCipherGrasshopper() : CIridiumCipher()
{
   // Подготовка контектов кодирования и декодирования
   memset(&m_ECTX, 0, sizeof(grasshopper_context_t));
   memset(&m_DCTX, 0, sizeof(grasshopper_context_t));

#if(USE_TABLES)
   // Проверка инициализации таблиц декодирования
   if(!m_bInitTable)
   {
      // Подготовка таблиц к заполнению
      memset(m_aPILEnc128, 0, sizeof(m_aPILEnc128));
      memset(m_aLDec128, 0, sizeof(m_aLDec128));
      memset(m_aPILDec128, 0, sizeof(m_aPILDec128));

      // Формирование статических таблиц для упрощения кодирования/декодирования
      for(int j = 0; j < 256; j++)
      {
         for(int i = 0; i < MAX_BIT_PARTS; i++)
         {
            // Таблица для кодирования
            m_aPILEnc128[i][j].m_au8[i] = g_aGrasshoperPI[j];
            L(&m_aPILEnc128[i][j]);
            // Таблица для декодирования
            m_aLDec128[i][j].m_au8[i] = j;
            InverseL(&m_aLDec128[i][j]);
            // Таблица для декодирования инверсная
#if defined(IRIDIUM_MCU_AVR)
            m_aPILDec128[i][j].m_au8[i] = pgm_read_byte(&g_aGrasshopperPIInv[j]);
#else
            m_aPILDec128[i][j].m_au8[i] = g_aGrasshopperPIInv[j];
#endif
            InverseL(&m_aPILDec128[i][j]);
         }
      }
      m_bInitTable = true;
   }
#endif
}

/**
   Деструктор класса
*/
CIridiumCipherGrasshopper::~CIridiumCipherGrasshopper()
{
}

/**
   Инициализация кодера и декодера блочного шифра в соответствии с установленными параметрами
   на входе    :  in_pData - указатель на данные которые используются для генерации ключа и вектора инициализации
   на выходе   :  успешность инициализации
   примечание  :  размер данных должен быть не менее BLOCK_CIPHER_KEY_SIZE + BLOCK_CIPHER_SIZE
*/
bool CIridiumCipherGrasshopper::Init(const u8* in_pData)
{
   bool l_bResult = false;

   // Проверка наличия ключа
   if(in_pData)
   {
      // Инициализация контекстов шифрования и дешифрования
      EncryptInit(&m_ECTX, in_pData);
      DecryptInit(&m_DCTX, in_pData);

      // Инициализация векторов шифрования
#if defined(IRIDIUM_ENABLE_IV)
      if(m_bEnableIV)
      {
         memcpy(m_aEncodeIV, in_pData + BLOCK_CIPHER_KEY_SIZE, BLOCK_CIPHER_SIZE);
         memcpy(m_aDecodeIV, in_pData + BLOCK_CIPHER_KEY_SIZE, BLOCK_CIPHER_SIZE);
      }
#endif
      l_bResult = true;
   }

   return l_bResult;
}

/**
   Шифрование данных
   на входе    :  in_pBuffer     - указатель на буфер с данными для шифрования
                  in_stSize      - размер буфера с данными для шифрования
                  out_rMaxSize   - ссылка на перемунную которая содержит максимальный размер буфера который можно
                                   использовать при шифровании. При успешном кодировании буфера, переменная содержит
                                   результирующую длинну зашифрованных данных
   на выходе   :  успешность шифрования
*/
bool CIridiumCipherGrasshopper::Encode(u8* in_pBuffer, size_t in_stSize, size_t& out_rMaxSize)
{
   bool l_bResult = false;
   size_t l_stSize = 0;
   u8* l_pSrc = in_pBuffer;
   u8* l_pDst = in_pBuffer;
   block_128_bit_t l_Block;
   u8 l_aIV[BLOCK_CIPHER_SIZE];

   // Очистка вектора инициализации
   memset(l_aIV, 0, sizeof(l_aIV));

   // Проверка входных параметров
   if(in_pBuffer && in_stSize && out_rMaxSize)
   {
      // Проверка хватит ли места в исходящем буфере
      if(out_rMaxSize >= ((in_stSize + (BLOCK_CIPHER_SIZE - 1)) & ~0xF))
      {
         // Получение вектора инициализации
#if defined(IRIDIUM_ENABLE_IV)
         if(m_bEnableIV)
            memcpy(l_aIV, m_aEncodeIV, BLOCK_CIPHER_SIZE);
#endif
         // Поблочное шифрование
         while(in_stSize)
         {
            // Вычисление размера блока
            l_stSize = (in_stSize < BLOCK_CIPHER_SIZE) ? in_stSize : BLOCK_CIPHER_SIZE;
            // Занесение в блок открытого текста
            memcpy(&l_Block, l_pSrc, l_stSize);
            // Заполнение "хвоста" если блок не полный
            if(l_stSize < BLOCK_CIPHER_SIZE)
               memset((u8*)&l_Block + l_stSize, BLOCK_CIPHER_SIZE - l_stSize, BLOCK_CIPHER_SIZE - l_stSize);
            // Наложение вектора инициализации
            for(u8 i = 0; i < BLOCK_CIPHER_SIZE; i++)
               l_Block.m_au8[i] ^= l_aIV[i];
            // Кодирование блока
            EncryptBlock(&m_ECTX, &l_Block);
            // Помещение закодированных данных в буфер
            memcpy(l_pDst, &l_Block, BLOCK_CIPHER_SIZE);
            // Обновление вектора инициализации
            memcpy(l_aIV, l_pDst, BLOCK_CIPHER_SIZE);
            // Сдвиг позиции и уменьшение размера данных
            l_pDst += BLOCK_CIPHER_SIZE;
            l_pSrc += l_stSize;
            in_stSize -= l_stSize;
         }
         // Сохранение вектора инициализации
#if defined(IRIDIUM_ENABLE_IV)
         if(m_bEnableIV)
            memcpy(m_aEncodeIV, l_aIV, BLOCK_CIPHER_SIZE);
#endif
         out_rMaxSize = l_pDst - in_pBuffer;
         l_bResult = true;
      }
   }
   return l_bResult;
}

/**
   Расшифровка данных
   на входе    :  in_pBuffer  - указатель на буфер с данными для расшифровки
                  in_stSize   - размер буфера с данными для расшифровки
   на выходе   :  успешность расшифровывания
*/
bool CIridiumCipherGrasshopper::Decode(u8* in_pBuffer, size_t in_stSize)
{
   bool l_bResult = false;
   u8* l_pSrc = in_pBuffer;
   u8* l_pDst = in_pBuffer;
   block_128_bit_t l_Block;
   u8 l_aIV[BLOCK_CIPHER_SIZE];

   // Очистка вектора инициализации
   memset(l_aIV, 0, sizeof(l_aIV));

   // Проверка входных параметров
   if(in_pBuffer && in_stSize)
   {
      // Проверка что входные данные кратны размеру блока
      if(!(in_stSize % BLOCK_CIPHER_SIZE))
      {
         // Получение вектора инициализации
#if defined(IRIDIUM_ENABLE_IV)
         if(m_bEnableIV)
            memcpy(l_aIV, m_aDecodeIV, BLOCK_CIPHER_SIZE);
#endif
         // Поблочная расшифровка
         while(in_stSize)
         {
            // Занесение в блок зашифрованного текста
            memcpy(&l_Block, l_pSrc, BLOCK_CIPHER_SIZE);
            // Декодирование блока
            DecryptBlock(&m_DCTX, &l_Block);
            // Наложение вектора инициализации
            for(u8 i = 0; i < BLOCK_CIPHER_SIZE; i++)
               l_Block.m_au8[i] ^= l_aIV[i];
            // Обновление вектора инициализации
            memcpy(l_aIV, l_pSrc, BLOCK_CIPHER_SIZE);
            // Помещение закодированных данных в буфер
            memcpy(l_pDst, &l_Block, BLOCK_CIPHER_SIZE);
            // Сдвиг позиции и уменьшение размера данных
            l_pDst += BLOCK_CIPHER_SIZE;
            l_pSrc += BLOCK_CIPHER_SIZE;
            in_stSize -= BLOCK_CIPHER_SIZE;
         }
         // Сохранение вектора инициализации
#if defined(IRIDIUM_ENABLE_IV)
         if(m_bEnableIV)
            memcpy(m_aDecodeIV, l_aIV, BLOCK_CIPHER_SIZE);
#endif
         l_bResult = true;
      }
   }
   return l_bResult;
}

#if 0
//////////////////////////////////////////////////////////////////////////
// Тест от автора
//////////////////////////////////////////////////////////////////////////
const u8 test_key[32] =
{
   0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
   0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
   0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
   0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
};

const u8 test_pt[16] =
{
   0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00,
   0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88
};

const u8 test_ct[16] =
{
   0x7F, 0x67, 0x9D, 0x90, 0xBE, 0xBC, 0x24, 0x30,
   0x5A, 0x46, 0x8D, 0x42, 0xB9, 0xD4, 0xED, 0xCD
};

//////////////////////////////////////////////////////////////////////////
// ГОСТ тест
//////////////////////////////////////////////////////////////////////////
const u8 gost_key[32] = 
{
   0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
   0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
   0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
   0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
};

// значение открытого текста
const u8 gost_pt[] = 
{
   0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00,
   0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88
}; 

// значение шифротекста
const u8 gost_ct[] = 
{
   0x7f, 0x67, 0x9d, 0x90, 0xbe, 0xbc, 0x24, 0x30,
   0x5a, 0x46, 0x8d, 0x42, 0xb9, 0xd4, 0xed, 0xcd
}; 

void print_w128(block_128_bit_t* in_pBlock)
{
   int i;

   for(i = 0; i < 16; i++)
      printf(" %02X", in_pBlock->m_au8[i]);
   printf("\n");
} 

void CIridiumGrasshopper::self_test(const u8* in_pPT, const u8* in_pCT)
{
   block_128_bit_t x;

   Init(NULL);

   printf("Self-test:\n");

   for(int i = 0; i < 10; i++)
   {
      printf("K_%d\t=", i + 1);
      print_w128(&m_ECTX.m_aKeys[i]);
   }

   for(int i = 0; i < 16; i++)
      x.m_au8[i] = in_pPT[i];
   printf("PT\t=");
   print_w128(&x);

   EncryptBlock(&m_ECTX, &x);

   printf("CT\t=");
   print_w128(&x);

   for(int i = 0; i < 16; i++)
   {
      if(in_pCT[i] != x.m_au8[i])
      {
         fprintf(stderr, "Encryption self-test failure.\n");
         return;// (-1);
         //exit(-1);
      }
   }

   DecryptBlock(&m_DCTX, &x);

   printf("PT\t=");
   print_w128(&x);

   for (int i = 0; i < 16; i++)
   {
      if(in_pPT[i] != x.m_au8[i])
      {
         fprintf(stderr, "Decryption self-test failure.\n");
         return;// (-2);
         //exit(-2);
      }
   }

   printf("Self-test OK!\n");
} 

/**
   Тесты шифрования и дешифрования
*/
bool CIridiumGrasshopper::UnitTests()
{
   {
      printf("Config: bits: %d, use tables: %d\n", BITS, USE_TABLES);

      grasshopper_context_t key;
      u32 buf[0x100];

      SetKey(gost_key);
      self_test(gost_pt, gost_ct);
      SetKey(test_key);
      self_test(test_pt, test_ct);

      // == Speed Test ==

      for(int i = 0; i < 0x100; i++)
         buf[i] = i;

      EncryptInit(&key, gost_key);
      /*
      for(int n = 4000, tim = 0; tim < 2 * CLOCKS_PER_SEC; n <<= 1) {
         tim = clock();
         for (int j = 0; j < n; j++) {
            for (int i = 0; i < 0x100; i += 4)
               EncryptBlock(&key, buf[i]);
         }
         tim = clock() - tim;
         printf("kuz_encrypt_block(): %.3f kB/s (n=%dkB,t=%.3fs)\r",
            ((double) CLOCKS_PER_SEC * n) / ((double) tim),
            n, ((double) tim) / ((double) CLOCKS_PER_SEC));
         fflush(stdout);
      }
      printf("\n");
      */

      for (int i = 0; i < 0x100; i++)
         buf[i] = i;
      DecryptInit(&key, gost_key);
      /*
      for (int n = 4000, tim = 0; tim < 2 * CLOCKS_PER_SEC; n <<= 1) {
         tim = clock();
         for (int j = 0; j < n; j++) {
            for (int i = 0; i < 0x100; i += 4)
               DecryptBlock(&key, buf[i]);
         }
         tim = clock() - tim;
         printf("kuz_decrypt_block(): %.3f kB/s (n=%dkB,t=%.3fs)\r",
            ((double) CLOCKS_PER_SEC * n) / ((double) tim), 
            n, ((double) tim) / ((double) CLOCKS_PER_SEC));
         fflush(stdout);
      }
      printf("\n");
      */

      //printf("Tables: %.1fK (encrypt: %.1fK, decrypt: %.1fK)\n", (float)get_used_memory_count() / 1024, (float)get_encrypt_used_memory_count() / 1024, (float)get_decrypt_used_memory_count() / 1024);
   }
   return true;
}
#endif
