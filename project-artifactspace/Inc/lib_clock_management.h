/*
 * lib_clock_management.h
 *
 *  Created on: Jan 6, 2026
 *      Author: shanghuang
 */

#ifndef LIB_CLOCK_MANAGEMENT_H_
  #define LIB_CLOCK_MANAGEMENT_H_

  // Khai báo các thư viện sử dụng chung

  #include <stdint.h>
  #include "lib_keyword_define.h"

  // Khai báo địa chỉ ngoại vi

  #define RCC_REGS_BASEADDR 0x40021000ul

  // Khai báo địa chỉ thanh ghi cụ thể

  #define RCC_CR_REG_ADDR (RCC_REGS_BASEADDR + 0x00ul)
  #define RCC_CFGR_REG_ADDR (RCC_REGS_BASEADDR + 0x04ul)
  #define RCC_CIR_REG_ADDR (RCC_REGS_BASEADDR + 0x08ul)
  #define RCC_CSR_REG_ADDR (RCC_REGS_BASEADDR + 0x24ul)

  // Khai báo cấu trúc thanh ghi

  /*
  * Ghi chú:
  * STM32 sử dụng kiểu little-endian, 
  * nghĩa là bit thấp nhất được đánh số 0. 
  */

  typedef struct RCC_CR_REG
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

  typedef struct RCC_CFGR_REG
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

  typedef struct RCC_CIR_REG
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

  typedef struct RCC_CSR
  {
    __vo unsigned int LSION : 1;
    __vo unsigned int LSIRDY : 1;
    __vo unsigned int RESERVED0 : 22;
    __vo unsigned int RMVF : 1;
    __vo unsigned int RESERVED1 : 1;
    __vo unsigned int PINRSTF : 1;
    __vo unsigned int PORRSTF : 1;
    __vo unsigned int SFTRSTF : 1;
    __vo unsigned int IWDGRSTF : 1;
    __vo unsigned int WWDGRSTF : 1;
    __vo unsigned int LPWRRSTF : 1;
  } RCC_CSR;


  typedef struct RCC_REGS
  {
    __vo RCC_CR_REG CR;
    __vo RCC_CFGR_REG CFGR;
    __vo RCC_CIR_REG CIR;
    __vo BLANK_REG RESERVED[6];
    __vo RCC_CSR CSR;
  } RCC_REGS;

  // >> Tạo con trỏ tới thanh ghi

  #define RCC_REGS_PTR ((RCC_REGS *)RCC_REGS_BASEADDR)

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CR_REG

  #define RCC_CR_REG_HSION_RESET RESET
  #define RCC_CR_REG_HSION_SET SET
  #define RCC_CR_REG_HSEON_RESET RESET
  #define RCC_CR_REG_HSEON_SET SET
  #define RCC_CR_REG_CSSON_RESET RESET
  #define RCC_CR_REG_CSSON_SET SET
  #define RCC_CR_REG_PLLON_RESET RESET
  #define RCC_CR_REG_PLLON_SET SET

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CFGR_REG

  #define RCC_CFGR_REG_SW_SET_HSI 0x00ul
  #define RCC_CFGR_REG_SW_SET_HSE 0x01ul
  #define RCC_CFGR_REG_SW_SET_PLL 0x02ul

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CIR_REG

  #define RCC_CIR_REG_LSIRDYF_NOT_READY RESET
  #define RCC_CIR_REG_LSIRDYF_READY SET

  #define RCC_CIR_REG_HSIRDYF_NOT_READY RESET
  #define RCC_CIR_REG_HSIRDYF_READY SET

  #define RCC_CIR_REG_HSERDYF_NOT_READY RESET
  #define RCC_CIR_REG_HSERDYF_READY SET

  #define RCC_CIR_REG_PLLRDYF_NOT_READY RESET
  #define RCC_CIR_REG_PLLRDYF_READY SET

  #define RCC_CIR_REG_CSSF_NO_CLOCK_FAILURE RESET
  #define RCC_CIR_REG_CSSF_CLOCK_FAILURE SET

  #define RCC_CIR_REG_LSIRDYIE_DISABLE RESET
  #define RCC_CIR_REG_LSIRDYIE_ENABLE SET

  #define RCC_CIR_REG_HSIRDYIE_DISABLE RESET
  #define RCC_CIR_REG_HSIRDYIE_ENABLE SET

  #define RCC_CIR_REG_HSERDYIE_DISABLE RESET
  #define RCC_CIR_REG_HSERDYIE_ENABLE SET

  #define RCC_CIR_REG_PLLRDYIE_DISABLE RESET
  #define RCC_CIR_REG_PLLRDYIE_ENABLE SET

  #define RCC_CIR_REG_LSIRDYC_NO_EFFECT RESET
  #define RCC_CIR_REG_LSIRDYC_CLEAR SET

  #define RCC_CIR_REG_HSIRDYC_NO_EFFECT RESET
  #define RCC_CIR_REG_HSIRDYC_CLEAR SET

  #define RCC_CIR_REG_HSERDYC_NO_EFFECT RESET
  #define RCC_CIR_REG_HSERDYC_CLEAR SET

  #define RCC_CIR_REG_PLLRDYC_NO_EFFECT RESET
  #define RCC_CIR_REG_PLLRDYC_CLEAR SET

  #define RCC_CIR_REG_CSSC_NO_EFFECT RESET
  #define RCC_CIR_REG_CSSC_CLEAR SET

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CSR

  #define RCC_CSR_REG_LSION_RESET RESET
  #define RCC_CSR_REG_LSION_SET SET

  #define RCC_CSR_REG_LSIRDY_NOT_READY RESET
  #define RCC_CSR_REG_LSIRDY_READY SET

  #define RCC_CSR_REG_RMVF_NO_EFFECT RESET
  #define RCC_CSR_REG_RMVF_CLEAR SET

  #define RCC_CSR_REG_PINRSTF_NO_RESET_OCCURRED RESET
  #define RCC_CSR_REG_PINRSTF_RESET_OCCURRED SET

  #define RCC_CSR_REG_PORRSTF_NO_RESET_OCCURRED RESET
  #define RCC_CSR_REG_PORRSTF_RESET_OCCURRED SET

  #define RCC_CSR_REG_SFTRSTF_NO_RESET_OCCURRED RESET
  #define RCC_CSR_REG_SFTRSTF_RESET_OCCURRED SET

  #define RCC_CSR_REG_IWDGRSTF_NO_RESET_OCCURRED RESET
  #define RCC_CSR_REG_IWDGRSTF_RESET_OCCURRED SET

  #define RCC_CSR_REG_WWDGRSTF_NO_RESET_OCCURRED RESET
  #define RCC_CSR_REG_WWDGRSTF_RESET_OCCURRED SET

  #define RCC_CSR_REG_LPWRRSTF_NO_RESET_OCCURRED RESET
  #define RCC_CSR_REG_LPWRRSTF_RESET_OCCURRED SET

  // Khai báo các nguồn clock khởi tạo

  # define RCC_SYSCLK_SOURCE_HSI RCC_CFGR_REG_SW_SET_HSI
  # define RCC_SYSCLK_SOURCE_HSE RCC_CFGR_REG_SW_SET_HSE
  # define RCC_IWDG_SOURCE_LSI 0xFul

  /**
   * Ghi chú:
   * RCC_IWDG_SOURCE_LSI là giá trị giả định để truyền vào hàm khởi tạo
   * LSI không được sử dụng làm SYSCLK select nên không có định nghĩa tương ứng
   */

  // Khai báo cấu trúc tham số hàm khởi tạo
  typedef struct
  {
    ul SYSCLK_Source; // Chọn nguồn clock hệ thống
  } RCC_CLK_Init_Param;

  // Khai báo các kiểm tra tham số đầu vào nội bộ
  #define IS_RCC_SYSCLK_SOURCE(SOURCE) (((SOURCE) == RCC_SYSCLK_SOURCE_HSI) || \
                                        ((SOURCE) == RCC_SYSCLK_SOURCE_HSE) || \
                                        ((SOURCE) == RCC_SYSCLK_SOURCE_PLL))
  #define IS_RCC_IWDG_SOURCE(SOURCE) (((SOURCE) == RCC_IWDG_SOURCE_LSI))

  // Khai báo các hàm thành phần 

  RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param init_param);
  RETR_STAT RCC_CLK_DeInit(RCC_CLK_Init_Param init_param);
  void RCC_CSS_Enable(void);
  void RCC_CSS_Disable(void);
  void RCC_NMI_IRQ_Handler(void);
  void RCC_CSS_Callback(void);
#endif /* LIB_CLOCK_MANAGEMENT_H_ */
