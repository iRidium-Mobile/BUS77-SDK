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
#include "CIridiumBusProtocol.h"
#include "IridiumBytes.h"
#include "IridiumCRC16.h"
#include <stdlib.h>

/**
   Конструктор класса
   на входе    :  *
*/
CIridiumBusProtocol::CIridiumBusProtocol() : CIridiumProtocol()
{
   // Подготовка входящего буфера
   m_pInMessage = &m_MessageBuffer;

   // Подготовка исходящего буфера
   m_pOutMessage = &m_OutBuffer;

   // Сброс данных
   Reset();

   // Инициализация параметров протокола
   m_OutPH.m_u8Type              = IRIDIUM_BUS_PROTOCOL_ID;
   m_OutPH.m_Flags.m_bPriority   = false;
   m_OutPH.m_Flags.m_bSrcSeg     = false;
   m_OutPH.m_Flags.m_bDstSeg     = false;
   m_OutPH.m_Flags.m_bAddress    = true;
   m_OutPH.m_Flags.m_u2Version   = IRIDIUM_PROTOCOL_BUS_VERSION;
   m_OutPH.m_Flags.m_u3Crypt     = IRIDIUM_CRYPTION_NONE;
   m_OutPH.m_SrcAddr             = 0;
   m_OutPH.m_DstAddr             = 0;
}

/**
   Деструктор класса
*/
CIridiumBusProtocol::~CIridiumBusProtocol()
{
}

/**
   Сброс соединения
   на входе    :  *
   на выходе   :  *
*/
void CIridiumBusProtocol::Reset()
{
   // Сброс родительского класса
   CIridiumProtocol::Reset();

   // Очистка потоков
   m_InBuffer.Clear();
   m_MessageBuffer.Reset();
   m_OutBuffer.Clear();

#if defined(IRIDIUM_ENABLE_CIPHER)
   m_pCipher = NULL;
   m_u8Count = 0;
#endif
}

#if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_MASTER)

/**
   Отправка запроса поиска всех устройств в сети
   на входе    :  in_DstAddr  - адрес сегмента
                  in_u8Mask   - маска поиска
   на выходе   :  успешность
   примечание  :  сообщение является широковещательным (без получателя) и отправляется в открытом виде
*/
bool CIridiumBusProtocol::SendSearchRequest(iridium_address_t in_DstAddr, u8 in_u8Mask)
{
   // Инициализация широковещательного запроса
   InitRequestPacket(true, in_DstAddr, IRIDIUM_MESSAGE_SYSTEM_SEARCH);
   // Начало работы с пакетом
   Begin();
   // Добавление маски поиска
   m_pOutMessage->AddU8(in_u8Mask);
   // Окончание работы и отправка пакета
   return End();
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SEARCH_MASTER)

#if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_MASTER)

/**
   Отправка запроса на получение информации об удаленном сервере
   на входе    :  in_DstAddr  - адрес сегмента
                  in_pszHWID  - указатель на HWID для которого нужно установить локальный идентификатор
                  in_u8LID    - локальный идентификатор который надо установить
                  in_u32PIN   - PIN код
   на выходе   :  успешность
   примечание  :  сообщение является широковещательным (без получателя) и отправляется в открытом виде
*/
bool CIridiumBusProtocol::SendSetLIDRequest(iridium_address_t in_DstAddr, const char* in_pszHWID, u8 in_u8LID, u32 in_u32PIN)
{
   // Инициализация широковещательного запроса
   InitRequestPacket(true, in_DstAddr, IRIDIUM_MESSAGE_SYSTEM_SET_LID);
   //m_OutMH.m_Flags.m_bNoTID = true;

   // Начало работы с пакетом
   Begin();
   // Добавление HWID и LID
   m_pOutMessage->AddString(in_pszHWID);
   m_pOutMessage->AddU8(in_u8LID);
   // Добавление PIN кода
   if(in_u32PIN)
      m_pOutMessage->AddU32LE(in_u32PIN);
   // Окончание работы и отправка пакета
   return End();
}

#endif   // #if defined(IRIDIUM_CONFIG_SYSTEM_SET_LID_MASTER)

/**
   Инициализация данных пакета
   на входе    :  in_bBroadcast  - признак широковещательного пакета
                  in_DstAddr     - адрес получателя
                  in_u8Type      - тип сообщения
   на выходе   :  *
   примечание  :  данная процедура нужна для уменьшения размера исполняемого кода
*/
void CIridiumBusProtocol::InitRequestPacket(bool in_bBrodcast, iridium_address_t in_DstAddr, u8 in_u8Type)
{
   // Заполнение заголовка пакета
   m_OutPH.m_Flags.m_bSrcSeg     = false;
   m_OutPH.m_Flags.m_bDstSeg     = (0 != (in_DstAddr & 0xFF00));
   m_OutPH.m_Flags.m_bAddress    = (false == in_bBrodcast);
   m_OutPH.m_u8Route             = 0;
   m_OutPH.m_SrcAddr             = m_Address;
   m_OutPH.m_DstAddr             = in_DstAddr;

   // Заполнение заголовка сообщения
   m_OutMH.m_Flags.m_bDirection  = IRIDIUM_REQUEST;
   m_OutMH.m_Flags.m_bError      = IRIDIUM_NO_ERROR;
   m_OutMH.m_Flags.m_bEnd        = true;
   m_OutMH.m_Flags.m_u4Version   = GetMessageVersion(in_u8Type);
   m_OutMH.m_u8Type              = in_u8Type;
   m_OutMH.m_u16TID              = GetTID();
}

/**
   Инициализация данных пакета
   на входе    :  *
   на выходе   :  *
   примечание  :  данная процедура нужна для уменьшения размера исполняемого кода
*/
void CIridiumBusProtocol::InitResponsePacket()
{
   // Заполнение заголовка пакета
   m_OutPH.m_Flags.m_bSrcSeg     = false;
   m_OutPH.m_Flags.m_bDstSeg     = m_pInPH->m_Flags.m_bSrcSeg;
   m_OutPH.m_Flags.m_bAddress    = true;
   m_OutPH.m_u8Route             = m_pInPH->m_u8Route;
   m_OutPH.m_SrcAddr             = m_Address;
   m_OutPH.m_DstAddr             = m_pInPH->m_SrcAddr;

   // Заполнение заголовка сообщения
   m_OutMH.m_Flags.m_bDirection  = IRIDIUM_RESPONSE;
   m_OutMH.m_Flags.m_bError      = IRIDIUM_NO_ERROR;
   m_OutMH.m_Flags.m_bEnd        = true;
   m_OutMH.m_Flags.m_u4Version   = m_InMH.m_Flags.m_u4Version;
   m_OutMH.m_u8Type              = m_InMH.m_u8Type;
   m_OutMH.m_u16TID              = m_InMH.m_u16TID;
}

#if defined(IRIDIUM_ENABLE_CIPHER)

/**
   Инициализация шифрования
   на входе    :  in_u8Crypt  - тип шифрования
                  in_pData    - указатель на данные шифрования
   на выходе   :  *
*/
void CIridiumBusProtocol::InitCrypt(u8 in_u8Crypt, u8* in_pData)
{
   // Установка типа шифрования
   switch(in_u8Crypt)
   {
#if defined(IRIDIUM_ENABLE_GRASSHOPPER_CIPHER)
   case IRIDIUM_CRYPTION_GRASSHOPPER:
      m_pCipher = &m_Grasshopper;
   
#if defined(IRIDIUM_ENABLE_IV)
      m_pCipher->EnableIV(false);
#endif
      m_pCipher->Init(in_pData);
      break;
#endif   // defined(IRIDIUM_ENABLE_GRASSHOPPER_CIPHER)

   default:
      m_pCipher = NULL;
      in_u8Crypt = IRIDIUM_CRYPTION_NONE;
      break;
   }
   // Пропишем тип шифрования в заголовок пакета
   m_OutPH.m_Flags.m_u3Crypt = in_u8Crypt;
}

/**
   Кодирование сообщения
   на входе    :  in_pBuffer  - указатель на буфер
                  in_stSize   - размер буфера
                  out_rMax    - ссылка на переменную с максимальным размером буфера для кодирования,
                                по окончанию кодирования переменная содержит размер закодированых данных
   на выходе   :  успешность кодирования
*/
bool CIridiumBusProtocol::EncodeMessage(u8* in_pBuffer, size_t in_stSize, size_t& out_rMax)
{
   bool l_bResult = true;

   // Проверка наличия шифра
   if(m_pCipher)
   {
      // Добавление заголовка в тело 
      if(m_pCipher->GetBlockSize())
      {
         u16 l_u16CRC = 0;
         // Запись размера зашифрованного сообщения 
         WriteByte(in_pBuffer + IRIDIUM_BUS_CIPHER_SIZE_OFFSET, (u8)in_stSize - IRIDIUM_BUS_CIPHER_HEADER_SIZE);
         // Добавление случайного числа
         WriteByte(in_pBuffer + IRIDIUM_BUS_CIPHER_RAND_OFFSET, m_u8Count++);
         // Вычисление и добавление CRC16 для зашифрованного сообщения
         l_u16CRC = GetCRC16Modbus(0xFFFF, in_pBuffer + IRIDIUM_BUS_CIPHER_SIZE_OFFSET, (u8)in_stSize - IRIDIUM_BUS_CIPHER_CRC_SIZE);
         WriteLE(in_pBuffer + IRIDIUM_BUS_CIPHER_CRC_OFFSET, &l_u16CRC, 2);
      }

      // Шифрование тела сообщения
      l_bResult = m_pCipher->Encode(in_pBuffer, in_stSize, out_rMax);
   } else
      out_rMax = in_stSize;

   return l_bResult;
}

/**
   Декодирование сообщения
   на входе    :  in_u8Crypt  - тип кодирования
                  out_rBuffer - ссылка на переменную содержащую указатель на буфер с закодироваными данными,
                                после выполнения декодирования переменная содержит укатаель на буфер с раскодироваными данными
                  out_rSize   - ссылка на переменную содержащую размер буфера с закодироваными данными,
                                после выполнения декодирования переменная содержит размер раскодированных данных
   на выходе   :  успешность декодирования
*/
bool CIridiumBusProtocol::DecodeMessage(u8 in_u8Crypt, u8*& out_rBuffer, size_t& out_rSize)
{
   bool l_bResult = false;

   // Проверка наличия шифрования
   if(m_pCipher)
   {
      // Проверим соответствие типов и правильности декодирования
      if(in_u8Crypt == m_pCipher->GetType() && m_pCipher->Decode(out_rBuffer, out_rSize))
      {
         // Если блочный шифр, проверим правильность декодирования
         if(m_pCipher->GetBlockSize())
         {
            u16 l_u16CRC = 0;
            u8 l_u8Size = 0;

            // Чтение CRC16 и размера данных
            u8* l_pPtr = out_rBuffer;
            ReadLE(l_pPtr + IRIDIUM_BUS_CIPHER_CRC_OFFSET, &l_u16CRC, 2);
            ReadByte(l_pPtr + IRIDIUM_BUS_CIPHER_SIZE_OFFSET, l_u8Size);

            // Проверка правильности декодирования
            if(l_u16CRC == GetCRC16Modbus(0xFFFF, l_pPtr + IRIDIUM_BUS_CIPHER_SIZE_OFFSET, l_u8Size + (IRIDIUM_BUS_CIPHER_HEADER_SIZE - IRIDIUM_BUS_CIPHER_CRC_SIZE)))
            {
               // Заполним указатель и размер на расшифрованных данных
               out_rBuffer = l_pPtr + IRIDIUM_BUS_CIPHER_HEADER_SIZE;
               out_rSize = l_u8Size;
               l_bResult = true;
            }
         } else
            l_bResult = true;
      }
   } else
   {
      // Если нет шифрования
      if(!in_u8Crypt)
         l_bResult = true;
   }
   return l_bResult;
}

#endif   // defined(IRIDIUM_ENABLE_CIPHER)
