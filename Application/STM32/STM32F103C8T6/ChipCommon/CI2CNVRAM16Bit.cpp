#include "CI2CNVRAM16Bit.h"

// Прототипы функций
extern "C" u32 HAL_GetTick();
extern "C" u16 GetCRC16Modbus(u16 in_u16Init, const u8* in_pBuffer, size_t in_stSize);

/**
   Конструктор класса
   на выходе   :  *
*/
CI2CNVRAM16Bit::CI2CNVRAM16Bit() : CI2CNVRAM()
{
   memset(&m_Header, 0, sizeof(m_Header));
   m_pCacheBuffer = NULL;
   m_u16CacheSize = 0;
   m_u8Flags = 0;
}

/**
   Деструктор класса
*/
CI2CNVRAM16Bit::~CI2CNVRAM16Bit()
{
}

/**
   Инициализация EEPROM
   на входе    :  *
   на выходе   :  успешность чтения данных
                  > 0   - данные прочитаны, значение содержит номер банка 1 или 2
                  = 0   - все банки не содержат данных
                  < 0   - аппаратная проблема
   примечание  :  Структура данных
*/
s8 CI2CNVRAM16Bit::Init()
{
   s8 l_s8Result = -1;

   // Чтение заголовка
   if(Read16(I2C_NVRAM_HEADER_START, (u8*)&m_Header, sizeof(m_Header)))
   {
      // Получение размера банка
      u8 l_u8Size = m_Header.m_u8BankData & I2C_NVRAM_SIZE_MASK;
      if(l_u8Size < 7 || l_u8Size > 15)
      {
         // Определение размера EEPROM
         l_u8Size = 7;
         do
         {
            // Проверка наличия адреса
            l_s8Result = IsAddress(1 << l_u8Size);
            if(l_s8Result > 0)
               l_u8Size++;
            else
               break;

         } while(l_u8Size <= 15);

         // Отформатируем заголовок
         m_Header.m_u8BankData = l_u8Size;
         m_Header.m_u16Size = 0;
         m_Header.m_u16CRC[0] = 0;
         m_Header.m_u16CRC[1] = 0;

         // Запишем отформатированный заголовок
         Write16(I2C_NVRAM_HEADER_START, (u8*)&m_Header, sizeof(m_Header));
      }

      // Вычисление размера банка
      u16 l_u16BankSize = ((1 << l_u8Size) - I2C_NVRAM_BANK_START) / 2;

      // Проверка наличия кеша и размеров
      if(m_pCacheBuffer && m_u16CacheSize && m_u16CacheSize < l_u16BankSize)
      {
         // Получение данных для чтения и проверки банка
         u8 l_u8Bank = (I2C_NVRAM_BANK == (m_Header.m_u8BankData & I2C_NVRAM_BANK_MASK)) ? 1 : 0;

         // Проверка банка
         for(u8 i = 0; i < 2; i++)
         {
            // Вычисление CRC16 банка
            if(CalcCRC(I2C_NVRAM_BANK_START + l_u8Bank * l_u16BankSize, m_Header.m_u16Size, 0xFFFF) == m_Header.m_u16CRC[l_u8Bank])
            {
               // Чтение данных
               if(!Read16(I2C_NVRAM_BANK_START + l_u8Bank * l_u16BankSize, m_pCacheBuffer, m_u16CacheSize))
                  break;
               // Найден банк с проверенными данными
               l_s8Result = l_u8Bank + 1;
               break;

            } else
            {
               // Банк содержит битые данные
               l_s8Result = 0;
               l_u8Bank = (l_u8Bank + 1) & 1;
            }
         }
      }
   }
   return l_s8Result;
}

/**
   Чтение данных
   на входе    :  in_u16Address  - адрес данных с которого надо читать данные
                  in_pBuffer     - указатель на буфер с данными
                  in_u16Size     - размер читаемых данных
   на выходе   :  успешность чтения данных
*/
bool CI2CNVRAM16Bit::Read(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
{
   return Read16(in_u16Address, in_pBuffer, in_u16Size);
}

/**
   Запись данных
   на входе    :  in_u16Address  - адрес данных с которого надо записывать данные
                  in_pBuffer     - указатель на буфер с данными
                  in_u16Size     - размер записываемых данных
   на выходе   :  успешность записи данных
*/
bool CI2CNVRAM16Bit::Write(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
{
   return Write16(in_u16Address, in_pBuffer, in_u16Size);
}

/**
   Проверка наличия адреса
   на выходе   :  > 0   - адрес существует
                  = 0   - адрес не существует
                  < 0   - аппаратная ошибка
*/
s8 CI2CNVRAM16Bit::IsAddress(u16 in_u16Address)
{
   s8 l_s8Result = -1;
   u8 l_u8V0 = 0;
   u8 l_u8VN = 0;

   // Чтение значения по 0 и указанному адресу
   if(Read16(0, &l_u8V0, sizeof(l_u8V0)) && Read16(in_u16Address, &l_u8VN, sizeof(l_u8VN)))
   {
      // Проверим отличаются ли значения, если значения отличаются, значит адрес существует
      if(l_u8V0 == l_u8VN)
      {
         // Если значения одинаковые, попробуем изменить значение в 0 адресе
         l_u8V0++;
         if(Write16(0, &l_u8V0, sizeof(l_u8V0)) && Read16(in_u16Address, &l_u8VN, sizeof(l_u8VN)))
         {
            // Проверим отличаются ли значения, если значения отличаются, значит адрес существует
            l_s8Result = (l_u8V0 == l_u8VN) ? 0 : 1;

            // Вернем значение в 0 адресе
            l_u8V0--;
            if(!Write16(0, &l_u8V0, sizeof(l_u8V0)))
               l_s8Result = -1;
         }
      } else
         l_s8Result = 1;
   }

   return l_s8Result;
}

/**
   Проверка контрольной суммы
   на входе    :  in_u16Start - начальный индекс данных
                  in_u16Size  - размер данных
   на выходе   :  контрольная сумма
*//*
u16 CI2CNVRAM16Bit::CalcCRC(u16 in_u16Index, u16 in_u16Size, u16 in_u16CRC)
{
   // Промежуточный буфер
   u8 l_aBuffer[8];
   u16 l_u16Result = in_u16CRC;
   u16 l_u16Size = 0;

   // Постепенное вычисление CRC16
   while(in_u16Size)
   {
      // Вычисление размера блока
      l_u16Size = sizeof(l_aBuffer);
      if(in_u16Size < l_u16Size)
         l_u16Size = in_u16Size;

      // Заполнение буфера
      if(!Read16(in_u16Index, l_aBuffer, l_u16Size))
         break;

      // Вычисление контрольной суммы блока
      l_u16Result = GetCRC16Modbus(l_u16Result, l_aBuffer, l_u16Size);

      // Сдвиг позиции и уменьшение оставшегося места
      in_u16Index += l_u16Size;
      in_u16Size -= l_u16Size;
   }
   // Вернем результат
   return l_u16Result;
}*/
