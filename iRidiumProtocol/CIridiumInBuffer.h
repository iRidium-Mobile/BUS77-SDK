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
#ifndef _C_IRIDIUM_IN_BUFFER_H_INCLUDED_
#define _C_IRIDIUM_IN_BUFFER_H_INCLUDED_

// Включения
#include "IridiumConfig.h"
#include "CInBuffer.h"
#include "Iridium.h"

// Структура для получения параметров поиска
typedef struct iridium_packet_s
{
   size_t   m_stHeader;                            // Размер заголовка
   size_t   m_stBody;                              // Размер тела сообщения
   size_t   m_stSize;                              // Размер сообщения (Размер тела + CRC)
} iridium_packet_t;

//////////////////////////////////////////////////////////////////////////
// class CIridiumInBuffer
//////////////////////////////////////////////////////////////////////////
class CIridiumInBuffer : public CInBuffer
{
public:
   // Конструктор/деструктор
   CIridiumInBuffer();
   virtual ~CIridiumInBuffer();

   virtual void Clear();

   // Перегруженные методы
   // Открытие/закрытие пакета
   virtual s8 OpenPacket()
      { return 0; }
   virtual void ClosePacket()
      { }

   // Получение указателя на данные сообщения
   void* GetMessagePtr()
      { return (u8*)GetBuffer() + m_Packet.m_stHeader; }
   // Получение размера сообщения
   size_t GetMessageSize()
      { return m_Packet.m_stBody; }

   // Получение указателя на данные пакета
   void* GetPacketPtr()
      { return GetBuffer(); }
   // Получение размера пакета
   size_t GetPacketSize()
      { return m_Packet.m_stHeader + m_Packet.m_stSize; }

   // Получение указателя на заголовок пакета
   iridium_packet_header_t* GetPacketHeader()
      { return &m_InPH; }

   // Общие методы
   bool GetMessageHeader(iridium_message_header_t& out_rMH);
   bool GetSearchInfo(iridium_search_info_t& out_rInfo);
   bool GetDeviceInfo(iridium_device_info_t& out_rInfo);
   bool GetDescription(iridium_description_t& out_rDesc);
   bool GetTagDescription(iridium_tag_description_t& out_rDesc);
   bool GetChannelDescription(iridium_channel_description_t& out_rDesc);

protected:
   iridium_packet_t        m_Packet;               // Данные пакета
   iridium_packet_header_t m_InPH;                 // Заголовок входящего пакета
};
#endif   // _C_IRIDIUM_IN_BUFFER_H_INCLUDED_
