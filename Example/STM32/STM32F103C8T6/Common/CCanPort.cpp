#include "CCANPort.h"
#include "Iridium.h"

/**
   Конструктор класса
   на входе    :  *
   на выходе   :  *
*/
CCANPort::CCANPort()
{
   m_u8Address = 0;
   m_u8TID = 0;
   m_u16CANID = 0;
   memset(&m_InBuffer, 0, sizeof(m_InBuffer));
   memset(&m_OutBuffer, 0, sizeof(m_OutBuffer));
   memset(&m_aPacketBuffer, 0, sizeof(m_aPacketBuffer));
   m_stPacketSize = 0;
   m_bTransmite = false;
}

/**
   Деструктор класса
   
*/
CCANPort::~CCANPort()
{
}

/**
   Установка входящего буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  in_stSize   - размер буфера
   на выходе   :  успешность установки буфера
*/
bool CCANPort::SetInBuffer(void* in_pBuffer, size_t in_stSize)
{
   bool l_bResult = false;
   
   if(in_pBuffer && in_stSize > sizeof(can_frame_t))
   {
      // Вычисление максимального колличества фреймов в буфере
      m_InBuffer.m_stMax = in_stSize / sizeof(can_frame_t);
      m_InBuffer.m_pBuffer = (can_frame_t*)in_pBuffer;
      // Очистка буфера
      memset(in_pBuffer, 0, in_stSize);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Установка входящего буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  in_stSize   - размер буфера
   на выходе   :  успешность установки буфера
*/
bool CCANPort::SetOutBuffer(void* in_pBuffer, size_t in_stSize)
{
   bool l_bResult = false;
   
   if(in_pBuffer && in_stSize > sizeof(can_frame_t))
   {
      // Вычисление максимального колличества фреймов в буфере
      m_OutBuffer.m_stMax = in_stSize / sizeof(can_frame_t);
      m_OutBuffer.m_pBuffer = (can_frame_t*)in_pBuffer;
      // Очистка буфера
      memset(in_pBuffer, 0, in_stSize);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Добавление фрейма в буфер
   на входе    :  in_pFrame   - указатель на данные фрейма
   на выходе   :  успешность добавления фрейма
*/
bool CCANPort::AddFrame(can_frame_t* in_pFrame)
{
   bool l_bResult = false;

   // Проверка наличия места в буфере
   if(m_InBuffer.m_stCount < m_InBuffer.m_stMax)
   {
      // Копирование фрейма
      memcpy(m_InBuffer.m_pBuffer + m_InBuffer.m_stCount, in_pFrame, sizeof(can_frame_t));
      m_InBuffer.m_stCount++;
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Добавление пакета в буфер (разложение на фреймы)
   на входе    :  in_bBroadcast  - признак широковещательного пакета
                  in_u8Address   - адрес на который надо отправлять CAN фреймы
                  in_pBuffer     - указатель на буфер с данными
                  in_stSize      - размер данных в буфере
   на выходе   :  успешность добавления данных
*/
bool CCANPort::AddPacket(bool in_bBroadcast, u8 in_u8Address, void* in_pBuffer, size_t in_stSize)
{
   bool l_bResult = false;

   size_t l_stSize = in_stSize;
   u8* l_pPtr = (u8*)in_pBuffer;

   // Вычисление количества фреймов
   u8 l_u8Frames = (l_stSize + 7) / 8;

   // Проверим поместятся ли данные в буфер
   if((m_OutBuffer.m_stMax - m_OutBuffer.m_stCount) >= l_u8Frames)
   {
      // Вычисление указателя на первый фрейм
      size_t l_stIndex = (m_OutBuffer.m_stStart + m_OutBuffer.m_stCount) % m_OutBuffer.m_stMax;
      u8 l_u8TID = GetTID();
      // Разбивка на буфера на фреймы
      for(u8 i = 0; i < l_u8Frames; i++)
      {
         can_frame_t* l_pFrame = m_OutBuffer.m_pBuffer + l_stIndex;

         // Заполнение полей структуры
         l_pFrame->m_u32ExtID =  ((m_u16CANID & IRIDIUM_EXT_ID_CAN_ID_MASK) << IRIDIUM_EXT_ID_CAN_ID_SHIFT) |
                                 ((l_u8TID & IRIDIUM_EXT_ID_TID_MASK) << IRIDIUM_EXT_ID_TID_SHIFT) |
                                 (in_bBroadcast << IRIDIUM_EXT_ID_BROADCAST_SHIFT) |
                                 (in_u8Address << IRIDIUM_EXT_ID_ADDRESS_SHIFT) |
                                 (l_u8Frames == (i + 1));
         l_pFrame->m_u8Size = (l_stSize < 8) ? l_stSize : 8;
         // Копирование данных
         memcpy(l_pFrame->m_aData, l_pPtr, l_pFrame->m_u8Size);
         // Уменьшение размера
         l_stSize -= l_pFrame->m_u8Size;
         l_pPtr += l_pFrame->m_u8Size;

         l_stIndex = (l_stIndex + 1) % m_OutBuffer.m_stMax;
      }
      // Увеличение количества фреймов
      m_OutBuffer.m_stCount += l_u8Frames;
      l_bResult = true;
   } else
      l_bResult = false;
   
   return l_bResult;
}

/**
   Получение текущего пакета
   на входе    :  out_rBuffer - ссылка на указатель куда нужно поместить указатель на данные полученого пакета
                  in_rSize    - ссылка на переменную куда нужно поместить размер полученого пакета
   на выходе   :  успешность получения пакета
*/
bool CCANPort::GetPacket(void*& out_rBuffer, size_t& out_rSize)
{
   // Проверка наличия в буфере ранее полученого пакета
   bool l_bResult = (0 != m_stPacketSize);
   if(!l_bResult)
   {
      // Поиск замыкающего фрейма
      can_frame_t* l_pEnd = m_InBuffer.m_pBuffer + m_InBuffer.m_stCount;
      for(can_frame_t* l_pCur = m_InBuffer.m_pBuffer; l_pCur != l_pEnd; l_pCur++)
      {
         // Поиск замыкающего пакета
         if(l_pCur->m_u32ExtID & IRIDIUM_EXT_ID_END_MASK)
         {
            // Найден замыкающий пакет, соберем пакет
            m_stPacketSize = Assembly(l_pCur, m_aPacketBuffer, sizeof(m_aPacketBuffer));
            // Удаление обработанных пакетов
            //Flush();
            l_bResult = (0 != m_stPacketSize);
            break;
         }
      }
   }

   // Если данные получены, вернем указатель и длинну пакета
   if(l_bResult)
   {
      // Вернем уже полученый пакет
      out_rBuffer = m_aPacketBuffer;
      out_rSize = m_stPacketSize;
   }

   return l_bResult;
}

/**
   Удаление обработаного пакета 
   на входе    :  *
   на выходе   :  *
*/
void CCANPort::DeletePacket()
{
   m_stPacketSize = 0;
}

/**
   Удаление всех неиспользуемых пакетов
   на входе    :  *
   на выходе   :  *
*/
void CCANPort::Flush()
{
   // Сборка пакета
   can_frame_t* l_pEnd = m_InBuffer.m_pBuffer + m_InBuffer.m_stCount;
   can_frame_t* l_pCur = m_InBuffer.m_pBuffer;
   while(l_pCur != l_pEnd)
   {
      if(!l_pCur->m_u32ExtID)
      {
         // Уменьшение количества фреймов
         m_InBuffer.m_stCount--;
         // Удаление фрейма из массива
         u8* l_pDst = (u8*)l_pCur;
         u8* l_pSrc = (u8*)(l_pCur + 1);
         size_t l_stSize = (u8*)l_pEnd - l_pSrc;
         memmove(l_pDst, l_pSrc, l_stSize);
         // Вычисление нового указателя на конец
         l_pEnd = m_InBuffer.m_pBuffer + m_InBuffer.m_stCount;
      } else
         l_pCur++;
   }
}

/**
   Получение указателя на фрейм по индексу
   на входе    :  *
   на выходе   :  указатель на данные текущего фрейма, если NULL нет текущего фрейма, то есть нет фреймов для отправки
*/
can_frame_t* CCANPort::GetFrame()
{
   can_frame_t* l_pResult = NULL;

   // Проверка наличия фреймов для отправки
   if(m_OutBuffer.m_stCount)
      l_pResult = m_OutBuffer.m_pBuffer + m_OutBuffer.m_stStart;

   return l_pResult;
}

/**
Удаление фрейма по индексу
   на входе    :  *
   на выходе   :  *
*/
void CCANPort::DeleteFrame()
{
   // Проверка наличия фреймов
   if(m_OutBuffer.m_stCount)
   {
      m_OutBuffer.m_stStart = (m_OutBuffer.m_stStart + 1) % m_OutBuffer.m_stMax;
      m_OutBuffer.m_stCount--;
   }
}

/**
   Сборка пакета из фреймов
   на входе    :  in_pEnd     - указатель на замыкающий пакет
                  out_pBuffer - указатель на буфер куда нужно поместить данные
                  in_stSize   - размер буфера куда нужно поместить данные
   на выходе   :  длинна собранного пакета
*/
size_t CCANPort::Assembly(can_frame_t* in_pEnd, void* out_pBuffer, size_t in_stSize)
{
   size_t l_stResult = 0;
   u8* l_pOut = (u8*)out_pBuffer;
   u32 l_u32ID = in_pEnd->m_u32ExtID & IRIDIUM_EXT_ID_COMPARE_MASK;
   // Сборка пакета
   for(can_frame_t* l_pCur = m_InBuffer.m_pBuffer; l_pCur <= in_pEnd; l_pCur++)
   {
      // Поиск фрейма с указанными параметрами
      if((l_pCur->m_u32ExtID & IRIDIUM_EXT_ID_COMPARE_MASK) == l_u32ID)
      {
         // Получение размера
         size_t l_stSize = (in_stSize > l_pCur->m_u8Size) ? l_pCur->m_u8Size : in_stSize;
         // Добавление содержимого фрейма в буфер
         if(l_stSize)
         {
            memcpy(l_pOut, l_pCur->m_aData, l_stSize);
            // Сдвиг позиции в буфере
            l_pOut += l_stSize;
            l_stResult += l_stSize;
            in_stSize -= l_stSize;
         }
         // Отметим что фрейм обработан
         l_pCur->m_u32ExtID = 0;
      }
   }
   return l_stResult;
}

/**
   Получение идентификатора транзакции
   на входе    :  *
   на выходе   :  идентификатор транзакции в диапазоне от 0 до 7
*/
u8 CCANPort::GetTID()
{
   m_u8TID = (m_u8TID + 1) & 0x07;
   return m_u8TID;
}
