#ifndef _C_I2C_NVRAM_H_INCLUDED_
#define _C_I2C_NVRAM_H_INCLUDED_

#include "CI2C.h"

// Флаги и маски
#define  I2C_NVRAM_BANK          0x80              // Флаг второго банка
#define  I2C_NVRAM_BANK_MASK     0x80              // Макса отделения флага банка
#define  I2C_NVRAM_SIZE_MASK     0x7F              // Макска отделения размера банка

#define  I2C_NVRAM_NEED_SAVE     0x80              // Флаг указывающий что нужно сохранить значение кеша в энергонезависимую память

// Карта данных i2c eeprom
#define  I2C_NVRAM_HEADER_START  2                 // Первые 2 байт зарезервированы для автоопределения размера EEPROM
#define  I2C_NVRAM_HEADER_SIZE   14                // Зарезервированный размер заголовка
#define  I2C_NVRAM_BANK_START    (I2C_NVRAM_HEADER_START + I2C_NVRAM_HEADER_SIZE)

// Структура заголовка энергонезависимой памяти
typedef struct i2c_nvram_header_s
{
   u8    m_u8BankData;                             // Флаг и размер банка
   u16   m_u16Size;                                // Размер данных в энергонезависимой памяти
   u16   m_u16CRC[2];                              // Контрольные суммы банков
} i2c_nvram_header_t;

// Класс для работы с энергонезависимой памятью, работающей через I2C интерфейс
class CI2CNVRAM : public CI2C
{
public:
   // Конструктор/деструктор
   CI2CNVRAM();
   virtual ~CI2CNVRAM();

   // Установка буфера кеширования
   void SetCache(void* in_pBuffer, u16 in_u16Size);
   void SetUpdateTime(u32 in_u32Time)
      { m_u32UpdateTime = in_u32Time; }

   // Установка флага сохранения данных
   void NeedSave()
      { m_u8Flags |= I2C_NVRAM_NEED_SAVE; }
   // Принудительное сохранение данных
   void ForceSave();

   // Обработка
   void Work();

   ////////////////////////////////////////////////////////////////////////////
   // Перегруженные методы
   ////////////////////////////////////////////////////////////////////////////
   // Инициализация EEPROM
   virtual s8 Init()
      { return -1; }

   // Методы для чтения и записи данных в энергонезависимую память
   virtual bool Read(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
      { return false; }
   virtual bool Write(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
      { return false; }

protected:
   u16 CalcCRC(u16 in_u16Index, u16 in_u16Size, u16 in_u16CRC);

   i2c_nvram_header_t   m_Header;                  // Флаг и размер банка
   u8                   m_u8Flags;                 // Список флагов
   u8*                  m_pCacheBuffer;            // Указатель на буфер кеша
   u16                  m_u16CacheSize;            // Размер кеша
   u32                  m_u32UpdateTime;           // Время между записью данных
   u32                  m_u32OldTime;              // Время последнего сохранения
};

#endif   // _C_I2C_NVRAM_H_INCLUDED_
