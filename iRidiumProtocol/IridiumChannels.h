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
// Описание битовых полей старшего байта 
//  80 40 20 10 08 04 02 01 
// [S][S][A][0][0][0][0][0]
//  |  |  |  |  |  |  |  |
//  |  |  |  |  |  |  |  +--- Зарезервировано
//  |  |  |  |  |  |  +------ Зарезервировано
//  |  |  |  |  |  +--------- Зарезервировано
//  |  |  |  |  +------------ Зарезервировано
//  |  |  |  +--------------- Зарезервировано
//  |  |  +------------------ Признак канала указывающий что для применения настроек нужно рименить настройки
//  +--+--------------------- Признак системного канала BUS 77, если канал системный - биты должны быть равны 1

// Флаги
#define SYSTEM_MARKER                  0xC0000000                                   // Флаг указывающий что канал является системным и принадлежит BUS 77
#define SYSTEM_NEED_APPLY              0x20000000                                   // Флаг указывающий что канал требует применение параметров через вызов канала SYSTEM_CHANNEL_APPLY
// Маски
#define SYSTEM_ID_MASK                 0x000FFFFF                                   // Маска для получения числа
#define SYSTEM_MARKER_MASK             0xC0000000                                   // Маска для проверки признака системности канала
#define SYSTEM_NEED_APPLY_MASK         0x20000000                                   // Маска для проверки признака применения параметров

#define SYSTEM_CHANNEL_REBOOT          (SYSTEM_MARKER | 1)                          // Канал перезагрузки устройства (IVT_NONE)
#define SYSTEM_CHANNEL_NAME            (SYSTEM_MARKER | 2)                          // Имя устройства (IVT_STRING8)
#define SYSTEM_CHANNEL_PIN             (SYSTEM_MARKER | 3)                          // Пин код для изменения значения (IVT_U32)
#define SYSTEM_CHANNEL_DATE_AND_TIME   (SYSTEM_MARKER | 4)                          // Установка даты и времени (IVT_TIME)
#define SYSTEM_CHANNEL_RESET           (SYSTEM_MARKER | 5)                          // Сброс контроллера до заводских настроек (IVT_NONE)
#define SYSTEM_CHANNEL_APPLY           (SYSTEM_MARKER | 6)                          // Применение настроек (IVT_BOOL)
#define SYSTEM_CHANNEL_USER_ID         (SYSTEM_MARKER | 7)                          // Определенный пользователей идентификатор
#define SYSTEM_CHANNEL_CLOUD_TIME      (SYSTEM_MARKER | 8)                          // Включить облачную синхронизацию времени (IVT_BOOL)
#define SYSTEM_CHANNEL_TIMEZONE        (SYSTEM_MARKER | 9)                          // Часовой пояс (IVT_S8)
#define SYSTEM_CHANNEL_BLINK           (SYSTEM_MARKER | 10)                         // Включение/выключение мигания (IVT_BOOL)
#define SYSTEM_CHANNEL_BLINK_TIME      (SYSTEM_MARKER | 11)                         // Время мигания при включении канала мигания, в миллисекундах (IVT_U32)

// Сетевые параметры
#define SYSTEM_CHANNEL_DHCP            (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 100)    // Флга использования DHCP (IVT_BOOL)
#define SYSTEM_CHANNEL_IP              (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 101)    // IP адрес шлюза (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_PORT            (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 102)    // Порт шлюза (IVT_U16)
#define SYSTEM_CHANNEL_MASK            (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 103)    // Маска сети (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_DNS             (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 104)    // IP адрес DNS шлюза (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_GATEWAY         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 105)    // IP адрес сетевого шлюза (IVT_ARRAY_U8)

// Данные для работы с сервером авторизации
#define SYSTEM_CHANNEL_AS_TOKEN        (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 107)    // Токен доступа к серверу авторизации (IVT_STRING_U8)
#define SYSTEM_CHANNEL_AS_HOST         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 108)    // Хост сервера авторизации (IVT_STRING_U8)
#define SYSTEM_CHANNEL_AS_PORT         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 109)    // Порт сервера авторизации (IVT_U16)

// Шина CAN
#define SYSTEM_CHANNEL_CAN_0_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 120)    // Скорость CAN-шины 1 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_1_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 121)    // Скорость CAN-шины 2 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_2_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 122)    // Скорость CAN-шины 3 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_3_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 123)    // Скорость CAN-шины 4 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_4_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 124)    // Скорость CAN-шины 5 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_5_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 125)    // Скорость CAN-шины 6 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_6_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 126)    // Скорость CAN-шины 7 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_7_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 127)    // Скорость CAN-шины 8 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_8_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 128)    // Скорость CAN-шины 9 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_9_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 129)    // Скорость CAN-шины 10 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_10_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 130)    // Скорость CAN-шины 11 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_11_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 131)    // Скорость CAN-шины 12 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_12_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 132)    // Скорость CAN-шины 13 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_13_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 133)    // Скорость CAN-шины 14 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_14_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 134)    // Скорость CAN-шины 15 (IVT_U8)
#define SYSTEM_CHANNEL_CAN_15_BAUDRATE	(SYSTEM_MARKER | SYSTEM_NEED_APPLY | 135)    // Скорость CAN-шины 16 (IVT_U8)

// Ключи шифрования
#define SYSTEM_CHANNEL_UDP_KEY         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 200)    // Ключ шифрования для работы с по UDP (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_EXT_CAN_1_KEY   (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 210)    // Ключ шифрования внешнего CAN интерфейса (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_INT_CAN_1_KEY   (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 220)    // Ключ шифрования внутреннего CAN интерфейса (IVT_ARRAY_U8)

#endif   // _IRIDIUM_CHANNELS_H_INCLUDED_

