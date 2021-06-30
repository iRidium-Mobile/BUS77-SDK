//////////////////////////////////////////////////////////////////////////
// class CDevice
//////////////////////////////////////////////////////////////////////////
// Включения
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "Main.h"
#include "stdlib.h"

// Common
#include "NVRAMCommon.h"
#include "UTF8.h"

// MCUCommon
#include "CommonCAN.h"
#include "CI2CNVRAM8Bit.h"
#include "CI2CNVRAM16Bit.h"
#include "CFlashNVRAM.h"
#include "InputOutput.h"

// DeviceCommon
#include "DeviceDefine.h"
#include "FLASHMemoryMap.h"
#include "CDeviceIndicator.h"

// iRidiumDevice
#include "CDevice.h"

// iRidium protocol
#include "IridiumBytes.h"
#include "IridiumCRC16.h"
#include "CIridiumStreebog.h"
#include "IridiumChannels.h"
#include "IridiumUnits.h"


#define MAX_VARIABLES                  16          // Максимальное количество глобальных переменных на канал управления

#define FIRMWARE_READ_STREAM_ID        1           // Идентификатор потока чтения прошивки
#define FIRMWARE_WRITE_STREAM_ID       2           // Идентификатор потока записи прошивки

#define MAX_INPUTS                     0           // Максимальное количество дискретных каналов

// Флаги для канала управления
#define CF_W   0x40                                // Признак изменения значения канала по PIN коду
#define CF_R   0x20                                // Признак чтения значения канала по PIN коду

// Таблица значений (минимальное, максимальное и шаг)
const bool  g_aRangeBool[]          =  { false,    true,       true  };
const u8    g_aRangeU8[]            =  { 0,        0xFF,       1     };
//const u16   g_aRangeU16[]           =  { 0,        0xFFFF,     1     };
const u32   g_aRangeU32[]           =  { 0,        0xFFFFFFFF, 1     };
const u32   g_aRangeName[]          =  { 0,        32,         1     };

///////////////////////////////////////////////////////////////////////////////
// Информация о каналах управления
///////////////////////////////////////////////////////////////////////////////
// Имена каналов управления
const char g_pszChannelName[]             = "Имя устройства";
const char g_pszChannelPIN[]              = "PIN код";
const char g_pszChannelUserID[]           = "Пользовательский идентификатор";
const char g_pszChannelReboot[]           = "Перезагрузка устройства";
const char g_pszChannelDimmerSet[]        = "Значение канала %d";
const char g_pszChannelDimmerOn[]         = "Включение канала %d";
const char g_pszChannelDimmerOff[]        = "Выключение канала %d";
const char g_pszChannelDimmerUp[]         = "Увеличение значения канала %d";
const char g_pszChannelDimmerDown[]       = "Уменьшение значения канала %d";
const char g_pszChannelDimmerRecallMin[]  = "Включение минимальное значение канала %d";
const char g_pszChannelDimmerRecallMax[]  = "Включение максимальное значение канала %d";
const char g_pszChannelDimmerThreshold[]  = "Значение порога мерцания канала %d";
const char g_pszChannelDimmerMin[]        = "Минимальное значение канала %d";
const char g_pszChannelDimmerMax[]        = "Максимальное значение канала %d";
const char g_pszChannelDimmerPowerOn[]    = "Значение канала %d при включении";
const char g_pszChannelDimmerError[]      = "Значение канала %d в случае ошибки";
const char g_pszChannelDimmerFadeIn[]     = "Время увеличения значения канала %d";
const char g_pszChannelDimmerFadeOut[]    = "Время уменьшения значения канала %d";

// Текстовое описание каналов обратной связи
const char g_pszChannelDescriptionName[]  = "32 байта(16 сим. латин(кирил)ицей)";
const char g_pszChannelDescriptionPIN[]   = "Персональное идентификационное число";
const char g_pszChannelTest[]             = "Вписать";

enum eChannelComm
{
   CC_SET = 0,                                     // Канал установки значения диммера
   CC_ON,                                          // Канал включения диммера
   CC_OFF,                                         // Канал выключения диммера
   CC_UP,                                          // Канал увеличения уровня диммера
   CC_DOWN,                                        // Канал уменьшения уровня диммера
   CC_RECALL_MIN,                                  // Вызов минимального значения
   CC_RECALL_MAX,                                  // Вызов максимального значения
   CC_THRESHOLD,                                   // Порог мерцания
   CC_MIN,                                         // Канал минимального значения диммера
   CC_MAX,                                         // Канал максимального значения диммера
   CC_POWER_ON,                                    // Значение диммера при включении
   CC_ERROR,                                       // Значение диммера при ошибке
   CC_FADE_IN_TIME,                                // Значения частоты обновления значения диммера
   CC_FADE_OUT_TIME,                               // Время изменения диммера
   CC_SIZEOF                                       // Максимальное количество 
};

// Идентификаторы каналов управления
enum eChannels
{
   CHANNEL_1_BASE = CC_SIZEOF * 0 + 1,             // Смещение на базу канала 1
   CHANNEL_2_BASE = CC_SIZEOF * 1 + 1,             // Смещение на базу канала 2
   CHANNEL_3_BASE = CC_SIZEOF * 2 + 1,             // Смещение на базу канала 3
   CHANNEL_4_BASE = CC_SIZEOF * 3 + 1,             // Смещение на базу канала 4
   CHANNEL_5_BASE = CC_SIZEOF * 4 + 1,             // Смещение на базу канала 5
   CHANNEL_6_BASE = CC_SIZEOF * 5 + 1,             // Смещение на базу канала 6
};

// Данные канала управления
typedef struct device_channel_s
{
   u32         m_u32ID;                            // Идентификатор канала
   const char* m_pszName;                          // Указатель на имя канала
   u8          m_u8Type;                           // Тип значения канала обратной связи
   const void* m_pRange;                           // Указатель на минимаьное, максимальное и шаговое значение
   const char* m_pszDesc;                          // указатель на описание канала
   u8          m_u8Flags;                          // Список флагов
   u16         m_u16SubdeviceID;                   // Идентификатор типа подустройства
   u8          m_u8FunctionID;                     // Идентификатор функции канала
   u8          m_u8GroupID;                        // Группа к которой принадлежит канал
   u16         m_u16Units;                         // Единицы измерения
   u8          m_u8MaxVariables;                   // Максимальное количество глобальных переменных на канал управления
   u8          m_u8VarIndex;                       // Индекс списка переменных
} device_channel_t;


// Массив с описанием канала управления
const device_channel_t g_aDeviceChannels[] =
{
   {  SYSTEM_CHANNEL_REBOOT,              (char*)g_pszChannelReboot,          IVT_BOOL,      g_aRangeBool,  (char*)g_pszChannelReboot,          CF_W,          0, 1, 0, 0,                            0,             0  },
   {  SYSTEM_CHANNEL_NAME,                (char*)g_pszChannelName,            IVT_STRING8,   g_aRangeName,  (char*)g_pszChannelDescriptionName, CF_W,          0, 1, 0, 0,                            0,             0  },
   {  SYSTEM_CHANNEL_PIN,                 (char*)g_pszChannelPIN,             IVT_U32,       g_aRangeU32,   (char*)g_pszChannelDescriptionPIN,  CF_W | CF_R,   0, 1, 0, 0,                            0,             0  },
   {  SYSTEM_CHANNEL_USER_ID,             (char*)g_pszChannelUserID,          IVT_U32,       g_aRangeU32,   (char*)NULL,                        CF_W,          0, 1, 0, 0,                            0,             0  },

   {  CHANNEL_1_BASE + CC_SET,            (char*)g_pszChannelDimmerSet,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            MAX_VARIABLES, 0  },
   {  CHANNEL_1_BASE + CC_ON,             (char*)g_pszChannelDimmerOn,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            MAX_VARIABLES, 1  },
   {  CHANNEL_1_BASE + CC_OFF,            (char*)g_pszChannelDimmerOff,       IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            MAX_VARIABLES, 2  },
   {  CHANNEL_1_BASE + CC_UP,             (char*)g_pszChannelDimmerUp,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            MAX_VARIABLES, 3  },
   {  CHANNEL_1_BASE + CC_DOWN,           (char*)g_pszChannelDimmerDown,      IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            MAX_VARIABLES, 4  },
   {  CHANNEL_1_BASE + CC_RECALL_MIN,     (char*)g_pszChannelDimmerRecallMin, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            MAX_VARIABLES, 5  },
   {  CHANNEL_1_BASE + CC_RECALL_MAX,     (char*)g_pszChannelDimmerRecallMax, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            MAX_VARIABLES, 6  },
   {  CHANNEL_1_BASE + CC_THRESHOLD,      (char*)g_pszChannelDimmerThreshold, IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            0,             0  },
   {  CHANNEL_1_BASE + CC_MIN,            (char*)g_pszChannelDimmerMin,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            0,             0  },
   {  CHANNEL_1_BASE + CC_MAX,            (char*)g_pszChannelDimmerMax,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            0,             0  },
   {  CHANNEL_1_BASE + CC_POWER_ON,       (char*)g_pszChannelDimmerPowerOn,   IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            0,             0  },
   {  CHANNEL_1_BASE + CC_ERROR,          (char*)g_pszChannelDimmerError,     IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 1, 0,                            0,             0  },
   {  CHANNEL_1_BASE + CC_FADE_IN_TIME,   (char*)g_pszChannelDimmerFadeIn,    IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 1, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },
   {  CHANNEL_1_BASE + CC_FADE_OUT_TIME,  (char*)g_pszChannelDimmerFadeOut,   IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 1, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },

   {  CHANNEL_2_BASE + CC_SET,            (char*)g_pszChannelDimmerSet,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            MAX_VARIABLES, 7  },
   {  CHANNEL_2_BASE + CC_ON,             (char*)g_pszChannelDimmerOn,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            MAX_VARIABLES, 8  },
   {  CHANNEL_2_BASE + CC_OFF,            (char*)g_pszChannelDimmerOff,       IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            MAX_VARIABLES, 9  },
   {  CHANNEL_2_BASE + CC_UP,             (char*)g_pszChannelDimmerUp,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            MAX_VARIABLES, 10 },
   {  CHANNEL_2_BASE + CC_DOWN,           (char*)g_pszChannelDimmerDown,      IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            MAX_VARIABLES, 11 },
   {  CHANNEL_2_BASE + CC_RECALL_MIN,     (char*)g_pszChannelDimmerRecallMin, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            MAX_VARIABLES, 12 },
   {  CHANNEL_2_BASE + CC_RECALL_MAX,     (char*)g_pszChannelDimmerRecallMax, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            MAX_VARIABLES, 13 },
   {  CHANNEL_2_BASE + CC_THRESHOLD,      (char*)g_pszChannelDimmerThreshold, IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            0,             0  },
   {  CHANNEL_2_BASE + CC_MIN,            (char*)g_pszChannelDimmerMin,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            0,             0  },
   {  CHANNEL_2_BASE + CC_MAX,            (char*)g_pszChannelDimmerMax,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            0,             0  },
   {  CHANNEL_2_BASE + CC_POWER_ON,       (char*)g_pszChannelDimmerPowerOn,   IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            0,             0  },
   {  CHANNEL_2_BASE + CC_ERROR,          (char*)g_pszChannelDimmerError,     IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 2, 0,                            0,             0  },
   {  CHANNEL_2_BASE + CC_FADE_IN_TIME,   (char*)g_pszChannelDimmerFadeIn,    IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 2, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },
   {  CHANNEL_2_BASE + CC_FADE_OUT_TIME,  (char*)g_pszChannelDimmerFadeOut,   IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 2, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },

   {  CHANNEL_3_BASE + CC_SET,            (char*)g_pszChannelDimmerSet,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            MAX_VARIABLES, 14 },
   {  CHANNEL_3_BASE + CC_ON,             (char*)g_pszChannelDimmerOn,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            MAX_VARIABLES, 15 },
   {  CHANNEL_3_BASE + CC_OFF,            (char*)g_pszChannelDimmerOff,       IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            MAX_VARIABLES, 16 },
   {  CHANNEL_3_BASE + CC_UP,             (char*)g_pszChannelDimmerUp,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            MAX_VARIABLES, 17 },
   {  CHANNEL_3_BASE + CC_DOWN,           (char*)g_pszChannelDimmerDown,      IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            MAX_VARIABLES, 18 },
   {  CHANNEL_3_BASE + CC_RECALL_MIN,     (char*)g_pszChannelDimmerRecallMin, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            MAX_VARIABLES, 19 },
   {  CHANNEL_3_BASE + CC_RECALL_MAX,     (char*)g_pszChannelDimmerRecallMax, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            MAX_VARIABLES, 20 },
   {  CHANNEL_3_BASE + CC_THRESHOLD,      (char*)g_pszChannelDimmerThreshold, IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            0,             0  },
   {  CHANNEL_3_BASE + CC_MIN,            (char*)g_pszChannelDimmerMin,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            0,             0  },
   {  CHANNEL_3_BASE + CC_MAX,            (char*)g_pszChannelDimmerMax,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            0,             0  },
   {  CHANNEL_3_BASE + CC_POWER_ON,       (char*)g_pszChannelDimmerPowerOn,   IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            0,             0  },
   {  CHANNEL_3_BASE + CC_ERROR,          (char*)g_pszChannelDimmerError,     IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 3, 0,                            0,             0  },
   {  CHANNEL_3_BASE + CC_FADE_IN_TIME,   (char*)g_pszChannelDimmerFadeIn,    IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 3, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },
   {  CHANNEL_3_BASE + CC_FADE_OUT_TIME,  (char*)g_pszChannelDimmerFadeOut,   IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 3, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },

   {  CHANNEL_4_BASE + CC_SET,            (char*)g_pszChannelDimmerSet,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            MAX_VARIABLES, 21 },
   {  CHANNEL_4_BASE + CC_ON,             (char*)g_pszChannelDimmerOn,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            MAX_VARIABLES, 22 },
   {  CHANNEL_4_BASE + CC_OFF,            (char*)g_pszChannelDimmerOff,       IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            MAX_VARIABLES, 23 },
   {  CHANNEL_4_BASE + CC_UP,             (char*)g_pszChannelDimmerUp,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            MAX_VARIABLES, 24 },
   {  CHANNEL_4_BASE + CC_DOWN,           (char*)g_pszChannelDimmerDown,      IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            MAX_VARIABLES, 25 },
   {  CHANNEL_4_BASE + CC_RECALL_MIN,     (char*)g_pszChannelDimmerRecallMin, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            MAX_VARIABLES, 26 },
   {  CHANNEL_4_BASE + CC_RECALL_MAX,     (char*)g_pszChannelDimmerRecallMax, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            MAX_VARIABLES, 27 },
   {  CHANNEL_4_BASE + CC_THRESHOLD,      (char*)g_pszChannelDimmerThreshold, IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            0,             0  },
   {  CHANNEL_4_BASE + CC_MIN,            (char*)g_pszChannelDimmerMin,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            0,             0  },
   {  CHANNEL_4_BASE + CC_MAX,            (char*)g_pszChannelDimmerMax,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            0,             0  },
   {  CHANNEL_4_BASE + CC_POWER_ON,       (char*)g_pszChannelDimmerPowerOn,   IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            0,             0  },
   {  CHANNEL_4_BASE + CC_ERROR,          (char*)g_pszChannelDimmerError,     IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 4, 0,                            0,             0  },
   {  CHANNEL_4_BASE + CC_FADE_IN_TIME,   (char*)g_pszChannelDimmerFadeIn,    IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 4, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },
   {  CHANNEL_4_BASE + CC_FADE_OUT_TIME,  (char*)g_pszChannelDimmerFadeOut,   IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 4, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },

   {  CHANNEL_5_BASE + CC_SET,            (char*)g_pszChannelDimmerSet,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            MAX_VARIABLES, 28 },
   {  CHANNEL_5_BASE + CC_ON,             (char*)g_pszChannelDimmerOn,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            MAX_VARIABLES, 29 },
   {  CHANNEL_5_BASE + CC_OFF,            (char*)g_pszChannelDimmerOff,       IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            MAX_VARIABLES, 30 },
   {  CHANNEL_5_BASE + CC_UP,             (char*)g_pszChannelDimmerUp,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            MAX_VARIABLES, 31 },
   {  CHANNEL_5_BASE + CC_DOWN,           (char*)g_pszChannelDimmerDown,      IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            MAX_VARIABLES, 32 },
   {  CHANNEL_5_BASE + CC_RECALL_MIN,     (char*)g_pszChannelDimmerRecallMin, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            MAX_VARIABLES, 33 },
   {  CHANNEL_5_BASE + CC_RECALL_MAX,     (char*)g_pszChannelDimmerRecallMax, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            MAX_VARIABLES, 34 },
   {  CHANNEL_5_BASE + CC_THRESHOLD,      (char*)g_pszChannelDimmerThreshold, IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            0,             0  },
   {  CHANNEL_5_BASE + CC_MIN,            (char*)g_pszChannelDimmerMin,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            0,             0  },
   {  CHANNEL_5_BASE + CC_MAX,            (char*)g_pszChannelDimmerMax,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            0,             0  },
   {  CHANNEL_5_BASE + CC_POWER_ON,       (char*)g_pszChannelDimmerPowerOn,   IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            0,             0  },
   {  CHANNEL_5_BASE + CC_ERROR,          (char*)g_pszChannelDimmerError,     IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 5, 0,                            0,             0  },
   {  CHANNEL_5_BASE + CC_FADE_IN_TIME,   (char*)g_pszChannelDimmerFadeIn,    IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 5, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },
   {  CHANNEL_5_BASE + CC_FADE_OUT_TIME,  (char*)g_pszChannelDimmerFadeOut,   IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 5, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },

   {  CHANNEL_6_BASE + CC_SET,            (char*)g_pszChannelDimmerSet,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            MAX_VARIABLES, 35 },
   {  CHANNEL_6_BASE + CC_ON,             (char*)g_pszChannelDimmerOn,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            MAX_VARIABLES, 36 },
   {  CHANNEL_6_BASE + CC_OFF,            (char*)g_pszChannelDimmerOff,       IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            MAX_VARIABLES, 37 },
   {  CHANNEL_6_BASE + CC_UP,             (char*)g_pszChannelDimmerUp,        IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            MAX_VARIABLES, 38 },
   {  CHANNEL_6_BASE + CC_DOWN,           (char*)g_pszChannelDimmerDown,      IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            MAX_VARIABLES, 39 },
   {  CHANNEL_6_BASE + CC_RECALL_MIN,     (char*)g_pszChannelDimmerRecallMin, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            MAX_VARIABLES, 40 },
   {  CHANNEL_6_BASE + CC_RECALL_MAX,     (char*)g_pszChannelDimmerRecallMax, IVT_NONE,      NULL,          (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            MAX_VARIABLES, 41 },
   {  CHANNEL_6_BASE + CC_THRESHOLD,      (char*)g_pszChannelDimmerThreshold, IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            0,             0  },
   {  CHANNEL_6_BASE + CC_MIN,            (char*)g_pszChannelDimmerMin,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            0,             0  },
   {  CHANNEL_6_BASE + CC_MAX,            (char*)g_pszChannelDimmerMax,       IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            0,             0  },
   {  CHANNEL_6_BASE + CC_POWER_ON,       (char*)g_pszChannelDimmerPowerOn,   IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            0,             0  },
   {  CHANNEL_6_BASE + CC_ERROR,          (char*)g_pszChannelDimmerError,     IVT_U8,        g_aRangeU8,    (char*)g_pszChannelTest,            CF_W,          0, 1, 6, 0,                            0,             0  },
   {  CHANNEL_6_BASE + CC_FADE_IN_TIME,   (char*)g_pszChannelDimmerFadeIn,    IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 6, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },
   {  CHANNEL_6_BASE + CC_FADE_OUT_TIME,  (char*)g_pszChannelDimmerFadeOut,   IVT_U32,       g_aRangeU32,   (char*)g_pszChannelTest,            CF_W,          0, 1, 6, IRIDIUM_UNITS_MILLISECONDS,   0,             0  },
};

///////////////////////////////////////////////////////////////////////////////
// Информация о каналах обратной связи
///////////////////////////////////////////////////////////////////////////////
// Имена каналов обратной связи
const char g_pszTagError[]          = "Код ошибки";
const char g_pszTagDimmerValue[]    = "Значение канала %d";
const char g_pszTagDimmerOn[]       = "Канал %d включен";
const char g_pszTagDimmerOff[]      = "Канал %d выключен";
const char g_pszTagDimmerMin[]      = "Канал %d достиг минимального значения";
const char g_pszTagDimmerMax[]      = "Канал %d достиг максимального значения";
const char g_pszTagDimmerStatus[]   = "Соединение канала %d";

// Текстовое описание каналов обратной связи
const char g_pszTagDescriptionError[]  = "Код ошибки";
const char g_pszTagDescriptionTest[]   = "Тестовые каналы";

enum eTagComm
{
   TC_VALUE = 0,                                   // Текущее значение
   TC_ON,                                          // Канал был включен
   TC_OFF,                                         // Канал был выключен
   TC_MIN,                                         // Достигнуто минамальное значение
   TC_MAX,                                         // Достигнуто максимальное значение
   TC_STATUS,                                      // Состояние канала
   TC_SIZEOF
};

// Идентификаторы каналов обратной связи
enum eTags
{
   TAG_ERROR = 1,                                  // Код ошибки
   TAG_1_BASE,                                     // Тестовый канал
   TAG_2_BASE = TAG_1_BASE + TC_SIZEOF,            // Тестовый канал
   TAG_3_BASE = TAG_2_BASE + TC_SIZEOF,            // Тестовый канал
   TAG_4_BASE = TAG_3_BASE + TC_SIZEOF,            // Тестовый канал
   TAG_5_BASE = TAG_4_BASE + TC_SIZEOF,            // Тестовый канал
   TAG_6_BASE = TAG_5_BASE + TC_SIZEOF,            // Тестовый канал
};

// Данные канала обратной связи
typedef struct device_tag_s
{
   u32         m_u32ID;                            // Идентификатор канала
   const char* m_pszName;                          // Указатель на имя канала
   u8          m_u8Type;                           // Тип значения канала обратной связи
   const void* m_pRange;                           // Указатель на минимаьное, максимальное и шаговое значение
   const char* m_pszDesc;                          // Указатель на описание канала
   u16         m_u16SubdeviceID;                   // Идентификатор типа подустройства
   u8          m_u8FunctionID;                     // Идентификатор функции канала
   u8          m_u8GroupID;                        // Группа к которой принадлежит канал
   u16         m_u16Units;                         // Единицы измерения
} device_tag_t;

// Массив с описанием канала управления
const device_tag_t g_aDeviceTags[] =
{
   {  TAG_ERROR,              (char*)g_pszTagError,         IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionError, 0, 0, 0, 0  },

   {  TAG_1_BASE + TC_VALUE,  (char*)g_pszTagDimmerValue,   IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 1, 0  },
   {  TAG_1_BASE + TC_ON,     (char*)g_pszTagDimmerOn,      IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 1, 0  },
   {  TAG_1_BASE + TC_OFF,    (char*)g_pszTagDimmerOff,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 1, 0  },
   {  TAG_1_BASE + TC_MIN,    (char*)g_pszTagDimmerMin,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 1, 0  },
   {  TAG_1_BASE + TC_MAX,    (char*)g_pszTagDimmerMax,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 1, 0  },
   {  TAG_1_BASE + TC_STATUS, (char*)g_pszTagDimmerStatus,  IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 1, 0  },

   {  TAG_2_BASE + TC_VALUE,  (char*)g_pszTagDimmerValue,   IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 2, 0  },
   {  TAG_2_BASE + TC_ON,     (char*)g_pszTagDimmerOn,      IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 2, 0  },
   {  TAG_2_BASE + TC_OFF,    (char*)g_pszTagDimmerOff,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 2, 0  },
   {  TAG_2_BASE + TC_MIN,    (char*)g_pszTagDimmerMin,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 2, 0  },
   {  TAG_2_BASE + TC_MAX,    (char*)g_pszTagDimmerMax,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 2, 0  },
   {  TAG_2_BASE + TC_STATUS, (char*)g_pszTagDimmerStatus,  IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 2, 0  },

   {  TAG_3_BASE + TC_VALUE,  (char*)g_pszTagDimmerValue,   IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 3, 0  },
   {  TAG_3_BASE + TC_ON,     (char*)g_pszTagDimmerOn,      IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 3, 0  },
   {  TAG_3_BASE + TC_OFF,    (char*)g_pszTagDimmerOff,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 3, 0  },
   {  TAG_3_BASE + TC_MIN,    (char*)g_pszTagDimmerMin,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 3, 0  },
   {  TAG_3_BASE + TC_MAX,    (char*)g_pszTagDimmerMax,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 3, 0  },
   {  TAG_3_BASE + TC_STATUS, (char*)g_pszTagDimmerStatus,  IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 3, 0  },

   {  TAG_4_BASE + TC_VALUE,  (char*)g_pszTagDimmerValue,   IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 4, 0  },
   {  TAG_4_BASE + TC_ON,     (char*)g_pszTagDimmerOn,      IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 4, 0  },
   {  TAG_4_BASE + TC_OFF,    (char*)g_pszTagDimmerOff,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 4, 0  },
   {  TAG_4_BASE + TC_MIN,    (char*)g_pszTagDimmerMin,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 4, 0  },
   {  TAG_4_BASE + TC_MAX,    (char*)g_pszTagDimmerMax,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 4, 0  },
   {  TAG_4_BASE + TC_STATUS, (char*)g_pszTagDimmerStatus,  IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 4, 0  },

   {  TAG_5_BASE + TC_VALUE,  (char*)g_pszTagDimmerValue,   IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 5, 0  },
   {  TAG_5_BASE + TC_ON,     (char*)g_pszTagDimmerOn,      IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 5, 0  },
   {  TAG_5_BASE + TC_OFF,    (char*)g_pszTagDimmerOff,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 5, 0  },
   {  TAG_5_BASE + TC_MIN,    (char*)g_pszTagDimmerMin,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 5, 0  },
   {  TAG_5_BASE + TC_MAX,    (char*)g_pszTagDimmerMax,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 5, 0  },
   {  TAG_5_BASE + TC_STATUS, (char*)g_pszTagDimmerStatus,  IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 5, 0  },

   {  TAG_6_BASE + TC_VALUE,  (char*)g_pszTagDimmerValue,   IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 6, 0  },
   {  TAG_6_BASE + TC_ON,     (char*)g_pszTagDimmerOn,      IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 6, 0  },
   {  TAG_6_BASE + TC_OFF,    (char*)g_pszTagDimmerOff,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 6, 0  },
   {  TAG_6_BASE + TC_MIN,    (char*)g_pszTagDimmerMin,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 6, 0  },
   {  TAG_6_BASE + TC_MAX,    (char*)g_pszTagDimmerMax,     IVT_NONE,   NULL,       (char*)g_pszTagDescriptionTest,  0, 0, 6, 0  },
   {  TAG_6_BASE + TC_STATUS, (char*)g_pszTagDimmerStatus,  IVT_U8,     g_aRangeU8, (char*)g_pszTagDescriptionTest,  0, 0, 6, 0  },
};

/////////////////////////////////////////////////////////////////////////////////////
// Энергонезависимая память // переменные, которые нужно сохранить, дабавляем сюда //
/////////////////////////////////////////////////////////////////////////////////////
typedef struct nvram_device_s
{
   // Обшая структура данных (для загрузчика и прошивки)
   nvram_common_t    m_Common;
   // Список битов признаков владения переменными
   u8                m_aOwners[((sizeof(g_aDeviceTags) / sizeof(g_aDeviceTags[0])) + 7) / 8];
   // Массив глобальных переменных связанных с каналами управления
   u16               m_aChannelVariables[42][MAX_VARIABLES];
   // Массив глобальных переменных связанных с каналами обратной связи
   u16               m_aTagVariables[sizeof(g_aDeviceTags) / sizeof(g_aDeviceTags[0])];

   u8                m_aLastValue[6];              // Последнее значение канала
   u8                m_aThreshold[6];              // Порог мерцания
   u8                m_aMin[6];                    // Минимальное значение канала
   u8                m_aMax[6];                    // Максимальное значение канала
   u8                m_aPower[6];                  // Значение канала при старте
   u8                m_aError[6];                  // Значение канала при наличии ошибки
   u32               m_aInTime[6];                 // Время уменьшения значения
   u32               m_aOutTime[6];                // Время увеличения значения

} nvram_device_t;

// Данные энергонезависимой памяти устройства
nvram_device_t   g_NVRAMData;

///////////////////////////////////////////////////////////////////////////////
// Информация об устройстве
///////////////////////////////////////////////////////////////////////////////
const char g_pszProducer[]    = "iRidium";
const char g_pszModelName[]   = "iRidium firmware";

char g_pszHWID[STREEBOG_HASH_256_BYTES + 1];       // Аппаратный идентификатор шеснадцатеричное представление 128 битного числа + 1 байт

char g_szTemp[128];                                // Промежуточный буфер

// Информация об устройстве
const iridium_device_info_t g_DeviceInfo =
{
   IRIDIUM_GROUP_TYPE_ACTUATOR,
   g_NVRAMData.m_Common.m_szDeviceName,
   (char*)g_pszProducer,
   (char*)g_pszModelName,
   (char*)g_pszHWID,
   DCT_MICROCONTROLLER,
   PT_ARM,
   OST_NONE,
   IRIDIUM_DEVICE_FLAG_FIRMWARE | IRIDIUM_DEVICE_FLAG_ACTUATOR,
   0,
   { 1, 0 },
   0,
   0
};

///////////////////////////////////////////////////////////////////////////////
// Прочее
///////////////////////////////////////////////////////////////////////////////
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

//CI2CNVRAM8Bit          g_NVRAM;
CI2CNVRAM16Bit          g_NVRAM;                   // Энергонезависимая память
//CFlashNVRAM             g_NVRAM;

CDevice                 g_Device;                  // Данные объекта

// Указатель на данные CAN интерфейса
extern CAN_HandleTypeDef   hcan1;

// Параметры CAN порта для сборки и разборки пакетов
CCANPort                g_CAN;                     // Данные внешнего CAN порта
can_frame_t             g_aCANInBuffer[256];       // Массив для приема и сборки CAN пакетов
can_frame_t             g_aCANOutBuffer[256*8];    // Массив для отправки CAN пакетов
u16                     g_u16CANID = 0;            // Идентификатор CAN

// Структура канала диммера
typedef struct dimmer_channel_s
{
   __IO u32*   m_pRegister;                        // Указатель на регистр изменения значения диммера
   u8          m_u8Current;                        // Текущее значение
   u8          m_u8Target;                         // Желаемое значение диммера
   u16         m_u16Tick;                          // Время между итерациями
   u32         m_u32Timer;                         // Промежуточный таймер
} dimmer_channel_t;

// Данные каналов диммеров
dimmer_channel_t g_aDimmers[] =
{
   {  &TIM1->CCR1,   0, 0, 0  }, 
   {  &TIM1->CCR2,   0, 0, 0  },
   {  &TIM2->CCR1,   0, 0, 0  }, 
   {  &TIM2->CCR2,   0, 0, 0  }, 
   {  &TIM15->CCR1,  0, 0, 0  }, 
   {  &TIM15->CCR2,  0, 0, 0  }, 
};

// Индексы кнопок
enum eInputs
{
   INPUT_1_INDEX = 0,                              // Дискретный вход 1 управляется замыканием на "землю"
   INPUT_2_INDEX,                                  // Дискретный вход 2 управляется замыканием на "землю"
   INPUT_3_INDEX,                                  // Дискретный вход 3 управляется замыканием на "землю"
   INPUT_4_INDEX,                                  // Дискретный вход 4 управляется замыканием на "землю"
   ONBOARD_BUTTON_INDEX,                           // Набортная кнопка
};

// Режим работы кнопки
enum eButtonMode 
{
   BUTTON_MODE_OFF = 0,                            // Канал выключен
   BUTTON_MODE_DIRECT,                             // Прямое управление (Нажатие - 1, Отжатие - 0)
   BUTTON_MODE_TRIGGER,                            // Триггерное переключение (изменение фронта изменение значения)
};

// Структура для хранения состояния бинарного входа
typedef struct input_data_s
{
   u8             m_u8Mode;                        // Режим работы дискретного входа
   u16            m_u16Delay;                      // Время долгого удержания
   struct
   {
      unsigned m_bMode        :  1;                // Режим работы триггера
      unsigned m_bOldValue    :  1;                // Предедущее значение канала
      unsigned m_bPressValue  :  1;                // Текущее тригерное значение короткого нажатия
      unsigned m_bHoldValue   :  1;                // Текущее тригерное значение длинного удержания
      unsigned m_bSendLong    :  1;                // Было отправлена датаграмма долгого удержания
   } m_Flags;
} input_data_t;

#if MAX_INPUTS != 0

// Данные для настройки бинарных каналов
input_data_t   g_aInputData[MAX_INPUTS];

#endif

/*
u16 g_aLEDTable[] =
{
       0,   403,   807,  1211,  1614,  2018,  2421,  2824,  3228,  3631,  4034,  4437,  4839,  5242,  5644,  6046,
    6448,  6850,  7251,  7652,  8053,  8453,  8854,  9253,  9653, 10052, 10451, 10849, 11247, 11644, 12042, 12438,
   12834, 13230, 13625, 14020, 14414, 14807, 15200, 15593, 15984, 16376, 16766, 17156, 17545, 17934, 18322, 18709,
   19096, 19482, 19867, 20251, 20634, 21017, 21399, 21780, 22161, 22540, 22919, 23297, 23673, 24049, 24425, 24799,
   25172, 25544, 25915, 26286, 26655, 27023, 27391, 27757, 28122, 28486, 28849, 29211, 29572, 29931, 30290, 30647,
   31004, 31359, 31713, 32065, 32417, 32767, 33116, 33464, 33810, 34155, 34499, 34842, 35183, 35523, 35862, 36199,
   36535, 36869, 37202, 37534, 37864, 38193, 38520, 38846, 39170, 39493, 39815, 40134, 40453, 40770, 41085, 41399,
   41711, 42021, 42330, 42638, 42944, 43248, 43550, 43851, 44150, 44448, 44743, 45038, 45330, 45621, 45910, 46197,
   46482, 46766, 47048, 47328, 47606, 47883, 48158, 48430, 48701, 48971, 49238, 49504, 49767, 50029, 50289, 50547,
   50802, 51057, 51309, 51559, 51807, 52053, 52298, 52540, 52780, 53018, 53255, 53489, 53721, 53951, 54180, 54406,
   54630, 54852, 55071, 55289, 55505, 55718, 55930, 56139, 56346, 56552, 56754, 56955, 57154, 57350, 57545, 57737,
   57927, 58114, 58300, 58483, 58664, 58843, 59019, 59194, 59366, 59536, 59703, 59869, 60032, 60193, 60351, 60507,
   60661, 60813, 60962, 61109, 61254, 61396, 61536, 61674, 61809, 61942, 62073, 62201, 62327, 62451, 62572, 62691,
   62807, 62921, 63033, 63142, 63249, 63353, 63455, 63555, 63652, 63747, 63840, 63930, 64017, 64102, 64185, 64265,
   64343, 64419, 64492, 64562, 64630, 64696, 64759, 64820, 64878, 64934, 64987, 65038, 65086, 65132, 65175, 65216,
   65255, 65291, 65324, 65356, 65384, 65410, 65434, 65455, 65474, 65490, 65503, 65515, 65523, 65530, 65533, 65535,
};
*/

u16 g_aLEDTable[] =
{
       0,   370,   738,  1105,  1471,  1835,  2198,  2560,  2920,  3279,  3636,  3992,  4347,  4701,  5053,  5404,
    5753,  6101,  6448,  6794,  7139,  7482,  7824,  8164,  8504,  8842,  9179,  9515,  9850, 10183, 10516, 10847,
   11177, 11506, 11833, 12160, 12485, 12810, 13133, 13455, 13776, 14096, 14415, 14733, 15049, 15365, 15680, 15993,
   16306, 16617, 16928, 17237, 17546, 17853, 18160, 18465, 18770, 19073, 19376, 19678, 19978, 20278, 20577, 20874,
   21171, 21467, 21762, 22056, 22349, 22642, 22933, 23223, 23513, 23802, 24090, 24376, 24663, 24948, 25232, 25516,
   25798, 26080, 26361, 26641, 26920, 27199, 27477, 27754, 28030, 28305, 28579, 28853, 29126, 29398, 29669, 29940,
   30209, 30478, 30747, 31014, 31281, 31547, 31812, 32076, 32340, 32603, 32865, 33127, 33388, 33648, 33907, 34166,
   34424, 34681, 34938, 35194, 35449, 35703, 35957, 36210, 36463, 36715, 36966, 37216, 37466, 37715, 37964, 38211,
   38459, 38705, 38951, 39196, 39441, 39685, 39928, 40171, 40413, 40655, 40895, 41136, 41375, 41614, 41853, 42091,
   42328, 42565, 42801, 43036, 43271, 43505, 43739, 43972, 44205, 44437, 44668, 44899, 45130, 45359, 45589, 45817,
   46045, 46273, 46500, 46726, 46952, 47178, 47402, 47627, 47851, 48074, 48297, 48519, 48740, 48962, 49182, 49402,
   49622, 49841, 50060, 50278, 50495, 50713, 50929, 51145, 51361, 51576, 51791, 52005, 52218, 52432, 52644, 52857,
   53068, 53280, 53490, 53701, 53911, 54120, 54329, 54537, 54745, 54953, 55160, 55367, 55573, 55779, 55984, 56189,
   56393, 56597, 56801, 57004, 57207, 57409, 57611, 57812, 58013, 58214, 58414, 58613, 58813, 59011, 59210, 59408,
   59605, 59803, 59999, 60196, 60392, 60587, 60782, 60977, 61171, 61365, 61559, 61752, 61945, 62137, 62329, 62521,
   62712, 62903, 63093, 63283, 63473, 63662, 63851, 64040, 64228, 64416, 64603, 64790, 64977, 65163, 65349, 65535,
};

// Индикатор устройства
CDeviceIndicator  g_Indicator;

/**
   Получение номера канала диммера
   на входе    :  in_u32ChannelID   - идентификатор канала
   на выходе   :  = ~0 (-1)   - ошибка
                  != 0        - номер канала
*/
size_t GetChannelBase(u32 in_u32ChannelID)
{
   size_t l_stResult = ~0;

   // Проверка диапазона идентификатора канала
   if(in_u32ChannelID >= CHANNEL_1_BASE && in_u32ChannelID <= CHANNEL_6_BASE + CC_SIZEOF)
   {
      // Вычисление индекса от 0 до максимального
      l_stResult = (in_u32ChannelID - CHANNEL_1_BASE) / CC_SIZEOF;
   }
   return l_stResult;
}

/**
   Получение номера команды диммера
   на входе    :  in_u32ChannelID   - идентификатор канала
   на выходе   :  = ~0 (-1)   - ошибка
                  != 0        - номер канала
*/
size_t GetChannelComm(u32 in_u32ChannelID)
{
   size_t l_stResult = ~0;

   // Проверка диапазона идентификатора канала
   if(in_u32ChannelID >= CHANNEL_1_BASE && in_u32ChannelID <= CHANNEL_6_BASE + CC_SIZEOF)
   {
      // Вычисление индекса от 0 до максимального
      l_stResult = (in_u32ChannelID - CHANNEL_1_BASE) % CC_SIZEOF;
   }
   return l_stResult;
}

/**
   Получение номера канала диммера
   на входе    :  in_u32TagID - идентификатор канала
   на выходе   :  = ~0 (-1)   - ошибка
                  != 0        - номер канала
*/
size_t GetTagBase(u32 in_u32TagID)
{
   size_t l_stResult = ~0;

   // Проверка диапазона идентификатора канала
   if(in_u32TagID >= CHANNEL_1_BASE && in_u32TagID <= CHANNEL_6_BASE + CC_SIZEOF)
   {
      // Вычисление индекса от 0 до максимального
      l_stResult = (in_u32TagID - TAG_1_BASE) / TC_SIZEOF;
   }
   return l_stResult;
}

/**
   Получение номера команды диммера
   на входе    :  in_u32TagID - идентификатор канала
   на выходе   :  = ~0 (-1)   - ошибка
                  != 0        - номер канала
*/
size_t GetTagComm(u32 in_u32TagID)
{
   size_t l_stResult = ~0;

   // Проверка диапазона идентификатора канала
   if(in_u32TagID >= TAG_1_BASE && in_u32TagID <= TAG_6_BASE + CC_SIZEOF)
   {
      // Вычисление индекса от 0 до максимального
      l_stResult = (in_u32TagID - TAG_1_BASE) % TC_SIZEOF;
   }
   return l_stResult;
}

/**
   Корректировка значения
   на входе    :  in_u8Index  - индекс канала
                  in_u8Value  - значение
   на выходе   :  скорректированное значение
*/
u8 GetCorrectionValue(u8 in_u8Index, u8 in_u8Value)
{
   u8 l_u8Result = in_u8Value;

   // Проверка выхода за пределы минимального и максимального значения
   if(l_u8Result < g_NVRAMData.m_aMin[in_u8Index])
      l_u8Result = g_NVRAMData.m_aMin[in_u8Index];
   else if(l_u8Result > g_NVRAMData.m_aMax[in_u8Index])
      l_u8Result = g_NVRAMData.m_aMax[in_u8Index];

   return l_u8Result;
}

/**
   Корректировка порога мерцания
   на входе    :  in_u8Index  - индекс канала
                  in_u8Value  - порог мерцания
   на выходе   :  *
*/
void SetCorrectionThreshold(u8 in_u8Index, u8 in_u8Value)
{
   // Если порога мерцания больше минимума, минимум будет равен порогу мерцанию
   if(in_u8Value > g_NVRAMData.m_aThreshold[in_u8Index])
      g_NVRAMData.m_aMin[in_u8Index] = in_u8Value;
   // Если порог мерцания больше максимума, максимум будет равен порогу мерцанию
   if(in_u8Value > g_NVRAMData.m_aMax[in_u8Index])
      g_NVRAMData.m_aMax[in_u8Index] = in_u8Value;
   // Установка минимального значения
   g_NVRAMData.m_aThreshold[in_u8Index] = in_u8Value;
}

/**
   Корректировка минимального значения
   на входе    :  in_u8Index  - индекс канала
                  in_u8Value  - минимальное значение
   на выходе   :  *
*/
void SetCorrectionMin(u8 in_u8Index, u8 in_u8Value)
{
   // Если минимум больше максимума, максимум будет равен минимуму
   if(in_u8Value > g_NVRAMData.m_aMax[in_u8Index])
      g_NVRAMData.m_aMax[in_u8Index] = in_u8Value;
   // Если минимум меньше порога мерцания, порог мерцания равен минимуму
   if(in_u8Value < g_NVRAMData.m_aThreshold[in_u8Index])
      g_NVRAMData.m_aThreshold[in_u8Index] = in_u8Value;
   // Установка минимального значения
   g_NVRAMData.m_aMin[in_u8Index] = in_u8Value;
}

/**
   Корректировка максимального значения
   на входе    :  in_u8Index  - индекс канала
                  in_u8Value  - максимальное значение
   на выходе   :  *
*/
void SetCorrectionMax(u8 in_u8Index, u8 in_u8Value)
{
   // Если максимум меньше минимума, максимум будет равен минимуму
   if(in_u8Value < g_NVRAMData.m_aMin[in_u8Index])
      g_NVRAMData.m_aMin[in_u8Index] = in_u8Value;
   // Если максимум меньше порога мерцания, порог мерцания равен максимуму
   if(in_u8Value < g_NVRAMData.m_aThreshold[in_u8Index])
      g_NVRAMData.m_aThreshold[in_u8Index] = in_u8Value;
   // Установка минимального значения
   g_NVRAMData.m_aMax[in_u8Index] = in_u8Value;
}

/**
   Инициализация общего описания каналов управления и обратной связи
   на входе    :  out_pDesc   - указатель на заполняемую структуру
                  in_u8Type   - тип переменной
                  in_pszDesc  - указатель на текстовое описание
                  in_pRange   - указатель на данные с минимальным, максимальным и шаговым значением
                  in_u16Units - единицы измерения
   на выходе   :  размер значения
*/
static void InitDescription(iridium_description_t* in_pDesc, u8 in_u8Type, const char* in_pszDesc, const void* in_pRange, u16 in_u16Units)
{
   in_pDesc->m_u8Type = in_u8Type;
   in_pDesc->m_pszDescription = (char*)in_pszDesc;
   in_pDesc->m_u16Units = in_u16Units;

   switch(in_u8Type)
   {
      case IVT_NONE:
         break;
      case IVT_BOOL:
      {
         in_pDesc->m_Min.m_bValue = false;
         in_pDesc->m_Max.m_bValue = true;
         in_pDesc->m_Step.m_bValue = false;
         break;
      }
      case IVT_U8:
      case IVT_S8:
      {
         u8* l_pPtr = (u8*)in_pRange;
         in_pDesc->m_Min.m_u8Value = l_pPtr[0];
         in_pDesc->m_Max.m_u8Value = l_pPtr[1];
         in_pDesc->m_Step.m_u8Value = l_pPtr[2];
         break;
      }
      case IVT_U16:
      case IVT_S16:
      {
         u16* l_pPtr = (u16*)in_pRange;
         in_pDesc->m_Min.m_u16Value = l_pPtr[0];
         in_pDesc->m_Max.m_u16Value = l_pPtr[1];
         in_pDesc->m_Step.m_u16Value = l_pPtr[2];
         break;
      }
      case IVT_U32:
      case IVT_S32:
      case IVT_F32:
      case IVT_STRING8:
      case IVT_ARRAY_U8:
      {
         u32* l_pPtr = (u32*)in_pRange;
         in_pDesc->m_Min.m_u32Value = l_pPtr[0];
         in_pDesc->m_Max.m_u32Value = l_pPtr[1];
         in_pDesc->m_Step.m_u32Value = l_pPtr[2];
         break;
      }
      case IVT_U64:
      case IVT_S64:
      case IVT_F64:
      {
         u64* l_pPtr = (u64*)in_pRange;
         in_pDesc->m_Min.m_u64Value = l_pPtr[0];
         in_pDesc->m_Max.m_u64Value = l_pPtr[1];
         in_pDesc->m_Step.m_u64Value = l_pPtr[2];
         break;
      }
      default:
         break;
   }
}

/**
   Получение размера значения
   на входе    :  in_u8Type   - тип значения
                  in_rValue   - ссылка на данные значения
   на выходе   :  размер значения
*/
static size_t GetValueSize(u8 in_u8Type, universal_value_t& in_rValue)
{
   size_t l_stResult = 1;
   
   switch(in_u8Type)
   {
      case IVT_NONE:
      case IVT_BOOL:
         break;
      case IVT_U8:
      case IVT_S8:
         if(in_rValue.m_u8Value)
            l_stResult += 1;
         break;
      case IVT_U16:
      case IVT_S16:
         if(in_rValue.m_u16Value)
            l_stResult += 2;
         break;
      case IVT_U32:
      case IVT_S32:
         if(in_rValue.m_u32Value)
            l_stResult += 4;
         break;
      case IVT_F32:
         if(in_rValue.m_f32Value != 0.0f)
            l_stResult += 4;
         break;
      case IVT_U64:
      case IVT_S64:
         if(in_rValue.m_u64Value)
            l_stResult += 8;
         break;
      case IVT_F64:
         if(in_rValue.m_f64Value != 0.0)
            l_stResult += 8;
         break;
      case IVT_STRING8:
      case IVT_ARRAY_U8:
         if(in_rValue.m_Array.m_stSize)
            l_stResult += in_rValue.m_Array.m_stSize;
         break;
      case IVT_TIME:
         if(in_rValue.m_Time.m_u8Flags)
            l_stResult += 8;
         break;

      default:
         l_stResult = 0;
         break;
   }
   return l_stResult;
}

/**
   Перезагрузка микроконтроллера
   на входе    :  *
   на выходе   :  *
*/
void Reboot()
{
   NVIC_SystemReset();
}

/**
   Генерация HWID устройства
   на входе    :  in_pHWID    - указатель на буфер куда нужно поместить строку с HWID
                  in_stSize   - размер буфера
   на выходе   :  успешность получения
   примечание  :  HWID это строка шеснадцатеричного представления 16 байтного значения.
                  Длинна строки 33 байта (32 байта + 1 байт конец)
*/
static void GenerateHWID()
{
   char l_szHexTab[] = "0123456789ABCDEF";
   u8 l_aHash[STREEBOG_BLOCK_SIZE];
   CIridiumStreebog l_Streebog;
   
   // Вычисление хэша для 96 битного идентификатора устройства
   l_Streebog.Calc((const void*)UID_BASE, 12, l_aHash, sizeof(l_aHash), SHT_HASH_256);

   // Преобразование хэша в строку
   char* l_pszHWID = g_pszHWID;
   for(size_t i = 0; i < STREEBOG_HASH_256_BYTES / 2; i++)
   {
      u8 l_u8Byte = l_aHash[i] ^ l_aHash[STREEBOG_HASH_256_BYTES / 2 + i];
      *l_pszHWID++ = l_szHexTab[l_u8Byte >> 4];
      *l_pszHWID++ = l_szHexTab[l_u8Byte & 0xF];
   }
   *l_pszHWID = 0;
}

/**
   Конструктор класса
   на входе    :  *
*/
CDevice::CDevice() : CIridiumBusProtocol()
{
   // Настройка входящего буфера
   m_InBuffer.SetBuffer(m_aInBuffer, IRIDIUM_BUS_IN_BUFFER_SIZE);
   m_InBuffer.Clear();
   
   // Настройка исходящего буфера
   m_OutBuffer.SetBuffer(IRIDIUM_BUS_MAX_HEADER_SIZE, IRIDIUM_BUS_CRC_SIZE, m_aOutBuffer, sizeof(m_aOutBuffer));
   m_OutBuffer.Clear();
}

/**
   Деструктор класса
*/
CDevice::~CDevice()
{
}

//////////////////////////////////////////////////////////////////////////
// Перегруженные методы для взаимодействия с протоколом
//////////////////////////////////////////////////////////////////////////
/**
   Отправка буфера на устройство
   на входе    :  in_pBuffer  - указатель на буфер с данными
                  in_stSize   - размер данных
   на выходе   :  успешность отправки
*/
bool CDevice::SendPacket(bool in_bBroadcast, iridium_address_t in_Address, void* in_pBuffer, size_t in_stSize)
{
   bool l_bResult = false;
   // Добавление пакета в очередь отправки
   do
   {
      // Попробуем добавить данные в буфер
      l_bResult = g_CAN.AddPacket(in_bBroadcast, in_Address, in_pBuffer, in_stSize);
      // Запустим отправку фреймов, если отправка не запущена
      if(!g_CAN.IsTransmite())
      {
         // Отправка фрейма
         CAN_SendFrame(0);
         // Отметим что отправка пакета начата
         g_CAN.SetTransmite(true);
      }
   } while(!l_bResult);

   return l_bResult;
}

/**
   Сравнение HWID
   на входе    :  in_pszHWID  - указатель на строку с аппаратным идентификатором
   на выходе   :  false - HWID не совпадает
                  true  - HWID совпадают
*/
bool CDevice::CompareHWID(const char* in_pszHWID)
{
   // Проверка входящего параметра
   return (in_pszHWID && !strcmp(in_pszHWID, g_pszHWID)) ? true : false;
}

/**
   Установка локального идентификатора
   на входе    :  in_u8LID - локальный идентификатор устройства
   на выходе   :  *
*/
void CDevice::SetLID(u8 in_u8LID)
{
   // Запись локального идентификатора в энергонезависимую память
   g_NVRAMData.m_Common.m_u8LID = in_u8LID;
   m_Address = g_NVRAMData.m_Common.m_u8LID;
   // Изменение фильтра
   CAN_SetFilter(0, g_u16CANID, m_Address);
   // Запись в энергонезависимую память
   g_NVRAM.NeedSave();
}

/**
   Проверка возможности выполнения операции
   на входе    :  in_eType    - тип операции
                  in_u32PIN   - пароль для доступа
                  in_pData    - указатель на данные
   на выходе   :  > 0   - операция доступна
                  = 0   - операция не доступна так как пароль не соответствует
                  < 0   - слишком много неудачных попыток, некоторое время доступ к значению канала будет заблокирован
*/
s8 CDevice::CheckOperation(eIridiumOperation in_eType, u32 in_u32PIN, void* in_pData)
{
   s8 l_s8Result = 0;
   size_t l_stIndex = 0;

   // Получение текущего PIN кода
   u32 l_u32PIN = g_NVRAMData.m_Common.m_u32PIN;

   // Проверка наличия PIN кода, если PIN кода нет, то проверка всегда дает положительный результат
   if(l_u32PIN)
   {
      switch(in_eType)
      {
         // Проверка возможности изменения значения локального адреса
         case IRIDIUM_OPERATION_TEST_PIN:
            l_s8Result = (l_u32PIN == in_u32PIN);
            break;

         // Чтение/запись канала управления
         case IRIDIUM_OPERATION_READ_CHANNEL:
         case IRIDIUM_OPERATION_WRITE_CHANNEL:
         {
            l_s8Result = 1;
            l_stIndex = GetChannelIndex(*(u32*)in_pData);
            if(l_stIndex != ~0)
            {
               l_s8Result = 1;
               // Проверка направления
               if(in_eType == IRIDIUM_OPERATION_READ_CHANNEL)
               {
                  // Проверка возможно ли чтение из канала управления
                  if(g_aDeviceChannels[l_stIndex].m_u8Flags & CF_R && l_u32PIN != in_u32PIN)
                     l_s8Result = 0;
               } else
               {
                  // Проверка возможна ли запись в канал управления
                  if(g_aDeviceChannels[l_stIndex].m_u8Flags & CF_W && l_u32PIN != in_u32PIN)
                     l_s8Result = 0;
               }
            }
            break;
         }
         // Чтение/запись потока
         case IRIDIUM_OPERATION_READ_STREAM:
         case IRIDIUM_OPERATION_WRITE_STREAM:
            l_s8Result = 1;
            break;
      }
   } else
      l_s8Result = 1;

   return l_s8Result;
}

/**
   Включение/выключение индикации
   на входе    :  in_u32Time  - время мигания
   на выходе   :  *
*/
void CDevice::SetBlinkTime(u32 in_u32Time)
{
   g_Indicator.SetBlinkTime(in_u32Time);
}

/**
   Получение информации об устройстве
   на входе    :  out_rInfo   - ссылка на структуру куда надо поместить данные об устройстве
   на выходе   :  успешность получения данных
*/
bool CDevice::GetSearchInfo(iridium_search_info_t& out_rInfo)
{
   // Заполнение информации об устройстве
   out_rInfo.m_u8Group     = IRIDIUM_GROUP_TYPE_ACTUATOR;
   out_rInfo.m_pszHWID     = (char*)g_pszHWID;
   out_rInfo.m_u8Event     = IRIDIUM_SEARCH_EVENT_REQUEST;
   out_rInfo.m_u8Flags     = g_DeviceInfo.m_u8Flags;
   out_rInfo.m_u32UserID   = g_NVRAMData.m_Common.m_u32UserID;
   out_rInfo.m_u16Change   = g_NVRAMData.m_Common.m_u16Change;
   return true;
}

/**
   Получение информации об устройстве
   на входе    :  out_rInfo   - ссылка на структуру куда надо поместить данные об устройстве
   на выходе   :  *
*/
bool CDevice::GetDeviceInfo(iridium_device_info_t& out_rInfo)
{
   bool l_bResult = true;
   // Копирование информации об устройстве
   memcpy(&out_rInfo, &g_DeviceInfo, sizeof(iridium_device_info_t));
   // Информация о каналах
   out_rInfo.m_u32Channels = GetChannels();
   out_rInfo.m_u32Tags = GetTags();
   out_rInfo.m_u32UserID = g_NVRAMData.m_Common.m_u32UserID;
   out_rInfo.m_u16Change = g_NVRAMData.m_Common.m_u16Change;
   return l_bResult;
}

/**
   Установка значения переменой
   на входе    :  in_u16VariableID  - идентификатор глобальной переменной
                  in_u8Type         - тип значения
                  in_rValue         - ссылка на данные значения
                  in_u8Flags        - список флагов
   на выходе   :  *
*/
void CDevice::SetVariable(u16 in_u16VariableID, u8 in_u8Type, universal_value_t& in_rValue, u8 in_u8Flags)
{
   // Проверка параметров
   if(in_u16VariableID)
   {
      // Пройдемся по всем каналам
      const device_channel_t* l_pPtr = g_aDeviceChannels;
      for(size_t i = 0; i < sizeof(g_aDeviceChannels) / sizeof(g_aDeviceChannels[0]); i++)
      {
         if(l_pPtr->m_u8MaxVariables)
         {
            u16* l_pVar = g_NVRAMData.m_aChannelVariables[l_pPtr->m_u8VarIndex];
            for(u8 j = 0; j < l_pPtr->m_u8MaxVariables; j++)
            {
               // Если есть связанная глобальная переменная, вызов изменения значения переменной
               if(l_pVar[j] == in_u16VariableID)
                  SetChannelValue(l_pPtr->m_u32ID, in_u8Type, in_rValue, in_u8Flags & IRIDIUM_FLAGS_END);
            }
         }
         l_pPtr++;
      }
   }
}

/**
   Получение значения переменной
   на входе    :  in_u16VariableID - идентификатор переменной
                  out_rType        - ссылка куда нужно поместить тип переменной
                  out_rValue       - ссылка куда нужно поместить данные переменной
   на выходе   :  успешность
*/
bool CDevice::GetVariable(u16 in_u16VariableID, u8& out_rType, universal_value_t& out_rValue)
{
   bool l_bResult = false;
   // Проверка входных параметров
   if(in_u16VariableID)
   {
      for(u8 i = 0; i < (sizeof(g_aDeviceTags) / sizeof(g_aDeviceTags[0])); i++)
      {
         // Вычисление индекса и маски
         u8 l_u8Index = i >> 3;
         u8 l_u8Mask = 1 << (i & 0x7);
         // Проверка владеет ли устройство переменной
         if((g_NVRAMData.m_aOwners[l_u8Index] & l_u8Mask) && g_NVRAMData.m_aTagVariables[i] == in_u16VariableID)
         {
            // Получение значения тега
            if(GetTagValue(g_aDeviceTags[i].m_u32ID, out_rType, out_rValue))
               l_bResult = true;
            break;
         }
      }
   }
   return l_bResult;
}

/**
   Получение количества каналов обратной связи на устройстве
   на входе    :  *
   на выходе   :  количество каналов обратной связи
*/
size_t CDevice::GetTags()
{
   return sizeof(g_aDeviceTags) / sizeof(g_aDeviceTags[0]);
}

/**
   Получение индекса по идентификатору канала обратной связи
   на входе    :  in_u32TagID - идентификатор канала обратной связи
   на выходе   :  == -1 - идентификатор не найден
                  != -1 - индекс канала обратной связи
*/
size_t CDevice::GetTagIndex(u32 in_u32TagID)
{
   size_t l_stResult = ~0;

   // Обход таблицы каналов обратной связи
   for(size_t i = 0; i < sizeof(g_aDeviceTags) / sizeof(g_aDeviceTags[0]); i++)
   {
      // Поиск идентификтора в таблице
      if(g_aDeviceTags[i].m_u32ID != in_u32TagID)
         continue;
      // Канал обратной связи найден
      l_stResult = i;
      break;
   }
   return l_stResult;
}

/**
   Получение данных тега, по индексу
   на входе    :  in_stIndex  - индекс тега
                  out_rInfo   - ссылка на структуру куда нужно поместить данные
                  in_stSize   - размер структуры, в байтах, которую нужно заполнить
   на выходе   :  размер данных
*/
size_t CDevice::GetTagData(size_t in_stIndex, iridium_tag_info_t& out_rInfo, size_t in_stSize)
{
   u8 l_u8Result = 0;

   // Подготовка структуры к заполнению
   memset(&out_rInfo, 0, in_stSize);

   // Проверка индекса канала
   if(in_stIndex < (sizeof(g_aDeviceTags) / sizeof(g_aDeviceTags[0])))
   {
      // Копирование описания канала обратной связи в обучную память
      const device_tag_t* l_pTag = &g_aDeviceTags[in_stIndex];
      // Копирование данных
      out_rInfo.m_u32ID    = l_pTag->m_u32ID;
      out_rInfo.m_pszName  = (char*)l_pTag->m_pszName;
      out_rInfo.m_u8Type   = l_pTag->m_u8Type;

      // Заполнение данных DeviceAPI
      out_rInfo.m_u16SubdeviceID = l_pTag->m_u16SubdeviceID;
      out_rInfo.m_u8FunctionID = l_pTag->m_u8FunctionID;
      out_rInfo.m_u8GroupID = l_pTag->m_u8GroupID;

      size_t l_stIndex = GetTagBase(out_rInfo.m_u32ID);
      if(l_stIndex != ~0)
      {
         sprintf(g_szTemp, out_rInfo.m_pszName, l_stIndex + 1);
         out_rInfo.m_pszName = g_szTemp;
      }

      // Получение значения канала обратной связи
      if(GetTagValue(out_rInfo.m_u32ID, out_rInfo.m_u8Type, out_rInfo.m_Value))
      {
         // Получение размера данных канала
         l_u8Result = GetValueSize(out_rInfo.m_u8Type, out_rInfo.m_Value);
         // Добавление размера имени (4 байта на идентификатор плюс размер имени и 1 байт на 0 в конце строки)
         l_u8Result += (4 + strlen(out_rInfo.m_pszName) + 1);
      }
   }
   return l_u8Result;
}

/**
   Получение описания канала обратной связи, по идентификатору
   на входе    :  in_u32TagID       - идентификатор канала обратной связи
                  out_rDescription  - ссылка на структуру куда нужно поместить описание канала обратной связи
   на выходе   :  успешность получения данных тега
*/
bool CDevice::GetTagDescription(u32 in_u32TagID, iridium_tag_description_t& out_rDescription)
{
   bool l_bResult = false;
   
   // Поиск канала управления в таблице
   size_t l_stIndex = GetTagIndex(in_u32TagID);
   if(l_stIndex != ~0)
   {
      // Получение индекса и маски в массиве хранения владения
      u8 l_u8Index = l_stIndex >> 3;
      u8 l_u8Mask = 1 << (l_stIndex & 0x7);

      // Копирование данных в обычную память
      const device_tag_t* l_pTag = &g_aDeviceTags[l_stIndex];
      // Копируем данные описания канала управления
      InitDescription(&out_rDescription.m_ID, l_pTag->m_u8Type, (char*)l_pTag->m_pszDesc, l_pTag->m_pRange, l_pTag->m_u16Units);
      out_rDescription.m_ID.m_u16Units = 0;
      
      // Получение флага владения
      out_rDescription.m_Flags.m_bOwner = (0 != (g_NVRAMData.m_aOwners[l_u8Index] & l_u8Mask));
      // Получение переменной
      out_rDescription.m_u16Variable = g_NVRAMData.m_aTagVariables[l_stIndex];
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Установка каналу обратной связи глобальной переменной
   на входе    :  in_u32TagID      - идентификатор тега
                  in_bOwner        - признак владения
                  in_u16Variable   - идентификатор переменной
   на выходе   :  *
*/
bool CDevice::LinkTagAndVariable(u32 in_u32TagID, bool in_bOwner, u16 in_u16Variable)
{
   bool l_bResult = false;
   size_t l_stIndex = GetTagIndex(in_u32TagID);
   if(l_stIndex != ~0)
   {
      u8 l_u8Index = l_stIndex >> 3;
      u8 l_u8Mask = 1 << (l_stIndex & 0x7);
      // Запись глобальной переменной в энергонезависимую память
      g_NVRAMData.m_aTagVariables[l_stIndex] = in_u16Variable;
      // Выставим флаг владения
      if(in_bOwner)
         g_NVRAMData.m_aOwners[l_u8Index] |= l_u8Mask;
      else
         g_NVRAMData.m_aOwners[l_u8Index] &= ~l_u8Mask;
      // Отметим что нужно записать данные в энергонезависимую память
      g_NVRAM.NeedSave();
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Установка значения канала обратной связи
   на входе    :  in_u32TagID - идентификатор канала обратной связи
                  in_u8Type   - тип значения канала управления
                  in_rValue   - ссылка на значение канала управления
                  in_u8Flags  - список флагов
   на выходе   :  успешность установки значения канала управления
*/
bool CDevice::SetTagValue(u32 in_u32TagID, u8 in_u8Type, universal_value_t& in_rValue, u8 in_u8Flags)
{
   bool l_bResult = false;
   // Поиск канала управления в таблице
   size_t l_stIndex = GetTagIndex(in_u32TagID);
   if(l_stIndex != ~0)
   {
      // Отправка значения в канал
      ChangeVariable(g_NVRAMData.m_aTagVariables[l_stIndex], in_u8Type, in_rValue);
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Получение количества каналов управления на устройстве
   на входе    :  *
   на выходе   :  количество каналов управления
*/
size_t CDevice::GetChannels()
{
   return sizeof(g_aDeviceChannels) / sizeof(g_aDeviceChannels[0]);
}

/**
   Получение индекса по идентификатору канала управления
   на входе    :  in_u32ChannelID   - идентификатор канала управления
   на выходе   :  == -1 - идентификатор не найден
                  != -1 - индекс канала управления
*/
size_t CDevice::GetChannelIndex(u32 in_u32ChannelID)
{
   size_t l_stResult = ~0;

   // Обход таблицы каналов управления
   for(size_t i = 0; i < sizeof(g_aDeviceChannels) / sizeof(g_aDeviceChannels[0]); i++)
   {
      // Поиск идентификтора в таблице
      if(g_aDeviceChannels[i].m_u32ID != in_u32ChannelID)
         continue;
      // Канал управления найден
      l_stResult = i;
      break;
   }

   return l_stResult;
}

/**
   Получение данных канала, по индексу
   на входе    :  in_stIndex  - индекс канала
                  out_rInfo   - ссылка на структуру куда нужно поместить данные
   на выходе   :  размер данных
*/
size_t CDevice::GetChannelData(size_t in_stIndex, iridium_channel_info_t& out_rInfo)
{
   size_t l_stResult = 0;
   // Подготовка структуры к заполнению
   memset(&out_rInfo, 0, sizeof(iridium_channel_info_t));
   // Копирование описания канала
   if(in_stIndex < sizeof(g_aDeviceChannels) / sizeof(g_aDeviceChannels[0]))
   {
      // Перенос из флеша в обычную память
      const device_channel_t* l_pChannel = &g_aDeviceChannels[in_stIndex];
      
      // Копирование параметров
      out_rInfo.m_u32ID    = l_pChannel->m_u32ID;
      out_rInfo.m_pszName  = (char*)l_pChannel->m_pszName;
      out_rInfo.m_u8Type   = l_pChannel->m_u8Type;

      // Заполнение данных DeviceAPI
      out_rInfo.m_u16SubdeviceID = l_pChannel->m_u16SubdeviceID;
      out_rInfo.m_u8FunctionID = l_pChannel->m_u8FunctionID;
      out_rInfo.m_u8GroupID = l_pChannel->m_u8GroupID;

      // Формирование имени канала
      size_t l_stIndex = GetChannelBase(out_rInfo.m_u32ID);
      if(l_stIndex != ~0)
      {
         sprintf(g_szTemp, out_rInfo.m_pszName, l_stIndex + 1);
         out_rInfo.m_pszName = g_szTemp;
      }

      // Получение значения канала
      if(GetChannelValue(out_rInfo.m_u32ID, out_rInfo.m_u8Type, out_rInfo.m_Value))
      {
         // Вычисление размера структуры (4 байта на идентификатор плюс длинна имени плюс 1 байт на конец строки)
         l_stResult = 4 + strlen(out_rInfo.m_pszName) + 1;
      }
   }

   return l_stResult;
}

/**
   Получение описания канала управления, по идентификатору
   на входе    :  in_u32ChannelID   - идентификатор канала управления
                  out_rDescription  - ссылка на структуру куда нужно поместить описание канала управления
   на выходе   :  успешность
*/
bool CDevice::GetChannelDescription(u32 in_u32ChannelID, iridium_channel_description_t& out_rDescription)
{   
   bool l_bResult = false;
   device_channel_t l_Channel;
   // Поиск канала обратной связи в таблице
   size_t l_stChannel = GetChannelIndex(in_u32ChannelID);
   if(l_stChannel != ~0)
   {
      // Копируем данные описания канала
      memcpy(&l_Channel, &g_aDeviceChannels[l_stChannel], sizeof(device_channel_t));

      // Заполнение параметров
      InitDescription(&out_rDescription.m_ID, l_Channel.m_u8Type, (char*)l_Channel.m_pszDesc, l_Channel.m_pRange, l_Channel.m_u16Units);
      
      out_rDescription.m_Flags.m_bWritePassword = (0 != (l_Channel.m_u8Flags & CF_W));
      out_rDescription.m_Flags.m_bReadPassword  = (0 != (l_Channel.m_u8Flags & CF_R));
      out_rDescription.m_u8MaxVariables         = l_Channel.m_u8MaxVariables;

      // Получение количества переменных
      out_rDescription.m_u8Variables = l_Channel.m_u8MaxVariables;
      out_rDescription.m_pVariables = NULL;
      // Если список переменных есть
      if(l_Channel.m_u8MaxVariables)
         out_rDescription.m_pVariables = g_NVRAMData.m_aChannelVariables[l_Channel.m_u8VarIndex];
      l_bResult = true;
   }
   return l_bResult;
}

/**
   Установка каналу глобальной переменной
   на входе    :  in_u32ChannelID  - идентификатор канала
                  in_u8Variables   - колличество переменных
                  in_pVariables    - указатель на список идентификаторов глобальных переменных
   на выходе   :  успешность установки
*/
bool CDevice::LinkChannelAndVariable(u32 in_u32ChannelID, u8 in_u8Variables, u16* in_pVariables)
{
   bool l_bResult = false;

   // Проверка входных параметров
   size_t l_stIndex = GetChannelIndex(in_u32ChannelID);
   if(l_stIndex != ~0)
   {
      // Перенос из флеша в обычную память
      const device_channel_t* l_Channel = &g_aDeviceChannels[l_stIndex];

      // Проверка наличия массива переменных
      if(l_Channel->m_u8MaxVariables)
      {
         if(in_u8Variables > l_Channel->m_u8MaxVariables)
            in_u8Variables = l_Channel->m_u8MaxVariables;

         u16* l_pVar = g_NVRAMData.m_aChannelVariables[l_Channel->m_u8VarIndex];

         // Запись списка глобальных переменных в энергонезависимую память
         for(u8 i = 0; i < l_Channel->m_u8MaxVariables; i++)
            l_pVar[i] = (i < in_u8Variables ? in_pVariables[i] : 0);
         g_NVRAM.NeedSave();
         l_bResult = true;
      }
   }

   return l_bResult;
}

/**
   Установка значения канала управления
   на входе    :  in_u32ChannelID   - идентификатор канала управления
                  in_u8Type         - тип значения канала управления
                  in_rValue         - ссылка на значение канала управления
                  in_u8Flags        - список флагов
   на выходе   :  успешность установки значения канала управления
*/
bool CDevice::SetChannelValue(u32 in_u32ChannelID, u8 in_u8Type, universal_value_t& in_rValue, u8 in_u8Flags)
{
   bool l_bResult = false;
   bool l_bSave = false;
   
   switch(in_u32ChannelID)
   {
   // Изменение имени устройства
   case SYSTEM_CHANNEL_REBOOT:
      Reboot();
      break;
   // Изменение имени устройства
   case SYSTEM_CHANNEL_NAME:
      if(in_u8Type == IVT_STRING8 && in_rValue.m_Array.m_stSize)
      {
         // Сохраним новое имя устройства
         size_t l_stMax = sizeof(g_NVRAMData.m_Common.m_szDeviceName);
         size_t l_stSize = (in_rValue.m_Array.m_stSize > l_stMax) ? l_stMax : in_rValue.m_Array.m_stSize;
         memcpy(g_NVRAMData.m_Common.m_szDeviceName, in_rValue.m_Array.m_pPtr, l_stSize);
         l_bSave = true;
         l_bResult = true;
      }
      break;
      
   // Изменение PIN кода
   case SYSTEM_CHANNEL_PIN: 
      if(in_u8Type == IVT_U32)
      {
         // Сохраним новое значение PIN
         g_NVRAMData.m_Common.m_u32PIN = in_rValue.m_u32Value;
         l_bSave = true;
         l_bResult = true;
      }
      break;

   // Изменение пользовательского идентификатора
   case SYSTEM_CHANNEL_USER_ID:
      if(in_u8Type == IVT_U32)
      {
         // Сохраним новое значение PIN
         g_NVRAMData.m_Common.m_u32UserID = in_rValue.m_u32Value;
         l_bSave = true;
         l_bResult = true;
      }
      break;
   default:
      {
         // Получение индекса диммера
         size_t l_stIndex = GetChannelBase(in_u32ChannelID);
         if(l_stIndex != ~0)
         {
            // Получение номера команды
            size_t l_stComm = GetChannelComm(in_u32ChannelID);
            if(l_stComm != ~0)
            {
               // Обработка команд
               switch(l_stComm)
               {
                  // Канал установки значения диммера
                  case CC_SET:
                     if(in_u8Type == IVT_U8)
                     {
                        // Коррекция полученного значения
                        in_rValue.m_u8Value = GetCorrectionValue(l_stIndex, in_rValue.m_u8Value);
                        l_bResult = true;
                     }
                     break;
                  // Канал включения диммера
                  case CC_ON:
                     // Получение последнего значения и его коррекция
                     in_rValue.m_u8Value = GetCorrectionValue(l_stIndex, g_NVRAMData.m_aLastValue[l_stIndex]);
                     l_bResult = true;
                     break;
                  // Канал выключения диммера
                  case CC_OFF:
                     // Сохранение текущего значения канала
                     g_NVRAMData.m_aLastValue[l_stIndex] = g_aDimmers[l_stIndex].m_u8Current;
                     in_rValue.m_u8Value = 0;
                     l_bResult = true;
                     break;
                  // Канал увеличения уровня диммера
                  case CC_UP:
                     in_rValue.m_u8Value = GetCorrectionValue(l_stIndex, g_aDimmers[l_stIndex].m_u8Current);
                     if(in_rValue.m_u8Value < g_NVRAMData.m_aMax[l_stIndex])
                     {
                        in_rValue.m_u8Value++;
                        l_bResult = true;
                     }
                     break;
                  // Канал уменьшения уровня диммера
                  case CC_DOWN:
                     in_rValue.m_u8Value = GetCorrectionValue(l_stIndex, g_aDimmers[l_stIndex].m_u8Current);
                     if(in_rValue.m_u8Value > g_NVRAMData.m_aMin[l_stIndex])
                     {
                        in_rValue.m_u8Value--;
                        l_bResult = true;
                     }
                     break;
                  // Канал вызова минимального значения
                  case CC_RECALL_MIN:
                     in_rValue.m_u8Value = g_NVRAMData.m_aMin[l_stIndex];
                     l_bResult = true;
                     break;
                  // Канал вызова максимального значения канала
                  case CC_RECALL_MAX:
                     in_rValue.m_u8Value = g_NVRAMData.m_aMax[l_stIndex];
                     l_bResult = true;
                     break;
                  // Канал установки порога мерцания
                  case CC_THRESHOLD:
                     if(in_u8Type == IVT_U8)
                     {
                        SetCorrectionThreshold(l_stIndex, in_rValue.m_u8Value);
                        l_bSave = true;
                     }
                     break;
                  // Канал установки минимального значения диммера
                  case CC_MIN:
                     if(in_u8Type == IVT_U8)
                     {
                        // Установка минимального значения
                        SetCorrectionMin(l_stIndex, in_rValue.m_u8Value);
                        // Коррекция значения
                        if(*g_aDimmers[l_stIndex].m_pRegister)
                        {
                           in_rValue.m_u8Value = GetCorrectionValue(l_stIndex, g_aDimmers[l_stIndex].m_u8Current);
                           l_bResult = true;
                        }
                        l_bSave = true;
                     }
                     break;
                  // Канал установки максимального значения диммера
                  case CC_MAX:
                     if(in_u8Type == IVT_U8)
                     {
                        // Установка минимального значения
                        SetCorrectionMax(l_stIndex, in_rValue.m_u8Value);
                        // Коррекция значения
                        if(*g_aDimmers[l_stIndex].m_pRegister)
                        {
                           in_rValue.m_u8Value = GetCorrectionValue(l_stIndex, g_aDimmers[l_stIndex].m_u8Current);
                           l_bResult = true;
                        }
                        l_bSave = true;
                     }
                     break;
                  // Канал установки значение диммера при включении
                  case CC_POWER_ON:
                     if(in_u8Type == IVT_U8)
                     {
                        g_NVRAMData.m_aPower[l_stIndex] = in_rValue.m_u8Value;
                        l_bSave = true;
                     }
                     break;
                  // Канал установки значения диммера при ошибке
                  case CC_ERROR:
                     if(in_u8Type == IVT_U8)
                     {
                        g_NVRAMData.m_aError[l_stIndex] = in_rValue.m_u8Value;
                        l_bSave = true;
                     }
                     break;
                  // Канал установки частоты обновления значения диммера
                  case CC_FADE_IN_TIME:
                     if(in_u8Type == IVT_U32)
                     {
                        g_NVRAMData.m_aInTime[l_stIndex] = in_rValue.m_u32Value;
                        l_bSave = true;
                     }
                     break;
                  // Канал установки времени изменения диммера
                  case CC_FADE_OUT_TIME:
                     if(in_u8Type == IVT_U32)
                     {
                        g_NVRAMData.m_aOutTime[l_stIndex] = in_rValue.m_u32Value;
                        l_bSave = true;
                     }
                     break;
               }

               // Сохранение данных в энергонезависимую память
               if(l_bSave)
                  g_NVRAM.NeedSave();

               // Изменение значения значения канала
               if(l_bResult)
               {
                  // Сохранение значения
                  g_aDimmers[l_stIndex].m_u8Target = in_rValue.m_u8Value;
                  // Проверка направления
                  if(g_aDimmers[l_stIndex].m_u8Target > g_aDimmers[l_stIndex].m_u8Current)
                     g_aDimmers[l_stIndex].m_u16Tick = g_NVRAMData.m_aInTime[l_stIndex] / 255;
                  else
                     g_aDimmers[l_stIndex].m_u16Tick = g_NVRAMData.m_aOutTime[l_stIndex] / 255;
               }
               l_bResult = true;
            }
         }
         break;
      }
   }
   return l_bResult;
}

/**
   Обработчик получения запроса на открытие потока
   на входе    :  in_pszName  - имя потока
                  in_eMode    - режим открытия потока
   на выходе   :  идентификатор открытого потока, если поток не был открыт возвражаемый результат равен 0
*/
u8 CDevice::StreamOpen(const char* in_pszName, eIridiumStreamMode in_eMode)
{
   u8 l_u8Result = 0;

   // Проверка имени запрашиваемого потока
   if(in_pszName && !strcmp(in_pszName, FIRMWARE_NAME))
   {
      if(in_eMode == IRIDIUM_STREAM_MODE_WRITE)
      {
         // Запишем в энергонезависимую память состояние загрузки прошивки
         g_NVRAMData.m_Common.m_u8Mode = BOOTLOADER_MODE_DOWNLOAD;
         // Запись адреса
         g_NVRAMData.m_Common.m_u16FirmwareAddress = GetSrcAddress();
         // Запись идентификатора транзакции
         g_NVRAMData.m_Common.m_u16FirmwareTID = m_InMH.m_u16TID;
         // Сохранение в энергонезависимую память
         g_NVRAM.Save();
         // Осуществим переход в загрузчик через сброс контроллера
         Reboot();
      }
   }

   return l_u8Result;
}

/**
   Обработчик получения ответа на запрос открытия потока
   на входе    :  in_pszName     - имя потока
                  in_eMode       - режим открытия потока
                  in_u8StreamID  - идентификатор потока
   на выходе   :  *
*/
void CDevice::StreamOpenResult(const char* in_pszName, eIridiumStreamMode in_eMode, u8 in_u8StreamID)
{
}

/**
   Обработчик получения запроса передачи данных блока
   на входе    :  in_u8StreamID  - идентификатор потока
                  in_u8BlockID   - идентификатор блока
                  in_stSize      - размер данных блока
                  in_pBuffer     - указатель на буфер с данными блока
   на выходе   :  количество обработанных данных
*/
size_t CDevice::StreamBlock(u8 in_u8StreamID, u8 in_u8BlockID, size_t in_stSize, const void* in_pBuffer)
{
   return in_stSize;
}

/**
   Обработчик получения ответа на запрос передачи данных блока
   на входе    :  in_u8StreamID  - идентификатор потока
                  in_u8BlockID   - идентификатор блока
                  in_stSize      - количество обработанных данных
   на выходе   :  *
*/
void CDevice::StreamBlockResult(u8 in_u8StreamID, u8 in_u8BlockID, size_t in_stSize)
{
}

/**
   Обработчик закрытия потока
   на входе    :  in_u8StreamID  - идентификатор потока
   на выходе   :  *
*/
void CDevice::StreamClose(u8 in_u8StreamID)
{
}

/**
   Получение Smart API информации
   на входе    :  out_rBuffer - ссылка на указатель куда нужно указатель на данные Smart API
                  out_rSize   - ссылка на переменную куда нужно поместить размер данных Smart API
   на выход    :  успешность получения данных
*/
bool CDevice::GetSmartAPI(void*& out_rBuffer, size_t& out_rSize)
{
   out_rBuffer = NULL;//&g_SmartAPI;
   out_rSize   = 0;//sizeof(g_SmartAPI);
   return true;
}

/**
   Инициализация устройства
   на входе    :  *
   на выходе   :  *
*/
void CDevice::Setup()
{
   iridium_search_info_t l_Search;
  
   // Чтение текущего состояния
   if(g_NVRAMData.m_Common.m_u16FirmwareID != DIMMER_DND3P)
   {
      // Сброс PIN кода
      g_NVRAMData.m_Common.m_u32PIN = 0;
      // Сброс флагов владения
      memset(g_NVRAMData.m_aOwners, 0, sizeof(g_NVRAMData.m_aOwners));
      // Сброс глобальных переменных связанных с каналами обратной связи
      memset(g_NVRAMData.m_aTagVariables, 0, sizeof(g_NVRAMData.m_aTagVariables));
      // Сброс глобальных переменных связанных с каналами управления
      memset(g_NVRAMData.m_aChannelVariables, 0, sizeof(g_NVRAMData.m_aChannelVariables));

      // Запись данных для бутлоадера
      g_NVRAMData.m_Common.m_u16FirmwareID = DIMMER_DND3P;
      g_NVRAMData.m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;

      // Сохранение данных в энергонезависимую память
      g_NVRAM.Save();
   }
   
   // Загрузка локального идентификатора
   m_Address = g_NVRAMData.m_Common.m_u8LID;
   
   // Корректировка имени
   //SetValidEndByUTF8(g_EEPROM.m_Common.m_szDeviceName, sizeof(g_EEPROM.m_Common.m_szDeviceName), sizeof(g_EEPROM.m_Common.m_szDeviceName));
      
   // Генерация идентификатора
   GenerateHWID();

   // Инициализация CAN порта
   g_CAN.SetInBuffer(g_aCANInBuffer, sizeof(g_aCANInBuffer));
   g_CAN.SetOutBuffer(g_aCANOutBuffer, sizeof(g_aCANOutBuffer));

   // Инициализация шины
   CAN_Init(0, &hcan1, &g_CAN);
   
   // Установка фильтров
   g_u16CANID = GetCRC16Modbus(1, (u8*)g_pszHWID, sizeof(g_pszHWID));
   CAN_SetFilter(0, g_u16CANID, m_Address);

   // Выключение набортного светодиода
   HAL_GPIO_WritePin(Onboard_LED_GPIO_Port, Onboard_LED_Pin, GPIO_PIN_SET);
   // Отправка информации об устройстве
   if(GetSearchInfo(l_Search))
      SendSearchResponse(GetTID(), l_Search);

   for(size_t i = 0; i < sizeof(g_aDimmers)/sizeof(g_aDimmers[0]); i++)
   {
      dimmer_channel_t* l_pDimmer = &g_aDimmers[i];
      l_pDimmer->m_u8Target = g_NVRAMData.m_aPower[i];
   }
}

/**
   Основной цикл работы устройства
   на входе    :  *
   на выходе   :  *
*/
void CDevice::Loop()
{
   // обработка нажатий
   WorkInputs();
   
   {
      bool l_bResult = false;
      void* l_pBuffer = NULL;
      size_t l_stSize = 0;

      l_bResult = g_CAN.GetPacket(l_pBuffer, l_stSize);

      // Удаление обработанных пакетов
      g_CAN.Flush();

      // Проверка наличия данных
      if(l_bResult)
      {
         // Обработка пакета
         m_InBuffer.SetBuffer(l_pBuffer, l_stSize);
         // Обрабатывать входящий поток пока исходящий буфер не заполнен на половину
         if(m_InBuffer.OpenPacket() > 0)
         {
            iridium_packet_header_t* l_pPH = m_InBuffer.GetPacketHeader();

            u8* l_pPacketPtr = (u8*)m_InBuffer.GetMessagePtr();
            size_t l_stPacketSize = m_InBuffer.GetMessageSize();

   #if defined(IRIDIUM_ENABLE_CIPHER)
            // Декодирование сообщения
            if(DecodeMessage(l_pPH->m_Flags.m_u3Crypt, l_pPacketPtr, l_stPacketSize))
   #endif
            {
               // Обработка сообщения
               ProcessMessage(l_pPH, l_pPacketPtr, l_stPacketSize);
            }
            // Закрытие шинного сообщения
            m_InBuffer.ClosePacket();
         }

         // Обработка полученого пакета
         g_CAN.DeletePacket();
      }
   }

   for(size_t i = 0; i < sizeof(g_aDimmers)/sizeof(g_aDimmers[0]); i++)
   {
      dimmer_channel_t* l_pDimmer = &g_aDimmers[i];
      u8 l_u8Val = l_pDimmer->m_u8Current;
      if(l_pDimmer->m_u8Target != l_u8Val)
      {
         // Проверка инициализации таймера
         if(!l_pDimmer->m_u32Timer)
            l_pDimmer->m_u32Timer = HAL_GetTick();

         if((HAL_GetTick() - l_pDimmer->m_u32Timer) > l_pDimmer->m_u16Tick)
         {
            if(l_u8Val > l_pDimmer->m_u8Target)
               l_u8Val--;
            else
               l_u8Val++;

            universal_value_t l_Val;
            l_Val.m_u8Value = l_u8Val;

            // Генерация изменения значения 
            size_t l_stInd = GetTagIndex(TAG_1_BASE + i * TC_SIZEOF + TC_VALUE);
            if(l_stInd != ~0)
               SendSetVariableRequest(g_NVRAMData.m_aTagVariables[l_stInd], IVT_U8, l_Val);

            // Генерация сигнала достижения минимума
            if(l_u8Val == g_NVRAMData.m_aMin[i])
            {
               l_stInd = GetTagIndex(TAG_1_BASE + i * TC_SIZEOF + TC_MIN);
               if(l_stInd != ~0)
                  SendSetVariableRequest(g_NVRAMData.m_aTagVariables[l_stInd], IVT_NONE, l_Val);
            }

            // Генерация сигнала достижения максимума
            if(l_u8Val == g_NVRAMData.m_aMax[i])
            {
               l_stInd = GetTagIndex(TAG_1_BASE + i * TC_SIZEOF + TC_MAX);
               if(l_stInd != ~0)
                  SendSetVariableRequest(g_NVRAMData.m_aTagVariables[l_stInd], IVT_NONE, l_Val);
            }
            // Проверка изменения
            if(l_pDimmer->m_u8Current != l_u8Val)
            {
               if(l_pDimmer->m_u8Current == 0 && l_u8Val)
               {
                  l_stInd = GetTagIndex(TAG_1_BASE + i * TC_SIZEOF + TC_ON);
                  if(l_stInd != ~0)
                     SendSetVariableRequest(g_NVRAMData.m_aTagVariables[l_stInd], IVT_NONE, l_Val);

               } else if(l_pDimmer->m_u8Current && l_u8Val == 0)
               {
                  l_stInd = GetTagIndex(TAG_1_BASE + i * TC_SIZEOF + TC_OFF);
                  if(l_stInd != ~0)
                     SendSetVariableRequest(g_NVRAMData.m_aTagVariables[l_stInd], IVT_NONE, l_Val);
               }
                  
            }


            // Изменение значения
            l_pDimmer->m_u8Current = l_u8Val;
            *l_pDimmer->m_pRegister = l_u8Val << 8;
            // Сброс таймера
            l_pDimmer->m_u32Timer = 0;
         }
      } else
         l_pDimmer->m_u32Timer = 0;
   }
}

/**
   Цикл обработки нажатий
   на входе    :  *
   на выходе   :  *
*/
void CDevice::WorkInputs()
{

#if MAX_INPUTS != 0

   universal_value_t l_Val;
   iridium_search_info_t l_Search;
   // Обработка нажатий
   IO_UpdateInput(g_aInputs, sizeof(g_aInputs) / sizeof(g_aInputs[0]));

   // Если была нажата набортная кнопка, пошлем в шину информацию о себе
   if(g_aInputs[ONBOARD_BUTTON_INDEX].m_Flags.m_bChange && g_aInputs[ONBOARD_BUTTON_INDEX].m_Flags.m_bCurValue)
   {
      // Получение информации об устройстве
      if(GetSearchInfo(l_Search))
      {
         // Отправка информации в шину
         SendSearchResponse(GetTID(), l_Search);
      }
   }

   // Обработка дискретных входов
   for(u8 i = 0; i < MAX_INPUTS; i++)
   {
      // Получение значения у учетом инвертирования значения
      bool l_bValue = g_aInputs[i].m_Flags.m_bCurValue;
      bool l_bOld = g_aInputData[i].m_Flags.m_bOldValue;
      u8 l_u8Press = TAG_PRESS_1 + i * 2 - 1;
      u8 l_u8Long = l_u8Press + 1;

      // Прямое управление
      if(g_aInputData[i].m_u8Mode == BUTTON_MODE_DIRECT)
      {
         // Подготовка значения
         l_Val.m_bValue = l_bValue;

         // Проверка изменения значения
         if(l_bValue != l_bOld)
         {
            // Если было длительное удержание, сбросим длинное удержание
            if(g_aInputData[i].m_Flags.m_bSendLong && !l_bValue)
               ChangeVariable(g_aTagsVariable[l_u8Long], IVT_BOOL, l_Val);

            // Отправка значения в шину при коротком нажатии
            ChangeVariable(g_aTagsVariable[l_u8Press], IVT_BOOL, l_Val);
            // Сбросим флаг отправки значения долгого удержания
            g_aInputData[i].m_Flags.m_bSendLong = false;
            // Запомним значение
            g_aInputData[i].m_Flags.m_bOldValue = l_bValue;
         } else
         {
            // Проверка на долгое удержание
            if(!g_aInputData[i].m_Flags.m_bSendLong && l_bValue && g_aInputs[i].m_u32ChangeTime && (HAL_GetTick() - g_aInputs[i].m_u32ChangeTime) > g_aInputData[i].m_u16Delay)
            {
               // Отправка значения в шину при долгом удержании
               ChangeVariable(g_aTagsVariable[l_u8Long], IVT_BOOL, l_Val);
               // Установим флаг отправки значения долгого удержания
               g_aInputData[i].m_Flags.m_bSendLong = true;
            }
         }
      // Тригерное управление
      } else if(g_aInputData[i].m_u8Mode == BUTTON_MODE_TRIGGER)
      {
         // Проверка изменения значения
         if(l_bValue != l_bOld)
         {
            // Запомним текущее значение
            g_aInputData[i].m_Flags.m_bOldValue = l_bValue;
            // Коррекция входа, по нажатию или по отжатию
            l_bValue ^= g_aInputData[i].m_Flags.m_bMode;
            if(l_bValue)
            {
               g_aInputData[i].m_Flags.m_bPressValue ^= 1;
               // Отправка значения в шину при коротком нажатии
               l_Val.m_bValue = g_aInputData[i].m_Flags.m_bPressValue;
               ChangeVariable(g_aTagsVariable[l_u8Press], IVT_BOOL, l_Val);
               // Сбросим флаг отправки значения долгого удержания
               g_aInputData[i].m_Flags.m_bSendLong = false;
            }
         } else
         {
            // Проверка на долгое удержание
            if(!g_aInputData[i].m_Flags.m_bSendLong && l_bValue && g_aInputs[i].m_u32ChangeTime && (HAL_GetTick() - g_aInputs[i].m_u32ChangeTime) > g_aInputData[i].m_u16Delay)
            {
               g_aInputData[i].m_Flags.m_bHoldValue ^= 1;
               // Отправка значения в шину при долгом удержании
               l_Val.m_bValue = g_aInputData[i].m_Flags.m_bHoldValue;
               ChangeVariable(g_aTagsVariable[l_u8Long], IVT_BOOL, l_Val);
               // Установим флаг отправки значения долгого удержания
               g_aInputData[i].m_Flags.m_bSendLong = true;
            }
         }
      }
   }

#endif

}

/**
   Изменение значения глобальной переменной
   на входе    :  in_u16Variable - идентификатор изменяемой переменной
                  in_u8Type      - тип переменной
                  in_rValue      - ссылка на данные значения
   на выходе   :  *
*/
void CDevice::ChangeVariable(u16 in_u16Variable, u8 in_u8Type, universal_value_t& in_rValue)
{
   // Отправка значения на шину
   SendSetVariableRequest(in_u16Variable, in_u8Type, in_rValue);
   // Изменение значения
   SetVariable(in_u16Variable, in_u8Type, in_rValue, IRIDIUM_FLAGS_END);
}

/**
   Получение значения канала управления
   на входе    :  in_u32ChannelID   - идентиифкатор канала управления
                  out_rType         - ссылка на переменную куда нужно тип значения канала управления
                  out_rValue        - ссылка на переменную куда нужно поместить значение канала управления
   на выходе   :  успешность получения значения
*/
bool CDevice::GetChannelValue(u32 in_u32ChannelID, u8& out_rType, universal_value_t& out_rValue)
{
   bool l_bResult = true;

   switch(in_u32ChannelID)
   {
      // Системный канал с именем устройства
      case SYSTEM_CHANNEL_NAME:
         out_rType = IVT_STRING8;
         out_rValue.m_Array.m_pPtr = g_NVRAMData.m_Common.m_szDeviceName;
         out_rValue.m_Array.m_stSize = strlen((char*)out_rValue.m_Array.m_pPtr);
         break;

      // Системный канал с PIN кодом
      case SYSTEM_CHANNEL_PIN:
         out_rType = IVT_U32;
         out_rValue.m_u32Value = g_NVRAMData.m_Common.m_u32PIN;
         break;

      // Системный канал с пользовательским идентиифкатором
      case SYSTEM_CHANNEL_USER_ID:
         out_rType = IVT_U32;
         out_rValue.m_u32Value = g_NVRAMData.m_Common.m_u32UserID;
         break;

      // Системный канал для перезагрузки контроллера
      case SYSTEM_CHANNEL_REBOOT:
         out_rType = IVT_BOOL;
         out_rValue.m_bValue = false;
         break;

      default:
      {
         l_bResult = false;
         size_t l_stIndex = GetChannelBase(in_u32ChannelID);
         if(l_stIndex != ~0)
         {
            size_t l_stComm = GetChannelComm(in_u32ChannelID);
            if(l_stComm != ~0)
            {
               switch(l_stComm)
               {
                  // Канал установки значения диммера
                  case CC_SET:
                     out_rType = IVT_U8;
                     out_rValue.m_u8Value = g_aDimmers[l_stIndex].m_u8Current;//*g_aDimmers[l_stIndex].m_pRegister;
                     break;
                  // Канал минимального значения диммера
                  case CC_THRESHOLD:
                     out_rType = IVT_U8;
                     out_rValue.m_u8Value = g_NVRAMData.m_aThreshold[l_stIndex];
                     break;
                  // Канал минимального значения диммера
                  case CC_MIN:
                     out_rType = IVT_U8;
                     out_rValue.m_u8Value = g_NVRAMData.m_aMin[l_stIndex];
                     break;
                  // Канал максимального значения диммера
                  case CC_MAX:
                     out_rType = IVT_U8;
                     out_rValue.m_u8Value = g_NVRAMData.m_aMax[l_stIndex];
                     break;
                  // Значение диммера при включении
                  case CC_POWER_ON:
                     out_rType = IVT_U8;
                     out_rValue.m_u8Value = g_NVRAMData.m_aPower[l_stIndex];
                     break;
                  // Значение диммера при ошибке
                  case CC_ERROR:
                     out_rType = IVT_U8;
                     out_rValue.m_u8Value = g_NVRAMData.m_aError[l_stIndex];
                     break;
                  // Время изменения диммера
                  case CC_FADE_IN_TIME:
                     out_rType = IVT_U32;
                     out_rValue.m_u32Value = g_NVRAMData.m_aInTime[l_stIndex];
                     break;
                  // Время изменения диммера
                  case CC_FADE_OUT_TIME:
                     out_rType = IVT_U32;
                     out_rValue.m_u32Value = g_NVRAMData.m_aOutTime[l_stIndex];
                     break;
                  default:
                     out_rType = IVT_NONE;
                     out_rValue.m_u8Value = 0;
                     break;
               }
               l_bResult = true;
            }
         }
      }
   }

   return l_bResult;
}

/**
   Получение значения канала обратной связи
   на входе    :  in_u32TagID - идентификатор канала обратной связи
                  out_rType   - ссылка на переменную куда нужно тип значения канала обратной связи
                  out_rValue  - ссылка на переменную куда нужно поместить значение канала обратной связи
   на выходе   :  успешность получения значения
*/
bool CDevice::GetTagValue(u32 in_u32TagID, u8& out_rType, universal_value_t& out_rValue)
{
   bool l_bResult = true;

   switch(in_u32TagID)
   {
   case TAG_ERROR:
      out_rType = IVT_U8;
      out_rValue.m_u8Value = 0;
      break;

   default:
      {
         l_bResult = false;

         size_t l_stIndex = GetTagBase(in_u32TagID);
         if(l_stIndex != ~0)
         {
            size_t l_stComm = GetTagComm(in_u32TagID);
            if(l_stComm != ~0)
            {
               switch(l_stComm)
               {
                  case TC_VALUE:
                     out_rType = IVT_U8;
                     out_rValue.m_u8Value = g_aDimmers[l_stIndex].m_u8Current;//*g_aDimmers[l_stIndex].m_pRegister;
                     break;
/*
                  case TC_MIN:
                     out_rType = IVT_U8;
                     out_rValue.m_u8Value = g_NVRAMData.m_aMin[l_stIndex];
                     break;
                  case TC_MAX:
                     out_rType = IVT_U8;
                     out_rValue.m_u8Value = g_NVRAMData.m_aMax[l_stIndex];
                     break;
*/
                  case TC_STATUS:
                     out_rType = IVT_NONE;
                     out_rValue.m_u8Value = 0;
                     break;
                  default:
                     out_rType = IVT_NONE;
                     out_rValue.m_u8Value = 0;
                     break;
               }
               l_bResult = true;
            }
         }
         break;
      }
   }
   return l_bResult;
}

/**
   Инициализация энергонезависимой памяти
   на входе    :  *
   на выходе   :  указатель на данные энергонезависимой памяти
*//*
nvram_common_t* iRidiumDevice_InitNVRAM()
{
   // Установка интерфейса и адреса
   g_NVRAM.SetInterface(&hi2c2);
   g_NVRAM.SetAddress(0xA0);

   // Размер памяти 4кб, размер страницы 32 байта 
   g_NVRAM.Setup(4096, 32);

   // Установка флеша
   //g_NVRAM.SetFlashRange(EEPROM_START, EEPROM_END);

   // Установка кеширования
   g_NVRAM.SetCache(&g_NVRAMData, sizeof(g_NVRAMData));
   g_NVRAM.SetUpdateTime(500);

   // Инициализация и чтение нужного банка энергонезависимой памяти
   return (g_NVRAM.Init() < 0) ? NULL : (nvram_common_t*)&g_NVRAMData;
}*/

/**
   Вызов настройки устройства
   на входе    :  *
   на выходе   :  *
*/
void iRidiumDevice_Setup()
{
   // Включение режима работы
   g_Indicator.SetMode(INDICATOR_MODE_WORK);

   // Установка интерфейса и адреса
   g_NVRAM.SetInterface(&hi2c2);
   g_NVRAM.SetAddress(0xA0);

   // Размер памяти 4кб, размер страницы 32 байта
   g_NVRAM.Setup(4096, 32);

   // Установка флеша
   //g_NVRAM.SetFlashRange(EEPROM_START, EEPROM_END);

   // Установка кеширования
   g_NVRAM.SetCache(&g_NVRAMData, sizeof(g_NVRAMData));
   g_NVRAM.SetUpdateTime(500);

   // Инициализация и чтение нужного банка энергонезависимой памяти
   if(g_NVRAM.Init() < 0)
   {
      // Индикация ошибки: неисправность энергонезависимой памяти
      g_Indicator.SetMode(INDICATOR_MODE_1_1);
   }

   // Инициализация устройства
   g_Device.Setup();
}

/**
   Вызов основного цикла устройства
   на входе    :  *
   на выходе   :  *
*/
void iRidiumDevice_Loop()
{
   // Работа с индикатором
   g_Indicator.Work();
   // Работа с энергонезависимой памятью
   g_NVRAM.Work();
   // Обработка устройства
   g_Device.Loop();

}
