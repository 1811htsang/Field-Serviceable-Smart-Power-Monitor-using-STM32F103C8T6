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

  /*
   * Hàm khởi tạo và cấu hình chân GPIO theo tham số đầu vào.
   *
   * Tham số:
   *   GPIOx - Con trỏ tới cấu trúc thanh ghi GPIO.
   *   init_param - Con trỏ tới cấu trúc tham số khởi tạo (Pin, Mode, Pull).
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ.
   *   - Kiểm tra giá trị tham số (Pin, Mode, Pull).
   *   - Duyệt qua từng chân GPIO được chọn trong bitmask Pin.
   *   - Đối với mỗi chân, cấu hình chế độ hoạt động và pull-up/pull-down nếu cần.
   *   - Sử dụng thanh ghi CRL hoặc CRH tùy vào vị trí chân (0-7 hoặc 8-15).
   *   - Cập nhật ODR khi ở chế độ unit test để mô phỏng hiệu ứng BSRR.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu cấu hình thành công, STAT_ERROR nếu có lỗi kiểm tra.
   *
   * Phụ thuộc ngoài module GPIO:
   *   - assert_param() - Macro kiểm tra điều kiện (định nghĩa trong lib_condition_def.h)
   *   - MODIFY_REG() - Macro sửa đổi bit thanh ghi
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   *   - assert_failed() - Hàm xử lý lỗi assert
   */
  RETR_STAT GPIO_Init(GPIO_REGS_Typedef *GPIOx, GPIO_Init_Param *init_param) {
    
    // Kiểm tra con trỏ và giá trị tham số đầu vào

      if (GPIOx == NULL || init_param == NULL) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số đầu vào

      assert_param(IS_GPIO_INSTANCE(GPIOx));
      assert_param(IS_GPIO_PIN(init_param->Pin));
      assert_param(IS_GPIO_AFIO_MODE(init_param->Mode));
      assert_param(IS_GPIO_PULL(init_param->Pull));
    
    // Cấu hình hoạt hóa chân GPIO theo tham số đầu vào
    
      // Bộ biến giám sát chân

        ui16 pos = 0x0000u;
        ui16 io_pos;
        ui16 io_current;

      // Bộ biến tạm để lưu giá trị tham số

        ui16 pin_num = init_param->Pin;
        ui16 pin_mode = init_param->Mode;
        ui16 pin_pull = init_param->Pull;

      // Bộ biến tạm thanh ghi để select cấu hình

        __vo BLANK_REG* config_register;
        ui32 config_offset;

    // Vòng lặp để cấu hình từng chân GPIO được chọn

      while ((pin_num >> pos) != 0x0000u) {
        
        // Lấy vị trí chân GPIO hiện tại

          io_pos = (0x0001u << pos);
          io_current = pin_num & io_pos;

        // Nếu chân GPIO hiện tại được chọn thì cấu hình

          if (io_current == io_pos) {

            // Kiểm tra chế độ AFIO nếu được chọn

              assert_param(IS_AFIO_GPIO_SUPPORT(GPIOx));

            // Cấu hình chế độ hoạt động và cấu hình chân GPIO
              
              assert_param(IS_GPIO_AFIO_MODE(pin_mode));

              switch (pin_mode) {
                // Đối với 2 chế độ input analog và input floating thì không cần cấu hình thêm gì
                case GPIO_MODE_INPUT_ANALOG:                  
                case GPIO_MODE_INPUT_FLOATING:
                  break;

                case GPIO_MODE_INPUT_PU_PD:
                  assert_param(IS_GPIO_PULL(pin_pull));
                  if (pin_pull == GPIO_PULLUP) {
                    SET_BIT(GPIOx->GPIO_ODR, io_pos); // Kích hoạt pull-up bằng cách set bit tương ứng trong ODR
                  } else if (pin_pull == GPIO_PULLDOWN) {
                    CLEAR_BIT(GPIOx->GPIO_ODR, io_pos); // Kích hoạt pull-down bằng cách clear bit tương ứng trong ODR
                  }
                  break;

                // Đối với các chế độ output thì không cần cấu hình thêm gì
                case GPIO_MODE_OUTPUT_10MHz_PP:
                case GPIO_MODE_OUTPUT_10MHz_OD:
                case GPIO_MODE_OUTPUT_2MHz_PP:
                case GPIO_MODE_OUTPUT_2MHz_OD:
                case GPIO_MODE_OUTPUT_50MHz_PP:
                case GPIO_MODE_OUTPUT_50MHz_OD:
                  break;

                default:
                  break;
              }

              config_register = (io_pos < GPIO_PIN_8) ? 
                &GPIOx->GPIO_CRL : &GPIOx->GPIO_CRH;
              config_offset = (io_pos < GPIO_PIN_8) ? 
                (pos << 2u) : ((pos - 8u) << 2u);

              MODIFY_REG(
                *config_register, 
                (GPIO_CNF_MODE_MASK << config_offset), 
                (pin_mode << config_offset)
              );
          }
        
        // Tiếp tục với chân GPIO tiếp theo
          
          pos++;
      }
      
    // Kết thúc cấu hình, trả về trạng thái thành công

      return STAT_DONE;
  }

  /*
   * Hàm vô hiệu hóa và đặt lại cấu hình chân GPIO về trạng thái mặc định.
   *
   * Tham số:
   *   GPIOx - Con trỏ tới cấu trúc thanh ghi GPIO.
   *   Pin - Bitmask chọn các chân GPIO cần vô hiệu hóa.
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ.
   *   - Kiểm tra giá trị tham số Pin.
   *   - Duyệt qua từng chân GPIO được chọn trong bitmask Pin.
   *   - Đối với mỗi chân, đặt lại cấu hình về giá trị mặc định (GPIO_CNF_MODE_RESET).
   *   - Xóa bit ODR tương ứng để đưa chân về trạng thái mặc định.
   *   - Sử dụng thanh ghi CRL hoặc CRH tùy vào vị trí chân (0-7 hoặc 8-15).
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu vô hiệu hóa thành công, STAT_ERROR nếu có lỗi kiểm tra.
   *
   * Phụ thuộc ngoài module GPIO:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - MODIFY_REG() - Macro sửa đổi bit thanh ghi
   *   - CLEAR_BIT() - Macro xóa bit
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT GPIO_DeInit(GPIO_REGS_Typedef *GPIOx, ui16 Pin) {

    // Kiểm tra con trỏ và giá trị tham số đầu vào

      if (GPIOx == NULL) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số đầu vào

      assert_param(IS_GPIO_INSTANCE(GPIOx));
      assert_param(IS_GPIO_PIN(Pin));
    
    // Cấu hình vô hiệu hóa chân GPIO theo tham số đầu vào

      // Bộ biến giám sát chân

        ui16 pos = 0x0000u;
        ui16 io_pos;
        ui16 io_current;
        ui16 pin_num = Pin;

      // Bộ biến tạm thanh ghi để select cấu hình

        __vo BLANK_REG* config_register;
        ui32 config_offset;

      // Vòng lặp để vô hiệu hóa từng chân GPIO được chọn

        while ((pin_num >> pos) != 0x0000u) {

          // Lấy vị trí chân GPIO hiện tại

            io_pos = (0x0001u << pos);
            io_current = pin_num & io_pos;

         // Nếu chân GPIO hiện tại được chọn thì vô hiệu hóa

            if (io_current == io_pos) {
              
              // Cấu hình lại chân GPIO về trạng thái mặc định (GPIO_CNF_MODE_RESET)
              
                config_register = (io_pos < GPIO_PIN_8) ? 
                  &GPIOx->GPIO_CRL : &GPIOx->GPIO_CRH;
                config_offset = (io_pos < GPIO_PIN_8) ? 
                  (pos << 2u) : ((pos - 8u) << 2u);

                MODIFY_REG(
                  *config_register, 
                  (GPIO_CNF_MODE_MASK << config_offset), 
                  (GPIO_CNF_MODE_RESET << config_offset)
                );
                CLEAR_BIT(GPIOx->GPIO_ODR, io_current); // Reset ODR bit tương ứng để đưa chân về trạng thái mặc định
            }

          // Tiếp tục với chân GPIO tiếp theo
            pos++;
        }

    // Kết thúc vô hiệu hóa, trả về trạng thái thành công
    
      return STAT_DONE;
  }

  /*
   * Hàm xử lý ngắt ngoại vi GPIO/EXTI cho các chân được chỉ định.
   *
   * Tham số:
   *   Pin - Bitmask chọn chân GPIO gây ra ngắt.
   *
   * Logic:
   *   - Gọi hàm callback weak GPIO_EXTI_Callback() để user xử lý ngắt.
   *   - Callback này có thể được override trong user code theo nhu cầu.
   *   - Hàm này thường được gọi từ các handler ngắt ngoài (EXTI0_IRQHandler, v.v.).
   *
   * Trả về:
   *   Không có (void).
   *
   * Phụ thuộc ngoài module GPIO:
   *   - GPIO_EXTI_Callback() - Hàm callback weak (định nghĩa trong lib_gpio_hal.h)
   */
  void GPIO_EXTI_IRQHandler(ui16 Pin) {
    GPIO_EXTI_Callback(Pin);
  }
