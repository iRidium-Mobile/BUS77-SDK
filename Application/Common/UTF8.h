#ifndef _C_UTF8_H_INCLUDED_
#define _C_UTF8_H_INCLUDED_

// Включения
#include "IridiumTypes.h"

// Установить корректное окончание UTF 8 строки
void SetValidEndByUTF8(char* in_pszString, size_t in_stSize, size_t in_stMaxSize);

// Проверка метода установки корректного окончания UTF 8 строки
//bool TEST_SetValidEndByUtf8(size_t in_stMaxSize);

#endif   // _C_UTF8_H_INCLUDED_
