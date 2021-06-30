
#include "usart.h"
#include "string.h"
//#include "gpio.h"
#include "uart_1W_drv.h"
#include "LinkChan.h"
#include  "IridiumValue.h"

#define         sleep_speed_low    3200
#define         sleep_speed_high   6400
#define         reset_speed        7500     
#define         protokol_speed     93000
/**
* @brief This function handles DMA1 channel2 global interrupt.
*/
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */
  
  /* USER CODE END DMA1_Channel2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_tx);
  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */
  
  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel3 global interrupt.
*/
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */
  
  /* USER CODE END DMA1_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */
  
  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel4 global interrupt.
*/
void DMA1_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */
  
  /* USER CODE END DMA1_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */
  
  /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel5 global interrupt.
*/
void DMA1_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */
  
  /* USER CODE END DMA1_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */
  
  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
* @brief This function handles USART1 global interrupt.
*/
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
  
  /* USER CODE END USART1_IRQn 1 */
}

/**
* @brief This function handles USART3 global interrupt.
*/
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
  
  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */
  
  /* USER CODE END USART3_IRQn 1 */
}
const t_family_code list_family_code[5] ={{0,    "Ќет или неизвестное"},
                                          {0x28, "DS18B20,точность +/-0,5гр"},
                                          {0x01, "DS1990, 64-бит пам€ть"},
                                          {0x10, "DS18S20,точность +/-0,5гр"},
                                          {0x22, "DS1822, точность +/- 2гр"},};

static uint16_t sleep_speed;
static uint16_t sleep_len;
const uint8_t pSleepBuf[2]={0xFF,0xFF};
static uint8_t pBuf1[100],pBuf2[100];
static uint8_t pByteBuf1[12],pByteBuf2[12];

uint8_t docrc8(uint8_t *value, uint8_t len);
typedef struct time_slot {
  uint32_t metka;
  uint32_t delta;
  char    pos[8];
}time_slot;



enum enState{usIni=0, usWait,usScan,usGetTemp,usReadRom,usWaitReadRom,usSendReset,usConvertT,usSkip_Rom,usRead_Scrat,usMatch_Rom,usWaitMatch_Rom,
usWaitConvertT,usWaitSkipRom, usWaitReadScrat,usSearchRom,usWaitSearchRom, usReadBit2, usWaitReadBit2,usWaitReset,usSendChoise,usWaitChoise,usWaitWait};//,usWaitAfterReset
enum enDrv{udInit=0,udNone, udTrmReset,udRcvReset,udWait,udRcvWaitReset,udStartWait,udTxDataStart,udRxData, udDmaRXError};
#define                 presence       0xFF
#define                 absence        0
#define                 fromtx         0
#define                 fromrx         1
#define                 fromerr        2
#define                 fromini        3
#define                 max_dev_OW     4
typedef struct PIN_STRUCT {
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;
} PIN_STRUCT, * PTR_PIN_STRUCT;

const PIN_STRUCT const_pin_uart[2]={{GPIOA,GPIO_PIN_9},{GPIOB,GPIO_PIN_10}}; 

typedef struct tp_OW_cmd {
  uint8_t       cmd;
  uint8_t       length;
}tp_OW_cmd, * ptr_tp_OW_cmd;

#define ow_cmd_Read_Rom         0x33
#define ow_cmd_Skip_Rom         0xCC
#define ow_cmd_ConvertT         0x44
#define ow_cmd_Read_Scrat       0xBE
#define ow_cmd_Reset            0x0
#define ow_cmd_Match_Rom        0x55
#define ow_cmd_Search_Rom       0xF0

const tp_OW_cmd ow_Read_Rom = {ow_cmd_Read_Rom , 8};
const tp_OW_cmd ow_Skip_Rom = {ow_cmd_Skip_Rom, 0};
const tp_OW_cmd ow_ConvertT = {ow_cmd_ConvertT, 0};
const tp_OW_cmd ow_Read_Scrat = {ow_cmd_Read_Scrat, 9};
const tp_OW_cmd ow_Reset = {ow_cmd_Reset, 0};
const tp_OW_cmd ow_Match_Rom = {ow_cmd_Match_Rom, 0};
const tp_OW_cmd ow_Search_Rom = {ow_cmd_Search_Rom, 0};

const ptr_tp_OW_cmd ow_script_temp[] = {(ptr_tp_OW_cmd)&ow_Reset,(ptr_tp_OW_cmd)&ow_Match_Rom ,(ptr_tp_OW_cmd)&ow_ConvertT,(ptr_tp_OW_cmd)&ow_Reset,(ptr_tp_OW_cmd)&ow_Match_Rom ,(ptr_tp_OW_cmd)&ow_Read_Scrat};

const ptr_tp_OW_cmd ow_script_other[] = {(ptr_tp_OW_cmd)&ow_Reset,(ptr_tp_OW_cmd)&ow_Match_Rom ,(ptr_tp_OW_cmd)&ow_Read_Rom};
const uint8_t link_tag[2][4]={{
#ifdef  onwire_val_1w1    
  tg_lnk_1w1_1_value,tg_lnk_1w1_2_value,tg_lnk_1w1_3_value,tg_lnk_1w1_4_value
#else
   0,0,0,0
#endif     
} ,{
#ifdef  onwire_val_1w2       
tg_lnk_1w2_1_value,tg_lnk_1w2_2_value,tg_lnk_1w2_3_value,tg_lnk_1w2_4_value
#else
0,0,0,0
#endif
}
};

typedef struct tp_OW_dev {
  uint8_t               type;
  uint32_t              cnt_crc_error;
  uint64_t              uid;
  uint64_t              value;
  uint8_t               newvalue;
  float                 f_value;
  uint8_t               step;
  uint32_t              cnt_good;
}tp_OW_dev, *ptr_tp_OW_dev;

typedef struct tp_OW_search{
  uint32_t Srch,SrchStep;
  unsigned char ROM_NO[8];
  int LastDiscrepancy;
  int LastFamilyDiscrepancy;
  int LastDeviceFlag;
  unsigned char crc8;
  int id_bit_number;
  int last_zero, rom_byte_number, search_result;
  int id_bit, cmp_id_bit;
  int int_cnt_dev;
  unsigned char rom_byte_mask, search_direction;
  
}tp_OW_search, * ptr_tp_OW_search;

tp_OW_search OW_search_arr[2];

typedef struct tp_state_drv {
  UART_HandleTypeDef    *huart; 
  uint32_t               curr_state; 
  uint8_t               *pBuf;
  uint8_t               present;
  PTR_PIN_STRUCT        pin_uart;
  uint8_t               length;
  uint32_t               mashine_state;
  uint8_t               *pByteBuf;
  uint8_t               curr_OW_dev;
  uint8_t               cnt_OW_dev;
  uint32_t              cnt_sess_error;
  uint32_t              time_pause;
  uint32_t              pause_start;
  ptr_tp_OW_search      OW_search;
  tp_OW_dev             OW_dev[max_dev_OW];
}tp_state_drv, * ptr_tp_state_drv;

typedef struct tp_state_protokol {
  ptr_tp_state_drv       drv_1w;
  uint32_t               curr_state;             
}tp_state_protokol, * ptr_tp_state_protokol;

tp_state_drv line[2]= {{&huart1, udInit, &pBuf1[0],absence,(PTR_PIN_STRUCT) &const_pin_uart[0],0,usIni,&pByteBuf1[0],0,0,0,0,0,&OW_search_arr[0]},
{&huart3, udInit, &pBuf2[0],absence,(PTR_PIN_STRUCT) &const_pin_uart[1],0,usIni,&pByteBuf2[0],0,0,0,0,0,&OW_search_arr[1]}};


tp_state_protokol data_proto[2] ={{&line[0],usIni},{&line[1],usIni}};
void OWSearch(ptr_tp_state_drv curr_line);

//__root uint32_t c1,c2,c3,c4,c5,c6,c7,c8,c9,c10;

uint8_t setfindDS=0;
uint32_t addreeprom;

enum enSrch{stIni=0, stReset, stFindCmd,stSendFind,stReadFind,stFinal,stSendReset};
enum enSrchStep{stepFirst=0,stepNext};

void OW_bin_byte(uint8_t *src,uint8_t *dst, uint8_t length) {
  
  for (;length>0;length--) {
    * dst=0;
    uint8_t bool=1;
    for (uint8_t i=0; i<8; i++) {
      
      if (* src == 0xFF) 
        * dst |= (uint8_t) bool ;
      src++;
      bool <<= 1;
    }
    dst++;
  }
};
void OW_byte_bin(uint8_t *src,uint8_t *dst, uint8_t length) {
  for (;length>0;length--) {
    uint8_t bool=1; 
    for (uint8_t i=0; i<8; i++) {
      if (* src&bool)
        * dst= 0xFF;
      else
        * dst =0;    
      dst ++; 
      bool <<= 1;
    }
    src++;
  }
  
};
void UART_set_speed(UART_HandleTypeDef *huart,uint32_t speed) {
  
  assert_param(IS_UART_INSTANCE(huart->Instance));
  //insert pin off
  HAL_UART_Abort(huart);  
  huart->Init.BaudRate = speed;
  
  if (HAL_HalfDuplex_Init(huart) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  } 
  
};


void UART_enable_pin(ptr_tp_state_drv curr_line) {
  
  GPIO_InitTypeDef GPIO_InitStruct;
  HAL_GPIO_WritePin(curr_line->pin_uart->GPIOx,curr_line->pin_uart->GPIO_Pin, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = curr_line->pin_uart->GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(curr_line->pin_uart->GPIOx, &GPIO_InitStruct);
  

}
extern  void Wait_Sync_ADC(uint32_t);

void UART_disable_pin(ptr_tp_state_drv curr_line) {
  GPIO_InitTypeDef GPIO_InitStruct;
  HAL_GPIO_WritePin(curr_line->pin_uart->GPIOx,curr_line->pin_uart->GPIO_Pin, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = curr_line->pin_uart->GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD ;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(curr_line->pin_uart->GPIOx, &GPIO_InitStruct);

  

};

void UART_set_wait(ptr_tp_state_drv curr_line) {  
  assert_param(IS_UART_INSTANCE(curr_line->huart->Instance));
  
  UART_set_speed(curr_line->huart,sleep_speed);
  HAL_UART_IRQHandler(curr_line->huart);  
  HAL_UART_Receive_IT(curr_line->huart,&curr_line->pBuf[0], sleep_len);
  HAL_UART_Transmit_IT(curr_line->huart,(uint8_t *) &pSleepBuf[0], sleep_len);

}
void State_Script(ptr_tp_state_drv curr_line) {
  uint8_t curr_script;
//  add_all("sc01");
  if (curr_line->present==absence) {
    curr_line->cnt_OW_dev=0;
    curr_line->mashine_state=usSendReset;
//    add_all(usIni);
    curr_line->OW_search->Srch=0;
    curr_line->OW_search->SrchStep=0;
    curr_line->pause_start=0;
    return;
  }
//  add_all("sc02");
  if (curr_line->cnt_OW_dev==0) {
    //start search
    // curr_line->curr_OW_dev=0;
    //    curr_line->cnt_OW_dev=1;
    //     curr_line->mashine_state=usReadRom;
    
    OWSearch(curr_line); 
//    add_all(usIni);
    return;
  }
  if ((curr_line->pause_start!=0) ){
    if ((curr_line->pause_start<HAL_GetTick())|
        ((HAL_GetTick()-curr_line->pause_start)>= curr_line->time_pause)){
          curr_line->pause_start=0;
          curr_line->present=absence;
          return;
        }
    return; 
  }
  
  
//  add_all("sc03");
  uint8_t size_script; 
  ptr_tp_OW_cmd * ptr_curr_script;
  
  
  
  if ((curr_line->OW_dev[curr_line->curr_OW_dev].type==0x10) ||
      (curr_line->OW_dev[curr_line->curr_OW_dev].type==0x28) ||
        (curr_line->OW_dev[curr_line->curr_OW_dev].type==0x22)  ) {
          ptr_curr_script=(ptr_tp_OW_cmd *)ow_script_temp;
          //curr_script =  ow_script_temp[curr_line->OW_dev[curr_line->curr_OW_dev].step]->cmd;
          size_script=sizeof(ow_script_temp)/sizeof(ow_script_temp[0]);
        }
  else 
  {
    ptr_curr_script=(ptr_tp_OW_cmd *)ow_script_other;
    //curr_script =  ow_script_other[curr_line->OW_dev[curr_line->curr_OW_dev].step]->cmd;  
    size_script=sizeof(ow_script_other)/sizeof(ow_script_other[0]);
  }
//  add_all("sc031");
  if (curr_line->OW_dev[curr_line->curr_OW_dev].step>=size_script) {
    curr_line->OW_dev[curr_line->curr_OW_dev].step=0;
    curr_line->OW_dev[curr_line->curr_OW_dev].cnt_good++;
    curr_line->curr_OW_dev++;
    curr_line->OW_dev[curr_line->curr_OW_dev].step=0;
    if (curr_line->curr_OW_dev>=curr_line->cnt_OW_dev) {

      curr_line->curr_OW_dev=0;
      curr_line->OW_dev[curr_line->curr_OW_dev].step=0;
      curr_line->pause_start=HAL_GetTick();
      curr_line->mashine_state=usWait;
      return;     
    }
  }
  
  
  
//  add_all("sc032");
  if (curr_line->OW_dev[curr_line->curr_OW_dev].step==6)
    while (1)
    {};
  curr_script = ptr_curr_script[curr_line->OW_dev[curr_line->curr_OW_dev].step]->cmd;
//  add_all("sc04");

//  add_all("sc05");
  switch (curr_script) {
    //reset
  case ow_cmd_Reset:
    curr_line->mashine_state=usSendReset;
//    add_all("scSndRst");
    break;
    //ow_ConvertT
  case ow_cmd_ConvertT:
    curr_line->mashine_state=usConvertT; 
//    add_all("scConvrt");
    break;
    //ow_Read_Rom
  case ow_cmd_Read_Rom :
    curr_line->mashine_state=usReadRom; 
//    add_all("scReadRom");
    break;
    //ow_Skip_Rom
  case ow_cmd_Skip_Rom:
    curr_line->mashine_state=usSkip_Rom;   
//    add_all("scSkipRom");
    break;
    //ow_Read_Scrat
  case ow_cmd_Read_Scrat:
    curr_line->mashine_state=usRead_Scrat;  
//    add_all("scRdScrat");
    break;
    //Match_Rom
  case ow_cmd_Match_Rom:
    curr_line->mashine_state=usMatch_Rom; 
//    add_all("scMatchRom");
    break;
    
  }
  curr_line->OW_dev[curr_line->curr_OW_dev].step++;
//  add_all("sc06");
  
}
void State_Get_Mashine_Protokol(ptr_tp_state_drv curr_line) {
  switch (curr_line->mashine_state) {
  case  usReadBit2:
    curr_line->pBuf[1]=0xFF;
    curr_line->pBuf[2]=0xFF;
    curr_line->length=2; 
    curr_line->curr_state= udTxDataStart; 
    curr_line->mashine_state=usWaitReadBit2;    
    break;  
  case  usMatch_Rom:
//    add_all("smMatchRom");
    OW_byte_bin((uint8_t *)&ow_Match_Rom.cmd,&curr_line->pBuf[1],1);
    OW_byte_bin((uint8_t *)&curr_line->OW_dev[curr_line->curr_OW_dev].uid,&curr_line->pBuf[9],8);
    curr_line->length=(ow_Match_Rom.length+1+8)*8; 
    curr_line->curr_state= udTxDataStart;
    curr_line->mashine_state=usWaitMatch_Rom;
    break;  
  case usRead_Scrat:
//    add_all("smRDScrat");
    if (curr_line->present==presence) 
    {
      OW_byte_bin((uint8_t *)&ow_Read_Scrat.cmd,&curr_line->pBuf[1],1);
      for (uint8_t i=9; i<81;i++)
        curr_line->pBuf[i]=0xff; 
      curr_line->length=(ow_Read_Scrat.length+1)*8; 
      curr_line->curr_state= udTxDataStart;
      curr_line->mashine_state=usWaitReadScrat; 
    } 
    else
    {
      curr_line->cnt_OW_dev=0;
      State_Script(curr_line);
    }
    break;
  case usConvertT:
  case usSkip_Rom:  
  case usSearchRom:  
//    add_all("smC&S&S");
    if (curr_line->present==presence) 
    {
      if (usConvertT==curr_line->mashine_state) {
        OW_byte_bin((uint8_t *)&ow_ConvertT.cmd,&curr_line->pBuf[1],1);
        curr_line->mashine_state=usWaitConvertT; 
      }
      if (usSkip_Rom==curr_line->mashine_state) {
        OW_byte_bin((uint8_t *)&ow_Skip_Rom.cmd,&curr_line->pBuf[1],1);
        curr_line->mashine_state=usWaitSkipRom; 
      }   
      
      if (usSearchRom==curr_line->mashine_state) {
        OW_byte_bin((uint8_t *)&ow_Search_Rom.cmd,&curr_line->pBuf[1],1);
        curr_line->mashine_state=usWaitSearchRom; 
      }   
      curr_line->length=8; 
      curr_line->curr_state= udTxDataStart;
      
    }
    else
    {
      curr_line->cnt_OW_dev=0;
      State_Script(curr_line);
    }
    break;
  case  usReadRom:
//    add_all("smRDRom");
    if (curr_line->present==presence) 
    {
      OW_byte_bin((uint8_t *)&ow_Read_Rom.cmd,&curr_line->pBuf[1],1);
      for (uint8_t i=9; i<81;i++)
        curr_line->pBuf[i]=0xff; 
      curr_line->length=(ow_Read_Rom.length+1)*8; 
      curr_line->curr_state= udTxDataStart;
      curr_line->mashine_state=usWaitReadRom; 
    } else
    {
      curr_line->cnt_OW_dev=0;
      State_Script(curr_line);
    }
    break;
    
  case usWait:
//    add_all("smWait");
    curr_line->curr_state= udWait;
    curr_line->mashine_state=usWaitWait; 
    break;
  case usSendChoise:
    curr_line->curr_state= udTxDataStart;
    curr_line->mashine_state=usWaitChoise;     
    break;
  case usSendReset:
//    add_all("smSndRST");
    curr_line->curr_state= udTrmReset;
    curr_line->mashine_state=usWaitReset; 
    
    break;

  }
  
}
void State_Mashine_Protokol(ptr_tp_state_drv curr_line) {
  
  
  switch (curr_line->mashine_state) {
    
  case usWaitChoise:
  case usWaitWait:
  case usIni: 
  case usWaitReadBit2:
  case usWaitReset:
//    add_all("smIni");
    curr_line->mashine_state=usWait;
    State_Script(curr_line);
    State_Get_Mashine_Protokol(curr_line);
    break;
//  case usWaitAfterReset:
    
//    break;
  case usWaitMatch_Rom:
//    add_all("smWMatchR");
    OW_bin_byte(&curr_line->pBuf[0],&curr_line->pByteBuf[0],curr_line->length/8);
    if (curr_line->pByteBuf[0]==ow_Match_Rom.cmd) {
      //надо ли провер€ть корректность передачи адреса целиком?
      uint8_t *temp=(uint8_t *)& curr_line->OW_dev[curr_line->curr_OW_dev].uid;
      uint8_t bool= (*temp++==curr_line->pByteBuf[1]);
      bool &=(*temp++==curr_line->pByteBuf[2]);
      bool &=  (*temp++==curr_line->pByteBuf[3]);
      bool &=    (*temp++==curr_line->pByteBuf[4]);
      bool &=      (*temp++==curr_line->pByteBuf[5]);
      bool &=        (*temp++==curr_line->pByteBuf[6]);
      bool &=          (*temp++==curr_line->pByteBuf[7]);
      bool &=            (*temp++==curr_line->pByteBuf[8]);
      if (bool==0) {
        curr_line->cnt_sess_error++;
        curr_line->cnt_OW_dev=0; 
      }
      else
      {
//        add_all("smWtMthOk");
        //все ок
      }
      
    }
    else
    {
      curr_line->cnt_sess_error++;
      curr_line->cnt_OW_dev=0;
//      add_all("smWtMthEr");
    };
    curr_line->mashine_state=usWait;
    State_Script(curr_line);
    State_Get_Mashine_Protokol(curr_line);
    break;
    
  case   usWaitReadScrat:
//    add_all("smWRDScrat");
    OW_bin_byte(&curr_line->pBuf[0],&curr_line->pByteBuf[0],curr_line->length/8);
    
    if (curr_line->pByteBuf[9]==docrc8(&curr_line->pByteBuf[1],8))
    {
//      add_all("smWRDS02");
      uint8_t *temp=(uint8_t *)& curr_line->OW_dev[curr_line->curr_OW_dev].value;
      memcpy(temp,&curr_line->pByteBuf[1],8);
      /*    *temp++=curr_line->pByteBuf[1];
      *temp++= curr_line->pByteBuf[2];
      *temp++= curr_line->pByteBuf[3];
      *temp++= curr_line->pByteBuf[4];
      *temp++= curr_line->pByteBuf[5];
      *temp++= curr_line->pByteBuf[6];
      *temp++= curr_line->pByteBuf[7];
      *temp++= curr_line->pByteBuf[8];*/
//      add_all("smWRDS03");
      uint16_t temp_u16=curr_line->OW_dev[curr_line->curr_OW_dev].value & 0xFFFF;
      float temp_float=0;
      float temp_prev=curr_line->OW_dev[curr_line->curr_OW_dev].f_value;
      switch (curr_line->OW_dev[curr_line->curr_OW_dev].type) {
      case 0x28:
      case 0x22: temp_float=0.0625f; break;
      case 0x10: temp_float=0.5f; break;
      }
      if (temp_u16& 0x8000) {
        //температура отрицательна€
        temp_u16 =~temp_u16+1;
        curr_line->OW_dev[curr_line->curr_OW_dev].f_value=-(temp_u16*temp_float);
      } else
      {
        //температура  положительна€
        curr_line->OW_dev[curr_line->curr_OW_dev].f_value=temp_u16*temp_float;
      }
      if (temp_prev!=curr_line->OW_dev[curr_line->curr_OW_dev].f_value)
        curr_line->OW_dev[curr_line->curr_OW_dev].newvalue=1;
    } //crc ok
    else
    {
      
   //   curr_line->OW_dev[curr_line->curr_OW_dev].value=0; //??
      curr_line->OW_dev[curr_line->curr_OW_dev].cnt_crc_error++;
    }
    
    curr_line->mashine_state=usWait;
    
    State_Script(curr_line);
    State_Get_Mashine_Protokol(curr_line);
    
    break;
    
  case usWaitConvertT:
  case usWaitSkipRom: 
  case usWaitSearchRom: 
//    add_all("smWC&S&S");
    OW_bin_byte(&curr_line->pBuf[0],&curr_line->pByteBuf[0],curr_line->length/8);
    if ((curr_line->pByteBuf[0]==ow_ConvertT.cmd)|
        (curr_line->pByteBuf[0]==ow_Skip_Rom.cmd) |
          (curr_line->pByteBuf[0]==ow_Search_Rom.cmd))
    {
      //все ок
    } else
    {
      curr_line->cnt_sess_error++;
      curr_line->cnt_OW_dev=0;
      State_Script(curr_line);
    }
    curr_line->mashine_state=usWait;
    State_Script(curr_line);
    State_Get_Mashine_Protokol(curr_line);
    break;    
    
  case  usWaitReadRom:
//    add_all("smWRDROM");
    OW_bin_byte(&curr_line->pBuf[0],&curr_line->pByteBuf[0],curr_line->length/8);
    if (curr_line->pByteBuf[8]==docrc8(&curr_line->pByteBuf[1],7))
    {
      uint8_t *temp=(uint8_t *)& curr_line->OW_dev[curr_line->curr_OW_dev].uid;
      memcpy(temp,&curr_line->pByteBuf[1],8);
      /*   *temp++=curr_line->pByteBuf[1];
      *temp++= curr_line->pByteBuf[2];
      *temp++= curr_line->pByteBuf[3];
      *temp++= curr_line->pByteBuf[4];
      *temp++= curr_line->pByteBuf[5];
      *temp++= curr_line->pByteBuf[6];
      *temp++= curr_line->pByteBuf[7];
      *temp++= curr_line->pByteBuf[8];*/
      /*curr_line->OW_dev[curr_line->curr_OW_dev].type=curr_line->pByteBuf[1];
      temp=(uint8_t *)& curr_line->OW_dev[curr_line->curr_OW_dev].s_uid;
      memcpy(temp,&curr_line->pByteBuf[2],4);
       *temp++=curr_line->pByteBuf[2];
      *temp++= curr_line->pByteBuf[3];
      *temp++= curr_line->pByteBuf[4];
      *temp++= curr_line->pByteBuf[5];*/
      
      
      
    } //crc ok
    else
    {
      curr_line->OW_dev[curr_line->curr_OW_dev].uid=0;
    //  curr_line->OW_dev[curr_line->curr_OW_dev].s_uid=0;
      curr_line->OW_dev[curr_line->curr_OW_dev].cnt_crc_error++;
    }
    curr_line->mashine_state=usWait;
    State_Script(curr_line);
    State_Get_Mashine_Protokol(curr_line);
    break;
    
    
    
  }
};
void DRV_Transmit(ptr_tp_state_drv curr_line) {
  if (HAL_GPIO_ReadPin(curr_line->pin_uart->GPIOx,curr_line->pin_uart->GPIO_Pin)== GPIO_PIN_RESET)
  {
    curr_line->curr_state=udWait;
    curr_line->present=absence;
    curr_line->cnt_sess_error++;
    curr_line->curr_state= udWait;
    curr_line->mashine_state= usIni;
  }  
  switch (curr_line->curr_state) {
    
    
  case udTrmReset:
//    add_all("drRST");
    UART_enable_pin(curr_line);
    UART_set_speed(curr_line->huart,reset_speed);
    curr_line->pBuf[0]=0;
    curr_line->pBuf[1]=0xF0;
 //   HAL_UART_IRQHandler(curr_line->huart);  
    if (HAL_OK!=HAL_UART_Receive_DMA(curr_line->huart, &curr_line->pBuf[0], 1))
      while (1)
      {};
    if (HAL_OK!=HAL_UART_Transmit_DMA(curr_line->huart, &curr_line->pBuf[1], 1))
      while (1)
      {};   
    curr_line->curr_state= udRcvReset;
//    curr_line->present=absence; 
    break;
    
  case udWait:
//    add_all("drWait"); 
    UART_disable_pin(curr_line);
    UART_set_wait(curr_line);    
    break;
    
  case udTxDataStart:
//    add_all("drTxData");
    UART_set_speed(curr_line->huart,protokol_speed);
    UART_enable_pin(curr_line);
 //   HAL_UART_IRQHandler(curr_line->huart); 
    if (HAL_OK!=HAL_UART_Receive_DMA(curr_line->huart, &curr_line->pBuf[0], curr_line->length))
      while (1)
      {};
    if (HAL_OK!=HAL_UART_Transmit_DMA(curr_line->huart, &curr_line->pBuf[1], curr_line->length))
      while (1)
      {};
    
    curr_line->curr_state=udRxData;  
    
    break;   
  } 
  
  
  
};
void State_Mashine_DRV(ptr_tp_state_drv curr_line, uint8_t from) {
  
  assert_param(IS_UART_INSTANCE(curr_line));
  
  
  switch (from) {
    
  case fromtx:
    switch (curr_line->curr_state) {
    case udWait:
//      add_all("drRxWait"); 
      curr_line->curr_state= udWait; 
      HAL_UART_Abort(curr_line->huart);  
      State_Mashine_Protokol(curr_line);
      DRV_Transmit(curr_line); 
      break;
    }
    break;
  case fromrx:
    switch (curr_line->curr_state) {
    case udInit: 
//      add_all("drINI");
      curr_line->present=absence;
      curr_line->cnt_OW_dev=0;
      curr_line->curr_state= udWait;
      State_Mashine_Protokol(curr_line);
      DRV_Transmit(curr_line);
      break;
    case udRcvReset:
//      add_all("drRcRST");     
      if ((curr_line->pBuf[0]<=0xE0)&&(curr_line->pBuf[0]>=0x10))
        curr_line->present=presence;
      else
      {
        curr_line->present=absence;
              curr_line->curr_state= udWait; 
                    DRV_Transmit(curr_line);   
                    break;
      }
      curr_line->curr_state= udWait;        
      State_Mashine_Protokol(curr_line);
      DRV_Transmit(curr_line);       
      break;
      
    case udRxData:  
//      add_all("drRxData");    
      curr_line->curr_state= udWait;  
      State_Mashine_Protokol(curr_line);
      DRV_Transmit(curr_line);        
      break;
      
    case udWait:
//      add_all("drRxWait"); 
      curr_line->curr_state= udWait;  
      State_Mashine_Protokol(curr_line);
      DRV_Transmit(curr_line);             
    }   
    
    
    break;
  case fromerr:
//    add_all("drDMAerr");
    HAL_UART_Abort(curr_line->huart);   
    curr_line->present=absence;
    curr_line->cnt_sess_error++;
    curr_line->curr_state= udWait;
    curr_line->mashine_state= usIni;
    State_Mashine_Protokol(curr_line);
    DRV_Transmit(curr_line);  
    break;
  case fromini:
    curr_line->present=absence;
    curr_line->curr_state= udWait;
    State_Mashine_Protokol(curr_line);
    DRV_Transmit(curr_line);     
    
  default:
    
    break;
  }
  
  
  
  
  
};

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart==&huart1) {
//    add_all("irqTX");  
    State_Mashine_DRV(&line[0],fromtx);
    
  }
  if (huart==&huart3) {
    
    State_Mashine_DRV(&line[1],fromtx);
    
  }
};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart==&huart1) {
//    add_all("irqRX");
    
    State_Mashine_DRV(&line[0],fromrx);
    
  }
  if (huart==&huart3) {
    
    State_Mashine_DRV(&line[1],fromrx);
    
  }
  
  
};

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  if (huart==&huart1) {
//    add_all("irqErr");
    volatile uint8_t temp;
    temp= huart->Instance->DR;
    if (huart->ErrorCode!=HAL_UART_ERROR_NONE) {
      
      State_Mashine_DRV(&line[0],fromerr);
      }
  }
  if (huart==&huart3) {
//    add_all("irqErr");
    volatile uint8_t temp;
    temp= huart->Instance->DR;
    if (huart->ErrorCode!=HAL_UART_ERROR_NONE) {
      State_Mashine_DRV(&line[1],fromerr);    
    
    }
  }
  
  
}
void Drv_1W_Start(uint32_t addr) {
  addreeprom = addr;
  //ReadUidFromEeprom(addr);
      __HAL_RCC_DMA1_CLK_ENABLE();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 1, 200);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 1, 200);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 200);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 200);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
 __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT );
 __HAL_DMA_DISABLE_IT(&hdma_usart1_tx, DMA_IT_HT );
 __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT );
 __HAL_DMA_DISABLE_IT(&hdma_usart3_tx, DMA_IT_HT );
  
  uint32_t Sysclk=HAL_RCC_GetSysClockFreq();
  if (Sysclk>48000000)
  {
    sleep_speed=sleep_speed_high;
    sleep_len=50; 
  }
  else
  {
    sleep_speed=sleep_speed_low;
    sleep_len=50; 
  }
  //  line[0].curr_state=udTrmReset;
  //  line[1].curr_state=udTrmReset;  
  //  UART_set_wait(&huart1);
  State_Mashine_DRV(&line[0],fromini);
//  State_Mashine_DRV(&line[1],fromini);  
  
}

static unsigned char dscrc_table[] = {
  0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
  157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
  35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
  190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
  70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
  219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
  101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
  248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
  140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
  17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
  175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
  50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
  202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
  87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
  233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
  116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

//--------------------------------------------------------------------------
// Calculate the CRC8 of the byte value provided with the current 
// global 'crc8' value. 
// Returns current global crc8 value
//
uint8_t docrc8(uint8_t *value, uint8_t len)
{
  uint8_t crc8=0;
  for (;len>0;len--) {
    crc8 = dscrc_table[crc8 ^ *value];
    value++;
  }
  return crc8;
}



// definitions
#define FALSE 0
#define TRUE  1

// method declarations



// global search state
unsigned char do_crc8(unsigned char value,unsigned char  crc8)
{
  // See Application Note 27
  
  // TEST BUILD
  crc8 = dscrc_table[crc8 ^ value];
  return crc8;
}
//enum enSrch{stIni=0, stReset, stFindCmd,stSendFind,stReadFind,stFinal,stSendReset};
//enum enSrchStep{stepFirst=0,stepNext};


void OWSearch(ptr_tp_state_drv curr_line)
{
//  if (setfindDS==0)
//    return;
  
  if (curr_line->OW_search->Srch==stIni)
    switch (curr_line->OW_search->SrchStep)  {
    case stepFirst:
      curr_line->OW_search->LastDiscrepancy = 0;
      curr_line->OW_search->LastDeviceFlag = FALSE;
      curr_line->OW_search->LastFamilyDiscrepancy = 0;
      curr_line->OW_search->int_cnt_dev=0;
      
      curr_line->OW_search->id_bit_number = 1;
      curr_line->OW_search->last_zero = 0;
      curr_line->OW_search->rom_byte_number = 0;
      curr_line->OW_search->rom_byte_mask = 1;
      curr_line->OW_search->search_result = 0;
      curr_line->OW_search->crc8 = 0;
      curr_line->OW_search->Srch=stFindCmd;
      curr_line->curr_OW_dev=0;
//      curr_line->cnt_OW_dev=0;
      curr_line->OW_search->Srch=stSendReset;
      curr_line->mashine_state=usSendReset; 
      break;
    case stepNext:
      curr_line->OW_search->id_bit_number = 1;
      curr_line->OW_search->last_zero = 0;
      curr_line->OW_search->rom_byte_number = 0;
      curr_line->OW_search->rom_byte_mask = 1;
      curr_line->OW_search->search_result = 0;
      curr_line->OW_search->crc8 = 0;   
      curr_line->OW_search->Srch=stSendReset;
      curr_line->mashine_state=usSendReset; 
      break;
    }
  
  // initialize for search
  if ( curr_line->OW_search->Srch==stSendReset)
    curr_line->OW_search->Srch=stFindCmd;
  else
    // if the last call was not the last one
    
    // issue the search command 
    if (curr_line->OW_search->Srch==stFindCmd) {
      curr_line->mashine_state=usSearchRom; 
      curr_line->OW_search->Srch=stSendFind;
    } else
      
      // loop to do the search
      if (curr_line->OW_search->Srch==stSendFind)
      {
        // read a bit and its complement
        curr_line->mashine_state=usReadBit2;
        curr_line->OW_search->Srch=stReadFind;
      } else
        if (curr_line->OW_search->Srch==stReadFind)
        {
          curr_line->OW_search->id_bit = curr_line->pBuf[0]==0xFF;
          curr_line->OW_search->cmp_id_bit = curr_line->pBuf[1]==0xFF;
          // check for no devices on 1-wire
          if (!((curr_line->OW_search->id_bit == 1) && (curr_line->OW_search->cmp_id_bit == 1)))
            
          {
            // all devices coupled have 0 or 1
            if (curr_line->OW_search->id_bit != curr_line->OW_search->cmp_id_bit)
              curr_line->OW_search->search_direction =curr_line->OW_search->id_bit;  // bit write value for search
            else
            {
              // if this discrepancy if before the Last Discrepancy
              // on a previous next then pick the same as last time
              if (curr_line->OW_search->id_bit_number < curr_line->OW_search->LastDiscrepancy)
                curr_line->OW_search->search_direction = ((curr_line->OW_search->ROM_NO[curr_line->OW_search->rom_byte_number] & curr_line->OW_search->rom_byte_mask) > 0);
              else
                // if equal to last pick 1, if not then pick 0
                curr_line->OW_search->search_direction = (curr_line->OW_search->id_bit_number == curr_line->OW_search->LastDiscrepancy);
              
              // if 0 was picked then record its position in LastZero
              if (curr_line->OW_search->search_direction == 0)
              {
                curr_line->OW_search->last_zero = curr_line->OW_search->id_bit_number;
                
                // check for Last discrepancy in family
                if (curr_line->OW_search->last_zero < 9)
                  curr_line->OW_search->LastFamilyDiscrepancy = curr_line->OW_search->last_zero;
              }
            }
            
            // set or clear the bit in the ROM byte rom_byte_number
            // with mask rom_byte_mask
            if (curr_line->OW_search->search_direction == 1)
              curr_line->OW_search->ROM_NO[curr_line->OW_search->rom_byte_number] |= curr_line->OW_search->rom_byte_mask;
            else
              curr_line->OW_search->ROM_NO[curr_line->OW_search->rom_byte_number] &= ~curr_line->OW_search->rom_byte_mask;
            
            // serial number search direction write bit
            if (curr_line->OW_search->search_direction==1) 
              curr_line->pBuf[1]=0xFF;
            else
              curr_line->pBuf[1]=0x0;
            curr_line->length=1; 
            curr_line->mashine_state= usSendChoise;
            //    OWWriteBit(search_direction);
            
            // increment the byte counter id_bit_number
            // and shift the mask rom_byte_mask
            curr_line->OW_search->id_bit_number++;
            curr_line->OW_search->rom_byte_mask <<= 1;
            
            // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
            if (curr_line->OW_search->rom_byte_mask == 0)
            {
              do_crc8(curr_line->OW_search->ROM_NO[curr_line->OW_search->rom_byte_number],curr_line->OW_search->crc8);  // accumulate the CRC
              curr_line->OW_search->rom_byte_number++;
              curr_line->OW_search->rom_byte_mask = 1;
            }
            if (curr_line->OW_search->rom_byte_number < 8)
              curr_line->OW_search->Srch=stSendFind;
            else
              curr_line->OW_search->Srch=stFinal;            
          }
          
          else
          {
            curr_line->OW_search->Srch=stIni;
            curr_line->OW_search->SrchStep=stepFirst;
          }
          
        }
  // while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7
  if ( curr_line->OW_search->Srch==stFinal) {
    // if the search was successful then
    if (!((curr_line->OW_search->id_bit_number < 65) || (curr_line->OW_search->crc8 != 0)))
    {
      // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
      curr_line->OW_search->LastDiscrepancy = curr_line->OW_search->last_zero;
      
      // check for last device
      if (curr_line->OW_search->LastDiscrepancy == 0)
        curr_line->OW_search->LastDeviceFlag = TRUE;
      
      curr_line->OW_search->search_result = TRUE;
    }
    
    
    // if no device found then reset counters so next 'search' will be like a first
    if (!curr_line->OW_search->search_result || !curr_line->OW_search->ROM_NO[0])
    {
      curr_line->OW_search->LastDiscrepancy = 0;
      curr_line->OW_search->LastDeviceFlag = FALSE;
      curr_line->OW_search->LastFamilyDiscrepancy = 0;
      curr_line->OW_search->search_result = FALSE;
    }
    if ((curr_line->OW_search->search_result==TRUE)&
        ((curr_line->OW_search->ROM_NO[0]==0x22)|(curr_line->OW_search->ROM_NO[0]==0x28)|
         (curr_line->OW_search->ROM_NO[0]==0x10)))
    {
      uint64_t *temp=(uint64_t *)& curr_line->OW_search->ROM_NO[0];
      curr_line->OW_dev[curr_line->OW_search->int_cnt_dev].uid=* temp;
      curr_line->OW_dev[curr_line->OW_search->int_cnt_dev].type=curr_line->OW_search->ROM_NO[0];
     /*
      uint8_t *ttemp= (uint8_t *)&curr_line->OW_dev[curr_line->OW_search->int_cnt_dev].s_uid;
      ttemp[0]=curr_line->OW_search->ROM_NO[1];
      ttemp[1]=curr_line->OW_search->ROM_NO[2];
      ttemp[2]=curr_line->OW_search->ROM_NO[3];
      ttemp[3]=curr_line->OW_search->ROM_NO[4];
      */
      curr_line->OW_search->int_cnt_dev++;
      if (curr_line->OW_search->int_cnt_dev>=max_dev_OW)
      {
        curr_line->cnt_OW_dev=4;
        //WriteUidToEeprom(addreeprom);
        setfindDS=0;
      } else
      {
        curr_line->OW_search->SrchStep=stepNext;  
        curr_line->OW_search->Srch=stIni;
      }
      if (curr_line->OW_search->LastDeviceFlag == TRUE ){
        curr_line->cnt_OW_dev=curr_line->OW_search->int_cnt_dev;
        curr_line->OW_search->SrchStep=stepFirst;
        if (curr_line->cnt_OW_dev<max_dev_OW)
          for (int j=max_dev_OW; j>curr_line->cnt_OW_dev; j--)
          {
            curr_line->OW_dev[j-1].uid=0;
            curr_line->OW_dev[j-1].value=0;
            curr_line->OW_dev[j-1].cnt_good=0;
          }
      }
    }
    else
    {
      curr_line->cnt_OW_dev=curr_line->OW_search->int_cnt_dev;
      curr_line->OW_search->SrchStep=stepFirst;
    }
    
  }
  
}



bool GetTemp1W(universal_value_t *value, u32 Param) 
{
   bool Result = 0;
   t_dev_1w   *   DEV = (t_dev_1w *)&Param; 
   if(DEV->line < 2 && DEV->device < max_dev_OW)
   {
      value->m_f32Value = line[DEV->line].OW_dev[DEV->device].f_value;
      Result = 1;
   }

   return Result;
}
bool SetTemp1W(universal_value_t *value, u32 Param) 
{
   bool Result = 0;
   t_dev_1w   *   DEV = (t_dev_1w *)&Param; 
   if(DEV->line < 2 && DEV->device < max_dev_OW)
   {
      line[DEV->line].OW_dev[DEV->device].f_value = value->m_f32Value;
      Result = 1;
   }

   return Result;
}


bool GetUID1W(universal_value_t *value, u32 Param) 
{
   bool Result = 0;
   t_dev_1w   *   DEV = (t_dev_1w *)&Param; 
   if(DEV->line < 2 && DEV->device < max_dev_OW)
   {
      value->m_u64Value= line[DEV->line].OW_dev[DEV->device].uid;
      Result = 1;
   }

   return Result;
}
bool SetUID1W(universal_value_t *value, u32 Param) 
{
   bool Result = 0;
   t_dev_1w   *   DEV = (t_dev_1w *)&Param; 
   if(DEV->line < 2 && DEV->device < max_dev_OW)
   {
      line[DEV->line].OW_dev[DEV->device].uid = value->m_u64Value;
      Result = 1;
   }

   return Result;
}

bool GetCntGood(universal_value_t *value, u32 Param) 
{
   bool Result = 0;
   t_dev_1w   *   DEV = (t_dev_1w *)&Param; 
   if(DEV->line < 2 && DEV->device < max_dev_OW)
   {
      value->m_u32Value= line[DEV->line].OW_dev[DEV->device].cnt_good;
      Result = 1;
   }

   return Result;
}
bool SetCntGood(universal_value_t *value, u32 Param) 
{
   bool Result = 0;
   t_dev_1w   *   DEV = (t_dev_1w *)&Param; 
   if(DEV->line < 2 && DEV->device < max_dev_OW)
   {
      line[DEV->line].OW_dev[DEV->device].cnt_good =value->m_u32Value;
      Result = 1;
   }

   return Result;
}


bool GetCountDev(universal_value_t *value, u32 Param) 
{
   bool Result = 0;
   t_dev_1w   *   DEV = (t_dev_1w *)&Param; 
   if(DEV->line < 2 && DEV->device < max_dev_OW)
   {
      value->m_u32Value= line[DEV->line].cnt_OW_dev;
      Result = 1;
   }

   return Result;
}
bool SetCountDev(universal_value_t *value, u32 Param) 
{
   return 0;
}



bool GetTypeDev(universal_value_t *value, u32 Param) 
{
   bool Result = 0;
   t_dev_1w   *   DEV = (t_dev_1w *)&Param; 
   if(DEV->line < 2 && DEV->device < max_dev_OW)
   {
      value->m_u32Value= line[DEV->line].OW_dev[DEV->device].type;
      Result = 1;
   }

   return Result;
}
bool SetTypeDev(universal_value_t *value, u32 Param)
{
  return 0;
}

bool GetMaxDev(universal_value_t *value, u32 Param) 
{
   bool Result = 0;
   t_dev_1w   *   DEV = (t_dev_1w *)&Param; 
   if(DEV->line < 2 && DEV->device < max_dev_OW)
   {
      value->m_u32Value= max_dev_OW;
      Result = 1;
   }

   return Result;
}
bool SetMaxDev(universal_value_t *value, u32 Param)
{
  return 0;
}


uint32_t  concate1WParam(uint32_t line, uint32_t device) {
  uint32_t  * return_val;
  t_dev_1w      test;
  test.device= device;
  test.line= line;
  return_val  =  (uint32_t *)&test;
  return * return_val;
};

const char* GetFamily( u32 Param) {
  for (uint32_t i=0; i<sizeof(list_family_code)/sizeof(list_family_code[0]); i++) 
    if (Param == list_family_code[i].code)
      return list_family_code[i].refer;
  return list_family_code[0].refer;
};
