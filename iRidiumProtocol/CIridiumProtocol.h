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
#ifndef _C_IRIDIUM_PROTOCOL_H_INCLUDED_
#define _C_IRIDIUM_PROTOCOL_H_INCLUDED_

#include "Iridium.h"
#include "CIridiumInBuffer.h"
#include "CIridiumOutBuffer.h"
#include "CIridiumCipher.h"

class CIridiumProtocol
{
public:
   // Конструктор/деструктор
   CIridiumProtocol();
   virtual ~CIridiumProtocol();

   // Сброс параметров протокола
   virtual void Reset();

   ////////////////////////////////////////////////////////////////////////////
   // Системные команды
   // IRIDIUM_MESSAGE_SYSTEM_PING (0x02)
#if defined(IRIDIUM_CONFIG_SYSTEM_PING_MASTER)
   bool SendPingRequest(bool in_bBroadcast, iridium_address_t in_DstAddr);
   void ReceivePingResponse();
#endif

#if defined(IRIDIUM_CONFIG_SYSTEM_PING_SLAVE)
   void ReceivePingRequest();
#endif

   // IRIDIUM_MESSAGE_SYSTEM_SEARCH (0x03)
#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_MASTER)
   virtual bool SendSearchRequest(iridium_address_t in_DstAddr, u8 in_u8Mask)
      { return false; }
   void ReceiveSearchResponse();
#endif

#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_SLAVE)
   void ReceiveSearchRequest();
   bool SendSearchResponse(u16 in_u16TID, iridium_search_info_t& in_rInfo);
#endif

   // IRIDIUM_MESSAGE_SYSTEM_GET_DEVICE_INFO (0x04)
#if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_MASTER)
   bool SendDeviceInfoRequest(iridium_address_t in_DstAddr);
   void ReceiveDeviceInfoResponse();
#endif

#if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_SLAVE)
   void ReceiveDeviceInfoRequest();
#endif

   // IRIDIUM_MESSAGE_SYSTEM_SET_LID (0x05)
#if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_MASTER)
   virtual bool SendSetLIDRequest(iridium_address_t in_DstAddr, const char* in_pszHWID, u8 in_u8LID, u32 in_u32PIN)
      { return false; }
   void ReceiveSetLIDResponse();
#endif

#if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_SLAVE)
   void ReceiveSetLIDRequest();
#endif

   // IRIDIUM_MESSAGE_SYSTEM_SESSION_TOKEN (0x08)
#if defined(IRIDIUM_CONFIG_SYSTEM_SESSION_TOKEN_MASTER)
   bool SendSessionTokenRequest(u64 in_u64Token);
   void ReceiveSessionTokenResponse();
#endif

#if defined(IRIDIUM_CONFIG_SYSTEM_SESSION_TOKEN_SLAVE)
   void ReceiveSessionTokenRequest();
#endif

   // IRIDIUM_MESSAGE_SYSTEM_SMART_API (0x0A)
#if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_MASTER)
   bool SendSmartAPIRequest(iridium_address_t in_DstAddr);
   void ReceiveSmartAPIResponse();
#endif

#if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_SLAVE)
   void ReceiveSmartAPIRequest();
#endif

   ////////////////////////////////////////////////////////////////////////////
   // Работа с глобальными переменными
   // IRIDIUM_MESSAGE_SET_VARIABLE (0x10)
#if defined(IRIDIUM_CONFIG_SET_VARIABLE_MASTER)
   bool SendSetVariableRequest(u16 in_u16Variable, u8 in_u8Type, universal_value_t& in_rValue);
   void ReceiveSetVariableResponse();
#endif

#if defined(IRIDIUM_CONFIG_SET_VARIABLE_SLAVE)
   void ReceiveSetVariableRequest();
#endif

   // IRIDIUM_MESSAGE_GET_VARIABLE (0x11)
#if defined(IRIDIUM_CONFIG_GET_VARIABLE_MASTER)
   bool SendGetVariableRequest(u16 in_u16Variable);
   void ReceiveGetVariableResponse();
#endif

#if defined(IRIDIUM_CONFIG_GET_VARIABLE_SLAVE)
   void ReceiveGetVariableRequest();
   bool SendGetVariableResponse(u16 in_u16Variable, u8 in_u8Type, universal_value_t& in_rValue);
#endif

   ////////////////////////////////////////////////////////////////////////////
   // Работа с каналами обратной связи
   // IRIDIUM_MESSAGE_GET_TAGS (0x20)
#if defined(IRIDIUM_CONFIG_GET_TAGS_MASTER)
   bool SendGetTagsRequest(iridium_address_t in_DstAddr);
   void ReceiveGetTagsResponse();
#endif

#if defined(IRIDIUM_CONFIG_GET_TAGS_SLAVE)
   void ReceiveGetTagsRequest();
#endif

   // IRIDIUM_MESSAGE_LINK_TAG_AND_VARIABLE (0x24)
#if defined(IRIDIUM_CONFIG_LINK_TAG_AND_VARIABLE_MASTER)
   bool SendLinkTagAndVariableRequest(iridium_address_t in_DstAddr, u32 in_u32TagID, bool in_bOwner, u16 in_u16Variable, u32 in_u32PIN);
   void ReceiveLinkTagAndVariableResponse();
#endif

#if defined(IRIDIUM_CONFIG_LINK_TAG_AND_VARIABLE_SLAVE)
   void ReceiveLinkTagAndVariableRequest();
#endif

   // IRIDIUM_MESSAGE_GET_TAG_DESCRIPTION (0x25)
#if defined(IRIDIUM_CONFIG_GET_TAG_DESCRIPTION_MASTER)
   bool SendGetTagDescriptionRequest(iridium_address_t in_DstAddr, u32 in_u32TagID);
   void ReceiveGetTagDescriptionResponse();
#endif

#if defined(IRIDIUM_CONFIG_GET_TAG_DESCRIPTION_SLAVE)
   void ReceiveGetTagDescriptionRequest();
#endif

   // IRIDIUM_MESSAGE_SET_TAG_VALUE (0x26)
#if defined(IRIDIUM_CONFIG_SET_TAG_VALUE_MASTER)
   bool SendSetTagValueRequest(iridium_address_t in_DstAddr, u32 in_u32TagID, u8 in_u8Type, universal_value_t& in_rValue);
   void ReceiveSetTagValueResponse();
#endif

#if defined(IRIDIUM_CONFIG_SET_TAG_VALUE_SLAVE)
   void ReceiveSetTagValueRequest();
#endif

   // IRIDIUM_MESSAGE_GET_TAG_VALUE (0x27)
#if defined(IRIDIUM_CONFIG_GET_TAG_VALUE_MASTER)
   bool SendGetTagValueRequest(iridium_address_t in_DstAddr, u32 in_u32TagID);
   void ReceiveGetTagValueResponse();
#endif

#if defined(IRIDIUM_CONFIG_GET_TAG_VALUE_SLAVE)
   void ReceiveGetTagValueRequest();
   bool SendGetTagValueResponse(u32 in_u32TagID, iridium_tag_info_t& in_rInfo);
#endif

   ////////////////////////////////////////////////////////////////////////////
   // Работа с каналами управления
   // IRIDIUM_MESSAGE_GET_CHANNELS (0x30)
#if defined(IRIDIUM_CONFIG_GET_CHANNELS_MASTER)
   bool SendGetChannelsRequest(iridium_address_t in_DstAddr);
   void ReceiveGetChannelsResponse();
#endif

#if defined(IRIDIUM_CONFIG_GET_CHANNELS_SLAVE)
   void ReceiveGetChannelsRequest();
#endif

   // IRIDIUM_MESSAGE_SET_CHANNEL_VALUE (0x33)
#if defined(IRIDIUM_CONFIG_SET_CHANNEL_VALUE_MASTER)
   bool SendSetChannelValueRequest(iridium_address_t in_DstAddr, u32 in_u32ChannelID, u8 in_u8Type, universal_value_t& in_rValue, u32 in_u32PIN);
   void ReceiveSetChannelValueResponse();
#endif

#if defined(IRIDIUM_CONFIG_SET_CHANNEL_VALUE_SLAVE)
   void ReceiveSetChannelValueRequest();
#endif

   // IRIDIUM_MESSAGE_LINK_CHANNEL_AND_VARIABLE (0x34)
#if defined(IRIDIUM_CONFIG_LINK_CHANNEL_AND_VARIABLE_MASTER)
   bool SendLinkChannelAndVariableRequest(iridium_address_t in_DstAddr, u32 in_u32ChannelID, u8 in_u8Variables, u16* in_pVariables, u32 in_u32PIN);
   void ReceiveLinkChannelAndVariableResponse();
#endif

#if defined(IRIDIUM_CONFIG_LINK_CHANNEL_AND_VARIABLE_SLAVE)
   void ReceiveLinkChannelAndVariableRequest();
#endif

   // IRIDIUM_MESSAGE_GET_CHANNEL_DESCRIPTION (0x35)
#if defined(IRIDIUM_CONFIG_GET_CHANNEL_DESCRIPTION_MASTER)
   bool SendGetChannelDescriptionRequest(iridium_address_t in_DstAddr, u32 in_u32ChannelID);
   void ReceiveGetChannelDescriptionResponse();
#endif

#if defined(IRIDIUM_CONFIG_GET_CHANNEL_DESCRIPTION_SLAVE)
   void ReceiveGetChannelDescriptionRequest();
#endif

   // IRIDIUM_MESSAGE_GET_CHANNEL_VALUE (0x36)
#if defined(IRIDIUM_CONFIG_GET_CHANNEL_VALUE_MASTER)
   bool SendGetChannelValueRequest(iridium_address_t in_DstAddr, u32 in_u32ChannelID, u32 in_u32PIN);
   void ReceiveGetChannelValueResponse();
#endif

#if defined(IRIDIUM_CONFIG_GET_CHANNEL_VALUE_SLAVE)
   void ReceiveGetChannelValueRequest();
   bool SendGetChannelValueResponse(u32 in_u32ChannelID, iridium_channel_info_t& in_rInfo);
#endif

   ////////////////////////////////////////////////////////////////////////////
   // Работа с каналами управления
   // IRIDIUM_MESSAGE_STREAM_OPEN (0x50)
#if defined(IRIDIUM_CONFIG_STREAM_OPEN_MASTER)
   bool SendStreamOpenRequest(iridium_address_t in_DstAddr, const char* in_pszName, eIridiumStreamMode in_eMode, u32 in_u32PIN);
   void ReceiveStreamOpenResponse();
#endif

#if defined(IRIDIUM_CONFIG_STREAM_OPEN_SLAVE)
   void ReceiveStreamOpenRequest();
   bool SendStreamOpenResponse(const char* in_pszName, eIridiumStreamMode in_eMode, u8 in_u8StreamID);
#endif

   // IRIDIUM_MESSAGE_STREAM_BLOCK (0x51)
#if defined(IRIDIUM_CONFIG_STREAM_BLOCK_MASTER)
   bool SendStreamBlockRequest(iridium_address_t in_DstAddr, u8 in_u8StreamID, u8 in_u8BlockID, u16 in_u16Size, const void* in_pBlock);
   void ReceiveStreamBlockResponse();
#endif

#if defined(IRIDIUM_CONFIG_STREAM_BLOCK_SLAVE)
   void ReceiveStreamBlockRequest();
   bool SendStreamBlockResponse(u8 in_u8StreamID, u8 in_u8BlockID, u16 in_u16Size);
#endif

   // IRIDIUM_MESSAGE_STREAM_CLOSE (0x52)
#if defined(IRIDIUM_CONFIG_STREAM_CLOSE_MASTER)
   bool SendStreamCloseRequest(iridium_address_t in_DstAddr, u8 in_u8StreamID);
   void ReceiveStreamCloseResponse();
#endif

#if defined(IRIDIUM_CONFIG_STREAM_CLOSE_SLAVE)
   void ReceiveStreamCloseRequest();
   bool SendStreamCloseResponse(u8 in_u8StreamID);
#endif

   // Установка адресации
   void SetAddress(iridium_address_t in_Address)
      { m_Address = in_Address; }
   iridium_address_t GetAddress()
      { return m_Address; }

   // Вспомогательные функции
   virtual void InitRequestPacket(bool in_bBrodcast, iridium_address_t in_DstAddr, u8 in_u8Type)
      {}
   virtual void InitResponsePacket()
      {}

   // Начало/окончанияе формирования исходящего пакета
   void Begin();
   bool End();

   // Получение заголовка сообщения
   iridium_message_header_t* GetMessageHeader()
      { return &m_InMH; }
   // Получение указателя на данные сообщения
   void* GetMessagePtr()
      { return m_pInMessage->GetBuffer(); }
   // Получение размера сообщения
   size_t GetMessageSize()
      { return m_pInMessage->Size(); }

   // Переотправка сообщения с модификацией заголовка
   bool Resend(iridium_packet_header_t* in_pPH, const void* in_pPtr, size_t in_stSize);

   //////////////////////////////////////////////////////////////////////////
   // Перегруженные методы, методы предназначены для реализации
   // особенностей протоколов типа TCP/UDP/CAN и так далее
   //////////////////////////////////////////////////////////////////////////
   // Обработка входящих сообщения
   virtual void ProcessMessageRequest();
   virtual void ProcessMessageResponse();

#if defined(IRIDIUM_ENABLE_CIPHER)
   // Инициализация шифрования
   virtual void InitCrypt(u8 in_u8Crypt, u8* in_pData)
      {}
   // Кодирование сообщения
   virtual bool EncodeMessage(u8* in_pBuffer, size_t in_stSize, size_t& out_rMax)
      { return false; }
   // Декодирование сообщения
   virtual bool DecodeMessage(u8 in_u8Crypt, u8*& out_rBuffer, size_t& out_rSize)
      { return false; }
#endif   // defined(IRIDIUM_ENABLE_CIPHER)

   // Обработка пакета из входящего буфера
   virtual bool ProcessMessage(iridium_packet_header_t* in_pPH, const void* in_pBuffer, size_t in_stSize);

   //////////////////////////////////////////////////////////////////////////
   // Перегруженные методы, методы предназначены для реализации
   // взимодействия протокола с программой
   //////////////////////////////////////////////////////////////////////////
   // Отправка данных
   virtual bool SendPacket(bool in_bBroadcast, iridium_address_t in_Address, void* in_pBuffer, size_t in_stSize)
      { return false; }

   // Установка локального идентификатора
   virtual void SetLID(u8 in_u8LID)
      {}
   // Сравнение HWID
   virtual bool CompareHWID(const char* in_pszHWID)
      { return false; }
   // Проверка возможности выполнения операции
   virtual s8 CheckOperation(eIridiumOperation in_eType, u32 in_u32PIN, void* in_pData)
      { return 1; }

   // Обработчик результата обработки
   virtual void ProcessingResult(eIridiumErrorSource in_eSource, eIridiumError in_eError, iridium_packet_header_t* in_pPH, iridium_message_header_t* in_pMH)
      {}

   // Получен ответ на пинг
#if defined(IRIDIUM_CONFIG_SYSTEM_PING_MASTER)
   virtual void ReceivedPong()
      {}
#endif
   // Установка/получение информации о найденом устройстве
#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_MASTER)
   virtual void SetSearchInfo(iridium_search_info_t& in_rInfo)
      {}
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_SLAVE)
   virtual bool GetSearchInfo(iridium_search_info_t& out_rInfo)
      { return false; }
#endif

   // Установка/получение подробной информации об устройстве
#if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_MASTER)
   virtual void SetDeviceInfo(iridium_device_info_t& in_rInfo)
      {}
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_SLAVE)
   virtual bool GetDeviceInfo(iridium_device_info_t& out_rInfo)
      { return false; }
#endif

   // Установка/получение информации о Smart API
#if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_MASTER)
   virtual void SetSmartAPI(void* in_pBuffer, size_t in_stSize, u8 in_u8Flags)
      { }
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_SLAVE)
   virtual bool GetSmartAPI(void*& out_rBuffer, size_t& out_rSize)
      { return false; }
#endif

   // Работа с глобальными переменными
   virtual void SetVariable(u16 in_u16Variable, u8 in_u8Type, universal_value_t& in_rValue, u8 in_u8Flags)
      { }
   virtual bool GetVariable(u16 in_u16Variable, u8& out_rType, universal_value_t& out_rValue)
      { return false; }

   // Работа с каналами обратной связи
   virtual size_t GetTags()
      { return 0; }
   virtual size_t GetTagIndex(u32 in_u32TagID)
      { return ~0; }
      //{ return (size_t)~0; }
   virtual void SetTagData(iridium_tag_info_t& in_rInfo)
      { }
   virtual size_t GetTagData(size_t in_stIndex, iridium_tag_info_t& out_rInfo, size_t in_stSize)
      { return 0; }
   virtual void SetTagDescription(u32 in_u32TagID, iridium_tag_description_t& in_rDescription)
      { }
   virtual bool GetTagDescription(u32 in_u32TagID, iridium_tag_description_t& out_rDescription)
      { return false; }
   virtual bool LinkTagAndVariable(u32 in_u32TagID, bool in_bOwner, u16 in_u16Variable)
      { return false;  }
   virtual bool SetTagValue(u32 in_u32TagID, u8 in_u8Type, universal_value_t& in_rValue, u8 in_u8Flags)
      { return false; }

   // Работа с каналами управления
   virtual size_t GetChannels()
      { return 0; }
   virtual size_t GetChannelIndex(u32 in_u32ChannelID)
      { return ~0; }//(size_t)-1; }
   virtual void SetChannelData(iridium_channel_info_t& in_rInfo)
      { }
   virtual size_t GetChannelData(size_t in_stIndex, iridium_channel_info_t& out_rInfo)
      { return 0; }
   virtual void SetChannelDescription(u32 in_u32ChannelID, iridium_channel_description_t& in_rDescription)
      { }
   virtual bool GetChannelDescription(u32 in_u32ChannelID, iridium_channel_description_t& out_rDescription)
      { return false; }
   virtual bool LinkChannelAndVariable(u32 in_u32ChannelID, u8 in_u8Variables, u16* in_pVariables)
      { return false; }
   virtual bool SetChannelValue(u32 in_u32ChannelID, u8 in_u8Type, universal_value_t& in_rValue, u8 in_u8Flags)
      { return false; }

   // Работа с потоками
   virtual u8 StreamOpen(const char* in_pszName, eIridiumStreamMode in_eMode)
      { return 0; }
   virtual void StreamOpenResult(const char* in_pszName, eIridiumStreamMode in_eMode, u8 in_u8StreamID)
      { }
   virtual size_t StreamBlock(u8 in_u8StreamID, u8 in_u8BlockID, size_t in_stSize, const void* in_pBuffer)
      { return 0; }
   virtual void StreamBlockResult(u8 in_u8StreamID, u8 in_u8BlockID, size_t in_stSize)
      { }
   virtual void StreamClose(u8 in_u8StreamID)
      { }

protected:
   // Вспомогательные функции
   u16 GetTID();
   bool SendRequest(bool in_bBroadcast, iridium_address_t in_DstAddr, u8 in_u8Type);
   bool SendResponse(eIridiumError in_eCode);

   virtual void EndTags()
      { }
   virtual void EndChannels()
      { }
   // Получение адреса источника
   iridium_address_t GetSrcAddress()
      { return m_pInPH->m_SrcAddr; }
   // Получение адреса приемника
   iridium_address_t GetDstAddress()
      { return m_pInPH->m_DstAddr; }

protected:
   bool                       m_bEnableLongString; // Флаг блокирующий длинные строки
   u16                        m_u16TID;            // Текущий идентификатор транзакции
   iridium_address_t          m_Address;           // Собственный адрес
   eIridiumError              m_eError;            // Код ошибки при обработке входящего сообщения
   // Данные для обработки входящих сообщения
   iridium_packet_header_t*   m_pInPH;             // Указатель на данные заголовка входящего пакета
   iridium_message_header_t   m_InMH;              // Заголовок входящего сообщения
   CIridiumInBuffer*          m_pInMessage;        // Указатель на входящий буфер сообщения
   // Данные для формирования исходящих сообщения
   iridium_packet_header_t    m_OutPH;             // Данные заголовка исходящего пакета
   iridium_message_header_t   m_OutMH;             // Заголовок исходящего сообщения
   CIridiumOutBuffer*         m_pOutMessage;       // Указатель на исходящий буфер сообщения

#if defined(IRIDIUM_ENABLE_CIPHER)
   // Шифрование тела сообщения
   CIridiumCipher*            m_pCipher;           // Указатель на кодер/декодер
   u8                         m_u8Count;           // "Случайное" значение
#endif   // defined(IRIDIUM_ENABLE_CIPHER)
};
#endif   // _C_IRIDIUM_PROTOCOL_H_INCLUDED_
