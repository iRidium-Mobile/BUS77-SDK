//////////////////////////////////////////////////////////////////////////
// class CDevice
//////////////////////////////////////////////////////////////////////////
// Включения
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "Main.h"
#include "stdlib.h"

// iRidium device
#include "CDevice.h"

// iRidium protocol
#include "IridiumBytes.h"
#include "IridiumCRC16.h"
#include "CIridiumStreebog.h"

// Common
#include "CCanPort.h"
#include "EEPROM.h"
#include "MemoryMap.h"
#include "InputOutput.h"
#include "UTF8.h"

#define MAX_DEVICE_CHANNELS            4           // Максимальное количество каналов управления
#define MAX_DEVICE_TAGS                2           // Максимальное количество каналов обратной связи

#define MAX_VARIABLES                  16          // Максимальное количество глобальных переменных на канал управления

#define FIRMWARE_READ_STREAM_ID        1           // Идентификатор потока чтения прошивки
#define FIRMWARE_WRITE_STREAM_ID       2           // Идентификатор потока записи прошивки

#define MAX_SAVE_CHANNELS              1           // Максимальное количество сохраняемых каналов
#define START_SAVE_CHANNEL_INDEX       3           // Начальный индекс сохраняемого канала управления
#define END_SAVE_CHANNEL_INDEX         4           // Конечный индекс сохраняемого канаоа управления

#define MAX_SAVE_TAGS                  MAX_DEVICE_TAGS   // Максимальное количество дискретных каналов

#define MAX_INPUTS                     0           // Максимальное количество дискретных каналов


///////////////////////////////////////////////////////////////////////////////
// Информация о каналах управления
///////////////////////////////////////////////////////////////////////////////
// Идентификаторы каналов управления
enum eChannels
{
   CHANNEL_NAME = 1,                               // Имя устройства
   CHANNEL_PIN,                                    // Пин код устройства
   CHANNEL_REBOOT,                                 // Перезагрузка устройства
   CHANNEL_TEST,                                   // Тестовый канал
};

// Имена каналов управления
const char g_pszChannelName[]    = "Имя устройства";
const char g_pszChannelPIN[]     = "PIN код";
const char g_pszChannelReboot[]  = "Перезагрузка устройства";
const char g_pszChannelTest[]    = "Тестовый канал";

// Текстовое описание каналов обратной связи
const char g_pszChannelDescriptionName[]  = "32 байта(16 сим. латин(кирил)ицей)";
const char g_pszChannelDescriptionPIN[]   = "Персональное идентификационное число";

// Массив с описанием канала управления
const device_channel_t g_aDeviceChannels[MAX_DEVICE_CHANNELS] =
{
   {  CHANNEL_NAME,     (char*)g_pszChannelName,   IVT_STRING8,   0,       32,         1,       (char*)g_pszChannelDescriptionName, CF_W,          0              },
   {  CHANNEL_PIN,      (char*)g_pszChannelPIN,    IVT_U32,       0,       0xFFFFFFFF, 1,       (char*)g_pszChannelDescriptionPIN,  CF_W | CF_R,   0              },
   {  CHANNEL_REBOOT,   (char*)g_pszChannelReboot, IVT_BOOL,      false,   true,       false,   (char*)g_pszChannelReboot,          CF_W,          0              },
   {  CHANNEL_TEST,     (char*)g_pszChannelTest,   IVT_U8,        0,       100,        1,       (char*)g_pszChannelTest,            CF_W,          MAX_VARIABLES  },
};

// Массив определяющий связь между каналом обратной связи и глобальной переменной
u16 g_aChannelsVariable[MAX_DEVICE_CHANNELS][MAX_VARIABLES];

///////////////////////////////////////////////////////////////////////////////
// Информация о каналах обратной связи
///////////////////////////////////////////////////////////////////////////////
// Идентификаторы каналов обратной связи
enum eTags
{
   TAG_ERROR = 1,                                  // Код ошибки
   TAG_TEST,                                       // Тестовый канал
};

// Имена каналов обратной связи
const char g_pszTagError[]    = "Код ошибки";
const char g_pszTagTest[]     = "Тестовые каналы";

// Текстовое описание каналов обратной связи
const char g_pszTagDescriptionError[]  = "Код ошибки";
const char g_pszTagDescriptionTest[]   = "Тестовые каналы";

// Массив с описанием канала управления
const device_tag_t g_aDeviceTags[MAX_DEVICE_TAGS] =
{
   {  TAG_ERROR,  (char*)g_pszTagError,   IVT_U8,     0,          255,              1,          (char*)g_pszTagDescriptionError, CF_W  },
   {  TAG_TEST,   (char*)g_pszTagTest,    IVT_U8,     0,          255,              1,          (char*)g_pszTagDescriptionTest,  CF_W  },
};

// Массив определяющий связь между каналом обратной связи и глобальными переменными
u16 g_aTagsVariable[MAX_SAVE_TAGS];

// Массив флагов определяющих владение глобальной переменной
u8 g_aTagOwners[(MAX_SAVE_TAGS + 7) / 8];

u8 g_u8Test = 0;

#pragma pack(push, 1)

// Описание канала управления/обратной связи
typedef struct smart_api_channel_s
{
   u32   m_u32ID;                                  // Идентификатор канала управления/обратной связи устройства
   u32   m_u32Type;                                // Смарт тип канала
} smart_api_channel_t;

// Описание смарт устройства
typedef struct smart_api_device_s
{
   u32                  m_u32SmartID;              // Идентификатор смарт устройства
   u32                  m_u32Tags;                 // Количество каналов обратной связи смарт устройства
   u32                  m_u32Channels;             // Количество каналов управления смарт устройства
} smart_api_device_t;

// Структура кнопки
typedef struct wb_smart_button_s
{
   smart_api_device_t   m_Header;   
   smart_api_channel_t  m_Data[4];
} wb_smart_button_t;

// Структура реле
typedef struct wb_smart_relay_s
{
   smart_api_device_t   m_Header;   
   smart_api_channel_t  m_Data[2];
} wb_smart_relay_t;

// Структура реле
typedef struct wb_smart_device_s
{
   u16                  m_u16Count;
   wb_smart_button_t    m_Button[7];
   wb_smart_relay_t     m_Relay[6];
} wb_smart_device_t;

#pragma pack(pop)

/*
// Смарт описание устройства
wb_smart_device_t    g_SmartAPI =
{
   13,
   {
      // Onboard Button
      {
         { 10, 2, 2 },
         {
            { 1, 17}, { 2, 92 },
            { CHANNEL_MODE_ONBOARD, 0  }, { CHANNEL_TIME_ONBOARD, 0  }
         }
      }, 
      // Button 1
      {
         { 10, 2, 2 },
         {
            { 3, 17}, { 4, 92 },
            { CHANNEL_MODE_1, 0  }, { CHANNEL_TIME_1, 0  }
         }
      }, 
      // Button 2
      {
         { 10, 2, 2 },
         {
            { 5, 17}, { 6, 92 },
            { CHANNEL_MODE_2, 0  }, { CHANNEL_TIME_2, 0  }
         }
      }, 
      // Button 3
      {
         { 10, 2, 2 },
         {
            { 7, 17}, { 8, 92 },
            { CHANNEL_MODE_3, 0  }, { CHANNEL_TIME_3, 0  }
         }
      }, 
      // Button 4
      {
         { 10, 2, 2 },
         {
            { 9, 17}, { 10, 92 },
            { CHANNEL_MODE_4, 0  }, { CHANNEL_TIME_4, 0  }
         }
      }, 
      // Button 5
      {
         { 10, 2, 2 },
         {
            { 11, 17}, { 12, 92 },
            { CHANNEL_MODE_5, 0  }, { CHANNEL_TIME_5, 0  }
         }
      }, 
      // Button 6
      {
         { 10, 2, 2 },
         {
            { 13, 17}, { 14, 92 },
            { CHANNEL_MODE_6, 0  }, { CHANNEL_TIME_6, 0  }
         }
      }
   },
   {
      // Relay 1
      {
         { 2, 1, 1 },
         {
            { 15, 1 },
            { CHANNEL_RELAY_1, 1  }
         }
      }, 
      // Relay 2
      {
         { 2, 1, 1 },
         {
            { 16, 1 },
            { CHANNEL_RELAY_2, 1  }
         }
      }, 
      // Relay 3
      {
         { 2, 1, 1 },
         {
            { 17, 1 },
            { CHANNEL_RELAY_3, 1  }
         }
      }, 
      // Relay 4
      {
         { 2, 1, 1 },
         {
            { 18, 1 },
            { CHANNEL_RELAY_4, 1  }
         }
      }, 
      // Relay 5
      {
         { 2, 1, 1 },
         {
            { 19, 1 },
            { CHANNEL_RELAY_5, 1  }
         }
      }, 
   }
};
*/

// Для работы с временем
volatile uint32_t g_u32TickPerUs    = HAL_RCC_GetHCLKFreq() / 1000000;  // Количество тиков в 1 микросекунде

CDevice                 g_Device;                  // Данные объекта
CIridiumBusInBuffer     g_InBuffer;                // Входящий буфер для приема и обработки собщений
CIridiumBusInBuffer     g_MessageBuffer;           // Входящий буфер для приема и обработки собщений
CIridiumBusOutBuffer    g_OutBuffer;               // Исходящий буфер 
u8                      g_aOutBuffer[IRIDIUM_BUS_OUT_BUFFER_SIZE];

// Указатель на данные HAL CAN порта
extern CAN_HandleTypeDef   hcan;

static CanTxMsgTypeDef  g_aCanTxMessage;           // Структура для отправляемого сообщения
static CanRxMsgTypeDef  g_aCanRxMessage;           // Структура для принимаемого сообщения

// Параметры CAN порта для сборки и разборки пакетов
CCANPort                g_ExtCAN;                  // Данные внешнего CAN порта
can_frame_t             g_aCANInBuffer[256];       // Массив для приема и сборки CAN пакетов
can_frame_t             g_aCANOutBuffer[33*8];     // Массив для отправки CAN пакетов
u16                     g_u16CANID = 0;            // Идентификатор CAN

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

/////////////////////////////////////////////////////////////////////////////////////
// Энергонезависимая память // переменные, которые нужно сохранить, дабавляем сюда //
/////////////////////////////////////////////////////////////////////////////////////
typedef struct eeprom_device_s
{
   // Обшая структура данных (для загрузчика и прошивки)
   eeprom_common_t   m_Common;
   // Список битов признаков владения переменными
   u8                m_aOwners[((sizeof(g_aDeviceTags) / sizeof(g_aDeviceTags[0])) + 7) / 8];
   // Массив глобальных переменных связанных с каналами управления
   u16               m_aChannelVariables[MAX_SAVE_CHANNELS][MAX_VARIABLES];
   // Массив глобальных переменных связанных с каналами обратной связи
   u16               m_aTagVariables[sizeof(g_aDeviceTags) / sizeof(g_aDeviceTags[0])];

} eeprom_device_t;

// Данные устройства
eeprom_device_t   g_EEPROM;

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
   g_EEPROM.m_Common.m_szDeviceName,
   (char*)g_pszProducer,
   (char*)g_pszModelName,
   (char*)g_pszHWID,
   DCT_MICROCONTROLLER,
   PT_ARM,
   OST_NONE,
   IRIDIUM_DEVICE_FLAG_FIRMWARE | IRIDIUM_DEVICE_FLAG_ACTUATOR,
   0,
   { 0, 0, 0 },
   0,
   0
};

/**
   Получение размера значения
   на входе    :  in_u8Type   - тип значения
                  in_rValue   
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
         if(in_rValue.m_u8Value)
            l_stResult += 1;
         break;
      case IVT_S8:
         if(in_rValue.m_s8Value)
            l_stResult += 1;
         break;
      case IVT_U16:
         if(in_rValue.m_u16Value)
            l_stResult += 2;
      case IVT_S16:
         if(in_rValue.m_s16Value)
            l_stResult += 2;
         break;
      case IVT_U32:
         if(in_rValue.m_u32Value)
            l_stResult += 4;
         break;
      case IVT_S32:
         if(in_rValue.m_s32Value)
            l_stResult += 4;
         break;
      case IVT_F32:
         if(in_rValue.m_f32Value != 0.0f)
            l_stResult += 4;
         break;
      case IVT_U64:
         if(in_rValue.m_u64Value)
            l_stResult += 8;
         break;
      case IVT_S64:
         if(in_rValue.m_s64Value)
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
   Получение указателя на порт по хэндлеру CAN порта
   на входе    :  in_pCanHandle  - указатель на хэндлер порта
   на выходе   :  указатель на данные порта
*/
CCANPort* GetCANPort(CAN_HandleTypeDef* in_pCanHandle)
{
   CCANPort* l_pResult = NULL;

   // Проверка на нужный порт
   if(in_pCanHandle->Instance == CAN1)
      l_pResult = &g_ExtCAN;
   
   return l_pResult;
}

/**
   Блокирование доступа к CAN порту
   на входе    :  in_pCanHandle  - указатель на хэндлер порта
   на выходе   :  *
*/
void LockCANPort(CAN_HandleTypeDef* in_pCanHandle)
{
   // Проверка на нужный порт
   if(in_pCanHandle->Instance == CAN1)
   {
      // Выключение прерываний CAN 1 на время сдвига буфера во избежании потери данных
      HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
      HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
   }
}

/**
   Блокирование доступа к CAN порту
   на входе    :  in_pCanHandle  - указатель на хэндлер порта
   на выходе   :  *
*/
void UnLockCANPort(CAN_HandleTypeDef* in_pCanHandle)
{
   // Проверка на нужный порт
   if(in_pCanHandle->Instance == CAN1)
   {
      // Включение прерываний CAN 1
      HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
      HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
   }
}

/**
   Отправка прерывания приема пакета с CAN
   на входе    :  in_pCanHandle - указатель на структуру CAN
   на выходе   :  *
*/
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* in_pCanHandle)
{
   can_frame_t l_Frame;

   // Получение указателя на CAN порт
   CCANPort* l_pPort = GetCANPort(in_pCanHandle);
   if(l_pPort)
   {
      // Извлечение полезной информации
      l_Frame.m_u32ExtID   = in_pCanHandle->pRxMsg->ExtId;
      l_Frame.m_u8Size     = in_pCanHandle->pRxMsg->DLC;

      // Скопируем полезную нагрузку
      memcpy(l_Frame.m_aData, in_pCanHandle->pRxMsg->Data, l_Frame.m_u8Size);

      // Добавление полученого фрейма в буфер
      l_pPort->AddFrame(&l_Frame);

      // Включим прерыване обратно
      HAL_CAN_Receive_IT(in_pCanHandle, CAN_FIFO0);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//// Работа с шиной
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Инициализация шины
   на входе    :  *
   на выходе   :  *
   примечание  :  микроконтроллер stm32f205vgt5 имеет два CAN порта. При этом CAN1 это master, а CAN2 это slave
                  это означает что CAN1 и CAN2 имеют блок фильтров один на двоих, то есть от 0 до 13 это фильтры
                  для CAN1, а фильтры от 14 по 27 это фильтры для CAN2
*/
void BUS_Init()
{
   // Настройка внешней шины
   hcan.pRxMsg = &g_aCanRxMessage;
   hcan.pTxMsg = &g_aCanTxMessage;

   // Установка фильтра CAN1, прием всех сообщений
   CAN_FilterConfTypeDef CAN_FilterInitStructure;
   CAN_FilterInitStructure.FilterIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterIdLow = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdLow = 0x0000;
   CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FIFO0;
   CAN_FilterInitStructure.FilterNumber = 0;
   CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
   CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;
   CAN_FilterInitStructure.FilterActivation = ENABLE;
   CAN_FilterInitStructure.BankNumber = 14;                 // Важно заполнить это поле
   HAL_CAN_ConfigFilter(&hcan, &CAN_FilterInitStructure);

   HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);

   g_ExtCAN.SetCANID(0);
   g_ExtCAN.SetTID(0x00);
   g_ExtCAN.SetAddress(0);
   g_ExtCAN.SetInBuffer(g_aCANInBuffer, sizeof(g_aCANInBuffer));
   g_ExtCAN.SetOutBuffer(g_aCANOutBuffer, sizeof(g_aCANOutBuffer));
}

/**
   Инициализация фильтров шины
   на входе    :  in_u16CanID    - идентификатор CAN шины
                  in_u8Address   - адрес внешней шины
   на выходе   :  *
   примечание  :  параметры фильтра
                  [               3 байт ][               2 байт ][               2 байт ][               2 байт ]
                   31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
                  [X][X][X][I][I][I][I][I][I][I][I][I][I][I][I][I][I][I][I][T][T][T][B][A][A][A][A][A][A][A][A][E]
                  где   :
                  X  - Резерв, должно быть равно 0
                  I  - Идентификатор CAN устройства
                  T  - Идентификатор транзакции
                  B  - Признак широковещательного фрейма (0 - адресный фрейм, 1 - широковещательный фрейм)
                  A  - Адрес кому предназначен фрейм, если признак широковещательного фрейма равен 1, значение должно быть равно 0
                  E  - Признак замыкающего фрейма
*/
void BUS_SetFilter(u16 in_u16CanID, u8 in_u8Address)
{
   // Установка идентификатора
   g_ExtCAN.SetCANID(in_u16CanID);
   g_ExtCAN.SetAddress(in_u8Address);
   
   // Настройка фильтра 0 (CAN1) для приема широковещательных фреймов
   // Маска          :  1 00000000 0   маска на 10 бит
   // Идентификатор  :  1 00000000 0   10 бит включен (признак широковещательного фрейма)
   CAN_FilterConfTypeDef CAN_FilterInitStructure;
   CAN_FilterInitStructure.FilterIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterIdLow = 0x0200 << 3;
   CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdLow = 0x0200 << 3;
   CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FIFO0;
   CAN_FilterInitStructure.FilterNumber = 0;
   CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
   CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;
   CAN_FilterInitStructure.FilterActivation = ENABLE;
   CAN_FilterInitStructure.BankNumber = 0;        // Важно заполнить это поле
   HAL_CAN_ConfigFilter(&hcan, &CAN_FilterInitStructure);
   
   // Настройка фильтра 1 (CAN1) для приема адресного фрейма
   // Маска          :  1 11111111 0   маска на 10 бит
   // Идентификатор  :  0 AAAAAAAA 0   10 бит выключен (признак адресного фрейма), биты от 9-1 адрес
   CAN_FilterInitStructure.FilterIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterIdLow = in_u8Address << (3 + 1);
   CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdLow = 0x03FE << 3;
   CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FIFO0;
   CAN_FilterInitStructure.FilterNumber = 1;
   CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
   CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;
   CAN_FilterInitStructure.FilterActivation = ENABLE;
   CAN_FilterInitStructure.BankNumber = 0;        // Важно заполнить это поле
   HAL_CAN_ConfigFilter(&hcan, &CAN_FilterInitStructure);

   HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
}

/**
   Отправка сообщения на шину
   на входе    :  in_bBroadcast  - признак широковещательно пакета
                  in_u8Address   - адрес кому предназначен пакет
                  in_pBuffer     - указатель на отправляемые данные
                  in_stSize      - размер отправляемых данных
   на выходе   :  успешность отправки данных
*/
bool BUS_Write(bool in_bBroadcast, u8 in_u8Address, void* in_pBuffer, size_t in_stSize)
{
   return g_ExtCAN.AddPacket(in_bBroadcast, in_u8Address, in_pBuffer, in_stSize);
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
   while(1)
   {
      // Попробуем отправить буфер в шину
      if(!BUS_Write(in_bBroadcast, in_Address, in_pBuffer, in_stSize))
      {
         // Отправка во внешний CAN порт во время простоя
         WriteToExtCan();
      } else
         break;
   }
   return true;
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
   g_EEPROM.m_Common.m_u8LID = in_u8LID;
   m_Address = g_EEPROM.m_Common.m_u8LID;
   // Изменение фильтра
   BUS_SetFilter(g_u16CANID, m_Address);
   // Запись в энергонезависимую память
   EEPROM_NeedSaveBuffer();
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
   u32 l_u32PIN = g_EEPROM.m_Common.m_u32PIN;

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
            if(l_stIndex != (size_t)-1)
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
   Получение информации об устройстве
   на входе    :  out_rInfo   - ссылка на структуру куда надо поместить данные об устройстве
   на выходе   :  *
*/
bool CDevice::GetSearchInfo(iridium_search_info_t& out_rInfo)
{
   // Заполнение информации об устройстве
   out_rInfo.m_u8Group  = IRIDIUM_GROUP_TYPE_ACTUATOR;
   out_rInfo.m_pszHWID  = (char*)g_pszHWID;
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
#if MAX_SAVE_CHANNELS != 0
   // Проверка параметров
   if(in_u16VariableID)
   {
      // Пройдемся по всем каналам
      for(u8 i = 0; i < MAX_SAVE_CHANNELS; i++)
      {
         for(u8 j = 0; j < MAX_VARIABLES; j++)
         {
            // Если есть связанная глобальная переменная, вызов изменения значения переменной
            if(g_aChannelsVariable[i][j] == in_u16VariableID)
               SetChannelValue(g_aDeviceChannels[START_SAVE_CHANNEL_INDEX + i].m_u32ID, in_u8Type, in_rValue, in_u8Flags & IRIDIUM_FLAGS_END);
         }
      }
   }
#endif
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
      for(u8 i = 0; i < MAX_DEVICE_TAGS; i++)
      {
         // Вычисление индекса и маски
         u8 l_u8Index = i >> 3;
         u8 l_u8Mask = 1 << (i & 0x7);
         // Проверка владеет ли устройство переменной
         if((g_aTagOwners[l_u8Index] & l_u8Mask) && g_aTagsVariable[i] == in_u16VariableID)
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
   return MAX_DEVICE_TAGS;
}

/**
   Получение индекса по идентификатору канала обратной связи
   на входе    :  in_u32TagID - идентификатор канала обратной связи
   на выходе   :  == -1 - идентификатор не найден
                  != -1 - индекс канала обратной связи
*/
size_t CDevice::GetTagIndex(u32 in_u32TagID)
{
   size_t l_stResult = (size_t)-1;
   
   // Обход таблицы каналов обратной связи
   for(size_t i = 0; i < MAX_DEVICE_TAGS; i++)
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
   if(in_stIndex < MAX_DEVICE_TAGS)
   {
      // Копирование описания канала обратной связи в обучную память
      const device_tag_t* l_pTag = &g_aDeviceTags[in_stIndex];
      // Копирование данных
      out_rInfo.m_u32ID    = l_pTag->m_u32ID;
      out_rInfo.m_pszName  = l_pTag->m_pszName;
      out_rInfo.m_u8Type   = l_pTag->m_u8Type;
      
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
   if(l_stIndex != (size_t)-1)
   {
      // Получение индекса и маски в массиве хранения владения
      u8 l_u8Index = l_stIndex >> 3;
      u8 l_u8Mask = 1 << (l_stIndex & 0x7);
      
      // Копирование данных в обычную память
      const device_tag_t* l_pTag = &g_aDeviceTags[l_stIndex];
      // Копируем данные описания канала управления
      out_rDescription.m_ID.m_u8Type = l_pTag->m_u8Type;
      out_rDescription.m_ID.m_Min.m_u64Value = l_pTag->m_u64Min;
      out_rDescription.m_ID.m_Max.m_u64Value = l_pTag->m_u64Max;
      out_rDescription.m_ID.m_Step.m_u64Value = l_pTag->m_u64Step;
      out_rDescription.m_ID.m_pszDescription = l_pTag->m_pszDesc;
      
      // Получение флага владения
      out_rDescription.m_Flags.m_bOwner = (0 != (g_aTagOwners[l_u8Index] & l_u8Mask));
      // Получение переменной
      out_rDescription.m_u16Variable = g_aTagsVariable[l_stIndex];
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
   if(l_stIndex != (size_t)-1)
   {
      u8 l_u8Index = l_stIndex >> 3;
      u8 l_u8Mask = 1 << (l_stIndex & 0x7);
      // Запись глобальной переменной в энергонезависимую память
      g_EEPROM.m_aTagVariables[l_stIndex] = in_u16Variable;
      // Выставим флаг владения
      if(in_bOwner)
         g_EEPROM.m_aOwners[l_u8Index] |= l_u8Mask;
      else
         g_EEPROM.m_aOwners[l_u8Index] &= ~l_u8Mask;
      // Отметим что нужно записать данные в энергонезависимую память
      EEPROM_NeedSaveBuffer();
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
   if(l_stIndex != (size_t)-1)
   {
      // Отправка значения в канал
      ChangeVariable(g_EEPROM.m_aTagVariables[l_stIndex], in_u8Type, in_rValue);
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
   size_t l_stResult = (size_t)-1;

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
      const device_channel_t* l_Channel = &g_aDeviceChannels[in_stIndex];
      
      // Копирование параметров
      out_rInfo.m_u32ID    = l_Channel->m_u32ID;
      out_rInfo.m_pszName  = l_Channel->m_pszName;
      out_rInfo.m_u8Type   = l_Channel->m_u8Type;

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
   if(l_stChannel != (size_t)-1)
   {
      // Копируем данные описания канала
      memcpy(&l_Channel, &g_aDeviceChannels[l_stChannel], sizeof(device_channel_t));

      // Заполнение параметров
      out_rDescription.m_ID.m_u8Type            = l_Channel.m_u8Type;
      out_rDescription.m_ID.m_Min.m_u64Value    = l_Channel.m_u64Min;
      out_rDescription.m_ID.m_Max.m_u64Value    = l_Channel.m_u64Max;
      out_rDescription.m_ID.m_Step.m_u64Value   = l_Channel.m_u64Step;
      out_rDescription.m_ID.m_pszDescription    = l_Channel.m_pszDesc;
      out_rDescription.m_Flags.m_bWritePassword = (0 != (l_Channel.m_u8Flags & CF_W));
      out_rDescription.m_Flags.m_bReadPassword  = (0 != (l_Channel.m_u8Flags & CF_R));
      out_rDescription.m_u8MaxVariables         = l_Channel.m_u8MaxVariables;

      out_rDescription.m_u8Variables = 0;
      out_rDescription.m_pVariables = NULL;

      // Если номер канала больше чем начальный сохраняемый канал
      if(l_stChannel >= START_SAVE_CHANNEL_INDEX)
      {
         l_stChannel -= START_SAVE_CHANNEL_INDEX;
         
         // Если номер канала меньше чем максимальный номер сохраняемого канала
         if(l_stChannel < MAX_SAVE_CHANNELS)
         {
            out_rDescription.m_u8Variables = MAX_VARIABLES;
            out_rDescription.m_pVariables = g_EEPROM.m_aChannelVariables[l_stChannel];
         }
      }
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
   if(l_stIndex != (size_t)-1)
   {
      // Если номер линкуемого канала больше чем начальный сохраняемый канал
      if(l_stIndex >= START_SAVE_CHANNEL_INDEX)
      {
         l_stIndex -= START_SAVE_CHANNEL_INDEX;
         
         // Если номер линкуемого канала меньше чем максимальный номер сохраняемого канала
         if(l_stIndex < MAX_SAVE_CHANNELS)
         {
            // Проверка количества переменных
            if(in_u8Variables > MAX_VARIABLES)
               in_u8Variables = MAX_VARIABLES;
            
            // Запись списка глобальных переменных в энергонезависимую память
            for(u8 i = 0; i < MAX_VARIABLES; i++)
               g_EEPROM.m_aChannelVariables[l_stIndex][i] = (i < in_u8Variables ? in_pVariables[i] : 0);
            EEPROM_NeedSaveBuffer();
            l_bResult = true;
         }
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
   
   switch(in_u32ChannelID)
   {
   // Изменение имени устройства
   case CHANNEL_NAME:
      if(in_u8Type == IVT_STRING8 && in_rValue.m_Array.m_stSize)
      {
         // Сохраним новое имя устройства
         size_t l_stMax = sizeof(g_EEPROM.m_Common.m_szDeviceName);
         size_t l_stSize = (in_rValue.m_Array.m_stSize > l_stMax) ? l_stMax : in_rValue.m_Array.m_stSize;
         memcpy(g_EEPROM.m_Common.m_szDeviceName, in_rValue.m_Array.m_pPtr, l_stSize);
         // Корректировка имени
         //SetValidEndByUTF8(g_szDeviceName, l_stSize, MAX_DEVICE_NAME_SIZE);
         l_bResult = true;
      }
      break;
      
   // Изменение PIN кода
   case CHANNEL_PIN: 
      if(in_u8Type == IVT_U32)
      {
         // Сохраним новое значение PIN
         g_EEPROM.m_Common.m_u32PIN = in_rValue.m_u32Value;
         l_bResult = true;
      }
      break;

   // Изменение Test
   case CHANNEL_TEST: 
      if(in_u8Type == IVT_U8)
      {
         // Сохраним новое значение PIN
         g_u8Test = in_rValue.m_u8Value;
         // Генерация изменения значения 
         size_t l_stIndex = GetTagIndex(TAG_TEST);
         if(l_stIndex != (size_t)-1)
            SendSetVariableRequest(g_aTagsVariable[l_stIndex], in_u8Type, in_rValue);
         l_bResult = true;
      }
      break;
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
         g_EEPROM.m_Common.m_u8Mode = BOOTLOADER_MODE_DOWNLOAD;
         // Запись адреса
         g_EEPROM.m_Common.m_u16FirmwareAddress = GetSrcAddress();
         // Запись идентификатора транзакции
         g_EEPROM.m_Common.m_u16FirmwareTID = m_InMH.m_u16TID;
         EEPROM_ForceSaveBuffer();

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
   if(g_EEPROM.m_Common.m_u8FirmwareID != FWID_02_07)
   {
      // Сброс PIN кода
      g_EEPROM.m_Common.m_u32PIN = 0;
      // Сброс флагов владения
      memset(g_EEPROM.m_aOwners, 0, sizeof(g_EEPROM.m_aOwners));
      // Сброс глобальных переменных связанных с каналами обратной связи
      memset(g_EEPROM.m_aTagVariables, 0, sizeof(g_EEPROM.m_aTagVariables));
      // Сброс глобальных переменных связанных с каналами управления
      memset(g_EEPROM.m_aChannelVariables, 0, sizeof(g_EEPROM.m_aChannelVariables));

      // Запись данных для бутлоадера
      g_EEPROM.m_Common.m_u8FirmwareID = FWID_02_07;
      g_EEPROM.m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
      
      EEPROM_ForceSaveBuffer();
   }
   
   // Загрузка локального идентификатора
   m_Address = g_EEPROM.m_Common.m_u8LID;
   
   // Корректировка имени
   //SetValidEndByUTF8(g_EEPROM.m_Common.m_szDeviceName, sizeof(g_EEPROM.m_Common.m_szDeviceName), sizeof(g_EEPROM.m_Common.m_szDeviceName));
      
   // Генерация идентификатора
   GenerateHWID();
   
   // Инициализация шины
   BUS_Init();
   
   // Установка фильтров
   g_u16CANID = GetCRC16Modbus(1, (u8*)g_pszHWID, sizeof(g_pszHWID));
   BUS_SetFilter(g_u16CANID, m_Address);

   // Выключение набортного светодиода
   HAL_GPIO_WritePin(Onboard_LED_GPIO_Port, Onboard_LED_Pin, GPIO_PIN_SET);
   // Отправка информации об устройстве
   if(GetSearchInfo(l_Search))
      SendSearchResponse(GetTID(), l_Search);
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
   
   // Обработка сохранения изменений
   EEPROM_WorkBuffer();
   
   // Запись во внешний CAN порт
   WriteToExtCan();

   // Чтение и обработка данных с внешнего CAN порта
   ReadFromExtCan();
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
   Обработка внешнего порта
   на входе    :  *
   на выходе   :  *
*/
void CDevice::ReadFromExtCan()
{
   // Поиск пакета в буфере
   void* l_pBuffer = NULL;
   size_t l_stSize = 0;
   bool l_bResult = false;
   
   // Получение указателя на данные порта
   CCANPort* l_pPort = GetCANPort(&hcan);
   if(l_pPort)
   {
      // Чтение входящих сообщений
      HAL_CAN_Receive_IT(&hcan, CAN_FIFO0); 

      // Получение пакета
      l_bResult = l_pPort->GetPacket(l_pBuffer, l_stSize);
      
      // Блокирование доступа к порту
      LockCANPort(&hcan);
      // Удаление обработанных пакетов
      l_pPort->Flush();
      // Разблокирование доступа к порту
      UnLockCANPort(&hcan);
      
      // Проверка наличия данных
      if(l_bResult)
      {
         // Установка данных буфера
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
         l_pPort->DeletePacket();
      }
   }
}

/**
   Обработка внешнего порта
   на входе    :  *
   на выходе   :  *
*/
void CDevice::WriteToExtCan()
{
   // Получение указателя на данные порта
   CCANPort* l_pPort = GetCANPort(&hcan);
   if(l_pPort)
   {
      // Получим фрейм для отправки в CAN
      can_frame_t* l_pPtr = l_pPort->GetFrame();
      if(l_pPtr)
      {
         // Установим расширенный размер кадра
         hcan.pTxMsg->IDE = CAN_ID_EXT;
      
         // Назначаем идентификатор сообщения
         hcan.pTxMsg->ExtId = l_pPtr->m_u32ExtID;
      
         // Указываем длинну сообщения
         hcan.pTxMsg->DLC = l_pPtr->m_u8Size;
      
         // Копируем данные для отправки
         memcpy(hcan.pTxMsg->Data, l_pPtr->m_aData, l_pPtr->m_u8Size);
      
         // Отправляем данные
         if(HAL_CAN_Transmit_IT(&hcan) == HAL_OK)
            l_pPort->DeleteFrame();
      }
   }
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
      case CHANNEL_NAME:
         out_rType = IVT_STRING8;
         out_rValue.m_Array.m_pPtr = g_EEPROM.m_Common.m_szDeviceName;
         out_rValue.m_Array.m_stSize = strlen((char*)out_rValue.m_Array.m_pPtr);
         break;

      // Системный канал с PIN кодом
      case CHANNEL_PIN:
         out_rType = IVT_U32;
         out_rValue.m_u32Value = g_EEPROM.m_Common.m_u32PIN;
         break;

      // Системный канал для перезагрузки контроллера
      case CHANNEL_REBOOT:
         out_rType = IVT_BOOL;
         out_rValue.m_bValue = false;
         break;

      // Системный канал для перезагрузки контроллера
      case CHANNEL_TEST:
         out_rType = IVT_U8;
         out_rValue.m_u8Value = g_u8Test;
         break;

      // Значение не найдено
      default:
         l_bResult = false;
         break;
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

   case TAG_TEST:
      out_rType = IVT_U8;
      out_rValue.m_u8Value = g_u8Test;
      break;

   default:
      l_bResult = false;
      break;
   }

   return l_bResult;
}

/**
   Вызов инициализации устройства
   на входе    :  *
   на выходе   :  *
*/
void iRidiumDevice_InitEEPROM()
{
   // Установка границ энергонезависимой памяти
   EEPROM_Init(EEPROM_START, EEPROM_END);
   
   // Включение буферизации EEPROM
   EEPROM_SetBuffer((u8*)&g_EEPROM, sizeof(g_EEPROM));
}

/**
   Вызов настройки устройства
   на входе    :  *
   на выходе   :  *
*/
void iRidiumDevice_Setup()
{
   g_Device.Setup();
}

/**
   Вызов основного цикла устройства
   на входе    :  *
   на выходе   :  *
*/
void iRidiumDevice_Loop()
{
   g_Device.Loop();
}
