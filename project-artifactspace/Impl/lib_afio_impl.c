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

  /*
   * Hàm cấu hình remap chân cho các ngoại vi sử dụng AFIO.
   *
   * Tham số:
   *   Peri - Mã định danh ngoại vi cần remap (bitmask từ AFIO_MAPR).
   *
   * Logic:
   *   - Kiểm tra giá trị tham số Peri hợp lệ.
   *   - Ghi giá trị Peri vào thanh ghi AFIO_MAPR để kích hoạt remap.
   *   - Thanh ghi MAPR cho phép remap các chức năng alternate của ngoại vi
   *     như USART, SPI, I2C, TIM sang các chân GPIO khác.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu remap thành công.
   *
   * Phụ thuộc ngoài module AFIO:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
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

  /*
   * Hàm khởi tạo EXTI line cho một chân GPIO cụ thể thông qua AFIO.
   *
   * Tham số:
   *   init_param - Con trỏ tới cấu trúc tham số khởi tạo EXTI (Port, Pin).
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ.
   *   - Kiểm tra giá trị tham số Port và Pin.
   *   - Tính toán index thanh ghi EXTICR cần cấu hình (index = Port / 4).
   *   - Tính toán vị trí bit shift trong thanh ghi (shift = (Pin % 4) * 4).
   *   - Xóa các bit cũ tại vị trí cần cấu hình (4 bits).
   *   - Ghi giá trị Port vào vị trí đã tính toán để map EXTI line với GPIO port.
   *   - Lưu thông tin shift vào init_param để sử dụng cho cấu hình EXTI tiếp theo.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu khởi tạo thành công, STAT_ERROR nếu có lỗi.
   *
   * Phụ thuộc ngoài module AFIO:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
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
        "AFIO_EXTI_Line_Init, DBG3: Initializing EXTI Line for Port %u with pin %u.\n", 
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

      init_param->Line = shift; // Lưu thông tin Line vào cấu trúc tham số để sử dụng cho EXTI

    return STAT_DONE;
  }

  /*
   * Hàm vô hiệu hóa EXTI line cho một chân GPIO cụ thể thông qua AFIO.
   *
   * Tham số:
   *   init_param - Con trỏ tới cấu trúc tham số EXTI đã khởi tạo (Port, Pin).
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ.
   *   - Kiểm tra giá trị tham số Port và Pin.
   *   - Tính toán index thanh ghi EXTICR cần xóa cấu hình (index = Port / 4).
   *   - Tính toán vị trí bit shift trong thanh ghi (shift = (Pin % 4) * 4).
   *   - Xóa các bit tại vị trí đã tính toán để vô hiệu hóa EXTI line.
   *   - Reset thông tin shift trong init_param về giá trị mặc định (15u).
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu vô hiệu hóa thành công, STAT_ERROR nếu có lỗi.
   *
   * Phụ thuộc ngoài module AFIO:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
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

      init_param->Line = 15u; // Reset thông tin Line trong cấu trúc tham số

    return STAT_DONE;
  }
