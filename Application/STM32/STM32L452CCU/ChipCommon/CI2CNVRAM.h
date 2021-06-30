#ifndef _C_I2C_NVRAM_H_INCLUDED_
#define _C_I2C_NVRAM_H_INCLUDED_

// Включения
#include "stm32l4xx_hal.h"
#include "CAbstractNVRAM.h"

// Флаги и маски
#define  I2C_NVRAM_BANK          0x80              // Флаг второго банка
#define  I2C_NVRAM_BANK_MASK     0x80              // Макса отделения флага банка

// Карта данных i2c eeprom
#define  I2C_NVRAM_HEADER_START  2                 // Первые 2 байт зарезервированы для автоопределения размера EEPROM
#define  I2C_NVRAM_HEADER_SIZE   14                // Зарезервированный размер заголовка
#define  I2C_NVRAM_BANK_START    (I2C_NVRAM_HEADER_START + I2C_NVRAM_HEADER_SIZE)

// Структура заголовка энергонезависимой памяти
typedef struct i2c_nvram_header_s
{
   u8    m_u8BankData;                             // Флаг и размер банка
   u16   m_u16CRC[2];                              // Контрольные суммы банков
} i2c_nvram_header_t;

// Класс для работы с энергонезависимой памятью, работающей через I2C интерфейс
class CI2CNVRAM : public CAbstractNVRAM
{
public:
   // Конструктор/деструктор
   CI2CNVRAM();
   virtual ~CI2CNVRAM();

   // Установка указателя на I2C интерфейс
   void SetInterface(I2C_HandleTypeDef* in_pInterface)
      { m_pInterface = in_pInterface; }
      
   // Установка адреса устройства
   void SetAddress(u8 in_u8Address)
      { m_u8Address = in_u8Address; }

   // Установка параметров памяти
   void Setup(u16 in_u16MemorySize, u16 in_u16PageSize);

   ////////////////////////////////////////////////////////////////////////////
   // Перегруженные методы
   ////////////////////////////////////////////////////////////////////////////
   // Инициализация EEPROM
   virtual s8 Init()
      { return -1; }

   // Принудительное сохранение данных
   virtual bool Save();

   ////////////////////////////////////////////////////////////////////////////
   // Перегруженные методы для чтения и записи данных в энергонезависимую память
   ////////////////////////////////////////////////////////////////////////////
   virtual bool Read(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
      { return false; }
   virtual bool Write(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size)
      { return false; }

protected:
   u16 CalcCRC(u16 in_u16Index, u16 in_u16Size, u16 in_u16CRC);

   I2C_HandleTypeDef*   m_pInterface;              // Указатель на интерфейс I2C
   u8                   m_u8Address;               // Адрес устройства

   i2c_nvram_header_t   m_Header;                  // Флаг и размер банка

   u16                  m_u16PageSize;             // Размер страницы
   u8                   m_u8Pages;                 // Размер памяти в страницах
};

#endif   // _C_I2C_NVRAM_H_INCLUDED_
