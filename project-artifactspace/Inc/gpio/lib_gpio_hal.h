/*
 * lib_gpio_hal.h
 *
 *  Created on: Feb 10, 2026
 *      Author: shanghuang
 */

#ifndef LIB_GPIO_HAL_H_
  #define LIB_GPIO_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "gpio/lib_gpio_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_gpio_def.h"
    #endif

    #include <stdint.h>

  // Khai báo cấu trúc tham số hàm khởi tạo

    #ifndef GPIO_INIT_PARAM_TYPE
      #define GPIO_INIT_PARAM_TYPE
        tdf_strc GPIO_Init_Param { // Cấu trúc tham số để khởi tạo chân GPIO
          ui16 Pin;       // Chọn chân GPIO cần cấu hình
          ui8 Mode;       // Chọn chế độ hoạt động cho chân GPIO
          ui8 Pull;       // 1: Kích hoạt pull-up, 0: Kích hoạt pull-down (chỉ áp dụng cho chế độ input)
          ui8 Trigger;    // Chọn kiểu trigger ngắt ngoài (rising/falling/both) nếu chân GPIO được cấu hình cho EXTI, ngược lại sẽ bỏ qua tham số này
        } GPIO_Init_Param;
    #endif

  // Khai báo kiểu dữ liệu trả về

    #ifndef PIN_RETR
      #define PIN_RETR
        tdf_enum PIN_RETR_Enum {
          GPIO_PIN_RESET = 0u,
          GPIO_PIN_SET = 1u,
          GPIO_PIN_UNF = 0xFFu
        } PIN_RETR_Enum;
    #endif

  // Khai báo các kiểm tra tham số đầu vào nội bộ

    #define IS_GPIO_INSTANCE(GPIOx) (((GPIOx) == GPIOA_REGS_PTR) || \
                                  ((GPIOx) == GPIOB_REGS_PTR) || \
                                  ((GPIOx) == GPIOC_REGS_PTR) || \
                                  ((GPIOx) == GPIOD_REGS_PTR) || \
                                  ((GPIOx) == GPIOE_REGS_PTR) || \
                                  ((GPIOx) == GPIOF_REGS_PTR) || \
                                  ((GPIOx) == GPIOG_REGS_PTR))

    #define IS_GPIO_PIN(PIN) ((((PIN) & (~GPIO_PIN_ALL)) == 0x00u) && ((PIN) != 0x00u))
    
    #define IS_GPIO_CONFIG(INPUT) (((INPUT) == GPIO_CNF_INPUT_ANALOG)    || \
                                  ((INPUT) == GPIO_CNF_INPUT_FLOATING)   || \
                                  ((INPUT) == GPIO_CNF_INPUT_PU_PD)      || \
                                  ((INPUT) == GPIO_CNF_OUTPUT_PP)        || \
                                  ((INPUT) == GPIO_CNF_OUTPUT_OD)        || \
                                  ((INPUT) == AFIO_OUTPUT_PP)            || \
                                  ((INPUT) == AFIO_OUTPUT_OD))

    #define IS_GPIO_INPUT_MODE(MODE) (((MODE) == GPIO_MODE_INPUT_ANALOG)     || \
                              ((MODE) == GPIO_MODE_INPUT_FLOATING)          || \
                              ((MODE) == GPIO_MODE_INPUT_PU_PD))
                                    
    #define IS_GPIO_AFIO_MODE(MODE) (((MODE) == GPIO_MODE_INPUT_ANALOG)     || \
                              ((MODE) == GPIO_MODE_INPUT_FLOATING)          || \
                              ((MODE) == GPIO_MODE_INPUT_PU_PD)             || \
                              ((MODE) == GPIO_MODE_OUTPUT_10MHz_PP)         || \
                              ((MODE) == GPIO_MODE_OUTPUT_10MHz_OD)         || \
                              ((MODE) == GPIO_MODE_OUTPUT_2MHz_PP)          || \
                              ((MODE) == GPIO_MODE_OUTPUT_2MHz_OD)          || \
                              ((MODE) == GPIO_MODE_OUTPUT_50MHz_PP)         || \
                              ((MODE) == GPIO_MODE_OUTPUT_50MHz_OD)         || \
                              ((MODE) == AFIO_MODE_OUTPUT_10MHz_PP)         || \
                              ((MODE) == AFIO_MODE_OUTPUT_2MHz_PP)          || \
                              ((MODE) == AFIO_MODE_OUTPUT_50MHz_PP)         || \
                              ((MODE) == AFIO_MODE_OUTPUT_10MHz_OD)         || \
                              ((MODE) == AFIO_MODE_OUTPUT_2MHz_OD)          || \
                              ((MODE) == AFIO_MODE_OUTPUT_50MHz_OD))
    
    #define IS_AFIO_GPIO_SUPPORT(GPIOx) (((GPIOx) == GPIOA_REGS_PTR) || \
                                  ((GPIOx) == GPIOB_REGS_PTR)        || \
                                  ((GPIOx) == GPIOC_REGS_PTR)        || \
                                  ((GPIOx) == GPIOD_REGS_PTR)        || \
                                  ((GPIOx) == GPIOE_REGS_PTR)        || \
                                  ((GPIOx) == GPIOF_REGS_PTR))
    
    #define IS_GPIO_PULL(PULL) (((PULL) == GPIO_NOPULL) || \
                                ((PULL) == GPIO_PULLUP) || \
                                ((PULL) == GPIO_PULLDOWN))

    #define IS_GPIO_TRIGGER(TRIGGER) (((TRIGGER) == GPIO_TRIGGER_NONE)   || \
                                    ((TRIGGER) == GPIO_TRIGGER_RISING) || \
                                    ((TRIGGER) == GPIO_TRIGGER_FALLING)|| \
                                    ((TRIGGER) == GPIO_TRIGGER_BOTH))

    #define IS_PINRETR_ENUM(STATE) (((STATE) == GPIO_PIN_RESET) || \
                                  ((STATE) == GPIO_PIN_SET)   || \
                                  ((STATE) == GPIO_PIN_UNF))

  // Khai báo các hàm thành phần

    // >> Hàm khởi tạo GPIO
    RETR_STAT GPIO_Init(GPIO_REGS_Typedef *GPIOx, GPIO_Init_Param *init_param);

    // >> Hàm vô hiệu hóa GPIO
    RETR_STAT GPIO_DeInit(GPIO_REGS_Typedef *GPIOx, ui16 Pin);

    // >> Hàm xử lý ngắt ngoại vi GPIO
    void GPIO_EXTI_IRQHandler(ui16 Pin);

    // >> Hàm callback ngắt ngoại vi GPIO weak
    __weak void GPIO_EXTI_Callback(ui16 Pin);

    // >> Hàm đọc trạng thái chân GPIO
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
    stinl PIN_RETR_Enum GPIO_ReadPin(GPIO_REGS_Typedef *GPIOx, ui16 Pin) {

      // Kiểm tra con trỏ và giá trị tham số đầu vào

        if (GPIOx == NULL) {
          return GPIO_PIN_UNF;
        }

      // Kiểm tra giá trị tham số đầu vào

        assert_param(IS_GPIO_INSTANCE(GPIOx));
        assert_param(IS_GPIO_PIN(Pin));

      // Đọc trạng thái chân GPIO theo tham số đầu vào

        if ((GPIOx->GPIO_IDR & Pin) != (ui32)0x00000000u) {
          return GPIO_PIN_SET;
        } else {
          return GPIO_PIN_RESET;
        }
    }

    // >> Hàm ghi trạng thái chân GPIO
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
    stinl void GPIO_WritePin(
      GPIO_REGS_Typedef *GPIOx,
      ui16 Pin,
      PIN_RETR_Enum PinState
    ) {
      // Kiểm tra con trỏ và giá trị tham số đầu vào

        if (GPIOx == NULL) {
          return;
        }

      // Kiểm tra giá trị tham số đầu vào

        assert_param(IS_GPIO_INSTANCE(GPIOx));
        assert_param(IS_GPIO_PIN(Pin));
        assert_param(IS_PINRETR_ENUM(PinState));

      // Ghi trạng thái chân GPIO theo tham số đầu vào

        if (PinState == GPIO_PIN_SET) {
          GPIOx->GPIO_BSRR = Pin; // Set bit tương ứng trong BSRR để đưa chân lên mức cao

          #ifdef UNIT_TEST
            GPIOx->GPIO_ODR |= Pin;
            /**
             * Ghi chú:
             * Cập nhật giá trị ODR trong môi trường unit test
             * để mô phỏng hiệu ứng của BSRR
             */
          #endif
        } else if (PinState == GPIO_PIN_RESET) {
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

      // Kết thúc ghi trạng thái, trả về

        return;
    }

    // >> Hàm đảo trạng thái chân GPIO
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
    stinl void GPIO_TogglePin(GPIO_REGS_Typedef *GPIOx, ui16 Pin) {

      // Kiểm tra con trỏ và giá trị tham số đầu vào

        if (GPIOx == NULL) {
          return;
        }

      // Kiểm tra giá trị tham số đầu vào

        assert_param(IS_GPIO_INSTANCE(GPIOx));
        assert_param(IS_GPIO_PIN(Pin));

      // Đảo trạng thái chân GPIO theo tham số đầu vào

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

    // >> Hàm khóa cấu hình chân GPIO
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
    stinl RETR_STAT GPIO_LockPin(GPIO_REGS_Typedef *GPIOx, ui16 Pin) {

      // Kiểm tra con trỏ và giá trị tham số đầu vào

        if (GPIOx == NULL) {
          return STAT_ERROR;
        }

      // Kiểm tra giá trị tham số đầu vào

        assert_param(IS_GPIO_INSTANCE(GPIOx));
        assert_param(IS_GPIO_PIN(Pin));

      // Khóa cấu hình chân GPIO theo tham số đầu vào

        BLANK_REG tmp = GPIO_LCKR_LCKK_MASK;
        SET_BIT(tmp, Pin); // Set bit tương ứng trong biến tạm để chuẩn bị cho chuỗi khóa

        GPIOx->GPIO_LCKR = tmp; // Viết giá trị biến tạm vào thanh ghi LCKR để bắt đầu chuỗi khóa
        GPIOx->GPIO_LCKR = Pin; // Viết giá trị chân GPIO vào thanh ghi LCKR để tiếp tục chuỗi khóa
        GPIOx->GPIO_LCKR = tmp; // Viết lại giá trị biến tạm vào thanh ghi LCKR để hoàn tất chuỗi khóa
        (void)GPIOx->GPIO_LCKR; // Đọc lại thanh ghi LCKR để đảm bảo chuỗi khóa đã hoàn tất

        if ((ui32)(GPIOx->GPIO_LCKR & GPIO_LCKR_LCKK_MASK)) {

        } else {
          return STAT_ERROR;
        }

      // Kết thúc khóa cấu hình, trả về trạng thái thành công

        return STAT_DONE;
    }

    
    // >> Hàm trả về index chân GPIO từ bitmask Pin
    stinl ui16 GPIO_GetPinIndex(ui16 Pin) {

      // Kiểm tra giá trị tham số đầu vào

        assert_param(IS_GPIO_PIN(Pin));

      // Tính toán index chân GPIO từ bitmask Pin

        ui16 index = 0;
        while ((Pin >> index) != 0x0000u) {
          if ((Pin & (0x0001u << index)) != 0) {
            return index; // Trả về index của chân GPIO được chọn
          }
          index++;
        }

      // Nếu không tìm thấy chân nào được chọn, trả về giá trị không hợp lệ

        return 0xFFFFu; // Trả về giá trị không hợp lệ nếu không tìm thấy chân nào được chọn
    }

#endif /* LIB_GPIO_HAL_H_ */
