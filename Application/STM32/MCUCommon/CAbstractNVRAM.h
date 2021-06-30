#ifndef _C_ABSTRACT_NVRAM_H_INCLUDED_
#define _C_ABSTRACT_NVRAM_H_INCLUDED_

// Включение
#include "IridiumTypes.h"

// Флаги
#define NVRAM_NEED_SAVE 0x80                       // Признак того что нужно сохранить кеш к память

// Классы
class CAbstractNVRAM
{
public:
   // Конструктор/деструктор
   CAbstractNVRAM();
   virtual ~CAbstractNVRAM();

   ////////////////////////////////////////////////////////////////////////////
   // Перегружаемые методы
   ////////////////////////////////////////////////////////////////////////////
   // Инициализация энергонезависимой памяти
   virtual s8 Init()
      { return 0; }
   // Загрузка данных из EEPROM в кеш
   virtual bool Load()
      { return false; }
   // Соханение данных из кеш в EEPROM (принудительное сохранение данных)
   virtual bool Save()
      { return false; }
   // Обработка устройства
   virtual void Work();

   ////////////////////////////////////////////////////////////////////////////
   // Собственные методы
   ////////////////////////////////////////////////////////////////////////////
   // Установка буфера кеширования
   void SetCache(void* in_pBuffer, u16 in_u16Size);

   // Установка времени обновления
   void SetUpdateTime(u32 in_u32Time)
      { m_u32UpdateTime = in_u32Time; }

   // Установка флага сохранения данных
   void NeedSave()
      { m_u8Flags |= NVRAM_NEED_SAVE; }

   // Получение указателя на данные энергонезависимой памяти
   void* GetPtr()
      { return m_pCacheBuffer; }

protected:
   u8*   m_pCacheBuffer;                           // Указатель на буфер кеша
   u16   m_u16CacheSize;                           // Размер кеша
   u32   m_u32UpdateTime;                          // Время между записью данных
   u32   m_u32OldTime;                             // Время последнего сохранения
   u8    m_u8Flags;
};

#endif   // _C_ABSTRACT_NVRAM_H_INCLUDED_

