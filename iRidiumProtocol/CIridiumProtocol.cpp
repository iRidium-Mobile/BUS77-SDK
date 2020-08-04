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
#include "CIridiumProtocol.h"
#include "CIridiumInBuffer.h"
#include "COutBuffer.h"
#include <stdlib.h>

/**
   Конструктор класса
   на входе    :  *
*/
CIridiumProtocol::CIridiumProtocol()
{
   m_Address = 0;
   m_eError = IRIDIUM_OK;

   // Подготовка структрур для приема данных
   m_pInPH = NULL;
   memset(&m_InMH, 0, sizeof(m_InMH));
   m_pInMessage = NULL;

   // Подготовка структрур для отправки данных
   memset(&m_OutPH, 0, sizeof(m_OutPH));
   memset(&m_OutMH, 0, sizeof(m_OutMH));
   m_pOutMessage = NULL;

   // Сброс данных
   Reset();
}

/**
   Деструктор класса
*/
CIridiumProtocol::~CIridiumProtocol()
{
}

/**
   Сброс соединения
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::Reset()
{
   m_bEnableLongString = false;
   m_u16TID = 0;
}

#if defined(IRIDIUM_CONFIG_SYSTEM_PING_MASTER)

/**
   Отправка запроса пинга
   на входе    :  in_bBroadcast  - признак широковещательного запроса
                  in_DstAddr     - адрес получателя
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendPingRequest(bool in_bBroadcast, iridium_address_t in_DstAddr)
{
   return SendRequest(in_bBroadcast, in_DstAddr, IRIDIUM_MESSAGE_SYSTEM_PING);
}

/**
   Обработка полученого ответа на запроса пинга
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceivePingResponse()
{
   // Был получен ответ на пинг
   ReceivedPong();
   m_eError = IRIDIUM_OK;
}

#endif

#if defined(IRIDIUM_CONFIG_SYSTEM_PING_SLAVE)

/**
   Обработка полученого запроса пинга
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceivePingRequest()
{
   // Отправка ответа на пинг
   SendResponse(IRIDIUM_OK);
}

#endif

#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_MASTER)

/**
   Обработка полученого ответа на запрос поиска устройств
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSearchResponse()
{
   iridium_search_info_t l_Info;
   memset(&l_Info, 0, sizeof(l_Info));
   m_eError = IRIDIUM_PROTOCOL_CORRUPT;

   // Получение информации об удаленном устройстве
   if(m_pInMessage->GetSearchInfo(l_Info))
   {
      // Установка полученных данных
      SetSearchInfo(l_Info);
      m_eError = IRIDIUM_OK;
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_MASTER)

#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_SLAVE)

/**
   Обработка полученого запроса поиска устройств
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSearchRequest()
{
   u8 l_u8Mask = 0;
   iridium_device_info_t l_Info;
   iridium_search_info_t l_Search;
   memset(&l_Info, 0, sizeof(l_Info));
   // Получение маски поиска
   if(m_pInMessage->GetU8(l_u8Mask))
   {
      // Получение данных устройства и проверка поддерживает ли устройство идентификацию
      if(GetDeviceInfo(l_Info))
      {
         // Если устройство принадлежит к запрашиваемой группе, отправим информацию об устройстве
         if((l_Info.m_u8Flags & l_u8Mask) && GetSearchInfo(l_Search))
            SendSearchResponse(m_InMH.m_u16TID, l_Search);
      }
   }
   m_eError = IRIDIUM_OK;
}

/**
   Отправка успешного ответа на поиск устройств
   на входе    :  in_u16TID   - идентификатор транзакции
                  in_rInfo    - ссылка на данные устройства
   на выходе   :  успешность
   примечание  :  сообщение является широковещательным (без получателя) и отправляется в открытом виде
*/
bool CIridiumProtocol::SendSearchResponse(u16 in_u16TID, iridium_search_info_t& in_rInfo)
{
   // Инициализация пакета с ответом
   InitResponsePacket();

   // Заполнение заголовка сообщения
   m_OutMH.m_Flags.m_bNoTID      = (0 == in_u16TID);
   m_OutMH.m_Flags.m_u4Version   = GetMessageVersion(IRIDIUM_MESSAGE_SYSTEM_SEARCH);
   m_OutMH.m_u8Type              = IRIDIUM_MESSAGE_SYSTEM_SEARCH;
   m_OutMH.m_u16TID              = in_u16TID;

/*
   // Заполнение заголовка пакета
   m_OutPH.m_Flags.m_bSegment    = m_pInPH->m_Flags.m_bSegment;
   m_OutPH.m_Flags.m_bAddress    = true;
   m_OutPH.m_SrcAddr             = m_Address;
   m_OutPH.m_DstAddr             = m_pInPH->m_Flags.m_bAddress ? m_pInPH->m_SrcAddr : 0;

   // Заполнение заголовка сообщения
   m_OutMH.m_Flags.m_bDirection  = IRIDIUM_RESPONSE;
   m_OutMH.m_Flags.m_bError      = IRIDIUM_NO_ERROR;
   m_OutMH.m_Flags.m_bNoTID      = (0 == in_u16TID);
   m_OutMH.m_Flags.m_bEnd        = true;
   m_OutMH.m_Flags.m_u4Version   = GetMessageVersion(IRIDIUM_MESSAGE_SYSTEM_SEARCH);
   m_OutMH.m_u8Type              = IRIDIUM_MESSAGE_SYSTEM_SEARCH;
   m_OutMH.m_u16TID              = in_u16TID;
*/
   // Начало работы с пакетом
   Begin();
   // Добавление информации об устройстве в поток
   m_pOutMessage->AddSearchInfo(in_rInfo);
   // Окончание работы и отправка пакета
   return End();
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_SLAVE)

#if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_MASTER)

/**
   Отправка запроса на получение информации об удаленном сервере
   на входе    :  in_DstAddr  - адрес получателя
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendDeviceInfoRequest(iridium_address_t in_DstAddr)
{
   return SendRequest(false, in_DstAddr, IRIDIUM_MESSAGE_SYSTEM_DEVICE_INFO);
}

/**
   Обработка полученого ответа на запрос информации об устройстве
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveDeviceInfoResponse()
{
   iridium_device_info_t l_Info;
   // Получение информации об удаленном устройстве
   if(m_pInMessage->GetDeviceInfo(l_Info))
   {
      // Установка полученных данных
      SetDeviceInfo(l_Info);
      m_eError = IRIDIUM_OK;
   } else
      m_eError = IRIDIUM_PROTOCOL_CORRUPT;
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_MASTER)

#if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_SLAVE)

/**
   Обработка полученого запроса информации об устройстве
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveDeviceInfoRequest()
{
   bool l_bResult = false;
   iridium_device_info_t l_Info;
   memset(&l_Info, 0, sizeof(iridium_device_info_t));

   // Получение данных устройства
   if(GetDeviceInfo(l_Info))
   {
      // Инициализация пакета ответа
      InitResponsePacket();
      // Начало работы с пакетом
      Begin();
      // Добавление информации об устройстве в поток
      m_pOutMessage->AddDeviceInfo(l_Info);
      // Окончание работы и отправка пакета
      if(m_pOutMessage->Result())
         l_bResult = End();
   }
   // Проверка наличия ошибки
   if(!l_bResult)
      m_eError = IRIDIUM_UNKNOWN_ERROR;
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_SLAVE)

#if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_MASTER)

/**
   Обработка полученого ответа на запрос установки локального идентификатора
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSetLIDResponse()
{
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_MASTER)

#if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_SLAVE)

/**
   Обработка полученого запроса установки локального идентификатора
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSetLIDRequest()
{
   char* l_pszHWID = NULL;
   u8 l_u8LID = 0;
   u32 l_u32PIN = 0;
   m_eError = IRIDIUM_PROTOCOL_CORRUPT;
   // Получение HWID
   if(m_pInMessage->GetString(l_pszHWID))
   {
      // Получение нового адреса
      if(m_pInMessage->GetU8(l_u8LID))
      {
         m_eError = IRIDIUM_OK;
         // Проверка HWID
         if(CompareHWID(l_pszHWID))
         {
            // Получение PIN кода
            if(m_pInMessage->Size() >= sizeof(u32))
               m_pInMessage->GetU32LE(l_u32PIN);

            // Проверка возможности изменения значения
            s8 l_s8Result = CheckOperation(IRIDIUM_OPERATION_TEST_PIN, l_u32PIN, 0);
            if(l_s8Result > 0)
            {
               // Установки локального идентификатора
               SetLID(l_u8LID);
               SendResponse(IRIDIUM_OK);

               m_eError = IRIDIUM_OK;
            } else
            {
               // Определение ошибки
               m_eError = (l_s8Result < 0) ? IRIDIUM_PIN_LOCK : IRIDIUM_BAD_PIN;
            }
         }
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_SLAVE)

#if defined(IRIDIUM_CONFIG_SYSTEM_SESSION_TOKEN_MASTER)

/**
   Отправка запроса на подключение к облаку
   на входе    :  in_u64Token - токен доступа
   нв выходе   :  успешность
*/
bool CIridiumProtocol::SendSessionTokenRequest(u64 in_u64Token)
{
   // Заполнение заголовка пакета
   m_OutPH.m_Flags.m_bAddress = false; // Запрос всегда широковещательный
   m_OutPH.m_Flags.m_u3Crypt  = IRIDIUM_CRYPTION_NONE;

   // Заполнение заголовка сообщения
   m_OutMH.m_Flags.m_bDirection  = IRIDIUM_REQUEST;
   m_OutMH.m_Flags.m_bError = IRIDIUM_NO_ERROR;
   m_OutMH.m_Flags.m_bNoTID = true;
   m_OutMH.m_Flags.m_bEnd = true;
   m_OutMH.m_Flags.m_u4Version = GetMessageVersion(IRIDIUM_MESSAGE_SYSTEM_SESSION_TOKEN);
   m_OutMH.m_u8Type = IRIDIUM_MESSAGE_SYSTEM_SESSION_TOKEN;
   m_OutMH.m_u16TID = 0;

   // Начало работы с пакетом
   Begin();
   // Добавление токена
   m_pOutMessage->AddU64BE(in_u64Token);
   // Окончание работы и отправка пакета
   return End();
}

/**
   Обработка полученого ответа на запрос подключения к облаку
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSessionTokenResponse()
{
   // Получен ответ на передачу токена
   //SendHandshakeRequest(GetSrcAddress());
   m_eError = IRIDIUM_OK;
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SESSION_TOKEN_MASTER)

#if defined(IRIDIUM_CONFIG_SYSTEM_SESSION_TOKEN_SLAVE)

/**
   Обработка полученого запроса подключения к облаку
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSessionTokenRequest()
{
   m_eError = IRIDIUM_AUTHORIZATION_ERROR;
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SESSION_TOKEN_SLAVE)

#if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_MASTER)

/**
   Отправка запроса на получение информации о Smart API
   на входе    :  in_DstAddr  - адрес получателя
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendSmartAPIRequest(iridium_address_t in_DstAddr)
{
   return SendRequest(false, in_DstAddr, IRIDIUM_MESSAGE_SYSTEM_SMART_API);
}

/**
   Обработка полученого ответа на запрос получения информации о Smart API
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSmartAPIResponse()
{
   u8 l_u8Type = 0;
   universal_value_t l_Value;

   // Получение данных (от сервера - клинету)
   if(m_pInMessage->GetValue(l_u8Type, l_Value) && l_u8Type == IVT_ARRAY_U8)
   {
      // Отправка ответа в ядро
      SetSmartAPI(l_Value.m_Array.m_pPtr, l_Value.m_Array.m_stSize, m_InMH.m_Flags.m_bEnd);
      m_eError = IRIDIUM_OK;

   } else
      m_eError = IRIDIUM_PROTOCOL_CORRUPT;
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_MASTER)

#if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_SLAVE)

/**
   Обработка полученого запроса получения информации о Smart API
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSmartAPIRequest()
{
   bool l_bResult = true;
   size_t l_stRemain = ~0;//(size_t)-1;
   universal_value_t l_Value;

   // Заполнение данных пакета
   InitResponsePacket();

   // Подготовка данных
   l_Value.m_Array.m_pPtr = NULL;
   l_Value.m_Array.m_stSize = 0;

   // Получение данных о Smart API
   GetSmartAPI(l_Value.m_Array.m_pPtr, l_Value.m_Array.m_stSize);

   // Выполнять пока нет ошибки и все данные значения не переданы
   while(l_bResult && l_stRemain)
   {
      // Начало работы с пакетом
      Begin();
      // Добавление значения запроса
      l_bResult = m_pOutMessage->AddValue(IVT_ARRAY_U8, l_Value, l_stRemain, 0);
      if(l_bResult)
      {
         // Установка флага конца цепочки
         m_OutMH.m_Flags.m_bEnd = (l_stRemain == 0);
         // Окончание работы и отправка пакета
         l_bResult = End();
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_SLAVE)

#if defined(IRIDIUM_CONFIG_SET_VARIABLE_MASTER)

/**
   Запрос на отправку изменения глобальной переменной
   на входе    :  in_u16Variable - идентифкатор глобальной переменной
                  in_u8Type      - тип переменной
                  in_rValue      - ссылка на универсальное значение
   на выходе   :  успешность
   примечание  :  сообщение является широковещательным (без получателя), не требует ответа и не имеет идентификатора транзакции
*/
bool CIridiumProtocol::SendSetVariableRequest(u16 in_u16Variable, u8 in_u8Type, universal_value_t& in_rValue)
{
   bool l_bResult = false;
   size_t l_stRemain = ~0;//(size_t)-1;
   // Проверка входнях данных
   if(in_u16Variable)
   {
      // Инициализация широковещательного запроса
      InitRequestPacket(true, 0, IRIDIUM_MESSAGE_SET_VARIABLE);
      m_OutMH.m_Flags.m_bNoTID      = true;
      // Выполнять пока нет ошибки и все данные значения не переданы
      l_bResult = true;
      while(l_bResult && l_stRemain)
      {
         // Начало работы с пакетом
         Begin();
         // Добавление значение глобальной переменной
         l_bResult = m_pOutMessage->AddValue(in_u8Type, in_rValue, l_stRemain, 0);
         if(l_bResult)
         {
            // Добавление идентификатора глобальной переменной
            m_pOutMessage->AddU16LE(in_u16Variable);

            // Если есть хотябы один канал, закончим формирование пакета с последующей отправкой
            if(m_pOutMessage->Result())
            {
               // Установка флага конца цепочки
               m_OutMH.m_Flags.m_bEnd = (l_stRemain == 0);
               // Окончание работы и отправка пакета
               l_bResult = End();
            }
         }
      }
   }
   return l_bResult;
}

/**
   Обработка полученого ответа на запрос изменения глобальной переменной
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSetVariableResponse()
{
}

#endif   // #if defined(IRIDIUM_CONFIG_SET_VARIABLE_MASTER)

#if defined(IRIDIUM_CONFIG_SET_VARIABLE_SLAVE)

/**
   Обработка полученого запроса изменения глобальной переменной
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSetVariableRequest()
{
   u16 l_u16Variable = 0;
   u8 l_u8Type = 0;
   universal_value_t l_Value;
   // Чтение значения глобальной переменной
   m_pInMessage->GetValue(l_u8Type, l_Value);
   // Чтение идентификатора глобальной переменной
   m_pInMessage->GetU16LE(l_u16Variable);
   // Установка переменной
   SetVariable(l_u16Variable, l_u8Type, l_Value, IRIDIUM_FLAGS_SET | m_InMH.m_Flags.m_bEnd);
}

#endif   // #if defined(IRIDIUM_CONFIG_SET_VARIABLE_SLAVE)

#if defined(IRIDIUM_CONFIG_GET_VARIABLE_MASTER)

/**
   Запрос на отправку изменения глобальных переменных
   на входе    :  in_u16Variable - указатель на массив идентифкаторов глобальных переменных
   на выходе   :  успешность
   примечание  :  сообщение является широковещательным (без получателя)
*/
bool CIridiumProtocol::SendGetVariableRequest(u16 in_u16Variable)
{
   bool l_bResult = false;
   // Проверка входнях данных
   if(in_u16Variable)
   {
      // Инициализация широковещательного запроса
      InitRequestPacket(true, 0, IRIDIUM_MESSAGE_SET_VARIABLE);
      // Начало работы с пакетом
      Begin();
      // Добавляем идентификатор глобальной переменной
      m_pOutMessage->AddU16LE(in_u16Variable);
      // Окончание работы и отправка пакета
      l_bResult = End();
   }
   return l_bResult;
}

/**
   Обработка полученого ответа на запроса изменения глобальной переменной
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetVariableResponse()
{
   u16 l_u16Variable = 0;
   u8 l_u8Type = 0;
   universal_value_t l_Value;
   // Чтение идентификатора глобальной переменной
   m_pInMessage->GetU16LE(l_u16Variable);
   // Чтение значения глобальной переменной
   m_pInMessage->GetValue(l_u8Type, l_Value);
   // Установка переменной
   SetVariable(l_u16Variable, l_u8Type, l_Value, m_InMH.m_Flags.m_bEnd);
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_VARIABLE_MASTER)

#if defined(IRIDIUM_CONFIG_GET_VARIABLE_SLAVE)

/**
   Обработка полученого запроса изменения глобальной переменной
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetVariableRequest()
{
   u16 l_u16Variable = 0;
   u8 l_u8Type = 0;
   universal_value_t l_Value;

   // Чтение идентификатора глобальной переменной
   m_pInMessage->GetU16LE(l_u16Variable);
   // Если устройство является владельцем переменной, отправим ответ
   if(GetVariable(l_u16Variable, l_u8Type, l_Value))
      SendGetVariableResponse(l_u16Variable, l_u8Type, l_Value);
}

/**
   Отправка ответа на запрос списка тегов с архивными данными
   на входе    :  in_u16Variable - идентификатор глобальной переменной
                  in_u8Type      - тип глобальной переменной
                  in_rValue      - ссылка на значение глобальной переменной
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendGetVariableResponse(u16 in_u16Variable, u8 in_u8Type, universal_value_t& in_rValue)
{
   size_t l_stPosition = ~0;//(size_t)-1;

   // Инициализация пакета ответа
   InitResponsePacket();

   // Заполнение заголовка пакета
   m_OutPH.m_Flags.m_bAddress    = false; // ответ всегда широковещательный
/*
   // Заполнение заголовка сообщения
   m_OutMH.m_Flags.m_bDirection  = IRIDIUM_RESPONSE;
   m_OutMH.m_Flags.m_bError      = IRIDIUM_NO_ERROR;
   m_OutMH.m_Flags.m_bNoTID      = false;
   m_OutMH.m_Flags.m_bEnd        = true;
   m_OutMH.m_Flags.m_u4Version   = GetMessageVersion(IRIDIUM_MESSAGE_GET_VARIABLE);
   m_OutMH.m_u8Type              = IRIDIUM_MESSAGE_GET_VARIABLE;
   m_OutMH.m_u16TID              = m_InMH.m_u16TID;
*/
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора глобальной переменной
   m_pOutMessage->AddU16LE(in_u16Variable);
   // Добавление значения глобальной переменной
   m_pOutMessage->AddValue(in_u8Type, in_rValue, l_stPosition, 0);
   // Окончание работы и отправка пакета
   return End();
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_VARIABLE_SLAVE)

#if defined(IRIDIUM_CONFIG_GET_TAGS_MASTER)

/**
   Отправка запроса на получение списка тегов
   на входе    :  in_DstAddr  - адрес получателя
   нв выходе   :  успешность
*/
bool CIridiumProtocol::SendGetTagsRequest(iridium_address_t in_DstAddr)
{
   return SendRequest(false, in_DstAddr, IRIDIUM_MESSAGE_GET_TAGS);
}

/**
   Обработка полученого ответа на запроса получения списка каналов обратной связи
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetTagsResponse()
{
   u16 l_u16Tags = 0;
   iridium_tag_info_t l_Info;
   // Получение количества тегов
   m_pInMessage->GetU16LE(l_u16Tags);

   // Чтение списка тегов
   for(u16 i = 0; i < l_u16Tags; i++)
   {
      // Получение идентификатора, имени и значения тега
      m_pInMessage->GetU32LE(l_Info.m_u32ID);
      m_pInMessage->GetString(l_Info.m_pszName);
      m_pInMessage->GetValue(l_Info.m_u8Type, l_Info.m_Value);
      // Отправка полученных данных родителю
      SetTagData(l_Info);
   }
   if(m_InMH.m_Flags.m_bEnd)
      EndTags();
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_TAGS_MASTER)

#if defined(IRIDIUM_CONFIG_GET_TAGS_SLAVE)

/**
   Обработка полученого запроса на получение списка каналов обратной связи
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetTagsRequest()
{
   bool l_bResult = true;
   iridium_tag_info_t l_Tag;
   size_t l_stPosition = ~0;//(size_t)-1;

   // Инициализация пакета ответа
   InitResponsePacket();

   // Получение количества тегов
   size_t l_stSize = GetTags();
   size_t l_stCurrent = 0;
   while(l_bResult && l_stCurrent < l_stSize)
   {
      u16 l_u16Count = 0;
      // Начало работы с пакетом
      Begin();
      // Зарезервируем количество тегов
      u8* l_pAnchor = m_pOutMessage->CreateAnchor(2);
      // Обработка списка тегов
      for( ; l_stCurrent < l_stSize; l_stCurrent++)
      {
         // Получение данных тега
         size_t l_stLen = GetTagData(l_stCurrent, l_Tag, sizeof(l_Tag));
         if(l_stLen)
         {
            l_stPosition = ~0;//(size_t)-1;
            // Проверка свободного места в буфере для помещения данных канала обратной связи, иначе остановка добавления данных
            if(m_pOutMessage->Free() > l_stLen)
            {
               // Запись данных тега
               m_pOutMessage->AddU32LE(l_Tag.m_u32ID);
#if defined(IRIDIUM_MCU_AVR)
               m_pOutMessage->AddStringFromFlash(l_Tag.m_pszName);
#else
               m_pOutMessage->AddString(l_Tag.m_pszName);
#endif
               m_pOutMessage->AddValue(l_Tag.m_u8Type, l_Tag.m_Value, l_stPosition, 0);
               l_u16Count++;
            } else
               break;
         }
      }
      // Установка флага конца цепочки
      m_OutMH.m_Flags.m_bEnd = (l_stCurrent == l_stSize);
      // Обновим количество тегов
      m_pOutMessage->SetAnchorLE(l_pAnchor, &l_u16Count, 2);
      // Окончание работы и отправка пакета
      l_bResult = End();
   }
   // Проверка наличия ошибки
   if(!l_bResult)
      m_eError = IRIDIUM_UNKNOWN_ERROR;
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_TAGS_SLAVE)

#if defined(IRIDIUM_CONFIG_LINK_TAG_AND_VARIABLE_MASTER)

/**
   Отправка запроса связывания канала обратной связи и глобальной переменной
   на входе    :  in_DstAddr        - адрес получателя
                  in_u32TagID       - идентификатор канала обратной связи сервера
                  in_bOwner         - флаг владения глобальной переменной
                  in_u16Variable    - идентификатор глобальной переменной
                  in_u32PIN         - PIN для линковки
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendLinkTagAndVariableRequest(iridium_address_t in_DstAddr, u32 in_u32TagID, bool in_bOwner, u16 in_u16Variable, u32 in_u32PIN)
{
   u8 l_u8Temp = 0;

   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_LINK_TAG_AND_VARIABLE);
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора канала обратной связи
   m_pOutMessage->AddU32LE(in_u32TagID);
   // Зарезирвируем место под флаги
   u8* l_pAnchor = m_pOutMessage->CreateAnchor(1);
   // Добавление PIN кода
   if(in_u32PIN)
   {
      m_pOutMessage->AddU32LE(in_u32PIN);
      // Добавим флаг наличия PIN код
      l_u8Temp |= 0x40;
   }
   // Добавление идентификатора глобальной переменной
   if(in_u16Variable)
   {
      m_pOutMessage->AddU16LE(in_u16Variable);
      // Добавим флаг наличия переменной
      l_u8Temp |= 0x80;
   }
   // Добавление флага владения
   l_u8Temp |= (u8)in_bOwner;
   // Запись флагов
   m_pOutMessage->SetAnchorLE(l_pAnchor, &l_u8Temp, 1);

   // Окончание работы и отправка пакета
   return m_pOutMessage->Result() ? End() : false;
}

/**
   Обработка полученого ответа на запрос связывания канала обратной связи и глобальной переменной
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveLinkTagAndVariableResponse()
{
}

#endif      // #if defined(IRIDIUM_CONFIG_LINK_TAG_AND_VARIABLE_MASTER)

#if defined(IRIDIUM_CONFIG_LINK_TAG_AND_VARIABLE_SLAVE)

/**
   Обработка полученого запроса связывания канала обратной связи и глобальной переменной
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveLinkTagAndVariableRequest()
{
   u32 l_u32ID = 0;
   u16 l_u16Variable = 0;
   u8 l_u8Flags = 0;
   u32 l_u32PIN = 0;
   m_eError = IRIDIUM_PROTOCOL_CORRUPT;

   // Получение идентификатора канала обратной связи
   if(m_pInMessage->GetU32LE(l_u32ID))
   {
      // Получение списка флагов
      if(m_pInMessage->GetU8(l_u8Flags))
      {
         // Получение PIN кода
         if(l_u8Flags & 0x40)
            m_pInMessage->GetU32LE(l_u32PIN);
         // Получение глобальной переменной
         if(l_u8Flags & 0x80)
            m_pInMessage->GetU16LE(l_u16Variable);

         // Проверка возможности изменения значения
         s8 l_s8Result = CheckOperation(IRIDIUM_OPERATION_TEST_PIN, l_u32PIN, &l_u32ID);
         if(l_s8Result > 0)
         {
            // Связывание канала обратной связи и глобальной переменной
            if(LinkTagAndVariable(l_u32ID, l_u8Flags & 1, l_u16Variable))
            {
               SendResponse(IRIDIUM_OK);
               m_eError = IRIDIUM_OK;
            } else
               m_eError = IRIDIUM_ERROR_BAD_TAG_ID;
         } else
         {
            // Определение ошибки
            m_eError = (l_s8Result < 0) ? IRIDIUM_PIN_LOCK : IRIDIUM_BAD_PIN;
         }
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_LINK_TAG_AND_VARIABLE_SLAVE)

#if defined(IRIDIUM_CONFIG_GET_TAG_DESCRIPTION_MASTER)

/**
   Отправка запроса информации о канале обратной связи
   на входе    :  in_DstAddr  - адрес получателя
                  in_u32TagID - идентификатор канала обратной связи устройства
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendGetTagDescriptionRequest(iridium_address_t in_DstAddr, u32 in_u32TagID)
{
   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_GET_TAG_DESCRIPTION);
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора канала обратной связи
   m_pOutMessage->AddU32LE(in_u32TagID);
   // Окончание работы и отправка пакета
   return End();
}

/**
   Обработка полученого ответа на запрос информации о канале обратной связи
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetTagDescriptionResponse()
{
   u32 l_u32TagID = 0;
   iridium_tag_description_t l_Desc;

   // Получение идентификатора канала обратной связи
   m_pInMessage->GetU32LE(l_u32TagID);
   // Поучение описания канала обратной связи
   if(m_pInMessage->GetTagDescription(l_Desc))
      SetTagDescription(l_u32TagID, l_Desc);
   else
      m_eError = IRIDIUM_PROTOCOL_CORRUPT;
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_TAG_DESCRIPTION_MASTER)

#if defined(IRIDIUM_CONFIG_GET_TAG_DESCRIPTION_SLAVE)

/**
   Обработка полученого запроса информации о канале обратной связи
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetTagDescriptionRequest()
{
   u32 l_u32TagID = 0;
   iridium_tag_description_t l_Desc;
   memset(&l_Desc, 0, sizeof(l_Desc));

   // Получение идентификатора канала обратной связи
   m_pInMessage->GetU32LE(l_u32TagID);
   // Получение описания канала обратной связи
   if(GetTagDescription(l_u32TagID, l_Desc))
   {
      // Инициализация пакета ответа
      InitResponsePacket();
      // Начало работы с пакетом
      Begin();
      // Добавление идентификатора канала управления
      m_pOutMessage->AddU32LE(l_u32TagID);
      // Добавление описания канала управления
      m_pOutMessage->AddTagDescription(l_Desc);
      // Окончание работы и отправка пакета
      if(!End())
         m_eError = IRIDIUM_UNKNOWN_ERROR;
   } else
      m_eError = IRIDIUM_ERROR_BAD_TAG_ID;
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_TAG_DESCRIPTION_SLAVE)

#if defined(IRIDIUM_CONFIG_SET_TAG_VALUE_MASTER)

/**
   Запрос на отправку значения канала обратной связи
   на входе    :  in_DstAddr  - адрес получателя
                  in_u32TagID - идентифкатор канала обратной связи
                  in_u8Type   - тип значения
                  in_rValue   - ссылка на данные значения
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendSetTagValueRequest(iridium_address_t in_DstAddr, u32 in_u32TagID, u8 in_u8Type, universal_value_t& in_rValue)
{
   bool l_bResult = true;
   size_t l_stRemain = ~0;//(size_t)-1;
   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_SET_TAG_VALUE);

   // Выполнять пока нет ошибки и все данные значения не переданы
   while(l_bResult && l_stRemain)
   {
      // Начало работы с пакетом
      Begin();
      // Добавление идентификатора канала обратной связи
      m_pOutMessage->AddU32LE(in_u32TagID);
      if(m_pOutMessage->Result())
      {
         // Добавление значения канала обратной связи
         m_pOutMessage->AddValue(in_u8Type, in_rValue, l_stRemain, 0);
         if(m_pOutMessage->Result())
         {
            // Установка флага конца цепочки
            m_OutMH.m_Flags.m_bEnd = (l_stRemain == 0);
            // Окончание работы и отправка пакета
            l_bResult = End();
         }
      }
   }
   return l_bResult;
}

/**
   Обработка полученого ответа на запрос установки значения канала
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSetTagValueResponse()
{
}

#endif   // #if defined(IRIDIUM_CONFIG_SET_TAG_VALUE_MASTER)

#if defined(IRIDIUM_CONFIG_SET_TAG_VALUE_SLAVE)

/**
   Обработка полученого запроса установки значения канала
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSetTagValueRequest()
{
   u8 l_u8Type = 0;
   universal_value_t l_Value;
   u32 l_u32ID = 0;
   m_eError = IRIDIUM_PROTOCOL_CORRUPT;

   // Получение идентификатора изменившего канала обратной связи
   if(m_pInMessage->GetU32LE(l_u32ID))
   {
      // Получение значения изменившего канала обратной связи
      if(m_pInMessage->GetValue(l_u8Type, l_Value))
      {
         // Изменение значения канала управления
         if(SetTagValue(l_u32ID, l_u8Type, l_Value, IRIDIUM_FLAGS_SET | m_InMH.m_Flags.m_bEnd))
         {
            SendResponse(IRIDIUM_OK);
            m_eError = IRIDIUM_OK;
         } else
            m_eError = IRIDIUM_ERROR_BAD_TAG_ID;
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_SET_TAG_VALUE_SLAVE)

#if defined(IRIDIUM_CONFIG_GET_TAG_VALUE_MASTER)

/**
   Запрос на отправку значения канала обратной связи
   на входе    :  in_DstAddr  - адрес получателя
                  in_u32TagID - идентифкатор канала обратной связи
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendGetTagValueRequest(iridium_address_t in_DstAddr, u32 in_u32TagID)
{
   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_GET_TAG_VALUE);
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора канала управления
   m_pOutMessage->AddU32LE(in_u32TagID);
   // Окончание работы и отправка пакета
   return End();
}

/**
   Обработка полученного ответа на запрос установки значения канала
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetTagValueResponse()
{
   u8 l_u8Type = 0;
   universal_value_t l_Value;
   u32 l_u32ID = 0;
   // Получение идентификатора канала обратной связи
   if(m_pInMessage->GetU32LE(l_u32ID))
   {
      // Получение значения канала обратной связи
      if(m_pInMessage->GetValue(l_u8Type, l_Value))
      {
         // Оповещение о получении значения канала обратной связи
         SetTagValue(l_u32ID, l_u8Type, l_Value, m_InMH.m_Flags.m_bEnd);
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_TAG_VALUE_MASTER)

#if defined(IRIDIUM_CONFIG_GET_TAG_VALUE_SLAVE)

/**
   Обработка полученного запроса установки значения канала
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetTagValueRequest()
{
   u32 l_u32ID = 0;
   iridium_tag_info_t l_Tag;
   // Получение идентификатора канала обратной связи
   if(m_pInMessage->GetU32LE(l_u32ID))
   {
      // Получение индекса канала обратной связи
      size_t l_stIndex = GetTagIndex(l_u32ID);
      if(l_stIndex != ~0)//(size_t)-1)
      {
         // Получение данных канала обратной связи
         if(GetTagData(l_stIndex, l_Tag, sizeof(l_Tag)))
         {
            // Отправка значения канала обратной связи
            SendGetTagValueResponse(l_u32ID, l_Tag);
         }
      } else
         m_eError = IRIDIUM_ERROR_BAD_TAG_ID;
   }
}

/**
   Отправка ответа на запрос значения канала обратной связи
   на входе    :  in_u32TagID - идентифкатор канала обратной связи
                  in_rInfo    - ссылка на данные канала обратной связи
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendGetTagValueResponse(u32 in_u32TagID, iridium_tag_info_t& in_rInfo)
{
   size_t l_stRemain = ~0;//(size_t)-1;

   // Инициализация пакета ответа
   InitResponsePacket();

   // Выполнять пока нет ошибки и все данные значения не переданы
   while(m_pOutMessage->Result() && l_stRemain)
   {
      // Начало работы с пакетом
      Begin();
      // Добавление идентификатора канала управления
      m_pOutMessage->AddU32LE(in_u32TagID);
      // Добавление значения тега
      m_pOutMessage->AddValue(in_rInfo.m_u8Type, in_rInfo.m_Value, l_stRemain, 0);
      // Установка флага конца цепочки
      m_OutMH.m_Flags.m_bEnd = (l_stRemain == 0);
      // Окончание работы и отправка пакета
      End();
   }
   return m_pOutMessage->Result();
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_TAG_VALUE_SLAVE)

#if defined(IRIDIUM_CONFIG_GET_CHANNELS_MASTER)

/**
   Отправка запроса на получение списка тегов
   на входе    :  in_DstAddr  - адрес получателя
   нв выходе   :  успешность
*/
bool CIridiumProtocol::SendGetChannelsRequest(iridium_address_t in_DstAddr)
{
   return SendRequest(false, in_DstAddr, IRIDIUM_MESSAGE_GET_CHANNELS);
}

/**
   Обработка полученого ответа на запрос каналов управления сервера
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetChannelsResponse()
{
   u16 l_u16Channels = 0;
   iridium_channel_info_t l_Info;
   memset(&l_Info, 0, sizeof(iridium_channel_info_t));

   // Получение количества тегов
   m_pInMessage->GetU16LE(l_u16Channels);

   // Чтение списка тегов
   for(u16 i = 0; i < l_u16Channels; i++)
   {
      // Получение идентификатора и имени канала
      m_pInMessage->GetU32LE(l_Info.m_u32ID);
      m_pInMessage->GetString(l_Info.m_pszName);

      // Отправка родителю данных полученного канала управления
      if(l_Info.m_pszName)
         SetChannelData(l_Info);
   }
   if(m_InMH.m_Flags.m_bEnd)
      EndChannels();
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_CHANNELS_MASTER)

#if defined(IRIDIUM_CONFIG_GET_CHANNELS_SLAVE)

/**
   Обработка полученого запроса каналов управления сервера
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetChannelsRequest()
{
   bool l_bResult = true;
   iridium_channel_info_t l_Channel;

   // Инициализация пакета ответа
   InitResponsePacket();

   // Получение количества каналов
   size_t l_stSize = GetChannels();
   size_t l_stCurrent = 0;
   while(l_bResult && l_stCurrent < l_stSize)
   {
      // Количество каналов управления в пакете
      u16 l_u16Count = 0;
      // Начало работы с пакетом
      Begin();

      // Создание точки для записи количества каналов
      u8* l_pAnchor = m_pOutMessage->CreateAnchor(2);
      // Обработка списка каналов управления
      for( ; l_stCurrent < l_stSize; l_stCurrent++)
      {
         // Получение данных канала управления
         size_t l_stLen = GetChannelData(l_stCurrent, l_Channel);
         if(l_stLen)
         {
            if(m_pOutMessage->Free() > l_stLen)
            {
               // Запись данных тега
               m_pOutMessage->AddU32LE(l_Channel.m_u32ID);
#if defined(IRIDIUM_MCU_AVR)
               m_pOutMessage->AddStringFromFlash(l_Channel.m_pszName);
#else
               m_pOutMessage->AddString(l_Channel.m_pszName);
#endif
               l_u16Count++;
            } else
               break;
         }
      }
      // Установка флага конца цепочки
      m_OutMH.m_Flags.m_bEnd = (l_stCurrent == l_stSize);
      // Запись количества каналов
      m_pOutMessage->SetAnchorLE(l_pAnchor, &l_u16Count, 2);
      // Окончание работы и отправка пакета
      l_bResult = End();
   }
   // Проверка на наличие ошибки
   if(!l_bResult)
      m_eError = IRIDIUM_UNKNOWN_ERROR;
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_CHANNELS_SLAVE)

#if defined(IRIDIUM_CONFIG_SET_CHANNEL_VALUE_MASTER)

/**
   Запрос на отправку значения канала управления
   на входе    :  in_DstAddr        - адрес получателя
                  in_u32ChannelID   - идентифкатор канала управления
                  in_u8Type         - тип значения
                  in_rValue         - ссылка на данные значения
                  in_u32PIN         - пароль доступа для записи значения
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendSetChannelValueRequest(iridium_address_t in_DstAddr, u32 in_u32ChannelID, u8 in_u8Type, universal_value_t& in_rValue, u32 in_u32PIN)
{
   size_t l_stRemain = ~0;//(size_t)-1;
   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_SET_CHANNEL_VALUE);

   // Выполнять пока нет ошибки и все данные значения не переданы
   while(m_pOutMessage->Result() && l_stRemain)
   {
      // Начало работы с пакетом
      Begin();
      // Добавление идентификатора канала управления
      m_pOutMessage->AddU32LE(in_u32ChannelID);
      // Добавление значения тега
      m_pOutMessage->AddValue(in_u8Type, in_rValue, l_stRemain, 0);
      // Добавление PIN кода
      if(in_u32PIN)
         m_pOutMessage->AddU32LE(in_u32PIN);
      // Установка флага конца цепочки
      m_OutMH.m_Flags.m_bEnd = (l_stRemain == 0);
      // Окончание работы и отправка пакета
      End();
   }

   return m_pOutMessage->Result();
}

/**
   Обработка полученого ответа на запрос установки значения канала управления
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSetChannelValueResponse()
{
}

#endif   // #if defined(IRIDIUM_CONFIG_SET_CHANNEL_VALUE_MASTER)

#if defined(IRIDIUM_CONFIG_SET_CHANNEL_VALUE_SLAVE)

/**
   Обработка полученого запроса установки значения канала управления
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveSetChannelValueRequest()
{
   u8 l_u8Type = 0;
   universal_value_t l_Value;
   u32 l_u32ID = 0;
   u32 l_u32PIN = 0;
   m_eError = IRIDIUM_PROTOCOL_CORRUPT;

   // Получение идентификатора изменившего канала управления
   if(m_pInMessage->GetU32LE(l_u32ID))
   {
      // Получение значения изменившего канала управления
      if(m_pInMessage->GetValue(l_u8Type, l_Value))
      {
         // Получение пароля доступа к каналу
         if(m_pInMessage->Size() >= sizeof(u32))
            m_pInMessage->GetU32LE(l_u32PIN);

         // Проверка возможности изменения значения
         s8 l_s8Result = CheckOperation(IRIDIUM_OPERATION_WRITE_CHANNEL, l_u32PIN, &l_u32ID);
         if(l_s8Result > 0)
         {
            // Изменение значения канала управления
            if(SetChannelValue(l_u32ID, l_u8Type, l_Value, IRIDIUM_FLAGS_SET | m_InMH.m_Flags.m_bEnd))
            {
               SendResponse(IRIDIUM_OK);
               m_eError = IRIDIUM_OK;
            } else
               m_eError = IRIDIUM_ERROR_BAD_CHANNEL_ID;
         } else
         {
            // Определение ошибки
            m_eError = (l_s8Result < 0) ? IRIDIUM_PIN_LOCK : IRIDIUM_BAD_PIN;
         }
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_SET_CHANNEL_VALUE_SLAVE)

#if defined(IRIDIUM_CONFIG_LINK_CHANNEL_AND_VARIABLE_MASTER)

/**
   Отправка запроса связывания канала управления с глобальной переменной
   на входе    :  in_DstAddr        - адрес получателя
                  in_u32ChannelID   - идентификатор канала управления сервера
                  in_u8Variables    - количество переменных
                  in_pVariables     - указатель на массив идентификаторов глобальных переменных
                  in_u32PIN         - PIN для линковки
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendLinkChannelAndVariableRequest(iridium_address_t in_DstAddr, u32 in_u32ChannelID, u8 in_u8Variables, u16* in_pVariables, u32 in_u32PIN)
{
   u8 l_u8Temp = 0;

   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_LINK_CHANNEL_AND_VARIABLE);
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора канала управления
   m_pOutMessage->AddU32LE(in_u32ChannelID);
   // Зарезервируем место под флаги
   u8* l_pAnchor = m_pOutMessage->CreateAnchor(1);
   // Добавление PIN кода
   if(in_u32PIN)
   {
      m_pOutMessage->AddU32LE(in_u32PIN);
      // Отметим наличие PIN кода
      l_u8Temp = 0x40;
   }
   // Проверка наличия списка глобальных переменных
   if(in_u8Variables && in_pVariables)
   {
      // Проверка максимального количества переменных
      if(in_u8Variables > IRIDIUM_MAX_VARIABLES)
         in_u8Variables = IRIDIUM_MAX_VARIABLES;
   
      // Добавление списка глобальных переменных
      for(u8 i = 0; i < in_u8Variables; i++)
         m_pOutMessage->AddU16LE(in_pVariables[i]);
      // Отметим наличие данных
      l_u8Temp |= (0x80 | (in_u8Variables - 1) & 0x1f);
   }

   // Добавление списка флагов
   m_pOutMessage->SetAnchorLE(l_pAnchor, &l_u8Temp, 1);
   // Окончание работы и отправка пакета
   return End();
}

/**
   Обработка полученого ответа на запрос связывания канала и переменной
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveLinkChannelAndVariableResponse()
{
}

#endif   // #if defined(IRIDIUM_CONFIG_LINK_CHANNEL_AND_VARIABLE_MASTER)

#if defined(IRIDIUM_CONFIG_LINK_CHANNEL_AND_VARIABLE_SLAVE)

/**
   Обработка полученого запроса связывания канала и переменной
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveLinkChannelAndVariableRequest()
{
   u32 l_u32ID = 0;
   u32 l_u32PIN = 0;
   u8 l_u8Temp = 0;
   u8 l_u8Count = 0;
   u16 l_aVariables[IRIDIUM_MAX_VARIABLES];
   m_eError = IRIDIUM_PROTOCOL_CORRUPT;

   // Получение идентификатора канала управления
   if(m_pInMessage->GetU32LE(l_u32ID))
   {
      // Вычисление количества глобальных переменных
      if(m_pInMessage->GetU8(l_u8Temp))
      {
         // Получение PIN кода
         if(l_u8Temp & 0x40)
            m_pInMessage->GetU32LE(l_u32PIN);

         // Проверка наличия списка глобальных переменных
         if(l_u8Temp & 0x80)
         {
            // Получение количества глобальных переменных
            l_u8Count = (l_u8Temp & 0x1f) + 1;

            // Ограничение на количество переменных
            if(l_u8Count > IRIDIUM_MAX_VARIABLES)
               l_u8Count = IRIDIUM_MAX_VARIABLES;

            // Получение списка глобальных переменных
            for(u8 i = 0; i < l_u8Count; i++)
               m_pInMessage->GetU16LE(l_aVariables[i]);
         }

         // Проверка возможности изменения значения
         s8 l_s8Result = CheckOperation(IRIDIUM_OPERATION_TEST_PIN, l_u32PIN, &l_u32ID);
         if(l_s8Result > 0)
         {
            // Связывание канала управления со списком глобальных переменных
            if(LinkChannelAndVariable(l_u32ID, l_u8Count, l_aVariables))
            {
               SendResponse(IRIDIUM_OK);
               m_eError = IRIDIUM_OK;
            }
            else
               m_eError = IRIDIUM_ERROR_BAD_CHANNEL_ID;
         } else
         {
            // Определение ошибки
            m_eError = (l_s8Result < 0) ? IRIDIUM_PIN_LOCK : IRIDIUM_BAD_PIN;
         }
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_LINK_CHANNEL_AND_VARIABLE_SLAVE)

#if defined(IRIDIUM_CONFIG_GET_CHANNEL_DESCRIPTION_MASTER)

/**
   Отправка запроса информации о канале управления
   на входе    :  in_DstAddr        - адрес получателя
                  in_u32ChannelID   - идентификатор канала управления устройства
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendGetChannelDescriptionRequest(iridium_address_t in_DstAddr, u32 in_u32ChannelID)
{
   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_GET_CHANNEL_DESCRIPTION);
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора тега
   m_pOutMessage->AddU32LE(in_u32ChannelID);
   // Окончание работы и отправка пакета
   return End();
}

/**
   Обработка полученого ответа на запрос информации о канале управления
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetChannelDescriptionResponse()
{
   u32 l_u32ChannelID = 0;
   iridium_channel_description_t l_Desc;

   m_eError = IRIDIUM_PROTOCOL_CORRUPT;
   // Получение идентификатора канала управления
   if(m_pInMessage->GetU32LE(l_u32ChannelID))
   {
      // Поучение описания канала управления
      if(m_pInMessage->GetChannelDescription(l_Desc))
      {
         SetChannelDescription(l_u32ChannelID, l_Desc);
         m_eError = IRIDIUM_OK;
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_CHANNEL_DESCRIPTION_MASTER)

#if defined(IRIDIUM_CONFIG_GET_CHANNEL_DESCRIPTION_SLAVE)

/**
   Обработка полученого запроса информации о канале управления
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetChannelDescriptionRequest()
{
   u32 l_u32ChannelID = 0;
   iridium_channel_description_t l_Desc;
   memset(&l_Desc, 0, sizeof(iridium_channel_description_t));

   m_eError = IRIDIUM_PROTOCOL_CORRUPT;
   // Получение идентификатора канала управления
   if(m_pInMessage->GetU32LE(l_u32ChannelID))
   {
      // Получение описания канала управления
      if(GetChannelDescription(l_u32ChannelID, l_Desc))
      {
         // Инициализация пакета ответа
         InitResponsePacket();
         // Начало работы с пакетом
         Begin();
         // Добавление идентификатора канала управления
         m_pOutMessage->AddU32LE(l_u32ChannelID);
         // Добавление описания канала управления
         m_pOutMessage->AddChannelDescription(l_Desc);
         // Окончание работы и отправка пакета
         if(End())
            m_eError = IRIDIUM_OK;
         else
            m_eError = IRIDIUM_UNKNOWN_ERROR;
      } else
         m_eError = IRIDIUM_ERROR_BAD_CHANNEL_ID;
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_CHANNEL_DESCRIPTION_SLAVE)

#if defined(IRIDIUM_CONFIG_GET_CHANNEL_VALUE_MASTER)

/**
   Запрос на отправку значения канала управления
   на входе    :  in_DstAddr        - адрес получателя
                  in_u32ChannelID   - идентифкатор канала управления
                  in_u32PIN         - пароль доступа для чтения значения
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendGetChannelValueRequest(iridium_address_t in_DstAddr, u32 in_u32ChannelID, u32 in_u32PIN)
{
   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_GET_CHANNEL_VALUE);

   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора канала управления
   m_pOutMessage->AddU32LE(in_u32ChannelID);
   // Добавление PIN кода
   if(in_u32PIN)
      m_pOutMessage->AddU32LE(in_u32PIN);
   // Окончание работы и отправка пакета
   return End();
}

/**
   Обработка полученого ответа на запрос установки значения канала
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetChannelValueResponse()
{
   u8 l_u8Type = 0;
   universal_value_t l_Value;
   u32 l_u32ID = 0;
   // Получение идентификатора канала управления
   if(m_pInMessage->GetU32LE(l_u32ID))
   {
      // Получение значения канала управления
      if(m_pInMessage->GetValue(l_u8Type, l_Value))
      {
         // Оповещение о получении значения канала управления
         SetChannelValue(l_u32ID, l_u8Type, l_Value, m_InMH.m_Flags.m_bEnd);
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_CHANNEL_VALUE_MASTER)

#if defined(IRIDIUM_CONFIG_GET_CHANNEL_VALUE_SLAVE)

/**
   Обработка полученого запроса установки значения канала
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveGetChannelValueRequest()
{
   u32 l_u32ID = 0;
   u32 l_u32PIN = 0;
   iridium_channel_info_t l_Channel;
   // Получение идентификатора канала управления
   if(m_pInMessage->GetU32LE(l_u32ID))
   {
      // Получение PIN, если таковой есть
      if(m_pInMessage->Size() >= sizeof(u32))
         m_pInMessage->GetU32LE(l_u32PIN);

      // Получение индекса канала управления
      size_t l_stIndex = GetChannelIndex(l_u32ID);
      if(l_stIndex != ~0)//(size_t)-1)
      {
         // Проверка PIN кода
         s8 l_s8Result = CheckOperation(IRIDIUM_OPERATION_READ_CHANNEL, l_u32PIN, &l_u32ID);
         if(l_s8Result > 0)
         {
            // Получение данных канала обратной связи
            if(GetChannelData(l_stIndex, l_Channel))
            {
               // Отправка значения канала управления
               SendGetChannelValueResponse(l_u32ID, l_Channel);
            }
         } else
         {
            // Определение ошибки
            m_eError = (l_s8Result < 0) ? IRIDIUM_PIN_LOCK : IRIDIUM_BAD_PIN;
         }
      } else
         m_eError = IRIDIUM_ERROR_BAD_CHANNEL_ID;
   }
}

/**
   Отправка ответа на запрос значения канала управления
   на входе    :  in_u32ChannelID   - идентифкатор канала управления
                  in_rInfo          - ссылка на данные канала управления
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendGetChannelValueResponse(u32 in_u32ChannelID, iridium_channel_info_t& in_rInfo)
{
   size_t l_stRemain = ~0;//(size_t)-1;

   // Инициализация пакета ответа
   InitResponsePacket();

   // Выполнять пока нет ошибки и все данные значения не переданы
   while(m_pOutMessage->Result() && l_stRemain)
   {
      // Начало работы с пакетом
      Begin();
      // Добавление идентификатора канала управления
      m_pOutMessage->AddU32LE(in_u32ChannelID);
      // Добавление значения тега
      m_pOutMessage->AddValue(in_rInfo.m_u8Type, in_rInfo.m_Value, l_stRemain, 0);
      // Установка флага конца цепочки
      m_OutMH.m_Flags.m_bEnd = (l_stRemain == 0);
      // Окончание работы и отправка пакета
      End();
   }
   return m_pOutMessage->Result();
}

#endif   // #if defined(IRIDIUM_CONFIG_GET_CHANNEL_VALUE_SLAVE)

#if defined(IRIDIUM_CONFIG_STREAM_OPEN_MASTER)

/**
   Запрос на открытие потока
   на входе    :  in_DstAddr  - адрес получателя
                  in_pszName  - указатель на имя потока
                  in_eMode    - режим открытия потока
                  in_u32PIN   - пин код
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendStreamOpenRequest(iridium_address_t in_DstAddr, const char* in_pszName, eIridiumStreamMode in_eMode, u32 in_u32PIN)
{
   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_STREAM_OPEN);
   // Начало работы с пакетом
   Begin();
   // Добавление имени потока
   m_pOutMessage->AddString(in_pszName);
   // Добавление режима открытия потока
   m_pOutMessage->AddU8(in_eMode);
   // Добавление PIN кода
   if(in_u32PIN)
      m_pOutMessage->AddU32LE(in_u32PIN);
   // Окончание работы и отправка пакета
   return End();
}

/**
   Обработка полученого ответа на запрос открытия потока
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveStreamOpenResponse()
{
   char* l_pszName = NULL;
   u8 l_u8Mode = 0;
   u8 l_u8StreamID = 0;
   m_eError = IRIDIUM_PROTOCOL_CORRUPT;
   // Получение имени потока
   if(m_pInMessage->GetString(l_pszName))
   {
      // Получение режима открытия потока
      if(m_pInMessage->GetU8(l_u8Mode))
      {
         // Получение идентификатора открытого потока
         if(m_pInMessage->GetU8(l_u8StreamID))
         {
            // Вызов обработчика успешного открытия потока
            StreamOpenResult(l_pszName, (eIridiumStreamMode)l_u8Mode, l_u8StreamID);
            m_eError = IRIDIUM_OK;
         }
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_STREAM_OPEN_MASTER)

#if defined(IRIDIUM_CONFIG_STREAM_OPEN_SLAVE)

/**
   Обработка полученого запроса открытия потока
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveStreamOpenRequest()
{
   char* l_pszName = NULL;
   u8 l_u8Mode = 0;
   u8 l_u8StreamID = 0;
   u32 l_u32PIN = 0;
   m_eError = IRIDIUM_PROTOCOL_CORRUPT;
   // Получение имени потока
   if(m_pInMessage->GetString(l_pszName))
   {
      // Получение режима открытия потока
      if(m_pInMessage->GetU8(l_u8Mode))
      {
         // Получение пина, если таковой есть
         if(m_pInMessage->Size() >= sizeof(u32))
            m_pInMessage->GetU32LE(l_u32PIN);
         // Проверка PIN кода
         s8 l_s8Result = CheckOperation((l_u8Mode == IRIDIUM_STREAM_MODE_READ) ? IRIDIUM_OPERATION_READ_STREAM : IRIDIUM_OPERATION_WRITE_STREAM, l_u32PIN, &l_pszName);
         if(l_s8Result > 0)
         {
            // Вызов обработчика открытия потока
            l_u8StreamID = StreamOpen(l_pszName, (eIridiumStreamMode)l_u8Mode);
            // Отправка ответа
            SendStreamOpenResponse(l_pszName, (eIridiumStreamMode)l_u8Mode, l_u8StreamID);
            m_eError = IRIDIUM_OK;
         } else
         {
            // Определение ошибки
            m_eError = (l_s8Result < 0) ? IRIDIUM_PIN_LOCK : IRIDIUM_BAD_PIN;
         }
      }
   }
}

/**
   Отправка ответа на запрос открытия потока
   на входе    :  in_pszName     - указатель на имя потока
                  in_eMode       - режим открытия потока
                  in_u8StreamID  - идентифкатор открытого потока
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendStreamOpenResponse(const char* in_pszName, eIridiumStreamMode in_eMode, u8 in_u8StreamID)
{
   // Инициализация пакета ответа
   InitResponsePacket();
   // Начало работы с пакетом
   Begin();
   // Добавление имени потока
   m_pOutMessage->AddString(in_pszName);
   // Добавление режима открытия потока
   m_pOutMessage->AddU8((u8)in_eMode);
   // Добавление идентификатора открытого потока
   m_pOutMessage->AddU8(in_u8StreamID);
   // Окончание работы и отправка пакета
   return End();
}

#endif   // #if defined(IRIDIUM_CONFIG_STREAM_OPEN_SLAVE)

#if defined(IRIDIUM_CONFIG_STREAM_BLOCK_MASTER)

/**
   Запрос на передачу блока
   на входе    :  in_DstAddr     - адрес получателя
                  in_u8StreamID  - идентификатор потока
                  in_u8BlockID   - идентификатор блока
                  in_u16Size     - размер блока
                  in_pBlock      - указатель на блок
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendStreamBlockRequest(iridium_address_t in_DstAddr, u8 in_u8StreamID, u8 in_u8BlockID, u16 in_u16Size, const void* in_pBlock)
{
   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_STREAM_BLOCK);
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора потока
   m_pOutMessage->AddU8(in_u8StreamID);
   // Добавление идентификатора блока
   m_pOutMessage->AddU8(in_u8BlockID);
   // Добавление длинны блока
   m_pOutMessage->AddU16LE(in_u16Size);
   // Добавление данных блока
   m_pOutMessage->AddData(in_pBlock, in_u16Size);
   // Окончание работы и отправка пакета
   return End();
}

/**
   Обработка полученого ответа на запрос передачи блока
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveStreamBlockResponse()
{
   u16 l_u16Size = 0;
   u8 l_u8StreamID = 0;
   u8 l_u8BlockID = 0;
   // Получение идентификатора потока
   if(m_pInMessage->GetU8(l_u8StreamID))
   {
      // Получение идентификатора блока
      if(m_pInMessage->GetU8(l_u8BlockID))
      {
         // Получение длинны блока
         if(m_pInMessage->GetU16LE(l_u16Size))
         {
            // Вызов обработчика получения блока
            StreamBlockResult(l_u8StreamID, l_u8BlockID, l_u16Size);
            m_eError = IRIDIUM_OK;
         }
      }
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_STREAM_BLOCK_MASTER)

#if defined(IRIDIUM_CONFIG_STREAM_BLOCK_SLAVE)

/**
   Обработка полученого запроса передачи блока
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveStreamBlockRequest()
{
   u16 l_u16Size = 0;
   u8 l_u8StreamID = 0;
   u8 l_u8BlockID = 0;
   // Получение идентификатора потока
   if(m_pInMessage->GetU8(l_u8StreamID))
   {
      // Получение идентификатора блока
      if(m_pInMessage->GetU8(l_u8BlockID))
      {
         // Получение длинны блока
         if(m_pInMessage->GetU16LE(l_u16Size))
         {
            // Вызов обработчика получения блока
            l_u16Size = (u16)StreamBlock(l_u8StreamID, l_u8BlockID, l_u16Size, m_pInMessage->GetDataPtr());
            SendStreamBlockResponse(l_u8StreamID, l_u8BlockID, l_u16Size);
            m_eError = IRIDIUM_OK;
         }
      }
   }
}

/**
   Ответ на запрос передачи блока
   на входе    :  in_u8StreamID  - идентификатор потока
                  in_u8BlockID   - идентификатор блока
                  in_u16Size     - размер блока
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendStreamBlockResponse(u8 in_u8StreamID, u8 in_u8BlockID, u16 in_u16Size)
{
   // Инициализация пакета ответа
   InitResponsePacket();
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора потока
   m_pOutMessage->AddU8(in_u8StreamID);
   // Добавление идентификатора блока
   m_pOutMessage->AddU8(in_u8BlockID);
   // Добавление длинны блока
   m_pOutMessage->AddU16LE(in_u16Size);
   // Окончание работы и отправка пакета
   return End();
}

#endif   // #if defined(IRIDIUM_CONFIG_STREAM_BLOCK_SLAVE)

#if defined(IRIDIUM_CONFIG_STREAM_CLOSE_MASTER)

/**
   Запрос на закрытие потока
   на входе    :  in_DstAddr     - адрес получателя
                  in_u8StreamID  - идентиифкатор потока
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendStreamCloseRequest(iridium_address_t in_DstAddr, u8 in_u8StreamID)
{
   // Инициализация адресного запроса
   InitRequestPacket(false, in_DstAddr, IRIDIUM_MESSAGE_STREAM_CLOSE);
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора потока
   m_pOutMessage->AddU8(in_u8StreamID);
   // Окончание работы и отправка пакета
   return End();
}

/**
   Обработка полученого ответа на запрос закрытия потока
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveStreamCloseResponse()
{
   u8 l_u8StreamID = 0;
   // Получение идентификатора открытого потока
   if(m_pInMessage->GetU8(l_u8StreamID))
   {
      // Вызов обработчика закрытия потока
      StreamClose(l_u8StreamID);
   }
}

#endif   // #if defined(IRIDIUM_CONFIG_STREAM_CLOSE_MASTER)

#if defined(IRIDIUM_CONFIG_STREAM_CLOSE_SLAVE)

/**
   Обработка полученого запроса закрытия потока
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ReceiveStreamCloseRequest()
{
   u8 l_u8StreamID = 0;
   // Получение идентификатора потока
   if(m_pInMessage->GetU8(l_u8StreamID))
   {
      // Отправка ответа на закрытие потока
      SendStreamCloseResponse(l_u8StreamID);
      // Вызов обработчика закрытия потока
      StreamClose(l_u8StreamID);
   }
}

/**
   Отправка ответа на запрос закрытия потока
   на входе    :  in_u8StreamID  - идентифкатор открытого потока
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendStreamCloseResponse(u8 in_u8StreamID)
{
   // Инициализация пакета ответа
   InitResponsePacket();
   // Начало работы с пакетом
   Begin();
   // Добавление идентификатора потока
   m_pOutMessage->AddU8(in_u8StreamID);
   // Окончание работы и отправка пакета
   return End();
}

#endif   // #if defined(IRIDIUM_CONFIG_STREAM_CLOSE_SLAVE)

/**
   Начало формирования пакета
   на входе    :  *
   на выходе   :  *
   примечание  :  данная процедура нужна для уменьшения размера исполняемого кода
*/
void CIridiumProtocol::Begin()
{
   size_t l_stSize = 0;

   // Получение размера блока
#if defined(IRIDIUM_ENABLE_CIPHER)
   if(m_pCipher)
      l_stSize = m_pCipher->GetBlockSize();
#endif

   m_pOutMessage->Clear();
   // Начало работы с пакетом
   m_pOutMessage->Begin(l_stSize);
   // Добавление заголовка сообщения
   m_pOutMessage->AddMessageHeader(m_OutMH);
}

/**
   Окончание формирования пакета
   на входе    :  *
   на выходе   :  успешность окончания работы с пакетов
*/
bool CIridiumProtocol::End()
{
   bool l_bResult = m_pOutMessage->Result();
   if(l_bResult)
   {
      // Установка признака конца цепочки
      m_pOutMessage->SetMessageHeaderEnd(m_OutMH.m_Flags.m_bEnd);

      // Кодирование сообщения
#if defined(IRIDIUM_ENABLE_CIPHER)

      size_t l_stMax = m_pOutMessage->GetMaxMessageSize();
      if(EncodeMessage(m_pOutMessage->GetMessagePtr(), m_pOutMessage->GetMessageSize(), l_stMax))
         m_pOutMessage->SetMessageSize(l_stMax);
#endif

      // Окончание работы с пакетом
      m_pOutMessage->End(m_OutPH);
      // Отправка пакета
      l_bResult = SendPacket(!m_OutPH.m_Flags.m_bAddress, m_OutPH.m_DstAddr, m_pOutMessage->GetPacketPtr(), m_pOutMessage->GetPacketSize());
   }
   return l_bResult;
}

/**
   Переотправка пакета
   на входе    :  in_pPH      - указатель на данные заголовка пакета
                  in_pPtr     - указатель на данные сообщения
                  in_stSize   - размер данных сообщения
   на выходе   :  успешность
*/
bool CIridiumProtocol::Resend(iridium_packet_header_t* in_pPH, const void* in_pPtr, size_t in_stSize)
{
   bool l_bResult = false;
   size_t l_stSize = 0;
   iridium_packet_header_t l_PH;

   // Проверка входных параметров
   if(in_pPH && in_pPtr && in_stSize)
   {
      // Формирование заголовка пакета
      l_PH                    = *in_pPH;
      l_PH.m_u8Type           = m_OutPH.m_u8Type;
      l_PH.m_Flags.m_u3Crypt  = m_OutPH.m_Flags.m_u3Crypt;

      // Получение размера блока
#if defined(IRIDIUM_ENABLE_CIPHER)
      if(m_pCipher)
         l_stSize = m_pCipher->GetBlockSize();
#endif

      // Начало работы с пакетом
      m_pOutMessage->Begin(l_stSize);
      // Добавление сообщения
      m_pOutMessage->AddData(in_pPtr, in_stSize);

      // Кодирование сообщения
#if defined(IRIDIUM_ENABLE_CIPHER)

      size_t l_stMax = m_pOutMessage->GetMaxMessageSize();
      if(EncodeMessage(m_pOutMessage->GetMessagePtr(), m_pOutMessage->GetMessageSize(), l_stMax))
         m_pOutMessage->SetMessageSize(l_stMax);

#endif
      // Конец работы с пакетом
      m_pOutMessage->End(l_PH);
      // Отправка сформированного пакета
      l_bResult = SendPacket(!m_OutPH.m_Flags.m_bAddress, m_OutPH.m_DstAddr, m_pOutMessage->GetPacketPtr(), m_pOutMessage->GetPacketSize());
   }
   return l_bResult;
}

/**
   Обработка сообщения
   на входе    :  in_pPH      - указатель на данные заголовка пакета
                  in_pBuffer  - указатель на данные пакета
                  in_stSize   - размер данных пакета
   на выходе   :  успешность обработки сообщения
*/
bool CIridiumProtocol::ProcessMessage(iridium_packet_header_t* in_pPH, const void* in_pBuffer, size_t in_stSize)
{
   bool l_bResult = false;
   if(in_pPH && in_pBuffer && in_stSize)
   {
      // Запомним указатель на данные заголовка пакета
      m_pInPH = in_pPH;
      m_eError = IRIDIUM_PROTOCOL_CORRUPT;
      // Инициализация сообщения
      m_pInMessage->SetBuffer(in_pBuffer, in_stSize);
      // Получение типа сообщения
      if(m_pInMessage->GetMessageHeader(m_InMH))
      {
         // Проверка номера версии сообщения
         if(m_InMH.m_Flags.m_u4Version <= GetMessageVersion(m_InMH.m_u8Type))
         {
            m_eError = IRIDIUM_OK;
            if(m_InMH.m_Flags.m_bDirection == IRIDIUM_REQUEST)
            {
               // Обработка запроса
               ProcessMessageRequest();

               // Если во время обрабоки возникла ошибка сообщим о ней источнику запроса
               if(m_eError)
                  SendResponse(m_eError);

            } else
            {
               // Обработка ответа на запрос
               if(m_InMH.m_Flags.m_bError)
               {
                  // Получение кода ошибки
                  u8 l_u8Error = 0;
                  m_pInMessage->GetU8(l_u8Error);
                  // Сообщим о полученой ошибке
                  ProcessingResult(IRIDIUM_ERROR_RECEIVED, (eIridiumError)l_u8Error, m_pInPH, &m_InMH);
                  // Сбросим ошибку
                  m_eError = IRIDIUM_OK;

               } else
               {
                  // Обработка ответа на запрос
                  ProcessMessageResponse();
               }
            }
            l_bResult = true;

         } else
            m_eError = IRIDIUM_UNKNOWN_MESSAGE_VERSION;

         // Проверка наличия ошибки
         if(m_eError)
         {
            // Сообщим об ошибке возникшей в процессе обработки пакета
            ProcessingResult(IRIDIUM_ERROR_PROCESSING, m_eError, m_pInPH, &m_InMH);
         }
      }
   }
   return l_bResult;
}

/**
   Обработка запросов. Вызывается из ProcessMessage
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ProcessMessageRequest()
{
   // Обработка данных пакета
   switch(m_InMH.m_u8Type)
   {
#if defined(IRIDIUM_CONFIG_SYSTEM_PING_SLAVE)
   // Пинг
   case IRIDIUM_MESSAGE_SYSTEM_PING:
      ReceivePingRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_SLAVE)
   // Поиск
   case IRIDIUM_MESSAGE_SYSTEM_SEARCH:
      ReceiveSearchRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_SLAVE)
   // Информация об устройстве
   case IRIDIUM_MESSAGE_SYSTEM_DEVICE_INFO:
      ReceiveDeviceInfoRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_SLAVE)
   // Установка локального идентификатора
   case IRIDIUM_MESSAGE_SYSTEM_SET_LID:
      ReceiveSetLIDRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SESSION_TOKEN_SLAVE)
      // Установка локального идентификатора
   case IRIDIUM_MESSAGE_SYSTEM_SESSION_TOKEN:
      ReceiveSessionTokenRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_SLAVE)
   // Получение информации о Smart API
   case IRIDIUM_MESSAGE_SYSTEM_SMART_API:
      ReceiveSmartAPIRequest();
      break;
#endif

#if defined(IRIDIUM_CONFIG_SET_VARIABLE_SLAVE)
   // Установка глобальной переменной
   case IRIDIUM_MESSAGE_SET_VARIABLE:
      ReceiveSetVariableRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_VARIABLE_SLAVE)
   // Получение глобальной переменной
   case IRIDIUM_MESSAGE_GET_VARIABLE:
      ReceiveGetVariableRequest();
      break;
#endif

#if defined(IRIDIUM_CONFIG_GET_TAGS_SLAVE)
   // Получение списка каналов обратной связи
   case IRIDIUM_MESSAGE_GET_TAGS:
      ReceiveGetTagsRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_LINK_TAG_AND_VARIABLE_SLAVE)
   // Связывание канала обратной связи и глобальной переменной
   case IRIDIUM_MESSAGE_LINK_TAG_AND_VARIABLE:
      ReceiveLinkTagAndVariableRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_TAG_DESCRIPTION_SLAVE)
   // Получение описания канала обратной связи
   case IRIDIUM_MESSAGE_GET_TAG_DESCRIPTION:
      ReceiveGetTagDescriptionRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SET_TAG_VALUE_SLAVE)
   // Установка значения канала обратной связи
   case IRIDIUM_MESSAGE_SET_TAG_VALUE:
      ReceiveSetTagValueRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_TAG_VALUE_SLAVE)
   // Получение значения канала обратной связи
   case IRIDIUM_MESSAGE_GET_TAG_VALUE:
      ReceiveGetTagValueRequest();
      break;
#endif

#if defined(IRIDIUM_CONFIG_GET_CHANNELS_SLAVE)
   // Получение списка каналов управления
   case IRIDIUM_MESSAGE_GET_CHANNELS:
      ReceiveGetChannelsRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SET_CHANNEL_VALUE_SLAVE)
   // Установка значения канала управления
   case IRIDIUM_MESSAGE_SET_CHANNEL_VALUE:
      ReceiveSetChannelValueRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_LINK_CHANNEL_AND_VARIABLE_SLAVE)
   // Связывание канала управления и глобальной переменной
   case IRIDIUM_MESSAGE_LINK_CHANNEL_AND_VARIABLE:
      ReceiveLinkChannelAndVariableRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_CHANNEL_DESCRIPTION_SLAVE)
   // Получение описания канала управления
   case IRIDIUM_MESSAGE_GET_CHANNEL_DESCRIPTION:
      ReceiveGetChannelDescriptionRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_CHANNEL_VALUE_SLAVE)
   // Получение значения канала управления
   case IRIDIUM_MESSAGE_GET_CHANNEL_VALUE:
      ReceiveGetChannelValueRequest();
      break;
#endif

#if defined(IRIDIUM_CONFIG_STREAM_OPEN_SLAVE)
   // Открытия потока
   case IRIDIUM_MESSAGE_STREAM_OPEN:
      ReceiveStreamOpenRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_STREAM_BLOCK_SLAVE)
   // Передача блока потока
   case IRIDIUM_MESSAGE_STREAM_BLOCK:
      ReceiveStreamBlockRequest();
      break;
#endif
#if defined(IRIDIUM_CONFIG_STREAM_CLOSE_SLAVE)
   // Закрытия потока
   case IRIDIUM_MESSAGE_STREAM_CLOSE:
      ReceiveStreamCloseRequest();
      break;
#endif

   // Неизвестное тип сообщения
   default:
      m_eError = IRIDIUM_UNKNOWN_MESSAGE;
      break;
   }
}

/**
   Обработка ответов. Вызывается из ProcessMessage
   на входе    :  *
   на выходе   :  *
*/
void CIridiumProtocol::ProcessMessageResponse()
{
   // Обработка данных пакета
   switch(m_InMH.m_u8Type)
   {
#if defined(IRIDIUM_CONFIG_SYSTEM_PING_MASTER)
   // Пинг
   case IRIDIUM_MESSAGE_SYSTEM_PING:
      ReceivePingResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_MASTER)
   // Поиск
   case IRIDIUM_MESSAGE_SYSTEM_SEARCH:
      ReceiveSearchResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_DEVICE_INFO_MASTER)
   // Информация об устройстве
   case IRIDIUM_MESSAGE_SYSTEM_DEVICE_INFO:
      ReceiveDeviceInfoResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_MASTER)
   // Установка локального идентификатора
   case IRIDIUM_MESSAGE_SYSTEM_SET_LID:
      ReceiveSetLIDResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SESSION_TOKEN_MASTER)
      // Установка локального идентификатора
   case IRIDIUM_MESSAGE_SYSTEM_SESSION_TOKEN:
      ReceiveSessionTokenResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SYSTEM_SMART_API_MASTER)
   // Получение информации о Smart API
   case IRIDIUM_MESSAGE_SYSTEM_SMART_API:
      ReceiveSmartAPIResponse();
      break;
#endif

#if defined(IRIDIUM_CONFIG_SET_VARIABLE_MASTER)
   // Установка глобальной переменной
   case IRIDIUM_MESSAGE_SET_VARIABLE:
      ReceiveSetVariableResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_VARIABLE_MASTER)
   // Получение глобальной переменной
   case IRIDIUM_MESSAGE_GET_VARIABLE:
      ReceiveGetVariableResponse();
      break;
#endif

#if defined(IRIDIUM_CONFIG_GET_TAGS_MASTER)
   // Получение списка каналов обратной связи
   case IRIDIUM_MESSAGE_GET_TAGS:
      ReceiveGetTagsResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_LINK_TAG_AND_VARIABLE_MASTER)
   // Связывание канала обратной связи и глобальной переменной
   case IRIDIUM_MESSAGE_LINK_TAG_AND_VARIABLE:
      ReceiveLinkTagAndVariableResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_TAG_DESCRIPTION_MASTER)
   // Получение описания канала обратной связи
   case IRIDIUM_MESSAGE_GET_TAG_DESCRIPTION:
      ReceiveGetTagDescriptionResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SET_TAG_VALUE_MASTER)
   // Установка значения канала обратной связи
   case IRIDIUM_MESSAGE_SET_TAG_VALUE:
      ReceiveSetTagValueResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_TAG_VALUE_MASTER)
   // Получение значения канала обратной связи
   case IRIDIUM_MESSAGE_GET_TAG_VALUE:
      ReceiveGetTagValueResponse();
      break;
#endif

#if defined(IRIDIUM_CONFIG_GET_CHANNELS_MASTER)
   // Получение списка каналов управления
   case IRIDIUM_MESSAGE_GET_CHANNELS:
      ReceiveGetChannelsResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_SET_CHANNEL_VALUE_MASTER)
   // Установка значения канала управления
   case IRIDIUM_MESSAGE_SET_CHANNEL_VALUE:
      ReceiveSetChannelValueResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_LINK_CHANNEL_AND_VARIABLE_MASTER)
   // Связывание канала управления и глобальной переменной
   case IRIDIUM_MESSAGE_LINK_CHANNEL_AND_VARIABLE:
      ReceiveLinkChannelAndVariableResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_CHANNEL_DESCRIPTION_MASTER)
   // Получение описания канала управления
   case IRIDIUM_MESSAGE_GET_CHANNEL_DESCRIPTION:
      ReceiveGetChannelDescriptionResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_GET_CHANNEL_VALUE_MASTER)
   // Получение значения канала управления
   case IRIDIUM_MESSAGE_GET_CHANNEL_VALUE:
      ReceiveGetChannelValueResponse();
      break;
#endif

#if defined(IRIDIUM_CONFIG_STREAM_OPEN_MASTER)
   // Открытия потока
   case IRIDIUM_MESSAGE_STREAM_OPEN:
      ReceiveStreamOpenResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_STREAM_BLOCK_MASTER)
   // Передача блока потока
   case IRIDIUM_MESSAGE_STREAM_BLOCK:
      ReceiveStreamBlockResponse();
      break;
#endif
#if defined(IRIDIUM_CONFIG_STREAM_CLOSE_MASTER)
   // Закрытия потока
   case IRIDIUM_MESSAGE_STREAM_CLOSE:
      ReceiveStreamCloseResponse();
      break;
#endif

   // Неизвестное тип сообщения
   default:
      m_eError = IRIDIUM_UNKNOWN_MESSAGE;
      break;
   }
}

/**
   Получение идентиифкатора транзакции
   на входе    :  *
   на выходе   :  идентификатор транзакции
*/
u16 CIridiumProtocol::GetTID()
{
   // Увеличение идентиифкатора с отсечением нулевого значения
   m_u16TID++;
   if(!m_u16TID)
      m_u16TID++;
   return m_u16TID;
}

/**
   Отправка запроса
   на входе    :  in_bBroadcast  - признак широковещательного запроса
                  in_DstAddr     - адрес получателя
                  in_eType       - тип сообщения
   нв выходе   :  успешность
*/
bool CIridiumProtocol::SendRequest(bool in_bBroadcast, iridium_address_t in_DstAddr, u8 in_u8Type)
{
   // Инициализация запроса
   InitRequestPacket(in_bBroadcast, in_DstAddr, in_u8Type);
   // Начало работы с пакетом
   Begin();
   // Окончание работы и отправка пакета
   return End();
}

/**
   Отправка ответа
   на входе    :  in_eCode - код ошибки
   на выходе   :  успешность
*/
bool CIridiumProtocol::SendResponse(eIridiumError in_eCode)
{
   // Инициализаци пакета ответа
   InitResponsePacket();

   // Установка флага наличия ошибки
   m_OutMH.m_Flags.m_bError = in_eCode ? IRIDIUM_ERROR : IRIDIUM_NO_ERROR;

   // Начало работы с пакетом
   Begin();
   // Если код ошибки определен, добавим кода ошибки
   if(in_eCode)
      m_pOutMessage->AddU8(in_eCode);
   // Окончание работы и отправка пакета
   return End();
}
