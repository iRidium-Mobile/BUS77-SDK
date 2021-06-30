#include "stm32f1xx_hal.h"
#include "Flash.h"

//////////////////////////////////////////////////////////////////////////
// Работа с flash
//////////////////////////////////////////////////////////////////////////
/**
   Очистка флеш памяти (производится разблокировка и стирание значения)
   на входе    :  in_stStart  - адрес начала очищаемой флеш памяти
                  in_stEnd    - адрес конца очищаемой флеш памяти
   на выходе	:  успешность очистки
*/
bool FLASH_Clear(size_t in_stStart, size_t in_stEnd)
{
   bool l_bResult = false;
   u32 l_u32Error = 0;
   FLASH_EraseInitTypeDef l_Data;
   
   // Инициализация структуры для очистки страниц
   l_Data.TypeErase     = FLASH_TYPEERASE_PAGES;
   l_Data.Banks         = FLASH_BANK_1;
   l_Data.PageAddress   = in_stStart - in_stStart % FLASH_PAGE_SIZE;
   l_Data.NbPages       = ((in_stEnd - l_Data.PageAddress) + (FLASH_PAGE_SIZE - 1)) / FLASH_PAGE_SIZE;

   // Разблокирование flash памяти
   HAL_FLASH_Unlock();

   // Очистка страниц
   l_bResult = (HAL_OK == HAL_FLASHEx_Erase(&l_Data, &l_u32Error));

   // Блокирование flash памяти
   HAL_FLASH_Lock();

   return l_bResult;
}

/**
   Запись страницы во флеш памяти из массива
   на входе    :  in_pBuffer  - указатель на буфер откуда нужно брать записываемые данные
                  in_stFlash  - адрес флеш памяти куда нужно записывать данные
                  in_stSize   - размер записываемых данных
   на выходе	:  успешность записи
*/
bool FLASH_Write(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize)
{
   bool l_bResult = true;
   
   // Разблокирование flash памяти
   HAL_FLASH_Unlock();

   // Размер
   u16* l_pPtr = (u16*)in_pBuffer;
   in_stSize = (in_stSize + sizeof(u16) - 1) / sizeof(u16);
   
   while(in_stSize && l_bResult)
   {
      // Запись одной итерации
      l_bResult = (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, in_stFlash, *l_pPtr));
   
      // Сдвиг позиции буфера и flash
      in_stFlash += sizeof(u16);
      l_pPtr++;
   
      // Уменьшение количества данных
      in_stSize--;
   }

   // Блокирование flash памяти
   HAL_FLASH_Lock();

   return l_bResult;
}

