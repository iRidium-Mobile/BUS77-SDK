/*******************************************************************************
 * Copyright (c) 2013-2019 iRidi Ltd. www.iridi.com
 *
 * Все права зарегистрированы. Эта программа и сопровождающие материалы доступны
 * на условиях Eclipse Public License v2.0 и Eclipse Distribution License v1.0,
 * которая сопровождает это распространение. 
 *
 * Текст Eclipse Public License доступен по ссылке
 *    http://www.eclipse.org/legal/epl-v20.html
 * Текст Eclipse Distribution License доступн по ссылке
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Участники:
 *    Марат Гилязетдинов, Сергей Королёв  - первая версия
 *******************************************************************************/
#include <stdio.h>
#include "CIridiumBusInBuffer.h"

#include "CalcParity.h"
#include "IridiumBus.h"
#include "Bytes.h"
#include "IridiumCRC16.h"

/**
   Конструктор класса
   на входе    :  *
*/
CIridiumBusInBuffer::CIridiumBusInBuffer() : CIridiumInBuffer()
{
   m_stCount = 0;
   m_stFiltered = 0;
   m_pLock = NULL;
   m_pUnLock = NULL;
}

/**
   Деструктор класса
*/
CIridiumBusInBuffer::~CIridiumBusInBuffer()
{
}

/**
   Установка внешнего буфера
   на входе    :  in_pBuffer  - указатель на буфер
                  in_stSize   - размер буфера
   на выходе   :  *
*/
void CIridiumBusInBuffer::SetBuffer(const void* in_pBuffer, size_t in_stSize)
{
   // Вызов родительского класса
   CInBuffer::SetBuffer(in_pBuffer, in_stSize);
   
   // Сброс собственных переменных
   m_stCount = 0;
   m_stFiltered = 0;
}

/**
   Открытие сообщения для обработки из входящего шинного буфера
   на входе    :  *
   на выходе   :  true  - сообщение было найдено
                  false - сообщение не найдено
   примечание  :  метод ищет во вхоящем буфере BUS пакет, в случае если пакет найден,
                  запоминается позиция в буфере
*/
s8 CIridiumBusInBuffer::OpenPacket()
{
   bool l_bResult = false;
   u8 l_u8Data = 0;

   // Проверка наличия пакетов в буфере
   if(m_stCount)
   {
      // Загрузка BUS пакета входящего буфера
      l_bResult = FindBUSPacket(GetDataPtr(), Size(), m_InPH, m_Packet);

      // Проверка наличия "мусора"
      if(m_Packet.m_stShift)
      {
         // Сдвиг позиции чтения
         Skip(m_Packet.m_stShift);
         // Блокируем доступ к входному буферу
         if(m_pLock)
            l_u8Data = m_pLock();
         // Сдвиг буфера
         Shift();
         m_stFiltered -= m_Packet.m_stShift;
         // Разблокирование буфера
         if(m_pUnLock)
            m_pUnLock(l_u8Data);
      }
   }
   // Вернем результат работы
   return l_bResult;
}

/**
   Закрытие обработанного сообшения из входящего буфера
   на входе    :  *
   на выходе   :  *
*/
void CIridiumBusInBuffer::ClosePacket()
{
   u8 l_u8Data = 0;
   size_t l_stSize = 0;
   if(m_stCount)
   {
      // Пропустим обработанные данные
      if(m_Packet.m_stHeader)
      {
         l_stSize = m_Packet.m_stHeader + m_Packet.m_stSize;
         Seek(l_stSize);
      }         

      // Блокируем доступ к входному буферу
      if(m_pLock)
         l_u8Data = m_pLock();
      // Сдвиг буфера
      Shift();
      m_stFiltered -= l_stSize;
      // Разблокирование буфера
      if(m_pUnLock)
         m_pUnLock(l_u8Data);
      // Уменьшение количества пакетов
      m_stCount--;
      
      m_Packet.m_stHeader = 0;
   }
}

/**
   Удаление шума из входящего шинного буфера
   на входе    :  *
   на выходе   :  true  - был найден и обработан пакет
                  false - пакетов найдено не было
*/
bool CIridiumBusInBuffer::FilterNoise()
{
   bool l_bResult = false;
   u8 l_u8Data = 0;
   iridium_packet_t l_Packet;
   iridium_packet_header_t l_InPH;

   // Получение текущего указателя и размера необработанных данных
   u8* l_pPtr = (u8*)GetBuffer() + m_stFiltered;
   size_t l_stSize = Used() - m_stFiltered;
   size_t l_stCur = m_stFiltered;

   // Поиск BUS пакета во входящем буфере
   l_bResult = FindBUSPacket(l_pPtr, l_stSize, l_InPH, l_Packet);
   // Получение количества пропускаемых данных
   l_stSize = l_Packet.m_stShift;
   // Если пакет найден
   if(l_bResult)
   {
      // Установка новой позиции упаковки
      m_stFiltered += (l_Packet.m_stHeader + l_Packet.m_stSize);
      m_stCount++;
   }

   // Проверка наличия "мусора" который нужно удалить
   if(l_stSize)
   {
      // Блокируем доступ к входному буферу
      if(m_pLock)
         l_u8Data = m_pLock();
      // Уплотнение буфера, удаление "мусора"
      Cut(l_stCur, l_stSize);
      // Разблокирование буфера
      if(m_pUnLock)
         m_pUnLock(l_u8Data);
   }
   // Вернем результат работы
   return l_bResult;
}

/**
   Удаление шума и пакетов не относящихся к устройству из входящего шинного буфера
   на входе    :  in_Address  - адрес устройства пакеты коротого нужно оставить в буфере
   на выходе   :  true  - был найден и обработан пакет
                  false - пакетов найдено не было
*/
bool CIridiumBusInBuffer::FilterNoiseAndForeignPacket(iridium_address_t in_Address)
{
   bool l_bResult = false;
   u8 l_u8Data = 0;
   iridium_packet_t l_Packet;
   iridium_packet_header_t l_InPH;

   // Получение текущего указателя и размера необработанных данных
   u8* l_pPtr = (u8*)GetBuffer() + m_stFiltered;
   size_t l_stSize = Used() - m_stFiltered;
   size_t l_stCur = m_stFiltered;

   // Поиск BUS пакета во входящем буфере
   l_bResult = FindBUSPacket(l_pPtr, l_stSize, l_InPH, l_Packet);
   // Получение количества пропускаемых данных
   l_stSize = l_Packet.m_stShift;
   // Если пакет найден
   if(l_bResult)
   {
      // Обрабатываем только широковещательные и пакеты предназначеные нам
      if(!l_InPH.m_Flags.m_bAddress || (l_InPH.m_Flags.m_bAddress && l_InPH.m_DstAddr == in_Address))
      {
         // Установка новой позиции упаковки
         m_stFiltered += (l_Packet.m_stHeader + l_Packet.m_stSize);
         m_stCount++;
      } else
      {
         // Пакет нам не предназначен, удалим его из буфера
         l_stSize += l_Packet.m_stHeader + l_Packet.m_stSize;
      }
   }

   // Проверка наличия "мусора" который нужно удалить
   if(l_stSize)
   {
      // Блокируем доступ к входному буферу
      if(m_pLock)
         l_u8Data = m_pLock();
      // Уплотнение буфера, удаление "мусора"
      Cut(l_stCur, l_stSize);
      // Разблокирование буфера
      if(m_pUnLock)
         m_pUnLock(l_u8Data);
   }
   // Вернем результат работы
   return l_bResult;
}

/**
   Поиск заголовка пакета для BUS протокола
   на входе    :  in_pLock    - указатель на обработчик блокирования доступа к входящему буферу
                  in_pUnLock  - указатель на обработчик разблокирования доступа к входящему буферу
   на выходе   :  *
*/
void CIridiumBusInBuffer::SetLockUnlock(lock_buffer_t in_pLock, unlock_buffer_t in_pUnLock)
{
   m_pLock = in_pLock;
   m_pUnLock = in_pUnLock;
}

/**
   Поиск заголовка пакета для BUS протокола
   на входе    :  in_pBuffer  - указатель на буфер в котором нужно искать пакет
                  in_stSize   - размер данных в буфере
                  out_rInPH   - ссылка на структуру куда нужно поместить данные заголовка
                  out_rPacket - ссылка на структуру куда нужно поместить данные о пакете
   на выходе   :  true  - пакет найден
                  false - пакета в буфере нет
*/
bool CIridiumBusInBuffer::FindBUSPacket(u8* in_pBuffer, size_t in_stSize, iridium_packet_header_t& out_rInPH, iridium_packet_t& out_rPacket)
{
   bool l_bResult = false;

   u8 l_u8Byte = 0;
   u8* l_pPtr = in_pBuffer;
   size_t l_stSize = in_stSize;
   // Подготовка структур
   memset(&out_rPacket, 0, sizeof(out_rPacket));
   memset(&out_rInPH, 0, sizeof(out_rInPH));
   out_rPacket.m_stShift = (size_t)-1;
   // Искать заголовок пока это позволяют данные
   while(l_stSize >= IRIDIUM_BUS_MIN_HEADER_SIZE)
   {
      // Извлечение типа протокола, версии и флагов
      out_rInPH.m_u8Type            = l_pPtr[0] & IRIDIUM_PROTOCOL_ID_MASK;
      out_rInPH.m_Flags.m_bPriority = (l_pPtr[0] >> 7) & 1;
      out_rInPH.m_Flags.m_bAddress  = (l_pPtr[0] >> 3) & 1;
      out_rInPH.m_Flags.m_bSegment  = (l_pPtr[1] >> 6) & 1;
      out_rInPH.m_Flags.m_u2Version = (l_pPtr[1] >> 3) & 3;
      // Получение размера сообщения
      out_rPacket.m_stSize          = l_pPtr[2];

      // Проверка маркера, версии протокола и минимального размера сообщения
      if(out_rInPH.m_u8Type == IRIDIUM_BUS_PROTOCOL_ID && out_rInPH.m_Flags.m_u2Version <= IRIDIUM_PROTOCOL_BUS_VERSION && out_rPacket.m_stSize >= IRIDIUM_BUS_MIN_BODY_SIZE)
      {
         // Проверка четности размера данных
         if((l_pPtr[1] >> 7) == GetParity(l_pPtr[2]))
         {
            // Вычисление размера заголовка
            out_rPacket.m_stHeader = IRIDIUM_BUS_MIN_HEADER_SIZE;
            out_rPacket.m_stHeader += out_rInPH.m_Flags.m_bSegment ? 2 : 0;
            out_rPacket.m_stHeader += out_rInPH.m_Flags.m_bAddress ? 2 : 0;

            // Проверка наличия данных заголовка в буфере
            if(l_stSize >= out_rPacket.m_stHeader)
            {
               // Получение флага конца цепочки сообщения и типа шифрации
               out_rInPH.m_Flags.m_u3Crypt = l_pPtr[1] & 7;

               u8* l_pBody = l_pPtr + IRIDIUM_BUS_MIN_HEADER_SIZE;

               // Чтение данных сегмента об источнике и приемнике
               if(out_rInPH.m_Flags.m_bSegment)
               {
                  l_pBody = ReadU8(l_pBody, l_u8Byte);
                  out_rInPH.m_SrcAddr = l_u8Byte << 8;
                  l_pBody = ReadU8(l_pBody, l_u8Byte);
                  out_rInPH.m_DstAddr = l_u8Byte << 8;
               }
               // Чтение данных адреса об источнике и приемнике
               if(out_rInPH.m_Flags.m_bAddress)
               {
                  l_pBody = ReadU8(l_pBody, l_u8Byte);
                  out_rInPH.m_SrcAddr |= l_u8Byte;
                  l_pBody = ReadU8(l_pBody, l_u8Byte);
                  out_rInPH.m_DstAddr |= l_u8Byte;
               }

               // Проверка доступности всех данных контейнера
               if(l_stSize >= (out_rPacket.m_stHeader + out_rPacket.m_stSize))
               {
                  u16 l_u16CRC = 0;
                  // Получение CRC16 из пакета
                  out_rPacket.m_stBody = out_rPacket.m_stSize - IRIDIUM_BUS_CRC_SIZE;
                  ReadU16LE(l_pBody + out_rPacket.m_stBody, l_u16CRC);
                  // Вычисление CRC16 пакета и справнение с CRC16 из пакета
                  if(GetCRC16Modbus(0xFFFF, l_pBody, out_rPacket.m_stBody) == l_u16CRC)
                  {
                     // Пакет найден, запишем размер тела
                     out_rPacket.m_stShift = l_pPtr - in_pBuffer;
                     l_bResult = true;
                     break;
                  }
               } else
               {
                  // Если это первый пакет у которого нехватает данных, запомним на него указатель
                  if(out_rPacket.m_stShift == (size_t)-1)
                     out_rPacket.m_stShift = l_pPtr - in_pBuffer;
               }
            } else
            {
               // В буфере недостаточно данных для получения данных заголовка, нужно прекратить поиск заголовков в буфере
               l_bResult = false;
               break;
            }
         }
      }
      // Переход на следующий байт
      l_pPtr++;
      l_stSize--;
   }
   // Если пакет не найден, начала сообщения нет
   if(!l_bResult)
   {
      out_rPacket.m_stHeader = 0;
      // Если в буфере не было найдено пакетов с недостающими данными, очистим буфер от мусора
      if(out_rPacket.m_stShift == (size_t)-1)
         out_rPacket.m_stShift = l_pPtr - in_pBuffer;
   }

   return l_bResult;
}

