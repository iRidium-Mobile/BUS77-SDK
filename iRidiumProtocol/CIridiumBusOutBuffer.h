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
#ifndef _C_IRIDIUM_BUS_OUT_BUFFER_H_INCLUDED_
#define _C_IRIDIUM_BUS_OUT_BUFFER_H_INCLUDED_

// Включения
#include "CIridiumOutBuffer.h"

// +----------+--------+------------------------------------------------------------------------------------------+
// | Смещение | Размер | Описание                                                                                 |
// +----------+--------+------------------------------------------------------------------------------------------+
// |        0 |      2 | Контрольная сумма CRC16                                                                  |
// |        2 |      1 | Размер данных                                                                            |
// |        3 |      1 | Случайное значение для исключения повторяемости данных пакета, при одинакомом седиржимом |
// +----------+--------+------------------------------------------------------------------------------------------+
#define IRIDIUM_BUS_CIPHER_CRC_OFFSET     0
#define IRIDIUM_BUS_CIPHER_CRC_SIZE       2
#define IRIDIUM_BUS_CIPHER_SIZE_OFFSET    (IRIDIUM_BUS_CIPHER_CRC_OFFSET + IRIDIUM_BUS_CIPHER_CRC_SIZE)
#define IRIDIUM_BUS_CIPHER_SIZE_SIZE      1
#define IRIDIUM_BUS_CIPHER_RAND_OFFSET    (IRIDIUM_BUS_CIPHER_SIZE_OFFSET + IRIDIUM_BUS_CIPHER_SIZE_SIZE)
#define IRIDIUM_BUS_CIPHER_RAND_SIZE      1

// Размер заголовка шифра в шинной реализации
#define IRIDIUM_BUS_CIPHER_HEADER_SIZE    (IRIDIUM_BUS_CIPHER_CRC_SIZE + IRIDIUM_BUS_CIPHER_SIZE_SIZE + IRIDIUM_BUS_CIPHER_RAND_SIZE)

//////////////////////////////////////////////////////////////////////////
// class CIridiumBusOutBuffer
//////////////////////////////////////////////////////////////////////////
class CIridiumBusOutBuffer : public CIridiumOutBuffer
{
public:
   // Конструктор/деструктор
   CIridiumBusOutBuffer();
   virtual ~CIridiumBusOutBuffer();

   // Начало создания данных
   virtual void Begin(size_t in_stBlockSize);
   // Окончание создания данных
   virtual bool End(iridium_packet_header_t& in_pHeader);
};
#endif   // _C_IRIDIUM_BUS_OUT_BUFFER_H_INCLUDED_

