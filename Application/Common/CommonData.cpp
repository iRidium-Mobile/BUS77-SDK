#include "CommonData.h"

/**
   Инициализация общего описания каналов управления и обратной связи
   на входе    :  out_pDesc   - указатель на заполняемую структуру
                  in_u8Type   - тип переменной
                  in_pszDesc  - указатель на текстовое описание
                  in_pRange   - указатель на данные с минимальным, максимальным и шаговым значением
                  in_u16Units - единицы измерения
   на выходе   :  размер значения
*/
void InitDescription(iridium_description_t* in_pDesc, u8 in_u8Type, const char* in_pszDesc, const void* in_pRange, u16 in_u16Units)
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
      case IVT_TIME:
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
size_t GetValueSize(u8 in_u8Type, universal_value_t& in_rValue)
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
