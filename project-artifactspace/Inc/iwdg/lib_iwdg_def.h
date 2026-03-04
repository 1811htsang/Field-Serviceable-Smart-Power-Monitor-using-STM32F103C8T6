/*
 * lib_iwdg_def.h
 *
 *  Created on: Jan 8, 2026
 *      Author: shanghuang
 */

#ifndef LIB_IWDG_DEF_H_
	#define LIB_IWDG_DEF_H_

	// Khai báo các thư viện sử dụng chung

		#ifndef UNIT_TEST
			#include "generic/lib_keyword_def.h"
		#else
			#include "lib_keyword_def.h"
		#endif

	// Khai báo địa chỉ ngoại vi

		#define IWDG_REGS_BASEADDR 0x40003000ul

	// Khai báo địa chỉ thanh ghi cụ thể

		#define IWDG_KR_REG_ADDR   (IWDG_REGS_BASEADDR + 0x00ul)
		#define IWDG_PR_REG_ADDR   (IWDG_REGS_BASEADDR + 0x04ul)
		#define IWDG_RLR_REG_ADDR  (IWDG_REGS_BASEADDR + 0x08ul)
		#define IWDG_SR_REG_ADDR   (IWDG_REGS_BASEADDR + 0x0Cul)

	// Khai báo cấu trúc thanh ghi

		tdf_strc IWDG_REGS {
			__vo BLANK_REG KR;
			__vo BLANK_REG PR;
			__vo BLANK_REG RLR;
			__vo BLANK_REG SR;
		} IWDG_REGS_Typedef;

	// >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

		#ifndef UNIT_TEST
			#define IWDG_REGS_PTR ((IWDG_REGS_Typedef *) IWDG_REGS_BASEADDR)
		#else
			extern IWDG_REGS_Typedef MOCK_IWDG_REGS;
			#define IWDG_REGS_PTR (&MOCK_IWDG_REGS)
		#endif

	// Khai báo các định nghĩa bit cần sử dụng trên IWDG_KR_REG

		#define IWDG_KR_REG_KEY_ENABLE_ACCESS  ((ui16)0x5555ul << 0)
		#define IWDG_KR_REG_KEY_DISABLE_ACCESS ((ui16)0x0000ul << 0)
		#define IWDG_KR_REG_KEY_START 				 ((ui16)0xCCCCul << 0)
		#define IWDG_KR_REG_KEY_RELOAD_COUNTER ((ui16)0xAAAAul << 0)
		#define IWDG_KR_REG_KEY_MASK 				   ((ui32)0xFFFFFFFFul << 0)

	// Khai báo các định nghĩa bit cần sử dụng trên IWDG_PR_REG

		#define IWDG_PR_REG_PR_DIV_4   ((ui8)0x00ul << 0)
		#define IWDG_PR_REG_PR_DIV_8   ((ui8)0x01ul << 0)
		#define IWDG_PR_REG_PR_DIV_16  ((ui8)0x02ul << 0)
		#define IWDG_PR_REG_PR_DIV_32  ((ui8)0x03ul << 0)
		#define IWDG_PR_REG_PR_DIV_64  ((ui8)0x04ul << 0)
		#define IWDG_PR_REG_PR_DIV_128 ((ui8)0x05ul << 0)
		#define IWDG_PR_REG_PR_DIV_256 ((ui8)0x06ul << 0)
		#define IWDG_PR_REG_PR_MASK    ((ui32)0xFFFFFFFFul << 0)

	// Khai báo các định nghĩa bit cần sử dụng trên IWDG_RLR_REG

		#define IWDG_RLR_REG_RL_MIN ((ui16)0x001ul << 0)
		#define IWDG_RLR_REG_RL_AVG ((ui16)0x7FFul << 0)
		#define IWDG_RLR_REG_RL_MAX ((ui16)0xFFFul << 0)
		#define IWDG_RLR_REG_RL_MASK ((ui32)0xFFFFFFFFul << 0)

	// Khai báo các định nghĩa bit cần sử dụng trên IWDG_SR_REG

		#define IWDG_SR_REG_PVU_UPDATE_ONGOING (SET << 0)
		#define IWDG_SR_REG_PVU_UPDATE_COMPLETED ~(SET << 0)

		#define IWDG_SR_REG_RVU_UPDATE_ONGOING (SET << 1)
		#define IWDG_SR_REG_RVU_UPDATE_COMPLETED ~(SET << 1)

#endif /* LIB_IWDG_DEF_H_ */
