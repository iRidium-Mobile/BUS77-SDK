//////////////////////////////////////////////////////////////////////////
// class CDevice
//////////////////////////////////////////////////////////////////////////
// Включения
// Inc
#include "stm32l4xx_hal.h"
#include "Main.h"
#include "stdlib.h"

// Common
#include "CCanPort.h"
#include "CFirmware.h"
#include "FLASHMemoryMap.h"
#include "NVRAMCommon.h"

// MCUCommon
#include "CFlashNVRAM.h"

// ChipCommon
#include "Flash.h"
#include "CI2CNVRAM16Bit.h"
#include "CommonCAN.h"

// DeviceCommon
#include "CDeviceIndicator.h"
#include "InputOutput.h"
#include "DeviceDefine.h"

// iRidiumDevice
#include "CDevice.h"

// iRidiumProtocol
#include "CIridiumStreebog.h"
#include "IridiumCRC16.h"
#include "IridiumBytes.h"
#include "IridiumChannels.h"

#define MAX_DEVICE_CHANNELS            0                 // Максимальное кол-во каналов управления / Maximum number of control channels
#define MAX_DEVICE_TAGS                0                 // Максимальное кол-во каналов обратнйо связи / Maximum number of feedback channels
#define MAX_VARIABLES                  0                 // Максимальное кол-во глобальных переменных на канал управления / Maximum number of global variables per control chanel

#define MAX_INPUTS                     1                 // Максимальное кол-во дискретных выходов (1-3) / Maximum numbers of discret channels (1-3)
   
#define FIRMWARE_READ_STREAM_ID        1                 // Идентификатор потока чтения прошивки
#define FIRMWARE_WRITE_STREAM_ID       2                 // Идентификатор потока записи прошивки

extern CAN_HandleTypeDef hcan1;
extern I2C_HandleTypeDef hi2c2;

///////////////////////////////////////////////////////////////////////////////
// Энергонезависимая память
///////////////////////////////////////////////////////////////////////////////
typedef struct nvram_device_s
{
   // Обшая структура данных (для загрузчика и прошивки)
   nvram_common_t m_Common;
   // Резервирование памяти
   //u8                m_aReserve[EEPROM_SIZE - sizeof(nvram_common_t)];
} nvram_device_t;

// Данные для кеширования энергонезависимой памяти
u8 g_aNVRAMCache[FLASH_PAGE_SIZE];

nvram_device_t*   g_pNVRAMData = (nvram_device_t*)&g_aNVRAMCache;
CI2CNVRAM16Bit    g_NVRAM;
//CFlashNVRAM       g_NVRAM;
///////////////////////////////////////////////////////////////////////////////
// Информация об устройстве
///////////////////////////////////////////////////////////////////////////////
const char g_pszProducer[]    = "iRidium LTD";
const char g_pszModelName[]   = "FM-808";
//const char g_pszDeviceName[]  = "FM_808_Bootloader";

char g_szHWID[STREEBOG_HASH_256_BYTES + 1];        // Аппаратный идентификатор шеснадцатеричное представление 128 битного числа + 1 байт

// Информация об устройстве
const static iridium_device_info_t g_DeviceInfo =
{
   IRIDIUM_GROUP_TYPE_ACTUATOR,
   g_pNVRAMData->m_Common.m_szDeviceName,
   (char*)g_pszProducer,
   (char*)g_pszModelName,
   (char*)g_szHWID,
   DCT_MICROCONTROLLER,
   PT_ARM,
   OST_NONE,
   IRIDIUM_DEVICE_FLAG_FIRMWARE | IRIDIUM_DEVICE_FLAG_ACTUATOR | IRIDIUM_DEVICE_FLAG_PLC,
   0,
   { 1, 0 },
   0,
   0
};

// Ключ шифрования и вектор инициализации R5
const u8 g_aKeyAndIV[] =
{
   // Ключ шифрования
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
   0xf0, 0xe0, 0xd0, 0xc0, 0xb0, 0xa0, 0x90, 0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00,
   // Вектор инициалиазции
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 000102030405060708090a0b0c0d0e0ff0e0d0c0b0a09080706050403020100000000000000000000000000000000000

bool                       g_bPress = false;
u32                        g_u32LEDTime = 0;
u32                        g_u32FirmwareSize = 0;  // Размер прошивки
u16                        g_u16FirmwareCRC = 0;   // Контрольная сумма прошивки
CFirmware                  g_Firmware;             // Прошивка устройства
CIridiumCipherGrasshopper  g_Cipher;               // Шифр для декодирования прошивки

CDevice                 g_Device;                  // Данные объекта
CDeviceIndicator        g_Indicator;               // Индикация устройства


CCANPort    g_CAN;

can_frame_t g_aCANInBuffer[256];
can_frame_t g_aCANOutBuffer[33*8];

u16         g_u16CANID = 0;

char        g_szTemp[128];

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

// Индексы кнопок
enum eInputs
{
   ONBOARD_BUTTON_INDEX,                           // Набортная кнопка
};

/**
   Получение указателя на порт по хэндлеру CAN порта
   на входе    :  in_pCanHandle  - указатель на хэндлер порта
   на выходе   :  указатель на данные порта
*//*
CCANPort* GetCANPort(CAN_HandleTypeDef* in_pCanHandle)
{
   CCANPort* l_pResult = NULL;

   // Проверка на нужный порт
   if(in_pCanHandle->Instance == CAN1)
      l_pResult = &g_CAN1;
   else 
        Error_Handler();
   
   return l_pResult;
}*/

/**
   Блокирование доступа к CAN порту
   на входе    :  in_pCanHandle  - указатель на хэндлер порта
   на выходе   :  *
*//*
void LockCANPort(CAN_HandleTypeDef* in_pCanHandle)
{
   // Проверка на нужный порт
   if(in_pCanHandle->Instance == CAN1)
   {
      // Выключение прерываний CAN 1 на время сдвига буфера во избежании потери данных
      HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
      HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
      
   } else     Error_Handler();
   {
      // Выключение прерываний CAN 2 на время сдвига буфера во избежании потери данных

   }
}*/

/**
   Блокирование доступа к CAN порту
   на входе    :  in_pCanHandle  - указатель на хэндлер порта
   на выходе   :  *
*//*
void UnLockCANPort(CAN_HandleTypeDef* in_pCanHandle)
{
   // Проверка на нужный порт
   if(in_pCanHandle->Instance == CAN1)
   {
      // Включение прерываний CAN 1
      HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
      HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
      
   } else     Error_Handler();
   {
      // Включение прерываний CAN 2

   }
}

u32 g_u32Abort = 0;
u32 g_u32Full0 = 0;
u32 g_u32Full1 = 0;

u32 g_u32Sleep = 0;
u32 g_u32WakeUp = 0;

u32 g_u32Error = 0;
*/
/**
   Обработчик получения фрейма
   на входе    :  in_pCan        - указатель на CAN хэндлер
                  in_u8FifoNum   - номер FIFO буфера
   на выходе   :  *
*//*
void FrameReceived(CAN_HandleTypeDef* in_pCan, u8 in_u8FifoNum)
{
   can_frame_t l_Frame;
   CAN_RxHeaderTypeDef l_Header;

   // Получение указателя на CAN порт
   CCANPort* l_pPort = GetCANPort(in_pCan);
   if(l_pPort)
   {
      // Чтение всех пакетов из ящиков
      while(HAL_CAN_GetRxMessage(in_pCan, in_u8FifoNum, &l_Header, l_Frame.m_aData) == HAL_OK)
      {
         // Получение фрейма
         l_Frame.m_u32ExtID = l_Header.ExtId;
         l_Frame.m_u8Size = l_Header.DLC;

         // Добавление фрейма в буфер фреймов
         l_pPort->AddFrame(&l_Frame);
      }
   }
}*/

/**
   Обработчик окончания отправки фрейма
   на входе    :  in_pCan           - указатель на CAN хэндлер
                  in_u8MailBoxNum   - номер ящика который освободился
   на выходе   :  *
*//*
void SendFrameComplited(CAN_HandleTypeDef* in_pCan, u8 in_u8MailBoxNum)
{
   u32 l_u32MailBox = 0;
   CAN_TxHeaderTypeDef l_Header;

   // Получение указателя на CAN порт
   CCANPort* l_pPort = GetCANPort(in_pCan);
   if(l_pPort)
   {
      // Получим фрейм для отправки в CAN
      can_frame_t* l_pPtr = l_pPort->GetFrame();
      if(l_pPtr)
      {
         // Установим расширенный размер кадра
         memset(&l_Header, 0, sizeof(l_Header));
         l_Header.IDE = CAN_ID_EXT;
         l_Header.RTR = CAN_RTR_DATA;
         // Назначаем идентификатор сообщения
         l_Header.ExtId = l_pPtr->m_u32ExtID;
         // Указываем длинну сообщения
         l_Header.DLC = l_pPtr->m_u8Size;
         // Отправляем данные и удаляем фрейм
         if(HAL_CAN_AddTxMessage(in_pCan, &l_Header, l_pPtr->m_aData, &l_u32MailBox) == HAL_OK)
            l_pPort->DeleteFrame();

      } else
         l_pPort->SetTransmite(false);
   }
}*/

/**
   Обработчик отмены отправки фрейма
   на входе    :  in_pCan        - указатель на CAN хэндлер
                  in_u8MailBox   - номер ящика который был отменен
   на выходе   :  *
*//*
void SendFrameAborted(CAN_HandleTypeDef* in_pCan, u8 in_u8MailBox)
{
   g_u32Abort++;
}

// Обработчики окончания отправки фрейма
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef* hcan) { SendFrameComplited(hcan, 0); }
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef* hcan) { SendFrameComplited(hcan, 1); }
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef* hcan) { SendFrameComplited(hcan, 2); }

// Обработчики отмены отправки фрейма
void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef* hcan) { SendFrameAborted(hcan, 0); }
void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef* hcan) { SendFrameAborted(hcan, 1); }
void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef* hcan) { SendFrameAborted(hcan, 2); }

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) { FrameReceived(hcan, CAN_RX_FIFO0); }
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan) { FrameReceived(hcan, CAN_RX_FIFO1); }

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef* hcan) { g_u32Full0++; }
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef* hcan) { g_u32Full1++; }
*/
/**
   Обработчик засыпания
*//*
void HAL_CAN_SleepCallback(CAN_HandleTypeDef* hcan)
{
   g_u32Sleep++;
}*/

/**
   Обработчик пробуждения
*//*
void HAL_CAN_WakeUpFromRxMsgCallback(CAN_HandleTypeDef* hcan)
{
   g_u32WakeUp++;
}*/

/**
   Обработчик получения ошибки
*//*
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef* hcan)
{
   // handle transmit errors in all three mailboxes
   if(hcan->ErrorCode & HAL_CAN_ERROR_TX_ALST0)
   {
      SET_BIT(hcan->Instance->sTxMailBox[0].TIR, CAN_TI0R_TXRQ);
      hcan->ErrorCode &= ~HAL_CAN_ERROR_TX_ALST0;

   } else if(hcan->ErrorCode & HAL_CAN_ERROR_TX_TERR0)
   {
      hcan->ErrorCode &= ~HAL_CAN_ERROR_EWG;
      hcan->ErrorCode &= ~HAL_CAN_ERROR_ACK;
      hcan->ErrorCode &= ~HAL_CAN_ERROR_TX_TERR0;
   }

   if(hcan->ErrorCode & HAL_CAN_ERROR_TX_ALST1)
   {
      SET_BIT(hcan->Instance->sTxMailBox[1].TIR, CAN_TI1R_TXRQ);
      hcan->ErrorCode &= ~HAL_CAN_ERROR_TX_ALST1;

   } else if(hcan->ErrorCode & HAL_CAN_ERROR_TX_TERR1)
   {
      hcan->ErrorCode &= ~HAL_CAN_ERROR_EWG;
      hcan->ErrorCode &= ~HAL_CAN_ERROR_ACK;
      hcan->ErrorCode &= ~HAL_CAN_ERROR_TX_TERR1;
   }

   if(hcan->ErrorCode & HAL_CAN_ERROR_TX_ALST2)
   {
      SET_BIT(hcan->Instance->sTxMailBox[2].TIR, CAN_TI2R_TXRQ);
      hcan->ErrorCode &= ~HAL_CAN_ERROR_TX_ALST2;

   } else if(hcan->ErrorCode & HAL_CAN_ERROR_TX_TERR2)
   {
      hcan->ErrorCode &= ~HAL_CAN_ERROR_EWG;
      hcan->ErrorCode &= ~HAL_CAN_ERROR_ACK;
      hcan->ErrorCode &= ~HAL_CAN_ERROR_TX_TERR2;
   }

   g_u32Error++;
}*/

/**
   Обработка внешнего CAN порта
   на входе    :  *
   на выходе   :  *
*/
void CAN_WorkExternal()
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
      g_Device.AddPacket(l_pBuffer, l_stSize);

		// Обработка полученого пакета
		g_CAN.DeletePacket();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//// Работа с шиной
/////////////////////////////////////////////////////////////////////////////////////////////////
/**
   Инициализация шины
   на входе    :  *
   на выходе   :  *
   примечание  :  микроконтроллер stm32f405vgt5 имеет два CAN порта. При этом CAN1 это master, а CAN2 это slave
                  это означает что CAN1 и CAN2 имеют блок фильтров один на двоих, то есть от 0 до 13 это фильтры
                  для CAN1, а фильтры от 14 по 27 это фильтры для CAN2
*//*
void CAN_Init()
{
   // Установка фильтра CAN1, прием всех сообщений
	CAN_FilterTypeDef CAN_FilterInitStructure;
   CAN_FilterInitStructure.FilterIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterIdLow = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdLow = 0x0000;
   CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
   CAN_FilterInitStructure.FilterBank = 0;
   CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
   CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;
   CAN_FilterInitStructure.FilterActivation = ENABLE;
   CAN_FilterInitStructure.SlaveStartFilterBank = 14;                 // Важно заполнить это поле
   HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterInitStructure);

   g_CAN1.SetCANID(0xFFF);
   g_CAN1.SetTID(0x00);
   g_CAN1.SetAddress(0);
   g_CAN1.SetInBuffer(g_aCAN1InBuffer, sizeof(g_aCAN1InBuffer));
   g_CAN1.SetOutBuffer(g_aCAN1OutBuffer, sizeof(g_aCAN1OutBuffer));
  
   // Установка фильтра CAN2 прием всех сообщений
   CAN_FilterInitStructure.FilterIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterIdLow = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdLow = 0x0000;
   CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
   CAN_FilterInitStructure.FilterBank = 14;
   CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
   CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;
   CAN_FilterInitStructure.FilterActivation = ENABLE;
   CAN_FilterInitStructure.SlaveStartFilterBank = 14;              // Важно заполнить это поле
  
   HAL_CAN_Start(&hcan1);

   HAL_CAN_ActivateNotification(&hcan1,   CAN_IT_TX_MAILBOX_EMPTY | \
                                          CAN_IT_RX_FIFO0_MSG_PENDING | \
                                          CAN_IT_RX_FIFO0_FULL | \
                                          CAN_IT_RX_FIFO0_OVERRUN | \

                                          CAN_IT_RX_FIFO1_MSG_PENDING | \
                                          CAN_IT_RX_FIFO1_FULL | \
                                          CAN_IT_RX_FIFO1_OVERRUN | \

                                          CAN_IT_WAKEUP | \
                                          CAN_IT_SLEEP_ACK | \

                                          CAN_IT_ERROR_WARNING | \
                                          CAN_IT_ERROR_PASSIVE | \
                                          CAN_IT_BUSOFF | \
                                          CAN_IT_LAST_ERROR_CODE | \
                                          CAN_IT_ERROR);

}*/

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
*//*
void CAN_SetFilter(u16 in_u16CanID, u8 in_u8Address)
{
   // Установка идентификатора
   g_CAN1.SetCANID(in_u16CanID);
   g_CAN1.SetAddress(in_u8Address);

   // Настройка фильтра 0 (CAN1) для приема широковещательных фреймов
   // Маска          :  1 00000000 0   маска на 10 бит
   // Идентификатор  :  1 00000000 0   10 бит включен (признак широковещательного фрейма)
   CAN_FilterTypeDef CAN_FilterInitStructure;
   CAN_FilterInitStructure.FilterIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterIdLow = 0x0200 << 3;
   CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdLow = 0x0200 << 3;
   CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
   CAN_FilterInitStructure.FilterBank = 0;
   CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
   CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;
   CAN_FilterInitStructure.FilterActivation = ENABLE;
   CAN_FilterInitStructure.SlaveStartFilterBank = 14;        // Важно заполнить это поле
   HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterInitStructure);
   
   // Настройка фильтра 1 (CAN1) для приема адресного фрейма
   // Маска          :  1 11111111 0   маска на 10 бит
   // Идентификатор  :  0 AAAAAAAA 0   10 бит выключен (признак адресного фрейма), биты от 9-1 адрес
   CAN_FilterInitStructure.FilterIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterIdLow = in_u8Address << (3 + 1);
   CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
   CAN_FilterInitStructure.FilterMaskIdLow = 0x03FE << 3;
   CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
   CAN_FilterInitStructure.FilterBank = 1;
   CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
   CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;
   CAN_FilterInitStructure.FilterActivation = ENABLE;
   CAN_FilterInitStructure.SlaveStartFilterBank = 14;        // Важно заполнить это поле
   HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterInitStructure);

}*/

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
   char* l_pszHWID = g_szHWID;
   for(size_t i = 0; i < STREEBOG_HASH_256_BYTES / 2; i++)
   {
      u8 l_u8Byte = l_aHash[i] ^ l_aHash[STREEBOG_HASH_256_BYTES / 2 + i];
      *l_pszHWID++ = l_szHexTab[l_u8Byte >> 4];
      *l_pszHWID++ = l_szHexTab[l_u8Byte & 0xF];
   }
   *l_pszHWID = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// CCANDevice
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
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
   на входе    :  in_bBroadcast  - флаг широковещательного пакета
                  in_Address     - адрес
                  in_pBuffer     - указатель на буфер с данными
                  in_stSize      - размер данных
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
   Установка локального идентификатора
   на входе    :  in_u8LID - локальный идентификатор устройства
   на выходе   :  *
*/
void CDevice::SetLID(u8 in_u8LID)
{
   // Запись локального идентификатора в энергонезависимую память
   g_pNVRAMData->m_Common.m_u8LID = in_u8LID;
   m_Address = g_pNVRAMData->m_Common.m_u8LID;
   // Изменение фильтра
   CAN_SetFilter(0, g_u16CANID, m_Address);
   // Запись в энергонезависимую память
   g_NVRAM.NeedSave();
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
   return (in_pszHWID && !strcmp(in_pszHWID, g_szHWID)) ? true : false;
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
   u32 l_u32PIN = g_pNVRAMData->m_Common.m_u32PIN;

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
   на выходе   :  успешность получения информации
*/
bool CDevice::GetSearchInfo(iridium_search_info_t& out_rInfo)
{
   // Заполнение информации об устройстве
   out_rInfo.m_u8Group  = IRIDIUM_GROUP_TYPE_ACTUATOR;
   out_rInfo.m_pszHWID  = (char*)g_szHWID;
   return true;
}

/**
   Получение информации об устройстве
   на входе    :  out_rInfo   - ссылка на структуру куда надо поместить данные об устройстве
   на выходе   :  успешность получения информации об устройстве
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
   Получение количества каналов обратной связи на устройстве
   на входе    :  *
   на выходе   :  количество каналов обратной связи
*/
size_t CDevice::GetTags()
{
   return 0;
   
}
/**
   Получение количества каналов управления на устройстве
   на входе    :  *
   на выходе   :  количество каналов управления
*/
size_t CDevice::GetChannels()
{
   return 0;
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
      
      // Разблокируем флеш
      HAL_FLASH_Unlock();
      
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
            g_pNVRAMData->m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
            g_pNVRAMData->m_Common.m_u32FirmwareSize = l_u32Size;
            g_pNVRAMData->m_Common.m_u16FirmwareCRC16 = l_u16CRC;
            g_pNVRAMData->m_Common.m_u16FirmwareTID = 0xFF;
            
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
      
      // Заблокируем флеш
      HAL_FLASH_Lock();

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
      g_pNVRAMData->m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
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
   m_Address = g_pNVRAMData->m_Common.m_u8LID;   
   
   g_pNVRAMData->m_Common.m_szDeviceName[sizeof(g_pNVRAMData->m_Common.m_szDeviceName) - 1] = 0;
   g_pNVRAMData->m_Common.m_u32PIN = 0;

   // Генерация идентификатора
   GenerateHWID();

   // Инициализация CAN порта
   g_CAN.SetInBuffer(g_aCANInBuffer, sizeof(g_aCANInBuffer));
   g_CAN.SetOutBuffer(g_aCANOutBuffer, sizeof(g_aCANOutBuffer));

   // Инициализация шины
   CAN_Init(0, &hcan1, &g_CAN);
   
   // Установка фильтров
   g_u16CANID = GetCRC16Modbus(1, (u8*)g_szHWID, sizeof(g_szHWID));
   CAN_SetFilter(0, g_u16CANID, m_Address);
   
   // Отправка информации об устройстве
   if(GetSearchInfo(l_Search))
      SendSearchResponse(GetTID(), l_Search);
   
   // Выставим режим запуска прошивки, если не режим ожидания прошивки
   u8 l_u8Mode = g_pNVRAMData->m_Common.m_u8Mode;
   if(l_u8Mode != BOOTLOADER_MODE_DOWNLOAD)
   {
      g_pNVRAMData->m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
      g_NVRAM.Save();
   }

   g_Indicator.SetMode(INDICATOR_MODE_BOOTLOADER);
}

/**
   Основной цикл работы устройства
   на входе    :  *
   на выходе   :  *
*/
void CDevice::Loop()
{
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
         // Проверка на данные для сегмента
/*
         if(l_pPH->m_Flags.m_bSegment)
         {
            // Проверка что данные пришлю именно к нам
            if((l_pPH->m_DstAddr >> 8) == m_Address)
            {
               // Перекидывание данных во внутрений интерфейс
               l_pPH->m_DstAddr &= 0xFF;
               g_OutBuffer.Begin(0);
               g_OutBuffer.AddData(m_InBuffer.GetMessagePtr(), m_InBuffer.GetMessageSize());
               g_OutBuffer.End(*l_pPH);

               //g_CAN2.AddPacket(l_pPH->m_Flags.m_bAddress, l_pPH->m_DstAddr, g_OutBuffer.GetPacketPtr(), g_OutBuffer.GetPacketSize());
            }
         } else*/
         {
            // Обработка сообщения
            ProcessMessage(l_pPH, l_pPacketPtr, l_stPacketSize);

            // Добавление данных пакету
            if(m_InMH.m_u8Type == IRIDIUM_MESSAGE_SET_VARIABLE || m_InMH.m_u8Type == IRIDIUM_MESSAGE_GET_VARIABLE)
            {
               //g_CAN2.AddPacket(l_pPH->m_Flags.m_bAddress, l_pPH->m_DstAddr, l_pPacketPtr, l_stPacketSize);
            }
         }
      }
      // Закрытие шинного сообщения
      m_InBuffer.ClosePacket();
   }
   
   // Получение режима работы
   u8 l_u8Mode = g_pNVRAMData->m_Common.m_u8Mode;
   
  // Если загрузчик находится в режиме загрузки прошивки
   if(l_u8Mode == BOOTLOADER_MODE_DOWNLOAD)
   {
      iridium_packet_header_t l_PH;
      m_pInPH = &l_PH;
      
      // Переход в режим получения прошивки
      g_pNVRAMData->m_Common.m_u8Mode = BOOTLOADER_MODE_RUN;
      g_NVRAM.Save();
      
      // Открытие потока
      u8 l_u8Stream = StreamOpen(FIRMWARE_NAME, IRIDIUM_STREAM_MODE_WRITE);
      
      // Получение параметров для отправки
      m_pInPH->m_SrcAddr         = g_pNVRAMData->m_Common.m_u16FirmwareAddress;
      m_InMH.m_Flags.m_u4Version = GetMessageVersion(IRIDIUM_MESSAGE_STREAM_OPEN);
      m_InMH.m_u8Type            = IRIDIUM_MESSAGE_STREAM_OPEN;
      m_InMH.m_u16TID            = g_pNVRAMData->m_Common.m_u16FirmwareTID;
      
      // Отправка ответа
      SendStreamOpenResponse(FIRMWARE_NAME, IRIDIUM_STREAM_MODE_WRITE, l_u8Stream);

      g_bPress = false;
   }

    //Контроль работы прошивки
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
         g_pNVRAMData->m_Common.m_u8Mode =  BOOTLOADER_MODE_RUN;
         g_NVRAM.Save();
         
         // Осуществим переход в загрузчик через сброс контроллера
         Reboot();
      }
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
   
}

/**
   Изменение значения глобальной переменной
   на входе    :  in_u16Variable - идентификатор изменяемой переменной
                  in_u8Type      - тип переменной
                  in_rValue      - ссылка на данные значения
   на выходе   :  *
*//*
void CDevice::ChangeVariable(u16 in_u16Variable, u8 in_u8Type, universal_value_t& in_rValue)
{
   // Отправка значения на шину
   SendSetVariableRequest(in_u16Variable, in_u8Type, in_rValue);
   // Изменение значения
   SetVariable(in_u16Variable, in_u8Type, in_rValue, IRIDIUM_FLAGS_END);
}*/

/**
   Вызов инициализации устройства
   на входе    :  *
   на выходе   :  *
*/
nvram_common_t* iRidiumDevice_InitNVRAM()
{
   nvram_common_t* l_pResult = NULL;

   // Включение режима работы
   g_Indicator.SetMode(INDICATOR_MODE_BOOTLOADER);

   // Установка интерфейса и адреса
   g_NVRAM.SetInterface(&hi2c2);
   g_NVRAM.SetAddress(0xA0);

   // Размер памяти 4кб, размер страницы 32 байта 
   g_NVRAM.Setup(4096, 32);

   // Установка флеша
//   g_NVRAM.SetFlashRange(EEPROM_START, EEPROM_END);

   // Установка кеширования
   g_NVRAM.SetCache(g_pNVRAMData, sizeof(nvram_device_t));
   //g_NVRAM.SetCache(&g_aNVRAMCache, sizeof(g_aNVRAMCache));
   g_NVRAM.SetUpdateTime(500);

   // Получение указателя на данные
   l_pResult = (nvram_common_t*)g_NVRAM.GetPtr();

   // Инициализация и чтение нужного банка энергонезависимой памяти
   if(g_NVRAM.Init() < 0)
   {
      // Индикация ошибки: неисправность энергонезависимой памяти
      g_Indicator.SetMode(INDICATOR_MODE_1_1);
   }
   return l_pResult;
}

/**
   Вызов инициализации устройства
   на входе    :  *
   на выходе   :  *
*/
void iRidiumDevice_Setup()
{
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
   // Работа с CAN
   CAN_WorkExternal();

   // Обработка нажатий
   g_Device.WorkInputs();

   g_Device.Loop();

   g_Indicator.Work();

   // Обработка сохранения изменений
   g_NVRAM.Work();
}
