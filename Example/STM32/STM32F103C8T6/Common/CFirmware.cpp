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
   на входе    :  in_pBuffer  - указатель на буфер
                  in_stSize   - размер данных
   на выходе   :  *
*/
void CFirmware::Open(u8* in_pBuffer, size_t in_stSize)
{
   m_pBuffer = in_pBuffer;
   m_pEnd = m_pBuffer + in_stSize;
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
   m_pBuffer = NULL;
   m_pEnd = NULL;
}
      
/**
   Пропуск данных
   на входе    :  in_stSize   - количество пропускаемых данных
   на выходе   :  *
*/
void CFirmware::Skip(size_t in_stSize)
{
   // Сдвиг буфера
   m_pBuffer += in_stSize;
   
   // Проверка на выход за пределы
   if(m_pBuffer >= m_pEnd)
      m_pBuffer = m_pEnd;
}
