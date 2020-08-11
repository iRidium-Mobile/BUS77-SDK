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
#ifndef _IRIDIUM_CHANNELS_H_INCLUDED_
#define _IRIDIUM_CHANNELS_H_INCLUDED_

/////////////////////////////////////////////////////////////////////////////////
// Идентификаторы системных каналов управления
/////////////////////////////////////////////////////////////////////////////////
// Флаги
#define SYSTEM_MARKER                  0xC0000000                                   // Флаг указывающий что канал является системным и принадлежит BUS 77
#define SYSTEM_NEED_APPLY              0x20000000                                   // Флаг указывающий что канал требует применение параметров через вызов канала SYSTEM_CHANNEL_APPLY
// Маски
#define SYSTEM_ID_MASK                 0x000FFFFF                                   // Маска для получения числа
#define SYSTEM_MARKER_MASK             0xC0000000                                   // Маска для проверки признака системности канала

#define SYSTEM_CHANNEL_REBOOT          (SYSTEM_MARKER | 0)                          // Канал перезагрузки устройства (IVT_NONE)
#define SYSTEM_CHANNEL_NAME            (SYSTEM_MARKER | 1)                          // Имя устройства (IVT_STRING8)
#define SYSTEM_CHANNEL_PIN             (SYSTEM_MARKER | 2)                          // Пин код для изменения значения (IVT_U32)
#define SYSTEM_CHANNEL_DATE_AND_TIME   (SYSTEM_MARKER | 3)                          // Установка даты и времени (IVT_TIME)
#define SYSTEM_CHANNEL_RESET           (SYSTEM_MARKER | 4)                          // Сброс контроллера до заводских настроек (IVT_NONE)
#define SYSTEM_CHANNEL_APPLY           (SYSTEM_MARKER | 5)                          // Применение настроек (IVT_NONE)
#define SYSTEM_CHANNEL_START_BLINK     (SYSTEM_MARKER | 6)                          // Включить мигание устройства (IVT_NONE)
#define SYSTEM_CHANNEL_END_BLINK       (SYSTEM_MARKER | 7)                          // Выключить мигание устройства (IVT_NONE)

// Сетевые параметры
#define SYSTEM_CHANNEL_DHCP            (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 100)    // Флга использования DHCP (IVT_BOOL)
#define SYSTEM_CHANNEL_IP              (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 101)    // IP адрес шлюза (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_PORT            (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 102)    // Порт шлюза (IVT_U16)
#define SYSTEM_CHANNEL_MASK            (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 103)    // Маска сети (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_DNS             (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 104)    // IP адрес DNS шлюза (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_GATEWAY         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 105)    // IP адрес сетевого шлюза (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_CAN_BAUDRATE	   (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 106)    // Скорость CAN-шины (IVT_U8)

// Данные для работы с сервером авторизации
#define SYSTEM_CHANNEL_AS_TOKEN        (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 107)    // Токен доступа к серверу авторизации (IVT_STRING_U8)
#define SYSTEM_CHANNEL_AS_HOST         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 108)    // Хост сервера авторизации (IVT_STRING_U8)
#define SYSTEM_CHANNEL_AS_PORT         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 109)    // Порт сервера авторизации (IVT_U16)

// Ключи шифрования
#define SYSTEM_CHANNEL_UDP_KEY         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 200)    // Ключ шифрования для работы с по UDP (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_EXT_CAN_1_KEY   (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 210)    // Ключ шифрования внешнего CAN интерфейса (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_INT_CAN_1_KEY   (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 220)    // Ключ шифрования внутреннего CAN интерфейса (IVT_ARRAY_U8)

#endif   // _IRIDIUM_CHANNELS_H_INCLUDED_

