#include "stm32l4xx_hal.h"
#include "Flash.h"
#include "stm32l4xx_hal_flash.h"
#include "stm32l4xx_hal_flash_ex.h"
#include "IridiumBytes.h"

//////////////////////////////////////////////////////////////////////////
// Работа с flash
//////////////////////////////////////////////////////////////////////////
/**
   Очистка флеш памяти (производится разблокировка и стирание значения)
   на входе    :  in_stStart  - адрес начала очищаемой флеш памяти
                  in_stEnd    - адрес конца очищаемой флеш памяти
   на выходе	:  *
   примечание  :  перед использованием нужно вызывать метод HAL_FLASH_Unlock(), а по окончанию работы HAL_FLASH_Lock()
*/
void FLASH_Clear(size_t in_stStart, size_t in_stEnd)
{
   FLASH_EraseInitTypeDef l_Flash;
   uint32_t l_u32Error = 0;

   // Разблокирование памяти
   HAL_FLASH_Unlock();

   // Вычисление номера первой стираемой страницы
   u32 l_u32Start = (in_stStart - FLASH_BASE) / FLASH_PAGE_SIZE;
   // Вычисление номера последней стираемой страницы
   u32 l_u32End = (in_stEnd - FLASH_BASE + (FLASH_PAGE_SIZE - 1)) / FLASH_PAGE_SIZE;

   // Подготовка структуры к очистке
   l_Flash.TypeErase = FLASH_TYPEERASE_PAGES;
   l_Flash.Page = l_u32Start; 
   l_Flash.NbPages = l_u32End - l_u32Start;
   __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | /*FLASH_FLAG_PGPERR |*/ FLASH_FLAG_PGSERR));

   // Массовая очистка памяти
   HAL_FLASHEx_Erase(&l_Flash, &l_u32Error);

   // Блокирование память
   HAL_FLASH_Lock();
}

/**
   Запись страницы во флеш памяти из массива
   на входе    :  in_pBuffer  - указатель на буфер откуда нужно брать записываемые данные
                  in_stFlash  - адрес флеш памяти куда нужно записывать данные
                  in_stSize   - размер записываемых данных
   на выходе  :  *
   примечание  :  перед использованием нужно вызывать метод HAL_FLASH_Unlock(), а по окончанию работы HAL_FLASH_Lock()
*/
void FLASH_Write(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize)
{
   bool l_bResult = true;
   
    //Выставление флагов
   __HAL_FLASH_DATA_CACHE_DISABLE();
   __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
   
   __HAL_FLASH_DATA_CACHE_RESET();
   __HAL_FLASH_INSTRUCTION_CACHE_RESET();
   
   __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
   __HAL_FLASH_DATA_CACHE_ENABLE();

   // Разблокирование памяти
   HAL_FLASH_Unlock();
   
   in_stSize = (in_stSize + 7) / 8;
   
   u8* l_pPtr = (u8*)in_pBuffer;
   
   while(in_stSize && l_bResult)
   {
      u64 l_u64Temp = 0;//*l_pPtr;
      ReadDirect(l_pPtr, &l_u64Temp, 8);
      // Запись байта
      l_bResult = (HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, in_stFlash, l_u64Temp));
   
      // Сдвиг позиции буфера и флеша
      in_stFlash += sizeof(uint64_t);
      in_pBuffer += sizeof(uint64_t);
      l_pPtr     += sizeof(uint64_t);
   
      // Уменьшение количества данных
      in_stSize--;
   }
   // Блокирование память
   HAL_FLASH_Lock();
}

/**
   Чтение из флеша памяти в буфер
   на входе    :  in_pBuffer  - указатель на буфер куда нужно прочитать данные
                  in_stFlash  - адрес флеш памяти куда нужно записывать данные
                  in_stSize   - размер читаемых данных
   на выходе	:  *
*//*
void FLASH_Read(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize)
{
}*/

/**
   Проверка записаного
   на входе    :  in_pBuffer  - указатель на буфер куда нужно прочитать данные
                  in_stFlash  - адрес флеш памяти с чем нужно проверять данные
                  in_stSize   - размер проверяемых данных
   на выходе   :  *
*//*
bool FLASH_Test(u8* in_pBuffer, u8* in_stFlash, size_t in_stSize)
{
   return (0 == memcmp(in_pBuffer, in_stFlash, in_stSize));
}*/

