#include "CFirmware.h"

/**
   Конструктор класса
   на входе    :  *
*/
CFirmware::CFirmware()
{
   Close();
}

/**
   Деструктор класса
*/
CFirmware::~CFirmware()
{
}

/**
   Открытие прошивки
   на входе    :  in_stStart  - указатель на буфер
                  in_stSize   - размер данных
   на выходе   :  *
*/
void CFirmware::Open(size_t in_stStart, size_t in_stSize)
{
   m_stStart = in_stStart;
   m_stCur = m_stStart;
   m_stEnd = m_stStart + in_stSize;
}

/**
   Закрытие прошивки
   на входе    :  *
   на выходе   :  *
*/
void CFirmware::Close()
{
   m_Address = 0;
   m_u8StreamID = 0;
   m_u8BlockID = 0;
   m_u32Time = 0;
   m_stStart = 0;
   m_stCur = 0;
   m_stEnd = 0;
}
      
/**
   Пропуск данных
   на входе    :  in_stSize   - количество пропускаемых данных
   на выходе   :  *
*/
void CFirmware::Skip(size_t in_stSize)
{
   // Сдвиг буфера
   m_stCur += in_stSize;
   
   // Проверка на выход за пределы
   if(m_stCur >= m_stEnd)
      m_stCur = m_stEnd;
}
