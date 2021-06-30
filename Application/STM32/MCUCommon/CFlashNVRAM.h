#ifndef _C_FLASH_NVRAM_H_INCLUDED_
#define _C_FLASH_NVRAM_H_INCLUDED_

// Включение
#include "CAbstractNVRAM.h"

// Классы
class CFlashNVRAM : public CAbstractNVRAM
{
public:
   // Конструктор/деструктор
   CFlashNVRAM();
   virtual ~CFlashNVRAM();

   ////////////////////////////////////////////////////////////////////////////
   // Перегружаемые методы
   ////////////////////////////////////////////////////////////////////////////
   // Инициализация энергонезависимой памяти
   virtual s8 Init();
   // Загрузка данных из EEPROM в кеш
   virtual bool Load();
   // Соханение данных из кеш в EEPROM (принудительное сохранение данных)
   virtual bool Save();

   ////////////////////////////////////////////////////////////////////////////
   // Собственные методы
   ////////////////////////////////////////////////////////////////////////////
   void SetFlashRange(size_t in_stStart, size_t in_stEnd);

protected:
   size_t   m_stFlashStart;                        // Начальный адрес Flash памяти
   size_t   m_stFlashEnd;                          // Конечный адрес Flash памяти
};

#endif   // _C_FLASH_NVRAM_H_INCLUDED_

