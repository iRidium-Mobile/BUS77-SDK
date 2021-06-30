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
// Включения
#include "CIridiumBusOutBuffer.h"

#include "IridiumBus.h"
#include "CIridiumCipher.h"
#include "IridiumBytes.h"
#include "IridiumCRC16.h"

/**
   Конструктор класса
   на входе    :  *
*/
CIridiumBusOutBuffer::CIridiumBusOutBuffer() : CIridiumOutBuffer()
{
}

/**
   Деструктор класса
*/
CIridiumBusOutBuffer::~CIridiumBusOutBuffer()
{
}

/**
   Начало создания пакета
   на входе    :  in_stBlockSize - размер блока (0 - блоков нет, иначе размер блочного шифрования)
   на выходе   :  *
*/
void CIridiumBusOutBuffer::Begin(size_t in_stBlockSize)
{
   // Определение расположения заголовка и тела
   m_pPacket = NULL;
   m_pPtr = m_pMessage;
   m_pEnd = m_pMessage + m_stMaxMessageSize;
   // Проверка наличия блочного шифрования
   if(in_stBlockSize)
   {
      // Зарезервируем место для заголовка шифрованых данных
      m_pPtr += IRIDIUM_BUS_CIPHER_HEADER_SIZE;
      // Вычисление максимальной границы
      m_pEnd -= (m_stMaxMessageSize % in_stBlockSize);
   }
}

/**
   Окончание создания пакета
   на входе    :  in_rHeader  - ссылка на заголовок пакета
   на выходе   :  успешность окончания работы с пакетом
*/
bool CIridiumBusOutBuffer::End(iridium_packet_header_t& in_rHeader)
{
   bool l_bResult = false;
   
   // Получение указателя и размера полезной нагрузки
   u8* l_pPtr =  GetMessagePtr();
   size_t l_stSize = GetMessageSize();

   // Проверка размера буфера
   if(l_stSize)
   {
      // Расчет и добавление CRC16
      u16 l_u16CRC = GetCRC16Modbus(0xFFFF, l_pPtr, l_stSize);
      m_pPtr = WriteLE(m_pPtr, &l_u16CRC, 2);
      l_stSize += 2;

      // Проверка наличия маршрута
      bool l_bRoute = (0 != in_rHeader.m_u8Route & 0x7F);
      
      // Вычисление размера заголовка
      size_t l_stLen = 1;
      if(l_bRoute)
         l_stLen++;
      if(in_rHeader.m_Flags.m_bSrcSeg)
         l_stLen++;
      if(in_rHeader.m_Flags.m_bDstSeg)
         l_stLen++;
      if(in_rHeader.m_Flags.m_bAddress)
         l_stLen++;
      l_pPtr -= (l_stLen + IRIDIUM_BUS_MIN_HEADER_SIZE);

      // Сформируем указатель на пакет
      m_pPacket = l_pPtr;

      // Запись маркера и списка флагов
      l_pPtr = WriteByte(l_pPtr, in_rHeader.m_u8Type | in_rHeader.m_Flags.m_bAddress << 3 | in_rHeader.m_Flags.m_bPriority << 7);
      l_pPtr = WriteByte(l_pPtr, l_bRoute << 7 | in_rHeader.m_Flags.m_bSrcSeg << 6 | in_rHeader.m_Flags.m_bDstSeg << 5 | in_rHeader.m_Flags.m_u2Version << 3 | in_rHeader.m_Flags.m_u3Crypt);

      // Запись длины тела сообщения
      l_pPtr = WriteByte(l_pPtr, l_stSize);

      // Запись маршрута
      if(l_bRoute)
         l_pPtr = WriteByte(l_pPtr, in_rHeader.m_u8Route);

      // Запись сегмента источника, если сегмент указан
      if(in_rHeader.m_Flags.m_bSrcSeg)
         l_pPtr = WriteByte(l_pPtr, in_rHeader.m_SrcAddr >> 8);
      // Запись адреса источника
      l_pPtr = WriteByte(l_pPtr, in_rHeader.m_SrcAddr);

      // Запись сегмента приемника, если сегмент указан
      if(in_rHeader.m_Flags.m_bDstSeg)
         l_pPtr = WriteByte(l_pPtr, in_rHeader.m_DstAddr >> 8);
      // Запись данных приемника
      if(in_rHeader.m_Flags.m_bAddress)
         l_pPtr = WriteByte(l_pPtr, in_rHeader.m_DstAddr);
      l_bResult = true;
   }
   return l_bResult;
}
