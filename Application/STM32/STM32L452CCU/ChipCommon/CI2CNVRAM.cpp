#include "CI2CNVRAM.h"

// Прототипы функций
extern "C" u16 GetCRC16Modbus(u16 in_u16Init, const u8* in_pBuffer, size_t in_stSize);

/**
   Конструктор класса
   на выходе   :  *
*/
CI2CNVRAM::CI2CNVRAM() : CAbstractNVRAM()
{
   m_pInterface = NULL;
   m_u8Address = 0;

   memset(&m_Header, 0, sizeof(m_Header));
   m_pCacheBuffer = NULL;
   m_u16CacheSize = 0;
   m_u8Flags = 0;

   m_u8Pages = 0;
   m_u16PageSize = 0;
}

/**
   Деструктор класса
*/
CI2CNVRAM::~CI2CNVRAM()
{
}

/**
   Установка параметров памяти
   на входе    :  in_u16MemorySize  - размер размера
                  in_u16PageSize    - размер страницы
   на выходе   :  *
*/
void CI2CNVRAM::Setup(u16 in_u16MemorySize, u16 in_u16PageSize)
{
   m_u8Pages = in_u16MemorySize / in_u16PageSize;
   m_u16PageSize = in_u16PageSize;
}

/**
   Принудительная запись
   на входе    :  *
   на выходе   :  успешность записи данных
*/
bool CI2CNVRAM::Save()
{
   bool l_bResult = false;

   // Сменим банк
   m_Header.m_u8BankData ^= I2C_NVRAM_BANK;

   // Вычисление банка в который надо записать данные
   u8 l_u8Bank = (I2C_NVRAM_BANK == (m_Header.m_u8BankData & I2C_NVRAM_BANK_MASK)) ? 1 : 0;
   u16 l_u16BankSize = ((m_u8Pages * m_u16PageSize) - I2C_NVRAM_BANK_START) / 2;
   u16 l_u16Index = I2C_NVRAM_BANK_START + l_u8Bank * l_u16BankSize;

   // Запись данных банка
   l_bResult = Write(l_u16Index, m_pCacheBuffer, m_u16CacheSize);
   // Вычисление контрольной суммы
   m_Header.m_u16CRC[l_u8Bank] = GetCRC16Modbus(0xFFFF, m_pCacheBuffer, m_u16CacheSize);
   // Запись заголовка
   if(l_bResult)
      l_bResult = Write(I2C_NVRAM_HEADER_START, (u8*)&m_Header, I2C_NVRAM_HEADER_SIZE);

   return l_bResult;
}

/**
   Проверка контрольной суммы
   на входе    :  in_u16Start - начальный индекс данных
                  in_u16Size  - размер данных
                  in_u16Init  - начальное значение CRC16
   на выходе   :  контрольная сумма
*/
u16 CI2CNVRAM::CalcCRC(u16 in_u16Start, u16 in_u16Size, u16 in_u16Init)
{
   // Промежуточный буфер
   u8 l_aBuffer[32];
   u16 l_u16Result = in_u16Init;
   u16 l_u16Size = 0;

   // Постепенное вычисление CRC16
   while(in_u16Size)
   {
      // Вычисление размера блока
      l_u16Size = sizeof(l_aBuffer);
      if(in_u16Size < l_u16Size)
         l_u16Size = in_u16Size;

      // Заполнение буфера
      if(!Read(in_u16Start, l_aBuffer, l_u16Size))
         break;

      // Вычисление контрольной суммы блока
      l_u16Result = GetCRC16Modbus(l_u16Result, l_aBuffer, l_u16Size);

      // Сдвиг позиции и уменьшение оставшегося места
      in_u16Start += l_u16Size;
      in_u16Size -= l_u16Size;
   }
   // Вернем результат
   return l_u16Result;
}
