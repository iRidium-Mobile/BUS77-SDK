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
#include "CIridiumCipherGrasshopper.h"

// Common
#include "CCanPort.h"
#include "CFirmware.h"

// MCUCommon
#include "CFlashNVRAM.h"

// ChipCommon
#include "Flash.h"
#include "InputOutput.h"
#include "FLASHMemoryMap.h"

// DeviceCommon
#include "NVRAM.h"

#define MAX_DEVICE_CHANNELS            0           // Максимальное количество каналов управления
#define MAX_DEVICE_TAGS                0           // Максимальное количество каналов обратной связи

#define MAX_VARIABLES                  16          // Максимальное количество глобальных переменных на канал управления

#define FIRMWARE_READ_STREAM_ID        1           // Идентификатор потока чтения прошивки
#define FIRMWARE_WRITE_STREAM_ID       2           // Идентификатор потока записи прошивки

///////////////////////////////////////////////////////////////////////////////
// Информация об устройстве
///////////////////////////////////////////////////////////////////////////////
const char g_pszProducer[]    = "iRidium";
const char g_pszModelName[]   = "Шаблон 1.0";

char g_pszHWID[STREEBOG_HASH_256_BYTES + 1];       // Аппаратный идентификатор шеснадцатеричное представление 128 битного числа + 1 байт

typedef struct nvram_device_s
{
   // Обшая структура данных (для загрузчика и прошивки)
   nvram_common_t m_Common;
   // Резервирование памяти
   u8             m_aReserve[FLASH_PAGE_SIZE - sizeof(nvram_common_t)];
} nvram_device_t;

// Данные энергонезависимой памяти устройства
nvram_device_t g_NVRAMData;

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

// Ключ шифрования и вектор инициализации по умолчанию
const u8 g_aKeyAndIV[] =
{
   // Ключ шифрования
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
   0xf0, 0xe0, 0xd0, 0xc0, 0xb0, 0xa0, 0x90, 0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00,
   // Вектор инициалиазции
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

bool                       g_bPress = false;
u32                        g_u32LEDTime = 0;
u32                        g_u32FirmwareSize = 0;  // Размер прошивки
u16                        g_u16FirmwareCRC = 0;   // Контрольная сумма прошивки
CFirmware                  g_Firmware;             // Прошивка устройства
CIridiumCipherGrasshopper  g_Cipher;               // Шифр для декодирования прошивки

CDevice                    g_Device;               // Данные объекта

// Указатель на данные HAL CAN порта
extern CAN_HandleTypeDef   hcan;

static CanTxMsgTypeDef  g_aCanTxMessage;           // Структура для отправляемого сообщения
static CanRxMsgTypeDef  g_aCanRxMessage;           // Структура для принимаемого сообщения

CFlashNVRAM             g_NVRAM;

// Параметры CAN порта для сборки и разборки пакетов
CCANPort                g_ExtCAN;                  // Данные внешнего CAN порта
can_frame_t             g_aCANInBuffer[256];       // Массив для приема и сборки CAN пакетов
can_frame_t             g_aCANOutBuffer[33*8];     // Массив для отправки CAN пакетов
u16                     g_u16CANID = 0;            // Идентификатор CAN

// Индексы кнопок
enum eButton
{
   BUTTON_ONBOARD = 0,                             // Индекс набортной кнопки
};

// Массив для работы с бинарными входами
digital_input_t   g_aInputs[] =
{
   Onboard_Button_GPIO_Port, Onboard_Button_Pin, 0, 0, 0, { false, false, false, true }
};

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
   g_NVRAMData.m_Common.m_u8LID = in_u8LID;
   m_Address = g_NVRAMData.m_Common.m_u8LID;
   // Изменение фильтра
   BUS_SetFilter(g_u16CANID, m_Address);
   // Запись в энергонезависимую память
   //EEPROM_NeedSaveBuffer();
   g_NVRAM.Save();
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

         // Запись потока
         case IRIDIUM_OPERATION_WRITE_STREAM:
            l_s8Result = 1;
            break;

         default:
            l_s8Result = 0;
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
   Обработчик получения запроса на открытие потока
   на входе    :  in_pszName  - имя потока
                  in_eMode    - режим открытия потока
   на выходе   :  идентификатор открытого потока, если поток не был открыт возвражаемый результат равен 0
*/
u8 CDevice::StreamOpen(const char* in_pszName, eIridiumStreamMode in_eMode)
{
   u8 l_u8Result = 0;
   // Проверка имени запрашиваемого потока
   if(!g_Firmware.IsOpen() && in_pszName && !strcmp(in_pszName, FIRMWARE_NAME))
   {
      // Проверка на открытие потока для чтения или записи
      if(in_eMode == IRIDIUM_STREAM_MODE_WRITE)
      {
         l_u8Result = FIRMWARE_WRITE_STREAM_ID;
         // Установка адреса
         g_Firmware.SetAddress(GetSrcAddress());
         // Установка идентиифкатора потока
         g_Firmware.SetStreamID(l_u8Result);
         // Установка идентификатора блока
         g_Firmware.SetBlockID(0);
         // Установка времени по истечение которого нужно закрыть поток
         g_Firmware.SetTime(HAL_GetTick() + FIRMWARE_WAIT_TIME);
         // Установка данных потока
         g_Firmware.Open(FIRMWARE_START, FIRMWARE_SIZE);
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
   u8 l_u8Marker = 0;
   u32 l_u32Size = 0;
   u16 l_u16CRC = 0;
   u8* l_pBuffer = (u8*)in_pBuffer;
   size_t l_stSize = in_stSize;
   bool l_bFirst = false;
   
   // Проверка был ли откры поток, идентификатора потока и размер данных
   if(g_Firmware.IsOpen() && g_Firmware.GetStreamID() == in_u8StreamID && in_stSize >= 16)
   {
      // Проверка на первый блок
      l_bFirst = (g_Firmware.GetPtr() == FIRMWARE_START);
      
      // Проверка начала записи данных
      if(l_bFirst)
      {
         // Инициализация блочного шифра
         g_Cipher.EnableIV(true);
         g_Cipher.Init(g_aKeyAndIV);
      }
      
      // Декодирование полученого блока
      g_Cipher.Decode((u8*)in_pBuffer, in_stSize);
      
      // Проверка начала записи данных
      if(l_bFirst)
      {
         // Чтение случайного числа, маркера, размера и контрольной суммы прошивки из заголовка
         l_pBuffer = ReadByte(l_pBuffer, l_u8Marker);
         l_pBuffer = ReadByte(l_pBuffer, l_u8Marker);
         l_pBuffer = ReadLE(l_pBuffer, &l_u32Size, 4);
         l_pBuffer = ReadLE(l_pBuffer, &l_u16CRC, 2);
         // Уменьшение размера данных на размер заголовка
         l_stSize -= (l_pBuffer - (u8*)in_pBuffer);
         
         // Проверка размера и маркера
         if(l_u32Size && l_u8Marker == 0x77)
         {
            // Запись информации о прошивке
            g_NVRAMData.m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
            g_NVRAMData.m_Common.m_u32FirmwareSize = l_u32Size;
            g_NVRAMData.m_Common.m_u16FirmwareCRC16 = l_u16CRC;
            
            // Очистка памяти
            size_t l_stStart = (size_t)g_Firmware.GetPtr();
            size_t l_stEnd = (size_t)(g_Firmware.GetPtr() + g_Firmware.GetSize());
            FLASH_Clear(l_stStart, l_stEnd);
         } else
         {
            // Ошибка: маркер не найден
            in_stSize = 0;
         }
      }
      
      // Проверка на ошибку
      if(in_stSize)
      {
         // Запись во флеш память
         FLASH_Write((u8*)l_pBuffer, (size_t)g_Firmware.GetPtr(), l_stSize);
         // Сдвиг позиции
         g_Firmware.Skip(l_stSize);
         // Установка времени по истечению которого нужно закрыть поток
         g_Firmware.SetTime(HAL_GetTick() + FIRMWARE_WAIT_TIME);
      } else
      {
         // Установим текущее время чтобы закрыть поток
         g_Firmware.SetTime(HAL_GetTick());
      }

      // Сохранение информации о прошивке
      if(l_bFirst)
         g_NVRAM.Save();
   }
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
   // Проверка закрываемого потока
   if(g_Firmware.IsOpen() && g_Firmware.GetStreamID() == in_u8StreamID)
   {
      // Очистка прошивки
      g_Firmware.Close();
      
      // Запишем в энергонезависимую память состояние
      g_NVRAMData.m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
      g_NVRAM.Save();
      
      // Осуществим переход в загрузчик через сброс контроллера
      Reboot();
   }
}

/**
   Инициализация устройства
   на входе    :  *
   на выходе   :  *
*/
void CDevice::Setup()
{
   iridium_search_info_t l_Search;
   // Загрузка локального идентификатора
   m_Address = g_NVRAMData.m_Common.m_u8LID;

   // Поставим конец строки на всякий случай
   g_NVRAMData.m_Common.m_szDeviceName[sizeof(g_NVRAMData.m_Common.m_szDeviceName) - 1] = 0;

   // Генерация идентификатора
   GenerateHWID();
   
   // Обнуление PIN при первом запуске 
   if(g_NVRAMData.m_Common.m_u32PIN == 0xFFFFFFFF)
      g_NVRAMData.m_Common.m_u32PIN = 0;
   
   // Инициализация шины
   BUS_Init();
   
   // Установка фильтров
   g_u16CANID = GetCRC16Modbus(1, (u8*)g_pszHWID, sizeof(g_pszHWID));
   BUS_SetFilter(g_u16CANID, m_Address);

   // Ожидание в течении 50 миллисекунд нажатия набортной кнопки
   u32 l_u32Time = HAL_GetTick();
   while((HAL_GetTick() - l_u32Time) < 50)
      IO_UpdateInput(g_aInputs, sizeof(g_aInputs) / sizeof(g_aInputs[BUTTON_ONBOARD]));

   // Получение состояния набортной кнопки
   g_bPress = g_aInputs[BUTTON_ONBOARD].m_Flags.m_bCurValue;

   // Отправка информации об устройстве
   if(GetSearchInfo(l_Search))
      SendSearchResponse(GetTID(), l_Search);

   // Выставим режим запуска прошивки, если не режим ожидания прошивки
   u8 l_u8Mode = g_NVRAMData.m_Common.m_u8Mode;
   if(l_u8Mode != BOOTLOADER_MODE_DOWNLOAD)
   {
      g_NVRAMData.m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
      g_NVRAM.Save();
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
   
   // Обработка сохранения изменений
   g_NVRAM.Work();
   
   // Запись во внешний CAN порт
   WriteToExtCan();

   // Чтение и обработка данных с внешнего CAN порта
   ReadFromExtCan();

   // Получение режима работы
   u8 l_u8Mode = g_NVRAMData.m_Common.m_u8Mode;
   
   // Если загрузчик находится в режиме загрузки прошивки
   if(l_u8Mode == BOOTLOADER_MODE_DOWNLOAD)
   {
      iridium_packet_header_t l_PH;
      m_pInPH = &l_PH;
      
      // Переход в режим получения прошивки
      g_NVRAMData.m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
      g_NVRAM.Save();
      
      // Открытие потока
      u8 l_u8Stream = StreamOpen(FIRMWARE_NAME, IRIDIUM_STREAM_MODE_WRITE);
      
      // Получение параметров для отправки
      m_pInPH->m_SrcAddr         = g_NVRAMData.m_Common.m_u16FirmwareAddress;
      m_pInPH->m_u8Route         = 0;

      m_InMH.m_Flags.m_u4Version = GetMessageVersion(IRIDIUM_MESSAGE_STREAM_OPEN);
      m_InMH.m_u8Type            = IRIDIUM_MESSAGE_STREAM_OPEN;
      m_InMH.m_u16TID            = g_NVRAMData.m_Common.m_u16FirmwareTID;
      // Отправка ответа
      SendStreamOpenResponse(FIRMWARE_NAME, IRIDIUM_STREAM_MODE_WRITE, l_u8Stream);

      g_bPress = false;
   }

   // Контроль работы прошивки
   if(g_Firmware.IsOpen())
   {
      // Если время равно 0 или время вышло, закроем поток
      if(g_Firmware.GetTime() < HAL_GetTick())
      {
         // Пошлем сообщение что поток закрыт
         SendStreamCloseRequest(g_Firmware.GetAddress(), g_Firmware.GetStreamID());
         // Закроем поток
         g_Firmware.Close();
         
         // Запишем в энергонезависимую память состояние
         g_NVRAMData.m_Common.m_u8Mode =  BOOTLOADER_MODE_RUN;
         g_NVRAM.Save();
         
         // Осуществим переход в загрузчик через сброс контроллера
         Reboot();
      }
   }

   // Мигание набортного светодиода если не было зажатия набортной кнопки
   if(!g_bPress && HAL_GetTick() - g_u32LEDTime > 1000)
   {
      HAL_GPIO_TogglePin(Onboard_LED_GPIO_Port, Onboard_LED_Pin);
      g_u32LEDTime = HAL_GetTick();
   }
}

/**
   Цикл обработки нажатий
   на входе    :  *
   на выходе   :  *
*/
void CDevice::WorkInputs()
{
   iridium_search_info_t l_Search;
   // Обработка нажатий
   IO_UpdateInput(g_aInputs, sizeof(g_aInputs) / sizeof(g_aInputs[BUTTON_ONBOARD]));

   // Проверка была ли зажата набортная кнопка при старте
   if(g_bPress)
   {
      // Если набортаная кнопка была зажата на 5 секунд, произведем сброс данных
      if(g_aInputs[BUTTON_ONBOARD].m_Flags.m_bCurValue && (HAL_GetTick() - g_aInputs[BUTTON_ONBOARD].m_u32ChangeTime) > 5000)
      {
         // Индикация сброса параметров
         for(size_t i = 0; i < 10; i++)
         {
            HAL_GPIO_TogglePin(Onboard_LED_GPIO_Port, Onboard_LED_Pin);
            HAL_Delay(100);
         }

         // Сбросим PIN код
         g_NVRAMData.m_Common.m_u32PIN = 0;
         // Отметим что устройство надо сбросить
         g_NVRAMData.m_Common.m_u8FirmwareID = 0xFF;
         // Сброс ключа шифрования
         for(size_t i = 0; i < BLOCK_CIPHER_KEY_SIZE; i++)
            g_NVRAMData.m_Common.m_aKey[i] = 0;

         // Запись режима загрузки устройства
         g_NVRAMData.m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
         // Насильная запись данных во флешь память
         g_NVRAM.Save();
         // Перезагрузка контроллера
         HAL_Delay(100);
         Reboot();
      }

      // Проверка изменения значения набортной кнопки
      if(g_aInputs[BUTTON_ONBOARD].m_Flags.m_bChange && !g_aInputs[BUTTON_ONBOARD].m_Flags.m_bCurValue)
      {
         eBootloaderMode l_eMode = BOOTLOADER_MODE_RUN;
         // Проверка на переход в режим бутлоадера
         if(g_aInputs[BUTTON_ONBOARD].m_u32IntervalTime >= 1000)
         {
            // Индикация перехода в режим бутлоадера
            for(size_t i = 0; i < 3; i++)
            {
               HAL_GPIO_TogglePin(Onboard_LED_GPIO_Port, Onboard_LED_Pin);
               HAL_Delay(300);
            }
            // Переход в режим бутлоадера
            l_eMode = BOOTLOADER_MODE_BOOT;
         }
         // Запись режима загрузки устройства
         g_NVRAMData.m_Common.m_u8Mode = l_eMode;
         // Насильная запись данных во флешь память
         g_NVRAM.Save();
         // Перезагрузка контроллера
         Reboot();
      }
   } else
   {
      // Если была нажата набортная кнопка, пошлем в шину информацию о себе
      if(g_aInputs[BUTTON_ONBOARD].m_Flags.m_bChange && g_aInputs[BUTTON_ONBOARD].m_Flags.m_bCurValue)
      {
         // Получение информации об устройстве
         if(GetSearchInfo(l_Search))
         {
            // Отправка информации в шину
            SendSearchResponse(GetTID(), l_Search);
         }
      }
   }
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
         if(m_InBuffer.OpenPacket())
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
   Вызов инициализации устройства
   на входе    :  *
   на выходе   :  *
*/
void iRidiumDevice_InitEEPROM()
{
   // Установка границ энергонезависимой памяти
   g_NVRAM.SetFlashRange(EEPROM_FLASH_START, EEPROM_FLASH_END);

   // Настройка кеширования
   g_NVRAM.SetCache(&g_NVRAMData, sizeof(g_NVRAMData));
   // Установка времени через которе нужно сохранять данные в энергонезависимую память
   g_NVRAM.SetUpdateTime(500);
   
   // Инициализация энергонезависимой памяти
   g_NVRAM.Init();
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
