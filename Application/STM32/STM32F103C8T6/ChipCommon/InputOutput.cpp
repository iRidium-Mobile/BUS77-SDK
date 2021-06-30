#include "stm32f1xx_hal.h"
#include "InputOutput.h"

/**
   Опрос входов с блокирование дребезга контактов
   на входе    :  in_pArray   - указатель на массив с данными дискретного входа
                  in_stSize   - количество элементов в массиве
   на выходе	:  *
*/
void IO_UpdateInput(digital_input_t* in_pArray, size_t in_stSize)
{
   bool l_bCurrent = false;                                                                     // Создаём итоговый флаг
   digital_input_t* l_pCur = in_pArray;                                                         // Делаем слепок
   u32 l_u32Time = HAL_GetTick();                                                               // Запоминаем текущее время

   // Обработка всех входов
   for(size_t i = 0; i < in_stSize; i++)
   {
      // Получение состояние входа
      l_bCurrent = (GPIO_PIN_SET == HAL_GPIO_ReadPin(l_pCur->m_pPort, l_pCur->m_u16Pin));
      // Коррекция состояния входа
      l_bCurrent ^= l_pCur->m_Flags.m_bCorrection;

      // Проверка изменения промежуточного значения
      if(l_bCurrent != l_pCur->m_Flags.m_bTempValue)
      {
         // Запомним время и сохраним новое промежуточное значение
         l_pCur->m_u32TempTime = l_u32Time;
         l_pCur->m_Flags.m_bTempValue = l_bCurrent;

      } else
      {
         l_pCur->m_Flags.m_bChange = false;
         // Проверка что промежуточное значение держится больше определенного интервала
         if(l_pCur->m_u32TempTime + DEBOUNCE_DELAY < HAL_GetTick())
         {
            // Проверка изменения текущего значения
            if(l_bCurrent != l_pCur->m_Flags.m_bCurValue)
            {
               // Изменение значения
               l_pCur->m_Flags.m_bChange = true;
               // Запомним новое текущее значение
               l_pCur->m_Flags.m_bCurValue = l_bCurrent;
               // Вычислим интервал между изменениями значения
               l_pCur->m_u32IntervalTime =  l_u32Time - l_pCur->m_u32ChangeTime;
               // Запомним время
               l_pCur->m_u32ChangeTime = l_u32Time;
            }
         }
      }
      // Переход на следующее значение
      l_pCur++;
   }
}

