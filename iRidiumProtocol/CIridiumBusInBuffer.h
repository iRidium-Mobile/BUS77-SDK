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
#ifndef _C_IRIDIUM_BUS_IN_BUFFER_H_INCLUDED_
#define _C_IRIDIUM_BUS_IN_BUFFER_H_INCLUDED_

// Включения
#include "CIridiumInBuffer.h"

//////////////////////////////////////////////////////////////////////////
// class CIridiumBusInBuffer
//////////////////////////////////////////////////////////////////////////
class CIridiumBusInBuffer : public CIridiumInBuffer
{
public:
   // Конструктор/деструктор
   CIridiumBusInBuffer();
   virtual ~CIridiumBusInBuffer();

   // Открытие/закрытие пакета
   virtual s8 OpenPacket();
   virtual void ClosePacket();
};
#endif   // _C_IRIDIUM_BUS_IN_BUFFER_H_INCLUDED_
