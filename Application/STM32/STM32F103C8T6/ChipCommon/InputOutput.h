#ifndef _INPUT_OUTPUT_H_INCLUDED_
#define _INPUT_OUTPUT_H_INCLUDED_

#include "IridiumTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEBOUNCE_DELAY     5                       // Задержка против дребезга контактов

// Структура дискретного выхода
typedef struct digital_output_s
{
   GPIO_TypeDef*  m_pPort;                         // Указатель на данные порта выхода
   u16            m_u16Pin;                        // Номер пина
} digital_output_t;

// Структура для хранения состояния бинарного входа
typedef struct digital_input_s
{
   GPIO_TypeDef*  m_pPort;                         // Указатель на параметры порта входа
   u16            m_u16Pin;                        // Номер пина входа
   u32            m_u32TempTime;                   // Промежуточное время последнего изменения значения
   u32            m_u32ChangeTime;                 // Время начала изменения значения
   u32            m_u32IntervalTime;               // Временной интервал между нажатием/отжатием
   struct
   {
      unsigned m_bTempValue   :  1;                // Промежуточное значение бинарного входа
      unsigned m_bChange      :  1;                // Флаг изменения значения дискретного входа
      unsigned m_bCurValue    :  1;                // Текущее значение бинарного входа
      unsigned m_bCorrection  :  1;                // Флаг корректирования дискретного входа
   } m_Flags;
} digital_input_t;

// Обработка входов
void IO_UpdateInput(digital_input_t* in_pArray, size_t in_stSize);
   
#ifdef __cplusplus
}
#endif
#endif   // _INPUT_OUTPUT_H_INCLUDED_
