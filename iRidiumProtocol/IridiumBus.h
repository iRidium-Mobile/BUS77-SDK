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
#ifndef _IRIDIUM_BUS_H_INCLUDED_
#define _IRIDIUM_BUS_H_INCLUDED_

// Включения
#include "Iridium.h"

//////////////////////////////////////////////////////////////////////////
// BUS Шинная версия протокола
//////////////////////////////////////////////////////////////////////////
// Маркеры начала пакета
#define IRIDIUM_BUS_PROTOCOL_ID        0x75        // Идентификатор начала версии протокола для шин 77, 232/422/485, CAN

#define IRIDIUM_PROTOCOL_BUS_VERSION   1           // Версия заголовка протокола для шин 77, 232/422/485, CAN

// Минимальный и максимальный размер шинного заголовка пакета
#define IRIDIUM_BUS_MIN_HEADER_SIZE    3
#define IRIDIUM_BUS_MAX_HEADER_SIZE    7

// Размер CRC в байтах
#define IRIDIUM_BUS_CRC_SIZE           2

// Минимальный и максимальный размер тела шинного пакета
#define IRIDIUM_BUS_MIN_BODY_SIZE      2
#define IRIDIUM_BUS_MAX_BODY_SIZE      255

// Размер входящего и исходящего шинного буфера
#define IRIDIUM_BUS_OUT_BUFFER_SIZE    (IRIDIUM_BUS_MAX_HEADER_SIZE + IRIDIUM_BUS_MAX_BODY_SIZE)
#define IRIDIUM_BUS_IN_BUFFER_SIZE     (IRIDIUM_BUS_OUT_BUFFER_SIZE * 2)

#endif   // _IRIDIUM_BUS_H_INCLUDED_

