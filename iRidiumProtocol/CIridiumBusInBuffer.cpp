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

#include "IridiumBus.h"
#include "IridiumBytes.h"
#include "IridiumCRC16.h"

/**
   Конструктор класса
   на входе    :  *
*/
CIridiumBusInBuffer::CIridiumBusInBuffer() : CIridiumInBuffer()
{
}

/**
   Деструктор класса
*/
CIridiumBusInBuffer::~CIridiumBusInBuffer()
{
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
   s8 l_s8Result = 0;

   u8 l_u8Byte = 0;
   u8* l_pPtr = GetDataPtr();
   size_t l_stSize = Size();
   // Подготовка структур
   memset(&m_Packet, 0, sizeof(m_Packet));
   memset(&m_InPH, 0, sizeof(m_InPH));
   // Искать заголовок пока это позволяют данные
   if(l_stSize >= IRIDIUM_BUS_MIN_HEADER_SIZE)
   {
      // Извлечение типа протокола, версии и флагов
      m_InPH.m_u8Type            = l_pPtr[0] & IRIDIUM_PROTOCOL_ID_MASK;
      m_InPH.m_Flags.m_bPriority = (l_pPtr[0] >> 7) & 1;
      m_InPH.m_Flags.m_bAddress  = (l_pPtr[0] >> 3) & 1;
      m_InPH.m_Flags.m_bSegment  = (l_pPtr[1] >> 6) & 1;
      m_InPH.m_Flags.m_u2Version = (l_pPtr[1] >> 3) & 3;
      // Получение размера сообщения
      m_Packet.m_stSize          = l_pPtr[2];

      // Проверка маркера, версии протокола и минимального размера сообщения
      if(m_InPH.m_u8Type == IRIDIUM_BUS_PROTOCOL_ID && m_InPH.m_Flags.m_u2Version <= IRIDIUM_PROTOCOL_BUS_VERSION && m_Packet.m_stSize >= IRIDIUM_BUS_MIN_BODY_SIZE)
      {
         // Вычисление размера заголовка
         m_Packet.m_stHeader = 1;
         if(m_InPH.m_Flags.m_bSegment)
            m_Packet.m_stHeader <<= 1;
         if(m_InPH.m_Flags.m_bAddress)
            m_Packet.m_stHeader <<= 1;
         m_Packet.m_stHeader += IRIDIUM_BUS_MIN_HEADER_SIZE;

         // Проверка наличия данных заголовка в буфере
         if(l_stSize >= m_Packet.m_stHeader)
         {
            // Получение флага конца цепочки сообщения и типа шифрации
            m_InPH.m_Flags.m_u3Crypt = l_pPtr[1] & 7;

            u8* l_pBody = l_pPtr + IRIDIUM_BUS_MIN_HEADER_SIZE;

            // Чтение сегмента источника, если сегмент указан
            if(m_InPH.m_Flags.m_bSegment)
            {
               l_pBody = ReadByte(l_pBody, l_u8Byte);
               m_InPH.m_SrcAddr = l_u8Byte << 8;
            }
            // Чтение адреса источника
            l_pBody = ReadByte(l_pBody, l_u8Byte);
            m_InPH.m_SrcAddr |= l_u8Byte;

            // Получение данных о приемнике
            if(m_InPH.m_Flags.m_bAddress)
            {
               // Чтение сегмента приемника, если сегмент указан
               if(m_InPH.m_Flags.m_bSegment)
               {
                  l_pBody = ReadByte(l_pBody, l_u8Byte);
                  m_InPH.m_DstAddr = l_u8Byte << 8;
               }
               // Чтение адреса приемника
               l_pBody = ReadByte(l_pBody, l_u8Byte);
               m_InPH.m_DstAddr |= l_u8Byte;
            }

            // Проверка доступности всех данных контейнера
            if(l_stSize >= (m_Packet.m_stHeader + m_Packet.m_stSize))
            {
               l_s8Result = 1;
               // Вычисление размера тела сообщения
               m_Packet.m_stBody = m_Packet.m_stSize - IRIDIUM_BUS_CRC_SIZE;
               // Проверка CRC16 тела сообщения
               if(GetCRC16Modbus(0xFFFF, l_pBody, m_Packet.m_stSize))
                  l_s8Result = -1;
            } else
            {
               // Если это первый пакет у которого нехватает данных, запомним на него указатель
               l_s8Result = 0;
            }
         } else
         {
            // В буфере недостаточно данных для получения данных заголовка, нужно прекратить поиск заголовков в буфере
            l_s8Result = 0;
         }
      } else
         l_s8Result = -1;
   }
   // Если пакет не найден, начала сообщения нет
   if(l_s8Result <= 0)
      m_Packet.m_stHeader = 0;

   return l_s8Result;
}

/**
   Закрытие обработанного сообшения из входящего буфера
   на входе    :  *
   на выходе   :  *
*/
void CIridiumBusInBuffer::ClosePacket()
{
   // Пропустим обработанные данные
   if(m_Packet.m_stHeader)
      Seek(m_Packet.m_stHeader + m_Packet.m_stSize);
   // Сдвиг буфера
   Shift();
   m_Packet.m_stHeader = 0;
}
