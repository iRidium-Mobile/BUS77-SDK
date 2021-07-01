# Подготовка аппаратной части потребуется:
* Устройство на базе микроконтроллера stm32l452ccu.
* USB-to-CAN преобразователь https://github.com/iRidiumMobileLTD/BUS-77/tree/master/Application/STM32/STM32F103C8T6/GateUARTtoCAN

# Настройка инструментов разработки.
* Скачать и установить среду разработки Keil для ARM с официального сайта https://www.keil.com. В этой среде будет производится компиляция проекта.
* Скачать и установить STM32CubeMX с официального сайта https://www.st.com/en/development-tools/stm32cubemx.html. С помощью данного инструмента будет производиться настройка проекта.
* Скачать исходные коды iRidium SDK с github по ссылке https://github.com/iRidiumMobileLTD/BUS-77.
 
# Изготовление шлюза для программирования шины.
* Собрать схему шлюза. Пример на сайте: https://dev.iridi.com/Bus77_USB
* Открыть и скомпилировать программу шлюза, файл проект находится Example\STM32\STM32F103C8T6\GateUARTtoCAN\MDK-ARM\GateUARTtoCAN.uvprojx
Изготовление конечного устройства.
 
# Сборка проекта и загрузка проекта на микроконтроллер.
 Для этого надо открыть и скомпилировать загрузчик, файл проекта находится https://github.com/iRidiumMobileLTD/BUS-77/tree/master/Application/STM32/STM32L452CCU/Example/Dimmer/Bootloader
* С помощью Keil загрузить загрузчик на микроконтроллер.
* Для этого надо открыть и скомпилировать прошивку, файл проекта находится https://github.com/iRidiumMobileLTD/BUS-77/tree/master/Application/STM32/STM32L452CCU/Example/Dimmer/Firmware
* С помощью утилиты config.exe (находится в директории Utility) загрузить созданную прошивку на микроконтроллер.
* После компиляции с помощью среды разработки keil загрузить созданную прошивку на микроконтроллер.
