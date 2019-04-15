#include "stm32f1xx_hal.h"
#include "Flash.h"

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
   for(size_t i = in_stStart; i < in_stEnd; i += FLASH_PAGE_SIZE)
   {
      // Ждем пока память будет готова
      while(FLASH->SR & FLASH_SR_BSY) ;

      // Если операция завершена успешно. Сбрасываем флаг. Ставится аппаратно, снимаем записав "1" 
      if(FLASH->SR & FLASH_SR_EOP)          
         FLASH->SR = FLASH_SR_EOP;

      // Устанавливаем бит стирания одной страницы
      FLASH->CR |= FLASH_CR_PER;
      // Задаем адрес стираемой страницы
      FLASH->AR = (uint32_t)i;
      // Запускаем стирание
      FLASH->CR |= FLASH_CR_STRT;
     
      // Ожидание окончания стирания страницы
      while(!(FLASH->SR & FLASH_SR_EOP)) ;

      // Бит окончания стирания и записи(для сброса пишем "1")
      FLASH->SR = FLASH_SR_EOP;
      // Сбрасываем бит обратно
      FLASH->CR &= ~FLASH_CR_PER;
   }
}

/**
   Запись страницы во флеш памяти из массива
   на входе    :  in_pBuffer  - указатель на буфер откуда нужно брать записываемые данные
                  in_stFlash  - адрес флеш памяти куда нужно записывать данные
                  in_stSize   - размер записываемых данных
   на выходе	:  *
   примечание  :  перед использованием нужно вызывать метод HAL_FLASH_Unlock(), а по окончанию работы HAL_FLASH_Lock()
*/
void FLASH_Write(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize)
{
   // Ждем пока память будет готова
   while(FLASH->SR & FLASH_SR_BSY);

   // Если операция завершена успешно. Сбрасываем флаг. Ставится аппаратно, снимаем записав "1" 
   if(FLASH->SR & FLASH_SR_EOP)             
      FLASH->SR = FLASH_SR_EOP;

   // Разрешение записи во флеш
   FLASH->CR |= FLASH_CR_PG;

   // Запись массива в страницу
   for(size_t i = 0; i < (in_stSize / 2); i++)
   {
      *(volatile uint16_t*)(in_stFlash + i * 2) = (uint16_t)(in_pBuffer[i*2 + 1] << 8 | in_pBuffer[i*2]);
		
      // Ждем окончания действия
      while(!(FLASH->SR & FLASH_SR_EOP));

      // Сбрасываем флаг. Ставится аппаратно, снимаем записав "1" 
      FLASH->SR &= ~FLASH_SR_EOP;
   }

   // Пока этот бит установлен нам разрешено писать во флеш, сбрасываем его
   FLASH->CR &= ~(FLASH_CR_PG);
}

/**
   Чтение из флеша памяти в буфер
   на входе    :  in_pBuffer  - указатель на буфер куда нужно прочитать данные
                  in_stFlash  - адрес флеш памяти куда нужно записывать данные
                  in_stSize   - размер читаемых данных
   на выходе	:  *
*/
void FLASH_Read(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize)
{
   for(size_t i = 0; i < in_stSize; i++)
      *in_pBuffer++ = *(__IO uint8_t*)in_stFlash++;
}

/**
   Проверка записаного
   на входе    :  in_pBuffer  - указатель на буфер куда нужно прочитать данные
                  in_stFlash  - адрес флеш памяти с чем нужно проверять данные
                  in_stSize   - размер проверяемых данных
   на выходе	:  *
*/
bool FLASH_Test(u8* in_pBuffer, size_t in_stFlash, size_t in_stSize)
{
   for(size_t i = 0; i < in_stSize; i++)
   {
      if(*in_pBuffer++ != *(__IO uint8_t*)in_stFlash++)
         return false;
   }
   return true;
}

