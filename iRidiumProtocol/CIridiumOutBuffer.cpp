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
#include "CIridiumOutBuffer.h"

/**
   Конструктор класса
   на входе    :  *
*/
CIridiumOutBuffer::CIridiumOutBuffer() : COutBuffer()
{
   m_pPacket = NULL;
   m_pMessage = NULL;
   m_stMaxMessageSize = 0;
}

/**
   Деструктор класса
*/
CIridiumOutBuffer::~CIridiumOutBuffer()
{
}

/**
   Установка внешнего буфера
   на входе    :  in_stHeaderSize   - максимальный размер заголовка
                  in_stCRCSize      - размер CRC в байтах
                  in_pBuffer        - указатель на буфер
                  in_stSize         - полный размер буфера
   на выходе   :  *
   примечание  :  максимальный размер сообщения шинного пакета 255 байт, по этому не рекомендуется делать размер буфера больше чем размер заголовка (5 байт) + размер пакета (255 байт)
                  максимальный размер сообщения сетевого пакета 65535 байт, по этому не рекомендуется делать размер буфера больше чем размер заголовка (12 байт) + размер пакета (65535 байт)
*/
void CIridiumOutBuffer::SetBuffer(size_t in_stHeaderSize, size_t in_stCRCSize, void* in_pBuffer, size_t in_stSize)
{
   // Вызов родительского класса
   COutBuffer::SetBuffer(in_pBuffer, in_stSize - in_stCRCSize);

   // Инициализация параметров
   m_pPacket = NULL;
   m_pMessage = m_pBuffer + in_stHeaderSize;
   m_pPtr = m_pMessage;
   // Вычисление максимального размера сообщения
   m_stMaxMessageSize = m_pEnd - m_pMessage;
}

/**
   Добавление заголовка контейнера в поток
   на входе    :  in_rMH   - ссылка на данные заголовка контейнера
   на выходе   :  успешность добавления данных
*/
void CIridiumOutBuffer::AddMessageHeader(iridium_message_header_t& in_rMH)
{
   // Запись направления, признака ошибки и версии контейнера
   AddU8(in_rMH.m_Flags.m_bDirection << 7 | in_rMH.m_Flags.m_bError << 6 | in_rMH.m_Flags.m_bNoTID << 5 | in_rMH.m_Flags.m_bEnd << 4 | in_rMH.m_Flags.m_u4Version);
   // Запись типа
   AddU8(in_rMH.m_u8Type);
   if(!in_rMH.m_Flags.m_bNoTID)
   {
      // Запись идентификатора транзакции
      AddU16LE(in_rMH.m_u16TID);
   }
}

/**
   Добавление информации об устройстве в буфер
   на входе    :  in_rInfo - ссылка на данные устройства
   на выходе   :  успешность добавления данных
*/
void CIridiumOutBuffer::AddSearchInfo(iridium_search_info_t& in_rInfo)
{
   // Добавление группы клиента
   AddU8(in_rInfo.m_u8Group);

#if defined(IRIDIUM_MCU_AVR)
   // Добавление HWID
   if(in_rInfo.Mem.m_bHWID)
      AddStringFromFlash(in_rInfo.m_pszHWID);
   else
      AddString(in_rInfo.m_pszHWID);
#else
   // Добавление HWID
   AddString(in_rInfo.m_pszHWID);
#endif
   // Добавление события
   AddU8(in_rInfo.m_u8Event);
   // Добавление списка флагов
   AddU8(in_rInfo.m_u8Flags);
   // Добавление пользовательского идентификатора
   AddU32LE(in_rInfo.m_u32UserID);
   // Добавление версии изменения
   AddU16LE(in_rInfo.m_u16Change);
}

/**
   Добавление информации об устройстве в буфер
   на входе    :  in_rInfo - ссылка на данные устройства
   на выходе   :  успешность добавления данных
*/
void CIridiumOutBuffer::AddDeviceInfo(iridium_device_info_t& in_rInfo)
{
   // Добавление группы клиента
   AddU8(in_rInfo.m_u8Group);

#if defined(IRIDIUM_MCU_AVR)
   // Добавление имени устройства
   if(in_rInfo.Mem.m_bName)
      AddStringFromFlash(in_rInfo.m_pszName);
   else
      AddString(in_rInfo.m_pszName);
   // Добавление операционной системы
   if(in_rInfo.Mem.m_bOS)
      AddStringFromFlash(in_rInfo.m_pszProducer);
   else
      AddString(in_rInfo.m_pszProducer);
   // Добавление модели устройства
   if(in_rInfo.Mem.m_bModel)
      AddStringFromFlash(in_rInfo.m_pszModel);
   else
      AddString(in_rInfo.m_pszModel);
   // Добавление HWID
   if(in_rInfo.Mem.m_bHWID)
      AddStringFromFlash(in_rInfo.m_pszHWID);
   else
      AddString(in_rInfo.m_pszHWID);
#else
   // Добавление имени устройства
   AddString(in_rInfo.m_pszName);
   // Добавление операционной системы
   AddString(in_rInfo.m_pszProducer);
   // Добавление модели устройства
   AddString(in_rInfo.m_pszModel);
   // Добавление HWID
   AddString(in_rInfo.m_pszHWID);
#endif

   // Тип операционной системы и версия
   AddU8(in_rInfo.m_u8Class);
   AddU8(in_rInfo.m_u8Processor);
   AddU8(in_rInfo.m_u8OS);
   AddU8(in_rInfo.m_u8Flags);
   AddU16LE(in_rInfo.m_u16FirmwareID);
   AddData(in_rInfo.m_aVersion, sizeof(in_rInfo.m_aVersion));
   // Добавление количества каналов управления и обратной связи
   AddU32LE(in_rInfo.m_u32Channels);
   AddU32LE(in_rInfo.m_u32Tags);
   AddU32LE(in_rInfo.m_u32UserID);
   AddU16LE(in_rInfo.m_u16Change);
}

/**
   Добавление общего описания канала
   на входе    :  in_rDesc - ссылка на общее описание канала
   на выходе   :  успешность
*/
void CIridiumOutBuffer::AddDescription(iridium_description_t& in_rDesc)
{
   u8 l_u8Type = 0;
   u8 l_u8Flag = 0;

   // Получение типа значения
   l_u8Type = in_rDesc.m_u8Type;
   u8* l_pAnchor = CreateAnchor(1);
   if(l_pAnchor)
   {
      switch(l_u8Type)
      {
      // Неопределенный тип
      case IVT_NONE:
         break;
      // Запись бинарных параметров
      case IVT_BOOL:
         l_u8Flag |= (in_rDesc.m_Min.m_bValue << 7);
         l_u8Flag |= (in_rDesc.m_Max.m_bValue << 6);
         l_u8Flag |= (in_rDesc.m_Step.m_bValue << 5);
         break;

      // Запись S8 параметров
      case IVT_S8:
         if(in_rDesc.m_Min.m_s8Value)
         {
            l_u8Flag |= (1 << 7);
            AddS8(in_rDesc.m_Min.m_s8Value);
         }
         if(in_rDesc.m_Max.m_s8Value)
         {
            l_u8Flag |= (1 << 6);
            AddS8(in_rDesc.m_Max.m_s8Value);
         }
         if(in_rDesc.m_Step.m_s8Value)
         {
            l_u8Flag |= (1 << 5);
            AddS8(in_rDesc.m_Step.m_s8Value);
         }
         break;

      // Запись U8 параметров
      case IVT_U8:
         if(in_rDesc.m_Min.m_u8Value)
         {
            l_u8Flag |= (1 << 7);
            AddU8(in_rDesc.m_Min.m_u8Value);
         }
         if(in_rDesc.m_Max.m_u8Value)
         {
            l_u8Flag |= (1 << 6);
            AddU8(in_rDesc.m_Max.m_u8Value);
         }
         if(in_rDesc.m_Step.m_u8Value)
         {
            l_u8Flag |= (1 << 5);
            AddU8(in_rDesc.m_Step.m_u8Value);
         }
         break;

      // Запись S16 параметров
      case IVT_S16:
         if(in_rDesc.m_Min.m_s16Value)
         {
            l_u8Flag |= (1 << 7);
            AddS16LE(in_rDesc.m_Min.m_s16Value);
         }
         if(in_rDesc.m_Max.m_s16Value)
         {
            l_u8Flag |= (1 << 6);
            AddS16LE(in_rDesc.m_Max.m_s16Value);
         }
         if(in_rDesc.m_Step.m_s16Value)
         {
            l_u8Flag |= (1 << 5);
            AddS16LE(in_rDesc.m_Step.m_s16Value);
         }
         break;

      // Запись U16 параметров
      case IVT_U16:
         if(in_rDesc.m_Min.m_u16Value)
         {
            l_u8Flag |= (1 << 7);
            AddU16LE(in_rDesc.m_Min.m_u16Value);
         }
         if(in_rDesc.m_Max.m_u16Value)
         {
            l_u8Flag |= (1 << 6);
            AddU16LE(in_rDesc.m_Max.m_u16Value);
         }
         if(in_rDesc.m_Step.m_u16Value)
         {
            l_u8Flag |= (1 << 5);
            AddU16LE(in_rDesc.m_Step.m_u16Value);
         }
         break;

      // Запись S32 параметров
      case IVT_S32:
         if(in_rDesc.m_Min.m_s32Value)
         {
            l_u8Flag |= (1 << 7);
            AddS32LE(in_rDesc.m_Min.m_s32Value);
         }
         if(in_rDesc.m_Max.m_s32Value)
         {
            l_u8Flag |= (1 << 6);
            AddS32LE(in_rDesc.m_Max.m_s32Value);
         }
         if(in_rDesc.m_Step.m_s32Value)
         {
            l_u8Flag |= (1 << 5);
            AddS32LE(in_rDesc.m_Step.m_s32Value);
         }
         break;

      // Запись U32 параметров
      case IVT_U32:
      // Запись параметров строки (максимальная длинна строки 32 бита)
      // Минимум это минимальная длинна строки
      // Максимум это максимальная длинна строки
      // Шаг это шаг строки
      case IVT_STRING8:
      // Запись параметров массива (максимальная длинна массива 32 бита)
      // Минимум это минимальная длинна массива
      // Максимум это максимальная длинна массива
      // Шаг это шаг массива
      case IVT_ARRAY_U8:
         if(in_rDesc.m_Min.m_u32Value)
         {
            l_u8Flag |= (1 << 7);
            AddU32LE(in_rDesc.m_Min.m_u32Value);
         }
         if(in_rDesc.m_Max.m_u32Value)
         {
            l_u8Flag |= (1 << 6);
            AddU32LE(in_rDesc.m_Max.m_u32Value);
         }
         if(in_rDesc.m_Step.m_u32Value)
         {
            l_u8Flag |= (1 << 5);
            AddU32LE(in_rDesc.m_Step.m_u32Value);
         }
         break;

      // Запись F32 параметров
      case IVT_F32:
         if(in_rDesc.m_Min.m_f32Value != 0.0f)
         {
            l_u8Flag |= (1 << 7);
            AddF32LE(in_rDesc.m_Min.m_f32Value);
         }
         if(in_rDesc.m_Max.m_f32Value != 0.0f)
         {
            l_u8Flag |= (1 << 6);
            AddF32LE(in_rDesc.m_Max.m_f32Value);
         }
         if(in_rDesc.m_Step.m_f32Value != 0.0f)
         {
            l_u8Flag |= (1 << 5);
            AddF32LE(in_rDesc.m_Step.m_f32Value);
         }
         break;

      // Запись S64 параметров
      case IVT_S64:
         if(in_rDesc.m_Min.m_s64Value)
         {
            l_u8Flag |= (1 << 7);
            AddS64LE(in_rDesc.m_Min.m_s64Value);
         }
         if(in_rDesc.m_Max.m_s64Value)
         {
            l_u8Flag |= (1 << 6);
            AddS64LE(in_rDesc.m_Max.m_s64Value);
         }
         if(in_rDesc.m_Step.m_s64Value)
         {
            l_u8Flag |= (1 << 5);
            AddS64LE(in_rDesc.m_Step.m_s64Value);
         }
         break;

      // Запись U64 параметров
      case IVT_U64:
         if(in_rDesc.m_Min.m_u64Value)
         {
            l_u8Flag |= (1 << 7);
            AddU64LE(in_rDesc.m_Min.m_u64Value);
         }
         if(in_rDesc.m_Max.m_u64Value)
         {
            l_u8Flag |= (1 << 6);
            AddU64LE(in_rDesc.m_Max.m_u64Value);
         }
         if(in_rDesc.m_Step.m_u64Value)
         {
            l_u8Flag |= (1 << 5);
            AddU64LE(in_rDesc.m_Step.m_u64Value);
         }
         break;

      // Запись F64 параметров
      case IVT_F64:
         if(in_rDesc.m_Min.m_f64Value != 0.0)
         {
            l_u8Flag |= (1 << 7);
            AddF64LE(in_rDesc.m_Min.m_f64Value);
         }
         if(in_rDesc.m_Max.m_f64Value != 0.0)
         {
            l_u8Flag |= (1 << 6);
            AddF64LE(in_rDesc.m_Max.m_f64Value);
         }
         if(in_rDesc.m_Step.m_f64Value != 0.0)
         {
            l_u8Flag |= (1 << 5);
            AddF64LE(in_rDesc.m_Step.m_f64Value);
         }
         break;

      // Запись Time параметров
      case IVT_TIME:
         l_u8Flag |= (7 << 5);
         AddTime(in_rDesc.m_Min.m_Time);
         AddTime(in_rDesc.m_Max.m_Time);
         AddTime(in_rDesc.m_Step.m_Time);
         break;

      default:
         SetError();
         break;
      }

      // Добавление единиц измерения
      if(in_rDesc.m_u16Units)
      {
         l_u8Flag |= (1 << 4);
         AddU16LE(in_rDesc.m_u16Units);
      }

      // Запишем значение типа
      l_u8Type |= l_u8Flag;
      SetAnchorLE(l_pAnchor, &l_u8Type, 1);
   }

   // Добавление текстового описания канала
#if defined(IRIDIUM_MCU_AVR)
   // Добавление имени устройства
   if(in_rDesc.Mem.m_bDesc)
      AddStringFromFlash(in_rDesc.m_pszDescription);
   else
      AddString(in_rDesc.m_pszDescription);
#else
   // Добавление имени устройства
   AddString(in_rDesc.m_pszDescription);
#endif
}

/**
   Добавление описания канала обратной связи
   на входе    :  in_rDesc - ссылка на описание канала обратной связи
   на выходе   :  успешность
*/
void CIridiumOutBuffer::AddTagDescription(iridium_tag_description_t& in_rDesc)
{
   u8 l_u8Temp = 0;
   //u16 l_u16Temp = 0;
   // Добавление общего описания канала
   AddDescription(in_rDesc.m_ID);
   // Выставим флаг владения каналом обратной связи списком глобальных переменных
   if(in_rDesc.m_Flags.m_bOwner)
      l_u8Temp |= 0x80;
   // Добавление списка флагов
   AddU8(l_u8Temp);
   // Добавление идентификатора глобальной переменной
   if(in_rDesc.m_u16Variable)
      AddU16LE(in_rDesc.m_u16Variable);
}

/**
   Добавление описания канала управления
   на входе    :  in_rDesc - ссылка на описание канала управления
   на выходе   :  успешность
*/
void CIridiumOutBuffer::AddChannelDescription(iridium_channel_description_t& in_rDesc)
{
   u8 l_u8Temp = 0;
   //u16 l_u16Temp = 0;
   // Добавление общего описания канала
   AddDescription(in_rDesc.m_ID);
   // Выставим флаг наличия глобальной переменной
   if(in_rDesc.m_u8MaxVariables)
   {
      // Добавление максимального количества переменных на канал
      l_u8Temp = 0x80 | ((in_rDesc.m_u8MaxVariables - 1) & 0x1F);
   }
   // Выставим флаг изменения значения канала по паролю
   if(in_rDesc.m_Flags.m_bWritePassword)
      l_u8Temp |= 0x40;
   // Выставим флаг чтения значения канала по паролю
   if(in_rDesc.m_Flags.m_bReadPassword)
      l_u8Temp |= 0x20;
   // Добавление списка флагов
   AddU8(l_u8Temp);
   l_u8Temp = 0;
   // Зарезервируем место под количество глобальных переменных связзанных с каналом управления
   u8* l_pAnchor = CreateAnchor(1);
   // Добавление списка глобальных переменных
   for(u8 i = 0; i < in_rDesc.m_u8Variables; i++)
   {
      // Проверка наличия переменной
      if(in_rDesc.m_pVariables[i])
      {
         // Проверка количества переменных
         if(l_u8Temp < in_rDesc.m_u8MaxVariables)
         {
            // Добавление переменной
            AddU16LE(in_rDesc.m_pVariables[i]);
            l_u8Temp++;

         } else
            break;
      }
   }
   // Добавление количества переменных канала
   if(l_u8Temp)
      l_u8Temp = 0x80 | ((l_u8Temp - 1) & 0x1F);
   // Запишем количество глобальных переменных связзанных с каналом управления
   SetAnchorLE(l_pAnchor, &l_u8Temp, 1);
}

/**
   Добавление в заголовок сообщения флага окончания цепочки
   на входе    :  in_bEnd  - флаг окончания цепочки 
   на выходе   :  *
*/
void CIridiumOutBuffer::SetMessageHeaderEnd(bool in_bEnd)
{
   if(m_pMessage)
   {
      u8 l_u8Byte = m_pMessage[0] & ~(1 << 4);
      m_pMessage[0] = l_u8Byte | in_bEnd << 4;
   }
}
