#include "CAbstractNVRAM.h"

// Прототипы функций
extern "C" u32 HAL_GetTick();

/**
   Конструктор класса
   на входе    :  *
*/
CAbstractNVRAM::CAbstractNVRAM()
{
   // Инициализация переменных
   m_pCacheBuffer = NULL;                           
   m_u16CacheSize = 0;
   m_u32UpdateTime = 0;
   m_u32OldTime = 0;
   m_u8Flags = 0;
}

/**
   Деструктор класса
*/
CAbstractNVRAM::~CAbstractNVRAM()
{
}

/**
   Обработка энергонезависимой памяти
   на входе    :  *
   на выходе   :  *
*/
void CAbstractNVRAM::Work()
{
   // Проверка изменения кеша и истечения времени
   if((m_u8Flags & NVRAM_NEED_SAVE) && (HAL_GetTick() - m_u32OldTime) > m_u32UpdateTime)
   {
      // Обновим время
      m_u32OldTime = HAL_GetTick();

      // Запишем данные в EEPROM
      Save();

      // Сброс флага
      m_u8Flags &= ~NVRAM_NEED_SAVE;
   }
}

/**
   Установка кеширования
   на входе    :  in_pBuffer  - указатель на буфер кеширования
                  in_u16Size  - размер буфера кеширования
   на выходе   :  *
*/
void CAbstractNVRAM::SetCache(void* in_pBuffer, u16 in_u16Size)
{
   m_pCacheBuffer = (u8*)in_pBuffer;
   m_u16CacheSize = in_u16Size;
}
