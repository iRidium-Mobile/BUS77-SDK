#ifndef _API_H_INCLUDED_
#define _API_H_INCLUDED_

#include "IridiumTypes.h"
#include "IridiumValue.h"

#define CF_W   0x40                                // Признак изменения канала по PIN коду
#define CF_R   0x20                                // Признак чтения канала по PIN коду

// Данные канала обратной связи
typedef struct device_tag_s
{
   u32         m_u32ID;                            // Идентификатор канала
   const char* m_pszName;                          // Указатель на имя канала
   u8          m_u8Type;                           // Тип значения канала обратной связи
   const void* m_pRange;                           // Указатель на минимальное, максимальное и шаговое значение
   const char* m_pszDesc;                          // Указатель на описание канала
} device_tag_t;

// Данные канала управления
typedef struct device_channel_s
{
   u32         m_u32ID;                            // Идентификатор канала
   const char* m_pszName;                          // Указатель на имя канала
   u8          m_u8Type;                           // Тип значения канала обратной связи
   const void* m_pRange;                           // Указатель на минимальное, максимальное и шаговое значение
   const char* m_pszDesc;                          // указатель на описание канала
   u8          m_u8Flags;                          // Список флагов
   u8          m_u8MaxVariables;                   // Максимальное количество глобальных переменных на канал управления
} device_channel_t;

// Значения пределов и шага / Value of limits and step
const bool  g_aRangeBool[]       =        {false,  	true,       true  			};
const u8    g_aRangeU8[]         =        {0,      	0xFF,       1     			};
const u16   g_aRangeU16[]        =        {0,      	0xFFFF,     1     			};
const u32   g_aRangeU32[]        =        {0,      	0xFFFFFFFF, 1     			};
const u32   g_aRangePin[]        =        {0,      	0xFFFFFFFE, 1     			};
const u8   	g_aRangeName[]       =        {0,      	64,         1     			};
const u8    g_aRangeVoltage[]    =        {0,      	30,         1     			};
const f32   g_aRangeCoreTemp[]   =        {-50,    	100,        1     			};
const u64   g_aRangeU64[]      	=        {0,      	0xFFFFFFFFFFFFFFFF,    1   };                         //0xF4240 = 1 000 000
const bool  g_aRangeNone[]       =        {0,      	0,          0     			};
const f32   g_aRangeDS18B20[]    =        {-50, 		150, 			0.000001 		};
const u8		g_aRangeTSMode[]		=			{0,			3,				1					};
const u8		g_aRangeTS_THtype[]	=			{0,			1,				1					};
const u8		g_aRangeTDSMode[]		=			{0,			3,				1					};
const u8		g_aRangeRScenario[]	=			{0,			2,				1					};
const u8		g_aRangePercents[]	=			{0,			100,			1					};
const u8		g_aRangeSmoothTime[]	=			{0,			120,			1					};
const u32   g_aRangeDeadBand[]	=			{0,			200,			1					};

enum eChannels
{
   // Ststem
   SYSTEM_CHANNEL_NAME = 1,               // Имя устройства / Device name
   SYSTEM_CHANNEL_PIN,                    // PIN код / Device PIN
   SYSTEM_CHANNEL_REBOOT,                 // Перезагрузка / Device reboot
   SYSTEM_CHANNEL_RESET,                  // Сброс контроллера
   // Hardware
   //CHANNEL_FINDE_ME,                      // Управление набортным LED / Onboard LED direction
   CHANNEL_DEADBAND,
	// LED 1
	CHANNEL_LED_1_POWER_SET,					// Включить/выключить работу ленты
	CHANNEL_LED_1_POWER_ON,						// Включить работу ленты
	CHANNEL_LED_1_POWER_OFF,					// Выключить работу ленты
	CHANNEL_LED_1_SMOOTH_SET,					// Плавность перехода
	CHANNEL_LED_1_SET_STEP,                // Шаг переключения яркости
	CHANNEL_LED_1_INCREASE,                // Инкремент яркости на установленый шаг
	CHANNEL_LED_1_DECREASE,                // Декремент яркости на установленый шаг
	CHANNEL_LED_1_FLICKER,                 // Порог мерцания ленты
	CHANNEL_LED_1_SET_BRIGHTNES,           // Установить яркость
   CHANNEL_LED_1_DIMMING_UP,              // Начать плавное диммирование яркости вверх
   CHANNEL_LED_1_DIMMING_DOWN,            // Начать плавное диммиррование вверх
   CHANNEL_LED_1_DIMMING_STOP,            // Остановить диммирование
   CHANNEL_LED_1_SMOOTH_DIMMING,          // Скорость изменения яркости при диммировании
   // LED 2
	CHANNEL_LED_2_POWER_SET,					// Включить/выключить работу ленты
	CHANNEL_LED_2_POWER_ON,						// Включить работу ленты
	CHANNEL_LED_2_POWER_OFF,					// Выключить работу ленты
	CHANNEL_LED_2_SMOOTH_SET,					// Плавность перехода
	CHANNEL_LED_2_SET_STEP,                // Шаг переключения яркости
	CHANNEL_LED_2_INCREASE,                // Инкремент яркости на установленый шаг
	CHANNEL_LED_2_DECREASE,                // Декремент яркости на установленый шаг
	CHANNEL_LED_2_FLICKER,                 // Порог мерцания ленты
	CHANNEL_LED_2_SET_BRIGHTNES,           // Установить яркость
   CHANNEL_LED_2_DIMMING_UP,              // Начать плавное диммирование яркости вверх
   CHANNEL_LED_2_DIMMING_DOWN,            // Начать плавное диммиррование вверх
   CHANNEL_LED_2_DIMMING_STOP,            // Остановить диммирование
   CHANNEL_LED_2_SMOOTH_DIMMING,          // Скорость изменения яркости при диммировании
   // LED 3
	CHANNEL_LED_3_POWER_SET,					// Включить/выключить работу ленты
	CHANNEL_LED_3_POWER_ON,						// Включить работу ленты
	CHANNEL_LED_3_POWER_OFF,					// Выключить работу ленты
	CHANNEL_LED_3_SMOOTH_SET,					// Плавность перехода
	CHANNEL_LED_3_SET_STEP,                // Шаг переключения яркости
	CHANNEL_LED_3_INCREASE,                // Инкремент яркости на установленый шаг
	CHANNEL_LED_3_DECREASE,                // Декремент яркости на установленый шаг
	CHANNEL_LED_3_FLICKER,                 // Порог мерцания ленты
	CHANNEL_LED_3_SET_BRIGHTNES,           // Установить яркость
   CHANNEL_LED_3_DIMMING_UP,              // Начать плавное диммирование яркости вверх
   CHANNEL_LED_3_DIMMING_DOWN,            // Начать плавное диммиррование вверх
   CHANNEL_LED_3_DIMMING_STOP,            // Остановить диммирование
   CHANNEL_LED_3_SMOOTH_DIMMING,          // Скорость изменения яркости при диммировании
   // LED 4
	CHANNEL_LED_4_POWER_SET,					// Включить/выключить работу ленты
	CHANNEL_LED_4_POWER_ON,						// Включить работу ленты
	CHANNEL_LED_4_POWER_OFF,					// Выключить работу ленты
	CHANNEL_LED_4_SMOOTH_SET,					// Плавность перехода
	CHANNEL_LED_4_SET_STEP,                // Шаг переключения яркости
	CHANNEL_LED_4_INCREASE,                // Инкремент яркости на установленый шаг
	CHANNEL_LED_4_DECREASE,                // Декремент яркости на установленый шаг
	CHANNEL_LED_4_FLICKER,                 // Порог мерцания ленты
	CHANNEL_LED_4_SET_BRIGHTNES,           // Установить яркость
   CHANNEL_LED_4_DIMMING_UP,              // Начать плавное диммирование яркости вверх
   CHANNEL_LED_4_DIMMING_DOWN,            // Начать плавное диммиррование вверх
   CHANNEL_LED_4_DIMMING_STOP,            // Остановить диммирование
   CHANNEL_LED_4_SMOOTH_DIMMING,          // Скорость изменения яркости при диммировании
   // LED 5
	CHANNEL_LED_5_POWER_SET,					// Включить/выключить работу ленты
	CHANNEL_LED_5_POWER_ON,						// Включить работу ленты
	CHANNEL_LED_5_POWER_OFF,					// Выключить работу ленты
	CHANNEL_LED_5_SMOOTH_SET,					// Плавность перехода
	CHANNEL_LED_5_SET_STEP,                // Шаг переключения яркости
	CHANNEL_LED_5_INCREASE,                // Инкремент яркости на установленый шаг
	CHANNEL_LED_5_DECREASE,                // Декремент яркости на установленый шаг
	CHANNEL_LED_5_FLICKER,                 // Порог мерцания ленты
	CHANNEL_LED_5_SET_BRIGHTNES,           // Установить яркость
   CHANNEL_LED_5_DIMMING_UP,              // Начать плавное диммирование яркости вверх
   CHANNEL_LED_5_DIMMING_DOWN,            // Начать плавное диммиррование вверх
   CHANNEL_LED_5_DIMMING_STOP,            // Остановить диммирование
   CHANNEL_LED_5_SMOOTH_DIMMING,          // Скорость изменения яркости при диммировании
   // LED 6
	CHANNEL_LED_6_POWER_SET,					// Включить/выключить работу ленты
	CHANNEL_LED_6_POWER_ON,						// Включить работу ленты
	CHANNEL_LED_6_POWER_OFF,					// Выключить работу ленты
	CHANNEL_LED_6_SMOOTH_SET,					// Плавность перехода
	CHANNEL_LED_6_SET_STEP,                // Шаг переключения яркости
	CHANNEL_LED_6_INCREASE,                // Инкремент яркости на установленый шаг
	CHANNEL_LED_6_DECREASE,                // Декремент яркости на установленый шаг
	CHANNEL_LED_6_FLICKER,                 // Порог мерцания ленты
	CHANNEL_LED_6_SET_BRIGHTNES,           // Установить яркость
   CHANNEL_LED_6_DIMMING_UP,              // Начать плавное диммирование яркости вверх
   CHANNEL_LED_6_DIMMING_DOWN,            // Начать плавное диммиррование вверх
   CHANNEL_LED_6_DIMMING_STOP,            // Остановить диммирование
   CHANNEL_LED_6_SMOOTH_DIMMING,          // Скорость изменения яркости при диммировании
		LAST_CHANNEL
};

// Sys
const char g_pszChReboot[]     			= "Reboot";
const char g_pszChReset[]        		= "Reset";
const char g_pszChName[]       			= "Name";
const char g_pszChPIN[]        			= "PIN";
// HW
const char g_pszChLED[]          		= "Find device";
const char g_pszChDeadband[]           = "Deadband";
// LED
const char g_pszChPowerSet[]				= "LED %d power set";
const char g_pszChPowerOn[]				= "LED %d power on";
const char g_pszChPowerOff[]				= "LED %d power off";
const char g_pszChSmooth[]					= "LED %d smooth set";
const char g_pszChDimmingSmooth[]      = "Time changes in brightness %d LED";

const char g_pszChSetStep[]				= "LED %d set step";
const char g_pszChIncrease[]				= "LED %d increase brightness";
const char g_pszChDecrease[]				= "LED %d decrease brightness";
const char g_pszChFlickThreshold[]		= "LED %d flicker threshold";
const char g_pszChSetBrightnes[]			= "LED %d set brightness";
const char g_pszChDimmingUp[]          = "LED %d start dimming up";
const char g_pszChDimmingDown[]        = "LED %d start dimming down";
const char g_pszChDimmingStop[]        = "LED %d dimming stop";
// Текстовое описание канала управления / Text discription of control channel
const char g_pszChDReboot[]      		= "Reboot device";
const char g_pszChDReset[]      			= "Return to default settings";
const char g_pszChDFindMe[]      		= "Blinked onboard LED";
const char g_pszChDDefolt[]				= "";
const char g_pszChDName[]      			= "32 symbol";
const char g_pszChDTime[]      			= "sec";
const char g_pszChDBool[]	   			= "true/false";
const char g_pszChDPIN[]       			= "Personal identification number";
const char g_pszChDSetStep[]				= "Percents";
const char g_pszChDDeadband[]          = "°C";

// Идентификаторы каналов обратной связи / Feedback channel identifiers
enum eTag
{
   // Общие / Common 
   TAG_ERROR = 1,									// Error code message
   TAG_HW_COMM_VOL,								// Device incoming voltage
	TAG_HW_COMM_TEMP,
	// LED 1
	TAG_LED_1_BRIGHTNES,
	TAG_LED_1_POWER_STATE,
	// LED 2
	TAG_LED_2_BRIGHTNES,
	TAG_LED_2_POWER_STATE,
   // LED 3
	TAG_LED_3_BRIGHTNES,
	TAG_LED_3_POWER_STATE,
   // LED 4
	TAG_LED_4_BRIGHTNES,
	TAG_LED_4_POWER_STATE,
   // LED 5
	TAG_LED_5_BRIGHTNES,
	TAG_LED_5_POWER_STATE,
   // LED 6
	TAG_LED_6_BRIGHTNES,
	TAG_LED_6_POWER_STATE,
         LAST_TAG
};

// Sys
const char g_pszTgError[]              = "Error code";
const char g_pszTgVolageCommon[]     	= "Device voltage";
const char g_pszTgTemperCore[]     		= "Device temperature";
//const char g_pszTgColor[]     			= "RGBLED %d color";
const char g_pszTgPowerState[]     		= "LED %d power state";
const char g_pszTgPowerBrightness[]    = "LED %d brightness";


// Текстовое описание каналов обратной связи / Text discription of feedback channel                                                                                                 
const char g_pszTgDDefolt[]				= "";
const char g_pszTgDVol[]			      = "Volt";
const char g_pszTgDResistance[]			= "ohm";
const char g_pszTgDBool[]					= "true/false";
const char g_pszTgDTemper[]				= "C°";
const char g_pszTgDError[]				   = "error";
const char g_pszTgDMode[]        		= "0 - Normal, 1 - Night, 2 - Away, 3 - Day";
const char g_pszTgDModeReley[]			= "0-common 1-automat";									         // "true - auto, false - manual"
const char g_pszTgDType[]					= "0-NOpen, 1-NClose";			                           // " 0 -normally open, 1 - normally close"

#endif   // _API_H_INCLUDED_
