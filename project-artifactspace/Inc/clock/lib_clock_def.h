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
		* STM32 sử dụng kiểu little-endian,
		* nghĩa là bit thấp nhất được đánh số 0.
		*/

		tdf_strc RCC_CR_REG {
			__vo ui HSION : 1;
			__vo ui HSIRDY : 1;
			__vo ui RESERVED0 : 1;
			__vo ui HSITRIM : 5;
			__vo ui HSICAL : 8;
			__vo ui HSEON : 1;
			__vo ui HSERDY : 1;
			__vo ui HSEBYP : 1;
			__vo ui CSSON : 1;
			__vo ui RESERVED1 : 4;
			__vo ui PLLON : 1;
			__vo ui PLLRDY : 1;
			__vo ui RESERVED2 : 6;
		} RCC_CR_REG_Typedef;

		tdf_strc RCC_CFGR_REG {
			__vo ui SW : 2;
			__vo ui SWS : 2;
			__vo ui HPRE : 4;
			__vo ui PPRE1 : 3;
			__vo ui PPRE2 : 3;
			__vo ui ADCPRE : 2;
			__vo ui PLLSRC : 1;
			__vo ui PLLXTPRE : 1;
			__vo ui PLLMUL : 4;
			__vo ui USBPRE : 1;
			__vo ui RESERVED0 : 1;
			__vo ui MCO : 3;
			__vo ui RESERVED1 : 5;
		} RCC_CFGR_REG_Typedef;

		tdf_strc RCC_CIR_REG {
			__vo ui LSIRDYF : 1;
			__vo ui LSERDYF : 1;
			__vo ui HSIRDYF : 1;
			__vo ui HSERDYF : 1;
			__vo ui PLLRDYF : 1;
			__vo ui RESERVED0 : 2;
			__vo ui CSSF : 1;
			__vo ui LSIRDYIE : 1;
			__vo ui LSERDYIE : 1;
			__vo ui HSIRDYIE : 1;
			__vo ui HSERDYIE : 1;
			__vo ui PLLRDYIE : 1;
			__vo ui RESERVED1 : 3;
			__vo ui LSIRDYC : 1;
			__vo ui LSERDYC : 1;
			__vo ui HSIRDYC : 1;
			__vo ui HSERDYC : 1;
			__vo ui PLLRDYC : 1;
			__vo ui RESERVED2 : 2;
			__vo ui CSSC : 1;
			__vo ui RESERVED3 : 8;
		} RCC_CIR_REG_Typedef;

		tdf_strc RCC_CSR {
			__vo ui LSION : 1;
			__vo ui LSIRDY : 1;
			__vo ui RESERVED0 : 22;
			__vo ui RMVF : 1;
			__vo ui RESERVED1 : 1;
			__vo ui PINRSTF : 1;
			__vo ui PORRSTF : 1;
			__vo ui SFTRSTF : 1;
			__vo ui IWDGRSTF : 1;
			__vo ui WWDGRSTF : 1;
			__vo ui LPWRRSTF : 1;
		} RCC_CSR_REG_Typedef;

		tdf_strc RCC_REGS {
			__vo RCC_CR_REG_Typedef CR;
			__vo RCC_CFGR_REG_Typedef CFGR;
			__vo RCC_CIR_REG_Typedef CIR;
			__vo BLANK_REG RESERVED[6];
			__vo RCC_CSR_REG_Typedef CSR;
		} RCC_REGS_Typedef;

  // >> Tạo con trỏ tới ngoại vi

  	#ifndef UNIT_TEST
			#define RCC_REGS_PTR ((RCC_REGS_Typedef *) RCC_REGS_BASEADDR)
		#else
			extern RCC_REGS_Typedef MOCK_RCC_REGS;
			#define RCC_REGS_PTR (&MOCK_RCC_REGS)
		#endif

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

  // Khai báo các nguồn clock khởi tạo

		# define RCC_SYSCLK_SOURCE_HSI RCC_CFGR_REG_SW_SET_HSI
		# define RCC_SYSCLK_SOURCE_HSE RCC_CFGR_REG_SW_SET_HSE
		# define RCC_IWDG_SOURCE_LSI 0xFul

		/**
		 * Ghi chú:
		 * RCC_IWDG_SOURCE_LSI là giá trị giả định để truyền vào hàm khởi tạo
		 * LSI không được sử dụng làm SYSCLK select nên không có định nghĩa tương ứng
		 */

#endif /* LIB_CLOCK_DEF_H_ */
