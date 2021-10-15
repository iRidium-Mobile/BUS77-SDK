#ifndef _C_INDICATOR_H_INCLUDED_
#define _C_INDICATOR_H_INCLUDED_

// Включения
#include "IridiumTypes.h"

// Флаги цветов
#define INDICATOR_COLOR_RED   0x04                 // Красный цвет
#define INDICATOR_COLOR_GREEN 0x02                 // Зеленый цвет
#define INDICATOR_COLOR_BLUE  0x01                 // Синий цвет

// Список состояний для индикации
enum eIndicatorMode
{
   // Индикатор выключен
   INDICATOR_MODE_OFF   = 0,                       // Выключено
   INDICATOR_MODE_ON,                              // Включено
   INDICATOR_MODE_BOOTLOADER,                      // Работа в бутлоадере
   INDICATOR_MODE_WORK,                            // Обычный режим без ошибки
   // Группа 1
   INDICATOR_MODE_1_1   = 0x11,                    // -, .        Критическая ошибка/аппаратная неисправность (чтение памяти, инициализация трансивера)
   INDICATOR_MODE_1_2,                             // -, ..       Обрыв или КЗ
   INDICATOR_MODE_1_3,                             // -, ...      Зарезервировано
   INDICATOR_MODE_1_4,                             // -, ....     Зарезервировано
   INDICATOR_MODE_1_5,                             // -, .....    Общая ошибка линии
   // Группа 2
   INDICATOR_MODE_2_1   = 0x21,                    // --, .       Зарезервировано
   INDICATOR_MODE_2_2,                             // --, ..      Низкое напряжение питания устройства
   INDICATOR_MODE_2_3,                             // --, ...     Низкий заряд встроенной батареи
   INDICATOR_MODE_2_4,                             // --, ....    Перегрев устройства
   INDICATOR_MODE_2_5,                             // --, .....   Зарезервировано
   // Группа 3
   INDICATOR_MODE_3_1   = 0x31,                    // ---, .      Зарезервировано
   INDICATOR_MODE_3_2,                             // ---, ..     Зарезервировано
   INDICATOR_MODE_3_3,                             // ---, ...    Зарезервировано
   INDICATOR_MODE_3_4,                             // ---, ....   Зарезервировано
   INDICATOR_MODE_3_5,                             // ---, .....  Зарезервировано
   // Группа 4
   INDICATOR_MODE_4_1   = 0x41,                    // ----, .     Зарезервировано
   INDICATOR_MODE_4_2,                             // ----, ..    Зарезервировано
   INDICATOR_MODE_4_3,                             // ----, ...   Зарезервировано
   INDICATOR_MODE_4_4,                             // ----, ....  Зарезервировано
   INDICATOR_MODE_4_5,                             // ----, ..... Зарезервировано
   // Группа 5
   INDICATOR_MODE_5_1   = 0x51,                    // -----, .    Зарезервировано
   INDICATOR_MODE_5_2,                             // -----, ..   Зарезервировано
   INDICATOR_MODE_5_3,                             // -----, ...  Зарезервировано
   INDICATOR_MODE_5_4,                             // -----, .... Зарезервировано
   INDICATOR_MODE_5_5,                             // -----, .....Зарезервировано
};

class CIndicator
{
public:
   // Конструктор класса
   CIndicator();

   // Перегруженные методы
   virtual u32 GetTime() = 0;
      //{ return 0; }
   virtual void Set(u8 in_u8Color, bool in_bState) = 0;
      //{ }

   // Уставнока/получение режима
   void SetMode(eIndicatorMode in_u8Mode);
   eIndicatorMode GetMode()
      { return m_eMode; }
      
   // Установка/получение цвета индикатора
   void SetColor(u8 in_u8Color);
   u8 GetColor()
      { return m_u8Color; }
   // Установка времени мерцания
   void SetBlinkTime(u32 in_u32Time);
   u32 GetBlinkTime()
      { return m_u32Blink; }
   // Обработка индикатора
   void Work();

private:
   void Reset();

protected:
   u32            m_u32Time;                       // Время
   u8             m_u8Current;                     // Текущий индекс последовательности
   u8             m_u8Old;                         // Старое значение
   u16            m_aSequence[24];                 // Данные последовательности
   eIndicatorMode m_eMode;                         // Режим работы
   u8             m_u8Color;                       // Цвет
   u32            m_u32Blink;                      // Время мерцания
};

#endif   // _C_INDICATOR_H_INCLUDED_
