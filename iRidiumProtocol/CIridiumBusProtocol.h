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
#ifndef _C_IRIDIUM_BUS_PROTOCOL_H_INCLUDED_
#define _C_IRIDIUM_BUS_PROTOCOL_H_INCLUDED_

#include "IridiumBus.h"
#include "CIridiumProtocol.h"
#include "CIridiumBusInBuffer.h"
#include "CIridiumBusOutBuffer.h"

#if defined(IRIDIUM_ENABLE_CIPHER) && defined(IRIDIUM_ENABLE_GRASSHOPPER_CIPHER)
#include "CIridiumCipherGrasshopper.h"
#endif

class CIridiumBusProtocol : public CIridiumProtocol
{
public:
   // Конструктор/деструктор
   CIridiumBusProtocol();
   virtual ~CIridiumBusProtocol();

   // Сброс параметров протокола
   virtual void Reset();

   // IRIDIUM_MESSAGE_SYSTEM_SEARCH (0x03)
#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_MASTER)
   virtual bool SendSearchRequest(iridium_address_t in_DstAddr, u8 in_u8Mask);
#endif

   // IRIDIUM_MESSAGE_SYSTEM_SET_LID (0x05)
#if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_MASTER)
   virtual bool SendSetLIDRequest(iridium_address_t in_DstAddr, const char* in_pszHWID, u8 in_u8LID, u32 in_u32PIN);
#endif

   // Вспомогательные функции
   virtual void InitRequestPacket(bool in_bBrodcast, iridium_address_t in_DstAddr, u8 in_u8Type);
   virtual void InitResponsePacket();

#if defined(IRIDIUM_ENABLE_CIPHER)
   // Инициализация шифрования
   virtual void InitCrypt(u8 in_u8Crypt, u8* in_pData);
   // Кодирование сообщения
   virtual bool EncodeMessage(u8* in_pBuffer, size_t in_stSize, size_t& out_rMax);
   // Декодирование сообщения
   virtual bool DecodeMessage(u8 in_u8Crypt, u8*& out_rBuffer, size_t& out_rSize);
#endif

protected:
   // Данные для обработки входящих сообщения
   CIridiumBusInBuffer        m_InBuffer;          // Входящий буфер
   CIridiumBusInBuffer        m_MessageBuffer;     // Входящий буфер для обработки сообщений
   // Данные для формирования исходящих сообщения
   CIridiumBusOutBuffer       m_OutBuffer;         // Исходящий буфер

#if defined(IRIDIUM_ENABLE_CIPHER) && defined(IRIDIUM_ENABLE_GRASSHOPPER_CIPHER)
   CIridiumCipherGrasshopper  m_Grasshopper;       // Шифр "Кузнечик"
#endif   // defined(IRIDIUM_ENABLE_CIPHER) && defined(IRIDIUM_ENABLE_GRASSHOPPER_CIPHER)
};
#endif   // _C_IRIDIUM_BUS_PROTOCOL_H_INCLUDED_

