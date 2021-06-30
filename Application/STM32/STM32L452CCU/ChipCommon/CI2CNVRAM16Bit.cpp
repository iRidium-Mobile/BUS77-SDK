/**
   Работа с внешней EEPROM подключеной к микроконтроллеру по шине i2c
   проверялось на следующих микросхемах:
   - ST M24C32 - 4 килобайта
*/
#include "CI2CNVRAM16Bit.h"
#include "stm32l4xx_hal.h"

// Прототипы функций
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
*/
s8 CI2CNVRAM16Bit::Init()
{
   s8 l_s8Result = -1;
   // Чтение заголовка
   if(Read(I2C_NVRAM_HEADER_START, (u8*)&m_Header, sizeof(m_Header)))
   {
      // Вычисление размера банка
      u16 l_u16BankSize = (m_u8Pages * m_u16PageSize - I2C_NVRAM_BANK_START) / 2;

      // Проверка наличия кеша и размеров
      if(m_pCacheBuffer && m_u16CacheSize && m_u16CacheSize < l_u16BankSize)
      {
         // Получение данных для чтения и проверки банка
         u8 l_u8Bank = (I2C_NVRAM_BANK == (m_Header.m_u8BankData & I2C_NVRAM_BANK_MASK)) ? 1 : 0;

         // Проверка банка
         for(u8 i = 0; i < 2; i++)
         {
            // Вычисление CRC16 банка
            if(CalcCRC(I2C_NVRAM_BANK_START + l_u8Bank * l_u16BankSize, m_u16CacheSize, 0xFFFF) == m_Header.m_u16CRC[l_u8Bank])
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
         // Если нет ни одной копии, сохраним текущую
         if(!l_s8Result)
         {
            // Увеличим тестовое значение
            m_pCacheBuffer[0] = m_pCacheBuffer[0] + 1;
            // Сохранение значения
            Save();
            // Получение текущего банка
            u8 l_u8Bank = (I2C_NVRAM_BANK == (m_Header.m_u8BankData & I2C_NVRAM_BANK_MASK)) ? 1 : 0;
            // Вычисление CRC16 банка
            if(CalcCRC(I2C_NVRAM_BANK_START + l_u8Bank * l_u16BankSize, m_u16CacheSize, 0xFFFF) != m_Header.m_u16CRC[l_u8Bank])
               l_s8Result = -1;
            else
               l_s8Result = l_u8Bank + 1;
         }
      }
   }
   return l_s8Result;
}

/**
   Запись данных
   на входе    :  in_u16Address  - адрес данных с которого надо записывать данные
                  in_pBuffer     - указатель на буфер с данными
                  in_u16Size     - размер записываемых данных
   на выходе   :  успешность записи данных
*/
bool CI2CNVRAM16Bit::Read(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
{
   bool l_bResult = true;

   // Вычисление смещения в странице
   u16 l_u16PageOffset = in_u16Address % m_u16PageSize;
   u16 l_u16Probe = 0;

   while(in_u16Size && l_bResult)
   {
      // Вычисление размера данных в странице
      u16 l_u16Size = m_u16PageSize - l_u16PageOffset;
      l_u16PageOffset = 0;
      // Выход за пределы
      if(l_u16Size > in_u16Size)
         l_u16Size = in_u16Size;

      // Чтение данных
      do
      {
         l_bResult = (HAL_OK == HAL_I2C_Mem_Read(m_pInterface, m_u8Address, in_u16Address, I2C_MEMADD_SIZE_16BIT, in_pBuffer, l_u16Size, 1000));
      } while(l_u16Probe-- && !l_bResult);

      // Сдвиг адреса и буфера
      in_u16Address += l_u16Size;
      in_pBuffer += l_u16Size;
      // Уменьшение размера
      in_u16Size -= l_u16Size;
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
bool CI2CNVRAM16Bit::Write(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
{
   bool l_bResult = true;

   // Вычисление смещения в странице
   u16 l_u16PageOffset = in_u16Address % m_u16PageSize;
   u16 l_u16Probe = 0;

   while(in_u16Size && l_bResult)
   {
      // Вычисление размера данных в странице
      u16 l_u16Size = m_u16PageSize - l_u16PageOffset;
      l_u16PageOffset = 0;
      // Выход за пределы
      if(l_u16Size > in_u16Size)
         l_u16Size = in_u16Size;

      // Запись полной страницы
      l_u16Probe = 255;
      do
      {
         l_bResult = (HAL_OK == HAL_I2C_Mem_Write(m_pInterface, m_u8Address, in_u16Address, I2C_MEMADD_SIZE_16BIT, in_pBuffer, l_u16Size, 1000));
         HAL_Delay(10);
      } while(l_u16Probe-- && !l_bResult);
      // Сдвиг адреса и буфера
      in_u16Address += l_u16Size;
      in_pBuffer += l_u16Size;

      // Уменьшение размера
      in_u16Size -= l_u16Size;
   }

   return l_bResult;
}
