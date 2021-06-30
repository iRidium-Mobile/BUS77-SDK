#include "stm32l4xx_hal.h"
#include "InputOutput.h"

/**
   Опрос входов
   на входе    :  in_pArray   - указатель на массив с данными дискретного входа
                  in_stSize   - количество элементов в массиве
   на выходе	:  *
*/
void IO_UpdateInput(digital_input_t* in_pArray, size_t in_stSize)
{
   bool l_bCurrent = false;
   digital_input_t* l_pCur = in_pArray;
   u32 l_u32Time = HAL_GetTick();

   // Обработка всех входов
   for(size_t i = 0; i < in_stSize; i++)
   {
      // Получение текущего значения бинарного канала
      l_bCurrent = (GPIO_PIN_SET == HAL_GPIO_ReadPin(l_pCur->m_pPort, l_pCur->m_u16Pin));
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
      l_pCur++;
   }
}

/**
   Опрос входов
   на входе    :  in_pArray   - указатель на массив с данными дискретного входа
                  in_stSize   - количество элементов в массиве
   на выходе	:  *
*//*
void IO_UpdateInput(analog_input_t* in_pArray, size_t in_stSize)
{
   bool l_bCurrent = false;
   analog_input_t* l_pCur = in_pArray;
   universal_value_t l_Val;
   u8 l_u8Mode; 
   u32 l_u32Time = HAL_GetTick();

   // Обработка всех входов
   for(size_t i = 0; i < in_stSize; i++)
   {
      
         // Получим режим работы входа
         GetModeAIN(&l_Val,i);
      
         // Сохраним полученое значение
         l_u8Mode = l_Val.m_u8Value;
         // Получим значение входа
         GetAIN(&l_Val,i);
         if(l_Val.m_f32Value < 500000 && l_Val.m_f32Value > -1 && l_u8Mode ==3 )
         {
            l_bCurrent = false;
         } else 
         l_bCurrent = true;

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
      
   
   l_pCur++;
}
}*/
