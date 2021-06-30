#ifndef _C_I2C_NVRAM_16BIT_H_INCLUDED_
#define _C_I2C_NVRAM_16BIT_H_INCLUDED_

#include "CI2CNVRAM.h"

// Класс для работы с энергонезависимой памятью, работающей через I2C интерфейс
class CI2CNVRAM16Bit : public CI2CNVRAM
{
public:
   // Конструктор/деструктор
   CI2CNVRAM16Bit();
   virtual ~CI2CNVRAM16Bit();

   ////////////////////////////////////////////////////////////////////////////
   // Перегруженные методы
   ////////////////////////////////////////////////////////////////////////////
   // Инициализация EEPROM
   virtual s8 Init();

   // Методы для чтения и записи данных в энергонезависимую память
   virtual bool Read(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size);
   virtual bool Write(u16 in_u16Address, u8* in_pBuffer, u16 in_u16Size);

protected:
   // Проверка доступности адреса
   s8 IsAddress(u16 in_u16Address);
};

#endif   // _C_I2C_NVRAM_16BIT_H_INCLUDED_
