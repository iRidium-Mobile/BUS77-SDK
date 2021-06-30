#include "CommonCAN.h"

// Структура для связывания CAN интерфейса с CAN портом
typedef struct can_link_s
{
   CAN_HandleTypeDef*   m_pHandle;                 // Указатель на интерфейс CAN
   CCANPort*            m_pPort;                   // Указатель на CAN порт
} can_link_t;

u32 g_u32Abort = 0;
u32 g_u32Full0 = 0;
u32 g_u32Full1 = 0;

u32 g_u32Sleep = 0;
u32 g_u32WakeUp = 0;

u32 g_u32Error = 0;

// Список CAN связей
can_link_t  g_aLinkList[1];

/**
   Получение указателя на порт по хэндлеру CAN порта
   на входе    :  in_pCanHandle  - указатель на хэндлер порта
   на выходе   :  указатель на данные порта
*/
static CCANPort* GetCANPort(CAN_HandleTypeDef* in_pCan)
{
   CCANPort* l_pResult = NULL;

   // Проверка на нужный порт
   if(g_aLinkList[0].m_pHandle == in_pCan)
      l_pResult = g_aLinkList[0].m_pPort;
   
   return l_pResult;
}

/**
   Обработчик получения фрейма
   на входе    :  in_pCan        - указатель на CAN хэндлер
                  in_u8FifoNum   - номер FIFO буфера
   на выходе   :  *
*/
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
}

/**
   Обработчик окончания отправки фрейма
   на входе    :  in_pCan           - указатель на CAN хэндлер
                  in_u8MailBoxNum   - номер ящика который освободился
   на выходе   :  *
*/
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
}

/**
   Обработчик отмены отправки фрейма
   на входе    :  in_pCan        - указатель на CAN хэндлер
                  in_u8MailBox   - номер ящика который был отменен
   на выходе   :  *
*/
void SendFrameAborted(CAN_HandleTypeDef* in_pCan, u8 in_u8MailBox)
{
   g_u32Abort++;
}

// Обработчики окончания отправки CAN кадра
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef* in_pCan) { SendFrameComplited(in_pCan, 0); }
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef* in_pCan) { SendFrameComplited(in_pCan, 1); }
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef* in_pCan) { SendFrameComplited(in_pCan, 2); }

// Обработчики отмены отправки CAN кадра
void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef* in_pCan) { SendFrameAborted(in_pCan, 0); }
void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef* in_pCan) { SendFrameAborted(in_pCan, 1); }
void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef* in_pCan) { SendFrameAborted(in_pCan, 2); }

// Обработчики получения CAN кадра
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* in_pCan) { FrameReceived(in_pCan, CAN_RX_FIFO0); }
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* in_pCan) { FrameReceived(in_pCan, CAN_RX_FIFO1); }

// Обработчики переполнения очередей
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef* in_pCan) { g_u32Full0++; }
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef* in_pCan) { g_u32Full1++; }

/**
   Обработчик засыпания
   на входе    :  in_pCan  - указатель на CAN хэндлер
   на выходе   :  *
*/
void HAL_CAN_SleepCallback(CAN_HandleTypeDef* in_pCan)
{
   g_u32Sleep++;
}

/**
   Обработчик пробуждения
   на входе    :  in_pCan  - указатель на CAN хэндлер
   на выходе   :  *
*/
void HAL_CAN_WakeUpFromRxMsgCallback(CAN_HandleTypeDef* in_pCan)
{
   g_u32WakeUp++;
}

/**
   Обработчик получения ошибки
   на входе    :  in_pCan  - указатель на CAN хэндлер
   на выходе   :  *
*/
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef* in_pCan)
{
   // handle transmit errors in all three mailboxes
   if(in_pCan->ErrorCode & HAL_CAN_ERROR_TX_ALST0)
   {
      SET_BIT(in_pCan->Instance->sTxMailBox[0].TIR, CAN_TI0R_TXRQ);
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_TX_ALST0;

   } else if(in_pCan->ErrorCode & HAL_CAN_ERROR_TX_TERR0)
   {
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_EWG;
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_ACK;
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_TX_TERR0;
   }

   if(in_pCan->ErrorCode & HAL_CAN_ERROR_TX_ALST1)
   {
      SET_BIT(in_pCan->Instance->sTxMailBox[1].TIR, CAN_TI1R_TXRQ);
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_TX_ALST1;

   } else if(in_pCan->ErrorCode & HAL_CAN_ERROR_TX_TERR1)
   {
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_EWG;
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_ACK;
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_TX_TERR1;
   }

   if(in_pCan->ErrorCode & HAL_CAN_ERROR_TX_ALST2)
   {
      SET_BIT(in_pCan->Instance->sTxMailBox[2].TIR, CAN_TI2R_TXRQ);
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_TX_ALST2;

   } else if(in_pCan->ErrorCode & HAL_CAN_ERROR_TX_TERR2)
   {
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_EWG;
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_ACK;
      in_pCan->ErrorCode &= ~HAL_CAN_ERROR_TX_TERR2;
   }

   g_u32Error++;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//// Работа с шиной
/////////////////////////////////////////////////////////////////////////////////////////////////
/**
   Инициализация шины
   на входе    :  in_u8Num    - номер CAN интерфейса в списке
                  in_pHandle  - указатель на CAN интерфейс
                  in_pPort    - указатель на CAN порт
   на выходе   :  успешность инициализации
   примечание  :  микроконтроллер stm32f405vgt5 имеет два CAN порта. При этом CAN1 это master, а CAN2 это slave
                  это означает что CAN1 и CAN2 имеют блок фильтров один на двоих, то есть от 0 до 13 это фильтры
                  для CAN1, а фильтры от 14 по 27 это фильтры для CAN2
*/
bool CAN_Init(u8 in_u8Num, CAN_HandleTypeDef* in_pHandle, CCANPort* in_pPort)
{
   // Проверка возможности добавления в список
   bool l_bResult = (in_u8Num < sizeof(g_aLinkList) / sizeof(g_aLinkList[0]));
   if(l_bResult)
   {
      // Заполнение связи
      g_aLinkList[in_u8Num].m_pHandle = in_pHandle;
      g_aLinkList[in_u8Num].m_pPort = in_pPort;

      // Проверка наличия интерфейса и порта
      if(in_pHandle && in_pPort)
      {
         // Установка идентификатора
         in_pPort->SetCANID(0xFFF);
         in_pPort->SetTID(0x00);
         in_pPort->SetAddress(0);

         // Установка фильтра CAN, прием всех сообщений
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
         HAL_CAN_ConfigFilter(in_pHandle, &CAN_FilterInitStructure);
/*  
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
         HAL_CAN_ConfigFilter(&hcan2, &CAN_FilterInitStructure);

         g_CAN2.SetCANID(0);
         g_CAN2.SetTID(0x00);
         g_CAN2.SetAddress(0);
         g_CAN2.SetInBuffer(g_aCAN2InBuffer, sizeof(g_aCAN2InBuffer));
         g_CAN2.SetOutBuffer(g_aCAN2OutBuffer, sizeof(g_aCAN2OutBuffer));
*/
         HAL_CAN_Start(in_pHandle);
         //HAL_CAN_Start(&hcan2);

         HAL_CAN_ActivateNotification(in_pHandle,  CAN_IT_TX_MAILBOX_EMPTY | \
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
      }
   }
   return l_bResult;
}

/**
   Инициализация фильтров шины
   на входе    :  in_u8Num       - номер CAN порта в списке
                  in_u16CanID    - идентификатор CAN устройства
                  in_u8Address   - адрес CAN устройства
   на выходе   :  успешность
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
bool CAN_SetFilter(u8 in_u8Num, u16 in_u16CanID, u8 in_u8Address)
{
   // Проверка возможности добавления в список
   bool l_bResult = (in_u8Num < sizeof(g_aLinkList) / sizeof(g_aLinkList[0]));
   if(l_bResult)
   {
      l_bResult = false;
      // Получение интерфейса и порта
      CAN_HandleTypeDef* l_pHandle = g_aLinkList[in_u8Num].m_pHandle;
      CCANPort* l_pPort = g_aLinkList[in_u8Num].m_pPort;

      // Проверка наличия интерфейса и порта
      if(l_pHandle && l_pPort)
      {
         // Установка идентификатора
         l_pPort->SetCANID(in_u16CanID);
         //l_pPort->SetTID(0x00);
         l_pPort->SetAddress(in_u8Address);

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
         HAL_CAN_ConfigFilter(l_pHandle, &CAN_FilterInitStructure);
         
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
         HAL_CAN_ConfigFilter(l_pHandle, &CAN_FilterInitStructure);

         l_bResult = true;
      }
   }
   return l_bResult;
}

/**
   Отправка текущего CAN кадра
   на входе    :  in_u8Num - номер CAN порта в списке
   на выходе   :  *
*/
void CAN_SendFrame(u8 in_u8Num)
{
   if(in_u8Num < sizeof(g_aLinkList) / sizeof(g_aLinkList[0]))
      SendFrameComplited(g_aLinkList[in_u8Num].m_pHandle, 0);
}
