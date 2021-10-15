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
#ifndef _C_IRIDIUM_OUT_BUFFER_H_INCLUDED_
#define _C_IRIDIUM_OUT_BUFFER_H_INCLUDED_

// Включения
#include "Iridium.h"
#include "COutBuffer.h"

//////////////////////////////////////////////////////////////////////////
// class CIridiumOutBuffer
//////////////////////////////////////////////////////////////////////////
class CIridiumOutBuffer : public COutBuffer
{
public:
   // Конструктор/деструктор
   CIridiumOutBuffer();
   virtual ~CIridiumOutBuffer();

   // Настройка буфера
   void SetBuffer(size_t in_stHeaderSize, size_t in_stCRCSize, void* in_pBuffer, size_t in_stSize);
   
   // Начало создания данных
   virtual void Begin(size_t in_stBlockSize)
      { }
   // Окончание создания данных
   virtual bool End(iridium_packet_header_t& in_pHeader)
      { return false; }

   // Методы для добавления данных
   void AddMessageHeader(iridium_message_header_t& in_rMH);
   void AddSearchInfo(iridium_search_info_t& in_rInfo);
   void AddDeviceInfo(iridium_device_info_t& in_rInfo);
   void AddDescription(iridium_description_t& in_rDesc);
   void AddTagDescription(iridium_tag_description_t& in_rDesc);
   void AddChannelDescription(iridium_channel_description_t& in_rDesc);
   void AddMinMaxStepUnit(u8 in_u8Type, universal_value_t& in_rMin, universal_value_t& in_rMax, universal_value_t& in_rStep, u16 in_u16Units);

   // Установка флага окончания данных
   void SetMessageHeaderEnd(bool in_bEnd);

   // Получение данных сформированого пакета
   void* GetPacketPtr()
      { return m_pPacket; }
   size_t GetPacketSize()
      { return m_pPtr - m_pPacket; }

   // Установка/получение данных о данных сообщения
   u8* GetMessagePtr()
      { return m_pMessage; }
   size_t GetMessageSize()
      { return m_pPtr - m_pMessage; }
   void SetMessageSize(size_t in_stSize)
      { m_pPtr = m_pMessage + in_stSize; }
   size_t GetMaxMessageSize()
      { return m_pEnd - m_pMessage; }

protected:
   u8*      m_pPacket;                             // Указатель на данные пакета
   u8*      m_pMessage;                            // Указатель на данные сообщения
   size_t   m_stMaxMessageSize;                    // Максимальный размер данных сообщения
};
#endif   // _C_IRIDIUM_OUT_BUFFER_H_INCLUDED_
