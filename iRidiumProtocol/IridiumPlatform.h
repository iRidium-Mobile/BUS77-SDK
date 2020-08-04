/*******************************************************************************
 * Copyright (c) 2013-2019 iRidi Ltd. www.iridi.com
 *
 * Все права зарегистрированы. Эта программа и сопровождающие материалы доступны
 * на условиях Eclipse Public License v2.0 и Eclipse Distribution License v1.0,
 * которая сопровождает это распространение. 
 *
 * Текст Eclipse Public License доступен по ссылке
 *    http://www.eclipse.org/legal/epl-v20.html
 * Текст Eclipse Distribution License доступн по ссылке
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Участники:
 *    Марат Гилязетдинов, Сергей Королёв  - первая версия
 *******************************************************************************/
#ifndef _IRIDIUM_PLATFORM_H_INCLUDED_
#define _IRIDIUM_PLATFORM_H_INCLUDED_

// Windows x86/x86-64
#if defined(_WIN32) || defined(WIN32) || defined(WIN64) || defined(_WIN64)
   #define IRIDIUM_WINDOWS_PLATFORM
   #ifndef WIN32_LEAN_AND_MEAN
      #define WIN32_LEAN_AND_MEAN
   #endif

   #include <windows.h>

   #if defined(WIN64) || defined(_WIN64)
      #define IRIDIUM_ARCH_64
   #else
      #define IRIDIUM_ARCH_32
   #endif
#else
   #include <string.h>
#endif

// Mac OSX & iOS
#if defined(__APPLE__)
   #include <sys/types.h>

   #include "TargetConditionals.h"
   #if TARGET_OS_IOS
      #define IRIDIUM_IOS_PLATFORM
      //#warning IOS
   #elif TARGET_OS_TV
      #define IRIDIUM_IOS_PLATFORM
      //#warning TV
   #elif TARGET_OS_WATCH
      #define IRIDIUM_IOS_PLATFORM
      //#warning WATCH
   #else
      #define IRIDIUM_OSX_PLATFORM
      //#warning OSX
   #endif

   #if __LP64__
      #define IRIDIUM_ARCH_64
   #else
      #define IRIDIUM_ARCH_32
   #endif
#endif

// Android
#if defined(__ANDROID__)
   #define IRIDIUM_ANDROID_PLATFORM
#endif   // defined(__ANDROID__)

// AVR или Arduino
#if defined(AVR)
   #define IRIDIUM_MCU_PLATFORM
   #define IRIDIUM_MCU_AVR
#endif   // defined(AVR)

// Cortex-M0/1/3/4/7
#if \
   defined(ARM_MATH_CM0) || \
   defined(ARM_MATH_CM3) || \
   defined(ARM_MATH_CM4) || \
   defined(__TARGET_CPU_CORTEX_M0) || \
   defined(__TARGET_CPU_CORTEX_M3) || \
   defined(__TARGET_CPU_CORTEX_M4)
   #define IRIDIUM_MCU_PLATFORM
   #define IRIDIUM_MCU_CORTEX_M
#else
// Сделано специально для Atollic TrueSTUDIO
// STM32F0xx
#if \
   defined (STM32F030x6) || defined (STM32F030x8) || \
   defined (STM32F030xC) || \
   defined (STM32F031x6) || \
   defined (STM32F038xx) || \
   defined (STM32F042x6) || \
   defined (STM32F048xx) || \
   defined (STM32F051x8) || \
   defined (STM32F058xx) || \
   defined (STM32F070x6) || defined (STM32F070xB) || \
   defined (STM32F071xB) || \
   defined (STM32F072xB) || \
   defined (STM32F078xx) || \
   defined (STM32F091xC) || \
   defined (STM32F098xx)
   #define IRIDIUM_MCU_PLATFORM
   #define IRIDIUM_MCU_CORTEX_M
#endif

// STM32F1xx
#if \
   defined (STM32F100xB) || defined (STM32F100xE) || \
   defined (STM32F101x6) || defined (STM32F101xB) || defined (STM32F101xE) || defined (STM32F101xG) || \
   defined (STM32F102x6) || defined (STM32F102xB) || \
   defined (STM32F103x6) || defined (STM32F103xB) || defined (STM32F103xE) || defined (STM32F103xG) || \
   defined (STM32F105xC) || \
   defined (STM32F107xC)
   #define IRIDIUM_MCU_PLATFORM
   #define IRIDIUM_MCU_CORTEX_M
#endif


// STM32F2xx
#if \
   defined (STM32F205xx) || defined (STM32F215xx) || \
   defined (STM32F207xx) || defined (STM32F217xx)
   #define IRIDIUM_MCU_PLATFORM
   #define IRIDIUM_MCU_CORTEX_M
#endif

// STM32F3xx
#if \
   defined (STM32F301x8) || \
   defined (STM32F302x8) || defined (STM32F302xC) || defined (STM32F302xE) || \
   defined (STM32F303x8) || defined (STM32F303xC) || defined (STM32F303xE) || \
   defined (STM32F318xx) || \
   defined (STM32F328xx) || \
   defined (STM32F334x8) || \
   defined (STM32F358xx) || \
   defined (STM32F373xC) || \
   defined (STM32F378xx) || \
   defined (STM32F398xx)
   #define IRIDIUM_MCU_PLATFORM
   #define IRIDIUM_MCU_CORTEX_M
#endif

// STM32F4xx
#if \
   defined (STM32F405xx) || \
   defined (STM32F415xx) || \
   defined (STM32F407xx) || \
   defined (STM32F417xx) || \
   defined (STM32F427xx) || \
   defined (STM32F437xx) || \
   defined (STM32F429xx) || \
   defined (STM32F439xx) || \
   defined (STM32F401xC) || defined (STM32F401xE) || \
   defined (STM32F410Tx) || defined (STM32F410Cx) || defined (STM32F410Rx) || \
   defined (STM32F411xE) || \
   defined (STM32F446xx) || \
   defined (STM32F469xx) || \
   defined (STM32F479xx) || \
   defined (STM32F412Cx) || defined (STM32F412Rx) || defined (STM32F412Vx) || defined (STM32F412Zx) || \
   defined (STM32F413xx) || \
   defined (STM32F423xx)
   #define IRIDIUM_MCU_PLATFORM
   #define IRIDIUM_MCU_CORTEX_M
#endif
#endif

// Linux
#if \
   !defined(IRIDIUM_ANDROID_PLATFORM) && \
   !defined(IRIDIUM_WINDOWS_PLATFORM) && \
   !defined(IRIDIUM_OSX_PLATFORM) && \
   !defined(IRIDIUM_IOS_PLATFORM) && \
   !defined(IRIDIUM_MCU_PLATFORM)
   #define IRIDIUM_LINUX_PLATFORM
#endif   // define IRIDIUM_LINUX_PLATFORM

// Порядок байт в GCC
#if defined(_BYTE_ORDER)

   #if _BYTE_ORDER == _LITTLE_ENDIAN
      #define IRIDIUM_BYTE_ORDER IRIDIUM_LITTLE_ENDIAN
   #elif _BYTE_ORDER == _BIG_ENDIAN
      #define IRIDIUM_BYTE_ORDER IRIDIUM_BIG_ENDIAN
   #endif

#elif defined(__BYTE_ORDER)

   #if __BYTE_ORDER == __LITTLE_ENDIAN
      #define IRIDIUM_BYTE_ORDER IRIDIUM_LITTLE_ENDIAN
   #elif __BYTE_ORDER == __BIG_ENDIAN
      #define IRIDIUM_BYTE_ORDER IRIDIUM_BIG_ENDIAN
   #endif

#elif defined(__BYTE_ORDER__)

   #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      #define IRIDIUM_BYTE_ORDER IRIDIUM_LITTLE_ENDIAN
   #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      #define IRIDIUM_BYTE_ORDER IRIDIUM_BIG_ENDIAN
   #endif

// Порядок байт в MSVC
#elif defined(REG_DWORD)

   #if REG_DWORD == REG_DWORD_LITTLE_ENDIAN
      #define IRIDIUM_BYTE_ORDER IRIDIUM_LITTLE_ENDIAN
   #elif REG_DWORD == REG_DWORD_BIG_ENDIAN
      #define IRIDIUM_BYTE_ORDER IRIDIUM_BIG_ENDIAN
   #endif

#endif

// Проверка определения порядка байт
#if !defined(IRIDIUM_BYTE_ORDER)
   #if defined(IRIDIUM_MCU_CORTEX_M)
      #define IRIDIUM_BYTE_ORDER IRIDIUM_LITTLE_ENDIAN
   #else
      #error "byte order is not defined!"
   #endif
#endif

// Тип операционной системы
enum eOSType
{
   OST_UNKNOWN = 0,                                // Неизвестная операционная система
   OST_IOS,                                        // Apple iOS
   OST_WINDOWS,                                    // Windows x86/x86-64
   OST_WINDOWS_MOBILE,                             // Windows CE
   OST_MAC_OSX,                                    // Mac OS X
   OST_LINUX,                                      // Linux
   OST_ANDROID,                                    // Android
   OST_NONE,                                       // Без операционной системы
   OST_FREE_RTOS,                                  // Free RTOS
};

// Тип процессора
enum eProcessorType
{
   PT_UNKNOWN = 0,                                 // Неизвестная архитектура
   PT_X86,                                         // Intel x86
   PT_X86_64,                                      // Intel/AMD x86-64
   PT_ARM,                                         // ARM (Сюда же входят и микроконтроллеры на базе чипа Cortex-M0, Cortex-M3, Cortex-M4)
   PT_ARM_64,                                      // ARM 64
   PT_MIPS,                                        // Mips
   PT_AVR,                                         // AVR
};

// Класс устройства
enum eDeviceClassType
{
   DCT_UNKNOWN = 0,                                // Неизвестный тип устройства
   DCT_PHONE,                                      // Телефон
   DCT_TABLET,                                     // Планшет
   DCT_CONSOLE,                                    // Консоль (Apple TV)
   DCT_PC,                                         // Персональный компьютер
   DCT_PLC,                                        // Программируемый логический контроллер (сервер)
   DCT_MICROCONTROLLER,                            // Микроконтроллер
};

#if defined PLATFORM_x86
#define LINUX_TARGET_x86
#elif defined PLATFORM_x64
#define LINUX_TARGET_x64
#elif defined PLATFORM_RPI
#define LINUX_TARGET_RPI
#elif defined PLATFORM_GC
#define LINUX_TARGET_GC
#elif defined PLATFORM_LM || defined PLATFORM_LM2
#define LINUX_TARGET_LM
#elif defined PLATFORM_UMC
#define LINUX_TARGET_UMC
#elif defined PLATFORM_WB
#define LINUX_TARGET_WB
#elif defined PLATFORM_BS
#define LINUX_TARGET_BS
#elif defined PLATFORM_UC
#define LINUX_TARGET_UC
#elif defined PLATFORM_AT
#define LINUX_TARGET_AT
#elif defined PLATFORM_AA
#define LINUX_TARGET_AA
#else
#ifdef IRIDIUM_LINUX_PLATFORM
#warning "Linux defined but subplatform not"
#endif
#endif

#if defined LINUX_TARGET_RPI
#define IRIDIUM_OPENGL_ES
#endif

#endif   // _IRIDIUM_PLATFORM_H_INCLUDED_
