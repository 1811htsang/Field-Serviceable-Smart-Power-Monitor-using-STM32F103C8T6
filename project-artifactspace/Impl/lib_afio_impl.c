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

    // Kiểm tra con trỏ đầu vào hợp lệ

      if (init_param == NULL) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số Port và Pin

      assert_param(IS_AFIO_EXTI_PORT(init_param->Port));
      assert_param(IS_GPIO_PIN(init_param->Pin));

    // Tính toán index thanh ghi EXTICR cần cấu hình và vị trí bit shift

      ui16 pin_index = GPIO_GetPinIndex(init_param->Pin);
      ui16 reg_index = (pin_index / 4u); // Xác định chỉ số của AFIO_EXTICR cần cấu hình
      ui16 shift = (pin_index % 4u) * 4u; // Xác định vị trí bit cần cấu hình trong AFIO_EXTICR

      /**
       * Ghi chú:
       * Trong tài liệu, index-based là 1
       * Trong thiết kế code thì index-based là 0
       */

      // >> Xóa các bit cũ tại vị trí cần cấu hình

      	AFIO_REGS_PTR->AFIO_EXTICR[reg_index] &= ~(0x0Fu << shift);

      // >> Ghi giá trị Port vào vị trí cần cấu hình

      	AFIO_REGS_PTR->AFIO_EXTICR[reg_index] |= (init_param->Port << shift);

				/**
				 * Ghi chú:
				 * Ví dụ ta cần cấu hình cho chân GPIOB pin 6 (tương ứng với EXTI6):
				 * - Port = AFIO_EXTICR_PORTB (0x01), Shift = 6 % 4 = 2 → index = 1, shift = (6 % 4) * 4 = 2 * 4 = 8
				 * - Ta sẽ xóa các bit cũ tại vị trí bit 8-11 của AFIO_EXTICR[1] rồi ghi giá trị 0x01 vào đó để cấu hình EXTI6 cho GPIOB.
				 */

		// Lưu thông tin Line vào cấu trúc tham số để sử dụng cho EXTI

			init_param->Line = pin_index;

    // Trả về trạng thái hoàn thành

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

    // Kiểm tra con trỏ đầu vào hợp lệ

      if (init_param == NULL) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số Port và Pin

      assert_param(IS_AFIO_EXTI_PORT(init_param->Port));
      assert_param(IS_GPIO_PIN(init_param->Pin));

    // Tính toán index thanh ghi EXTICR cần xóa cấu hình và vị trí bit shift

      ui16 pin_index = GPIO_GetPinIndex(init_param->Pin);
      ui16 reg_index = (pin_index / 4u); // Xác định chỉ số của AFIO_EXTICR cần cấu hình
      ui16 shift = (pin_index % 4u) * 4u; // Xác định vị trí bit cần cấu hình trong AFIO_EXTICR

      // >> Xóa các bit tại vị trí cần cấu hình để vô hiệu hóa EXTI

      	AFIO_REGS_PTR->AFIO_EXTICR[reg_index] &= ~(0x0Fu << shift);


    // Reset thông tin Line trong cấu trúc tham số

      init_param->Line = 15u;

    // Trả về trạng thái hoàn thành

      return STAT_DONE;
  }
