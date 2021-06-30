#ifndef _C_I2C_NVRAM_8BIT_H_INCLUDED_
#define _C_I2C_NVRAM_8BIT_H_INCLUDED_

#include "CI2CNVRAM.h"

#define EEPROM_SIZE_1      0
#define EEPROM_SIZE_2      1
#define EEPROM_SIZE_4      2
#define EEPROM_SIZE_8      3
#define EEPROM_SIZE_16     4
#define EEPROM_SIZE_32     5
#define EEPROM_SIZE_64     6
#define EEPROM_SIZE_128    7
#define EEPROM_SIZE_256    8
#define EEPROM_SIZE_512    9
#define EEPROM_SIZE_1024   10
#define EEPROM_SIZE_2048   11

// Класс для работы с энергонезависимой памятью, работающей через I2C интерфейс
class CI2CNVRAM8Bit : public CI2CNVRAM
{
public:
   // Конструктор/деструктор
   CI2CNVRAM8Bit();
   virtual ~CI2CNVRAM8Bit();

   // Установка количества бит
   void SetBits(u8 in_u8Bits)
      { m_u8Bits = in_u8Bits; }

   ////////////////////////////////////////////////////////////////////////////
   // Перегруженные методы
   ////////////////////////////////////////////////////////////////////////////
   // Инициализация EEPROM
   virtual s8 Init();

   // Методы для чтения и записи данных в энергонезависимую память
   virtual bool Read(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size);
   virtual bool Write(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size);

protected:
   u8 m_u8Bits;
};

#endif   // _C_I2C_NVRAM_8BIT_H_INCLUDED_
