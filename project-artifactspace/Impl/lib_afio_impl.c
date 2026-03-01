/*
 * lib_afio_impl.c
 *
 *  Created on: Feb 28, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
		#include "lib_keyword_def.h"
		#include "lib_condition_def.h"
		#include "lib_gpio_def.h"
		#include "lib_gpio_hal.h"
    #include "lib_afio_def.h"
    #include "lib_afio_hal.h"
    #include "header_dependency.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
	#include <string.h>

  #ifndef UNIT_TEST
		#include "generic/lib_keyword_def.h"
  	#include "generic/lib_condition_def.h"
  	#include "gpio/lib_gpio_def.h"
  	#include "gpio/lib_gpio_hal.h"
    #include "afio/lib_afio_def.h"
    #include "afio/lib_afio_hal.h"
  #endif

// Định nghĩa các hàm thành phần

  RETR_STAT AFIO_PinRemap(ui32 Peri) {

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("AFIO_PinRemap, DBG1: Check Peripheral parameter.\n");
    }

      assert_param(IS_AFIO_PERI_REMAP(Peri));

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("AFIO_PinRemap, DBG2: Remapping peripheral with mask 0x%08lX.\n", Peri);
    }

      AFIO_REGS_PTR->AFIO_MAPR |= Peri;

    return STAT_DONE;
  }

  RETR_STAT AFIO_EXTI_Line_Init(AFIO_EXTI_Init_Param *init_param) {

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("AFIO_EXTI_Line_Init, DBG1: Check Null pointer.\n");
    }

      if (init_param == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("AFIO_EXTI_Line_Init, ERR: Null pointer detected.\n");
        }
        return STAT_ERROR;
      }

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("AFIO_EXTI_Line_Init, DBG2: Assert parameter.\n");
    }

      assert_param(IS_AFIO_EXTI_PORT(init_param->Port));
      assert_param(IS_GPIO_PIN(init_param->Pin));

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf(
        "AFIO_EXTI_Line_Init, DBG3: Initializing EXTI for Port %u with pin %u.\n", 
        init_param->Port, init_param->Pin
      );
    }

      ui16 index = init_param->Port / 4u; // Xác định chỉ số của AFIO_EXTICR cần cấu hình
      ui16 shift = (init_param->Pin % 4u) * 4u; // Xác định vị trí bit cần cấu hình trong AFIO_EXTICR

      // Xóa các bit cũ tại vị trí cần cấu hình
      AFIO_REGS_PTR->AFIO_EXTICR[index] &= ~(0x0Fu << shift);
      // Ghi giá trị Port vào vị trí cần cấu hình
      AFIO_REGS_PTR->AFIO_EXTICR[index] |= (init_param->Port << shift);

      /**
       * Ghi chú:
       * Ví dụ ta cần cấu hình cho chân GPIOB pin 6 (tương ứng với EXTI6):
       * - Port = AFIO_EXTICR_PORTB (0x01), Shift = 6 % 4 = 2 → index = 1, shift = (6 % 4) * 4 = 2 * 4 = 8
       * - Ta sẽ xóa các bit cũ tại vị trí bit 8-11 của AFIO_EXTICR[1] rồi ghi giá trị 0x01 vào đó để cấu hình EXTI6 cho GPIOB.
       */

      init_param->Shift = shift; // Lưu thông tin shift vào cấu trúc tham số để sử dụng cho EXTI

    return STAT_DONE;
  }

  RETR_STAT AFIO_EXTI_Line_DeInit(AFIO_EXTI_Init_Param *init_param) {

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("AFIO_EXTI_Line_DeInit, DBG1: Check Null pointer.\n");
    }

      if (init_param == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("AFIO_EXTI_Line_DeInit, ERR: Null pointer detected.\n");
        }
        return STAT_ERROR;
      }

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("AFIO_EXTI_Line_DeInit, DBG2: Assert parameter.\n");
    }

      assert_param(IS_AFIO_EXTI_PORT(init_param->Port));
      assert_param(IS_GPIO_PIN(init_param->Pin));

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf(
        "AFIO_EXTI_Line_DeInit, DBG3: Deinitializing EXTI for Port %u with pin %u.\n", 
        init_param->Port, init_param->Pin)
      ;
    }

      ui16 index = init_param->Port / 4u; // Xác định chỉ số của AFIO_EXTICR cần cấu hình
      ui16 shift = (init_param->Pin % 4u) * 4u; // Xác định vị trí bit cần cấu hình trong AFIO_EXTICR

      // Xóa các bit tại vị trí cần cấu hình để vô hiệu hóa EXTI
      AFIO_REGS_PTR->AFIO_EXTICR[index] &= ~(0x0Fu << shift);

      init_param->Shift = 15u; // Reset thông tin shift trong cấu trúc tham số

    return STAT_DONE;
  }
