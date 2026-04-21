/*
 * lib_clock_def.h
 *
 *  Created on: Jan 6, 2026
 *      Author: shanghuang
 */

#ifndef LIB_CLOCK_DEF_H_
  #define LIB_CLOCK_DEF_H_

  /**
   * Ghi chú:
   * Nhớ bổ sung thêm việc phân tách các khởi tạo
   * nhằm tuân theo tiêu chuẩn 3 lớp HAL
   * Điều này giúp việc bảo trì và mở rộng dễ dàng hơn trong tương lai
   */

  // Khai báo các thư viện sử dụng chung

		#include <stdint.h>

		#ifndef UNIT_TEST
			#include "generic/lib_keyword_def.h"
		#else
			#include "lib_keyword_def.h"
		#endif

  // Khai báo địa chỉ ngoại vi

  	#define RCC_REGS_BASEADDR 0x40021000ul

  // Khai báo cấu trúc thanh ghi

		/*
		* Ghi chú:
		* - STM32 sử dụng kiểu little-endian,
		* nghĩa là bit thấp nhất được đánh số 0.
		* - Tất cả thanh ghi đều sử dụng truy cập mức word.
		*/

		tdf_strc RCC_REGS {
			__vo BLANK_REG_32B CR;
			__vo BLANK_REG_32B CFGR;
			__vo BLANK_REG_32B CIR;
			__vo BLANK_REG_32B RCC_APB2RSTR;
			__vo BLANK_REG_32B RESERVED0[2];
			__vo BLANK_REG_32B RCC_APB2ENR;
			__vo BLANK_REG_32B RESERVED1[2];
			__vo BLANK_REG_32B CSR;
		} RCC_REGS_Typedef;

  // >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

  	#ifndef UNIT_TEST
			#define RCC_REGS_PTR ((RCC_REGS_Typedef *) RCC_REGS_BASEADDR)
		#else
			extern RCC_REGS_Typedef MOCK_RCC_REGS;
			#define RCC_REGS_PTR (&MOCK_RCC_REGS)
		#endif

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CR

		#define RCC_CR_MASK 0xFFFFFFFFul
		#define RCC_CR_HSION_SET (1u << 0)
		#define RCC_CR_HSIRDY_ON (1u << 1)
		#define RCC_CR_HSEON_SET (1u << 16)
		#define RCC_CR_HSERDY_ON (1u << 17)
		#define RCC_CR_CSSON_SET (1u << 19)
		#define RCC_CR_PLLON_SET (1u << 24)
		#define RCC_CR_PLLRDY_ON (1u << 25)

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CFGR_REG

		#define RCC_CFGR_SW_SET_HSI (((ui32)0x00ul) << 0)
		#define RCC_CFGR_SW_SET_HSE (((ui32)0x01ul) << 0)
		#define RCC_CFGR_SW_SET_PLL (((ui32)0x02ul) << 0)

		#define RCC_CFGR_SWS_HSI    (((ui32)0x00ul) << 2)
		#define RCC_CFGR_SWS_HSE    (((ui32)0x01ul) << 2)
		#define RCC_CFGR_SWS_PLL    (((ui32)0x02ul) << 2)

		#define RCC_CFGR_HPRE_DIV1  (((ui32)0x00ul) << 4)
		#define RCC_CFGR_HPRE_DIV2  (((ui32)0x08ul) << 4)

		#define RCC_CFGR_PPRE1_DIV1  (((ui32)0x00ul) << 8)
		#define RCC_CFGR_PPRE1_DIV2  (((ui32)0x04ul) << 8)
		#define RCC_CFGR_PPRE1_DIV4  (((ui32)0x05ul) << 8)

		#define RCC_CFGR_PLL_SRC_HSIDIV2 (((ui32)0x00ul) << 16)
		#define RCC_CFGR_PLL_SRC_HSE (((ui32)0x01ul) << 16)
		#define RCC_CFGR_PLLSRC_MASK (((ui32)0x0Ful) << 16)

		#define RCC_CFGR_PLLXTPRE_DIV1  (((ui32)0x00ul) << 17)
		#define RCC_CFGR_PLLXTPRE_DIV2  (((ui32)0x01ul) << 17)
		#define RCC_CFGR_PLLXTPRE_MASK 	(((ui32)0x0Ful) << 17)

		#define RCC_CFGR_PLLMUL_X9 (((ui32)0x09ul) << 18)
		#define RCC_CFGR_PLLMUL_X8 (((ui32)0x08ul) << 18)
		#define RCC_CFGR_PLLMUL_X4 (((ui32)0x04ul) << 18)
		#define RCC_CFGR_PLLMUL_X2 (((ui32)0x02ul) << 18)
		#define RCC_CFGR_PLLMUL_X1 (((ui32)0x01ul) << 18)
		#define RCC_CFGR_PLLMUL_MASK (((ui32)0x0Ful) << 18)
		

		#define RCC_CFGR_SW_MASK 0xFFFFFFFFul


  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CIR

		#define RCC_CIR_MASK 0xFFFFFFFFul
		#define RCC_CIR_CSSF_ON (1u << 7)
		#define RCC_CIR_CSSC_SET (1u << 8)


  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CSR

		#define RCC_CSR_MASK 0xFFFFFFFFul
		#define RCC_CSR_LSION_SET (1u << 0)
		#define RCC_CSR_LSIRDY_ON (1u << 1)

	// Khai báo các định nghĩa bit cần sử dụng trên RCC_APB2RSTR

		#define RCC_APB2RSTR_MASK 0xFFFFFFFFul

	// Khai báo các định nghĩa bit cần sử dụng trên RCC_APB2ENR

		#define RCC_APB2ENR_MASK 0xFFFFFFFFul

  // Khai báo các nguồn clock khởi tạo

		#define RCC_SYSCLK_SOURCE_HSI RCC_CFGR_SW_SET_HSI
		#define RCC_SYSCLK_SOURCE_HSE RCC_CFGR_SW_SET_HSE
		#define RCC_SYSCLK_SOURCE_PLL RCC_CFGR_SW_SET_PLL

	// Khai báo các ngoại vi cần mở nguồn

		#define AFIO  (0x01ul << 0)
		#define GPIOA (0x01ul << 2)
		#define GPIOB (0x01ul << 3)
		#define GPIOC (0x01ul << 4)
		#define GPIOD (0x01ul << 5)
		#define GPIOE (0x01ul << 6)
		#define GPIOF (0x01ul << 7)
		#define GPIOG (0x01ul << 8)
		#define SPI1	(0x01ul << 12)
		
#endif /* LIB_CLOCK_DEF_H_ */
