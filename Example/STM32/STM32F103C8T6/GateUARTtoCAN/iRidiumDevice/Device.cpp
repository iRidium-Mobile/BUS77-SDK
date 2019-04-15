#include "main.h"
#include "Device.h"
#include "stm32f1xx_hal.h"
#include "CCANPort.h"
#include "CIridiumBusInBuffer.h"
#include "IridiumCRC16.h"
#include "IridiumBus.h"

// Внешние структуры
extern CAN_HandleTypeDef       hcan;               // Структура для работы с CAN
extern UART_HandleTypeDef      huart2;             // Струкутра для работы с UART

char                    g_pszHWID[25];             // Для хранения уникального идентификатора CAN устройства

u16                     g_u16CanID = 0;            // Идентификатор CAN фреймов устройства
u16                     g_u16TID = 0;              // Текущий идентификатор транзакции

static CanTxMsgTypeDef  g_aCanTxMessage;           // Структура для отправляемого сообщения
static CanRxMsgTypeDef  g_aCanRxMessage;           // Структура для принимаемого сообщения

// Входящий UART буфер
CIridiumBusInBuffer     g_UARTInBuffer;
u8                      g_aUARTInBuffer[IRIDIUM_BUS_IN_BUFFER_SIZE];

// Входящий CAN буфер
CIridiumBusInBuffer     g_CANInBuffer;
u8                      g_aCANInBuffer[IRIDIUM_BUS_IN_BUFFER_SIZE];

CCANPort                g_CAN;
can_frame_t             g_aFromUart[512];
can_frame_t             g_aFromCan[512];

bool                    g_bTransmitEnd = true;
uint8_t                 g_aUartBuffer[1];

/**
   Поиск шинного пакета в буфере UART и отправка в буфер для формирования фреймов
   на входе    :  *
   на выходе   :  *
*/
void FindPacketsToCan()
{
   // Удаление шума из буфера
   g_UARTInBuffer.FilterNoise();
   
   // Найдем пакет в буфере UART
   if(g_UARTInBuffer.OpenPacket())
   { 
      // Добавим пакет в буфер для формирования фреймов и отправки в CAN
      g_CAN.AddPacket(true, 0, g_UARTInBuffer.GetPacketPtr(), g_UARTInBuffer.GetPacketSize());
   }
   // Закрытие шинного сообщения
   g_UARTInBuffer.ClosePacket();
}

/**
   Получение фреймов и отправка их в CAN
   на входе    :  *
   на выходе   :  *
*/
void SendPacketsToCan()
{
   // Получим фрейм для отправки в CAN
   can_frame_t* l_pPtr = g_CAN.GetFrame();
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
      {    
         // Если передача прошла успешно, удалим отправленный фрейм
         g_CAN.DeleteFrame();
      }
   }
}

/**
   Сборка пакета из фреймов, складывание в буфер для отправки в UART,
      поиск шинного пакета в буфере и отправка
   на входе    :  *
   на выходе   :  *
*/
void FindAndSendToUart()
{
   void* l_pBuffer = NULL;
   size_t l_stSize = 0;
   
   // Получим размер помещенного в буфер пакета
   bool l_bResult = g_CAN.GetPacket(l_pBuffer, l_stSize);
   
   // Отключим прерывания для избежания потери данных во время сдвига
   HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
   HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);

   // Удаление обработаных пакетов
   g_CAN.Flush();

   // Вернем прерывания
   HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
   HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

   // Если в буфере есть данные
   if(l_bResult)
   {
      // Добавление пакета в буфер 
      g_CANInBuffer.Add(l_pBuffer, l_stSize);

      // Поиск и удаление мусора
      g_CANInBuffer.FilterNoise();
      
      if(g_CANInBuffer.OpenPacket())
      {
         // Отключим прерывания UART перед установкой флага false, что бы не получить true раньше
         HAL_NVIC_DisableIRQ(USART2_IRQn);
         
         // Отправим валидный найденый пакет в UART
         if(HAL_UART_Transmit_IT(&huart2, (u8*)g_CANInBuffer.GetPacketPtr(), g_CANInBuffer.GetPacketSize()) == HAL_OK)
            g_bTransmitEnd = false;
         
         // Включим прерывания UART
         NVIC_EnableIRQ(USART2_IRQn);            
      }
      g_CANInBuffer.ClosePacket();
      
      // Освобождение пакета
      g_CAN.DeletePacket();
   }
}

uint32_t g_Count = 0;
/**
   Обработка приходящих фреймов с CAN и складывание их в буфер для формирования целых пакетов
   на входе    :  in_pCan  - указатель на структуру CAN
   на выходе   :  *
*/
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* in_pCan)
{
   can_frame_t l_Frame;

   // Извлечение полезной информации
   l_Frame.m_u32ExtID   = in_pCan->pRxMsg->ExtId;
   l_Frame.m_u8Size     = in_pCan->pRxMsg->DLC;
   
   // Скопируем полезную нагрузку
   memcpy(l_Frame.m_aData, in_pCan->pRxMsg->Data, l_Frame.m_u8Size);
   
   // Добавление полученого фрейма в буфер
   g_CAN.AddFrame(&l_Frame);
   
   // Включим прерыване обратно
   HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
   
   g_Count++;
}

/**
   Обработка прерывания отправки
   на входе    :  in_pCan  - указатель на структуру CAN
   на выходе   :  *
*/
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* in_pCan)
{
   //switch(in_pCan->ErrorCode)
   //{
   //case HAL_CAN_ERROR_NONE  : ERRORNo++;               break;//    0x00000000U    /*!< No error             */
   //case HAL_CAN_ERROR_EWG   : ERROREWG++;              break;//    0x00000001U    /*!< EWG error            */
   //case HAL_CAN_ERROR_EPV   : ERROREPV++;              break;//    0x00000002U    /*!< EPV error            */
   //case HAL_CAN_ERROR_BOF   : ERRORBOF++;              break;//    0x00000004U    /*!< BOF error            */
   //case HAL_CAN_ERROR_STF   : ERRORStuff++;            break;//    0x00000008U    /*!< Stuff error          */
   //case HAL_CAN_ERROR_FOR   : ERRORForm++;             break;//    0x00000010U    /*!< Form error           */
   //case HAL_CAN_ERROR_ACK   : ERRORAcknowledgment++; break;//    0x00000020U    /*!< Acknowledgment error */
   //case HAL_CAN_ERROR_BR    : ERRORBit++;             break;//    0x00000040U    /*!< Bit recessive        */
   //case HAL_CAN_ERROR_BD    : ERRORLEC++;             break;//    0x00000080U    /*!< LEC dominant         */
   //case HAL_CAN_ERROR_CRC   : ERRORLECt++;             break;//    0x00000100U    /*!< LEC transfer error   */
   //case HAL_CAN_ERROR_FOV0  : ERRORFIFO0++;           break;//    0x00000200U    /*!< FIFO0 overrun error  */
   //case HAL_CAN_ERROR_FOV1  : ERRORFIFO1++;           break;//    0x00000400U    /*!< FIFO1 overrun error  */
   //case HAL_CAN_ERROR_TXFAIL: ERRORTransmit++;       break;//    0x00000800U    /*!< Transmit failure     */
   //}
}

/**
   Обработчик получения одного байта из шины
   на входе    :  huart - указатель на хендлер UART
   на выходе   :  *
*/
void  HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
   g_UARTInBuffer.AddByte(g_aUartBuffer[0]);
   HAL_UART_Receive_IT(huart, g_aUartBuffer, 1);
}


/**
   Обработчик окончания передачи 
   на входе    :  huart - указатель на хендлер UART
   на выходе   :  *
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
   g_bTransmitEnd = true;
}

/**
   Инициализация устройства
   на входе    :  *
   на выходе   :  *
*/
void iRidiumDevice_Setup(void)
{
   // Инициализация структур сообщений
   hcan.pTxMsg = &g_aCanTxMessage;
   hcan.pRxMsg = &g_aCanRxMessage;
   
   // Инициализация фильтра CAN FIFO 0
	CAN_FilterConfTypeDef CAN_FilterFIFO0;
	CAN_FilterFIFO0.FilterNumber = 1;
	CAN_FilterFIFO0.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterFIFO0.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterFIFO0.FilterIdHigh = 0x0000;
	CAN_FilterFIFO0.FilterIdLow = 0x0000;
	CAN_FilterFIFO0.FilterMaskIdHigh = 0x0000;
	CAN_FilterFIFO0.FilterMaskIdLow = 0x0000;
	CAN_FilterFIFO0.FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterFIFO0.FilterActivation = ENABLE;

   HAL_CAN_ConfigFilter(&hcan, &CAN_FilterFIFO0);

   // Включаем прерывания на прием
   HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
   
   g_CAN.SetInBuffer(g_aFromCan, sizeof(g_aFromCan));
   g_CAN.SetOutBuffer(g_aFromUart, sizeof(g_aFromUart));
  
   // Инициализируем буфер UART
   g_UARTInBuffer.SetBuffer(g_aUARTInBuffer, sizeof(g_aUARTInBuffer));
   g_UARTInBuffer.Clear();
   
   // Инициализируем буфер CAN
   g_CANInBuffer.SetBuffer(g_aCANInBuffer, sizeof(g_aCANInBuffer));
   g_CANInBuffer.Clear();
   
   // Для формирования уникального идентификатора
   char l_szHexTab[] = "0123456789ABCDEF";

   // Получим указатель на уникальный идентификатор STM32
   u8* l_pSrc = (u8*)UID_BASE;
   char* l_pDst = g_pszHWID;
   
   // Сформируем HEX data
   for(size_t i = 0; i < 12; i++)
   {
      u8 l_u8Byte = *l_pSrc++;
      *l_pDst++ = l_szHexTab[l_u8Byte >> 4];
      *l_pDst++ = l_szHexTab[l_u8Byte & 0xF];
   }
   *l_pDst = 0;
   
   // Сформируем на основании HWID и запомним уникальный идентификатор устройства для формирования фреймов
   g_u16CanID = GetCRC16Modbus(1, (u8*)g_pszHWID, sizeof(g_pszHWID));
   
   g_CAN.SetCANID(g_u16CanID);
   g_CAN.SetTID(0);
   g_CAN.SetAddress(0);

   // Включим прерывание получения данных из UART
   HAL_UART_Receive_IT(&huart2, g_aUartBuffer, sizeof(g_aUartBuffer));
}

/**
   Выполнение одного шага устройства
   на входе    :  *
   на выходе   :  *
*/
void iRidiumDevice_Loop(void)
{
   //if(HAL_OK != RECEIVE_ENABLED)
   {
      /*
         Есть проблема, не всегда после обработки 
         прерывания его получалось включить обратно, 
         пока старамеся включить всегда
      */
      // Попытаемся установить возможность приема
      HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
   }
   
   // Обработаем буфер из уарта
   if(g_UARTInBuffer.Size())
      FindPacketsToCan();
   // Обработаем буфер на отправку в CAN
   if(g_CAN.GetFrame())
      SendPacketsToCan();
   
   // Обработаем буфер из CAN если есть данные
   if(g_bTransmitEnd && (HAL_UART_GetState(&huart2) & HAL_UART_STATE_READY) == HAL_UART_STATE_READY)
      FindAndSendToUart();
}
