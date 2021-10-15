#ifndef _COMMON_DATA_H_INCLUDED_
#define _COMMON_DATA_H_INCLUDED_

// Включения
#include "IridiumTypes.h"
#include "IridiumValue.h"
#include "Iridium.h"

#define CF_W   0x40                                // Признак изменения канала по PIN коду
#define CF_R   0x20                                // Признак чтения канала по PIN коду

// Данные канала обратной связи
typedef struct device_tag_s
{
   u32         m_u32ID;                            // Идентификатор канала
   const char* m_pszName;                          // Указатель на имя канала
   u8          m_u8Type;                           // Тип значения канала обратной связи
   const void* m_pRange;                           // Указатель на минимаьное, максимальное и шаговое значение
   const char* m_pszDesc;                          // Указатель на описание канала
   u16         m_u16SubdeviceID;                   // Идентификатор типа подустройства
   u8          m_u8FunctionID;                     // Идентификатор функции канала
   u8          m_u8GroupID;                        // Группа к которой принадлежит канал
   u16         m_u16Units;                         // Единицы измерения
} device_tag_t;

// Данные канала управления
typedef struct device_channel_s
{
   u32         m_u32ID;                            // Идентификатор канала
   const char* m_pszName;                          // Указатель на имя канала
   u8          m_u8Type;                           // Тип значения канала обратной связи
   const void* m_pRange;                           // Указатель на минимаьное, максимальное и шаговое значение
   const char* m_pszDesc;                          // указатель на описание канала
   u8          m_u8Flags;                          // Список флагов
   u16         m_u16SubdeviceID;                   // Идентификатор типа подустройства
   u8          m_u8FunctionID;                     // Идентификатор функции канала
   u8          m_u8GroupID;                        // Группа к которой принадлежит канал
   u16         m_u16Units;                         // Единицы измерения
   u8          m_u8MaxVariables;                   // Максимальное количество глобальных переменных на канал управления
} device_channel_t;

// Методы для работы с структурами
void InitDescription(iridium_description_t* in_pDesc, u8 in_u8Type, const char* in_pszDesc, const void* in_pRange, u16 in_u16Units);
size_t GetValueSize(u8 in_u8Type, universal_value_t& in_rValue);

#endif   // _COMMON_DATA_H_INCLUDED_
