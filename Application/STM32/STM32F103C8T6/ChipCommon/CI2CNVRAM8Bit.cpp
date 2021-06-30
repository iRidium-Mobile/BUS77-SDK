#include "CI2CNVRAM8Bit.h"

// Прототипы функций
extern "C" u32 HAL_GetTick();
extern "C" u16 GetCRC16Modbus(u16 in_u16Init, const u8* in_pBuffer, size_t in_stSize);

/**
   Микросхемы которые имеют 8 битную адресацию
   Запись данных
*/

/**
   Конструктор класса
   на выходе   :  *
*/
CI2CNVRAM8Bit::CI2CNVRAM8Bit() : CI2CNVRAM()
{
}

/**
   Деструктор класса
*/
CI2CNVRAM8Bit::~CI2CNVRAM8Bit()
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
s8 CI2CNVRAM8Bit::Init()
{
   s8 l_s8Result = -1;
   // Чтение заголовка
   if(Read(I2C_NVRAM_HEADER_START, (u8*)&m_Header, sizeof(m_Header)))
   {
      // Получение размера банка
      u8 l_u8Size = m_Header.m_u8BankData & I2C_NVRAM_SIZE_MASK;
      if(l_u8Size < 7 || l_u8Size > 15)
      {
         // Определение размера EEPROM
         l_u8Size = m_u8Bits;

         // Отформатируем заголовок
         m_Header.m_u8BankData = l_u8Size;
         m_Header.m_u16Size = 0;
         m_Header.m_u16CRC[0] = 0;
         m_Header.m_u16CRC[1] = 0;

         // Запишем отформатированный заголовок
         Write(I2C_NVRAM_HEADER_START, (u8*)&m_Header, sizeof(m_Header));
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
               if(!Read(I2C_NVRAM_BANK_START + l_u8Bank * l_u16BankSize, m_pCacheBuffer, m_u16CacheSize))
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
bool CI2CNVRAM8Bit::Read(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
{
   bool l_bResult = true;

   u16 l_u16Need = in_u16Size;
   u16 l_u16Address = in_u16Address;
   u16 l_u16Probe = 0;

   while(l_u16Need && l_bResult)
   {
      // Вычисление номера страницы
      u8 l_u8Block = (l_u16Address >> 8) << 1;
      // Вычисление индекса в странице
      u16 l_u16Index = l_u16Address & 255;
      // Вычисление размера данных в странице
      u16 l_u16Size = 256 - l_u16Index;
      // Выход за пределы
      if(l_u16Size > l_u16Need)
         l_u16Size = l_u16Need;
      // Чтение данных
      l_u16Probe = 255;
      do
      {
         l_bResult = (HAL_I2C_Mem_Read(m_pInterface, m_u8Address | l_u8Block, l_u16Index, I2C_MEMADD_SIZE_8BIT, in_pBuffer, l_u16Size, 1000) == HAL_OK);
      } while(l_u16Probe-- && !l_bResult);
      // Сдвиг адреса
      l_u16Address += l_u16Size;
      in_pBuffer += l_u16Size;
      // Уменьшение размера
      l_u16Need -= l_u16Size;
   }

   return l_bResult;
}

/**
   Запись данных
   на входе    :  in_u16Address  - адрес данных с которого надо записывать данные
                  in_pBuffer     - указатель на буфер с данными
                  in_u16Size     - размер записываемых данных
   на выходе   :  успешность записи данных
*/
bool CI2CNVRAM8Bit::Write(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
{
   bool l_bResult = true;

   u16 l_u16Need = in_u16Size;
   u16 l_u16Address = in_u16Address;
   u16 l_u16Probe = 0;

   while(l_u16Need && l_bResult)
   {
      // Вычисление номера блока
      u8 l_u8Block = (l_u16Address >> 8) << 1;
      // Вычисление индекса в странице
      u16 l_u16Index = l_u16Address & 0xFF;
      // Вычисление размера данных в странице
      u16 l_u16Size = 16 - (l_u16Address & 0x0F);
      // Проверим размер записанных данных
      if(l_u16Size > l_u16Need)
         l_u16Size = l_u16Need;
      // Запись страницы
      l_u16Probe = 255;
      do
      {
         l_bResult = (HAL_I2C_Mem_Write(m_pInterface, m_u8Address | l_u8Block, l_u16Index, I2C_MEMADD_SIZE_8BIT, in_pBuffer, l_u16Size, 1000) == HAL_OK);
      } while(l_u16Probe-- && !l_bResult);

      // Сдвиг адреса
      l_u16Address += l_u16Size;
      in_pBuffer += l_u16Size;
      // Уменьшение размера
      l_u16Need -= l_u16Size;
   }

   return l_bResult;
}
