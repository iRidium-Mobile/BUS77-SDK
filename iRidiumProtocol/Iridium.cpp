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
#include "Iridium.h"

// Таблица с версиями сообщения 
u8 g_aVersion[] =
{
   0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x00, 0x00, // 0x00-0x09
   0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10-0x11
   0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, // 0x20-0x27
   0x11, 0x11, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, // 0x30-0x36
   0x12, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x40-0x43
   0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x50-0x52
};

/**
   Получение версии сообщения
   на входе    :  in_eType - тип сообщения
   на выходе   :  версия сообщения, 0 тип не найден
*/
u8 GetMessageVersion(u8 in_u8Type)
{
   u8 l_u8Ver = 0;
   if(in_u8Type < IRIDIUM_MESSAGE_MAX)
   {
      l_u8Ver = g_aVersion[in_u8Type >> 1];
      if(in_u8Type & 1)
         l_u8Ver &= 0xF;
      else
         l_u8Ver >>= 4;
   }
   return l_u8Ver;
}
