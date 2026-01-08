/*
 * lib_rcc.h
 *
 *  Created on: Jan 6, 2026
 *      Author: shanghuang
 */

#ifndef LIB_RCC_H_
#define LIB_RCC_H_

#include "lib_keyword_define.h"

// Khai báo địa chỉ ngoại vi

#define SCB_REGS_BASEADDR 0xE000ED00ul
#define RCC_REGS_BASEADDR 0x40021000ul

// >> Nhóm địa chỉ thanh ghi cụ thể

#define SCB_AIRCR_REG_ADDR (SCB_REGS_BASEADDR + 0x0Cul)
#define RCC_CR_REG_ADDR (RCC_REGS_BASEADDR + 0x00ul)
#define RCC_CFGR_REG_ADDR (RCC_REGS_BASEADDR + 0x04ul)
#define RCC_CIR_REG_ADDR (RCC_REGS_BASEADDR + 0x08ul)

// Khai báo cấu trúc thanh ghi

/*
 * Ghi chú:
 * STM32 sử dụng kiểu little-endian, 
 * nghĩa là bit thấp nhất được đánh số 0. 
 */

typedef __vo struct SCB_AIRCR_REG
{
  __vo unsigned int VECTRESET : 1;
  __vo unsigned int VECTCLRACTIVE : 1;
  __vo unsigned int SYSRESETREQ : 1;
  __vo unsigned int RESERVED0 : 5;
  __vo unsigned int PRIGROUP : 3;
  __vo unsigned int RESERVED1 : 4;
  __vo unsigned int ENDIANNESS : 1;
  __vo unsigned int VECTKEY : 16;
} SCB_AIRCR_REG;

typedef __vo struct RCC_CR_REG
{
  __vo unsigned int HSION : 1;
  __vo unsigned int HSIRDY : 1;
  __vo unsigned int RESERVED0 : 1;
  __vo unsigned int HSITRIM : 5;
  __vo unsigned int HSICAL : 8;
  __vo unsigned int HSEON : 1;
  __vo unsigned int HSERDY : 1;
  __vo unsigned int HSEBYP : 1;
  __vo unsigned int CSSON : 1;
  __vo unsigned int RESERVED1 : 4;
  __vo unsigned int PLLON : 1;
  __vo unsigned int PLLRDY : 1;
  __vo unsigned int RESERVED2 : 6;
} RCC_CR_REG;

typedef __vo struct RCC_CFGR_REG
{
  __vo unsigned int SW : 2;
  __vo unsigned int SWS : 2;
  __vo unsigned int HPRE : 4;
  __vo unsigned int PPRE1 : 3;
  __vo unsigned int PPRE2 : 3;
  __vo unsigned int ADCPRE : 2;
  __vo unsigned int PLLSRC : 1;
  __vo unsigned int PLLXTPRE : 1;
  __vo unsigned int PLLMUL : 4;
  __vo unsigned int USBPRE : 1;
  __vo unsigned int RESERVED0 : 1;
  __vo unsigned int MCO : 3;
  __vo unsigned int RESERVED1 : 5;
} RCC_CFGR_REG;

typedef __vo struct RCC_CIR_REG
{
  __vo unsigned int LSIRDYF : 1;
  __vo unsigned int LSERDYF : 1;
  __vo unsigned int HSIRDYF : 1;
  __vo unsigned int HSERDYF : 1;
  __vo unsigned int PLLRDYF : 1;
  __vo unsigned int RESERVED0 : 2;
  __vo unsigned int CSSF : 1;
  __vo unsigned int LSIRDYIE : 1;
  __vo unsigned int LSERDYIE : 1;
  __vo unsigned int HSIRDYIE : 1;
  __vo unsigned int HSERDYIE : 1;
  __vo unsigned int PLLRDYIE : 1;
  __vo unsigned int RESERVED1 : 3;
  __vo unsigned int LSIRDYC : 1;
  __vo unsigned int LSERDYC : 1;
  __vo unsigned int HSIRDYC : 1;
  __vo unsigned int HSERDYC : 1;
  __vo unsigned int PLLRDYC : 1;
  __vo unsigned int RESERVED2 : 2;
  __vo unsigned int CSSC : 1;
  __vo unsigned int RESERVED3 : 8;
} RCC_CIR_REG;

typedef __vo struct RCC_REGS
{
  __vo RCC_CR_REG CR;
  __vo RCC_CFGR_REG CFGR;
  __vo RCC_CIR_REG CIR;
  // Các thanh ghi khác có thể được khai báo ở đây
} RCC_REGS;

// Tạo con trỏ tới thanh ghi

#define SCB_AIRCR_REG_PTR ((SCB_AIRCR_REG *)SCB_AIRCR_REG_ADDR)
#define RCC_REGS_PTR ((RCC_REGS *)RCC_REGS_BASEADDR)

// Khai báo các hàm tại bên dưới

RETR_STAT RCC_CLK_Init(void);
RETR_STAT RCC_CLK_DeInit(void);

/**
 * ghi chú:
 * bổ sung các khai báo sử dụng riêng cho LSI, HSI, HSE nhưng thống nhất param trong hàm init/deinit chung
 */

#endif /* LIB_RCC_H_ */
