/*
 * unit_test_implementation.c
 *
 *  Created on: Mar 2, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include <stdio.h>
  #include <assert.h>
  #include <string.h>
  #include <setjmp.h>
  #include "lib_keyword_def.h"
  #include "lib_condition_def.h"
  #include "lib_test_util.h"
  #include "lib_exti_def.h"
  #include "lib_exti_hal.h"
  #include "lib_afio_def.h"
  #include "lib_afio_hal.h"
  #include "lib_gpio_def.h"
  #include "lib_gpio_hal.h"
  #include "header_dependency.h"

// Khai báo ngoại vi giả cho mục đích unit test

  EXTI_REGS_Typedef MOCK_EXTI_REGS;

// Gọi ủy quyền các biến và hàm mock từ source_dependency.c

  /**
   * Ghi chú:
   * Do EXTI không sử dụng các hàm của GPIO hay AFIO mà chỉ sử dụng các khai báo định nghĩa từ các file header của GPIO và AFIO 
   * nên không cần ủy quyền biến hay hàm mock nào từ module GPIO hay AFIO.
   * Module EXTI cũng không có hàm nào phụ thuộc ngoài module nên không cần ủy quyền biến hay hàm mock nào từ module EXTI.
   */

// Khai báo cờ ngữ cảnh cho việc bắt assert trong unit test

  jmp_buf assert_env;
  ui8 assert_caught = FALSE; // Cờ để theo dõi xem assert fail đã được bắt hay chưa

// Định nghĩa các hàm

  void assert_failed(ui8* file, ui8 line) {
    printf("Assertion using assert_param failed in file %s on line %u.\n", file, line);
    assert_caught = TRUE;
    longjmp(assert_env, 1);
  }

  void setup() {
    /*
      Hàm này được gọi trước mỗi test case để khởi tạo lại trạng thái cho unit test
    */

    // Reset các ngoại vi EXTI về các giá trị reset
    memset(&MOCK_EXTI_REGS, 0, sizeof(EXTI_REGS_Typedef));
  }
