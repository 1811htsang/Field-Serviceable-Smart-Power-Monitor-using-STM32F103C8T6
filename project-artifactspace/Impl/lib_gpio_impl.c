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

      assert_param(IS_GPIO_INSTANCE(GPIOx));
      assert_param(IS_GPIO_PIN(init_param->Pin));
      assert_param(IS_GPIO_AFIO_MODE(init_param->Mode));
      assert_param(IS_GPIO_PULL(init_param->Pull));
    
    // Cấu hình hoạt hóa chân GPIO theo tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("GPIO_Init, DBG3: Configure GPIO variable.\n");
    }
    
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

            if (__DEBUG_GET_MODE(ENABLE)) {
              printf("GPIO_Init, DBG4-%u: Configuring pin %u.\n", pos, pos);
            }

            // Kiểm tra xem GPIO có hỗ trợ cấu hình AFIO hay không;
              if (__DEBUG_GET_MODE(ENABLE)) {
                printf("GPIO_Init, DBG4-%u: Checking AFIO support.\n", pos);
              }

                assert_param(IS_AFIO_GPIO_SUPPORT(GPIOx));

            // Cấu hình chế độ hoạt động và cấu hình chân GPIO
              if (__DEBUG_GET_MODE(ENABLE)) {
                printf("GPIO_Init, DBG4-%u: Configuring pin mode and config.\n", pos);
              }
                assert_param(IS_GPIO_AFIO_MODE(pin_mode));

                switch (pin_mode) {
                  // Đối với 2 chế độ input analog và input floating thì không cần cấu hình thêm gì
                  case GPIO_MODE_INPUT_ANALOG:                  
                  case GPIO_MODE_INPUT_FLOATING:
                    break;

                  case GPIO_MODE_INPUT_PU_PD:
                    assert_param(IS_GPIO_PULL(pin_pull));
                    if (pin_pull == GPIO_PULLUP) {
                      GPIOx->GPIO_BSRR = io_pos; // Kích hoạt pull-up bằng cách set bit tương ứng trong ODR
                    } else if (pin_pull == GPIO_PULLDOWN) {
                      GPIOx->GPIO_BRR = io_pos; // Kích hoạt pull-down bằng cách reset bit tương ứng trong ODR
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

                config_register = (io_pos < GPIO_PIN_8) ? &GPIOx->GPIO_CRL : &GPIOx->GPIO_CRH;
                config_offset = (io_pos < GPIO_PIN_8) ? (pos << 2u) : ((pos - 8u) << 2u);

                MODIFY_REG(
                  *config_register, 
                  (GPIO_CNF_MODE_MASK << config_offset), 
                  (pin_mode << config_offset)
                );
          }
        
        // Tiếp tục với chân GPIO tiếp theo
          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("GPIO_Init, DBG4-%u: Finished configuring pin %u.\n", pos, pos);
          }
          pos++;
      }
      
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
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("GPIO_DeInit, DBG1: Check Null pointer.\n");
    }

      if (GPIOx == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("GPIO_DeInit, ERR: Null pointer detected.\n");
        }
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("GPIO_DeInit, DBG2: Assert parameter.\n");
    }

      assert_param(IS_GPIO_INSTANCE(GPIOx));
      assert_param(IS_GPIO_PIN(Pin));
    
    // Cấu hình vô hiệu hóa chân GPIO theo tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("GPIO_DeInit, DBG3: Deinitialize GPIO pin %u.\n", Pin);
    }

      // Bộ biến giám sát chân
        ui16 pos = 0x0000u;
        ui16 io_pos;
        ui16 io_current;
        ui16 pin_num = Pin;

      // Bộ biến tạm thanh ghi để select cấu hình
        __vo BLANK_REG* config_register;
        ui32 config_offset;

      while ((pin_num >> pos) != 0x0000u) {
        // Lấy vị trí chân GPIO hiện tại
        io_pos = (0x0001u << pos);
        io_current = pin_num & io_pos;

        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("GPIO_DeInit, DBG3-%u: Checking pin %u.\n", pos, pos);
        }

          if (io_current == io_pos) {
            
            if (__DEBUG_GET_MODE(ENABLE)) {
              printf("GPIO_DeInit, DBG3-%u: Deinitializing pin %u.\n", pos, pos);
            }
            
            config_register = (io_pos < GPIO_PIN_8) ? &GPIOx->GPIO_CRL : &GPIOx->GPIO_CRH;
            config_offset = (io_pos < GPIO_PIN_8) ? (pos << 2u) : ((pos - 8u) << 2u);

            MODIFY_REG(
              *config_register, 
              (GPIO_CNF_MODE_MASK << config_offset), 
              (GPIO_CNF_MODE_RESET << config_offset)
            );
            CLEAR_BIT(GPIOx->GPIO_ODR, io_current); // Reset ODR bit tương ứng để đưa chân về trạng thái mặc định
          }

        // Tiếp tục với chân GPIO tiếp theo
          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("GPIO_DeInit, DBG3-%u: Finished deinitializing pin %u.\n", pos, pos);
          }
            pos++;
      }

    return STAT_DONE;
  }

  /*
   * Hàm đọc trạng thái logic của một chân GPIO cụ thể.
   *
   * Tham số:
   *   GPIOx - Con trỏ tới cấu trúc thanh ghi GPIO.
   *   Pin - Bitmask chọn chân GPIO cần đọc (chỉ hỗ trợ 1 chân).
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ, trả về GPIO_PIN_UNF nếu null.
   *   - Kiểm tra giá trị tham số Pin.
   *   - Đọc giá trị từ thanh ghi IDR (Input Data Register).
   *   - Nếu bit tương ứng được set (≠ 0) trả về GPIO_PIN_SET.
   *   - Nếu bit tương ứng được reset (= 0) trả về GPIO_PIN_RESET.
   *
   * Trả về:
   *   PIN_RETR_Enum - GPIO_PIN_SET nếu chân ở mức cao, GPIO_PIN_RESET nếu ở mức thấp,
   *                  GPIO_PIN_UNF nếu có lỗi (null pointer).
   *
   * Phụ thuộc ngoài module GPIO:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  PIN_RETR_Enum GPIO_ReadPin(GPIO_REGS_Typedef *GPIOx, ui16 Pin) {
    // Kiểm tra con trỏ và giá trị tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_ReadPin, DBG1: Check Null pointer.\n");
      }

        if (GPIOx == NULL) {
          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("GPIO_ReadPin, ERR: Null pointer detected.\n");
          }
          return GPIO_PIN_UNF;
        }
      
    // Kiểm tra giá trị tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_ReadPin, DBG2: Assert parameter.\n");
      }

        assert_param(IS_GPIO_INSTANCE(GPIOx));
        assert_param(IS_GPIO_PIN(Pin));

    // Đọc trạng thái chân GPIO theo tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_ReadPin, DBG3: Reading pin %u state.\n", Pin);
      }
  
        if ((GPIOx->GPIO_IDR & Pin) != (ui32)0x00000000u) {
          return GPIO_PIN_SET;
        } else {
          return GPIO_PIN_RESET;
        }
  }

  /*
   * Hàm ghi trạng thái logic cho một chân GPIO cụ thể.
   *
   * Tham số:
   *   GPIOx - Con trỏ tới cấu trúc thanh ghi GPIO.
   *   Pin - Bitmask chọn chân GPIO cần ghi (chỉ hỗ trợ 1 chân).
   *   PinState - Trạng thái muốn ghi (GPIO_PIN_SET hoặc GPIO_PIN_RESET).
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ, trả về nếu null.
   *   - Kiểm tra giá trị tham số Pin và PinState.
   *   - Nếu PinState == GPIO_PIN_SET: ghi giá trị vào nửa dưới BSRR (Bit Set).
   *   - Nếu PinState == GPIO_PIN_RESET: ghi giá trị vào nửa trên BSRR (Bit Reset, dịch trái 16 bit).
   *   - Trong chế độ unit test, cập nhật ODR để mô phỏng hiệu ứng BSRR.
   *
   * Trả về:
   *   Không có (void).
   *
   * Phụ thuộc ngoài module GPIO:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  void GPIO_WritePin(
    GPIO_REGS_Typedef *GPIOx, 
    ui16 Pin, 
    PIN_RETR_Enum PinState
  ) {
    // Kiểm tra con trỏ và giá trị tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_WritePin, DBG1: Check Null pointer.\n");
      }

        if (GPIOx == NULL) {
          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("GPIO_WritePin, ERR: Null pointer detected.\n");
          }
          return;
        }
      
    // Kiểm tra giá trị tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_WritePin, DBG2: Assert parameter.\n");
      }

        assert_param(IS_GPIO_INSTANCE(GPIOx));
        assert_param(IS_GPIO_PIN(Pin));
        assert_param(IS_PINRETR_ENUM(PinState));

    // Ghi trạng thái chân GPIO theo tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_WritePin, DBG3: Writing pin %u state to %s.\n", Pin, (PinState == GPIO_PIN_SET) ? "SET" : "RESET");
      }

        if (PinState != GPIO_PIN_RESET) {
          GPIOx->GPIO_BSRR = Pin; // Set bit tương ứng trong BSRR để đưa chân lên mức cao

          #ifdef UNIT_TEST
            GPIOx->GPIO_ODR |= Pin;
            /** 
             * Ghi chú:
             * Cập nhật giá trị ODR trong môi trường unit test 
             * để mô phỏng hiệu ứng của BSRR
             */
          #endif
        } else {
          GPIOx->GPIO_BSRR = (ui32)Pin << 16u; // Reset bit tương ứng trong BSRR để đưa chân về mức thấp

          #ifdef UNIT_TEST
            GPIOx->GPIO_ODR &= ~Pin;
            /** 
             * Ghi chú:
             * Cập nhật giá trị ODR trong môi trường unit test 
             * để mô phỏng hiệu ứng của BSRR
             */
          #endif
        }
  }

  /*
   * Hàm đảo trạng thái logic của một chân GPIO cụ thể (SET → RESET, RESET → SET).
   *
   * Tham số:
   *   GPIOx - Con trỏ tới cấu trúc thanh ghi GPIO.
   *   Pin - Bitmask chọn chân GPIO cần đảo.
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ, trả về nếu null.
   *   - Kiểm tra giá trị tham số Pin.
   *   - Đọc giá trị hiện tại từ thanh ghi ODR.
   *   - Tính toán mặt nạ đảo: các bit được set sẽ được reset, bit reset sẽ được set.
   *   - Ghi giá trị vào BSRR để thực hiện đảo (nửa dưới set bit được set, nửa trên reset bit được reset).
   *   - Trong chế độ unit test, cập nhật ODR bằng XOR để mô phỏng đảo.
   *
   * Trả về:
   *   Không có (void).
   *
   * Phụ thuộc ngoài module GPIO:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  void GPIO_TogglePin(GPIO_REGS_Typedef *GPIOx, ui16 Pin) {
    // Kiểm tra con trỏ và giá trị tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_TogglePin, DBG1: Check Null pointer.\n");
      }

        if (GPIOx == NULL) {
          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("GPIO_TogglePin, ERR: Null pointer detected.\n");
          }
          return;
        }
      
    // Kiểm tra giá trị tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_TogglePin, DBG2: Assert parameter.\n");
      }

        assert_param(IS_GPIO_INSTANCE(GPIOx));
        assert_param(IS_GPIO_PIN(Pin));

    // Đảo trạng thái chân GPIO theo tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_TogglePin, DBG3: Toggling pin %u state.\n", Pin);
      }

        ui32 odr = GPIOx->GPIO_ODR;
        ui32 toggle_mask = Pin;

        GPIOx->GPIO_BSRR = ((odr & toggle_mask) << 16u) | (~odr & toggle_mask);

        /**
         * Ghi chú:
         * Set bit tương ứng trong BSRR để đưa chân lên mức cao, 
         * reset bit tương ứng trong BSRR để đưa chân về mức thấp
         */
        
        #ifdef UNIT_TEST
          GPIOx->GPIO_ODR ^= toggle_mask;
          /** 
           * Ghi chú:
           * Cập nhật giá trị ODR trong môi trường unit test 
           * để mô phỏng hiệu ứng của BSRR
           */
        #endif
  }

  /*
   * Hàm khóa cấu hình của các chân GPIO để tránh thay đổi không mong muốn.
   *
   * Tham số:
   *   GPIOx - Con trỏ tới cấu trúc thanh ghi GPIO.
   *   Pin - Bitmask chọn các chân GPIO cần khóa.
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ.
   *   - Kiểm tra giá trị tham số Pin.
   *   - Thực hiện chuỗi khóa bằng cách:
   *       1. Tạo biến tạm với bit LCKK được set cùng các bit Pin.
   *       2. Ghi chuỗi: LCKK|Pin → Pin → LCKK|Pin → đọc lại LCKR.
   *   - Kiểm tra lại bit LCKK để xác nhận khóa thành công.
   *   - Trả về STAT_ERROR nếu khóa thất bại, STAT_DONE nếu thành công.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu khóa thành công, STAT_ERROR nếu khóa thất bại hoặc lỗi kiểm tra.
   *
   * Phụ thuộc ngoài module GPIO:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - SET_BIT() - Macro set bit
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT GPIO_LockPin(GPIO_REGS_Typedef *GPIOx, ui16 Pin) {
    // Kiểm tra con trỏ và giá trị tham số đầu vào
      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_LockPin, DBG1: Check Null pointer.\n");
      }

        if (GPIOx == NULL) {
          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("GPIO_LockPin, ERR: Null pointer detected.\n");
          }
          return STAT_ERROR;
        }
    
    // Kiểm tra giá trị tham số đầu vào

      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_LockPin, DBG2: Assert parameter.\n");
      }

        assert_param(IS_GPIO_INSTANCE(GPIOx));
        assert_param(IS_GPIO_PIN(Pin));

    // Khóa cấu hình chân GPIO theo tham số đầu vào

      if (__DEBUG_GET_MODE(ENABLE)) {
        printf("GPIO_LockPin, DBG3: Locking pin %u configuration.\n", Pin);
      }
  
        BLANK_REG tmp = GPIO_LCKR_LCKK_MASK;
        SET_BIT(tmp, Pin); // Set bit tương ứng trong biến tạm để chuẩn bị cho chuỗi khóa

        GPIOx->GPIO_LCKR = tmp; // Viết giá trị biến tạm vào thanh ghi LCKR để bắt đầu chuỗi khóa
        GPIOx->GPIO_LCKR = Pin; // Viết giá trị chân GPIO vào thanh ghi LCKR để tiếp tục chuỗi khóa
        GPIOx->GPIO_LCKR = tmp; // Viết lại giá trị biến tạm vào thanh ghi LCKR để hoàn tất chuỗi khóa
        (void)GPIOx->GPIO_LCKR; // Đọc lại thanh ghi LCKR để đảm bảo chuỗi khóa đã hoàn tất

        if ((ui32)(GPIOx->GPIO_LCKR & GPIO_LCKR_LCKK_MASK)) {
          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("GPIO_LockPin, DBG3: Pin %u locked successfully.\n", Pin);
          }
        } else {
          if (__DEBUG_GET_MODE(ENABLE)) {
            printf("GPIO_LockPin, ERR: Failed to lock pin %u.\n", Pin);
          }
          return STAT_ERROR;
        }

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

  ui16 GPIO_GetPinIndex(ui16 Pin) {
    // Kiểm tra giá trị tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("GPIO_GetPinIndex, DBG1: Assert parameter.\n");
    }

        assert_param(IS_GPIO_PIN(Pin));

    // Tính toán index chân GPIO từ bitmask Pin
    ui16 index = 0;
    while ((Pin >> index) != 0x0000u) {
      if ((Pin & (0x0001u << index)) != 0) {
        return index; // Trả về index của chân GPIO được chọn
      }
      index++;
    }

    return 0xFFFFu; // Trả về giá trị không hợp lệ nếu không tìm thấy chân nào được chọn
  }
