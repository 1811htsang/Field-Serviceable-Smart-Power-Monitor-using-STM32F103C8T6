/**
 ******************************************************************************
 * @file           : lib_keyword_def.h
 * @author         : shanghuang
 * @brief          : Header for define keyword
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef LIB_KEYWORD_DEF_H_
  #define LIB_KEYWORD_DEF_H_

	// Khai báo thư viện phụ thuộc

		#include <stdint.h>

  // Khai báo các định nghĩa dùng chung

		#define __vo volatile

		#define HIGH 0x1u
		#define LOW 0x0u

		#define SET HIGH
		#define RESET LOW

		#define ENABLE HIGH
		#define DISABLE LOW

		#define SUCCESS HIGH
		#define ERROR LOW

		#define ON HIGH
		#define OFF LOW

		#define TRUE HIGH
		#define FALSE LOW

		#define u unsigned
		#define ul unsigned long
		#define ui unsigned int
		#define ui8 uint8_t
		#define ui16 uint16_t
		#define ui32 uint32_t
		
		#define tdf_strc typedef struct
		#define tdf_enum typedef enum

		#define BLANK_REG uint32_t

		#define __weak __attribute__((weak))
		
		#define DEBUG_MODE ENABLE

		#define sta static
		#define ext extern
		#define inl inline
		#define stinl static inline

		#ifdef  USE_FULL_ASSERT
			#ifndef UNIT_TEST
        void assert_failed(ui8* file, ui32 line) {
          printf("Assertion failed in file %s on line %lu.\n", file, line);
          while(1) { }
        }
      #endif
			#define assert_param(expr) ((expr) ? (void)1u : assert_failed((ui8*)__FILE__, __LINE__))
			#ifdef UNIT_TEST
				void assert_failed(ui8* file, ui8 line);
			#endif
		#else
			#define assert_param(expr) ((void)0u)
		#endif /* USE_FULL_ASSERT */

  // Khai báo kiểu trả về chung

		typedef enum
		{
			STAT_OK       = 0x00U,
			STAT_ERROR    = 0x01U,
			STAT_BUSY     = 0x02U,
			STAT_TIMEOUT  = 0x03U,
			STAT_DONE     = 0x04U,
			STAT_NRDY     = STAT_ERROR,
			STAT_RDY      = STAT_OK
		} RETR_STAT;

	// Khai báo các thao tác bit
		
		#define SET_BIT(REG, BIT)     ((REG) |= (BIT)) // Bit sẽ có dạng (1u << n) để đảm bảo chỉ thao tác trên bit đó
		#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT)) // Bit sẽ có dạng (1u << n), khi đảo lại sẽ có tất cả bit là 1 ngoại trừ bit cần xóa sẽ là 0
		#define READ_BIT(REG, BIT)    ((REG) & (BIT)) // Bit sẽ có dạng (1u << n), phép AND sẽ trả về giá trị khác 0 nếu bit đó được set, ngược lại sẽ trả về 0

	// Khai báo các thao tác thanh ghi
	
		#define CLEAR_REG(REG)        ((REG) = (0x0))
		#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
		#define READ_REG(REG)         ((REG))
		#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#endif /* LIB_KEYWORD_DEF_H_ */

