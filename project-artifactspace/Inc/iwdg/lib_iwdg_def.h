/*
 * lib_iwdg_def.h
 *
 *  Created on: Jan 8, 2026
 *      Author: shanghuang
 */

#ifndef LIB_IWDG_DEF_H_
	#define LIB_IWDG_DEF_H_

	// Khai báo các thư viện sử dụng chung

		#include "lib_keyword_def.h"

	// Khai báo địa chỉ ngoại vi

		#define IWDG_REGS_BASEADDR 0x40003000ul

	// Khai báo địa chỉ thanh ghi cụ thể

		#define IWDG_KR_REG_ADDR   (IWDG_REGS_BASEADDR + 0x00ul)
		#define IWDG_PR_REG_ADDR   (IWDG_REGS_BASEADDR + 0x04ul)
		#define IWDG_RLR_REG_ADDR  (IWDG_REGS_BASEADDR + 0x08ul)
		#define IWDG_SR_REG_ADDR   (IWDG_REGS_BASEADDR + 0x0Cul)

	// Khai báo cấu trúc thanh ghi

		tdf_strc IWDG_KR_REG {
			__vo ui KEY : 16;
			__vo ui RESERVED : 16;
		} IWDG_KR_REG_Typedef;

		tdf_strc IWDG_PR_REG {
			__vo ui PR : 3;
			__vo ui RESERVED : 29;
		} IWDG_PR_REG_Typedef;

		tdf_strc IWDG_RLR_REG {
			__vo ui RL : 12;
			__vo ui RESERVED : 20;
		} IWDG_RLR_REG_Typedef;

		tdf_strc IWDG_SR_REG {
			__vo ui PVU : 1;
			__vo ui RVU : 1;
			__vo ui RESERVED : 30;
		} IWDG_SR_REG_Typedef;

		tdf_strc IWDG_REGS {
			__vo IWDG_KR_REG_Typedef KR;
			__vo IWDG_PR_REG_Typedef PR;
			__vo IWDG_RLR_REG_Typedef RLR;
			__vo IWDG_SR_REG_Typedef SR;
		} IWDG_REGS_Typedef;

	// >> Tạo con trỏ tới ngoại vi

		#define IWDG_REGS_PTR ((IWDG_REGS_Typedef *) IWDG_REGS_BASEADDR)

	// Khai báo các định nghĩa bit cần sử dụng trên IWDG_KR_REG

		#define IWDG_KR_REG_KEY_ENABLE_ACCESS 0x5555ul
		#define IWDG_KR_REG_KEY_DISABLE_ACCESS 0x0000ul
		#define IWDG_KR_REG_KEY_START 0xCCCCul
		#define IWDG_KR_REG_KEY_RELOAD_COUNTER 0xAAAAul

	// Khai báo các định nghĩa bit cần sử dụng trên IWDG_PR_REG

		#define IWDG_PR_REG_PR_DIV_4 0x00ul
		#define IWDG_PR_REG_PR_DIV_8 0x01ul
		#define IWDG_PR_REG_PR_DIV_16 0x02ul
		#define IWDG_PR_REG_PR_DIV_32 0x03ul
		#define IWDG_PR_REG_PR_DIV_64 0x04ul
		#define IWDG_PR_REG_PR_DIV_128 0x05ul
		#define IWDG_PR_REG_PR_DIV_256 0x06ul

	// Khai báo các định nghĩa bit cần sử dụng trên IWDG_RLR_REG

		#define IWDG_RLR_REG_RL_MIN 0x001ul
		#define IWDG_RLR_REG_RL_AVG 0x7FFul
		#define IWDG_RLR_REG_RL_MAX 0xFFFul
		
	// Khai báo các định nghĩa bit cần sử dụng trên IWDG_SR_REG

		#define IWDG_SR_REG_PVU_UPDATE_ONGOING SET
		#define IWDG_SR_REG_PVU_UPDATE_COMPLETED RESET

		#define IWDG_SR_REG_RVU_UPDATE_ONGOING SET
		#define IWDG_SR_REG_RVU_UPDATE_COMPLETED RESET

#endif /* LIB_IWDG_DEF_H_ */
