/**
 * Marlin 3D Printer Firmware
 *
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 * Copyright (c) 2016 Bob Cousins bobcousins42@googlemail.com
 * Copyright (c) 2015-2016 Nico Tonnhofer wurstnase.reprap@gmail.com
 * Copyright (c) 2017 Victor Perez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#define CPU_32_BIT

#include "../../inc/MarlinConfigPre.h"

#include "../shared/Marduino.h"
#include "../shared/math_32bit.h"
#include "../shared/HAL_SPI.h"

#include "fastio.h"
#include "watchdog.h"

#include <stdint.h>

#if defined(STM32F4) && USBCON
  #include <USBSerial.h>
#endif

// ------------------------
// Defines
// ------------------------

// Serial override
//extern HalSerial usb_serial;

#define _MSERIAL(X) SerialUART##X
#define MSERIAL(X) _MSERIAL(X)
#define SerialUART0 Serial1

#if defined(STM32F4) && SERIAL_PORT == 0
  #error "SERIAL_PORT cannot be 0. (Port 0 does not exist.) Please update your configuration."
#elif SERIAL_PORT == -1
  #define MYSERIAL0 SerialUSB
#elif WITHIN(SERIAL_PORT, 0, 6)
  #define MYSERIAL0 MSERIAL(SERIAL_PORT)
#else
  #error "SERIAL_PORT must be from -1 to 6. Please update your configuration."
#endif

#ifdef SERIAL_PORT_2
  #if defined(STM32F4) && SERIAL_PORT_2 == 0
    #error "SERIAL_PORT_2 cannot be 0. (Port 0 does not exist.) Please update your configuration."
  #elif SERIAL_PORT_2 == -1
    #define MYSERIAL1 SerialUSB
  #elif WITHIN(SERIAL_PORT_2, 0, 6)
    #define MYSERIAL1 MSERIAL(SERIAL_PORT_2)
  #else
    #error "SERIAL_PORT_2 must be from -1 to 6. Please update your configuration."
  #endif
#endif

#ifdef SERIAL_PORT_3
  #if defined(STM32F4) && SERIAL_PORT_3 == 0
    #error "SERIAL_PORT_3 cannot be 0. (Port 0 does not exist.) Please update your configuration."
  #elif SERIAL_PORT_3 == -1
    #define MYSERIAL2 SerialUSB
  #elif WITHIN(SERIAL_PORT_3, 0, 6)
    #define MYSERIAL2 MSERIAL(SERIAL_PORT_3)
  #else
    #error "SERIAL_PORT_3 must be from -1 to 6. Please update your configuration."
  #endif
#endif

#ifdef LCD_SERIAL_PORT
  #if defined(STM32F4) && LCD_SERIAL_PORT == 0
    #error "LCD_SERIAL_PORT cannot be 0. (Port 0 does not exist.) Please update your configuration."
  #elif LCD_SERIAL_PORT == -1
    #define LCD_SERIAL SerialUSB
  #elif WITHIN(LCD_SERIAL_PORT, 0, 6)
    #define LCD_SERIAL MSERIAL(LCD_SERIAL_PORT)
  #else
    #error "LCD_SERIAL_PORT must be from -1 to 6. Please update your configuration."
  #endif
#endif

/**
 * TODO: review this to return 1 for pins that are not analog input
 */
#ifndef analogInputToDigitalPin
  #define analogInputToDigitalPin(p) (p)
#endif

#define CRITICAL_SECTION_START()  uint32_t primask = __get_PRIMASK(); __disable_irq()
#define CRITICAL_SECTION_END()    if (!primask) __enable_irq()
#define ISRS_ENABLED() (!__get_PRIMASK())
#define ENABLE_ISRS()  __enable_irq()
#define DISABLE_ISRS() __disable_irq()
#define cli() __disable_irq()
#define sei() __enable_irq()

// On AVR this is in math.h?
#define square(x) ((x)*(x))

#ifndef strncpy_P
  #define strncpy_P(dest, src, num) strncpy((dest), (src), (num))
#endif

// Fix bug in pgm_read_ptr
#undef pgm_read_ptr
#define pgm_read_ptr(addr) (*(addr))

// ------------------------
// Types
// ------------------------

typedef int8_t pin_t;

#ifdef STM32F4
  #define HAL_SERVO_LIB libServo
#endif

// ------------------------
// Public Variables
// ------------------------

// Result of last ADC conversion
extern uint16_t HAL_adc_result;

// ------------------------
// Public functions
// ------------------------

// Memory related
#define __bss_end __bss_end__

inline void HAL_init() {}

// Clear reset reason
void HAL_clear_reset_source();

// Reset reason
uint8_t HAL_get_reset_source();

inline void HAL_reboot() {}  // reboot the board or restart the bootloader

void _delay_ms(const int delay);

/*
extern "C" {
  int freeMemory();
}
*/

extern "C" char* _sbrk(int incr);

/*
int freeMemory() {
  volatile int top;
  top = (int)((char*)&top - reinterpret_cast<char*>(_sbrk(0)));
  return top;
}
*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static inline int freeMemory() {
  volatile char top;
  return &top - reinterpret_cast<char*>(_sbrk(0));
}

#pragma GCC diagnostic pop

//
// ADC
//

#define HAL_ANALOG_SELECT(pin) pinMode(pin, INPUT)

inline void HAL_adc_init() {}

#define HAL_ADC_VREF         3.3
#define HAL_ADC_RESOLUTION  10
#define HAL_START_ADC(pin)  HAL_adc_start_conversion(pin)
#define HAL_READ_ADC()      HAL_adc_result
#define HAL_ADC_READY()     true

void HAL_adc_start_conversion(const uint8_t adc_pin);
uint16_t HAL_adc_get_result();

#define GET_PIN_MAP_PIN(index) index
#define GET_PIN_MAP_INDEX(pin) pin
#define PARSED_PIN_INDEX(code, dval) parser.intval(code, dval)

#ifdef STM32F4
  #define JTAG_DISABLE() afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY)
  #define JTAGSWD_DISABLE() afio_cfg_debug_ports(AFIO_DEBUG_NONE)
#endif
