/*
 * lib_exti_impl.c
 *
 *  Created on: Mar 1, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
		#include "lib_keyword_def.h"
		#include "lib_condition_def.h"
		#include "lib_exti_def.h"
		#include "lib_exti_hal.h"
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
    #include "exti/lib_exti_def.h"
    #include "exti/lib_exti_hal.h"
  	#include "afio/lib_afio_def.h"
  	#include "afio/lib_afio_hal.h"
  #endif

// Định nghĩa các hàm thành phần

  /*
   * Hàm khởi tạo và cấu hình line EXTI theo tham số đầu vào.
   *
   * Tham số:
   *   gpio_init_param - Con trỏ tới cấu trúc tham số GPIO (chứa thông tin trigger).
   *   afio_init_param - Con trỏ tới cấu trúc tham số AFIO EXTI (Port, Pin, Line).
   *
   * Logic:
   *   - Kiểm tra con trỏ afio_init_param hợp lệ.
   *   - Kiểm tra giá trị tham số trigger (NONE, FALLING, RISING, BOTH).
   *   - Dựa vào loại trigger được chọn, cấu hình bit FTSR (falling) và RTSR (rising):
   *       + FALLING: Set FTSR, Clear RTSR
   *       + RISING: Set RTSR, Clear FTSR
   *       + BOTH: Set cả FTSR và RTSR
   *   - Kích hoạt line EXTI bằng cách set bit tương ứng trong IMR.
   *   - Clear pending bit trong PR để xóa ngắt cũ nếu có.
   *   - Trả về lỗi nếu trigger NONE hoặc giá trị không hợp lệ.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu cấu hình thành công, STAT_ERROR nếu có lỗi.
   *
   * Phụ thuộc ngoài module EXTI:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - AFIO_EXTI_Line_Init() - Hàm khởi tạo AFIO EXTI
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT EXTI_Config_Init(
    GPIO_Init_Param *gpio_init_param,
    AFIO_EXTI_Init_Param *afio_init_param
  ) {
    
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_Config_Init, DBG1: Check Null pointer.\n");
    }

      if (afio_init_param == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("EXTI_Config_Init, ERR: Null pointer detected.\n");
        }
        return STAT_ERROR;
      }

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_Config_Init, DBG2: Assert parameter.\n");
    }

      assert_param(IS_GPIO_TRIGGER(gpio_init_param->Trigger));

    /**
     * Ghi chú:
     * Do AFIO_EXTI_Iinit_Param đã được kiểm tra kỹ lưỡng trong 
     * hàm AFIO_EXTI_Line_Init nên ta sẽ không kiểm tra lại ở đây nữa.
     */

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_Config_Init, DBG3: Initializing EXTI config for Line %u. \n", afio_init_param->Line);
    }

      // Cấu hình cạnh lên / xuống cho line EXTI dựa vào thông tin trigger trong GPIO_Init_Param
      switch (gpio_init_param->Trigger) {
        
        case GPIO_TRIGGER_NONE:
          printf("EXTI_Config_Init, DBG3: No trigger selected, skipping EXTI configuration for Line %u.\n", afio_init_param->Line);
          return STAT_ERROR; // Nếu không có trigger nào được chọn thì trả về lỗi vì không thể cấu hình EXTI cho line này  
          break;

        case GPIO_TRIGGER_FALLING:
          EXTI_REGS_PTR->EXTI_FTSR |= (0x0001u << afio_init_param->Line); // Set bit tương ứng trong FTSR để cấu hình trigger falling
          EXTI_REGS_PTR->EXTI_RTSR &= ~(0x0001u << afio_init_param->Line); // Clear bit tương ứng trong RTSR để đảm bảo chỉ có trigger falling được kích hoạt
          break;

        case GPIO_TRIGGER_RISING:
          EXTI_REGS_PTR->EXTI_RTSR |= (0x0001u << afio_init_param->Line); // Set bit tương ứng trong RTSR để cấu hình trigger rising
          EXTI_REGS_PTR->EXTI_FTSR &= ~(0x0001u << afio_init_param->Line); // Clear bit tương ứng trong FTSR để đảm bảo chỉ có trigger rising được kích hoạt
          break;

        case GPIO_TRIGGER_BOTH:
          EXTI_REGS_PTR->EXTI_RTSR |= (0x0001u << afio_init_param->Line); // Set bit tương ứng trong RTSR để cấu hình trigger rising
          EXTI_REGS_PTR->EXTI_FTSR |= (0x0001u << afio_init_param->Line); // Set bit tương ứng trong FTSR để cấu hình trigger falling
          break;
        
        default:
          return STAT_ERROR; // Nếu giá trị trigger không hợp lệ thì trả về lỗi
          break;
      }

      EXTI_REGS_PTR->EXTI_IMR |= (0x0001u << afio_init_param->Line); // Set bit tương ứng trong IMR để kích hoạt line EXTI
      EXTI_REGS_PTR->EXTI_PR |= (0x0001u << afio_init_param->Line); // Clear pending bit của line EXTI để đảm bảo không có ngắt nào bị bỏ sót ngay sau khi cấu hình

    return STAT_DONE;
  }

  /*
   * Hàm vô hiệu hóa và xóa cấu hình line EXTI.
   *
   * Tham số:
   *   gpio_init_param - Con trỏ tới cấu trúc tham số GPIO (chứa thông tin trigger).
   *   afio_init_param - Con trỏ tới cấu trúc tham số AFIO EXTI (Port, Pin, Line).
   *
   * Logic:
   *   - Kiểm tra con trỏ afio_init_param hợp lệ.
   *   - Kiểm tra giá trị tham số trigger.
   *   - Vô hiệu hóa line EXTI bằng cách clear bit trong IMR.
   *   - Xóa cấu hình trigger bằng cách clear bit trong RTSR.
   *   - Xóa cấu hình trigger bằng cách clear bit trong FTSR.
   *   - Clear pending bit trong PR để xóa ngắt nếu có.
   *   - Reset thông tin Line về giá trị mặc định (15u).
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu vô hiệu hóa thành công, STAT_ERROR nếu có lỗi.
   *
   * Phụ thuộc ngoài module EXTI:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - AFIO_EXTI_Line_DeInit() - Hàm vô hiệu hóa AFIO EXTI
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT EXTI_Config_DeInit(
    GPIO_Init_Param *gpio_init_param,
    AFIO_EXTI_Init_Param *afio_init_param
  ) {

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_Config_DeInit, DBG1: Check Null pointer.\n");
    }

      if (afio_init_param == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("EXTI_Config_DeInit, ERR: Null pointer detected.\n");
        }
        return STAT_ERROR;
      }

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_Config_DeInit, DBG2: Assert parameter.\n");
    }

      assert_param(IS_GPIO_TRIGGER(gpio_init_param->Trigger));

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_Config_DeInit, DBG3: Deinitializing EXTI config for Line %u. \n", afio_init_param->Line);
    }

      // Vô hiệu hóa line EXTI bằng cách xóa bit tương ứng trong IMR
      EXTI_REGS_PTR->EXTI_IMR &= ~(0x0001u << afio_init_param->Line);

      // Xóa cấu hình trigger cạnh lên / xuống cho line EXTI bằng cách xóa bit tương ứng trong RTSR và FTSR
      EXTI_REGS_PTR->EXTI_RTSR &= ~(0x0001u << afio_init_param->Line);
      EXTI_REGS_PTR->EXTI_FTSR &= ~(0x0001u << afio_init_param->Line);

      // Clear pending bit của line EXTI để đảm bảo không có ngắt nào bị bỏ sót ngay sau khi vô hiệu hóa
      EXTI_REGS_PTR->EXTI_PR |= (0x0001u << afio_init_param->Line);

      afio_init_param->Line = 15u; // Reset thông tin Line trong cấu trúc tham số về giá trị mặc định

    return STAT_DONE;
  }

  /*
   * Hàm xử lý ngắt EXTI cho một line cụ thể.
   *
   * Tham số:
   *   handle_param - Con trỏ tới cấu trúc handle EXTI (chứa Line và Callback).
   *
   * Logic:
   *   - Kiểm tra con trỏ handle_param hợp lệ.
   *   - Kiểm tra giá trị tham số Line.
   *   - Kiểm tra pending bit trong PR cho line tương ứng.
   *   - Nếu pending bit được set:
   *       + Clear pending bit bằng cách ghi 1 vào bit trong PR.
   *       + Gọi hàm callback đã đăng ký (nếu khác NULL).
   *   - Nếu không có pending bit, in thông báo debug.
   *
   * Trả về:
   *   Không có (void).
   *
   * Phụ thuộc ngoài module EXTI:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - EXTI_RegisterCallback() - Hàm đăng ký callback
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  void EXTI_IRQHandler(EXTI_Handle_Param *handle_param) {

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_IRQHandler, DBG1: Check Null pointer.\n");
    }

      if (handle_param == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("EXTI_IRQHandler, ERR: Null pointer detected.\n");
        }
        return;
      }

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_IRQHandler, DBG2: Assert parameter.\n");
    }

      assert_param(IS_EXTI_LINE(handle_param->Line));

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_IRQHandler, DBG3: Handling EXTI interrupt for Line %u.\n", handle_param->Line);
    }

      // Kiểm tra nếu pending bit của line EXTI được set thì gọi hàm callback tương ứng
      if ((EXTI_REGS_PTR->EXTI_PR & (0x0001u << handle_param->Line)) != 0) {
        EXTI_REGS_PTR->EXTI_PR |= (0x0001u << handle_param->Line); // Clear pending bit sau khi đã xử lý ngắt
        if (handle_param->Callback != NULL) {
          handle_param->Callback(); // Gọi hàm callback đã đăng ký để xử lý ngắt
        }
      } else {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("EXTI_IRQHandler, DBG3: No pending interrupt for Line %u.\n", handle_param->Line);
        }
      }
  }

  /*
   * Hàm đăng ký callback (hàm xử lý) cho một line EXTI.
   *
   * Tham số:
   *   handle_param - Con trỏ tới cấu trúc handle EXTI (sẽ lưu trữ callback).
   *   callback_event_type - Loại sự kiện callback (EXTI_COMMON_CB_ID).
   *   callback_func - Con trỏ tới hàm callback cần đăng ký (không được NULL).
   *
   * Logic:
   *   - Kiểm tra con trỏ handle_param hợp lệ.
   *   - Kiểm tra con trỏ callback_func không NULL.
   *   - Dựa vào callback_event_type, đăng ký hàm callback:
   *       + EXTI_COMMON_CB_ID: Đăng ký vào trường Callback của handle_param.
   *       + Loại khác: Trả về STAT_ERROR.
   *   - Lưu con trỏ callback_func vào handle_param->Callback.
   *   - Hàm callback sẽ được gọi từ EXTI_IRQHandler khi ngắt xảy ra.
   *
   * Trả về:
   *   RETR_STAT - STAT_OK nếu đăng ký thành công, STAT_ERROR nếu lỗi.
   *
   * Phụ thuộc ngoài module EXTI:
   *   - EXTI_IRQHandler() - Hàm xử lý ngắt gọi callback này
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT EXTI_RegisterCallback(
    EXTI_Handle_Param *handle_param,
    EXTI_Callback_Event_Param_Type callback_event_type,
    void (*callback_func)(void)
  ) {

    RETR_STAT status = STAT_OK;
    
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_RegisterCallback, DBG1: Check Null pointer.\n");
    }

      if (handle_param == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("EXTI_RegisterCallback, ERR: Null pointer detected.\n");
        }
        return STAT_ERROR;
      }

      if (callback_func == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("EXTI_RegisterCallback, ERR: Null pointer detected for callback function.\n");
        }
        return STAT_ERROR;
      }

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_RegisterCallback, DBG2: Registering callback for Line %u.\n", handle_param->Line);
    }

      switch (callback_event_type) {
        case EXTI_COMMON_CB_ID:
          handle_param->Callback = callback_func; // Đăng ký hàm callback vào trường Callback của handle_param
          break;
        
        default:
          status = STAT_ERROR; // Nếu callback_event_type không hợp lệ thì trả về lỗi
          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("EXTI_RegisterCallback, ERR: Invalid callback event type %u.\n", callback_event_type);
          }
          break;
      }
    
    return status;
  }

  /*
   * Hàm tạo ngắt EXTI bằng phần mềm (Software Interrupt) cho một line cụ thể.
   *
   * Tham số:
   *   handle_param - Con trỏ tới cấu trúc handle EXTI (chứa Line).
   *
   * Logic:
   *   - Kiểm tra con trỏ handle_param hợp lệ.
   *   - Kiểm tra giá trị tham số Line.
   *   - Set bit tương ứng trong SWIER để tạo ngắt EXTI bằng phần mềm.
   *   - Ngắt này sẽ kích hoạt EXTI_IRQHandler như ngắt thông thường.
   *   - Hữu ích cho testing hoặc mô phỏng sự kiện EXTI.
   *
   * Trả về:
   *   Không có (void).
   *
   * Phụ thuộc ngoài module EXTI:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - EXTI_IRQHandler() - Hàm xử lý ngắt được gọi
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  void EXTI_GenerateSWI(EXTI_Handle_Param *handle_param) {
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_GenerateSWI, DBG1: Check Null pointer.\n");
    }

      if (handle_param == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("EXTI_GenerateSWI, ERR: Null pointer detected.\n");
        }
        return;
      }

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_GenerateSWI, DBG2: Assert parameter.\n");
    }

      assert_param(IS_EXTI_LINE(handle_param->Line));

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("EXTI_GenerateSWI, DBG3: Generating software interrupt for Line %u.\n", handle_param->Line);
    }

      EXTI_REGS_PTR->EXTI_SWIER |= (0x0001u << handle_param->Line); // Set bit tương ứng trong SWIER để tạo ngắt EXTI bằng phần mềm
  }

