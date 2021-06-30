#include "CI2CNVRAM.h"

// Прототипы функций
extern "C" u32 HAL_GetTick();
extern "C" u16 GetCRC16Modbus(u16 in_u16Init, const u8* in_pBuffer, size_t in_stSize);

/**
   Конструктор класса
   на выходе   :  *
*/
CI2CNVRAM::CI2CNVRAM() : CI2C()
{
   memset(&m_Header, 0, sizeof(m_Header));
   m_pCacheBuffer = NULL;
   m_u16CacheSize = 0;
   m_u8Flags = 0;
}

/**
   Деструктор класса
*/
CI2CNVRAM::~CI2CNVRAM()
{
}

/**
   Установка кеширования
   на входе    :  in_pBuffer  - указатель на буфер кеширования
                  in_u16Size  - размер кеша
   на выходе   :  *
*/
void CI2CNVRAM::SetCache(void* in_pBuffer, u16 in_u16Size)
{
   m_pCacheBuffer = (u8*)in_pBuffer;
   m_u16CacheSize = in_u16Size;
}

/**
   Принудительная запись
   на входе    :  *
   на выходе   :  *
*/
void CI2CNVRAM::ForceSave()
{
   // Сменим банк
   m_Header.m_u8BankData ^= I2C_NVRAM_BANK;

   // Проверка размера данных
   if(m_Header.m_u16Size < m_u16CacheSize)
      m_Header.m_u16Size = m_u16CacheSize;

   // Вычисление банка в который надо записать данные
   u8 l_u8Bank = (I2C_NVRAM_BANK == (m_Header.m_u8BankData & I2C_NVRAM_BANK_MASK)) ? 1 : 0;
   u8 l_u8Size = m_Header.m_u8BankData & I2C_NVRAM_SIZE_MASK;
   u16 l_u16BankSize = ((1 << l_u8Size) - I2C_NVRAM_BANK_START) / 2;
   u16 l_u16Index = I2C_NVRAM_BANK_START + l_u8Bank * l_u16BankSize;

   // Запись банка
   Write(l_u16Index, (u8*)m_pCacheBuffer, m_u16CacheSize);

   // Вычисление CRC для кеша
   m_Header.m_u16CRC[l_u8Bank] = GetCRC16Modbus(0xFFFF, (const u8*)m_pCacheBuffer, m_u16CacheSize);

   // Если размер кеша меньше чем размер блока
   if(m_u16CacheSize < m_Header.m_u16Size)
      m_Header.m_u16CRC[l_u8Bank] = CalcCRC(l_u16Index + m_u16CacheSize, m_Header.m_u16Size - m_u16CacheSize, m_Header.m_u16CRC[l_u8Bank]);

   // Запись заголовка
   Write(I2C_NVRAM_HEADER_START, (u8*)&m_Header, I2C_NVRAM_HEADER_SIZE);
}

/**
   Принудительная запись
   на входе    :  *
   на выходе   :  *
*/
void CI2CNVRAM::Work()
{
   // Проверка нужность записи и время обновления записи
   if((m_u8Flags & I2C_NVRAM_NEED_SAVE) && (HAL_GetTick() - m_u32OldTime) > m_u32UpdateTime)
   {
      // Обновим время
      m_u32OldTime = HAL_GetTick();

      // Запишем данные в EEPROM
      ForceSave();

      // Сброс флага
      m_u8Flags &= ~I2C_NVRAM_NEED_SAVE;
   }
}

/**
   Проверка контрольной суммы
   на входе    :  in_u16Start - начальный индекс данных
                  in_u16Size  - размер данных
   на выходе   :  контрольная сумма
*/
u16 CI2CNVRAM::CalcCRC(u16 in_u16Index, u16 in_u16Size, u16 in_u16CRC)
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
      if(!Read(in_u16Index, l_aBuffer, l_u16Size))
         break;

      // Вычисление контрольной суммы блока
      l_u16Result = GetCRC16Modbus(l_u16Result, l_aBuffer, l_u16Size);

      // Сдвиг позиции и уменьшение оставшегося места
      in_u16Index += l_u16Size;
      in_u16Size -= l_u16Size;
   }
   // Вернем результат
   return l_u16Result;
}
