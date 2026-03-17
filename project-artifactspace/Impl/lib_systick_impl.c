/*
 * lib_systick_impl.c
 *
 *  Created on: Mar 17, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"
    #include "lib_systick_def.h"
    #include "lib_systick_hal.h"
    #include "header_dependency.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
  #include <string.h>

  #ifndef UNIT_TEST
    #include "generic/lib_keyword_def.h"
    #include "generic/lib_condition_def.h"
    #include "systick/lib_systick_def.h"
    #include "systick/lib_systick_hal.h"
  #endif

// Định nghĩa các hàm thành phần

  void SYSTICK_Init(ui32 ticks) {

    // Kiểm tra tham số đầu vào

      assert_param(IS_SYSTICK_TICKS(ticks)); // Kiểm tra tính hợp lệ của tham số ticks, đảm bảo nó nằm trong khoảng cho phép của thanh ghi LOAD

    // Xóa thanh ghi điều khiển để đảm bảo ngoại vi ở trạng thái ban đầu trước khi cấu hình

      CLEAR_REG(SYSTICK_REGS_PTR->SYSTICK_CTRL); // Đảm bảo thanh ghi điều khiển được xóa trước khi cấu hình

    // Cấu hình giá trị tải lại

      WRITE_REG(SYSTICK_REGS_PTR->SYSTICK_LOAD, ticks - 1); // Cấu hình giá trị tải lại vào thanh ghi LOAD, trừ đi 1 vì bộ đếm sẽ đếm từ giá trị này xuống 0

    // Cấu hình mức ưu tiên ngắt của SysTick 

      /**
       * Ghi chú:
       * Phần này ở hàm main sẽ được gọi cấu hình để tránh phải include nvic vào thư viện này, 
       * giúp giảm sự phụ thuộc giữa các thư viện và tăng tính module hóa của code
       */

    // Reset giá trị đếm hiện tại

      WRITE_REG(SYSTICK_REGS_PTR->SYSTICK_VAL, 0); // Đặt giá trị đếm hiện tại về 0 để bắt đầu đếm từ đầu

    // Bật SysTick với nguồn xung là clock hệ thống và cho phép ngắt

      SET_BIT(SYSTICK_REGS_PTR->SYSTICK_CTRL, SYSTICK_CTRL_CLKSOURCE); // Chọn nguồn xung là clock hệ thống
      SET_BIT(SYSTICK_REGS_PTR->SYSTICK_CTRL, SYSTICK_CTRL_TICKINT); // Cho phép ngắt khi bộ đếm về 0
      SET_BIT(SYSTICK_REGS_PTR->SYSTICK_CTRL, SYSTICK_CTRL_ENABLE); // Bật SysTick để bắt đầu đếm

  }

  void SYSTICK_DeInit(void) {

    // Tắt SysTick và xóa các cấu hình đã thiết lập

      CLEAR_BIT(SYSTICK_REGS_PTR->SYSTICK_CTRL, SYSTICK_CTRL_ENABLE); // Tắt SysTick để dừng đếm
      CLEAR_BIT(SYSTICK_REGS_PTR->SYSTICK_CTRL, SYSTICK_CTRL_TICKINT); // Vô hiệu hóa ngắt từ SysTick
      CLEAR_BIT(SYSTICK_REGS_PTR->SYSTICK_CTRL, SYSTICK_CTRL_CLKSOURCE); // Chọn lại nguồn xung mặc định (nếu cần)

    // Xóa giá trị tải lại và giá trị đếm hiện tại để đưa ngoại vi về trạng thái ban đầu

      WRITE_REG(SYSTICK_REGS_PTR->SYSTICK_LOAD, 0); // Đặt giá trị tải lại về 0
      WRITE_REG(SYSTICK_REGS_PTR->SYSTICK_VAL, 0); // Đặt giá trị đếm hiện tại về 0

    // Reset biến đếm tick về 0

      ms_ticks = 0; // Đặt lại biến đếm tick về 0 để chuẩn bị cho lần khởi tạo tiếp theo
  }

  void SYSTICK_DelayMs(ui32 delay_ms) {
    
    ui32 start_tick = SYSTICK_GetTick(); // Lấy giá trị tick hiện tại làm mốc bắt đầu

    // Chờ cho đến khi số lượng tick đã trôi qua đủ để tạo ra khoảng thời gian delay mong muốn

      while ((SYSTICK_GetTick() - start_tick) < delay_ms) {
        // Vòng lặp chờ, không làm gì trong khi chờ đợi
      }
  }
