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

		#ifndef UNIT_TEST
			#include "generic/lib_keyword_def.h"
		#else
			#include "lib_keyword_def.h"
		#endif

  

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
		* - STM32 sử dụng kiểu little-endian,
		* nghĩa là bit thấp nhất được đánh số 0.
		* - Tất cả thanh ghi đều sử dụng truy cập mức word.
		*/

		tdf_strc RCC_REGS {
			__vo BLANK_REG CR;
			__vo BLANK_REG CFGR;
			__vo BLANK_REG CIR;
			__vo BLANK_REG RCC_APB2RSTR;
			__vo BLANK_REG RESERVED0[2];
			__vo BLANK_REG RCC_APB2ENR;
			__vo BLANK_REG RESERVED1[2];
			__vo BLANK_REG CSR;
		} RCC_REGS_Typedef;

  // >> Tạo con trỏ tới ngoại vi

  	#define RCC_REGS_PTR ((RCC_REGS_Typedef *)RCC_REGS_BASEADDR)

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CR_REG

		#define RCC_CR_REG_MASK 0xFFFFFFFFul
		#define RCC_CR_REG_HSION_SET (1u << 0)
		#define RCC_CR_REG_HSIRDY_ON (1u << 1)
		#define RCC_CR_REG_HSEON_SET (1u << 16)
		#define RCC_CR_REG_HSERDY_ON (1u << 17)
		#define RCC_CR_REG_CSSON_SET (1u << 19)

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CFGR_REG

		#define RCC_CFGR_REG_SW_SET_HSI (((ui32)0x00ul) << 0)
		#define RCC_CFGR_REG_SW_SET_HSE (((ui32)0x01ul) << 0)
		#define RCC_CFGR_REG_SW_SET_PLL (((ui32)0x02ul) << 0)

		#define RCC_CFGR_REG_SWS_HSI    (((ui32)0x00ul) << 2)
		#define RCC_CFGR_REG_SWS_HSE    (((ui32)0x01ul) << 2)
		#define RCC_CFGR_REG_SWS_PLL    (((ui32)0x02ul) << 2)
		#define RCC_CFGR_REG_SW_MASK 0xFFFFFFFFul


  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CIR_REG

		#define RCC_CIR_REG_MASK 0xFFFFFFFFul
		#define RCC_CIR_REG_CSSF_ON (1u << 7)
		#define RCC_CIR_REG_CSSC_SET (1u << 8)


  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CSR

		#define RCC_CSR_REG_MASK 0xFFFFFFFFul
		#define RCC_CSR_REG_LSION_SET (1u << 0)
		#define RCC_CSR_REG_LSIRDY_ON (1u << 1)

	// Khai báo các định nghĩa bit cần sử dụng trên RCC_APB2RSTR

		#define RCC_APB2RSTR_MASK 0xFFFFFFFFul

	// Khai báo các định nghĩa bit cần sử dụng trên RCC_APB2ENR

		#define RCC_APB2ENR_MASK 0xFFFFFFFFul

  // Khai báo các nguồn clock khởi tạo

		# define RCC_SYSCLK_SOURCE_HSI RCC_CFGR_REG_SW_SET_HSI
		# define RCC_SYSCLK_SOURCE_HSE RCC_CFGR_REG_SW_SET_HSE
		# define RCC_IWDG_SOURCE_LSI 0xFul

		/**
		 * Ghi chú:
		 * RCC_IWDG_SOURCE_LSI là giá trị giả định để truyền vào hàm khởi tạo
		 * LSI không được sử dụng làm SYSCLK select nên không có định nghĩa tương ứng
		 */

	// Khai báo các ngoại vi cần mở nguồn

		#define AFIO  (0x01ul << 0)
		#define GPIOA (0x01ul << 2)
		#define GPIOB (0x01ul << 3)
		#define GPIOC (0x01ul << 4)
		#define GPIOD (0x01ul << 5)
		#define GPIOE (0x01ul << 6)
		#define GPIOF (0x01ul << 7)
		#define GPIOG (0x01ul << 8)
		
#endif /* LIB_CLOCK_DEF_H_ */
