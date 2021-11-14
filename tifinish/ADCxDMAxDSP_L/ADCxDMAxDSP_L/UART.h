/*
 * UART.h
 *
 *  Created on: 2021年11月9日
 *      Author: Administrator
 */

#ifndef UART_H_
#define UART_H_
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "stdio.h" //1.61328125kb

#ifdef __TI_COMPILER_VERSION__
//CCS骞冲彴
#include "stdarg.h"
#include "string.h"
#define USART3_MAX_SEND_LEN     600                 //鏈�澶у彂閫佺紦瀛樺瓧鑺傛暟
int printf(const char *str, ...);
#endif


#ifdef __clang__
  typedef __builtin_va_list va_list;
  #define va_start(ap, param) __builtin_va_start(ap, param)
  #define va_end(ap)          __builtin_va_end(ap)
  #define va_arg(ap, type)    __builtin_va_arg(ap, type)
  #if __STDC_VERSION__ >= 199900L || __cplusplus >= 201103L || !defined(__STRICT_ANSI__)
  #define va_copy(dest, src)  __builtin_va_copy(dest, src)
  #endif
#else
  #ifdef __TARGET_ARCH_AARCH64
    typedef struct __va_list {
      void *__stack;
      void *__gr_top;
      void *__vr_top;
      int __gr_offs;
      int __vr_offs;
    } va_list;
  //#else
    //typedef struct __va_list { void *__ap; } va_list;
// #endif
//void uart_init(uint32_t baudRate);


#endif /* UART_H_ */

#endif


#endif
