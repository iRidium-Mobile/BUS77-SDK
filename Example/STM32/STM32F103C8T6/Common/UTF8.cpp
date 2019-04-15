#include "UTF8.h"

/**
   Установка валидного окончания UTF строки
   на входе    :  in_pszString   - указатель на строку
                  in_stSize      - предпологаемый размер
   на выходе   :  *
*/
void SetValidEndByUTF8(char* in_pszString, size_t in_stSize, size_t in_stMaxSize)
{
   int8_t  l_s8Octets = 0;      // Колличество октет для символа UTF
   uint8_t l_u8OctetStart = 0;  // Позиция начального октета символа
   uint8_t i = 0;               // Позиция символа
   for(;(i < in_stMaxSize) && (i < in_stSize); i++)
   {
      // Если симол UTF8
      if(in_pszString[i] > 127)
      {
         
         if((in_pszString[i] >> 5) == 6)          // Если двух октетный символ          
         {  
            // Если предыдущий символ завершен назначим новое колличество последующих октет
            l_s8Octets = !l_s8Octets ? 1 : -1;
            // Прервем цикл если предыдущий символ не завершен или не хватает места для продолжения
            if(i+1 >= in_stMaxSize || !l_s8Octets)
               break;
            // Запомним позицию начала символа
            l_u8OctetStart = i;
         } else if((in_pszString[i] >> 4) == 14)   // Если трех октетный символ
         {
            l_s8Octets = !l_s8Octets ? 2 : -1; 
            
            if(i+2 >= in_stMaxSize || !l_s8Octets)
               break;

            l_u8OctetStart = i;
         } else if((in_pszString[i] >> 3) == 30)   // Если четырех октетный символ
         {
            l_s8Octets = !l_s8Octets ? 3 : -1; 

            if(i+3 >= in_stMaxSize || !l_s8Octets)
               break;

            l_u8OctetStart = i;
         } else 
               l_s8Octets--;

         // Если следующих пакетов было больше чем определено начальным октетом
         if(l_s8Octets < 0)
         {
            l_u8OctetStart = i;
            break;
         }
      }
   }
   in_pszString[l_s8Octets < 0 ? l_u8OctetStart : i] = 0;
}


/**
   Тест обработки конца UTF8 строки
   на входе    :  *
   на выходе   :  успешность
*//*
bool TEST_SetValidEndByUtf8(size_t in_stMaxSize)
{   
   #define TESTSIZE 14
   bool l_Result = true;
   char g_szName[in_stMaxSize + 1];
   
   char l_TestValues[TESTSIZE][40] = {
      "ИИИИИИИИИИИИИИИР",
      "И1ИИИИИИИИИИИИИИР",
      "￥￥￥￥￥￥￥￥￥￥",
      "异 异 异异异异异异异异异",
      {0xD0,0x98},
      {0xEF,0xBF,0xA5,0xA5,0xBF,0xA5,0xEF,0xBF,0xA5,0xEF,0xBF,0xA5},
      {0xEF,0xBF,0xA5,0xEF,0xBF,0xA5,0xEF},
      {0xEF,0xBF,0xA5,0xEF,0xBF,0xA5,0xEF,0xBF,0xA5,0xEF,0xBF,0xA5,0xA5},
      {0xD0},
      {0x98},
      {0xF0, 0xB2, 0xB2, 0xB2},
      {0xF0, 0xB2, 0xB2, 0xB2, 0xF0, 0xB2, 0xB2},
      {0xF0, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2},
      {0xB2, 0xF0, 0xB2, 0xB2, 0xB2}
   };
   
   char l_ControlValues[TESTSIZE][40] = {
      "ИИИИИИИИИИИИИИИР",
      "И1ИИИИИИИИИИИИИИ",
      "￥￥￥￥￥￥￥￥￥",
      "异 异 异异异异异异异异",
      "И",
      "￥",
      "￥￥",
      "￥￥￥￥",
      "",
      "",
      {0xF0, 0xB2, 0xB2, 0xB2},
      {0xF0, 0xB2, 0xB2, 0xB2},
      {0xF0, 0xB2, 0xB2, 0xB2},
      ""
   };
   
   for(u8 i = 0; i < TESTSIZE; i++)
   {
      memcpy(g_szName, 0, in_stMaxSize + 1);  
      memcpy(g_szName, l_TestValues[i], strlen(l_TestValues[i]));
      
      SetValidEndByUtf8(g_szName, in_stMaxSize);
      
      for(u8 j = 0; j < strlen(l_ControlValues[i]); j++)
      {
         if(g_szName[j] != l_ControlValues[i][j])
            l_Result = false;
      }
   }
     
   return l_Result;
}*/
