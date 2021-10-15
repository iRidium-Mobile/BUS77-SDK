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

// Типы устройств
enum eDeviceAPIType
{
   DEVICE_API_TYPE_SYSTEM  = 0,                    // Системные
   DEVICE_API_TYPE_RELAY,                          // Реле
   DEVICE_API_TYPE_BUTTON,                         // Кнопка
   DEVICE_API_TYPE_IP_INTERFACE,                   // IP интерфейс
   DEVICE_API_TYPE_DIMMER,                         // Диммер
   DEVICE_API_TYPE_VALVE,                          // Кран
   DEVICE_API_TYPE_LEAKAGE_SENSOR,                 // Датчик протечки
   DEVICE_API_TYPE_TEMPERATURE_SENSOR,             // Датчик температуры
   DEVICE_API_TYPE_THERMOSTAT,                     // Термостат
   DEVICE_API_TYPE_LIGTH_SENSOR,                   // Датчик света
   DEVICE_API_TYPE_MOTION_SENSOR,                  // Датчик движения
   DEVICE_API_TYPE_SOUND_SENSOR,                   // Датчик звука
   DEVICE_API_TYPE_WARM_SENSOR,                    // Датчик влажности
   DEVICE_API_TYPE_CO2_SENSOR,                     // Датчик CO2
   DEVICE_API_TYPE_VOC_SENSOR,                     // Датчик качества воздуха
};

// Системные функции каналов управления
enum eChannelSystemFuncton
{
   CHANNEL_SYSTEM_FUNCTION_REBOOT  = 1,            // Перезагрузка
   CHANNEL_SYSTEM_FUNCTION_NAME,                   // Имя
   CHANNEL_SYSTEM_FUNCTION_PIN,                    // пин код
   CHANNEL_SYSTEM_FUNCTION_DATE_TIME,              // Дата и время
   CHANNEL_SYSTEM_FUNCTION_RESET,                  // Сброс параметров
   CHANNEL_SYSTEM_FUNCTION_APPLY,                  // Применение значений
   CHANNEL_SYSTEM_FUNCTION_USER_ID,                // Пользовательский параметр
   CHANNEL_SYSTEM_FUNCTION_CLOUD_TIME,             // Включение/выключение синхронизации времени с облаком
   CHANNEL_SYSTEM_FUNCTION_TIME_ZONE,              // Часовой пояс
   CHANNEL_SYSTEM_FUNCTION_BLINK_LED,              // Включение/выключение мерцания
   CHANNEL_SYSTEM_FUNCTION_BLINK_TIME,             // Время мерцания
};

// Системные функции каналов управления
enum eTagSystemFuncton
{
   TAG_SYSTEM_FUNCTION_DEVICE_STATUS   = 1,
   TAG_SYSTEM_FUNCTION_CORE_TEMPERATURE,
   TAG_SYSTEM_FUNCTION_DEVICE_VOLTAGE,
   TAG_SYSTEM_FUNCTION_DEVICE_TEMPERATURE,
   TAG_SYSTEM_FUNCTION_TEMPERATURE_OVERLOAD,
   TAG_SYSTEM_FUNCTION_CURRENT_RESOURCE,
   TAG_SYSTEM_FUNCTION_ONTIME,
};

// Релейные функции
enum eChannelRelayFuncton
{
   CHANNEL_RELAY_FUNCTION_INVERSION   = 1,         // Инверсия
   CHANNEL_RELAY_FUNCTION_AUTO_OFF_TIME,           // Время автоотключения
   CHANNEL_RELAY_FUNCTION_BEHAVIOR_ON_START,       // Востановление последнего значения
   CHANNEL_RELAY_FUNCTION_STATE,                   // Установка состояния выходы
   CHANNEL_RELAY_FUNCTION_SWITCH_ON,               // Включение выхода
   CHANNEL_RELAY_FUNCTION_SWITCH_OFF,              // Выключение выходп
   CHANNEL_RELAY_FUNCTION_TOGGLE,                  // Перекидывание значения
};

// Релейные функции
enum eTagRelayFuncton
{
   TAG_RELAY_FUNCTION_STATE   = 1,
   TAG_RELAY_FUNCTION_ON,
   TAG_RELAY_FUNCTION_OFF,
};

// Кнопочные функции
enum eChannelButtonFuncton
{
   CHANNEL_BUTTON_FUNCTION_INVERSION   = 1,        // Инверсия
   CHANNEL_BUTTON_FUNCTION_MULTIPRESS_TIMEOUT,     // 
   CHANNEL_BUTTON_FUNCTION_LONGPRESS_TIMEOUT,      // 
   CHANNEL_BUTTON_FUNCTION_LONGPRESS_REPEAT_TIME,  //
};

// Кнопочные функции
enum eTagButtonFuncton
{
   TAG_BUTTON_FUNCTION_STATE   = 1,                //
   TAG_BUTTON_FUNCTION_PRESS,                      // 
   TAG_BUTTON_FUNCTION_RELEASE,                    // 
   TAG_BUTTON_FUNCTION_SINGLE_PRESS,               //
   TAG_BUTTON_FUNCTION_DOUBLE_PRESS,               //
   TAG_BUTTON_FUNCTION_TRIPLE_PRESS,               //
   TAG_BUTTON_FUNCTION_PANIC_PRESS,                //
   TAG_BUTTON_FUNCTION_LONG_PRESS_START,           //
   TAG_BUTTON_FUNCTION_LONG_PRESS_REPETITION,      //
   TAG_BUTTON_FUNCTION_LONG_PRESS_END,             //
};

// Диммер функции
enum eChannelDimmerFuncton
{
   CHANNEL_DIMMER_FUNCTION_TARGET_LEVEL   = 1,     //
   CHANNEL_DIMMER_FUNCTION_CURRENT_LEVEL_REFRESH_TIME,   //
   CHANNEL_DIMMER_FUNCTION_SWITCH_ON,              // 
   CHANNEL_DIMMER_FUNCTION_SWITCH_ON_MAX,          //
   CHANNEL_DIMMER_FUNCTION_SWITCH_ON_MIN,          //
   CHANNEL_DIMMER_FUNCTION_TOGGLE,                 //
   CHANNEL_DIMMER_FUNCTION_SWITCH_OFF,             //
   CHANNEL_DIMMER_FUNCTION_BEHAVIOR_ON_START,      //
   CHANNEL_DIMMER_FUNCTION_VALUE_ON_START,         //
   CHANNEL_DIMMER_FUNCTION_UP,                     //
   CHANNEL_DIMMER_FUNCTION_DOWN,                   //
   CHANNEL_DIMMER_FUNCTION_FADE_IN,                //
   CHANNEL_DIMMER_FUNCTION_FADE_OUT,               //
   CHANNEL_DIMMER_FUNCTION_FADE_IN_CURVE,          //
   CHANNEL_DIMMER_FUNCTION_FADE_OUT_CURVE,         //
   CHANNEL_DIMMER_FUNCTION_STEP,                   //
   CHANNEL_DIMMER_FUNCTION_FLICKER_THRESHOLD,      //
   CHANNEL_DIMMER_FUNCTION_MIN_LEVEL,              //
   CHANNEL_DIMMER_FUNCTION_MAX_LEVEL,              //
   CHANNEL_DIMMER_FUNCTION_TYPE_OF_LIGHT,          //
   CHANNEL_DIMMER_FUNCTION_ONTIME_RESOURCE,        //
};

// Диммер функции
enum eTagDimmerFuncton
{
   TAG_DIMMER_FUNCTION_TARGET_LEVEL   = 1,         //
   TAG_DIMMER_FUNCTION_CURRENT_LEVEL,              //
   TAG_DIMMER_FUNCTION_TARGET_LEVEL_REACHED,       // 
   TAG_DIMMER_FUNCTION_ON,                         //
   TAG_DIMMER_FUNCTION_OFF,                        //
   TAG_DIMMER_FUNCTION_STATE,                      //
   TAG_DIMMER_FUNCTION_MIN_REACHED,                //
   TAG_DIMMER_FUNCTION_MAX_REACHED,                //
   TAG_DIMMER_FUNCTION_CHANNEL_STATUS,             //
   TAG_DIMMER_FUNCTION_OPEN_CIRCUIT,               //
   TAG_DIMMER_FUNCTION_LAST_ONTIME_SESSION,        //
   TAG_DIMMER_FUNCTION_CURRENT_ONTIME_RESOURCE,    //
   TAG_DIMMER_FUNCTION_ONTIME_RESOURCE_EXCEDED,    //
};

/////////////////////////////////////////////////////////////////////////////////
// Идентификаторы системных каналов управления
/////////////////////////////////////////////////////////////////////////////////
// Описание битовых полей старшего байта 
//  80 40 20 10 08 04 02 01 
// [S][S][A][X][0][0][0][0]
//  |  |  |  |  |  |  |  |
//  |  |  |  |  |  |  |  +--- Зарезервировано, должно быть равно 0
//  |  |  |  |  |  |  +------ Зарезервировано, должно быть равно 0
//  |  |  |  |  |  +--------- Зарезервировано, должно быть равно 0
//  |  |  |  |  +------------ Зарезервировано, должно быть равно 0
//  |  |  |  +--------------- Признак канала указывающий что данный канал можно использовать для добавления в сценарий
//  |  |  +------------------ Признак канала указывающий что для применения настроек нужно рименить настройки
//  +--+--------------------- Признак системного канала BUS 77, если канал системный - биты должны быть равны 1

// Флаги
#define SYSTEM_MARKER                  0xC0000000                                   // Флаг указывающий что канал является системным и принадлежит BUS 77
#define SYSTEM_NEED_APPLY              0x20000000                                   // Флаг указывающий что канал требует применение параметров через вызов канала SYSTEM_CHANNEL_APPLY
#define SYSTEM_SCENARIO                0x10000000                                   // Флаг указывающий что канал можно добавлять в сценарии
// Маски
#define SYSTEM_ID_MASK                 0x000FFFFF                                   // Маска для получения числа
#define SYSTEM_MARKER_MASK             0xC0000000                                   // Маска для проверки признака системности канала
#define SYSTEM_NEED_APPLY_MASK         0x20000000                                   // Маска для проверки признака применения параметров
#define SYSTEM_SCENARIO_MASK           0x10000000                                   // Маска для проверки признака сценария

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
#define SYSTEM_CHANNEL_SCENARIO        (SYSTEM_MARKER | 12)                         // Канал выбора сценария (IVT_U16)

// Сетевые параметры
#define SYSTEM_CHANNEL_DHCP            (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 100)    // Флга использования DHCP (IVT_BOOL)
#define SYSTEM_CHANNEL_IP              (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 101)    // IP адрес шлюза (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_PORT            (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 102)    // Порт шлюза (IVT_U16)
#define SYSTEM_CHANNEL_MASK            (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 103)    // Маска сети (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_DNS             (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 104)    // IP адрес DNS шлюза (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_GATEWAY         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 105)    // IP адрес сетевого шлюза (IVT_ARRAY_U8)
#define SYSTEM_CHANNEL_MAC             (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 106)    // IP адрес сетевого шлюза (IVT_ARRAY_U8)
/*
// Данные для работы с сервером авторизации
#define SYSTEM_CHANNEL_AS_TOKEN        (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 107)    // Токен доступа к серверу авторизации (IVT_STRING_U8)
#define SYSTEM_CHANNEL_AS_HOST         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 108)    // Хост сервера авторизации (IVT_STRING_U8)
#define SYSTEM_CHANNEL_AS_PORT         (SYSTEM_MARKER | SYSTEM_NEED_APPLY | 109)    // Порт сервера авторизации (IVT_U16)
*/
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

