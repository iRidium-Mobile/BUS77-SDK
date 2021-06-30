#include "CFlashNVRAM.h"
#include "Flash.h"

/**
   Конструктор класса
   на входе    :  *
*/
CFlashNVRAM::CFlashNVRAM() : CAbstractNVRAM()
{
   m_stFlashStart = 0;
   m_stFlashEnd = 0;
}

/**
   Деструктор класса
*/
CFlashNVRAM::~CFlashNVRAM()
{
}

/**
   Инициализация энергонезависимой памяти
   на входе    :  *
   на выходе   :  успешность инициализации
                  > 0   - инициализация прошла успешно
                  = 0   - инициализация не пройдена
                  < 0   - аппаратная проблема
*/
s8 CFlashNVRAM::Init()
{
   s8 l_s8Result = 0;

   // Получение размера флеша
   size_t l_stFlashSize = (m_stFlashEnd - m_stFlashStart) + 1;

   // Проверка размера кеша и флеша
   if(l_stFlashSize >= m_u16CacheSize)
   {
      // Коррекция размера флеша
      m_stFlashEnd = m_stFlashStart + m_u16CacheSize;

      // Загрузка кеша
      if(Load())
         l_s8Result = 1;
   } else
   {
      // Невозможно поместить кеш в флеш память
      l_s8Result = -1;
   }

   return l_s8Result;
}

/**
   Загрузка данных из энергонезависимой памяти в кеш
   на входе    :  *
   на выходе   :  успешность чтения данных
*/
bool CFlashNVRAM::Load()
{
   u8* l_pDst = (u8*)m_pCacheBuffer;
   u8* l_pSrc = (u8*)m_stFlashStart;
   u16 l_u16Size = m_u16CacheSize;

   while(l_u16Size--)
      *l_pDst++ = *l_pSrc++;

   return true;
}

/**
   Сохранение данных из кеша в энергонезависимую память
   на входе    :  *
   на выходе   :  успешность записи данных
*/
bool CFlashNVRAM::Save()
{
   // Очистка страницы
   FLASH_Clear(m_stFlashStart, m_stFlashEnd);
   // Запись кеша во флеш
   FLASH_Write(m_pCacheBuffer, m_stFlashStart, m_u16CacheSize);
   // Вернем результат записи
   return (0 == memcmp(m_pCacheBuffer, (u8*)m_stFlashStart, m_u16CacheSize));
}

/**
   Установка кеширования
   на входе    :  in_stStart  - начальный адрес флеша
                  in_stEnd    - конечный адрес флеша
   на выходе   :  *
*/
void CFlashNVRAM::SetFlashRange(size_t in_stStart, size_t in_stEnd)
{
   m_stFlashStart = in_stStart;
   m_stFlashEnd = in_stEnd;
}
