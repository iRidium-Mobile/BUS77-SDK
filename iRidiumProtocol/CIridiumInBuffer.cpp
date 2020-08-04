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
#include "CIridiumInBuffer.h"

/**
   Конструктор класса
   на входе    :  *
*/
CIridiumInBuffer::CIridiumInBuffer() : CInBuffer()
{
}

/**
   Деструктор класса
*/
CIridiumInBuffer::~CIridiumInBuffer()
{
}

/**
   Очистка буфера
   на входе    :  *
   на выходе   :  *
*/
void CIridiumInBuffer::Clear()
{
   // Вызов родителя
   CInBuffer::Clear();
   // Очистка
   memset(&m_Packet, 0, sizeof(m_Packet));
}

/**
   Получение заголовка сообщения из потока
   на входе    :  out_rMH  - ссылка на структуру куда нужно поместить данные заголовка
   на выходе   :  успешность получения данных
*/
bool CIridiumInBuffer::GetMessageHeader(iridium_message_header_t& out_rMH)
{
   bool l_bResult = false;
   u8 l_u8Flags = 0;
   // Получение флагов
   l_bResult = GetU8(l_u8Flags);
   if(l_bResult)
   {
      // Разбор флагов
      out_rMH.m_Flags.m_bDirection = (l_u8Flags >> 7) & 1;
      out_rMH.m_Flags.m_bError = (l_u8Flags >> 6) & 1;
      out_rMH.m_Flags.m_bNoTID = (l_u8Flags >> 5) & 1;
      out_rMH.m_Flags.m_bEnd = (l_u8Flags >> 4) & 1;
      out_rMH.m_Flags.m_u4Version = l_u8Flags & 0xf;

      // Получение типа сообщения
      l_bResult = GetU8(out_rMH.m_u8Type);
      // Получение идентификатора транзации
      if(l_bResult && !out_rMH.m_Flags.m_bNoTID)
         l_bResult = GetU16LE(out_rMH.m_u16TID);
   }
   return l_bResult;
}

/**
   Получение информаци об устройстве из потока
   на входе    :  out_rInfo   - ссылка на структуру куда нужно поместить информацию об устройстве
   на выходе   :  успешность получения данных
*/
bool CIridiumInBuffer::GetSearchInfo(iridium_search_info_t& out_rInfo)
{
   bool l_bResult = false;

   // Получение группы клиента
   l_bResult = GetU8(out_rInfo.m_u8Group);
   if(l_bResult)
   {
      // Получение HWID
      l_bResult = GetString(out_rInfo.m_pszHWID);
   }
   return l_bResult;
}

/**
   Получение информаци об устройстве из потока
   на входе    :  out_rInfo   - ссылка на структуру куда нужно поместить информацию об устройстве
   на выходе   :  успешность получения данных
*/
bool CIridiumInBuffer::GetDeviceInfo(iridium_device_info_t& out_rInfo)
{
   bool l_bResult = false;

   // Подготовка структуры
   memset(&out_rInfo, 0, sizeof(iridium_device_info_t));

   // Получение группы клиента
   l_bResult = GetU8(out_rInfo.m_u8Group);
   if(l_bResult)
   {
      // Получение имени устройства
      l_bResult = GetString(out_rInfo.m_pszName);
      // Получение имени операционной системы клиента
      if(l_bResult)
         l_bResult = GetString(out_rInfo.m_pszProducer);
      // Получение имени модели устройства
      if(l_bResult)
         l_bResult = GetString(out_rInfo.m_pszModel);
      // Получение HWID
      if(l_bResult)
         l_bResult = GetString(out_rInfo.m_pszHWID);
      // Получение типа операционной системы и версии
      if(l_bResult)
         l_bResult = GetU8(out_rInfo.m_u8Class);
      if(l_bResult)
         l_bResult = GetU8(out_rInfo.m_u8Processor);
      if(l_bResult)
         l_bResult = GetU8(out_rInfo.m_u8OS);
      if(l_bResult)
         l_bResult = GetU8(out_rInfo.m_u8Flags);
      if(l_bResult)
         l_bResult = GetU8(out_rInfo.m_u8FirmwareID);
      if(l_bResult)
         l_bResult = FillData(out_rInfo.m_aVersion, 3);
      // Получение количества каналов управления и обратной связи
      if(l_bResult)
      {
         GetU32LE(out_rInfo.m_u32Channels);
         GetU32LE(out_rInfo.m_u32Tags);
      }
   }
   return l_bResult;
}

/**
   Получение общего описания канала
   на входе    :  out_rDesc   - ссылка на структуру куда нужно поместить общее описание канала
   на выходе   :  успешность получения данных
*/
bool CIridiumInBuffer::GetDescription(iridium_description_t& out_rDesc)
{
   bool l_bResult = false;
   bool l_bMin = false;
   bool l_bMax = false;
   bool l_bStep = false;

   memset(&out_rDesc, 0, sizeof(iridium_description_t));
   out_rDesc.m_u8Type = IVT_NONE;

   // Получение типа значения
   l_bResult = GetU8(out_rDesc.m_u8Type);
   if(l_bResult)
   {
      // Проверка наличия данных
      l_bMin = (0 != (out_rDesc.m_u8Type & 0x80));
      l_bMax = (0 != (out_rDesc.m_u8Type & 0x40));
      l_bStep = (0 != (out_rDesc.m_u8Type & 0x20));
      // Отделение типа
      out_rDesc.m_u8Type &= 0x1F;
      switch(out_rDesc.m_u8Type)
      {
      // Если тип не определен
      case IVT_NONE:
         break;
      // Получение бинарного значения
      case IVT_BOOL:
         out_rDesc.m_Min.m_bValue = l_bMin;
         out_rDesc.m_Max.m_bValue = l_bMax;
         out_rDesc.m_Step.m_bValue = l_bStep;
         break;
      // Получение S8 значения
      case IVT_S8:
         if(l_bMin)
            l_bResult = GetS8(out_rDesc.m_Min.m_s8Value);
         if(l_bMax && l_bResult)
            l_bResult = GetS8(out_rDesc.m_Max.m_s8Value);
         if(l_bStep && l_bResult)
            l_bResult = GetS8(out_rDesc.m_Step.m_s8Value);
         break;
      // Получение U8 значения
      case IVT_U8:
         if(l_bMin)
            l_bResult = GetU8(out_rDesc.m_Min.m_u8Value);
         if(l_bMax && l_bResult)
            l_bResult = GetU8(out_rDesc.m_Max.m_u8Value);
         if(l_bStep && l_bResult)
            l_bResult = GetU8(out_rDesc.m_Step.m_u8Value);
         break;
      // Получение S16 значения
      case IVT_S16:
         if(l_bMin)
            l_bResult = GetS16LE(out_rDesc.m_Min.m_s16Value);
         if(l_bMax && l_bResult)
            l_bResult = GetS16LE(out_rDesc.m_Max.m_s16Value);
         if(l_bStep && l_bResult)
            l_bResult = GetS16LE(out_rDesc.m_Step.m_s16Value);
         break;
      // Получение U16 значения
      case IVT_U16:
         if(l_bMin)
            l_bResult = GetU16LE(out_rDesc.m_Min.m_u16Value);
         if(l_bMax && l_bResult)
            l_bResult = GetU16LE(out_rDesc.m_Max.m_u16Value);
         if(l_bStep && l_bResult)
            l_bResult = GetU16LE(out_rDesc.m_Step.m_u16Value);
         break;
      // Получение S32 значения
      case IVT_S32:
         if(l_bMin)
            l_bResult = GetS32LE(out_rDesc.m_Min.m_s32Value);
         if(l_bMax && l_bResult)
            l_bResult = GetS32LE(out_rDesc.m_Max.m_s32Value);
         if(l_bStep && l_bResult)
            l_bResult = GetS32LE(out_rDesc.m_Step.m_s32Value);
         break;
      // Получение U32 значения
      case IVT_U32:
      // Получение параметров строки
      case IVT_STRING8:
      // Получение параметров массва
      case IVT_ARRAY_U8:
         if(l_bMin)
            l_bResult = GetU32LE(out_rDesc.m_Min.m_u32Value);
         if(l_bMax && l_bResult)
            l_bResult = GetU32LE(out_rDesc.m_Max.m_u32Value);
         if(l_bStep && l_bResult)
            l_bResult = GetU32LE(out_rDesc.m_Step.m_u32Value);
         break;
      // Получение F32 значения
      case IVT_F32:
         if(l_bMin)
            l_bResult = GetF32LE(out_rDesc.m_Min.m_f32Value);
         else
            out_rDesc.m_Min.m_f32Value = 0.0f;
         if(l_bMax && l_bResult)
            l_bResult = GetF32LE(out_rDesc.m_Max.m_f32Value);
         else
            out_rDesc.m_Max.m_f32Value = 0.0f;
         if(l_bStep && l_bResult)
            l_bResult = GetF32LE(out_rDesc.m_Step.m_f32Value);
         else
            out_rDesc.m_Step.m_f32Value = 0.0f;
         break;
      // Получение S64 значения
      case IVT_S64:
         if(l_bMin)
            l_bResult = GetS64LE(out_rDesc.m_Min.m_s64Value);
         if(l_bMax && l_bResult)
            l_bResult = GetS64LE(out_rDesc.m_Max.m_s64Value);
         if(l_bStep && l_bResult)
            l_bResult = GetS64LE(out_rDesc.m_Step.m_s64Value);
         break;
      // Получение U64 значения
      case IVT_U64:
         if(l_bMin)
            l_bResult = GetU64LE(out_rDesc.m_Min.m_u64Value);
         if(l_bMax && l_bResult)
            l_bResult = GetU64LE(out_rDesc.m_Max.m_u64Value);
         if(l_bStep && l_bResult)
            l_bResult = GetU64LE(out_rDesc.m_Step.m_u64Value);
         break;
      // Получение F64 значения
      case IVT_F64:
         if(l_bMin)
            l_bResult = GetF64LE(out_rDesc.m_Min.m_f64Value);
         else
            out_rDesc.m_Min.m_f64Value = 0.0;
         if(l_bMax && l_bResult)
            l_bResult = GetF64LE(out_rDesc.m_Max.m_f64Value);
         else
            out_rDesc.m_Max.m_f64Value = 0.0;
         if(l_bStep && l_bResult)
            l_bResult = GetF64LE(out_rDesc.m_Step.m_f64Value);
         else
            out_rDesc.m_Step.m_f64Value = 0.0;
         break;
      // Получение значения времени
      case IVT_TIME:
         if(l_bMin)
            l_bResult = GetTime(out_rDesc.m_Min.m_Time);
         if(l_bMax && l_bResult)
            l_bResult = GetTime(out_rDesc.m_Max.m_Time);
         if(l_bStep && l_bResult)
            l_bResult = GetTime(out_rDesc.m_Step.m_Time);
         break;
      default:
         l_bResult = false;
         break;
      }
   }

   // Получение текстового описания канала
   if(l_bResult)
      l_bResult = GetString(out_rDesc.m_pszDescription);
   return l_bResult;
}

/**
   Получение описания канала обратной связи
   на входе    :  out_rDesc   - ссылка на структуру куда нужно поместить описание канала обратной связи
   на выходе   :  успешность получения данных
*/
bool CIridiumInBuffer::GetTagDescription(iridium_tag_description_t& out_rDesc)
{
   u8 l_u8Temp = 0;
   // Получение общих данных
   bool l_bResult = GetDescription(out_rDesc.m_ID);
   if(l_bResult)
   {
      // Чтение списка флагов
      l_bResult = GetU8(l_u8Temp);
      if(l_bResult)
      {
         // Получение флага владения
         out_rDesc.m_Flags.m_bOwner = (l_u8Temp >> 7) & 1;
         // Получение связаной с каналом обратной связи глобальной переменной
         if(Size() >= 2)
            l_bResult = GetU16LE(out_rDesc.m_u16Variable);
         else
            out_rDesc.m_u16Variable = 0;
      }
   }
   return l_bResult;
}

/**
   Получение описания канала управления
   на входе    :  out_rDesc - ссылка на структуру куда нужно поместить описание канала управления
   на выходе   :  успешность получения данных
   примечание  :  [V][W][R][M][M][M][M][M]
                  V  - флаг наличия глобальной переменной
                  W  - флаг наличия пароля для измения значения
                  R  - флаг наличия пароля для чтения значения
                  M  - максимальное количество глобальных переменных - 1
*/
bool CIridiumInBuffer::GetChannelDescription(iridium_channel_description_t& out_rDesc)
{
   u8 l_u8Temp = 0;
   // Получение общих данных
   bool l_bResult = GetDescription(out_rDesc.m_ID);
   if(l_bResult)
   {
      // Чтение списка флагов
      l_bResult = GetU8(l_u8Temp);
      if(l_bResult)
      {
         // Извлечение флагов доступа
         out_rDesc.m_Flags.m_bWritePassword = (l_u8Temp >> 6) & 1;
         out_rDesc.m_Flags.m_bReadPassword = (l_u8Temp >> 5) & 1;

         // Определение количества переменных
         out_rDesc.m_u8MaxVariables = 0;
         if(l_u8Temp & 0x80)
            out_rDesc.m_u8MaxVariables = (l_u8Temp & 0x1F) + 1;
         
         // Подготовка переменных
         out_rDesc.m_pVariables = NULL;
         // Получение количества глобальных переменных
         l_bResult = GetU8(out_rDesc.m_u8Variables);
         if(l_bResult)
         {
            // Проверка на максимальное колличество переменных
            if(out_rDesc.m_u8Variables > out_rDesc.m_u8MaxVariables)
               out_rDesc.m_u8Variables = out_rDesc.m_u8MaxVariables;
         
            // Проверка наличия глобальных переменных
            if(out_rDesc.m_u8Variables)
            {
               // Проверка наличия данных глобальных переменных
               if(Size() >= (size_t)(out_rDesc.m_u8Variables * 2))
               {
                  // Запомним количество переменных и указатель на глобальные переменные
                  out_rDesc.m_pVariables = (u16*)GetDataPtr();
                  // Пропустим массив глобальных переменных (количество * sizeof(u16))
                  Skip(out_rDesc.m_u8Variables * 2);
               } else
                  l_bResult = false;
            }
         }
      }
   }
   return l_bResult;
}
