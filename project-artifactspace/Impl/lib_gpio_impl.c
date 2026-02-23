/*
 * lib_gpio_impl.c
 *
 *  Created on: Feb 23, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
		#include "lib_keyword_def.h"
		#include "lib_condition_def.h"
		#include "lib_gpio_def.h"
		#include "lib_gpio_hal.h"
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
  #endif

// Định nghĩa các hàm thành phần

  RETR_STAT GPIO_Init(GPIO_REGS_Typedef *GPIOx, GPIO_Init_Param *init_param) {
    
    // Kiểm tra con trỏ và giá trị tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("GPIO_Init, DBG1: Check Null pointer.\n");
    }

      if (GPIOx == NULL || init_param == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("GPIO_Init, ERR: Null pointer detected.\n");
        }
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("GPIO_Init, DBG2: Assert parameter.\n");
    }

      assert_param(IS_GPIO_PIN(init_param->Pin));
      assert_param(IS_GPIO_CONFIG(init_param->Config));
      assert_param(IS_GPIO_MODE(init_param->Mode));
    
    // Cấu hình chân GPIO theo tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("GPIO_Init, DBG3: Configure GPIO variable.\n");
    }
    
      // Bộ biến giám sát chân
      ui16 pos = 0x0000u;
      ui16 io_pos;
      ui16 io_current;

      // Bộ biến tạm để lưu giá trị tham số
      ui8 pin_num = init_param->Pin;
      ui8 pin_cnf = init_param->Config;
      ui8 pin_mode = init_param->Mode;
      bool pin_pull = init_param->Pull;

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("GPIO_Init, DBG4: Start configuring GPIO.\n");
    }

      // Vòng lặp để cấu hình từng chân GPIO được chọn
      while ((pin_num >> pos) != 0x0000u) {
        
        // Lấy vị trí chân GPIO hiện tại
          io_pos = (0x0001u << pos);
          io_current = pin_num & io_pos;

          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("GPIO_Init, DBG4-%u: Checking pin %u.\n", pos, pos);
          }

        // Nếu chân GPIO hiện tại được chọn thì cấu hình

          if (io_current == io_pos) {
            // Kiểm tra xem GPIO có hỗ trợ cấu hình AFIO hay không;
              if (__DEBUG_GET_MODE(ENABLE)) {
                printf("GPIO_Init, DBG4-%u: Checking AFIO support.\n", pos);
              }

                assert_param(IS_AFIO_GPIO_SUPPORT(GPIOx));

            // Cấu hình chế độ hoạt động và cấu hình chân GPIO
              if (__DEBUG_GET_MODE(ENABLE)) {
                printf("GPIO_Init, DBG4-%u: Configuring pin mode and config.\n", pos);
              }

                switch (pin_mode) {
                  case GPIO_MODE_INPUT:
                    // Cấu hình chân GPIO ở chế độ input
                    // Cấu hình pull-up/pull-down nếu cần
                    // (Cấu hình cụ thể sẽ phụ thuộc vào phần cứng và yêu cầu thiết kế)
                    break;
                  
                  case GPIO_MODE_OUTPUT_10MHz:
                    break;

                  case GPIO_MODE_OUTPUT_2MHz:
                    break;

                  case GPIO_MODE_OUTPUT_50MHz:
                    break;
                  
                  default:
                    // Trường hợp không hợp lệ, có thể thêm xử lý lỗi nếu cần
                    break;
                }
          }

        // Tiếp tục với chân GPIO tiếp theo
          pos++;
      }
      
    return STAT_DONE;
  }

  RETR_STAT GPIO_DeInit(GPIO_REGS_Typedef *GPIOx, ui8 Pin) {
    return STAT_DONE;
  }

  PIN_RETR_Enum GPIO_ReadPin(GPIO_REGS_Typedef *GPIOx, ui8 Pin) {
    return GPIO_PIN_UNF;
  }

  void GPIO_WritePin(
    GPIO_REGS_Typedef *GPIOx, 
    ui8 Pin, 
    PIN_RETR_Enum PinState
  ) {

  }

  void GPIO_TogglePin(GPIO_REGS_Typedef *GPIOx, ui8 Pin) {

  }

  RETR_STAT GPIO_LockPin(GPIO_REGS_Typedef *GPIOx, ui8 Pin) {
    return STAT_DONE;
  }

  void GPIO_EXTI_IRQHandler(ui8 Pin) {
    
  }