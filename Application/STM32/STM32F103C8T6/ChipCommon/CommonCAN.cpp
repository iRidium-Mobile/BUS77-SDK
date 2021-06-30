#include "CommonCAN.h"

// Структура для связывания CAN интерфейса с CAN портом
typedef struct can_link_s
{
   CAN_HandleTypeDef*   m_pHandle;                 // Указатель на интерфейс CAN
   CCANPort*            m_pPort;                   // Указатель на CAN порт
} can_link_t;

bool g_bCANTransmit = false;

static CanTxMsgTypeDef     g_aCanTxMessage;        // Структура для отправляемого сообщения
static CanRxMsgTypeDef     g_aCanRxMessage;        // Структура для принимаемого сообщения

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
   на входе    :  in_pCanHandle - указатель на структуру CAN
   на выходе   :  *
*/
void FrameReceived(CAN_HandleTypeDef* in_pCanHandle)
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

/**
   Получение фреймов и отправка их в CAN
   на входе    :  *
   на выходе   :  *
*/
void SendFrameComplited(CAN_HandleTypeDef* in_pCanHandle)
{
   bool l_bResult = false;
   // Получим фрейм для отправки в CAN
   // Получение указателя на CAN порт
   CCANPort* l_pPort = GetCANPort(in_pCanHandle);
   can_frame_t* l_pPtr = l_pPort->GetFrame();
   if(l_pPtr)
   {
      // Расширенный размер кадра
      in_pCanHandle->pTxMsg->IDE = CAN_ID_EXT;
      in_pCanHandle->pTxMsg->ExtId = l_pPtr->m_u32ExtID;

      // Указываем длинну сообщения
      in_pCanHandle->pTxMsg->DLC = l_pPtr->m_u8Size;
      
      // Копируем данные для отправки
      memcpy(in_pCanHandle->pTxMsg->Data, l_pPtr->m_aData, l_pPtr->m_u8Size);

      // Отправляем данные, если передача прошла успешно, удалим отправленный фрейм
      if(HAL_CAN_Transmit_IT(in_pCanHandle) == HAL_OK)
         l_pPort->DeleteFrame();

   } else
      l_pPort->SetTransmite(false);
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

/**
   Обработка прерывания отправки
   на входе    :  in_pCan  - указатель на структуру CAN
   на выходе   :  *
*/
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* in_pCan)
{
   // Проверка свободен ли UART
   SendFrameComplited(in_pCan);
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

         in_pHandle->pRxMsg = &g_aCanRxMessage;
         in_pHandle->pTxMsg = &g_aCanTxMessage;

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
         HAL_CAN_ConfigFilter(in_pHandle, &CAN_FilterInitStructure);

         HAL_CAN_Receive_IT(in_pHandle, CAN_FIFO0);
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
         l_pPort->SetAddress(in_u8Address);
         
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
         HAL_CAN_ConfigFilter(l_pHandle, &CAN_FilterInitStructure);
         
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
         HAL_CAN_ConfigFilter(l_pHandle, &CAN_FilterInitStructure);

         HAL_CAN_Receive_IT(l_pHandle, CAN_FIFO0);
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
      SendFrameComplited(g_aLinkList[in_u8Num].m_pHandle);
}
