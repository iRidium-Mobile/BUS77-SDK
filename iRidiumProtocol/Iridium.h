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
#ifndef _IRIDIUM_H_INCLUDED_
#define _IRIDIUM_H_INCLUDED_

// Включения
#include "IridiumConfig.h"
#include "IridiumTypes.h"
#include "IridiumValue.h"

#define IRIDIUM_PROTOCOL_ID_MASK       0x77        // Маска для идентификации маркера начала пакета
#define IRIDIUM_PROTOCOL_VERSION_MASK  0x18        // Маска для идентификации версии пакета

#define IRIDIUM_REQUEST                false       // Запрос
#define IRIDIUM_RESPONSE               true        // Ответ

#define IRIDIUM_NO_ERROR               false       // Нет ошибки
#define IRIDIUM_ERROR                  true        // Наличие ошибки

// Список флагов
#define IRIDIUM_FLAGS_END              0x01        // Флаг указывает что был получен замыкающий фрагмент значения
#define IRIDIUM_FLAGS_SET              0x02        // Флаг указывает что значение переменной было получено через запрос изменения канала, иначе через запрос получения значения

#define IRIDIUM_MAX_VARIABLES          32          // Максимальное количество переменных на канал управления и обратной связи (Максимальное количество 32)
//////////////////////////////////////////////////////////////////////////
// Идентификаторы контейнеров
//////////////////////////////////////////////////////////////////////////
// Системные команды
#define IRIDIUM_MESSAGE_SYSTEM_PING                0x02  // Пинг
#define IRIDIUM_MESSAGE_SYSTEM_SEARCH              0x03  // Поиск всех устройств
#define IRIDIUM_MESSAGE_SYSTEM_DEVICE_INFO         0x04  // Получение информации об устройстве
#define IRIDIUM_MESSAGE_SYSTEM_SET_LID             0x05  // Установить локальный идентификатор
#define IRIDIUM_MESSAGE_SYSTEM_SESSION_TOKEN       0x08  // Передача токена серверу
#define IRIDIUM_MESSAGE_SYSTEM_SMART_API           0x0A  // Получение информации о Smart API
#define IRIDIUM_MESSAGE_SYSTEM_BLINK               0x0B  // Включение/выключение индикации устройства

// Работа с глобальными переменными
#define IRIDIUM_MESSAGE_SET_VARIABLE               0x10  // Установка значения глобальной переменной
#define IRIDIUM_MESSAGE_GET_VARIABLE               0x11  // Получение значения глобальной переменной
#define IRIDIUM_MESSAGE_DELETE_VARIABLES           0x12  // Удаление списка глобальных переменных
// Работа с каналами обратной связи
#define IRIDIUM_MESSAGE_GET_TAGS                   0x20  // Получение списка каналов обратной связи
#define IRIDIUM_MESSAGE_LINK_TAG_AND_VARIABLE      0x24  // Связывание канала обратной связи и списка глобальных переменных
#define IRIDIUM_MESSAGE_GET_TAG_DESCRIPTION        0x25  // Получение описания канала обратной связи
#define IRIDIUM_MESSAGE_SET_TAG_VALUE              0x26  // Установка значения канала обратной связи
#define IRIDIUM_MESSAGE_GET_TAG_VALUE              0x27  // Получение значения канала обратной связи
// Работа с каналами управления
#define IRIDIUM_MESSAGE_GET_CHANNELS               0x30  // Получение списка каналов управления
#define IRIDIUM_MESSAGE_SET_CHANNEL_VALUE          0x33  // Изменение значения канала управления
#define IRIDIUM_MESSAGE_LINK_CHANNEL_AND_VARIABLE  0x34  // Связывание канала управления и глобальной переменной
#define IRIDIUM_MESSAGE_GET_CHANNEL_DESCRIPTION    0x35  // Получение описания канала управления
#define IRIDIUM_MESSAGE_GET_CHANNEL_VALUE          0x36  // Получение значения канала обратной связи
// Работа с потоками
#define IRIDIUM_MESSAGE_STREAM_OPEN                0x50  // Открытие потока
#define IRIDIUM_MESSAGE_STREAM_BLOCK               0x51  // Блок потока
#define IRIDIUM_MESSAGE_STREAM_CLOSE               0x52  // Закрытие потока
// Работа со сценариями
#define IRIDIUM_MESSAGE_GET_SCENARIOS              0x60  // Получение списка сценариев
#define IRIDIUM_MESSAGE_GET_SCENARIO_ACTIONS       0x61  // Получение данных сценария
#define IRIDIUM_MESSAGE_SET_SCENARIO_ACTIONS       0x62  // Изменение данных сценария
#define IRIDIUM_MESSAGE_GET_SCENARIO_VARIABLES     0x63  // Получение списка глобальных переменных сценария
#define IRIDIUM_MESSAGE_SET_SCENARIO_VARIABLES     0x64  // Изменение списка глобальных переменных сценария

#define IRIDIUM_MESSAGE_MAX                        IRIDIUM_MESSAGE_SET_SCENARIO_VARIABLES // Максимальный номер сообщения

// Группа клиента который подключается к серверу
enum eIridiumGroup
{
   IRIDIUM_GROUP_TYPE_UNKNOWN = 0,                 // Неизвестная группа
   IRIDIUM_GROUP_TYPE_UTILITY,                     // Инструмент разработчика (отладчик/системная программа) (Client)
   IRIDIUM_GROUP_TYPE_PANEL,                       // Панель управления (Client/Server)
   IRIDIUM_GROUP_TYPE_PLC,                         // Програмируемый логический контроллер (Client/Server)
   IRIDIUM_GROUP_TYPE_ACTUATOR,                    // Исполнительное устройство (Server)
   IRIDIUM_GROUP_MAX,                              // Максимальный номер
};

// Флаги свойств устройства
#define IRIDIUM_DEVICE_FLAG_RECERVED               0x80  // [1000 0000] Зарезервировано
#define IRIDIUM_DEVICE_FLAG_GATE                   0x40  // [0100 0000] Устройство имеет собственную шину
#define IRIDIUM_DEVICE_FLAG_FIRMWARE               0x20  // [0010 0000] Устройство может быть перепрошито
#define IRIDIUM_DEVICE_FLAG_BOOT                   0x10  // [0001 0000] Устройство находится в режиме загрузчика
#define IRIDIUM_DEVICE_FLAG_ACTUATOR               0x08  // [0000 1000] Устройство обладает свойствами исполнительного устройства
#define IRIDIUM_DEVICE_FLAG_PLC                    0x04  // [0000 0100] Устройство обладает свойствами свободно програмируемым контроллером
#define IRIDIUM_DEVICE_FLAG_PANEL                  0x02  // [0000 0010] Устройство обладает свойствами панели
#define IRIDIUM_DEVICE_FLAG_UTILITY                0x01  // [0000 0001] Устройство обладает свойствами утилиты

// Флаги запросов списка каналов
// [D][N][0][0][0][0][0][0]
//  |  |  |  |  |  |  |  |
//  |  |  +--+--+--+--+--+--- Зарезервировано, должны быть равны 0
//  |  +--------------------- В ответ не нужно добавлять имя канала
//  +------------------------ В ответ нужно добавить информацию о Device API
#define IRIDIUM_REQUEST_FLAG_DEVICE_API            0x80  // [1000 0000] Запрос списка каналов с данными Device API
#define IRIDIUM_REQUEST_FLAG_WITHOUT_NAME          0x40  // [0100 0000] Запрос списка каналов без имени каналов
#define IRIDIUM_REQUEST_FLAG_MASK                  0xC0  // [1100 0000] Маска запроса списка каналов

// Режим работы с потоком
enum eIridiumStreamMode
{
   IRIDIUM_STREAM_MODE_READ = 0,                   // Открытие потока для чтения
   IRIDIUM_STREAM_MODE_WRITE,                      // Открытие потока для записи
   IRIDIUM_STREAM_MODE_READ_WRITE,                 // Открытие потока для чтения и записи
};

// Тип операции
enum eIridiumOperation
{
   IRIDIUM_OPERATION_TEST_PIN = 0,                 // Проверка PIN кода
   IRIDIUM_OPERATION_READ_CHANNEL,                 // Чтение значения канала управления
   IRIDIUM_OPERATION_WRITE_CHANNEL,                // Запись значения канала управления
   IRIDIUM_OPERATION_READ_STREAM,                  // Чтение потока
   IRIDIUM_OPERATION_WRITE_STREAM,                 // Запись потока
};

// Идентификаторы источника ошибки
enum eIridiumErrorSource
{
   IRIDIUM_ERROR_PROCESSING = 0,                   // Ошибка произошла во время обработки пакета
   IRIDIUM_ERROR_RECEIVED,                         // Ошибка была получена от удаленного устройства
};

// Идентификаторы кодов ошибок
enum eIridiumError
{
   IRIDIUM_OK = 0,                                 // Нет ошибки
   IRIDIUM_UNKNOWN_ERROR,                          // Неизвестная ошибка
   IRIDIUM_AUTHORIZATION_ERROR,                    // Ошибка авторизации
   IRIDIUM_SERVER_IS_FULL,                         // На сервере нет свободного места
   IRIDIUM_UNKNOWN_MESSAGE,                        // Неизвестное сообщение
   IRIDIUM_UNKNOWN_MESSAGE_VERSION,                // Неизвестный номер версии сообщения
   IRIDIUM_PROTOCOL_CORRUPT,                       // Повреждение протокола
   IRIDIUM_ERROR_BAD_TAG_ID,                       // Неверный идентификатор канала обратной связи
   IRIDIUM_ERROR_BAD_CHANNEL_ID,                   // Неверный идентификатор канала управления
   IRIDIUM_BAD_TREND_PARAMETER,                    // Неверный параметр тренда
   IRIDIUM_BAD_PIN,                                // Пароль доступа не подходит
   IRIDIUM_PIN_LOCK,                               // Проверка пароля заблокирована
};

// Причина по которой устройство отправляет поисковый ответ
enum eIridiumSearchEvent
{
   IRIDIUM_SEARCH_EVENT_START = 0,                 // Сообщение при старте устройства
   IRIDIUM_SEARCH_EVENT_ID,                        // Сообщение при идентификации устройства
   IRIDIUM_SEARCH_EVENT_REQUEST,                   // Сообщение по запросу
};

// Определение размера адреса
#if defined(IRIDIUM_ENABLE_LONG_ADDRESS)
typedef u32 iridium_address_t;
#else
typedef u16 iridium_address_t;
#endif   // defined(IRIDIUM_ENABLE_LONG_ADDRESS)

// Структура для хранения флагов пакета
typedef struct iridium_packet_flags_s
{
   unsigned m_bPriority    :  1;                   // Флаг приоритета
   unsigned m_bSrcSeg      :  1;                   // Флаг налияия данных о сегменте источника
   unsigned m_bDstSeg      :  1;                   // Флаг налияия данных о сегменте приемника
   unsigned m_bAddress     :  1;                   // Флаг наличия данных адреса приемника
   unsigned m_u2Version    :  2;                   // Версия пакета
   unsigned m_u3Crypt      :  3;                   // Применяемый метод шифрования
} iridium_packet_flags_t;

// Структура заголовка пакета
typedef struct iridium_packet_header_s
{
   u8                      m_u8Type;               // Тип протокола
   iridium_packet_flags_t  m_Flags;                // Список флагов
   iridium_address_t       m_SrcAddr;              // Адрес источника
                                                   // для tcp идентификатор 32 бита
                                                   // для шины идентификатор 16 бит
   iridium_address_t       m_DstAddr;              // Адрес приемника
                                                   // для tcp идентификатор 32 бита
                                                   // для шины идентификатор 16 бит
   u8                      m_u8Route;              // Маршрут откуда пришел пакет
} iridium_packet_header_t;

// Структура для хранения флагов сообщения
typedef struct iridium_message_flags_s
{
   unsigned m_bDirection   :  1;                   // Направление сообщения
   unsigned m_bError       :  1;                   // Флаг наличия ошибки
   unsigned m_bNoTID       :  1;                   // Флаг указывающий что нет идентификатора транзакции
   unsigned m_bEnd         :  1;                   // Флаг указывающий конец цепочки пакетов
   unsigned m_u4Version    :  4;                   // Версия сообщения (пока везде используется 1 версия)
} iridium_message_flags_t;

// Структура для хранения заголовка сообщения
typedef struct iridium_message_header_s
{
   iridium_message_flags_t m_Flags;                // Список флагов сообщения
   u8                      m_u8Type;               // Идентификатор типа пакета
   u16                     m_u16TID;               // Идентификатор транзакции
} iridium_message_header_t;

// Структура с информацией о найденном устройстве
typedef struct iridium_search_info_s
{
   u8    m_u8Group;                                // Группа клиента
   char* m_pszHWID;                                // Указатель на HWID устройства
   u8    m_u8Event;                                // Причина по которой получены поисковые данные
   u8    m_u8Flags;                                // Список флагов
   u32   m_u32UserID;                              // Пользовательский идентификатор
   u16   m_u16Change;                              // Счетчик изменений

#ifdef IRIDIUM_MCU_AVR
   // Структра для хранения типа памяти в которой хранятся данные
   // false - данные в оперативной памяти
   // true  - данные в flash памяти
   struct                                          
   {
      unsigned m_bHWID;                            // Определение в каком типе памяти расположено HWID устройства
   } Mem;
#endif
} iridium_search_info_t;

// Структура с информацией об устройстве
typedef struct iridium_device_info_s
{
   u8    m_u8Group;                                // Группа клиента
   char* m_pszName;                                // Указатель на имя определенное пользователем
   char* m_pszProducer;                            // Указатель на имя производителя
   char* m_pszModel;                               // Указатель на модель клиента
   char* m_pszHWID;                                // Указатель на HWID устройства
   u8    m_u8Class;                                // Класс устройства
   u8    m_u8Processor;                            // Тип процессора
   u8    m_u8OS;                                   // Тип операционной системы
   u8    m_u8Flags;                                // Список флагов устройства
   u16   m_u16FirmwareID;                          // Профиль (идентификатор прошивки) согласно классификации iRidium API
   u8    m_aVersion[2];                            // Версия прошивки
   u32   m_u32Channels;                            // Количество каналов управления на устройстве
   u32   m_u32Tags;                                // Количество каналов обратной связи
   u32   m_u32UserID;                              // Определенный пользователем идентификатор
   u16   m_u16Change;                              // Версия изменения

#ifdef IRIDIUM_MCU_AVR
   // Структра для хранения типа памяти в которой хранятся данные
   // false - данные в оперативной памяти
   // true  - данные в flash памяти
   struct  
   {
     unsigned  m_bName  :  1;                      // Тип памяти где расположено имя устройства
     unsigned  m_bOS    :  1;                      // Тип памяти где расположено имя операционной системы
     unsigned  m_bModel :  1;                      // Тип памяти где расположено имя модели устройства
     unsigned  m_bHWID  :  1;                      // Тип памяти где расположено HWID устройства
   } Mem;
#endif
} iridium_device_info_t;

// Структура описания общих параметров канала
typedef struct iridium_description_s
{
   u8                m_u8Type;                     // Тип значения
   universal_value_t m_Min;                        // Минимальное значение (для типов IVT_BOOL, IVT_STRING и IVT_ARRAY_U8 не используется)
   universal_value_t m_Max;                        // Максимальное значение (для типов IVT_BOOL, IVT_STRING и IVT_ARRAY_U8 не используется)
   universal_value_t m_Step;                       // Шаг значения (для типов IVT_BOOL, IVT_STRING и IVT_ARRAY_U8 не используется)
   char*             m_pszDescription;             // Текстовое описание канала обратной связи
   u16               m_u16Units;                   // Единицы измерения канала
   
#ifdef IRIDIUM_MCU_AVR
   // Структра для хранения типа памяти в которой хранятся данные
   // false - данные в оперативной памяти
   // true  - данные в flash памяти
   struct
   {
      unsigned m_bDesc;                            // Тип памяти где расположено описание устройства
   } Mem;
#endif
} iridium_description_t;

// Структура данных канала обратной связи
typedef struct iridium_data_s
{
   u32               m_u32ID;                      // Идентификатор канала обратной связи
   char*             m_pszName;                    // Указатель на имя канала
   u8                m_u8Type;                     // Тип значения
   universal_value_t m_Value;                      // Данные значения
   u16               m_u16SubdeviceID;             // Идентификатор типа подустройства
   u8                m_u8FunctionID;               // Идентификатор функции канала
   u8                m_u8GroupID;                  // Группа к которой принадлежит канал

#ifdef IRIDIUM_MCU_AVR
   // Структра для хранения типа памяти в которой хранятся данные
   // false - данные в оперативной памяти
   // true  - данные в flash памяти
   struct
   {
      unsigned m_bName;                            // Тип памяти где расположено имя канала обратной связи
   } Mem;
#endif
} iridium_data_t;

// Структура данных канала обратной связи
typedef iridium_data_t iridium_tag_info_t;

// Структура описания списка флагов канала обратной связи
typedef struct itd_flags_s
{
   unsigned m_bOwner          :  1;                // Флаг владения глобальной переменной
} itd_flags_t;

// Структура описания канала обратной связи
typedef struct iridium_tag_description_s
{
   iridium_description_t   m_ID;                   // Общее описание канала обратной связи
   itd_flags_t             m_Flags;                // Список флагов
   u16                     m_u16Variable;          // Идентификатор глобальной переменной связанной с каналом обратной связи
} iridium_tag_description_t;

// Структура данных канала управления
typedef iridium_data_t iridium_channel_info_t;

// Структура описания списка флагов канала управления
typedef struct icd_flags_s
{
   unsigned m_bWritePassword  :  1;                // Флаг указывающий что для изменения значения нужен пароль
   unsigned m_bReadPassword   :  1;                // Флаг указывающий что для чтения значения нужен пароль
} icd_flags_t;

// Структура описания канала управления
typedef struct iridium_channel_description_s
{
   iridium_description_t   m_ID;                   // Общее описание канала управления
   icd_flags_t             m_Flags;                // Список флагов
   u8                      m_u8MaxVariables;       // Максимальное количество переменных на канал
   u8                      m_u8Variables;          // Количество идентификаторов глобальных переменных связанных с каналом управления
   u16*                    m_pVariables;           // Указатель на массив идентификаторов глобальных переменных связанных с каналом управления
} iridium_channel_description_t;

// Структура для хранения параметров действия сценария
typedef struct iridium_scenario_action_s
{
   u32               m_u32ID;                      // Идентификатор канала
   u8                m_u8Type;                     // Тип значения
   universal_value_t m_Value;                      // Универсальное значение
} iridium_scenario_action_t;

// Получение версии сообщения
u8 GetMessageVersion(u8 in_u8Type);

#endif   // _IRIDIUM_H_INCLUDED_

