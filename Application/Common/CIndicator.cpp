#include "CIndicator.h"

/**
   Конструктор класса
   на входе    :  *
*/
CIndicator::CIndicator()
{
   // Сброс состояния
   Reset();

   // Цвет
   SetColor(0);

   // Выключение индикатора
   SetMode(INDICATOR_MODE_OFF);
}

/**
   Устновка текущего состояния
   на входе    :  in_eMode - новое значение состояния
   на выходе   :  *
*/
void CIndicator::SetMode(eIndicatorMode in_eMode)
{
   size_t l_stIndex = 0;
   u8 l_u8Count = 0;

   // Проверка изменений 
   if(m_eMode != in_eMode)
   {
      // Сброс состояния индикатора
      Reset();

      // Сохранение режима работы
      m_eMode = in_eMode;
      
      // Настройка индикации представляет собой бинарное значение, 
      // где первый интервал означает время выключенного состояния светодиода, 
      // второе - включенного состояния (в цикле до применения дальнейшей директивы)
      switch(m_eMode)
      {
         // Режим включено и выключено
         case INDICATOR_MODE_ON:
         case INDICATOR_MODE_OFF:
            break;
         
         // Режим работы в загрузчике
         case INDICATOR_MODE_BOOTLOADER:
            m_aSequence[l_stIndex++] = 1000;
            m_aSequence[l_stIndex++] = 1000;
            break;
         
         // Режим обычной работы
         case INDICATOR_MODE_WORK:
            m_aSequence[l_stIndex++] = 3000;
            m_aSequence[l_stIndex++] = 100;
            break;

         // Режим индикации ошибки
         default:
         {
            // Первичная задержка
            m_aSequence[l_stIndex++] = 500;
            // Формирование группы (длинные сигналы)
            l_u8Count = (m_eMode >> 4) & 0x7;
            for(u8 i = 0; i < l_u8Count; i++)
            {
               m_aSequence[l_stIndex++] = 600;
               m_aSequence[l_stIndex++] = 600;
            }
            // Пауза межлу группами
            m_aSequence[l_stIndex - 1] += 1500;

            // Формирование кода (короткие сигналы)
            l_u8Count = m_eMode & 0x7;
            for(u8 i = 0; i < l_u8Count; i++)
            {
               m_aSequence[l_stIndex++] = 100;
               m_aSequence[l_stIndex++] = 600;
            }
            // Пауза межлу группами
            m_aSequence[l_stIndex - 1] += 1500;
            break;
         }
      }
      // Конец датаграммы
      m_aSequence[l_stIndex] = 0;
   }
}

/**
   Обработка индикации
   на входе    :  in_u8Color  - цвет индикатора
   на выходе   :  *
*/
void CIndicator::SetColor(u8 in_u8Color)
{
   // Сброс состония
   Reset();

   // Установка цвета
   m_u8Color = in_u8Color;
}

/**
   Установка времени мерцания
   на входе    :  in_u32Time  - время мерцания в миллисекундах (0 - мигать бесконечно, иначен время мигания)
   на выходе   :  *
*/
void CIndicator::SetBlinkTime(u32 in_u32Time)
{
   // Сброс состояния
   Reset();
   // Если значение равно 0, выставим время мигания в ~0 (бесконечность)
   m_u32Blink = (0 == in_u32Time) ? ~0 : in_u32Time;
}

/**
   Обработка индикации
   на входе    :  *
   на выходе   :  *
*/
void CIndicator::Work()
{
   u32 l_u32Time = GetTime();
   // Инициализация времени
   if(!m_u32Time)
      m_u32Time = l_u32Time;

   if(m_u32Blink)
   {
      // Проверка времени работы мерцания
      if((l_u32Time - m_u32Time) < m_u32Blink)
      {
         // Обработка мерцания
         m_u8Current = (l_u32Time / 100) & 1;
         if(m_u8Current != m_u8Old)
         {
            // Установка индикатора
            Set(m_u8Color, m_u8Current);
            m_u8Old = m_u8Current;
         }
      // Проверка на бесконечность
      } else if(m_u32Blink != ~0)
      {
         m_u32Blink = 0;
         m_u8Old = 0xFF;
      }
   } else
   {
      if(m_u8Current != m_u8Old)
      {
         bool l_bState = false;
         m_u8Old = m_u8Current;
         
         switch(m_eMode)
         {
            // Индикатор выключен
            case INDICATOR_MODE_OFF:
               break;
            // Индикатор включен
            case INDICATOR_MODE_ON:
               l_bState = true;
               break;
            // Индикатор работает в соответствии с таблицей
            default:
               l_bState = m_u8Current & 1;
               break;
         }
         // Установка индикатора
         Set(m_u8Color, l_bState);
      }
      // Изменение текущего состояния
      if(m_aSequence[m_u8Current] && (l_u32Time - m_u32Time) > m_aSequence[m_u8Current])
      {
         // Изменение последовательности
         if(!m_aSequence[++m_u8Current])
            m_u8Current = 0;
         
         // Запомним время
         m_u32Time = l_u32Time;
      }
   }   
}

/**
   Сброс состояния индикатора
   на входе    :  *
   на выходе   :  *
*/
void CIndicator::Reset()
{
   m_u32Time = 0;
   m_u8Current = 0;
   m_u8Old = 0xFF;
   m_u32Blink = 0;
}

