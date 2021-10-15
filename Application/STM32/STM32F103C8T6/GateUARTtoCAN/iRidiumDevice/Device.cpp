#include "main.h"
#include "Device.h"
#include "stm32f1xx_hal.h"
#include "string.h"

#define START_MARKER       0xAA                    // Маркер начала пакета
#define END_MARKER         0x55                    // Маркер конца пакета
#define ESCAPE_MARKER      0xA5                    // Маркер экранирования

#define EXT_ID_FLAG        0x80000000
#define EXT_ID_MASK        0x7fffffff
#define STD_ID_MASK        0x7ff

// Структура фрейма
typedef struct can_frame_s
{
   uint32_t m_u32ExtID;                            // Идентификатор стандарта CAN-2.0B
   uint8_t  m_u8Size;                              // Размер данных
   uint8_t  m_aData[8];                            // Полезная нагрузка CAN пакета
} can_frame_t;

// Структура для получения пакета
typedef struct uart_spliter_s
{
   uint8_t     m_u8Index;
   uint8_t     m_aBuffer[32];
   bool        m_bReadPacket;
   uint8_t     m_u8Old;
} uart_spliter_t;

class CRingBuffer
{
public:
   // Конструктор/деструктор
   CRingBuffer();
   virtual ~CRingBuffer();

   // Установка буфера
   void SetBuffer(void* in_pBuffer, size_t in_stSize);

   // Добавление/удаление CAN фрейма
   bool AddFrame(can_frame_t* in_pFrame);
   void SkipFrame();

   // Получение CAN фрейма
   can_frame_t* GetFrame();

protected:
   size_t         m_stMax;                         // Максимальное количество элементов
   size_t         m_stStart;                       // Первый элемент
   size_t         m_stSize;                        // Количество элементов
   can_frame_t*   m_pBuffer;                       // Указатель на буфер с данными
};


// Внешние структуры
extern CAN_HandleTypeDef   hcan;                   // Структура для работы с CAN
extern UART_HandleTypeDef  huart2;                 // Струкутра для работы с UART

static CanTxMsgTypeDef     g_aCanTxMessage;        // Структура для отправляемого сообщения
static CanRxMsgTypeDef     g_aCanRxMessage;        // Структура для принимаемого сообщения

bool                       g_bUARTTransmit = false;
uint8_t                    g_u8UartByte;
CRingBuffer                g_UARTtoCAN;
can_frame_t                g_aUARTtoCAN[512];

bool                       g_bCANTransmit = false;
CRingBuffer                g_CANtoUART;
can_frame_t                g_aCANtoUART[512];
uint8_t                    g_aBuffer[64];

uart_spliter_t             g_Spliter;

uint32_t g_u32CAN_RCV_Error = 0;
uint32_t g_u32CAN_RCV = 0;
uint32_t g_u32CAN_SND_Error = 0;
uint32_t g_u32CAN_SND = 0;

uint32_t g_u32UART_RCV_Error = 0;
uint32_t g_u32UART_RCV = 0;
uint32_t g_u32UART_SND_Error = 0;
uint32_t g_u32UART_SND = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Кольцевой буфер
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CRingBuffer::CRingBuffer()
{
   m_stMax     = 0;
   m_stStart   = 0;
   m_stSize    = 0; 
   m_pBuffer   = NULL;
}

CRingBuffer::~CRingBuffer()
{
}

// Установка буфера
void CRingBuffer::SetBuffer(void* in_pBuffer, size_t in_stSize)
{
   m_stMax     = in_stSize / sizeof(can_frame_t);
   m_stStart   = 0;
   m_stSize    = 0;
   m_pBuffer   = (can_frame_t*)in_pBuffer;
}

// Добавление/удаление CAN фрейма
bool CRingBuffer::AddFrame(can_frame_t* in_pFrame)
{
   bool l_bResult = false;
   if(m_stSize != m_stMax)
   {
      size_t l_stIndex = (m_stStart + m_stSize) % m_stMax;
      memcpy(m_pBuffer + l_stIndex, in_pFrame, sizeof(can_frame_t));
      m_stSize++;
      l_bResult = true;
   }
   return l_bResult;
}

// Сбросить кадр
void CRingBuffer::SkipFrame()
{
   if(m_stSize)
   {
      m_stStart = (m_stStart + 1) % m_stMax;
      m_stSize--;
   }
}

// Получение CAN кадра
can_frame_t* CRingBuffer::GetFrame()
{
   can_frame_t* l_pResult = NULL;
   if(m_stSize)
      l_pResult = m_pBuffer + m_stStart;
   return l_pResult;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Работа с CAN
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/**
Обработка приходящих фреймов из CAN шины в кольцевой буфер
   на входе    :  in_pCan  - указатель на структуру CAN
   на выходе   :  *
*/
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* in_pCan)
{
   can_frame_t l_Frame;
   memset(&l_Frame, 0, sizeof(l_Frame));

   // Извлечение полезной информации
   if(in_pCan->pRxMsg->IDE == CAN_ID_EXT)
      l_Frame.m_u32ExtID = in_pCan->pRxMsg->ExtId | EXT_ID_FLAG;
   else
      l_Frame.m_u32ExtID = in_pCan->pRxMsg->StdId;
   
   // Получение размера данных
   l_Frame.m_u8Size = in_pCan->pRxMsg->DLC;
   
   // Скопируем полезную нагрузку
   memcpy(l_Frame.m_aData, in_pCan->pRxMsg->Data, l_Frame.m_u8Size);
   
   // Добавление полученого фрейма в буфер
   if(!g_CANtoUART.AddFrame(&l_Frame))
      g_u32CAN_RCV_Error++;
   
   g_u32CAN_RCV++;
   
   HAL_GPIO_TogglePin(Onboard_LED_GPIO_Port, Onboard_LED_Pin);

   // Включим прерыване обратно
   HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
}

/**
   Получение фреймов и отправка их в CAN
   на входе    :  *
   на выходе   :  *
*/
static bool SendToCAN()
{
   bool l_bResult = false;
   // Получим фрейм для отправки в CAN
   can_frame_t* l_pPtr = g_UARTtoCAN.GetFrame();
   if(l_pPtr)
   {
      if(l_pPtr->m_u32ExtID & EXT_ID_FLAG)
      {
         // Расширенный размер кадра
         hcan.pTxMsg->IDE = CAN_ID_EXT;
         hcan.pTxMsg->ExtId = l_pPtr->m_u32ExtID & EXT_ID_MASK;
      } else
      {
         // Стандартный размер кадра
         hcan.pTxMsg->IDE = CAN_ID_STD;
         hcan.pTxMsg->ExtId = l_pPtr->m_u32ExtID & STD_ID_MASK;
      }

      // Указываем длинну сообщения
      hcan.pTxMsg->DLC = l_pPtr->m_u8Size;
      
      // Копируем данные для отправки
      memcpy(hcan.pTxMsg->Data, l_pPtr->m_aData, l_pPtr->m_u8Size);
      
      g_u32CAN_SND++;
      
      HAL_GPIO_TogglePin(Onboard_LED_GPIO_Port, Onboard_LED_Pin);

      // Отправляем данные
      if(HAL_CAN_Transmit_IT(&hcan) == HAL_OK)
      {    
         // Если передача прошла успешно, удалим отправленный фрейм
         g_UARTtoCAN.SkipFrame();
         l_bResult = true;
      } else
         g_u32CAN_SND_Error++;
   }
   return l_bResult;
}

/**
   Обработка прерывания отправки
   на входе    :  in_pCan  - указатель на структуру CAN
   на выходе   :  *
*/
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* in_pCan)
{
   // Проверка свободен ли UART
   g_bCANTransmit = SendToCAN();
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef* in_pCanHandle)
{
   // Включим прерывание обратно
   HAL_CAN_Receive_IT(in_pCanHandle, CAN_FIFO0);
}

/**
   Отправка данных из UART буфера в CAN порт
   на входе    :  *
   на выходе   :  *
*/
void UARTtoCAN()
{
   // Проверка свободен ли UART
   if(!g_bCANTransmit)
      g_bCANTransmit = SendToCAN();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Работа с UART
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/**
   Обработчик получения одного байта из шины
   на входе    :  huart - указатель на хендлер UART
   на выходе   :  *
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
   // Проверка режима работы
   if(g_Spliter.m_bReadPacket)
   {
      // Проверка на экранирование символа
      if(g_Spliter.m_u8Old == ESCAPE_MARKER)
      {
         // Если получен маркер конца
         g_Spliter.m_aBuffer[g_Spliter.m_u8Index++] = g_u8UartByte;
         g_u8UartByte = 0;

         // Проверка на конец пакета
      } else if(g_Spliter.m_u8Old == END_MARKER)
      {
         if(g_u8UartByte == END_MARKER && g_Spliter.m_u8Index)
         {
            // Найден конец пакета, вычисление суммы пакета
            uint8_t l_u8LCR = 0;
            for(size_t i = 0; i < g_Spliter.m_u8Index - 1; i++)
               l_u8LCR += g_Spliter.m_aBuffer[i];

            // Проверка суммы
            if(g_Spliter.m_aBuffer[g_Spliter.m_u8Index - 1] == l_u8LCR)
            {
               can_frame_t l_Frame;
               // Идентификатор пакета
               l_Frame.m_u32ExtID = g_Spliter.m_aBuffer[3] << 24 | g_Spliter.m_aBuffer[2] << 16 | g_Spliter.m_aBuffer[1] << 8 | g_Spliter.m_aBuffer[0];
               
               // Размер данных
               l_Frame.m_u8Size = g_Spliter.m_aBuffer[4];

               // Получение данных пакета
               for(size_t i = 0; i < 8; i++)
                  l_Frame.m_aData[i] = g_Spliter.m_aBuffer[i + 5];
               
               g_u32UART_RCV++;
               
               // Добавление полученого фрейма в буфер
               if(!g_UARTtoCAN.AddFrame(&l_Frame))
                  g_u32UART_RCV_Error++;
            }
         }
         // Переход в режим поиска начала пакета
         g_Spliter.m_bReadPacket = false;
         g_Spliter.m_u8Index = 0;

      } else
      {
         // Проверка на начало пакета
         if(g_u8UartByte == START_MARKER)
         {
            if(g_Spliter.m_u8Old != START_MARKER)
            {
               // Переход в режим поиска начала пакета
               g_Spliter.m_bReadPacket = false;
               g_Spliter.m_u8Index = 0;
            }
         } else if(g_u8UartByte != END_MARKER && g_u8UartByte != ESCAPE_MARKER)
            g_Spliter.m_aBuffer[g_Spliter.m_u8Index++] = g_u8UartByte;
      }

   } else
   {
      // Поиск начала пакета
      if(g_Spliter.m_u8Old == START_MARKER && g_u8UartByte == START_MARKER)
      {
         g_Spliter.m_bReadPacket = true;
         g_Spliter.m_u8Index = 0;
      }
   }
   // Сохраним последний полученый байт для анализа последующего байта
   g_Spliter.m_u8Old = g_u8UartByte;
   
   HAL_UART_Receive_IT(huart, &g_u8UartByte, 1);
}

/**
   Получение фреймов и отправка их в CAN
   на входе    :  *
   на выходе   :  *
*/
static bool SendToUART()
{
   bool    l_bResult = false;
   uint8_t l_aTemp[32];
   
   can_frame_t* l_pFrame = g_CANtoUART.GetFrame();
   if(l_pFrame)
   {
      // Сериализация пакета
      uint8_t* l_pPtr = l_aTemp;
      uint32_t l_u32ID = l_pFrame->m_u32ExtID;
      uint8_t l_u8LRC = 0;
      
      // Добавление ID
      *l_pPtr++ = l_u32ID & 0xFF;
      l_u32ID >>= 8;
      *l_pPtr++ = l_u32ID & 0xFF;
      l_u32ID >>= 8;
      *l_pPtr++ = l_u32ID & 0xFF;
      l_u32ID >>= 8;
      *l_pPtr++ = l_u32ID & 0xFF;
      
      // Добавление размера полезной нагрузки
      *l_pPtr++ = l_pFrame->m_u8Size;
      
      // Добавление данных
      for(size_t i = 0; i < 8; i++)
         *l_pPtr++ = l_pFrame->m_aData[i];

      for(size_t i = 0; i < 13; i++)
         l_u8LRC += l_aTemp[i];
      *l_pPtr++ = l_u8LRC;

      // Сериализация данных с экранированием
      l_pPtr = g_aBuffer;
      *l_pPtr++ = START_MARKER;
      *l_pPtr++ = START_MARKER;

      for(size_t i = 0; i < 14; i++)
      {
         if(l_aTemp[i] == START_MARKER || l_aTemp[i] == END_MARKER || l_aTemp[i] == ESCAPE_MARKER)
            *l_pPtr++ = ESCAPE_MARKER;

         *l_pPtr++ = l_aTemp[i];
      }

      *l_pPtr++ = END_MARKER;
      *l_pPtr++ = END_MARKER;

      g_u32UART_SND++;
      
      // Отправим валидный найденый пакет в UART
      if(HAL_UART_Transmit_IT(&huart2, g_aBuffer, l_pPtr - g_aBuffer) == HAL_OK)
      {
         g_CANtoUART.SkipFrame();
         l_bResult = true;
      } else
         g_u32UART_SND_Error++;
   }
   return l_bResult;
}

/**
   Обработчик окончания передачи 
   на входе    :  huart - указатель на хендлер UART
   на выходе   :  *
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
   g_bUARTTransmit = SendToUART();
}

/**
   Отправка данных из CAN буфера в UART порт
   на входе    :  *
   на выходе   :  *
*/
void CANtoUART()
{
   // Проверка свободен ли UART
   if(!g_bUARTTransmit)
      g_bUARTTransmit = SendToUART();
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
   
   g_CANtoUART.SetBuffer(g_aCANtoUART, sizeof(g_aCANtoUART));
   g_UARTtoCAN.SetBuffer(g_aUARTtoCAN, sizeof(g_aUARTtoCAN));
   
   // Включим прерывание получения данных из UART
   HAL_UART_Receive_IT(&huart2, &g_u8UartByte, 1);
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
      //HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
   }
   // Отправка данных из CAN в UART
   CANtoUART();
   // Отправка данных из UART в CAN
   UARTtoCAN();
}
