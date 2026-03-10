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
    #include "gpio/lib_gpio_def.h"
    #include "gpio/lib_gpio_hal.h"
  #endif

// Khai báo define

  #ifdef UNIT_TEST
    #define sta 
  #else
    // Không khai báo sta trong code thực tế vì nó đã được định nghĩa trong lib_keyword_def.h
  #endif

// Khai báo bảng quản lý table callback cho các line EXTI

  sta EXTI_Handle_Param *EXTI_Handle_Table[16] = {0}; // Bảng quản lý con trỏ tới cấu trúc handle EXTI cho 16 line EXTI (0-15)

// Định nghĩa các hàm thành phần

  /*
   * Hàm khởi tạo và cấu hình line EXTI theo tham số đầu vào.
   *
   * Tham số:
   *   gpio_init_param - Con trỏ tới cấu trúc tham số GPIO (chứa thông tin trigger).
   *   afio_init_param - Con trỏ tới cấu trúc tham số AFIO EXTI (Port, Pin, Line).
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ.
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

    // Kiểm tra tham số đầu vào hợp lệ

      if (gpio_init_param == NULL || afio_init_param == NULL) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số trigger hợp lệ

      assert_param(IS_GPIO_TRIGGER(gpio_init_param->Trigger));

    /**
     * Ghi chú:
    * Do AFIO_EXTI_Init_Param đã được kiểm tra kỹ lưỡng trong
     * hàm AFIO_EXTI_Line_Init nên ta sẽ không kiểm tra lại ở đây nữa.
     */

    // Cấu hình line EXTI dựa vào thông tin trigger trong GPIO_Init_Param và thông tin line đã khởi tạo trong AFIO_EXTI_Init_Param

      // >> Cấu hình cạnh lên / xuống cho line EXTI dựa vào thông tin trigger trong GPIO_Init_Param
        
        switch (gpio_init_param->Trigger) {
          
          case GPIO_TRIGGER_NONE:
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

      // >> Kích hoạt line EXTI bằng cách set bit tương ứng trong IMR

        EXTI_REGS_PTR->EXTI_IMR |= (0x0001u << afio_init_param->Line); // Set bit tương ứng trong IMR để kích hoạt line EXTI
        EXTI_REGS_PTR->EXTI_PR |= (0x0001u << afio_init_param->Line); // Set bit tương ứng trong PR để clear pending bit cũ nếu có, lưu ý rằng thanh ghi PR là rw_c1 

    // Kết thúc quy trình

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

    // Kiểm tra tham số đầu vào hợp lệ

      if (gpio_init_param == NULL || afio_init_param == NULL) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số trigger hợp lệ

      assert_param(IS_GPIO_TRIGGER(gpio_init_param->Trigger));

    // Vô hiệu hóa line EXTI và xóa cấu hình trigger cạnh lên / xuống

      // >> Vô hiệu hóa line EXTI bằng cách xóa bit tương ứng trong IMR
        
        EXTI_REGS_PTR->EXTI_IMR &= ~(0x0001u << afio_init_param->Line);

      // >> Xóa cấu hình trigger cạnh lên / xuống cho line EXTI bằng cách xóa bit tương ứng trong RTSR và FTSR
      
        EXTI_REGS_PTR->EXTI_RTSR &= ~(0x0001u << afio_init_param->Line);
        EXTI_REGS_PTR->EXTI_FTSR &= ~(0x0001u << afio_init_param->Line);

      // >> Clear pending bit của line EXTI để đảm bảo không có ngắt nào bị bỏ sót ngay sau khi vô hiệu hóa
      
        EXTI_REGS_PTR->EXTI_PR |= (0x0001u << afio_init_param->Line);

      // >> Reset thông tin Line về giá trị mặc định (15u) để tránh nhầm lẫn trong các thao tác tiếp theo
      
        afio_init_param->Line = 15u; // Reset thông tin Line trong cấu trúc tham số về giá trị mặc định

      // Kết thúc quy trình

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
   *   - Nếu không có pending bit, kết thúc hàm mà không xử lý thêm.
   *
   * Trả về:
   *   Không có (void).
   *
   * Phụ thuộc ngoài module EXTI:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - EXTI_RegisterCallback() - Hàm đăng ký callback
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  void EXTI_Generic_IRQHandler(EXTI_Handle_Param *handle_param) {

    // Kiểm tra con trỏ handle_param hợp lệ

      if (handle_param == NULL) {
        return;
      }

    // Kiểm tra giá trị tham số Line hợp lệ

      assert_param(handle_param->Line < 16); // Kiểm tra thông tin line EXTI hợp lệ (0-15)

    // Kiểm tra nếu pending bit của line EXTI được set thì gọi hàm callback tương ứng
    
      if ((EXTI_REGS_PTR->EXTI_PR & (0x0001u << handle_param->Line)) != 0) {
        EXTI_REGS_PTR->EXTI_PR |= (0x0001u << handle_param->Line); // Clear pending bit sau khi đã xử lý ngắt
        if (handle_param->Callback != NULL) {
          handle_param->Callback(); // Gọi hàm callback đã đăng ký để xử lý ngắt
        }
      } else {
        return; // Nếu không có pending bit nào được set thì không làm gì cả, có thể in thông báo debug nếu cần
      }
  }

  /*
   * Hàm xử lý ngắt EXTI dành cho một line cụ thể.
   *
   * Tham số:
   *   Pin - Số line EXTI cần xử lý (0-15).
   *
   * Logic:
   *   - Tìm kiếm trong bảng quản lý EXTI_Handle_Table[] với index = Pin.
   *   - Kiểm tra nếu handle tương ứng với line đó đã được đăng ký.
   *   - Nếu có handle, gọi hàm EXTI_Generic_IRQHandler() với handle đó.
   *   - Cho phép chia sẻ cùng một IRQ vector cho nhiều line EXTI
   *     (ví dụ: EXTI9_5_IRQHandler, EXTI15_10_IRQHandler).
   *
   * Trả về:
   *   Không có (void).
   *
   * Phụ thuộc ngoài module EXTI:
   *   - EXTI_Handle_Table[] - Bảng quản lý handle đăng ký cho mỗi line
   *   - EXTI_Generic_IRQHandler() - Hàm xử lý ngắt chung
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  void EXTI_IRQHandler(ui16 Pin) {

    // Kiểm tra giá trị line đầu vào hợp lệ

      if (Pin >= 16u) {
        return;
      }

    // Tìm kiếm trong bảng quản lý để xác định line EXTI tương ứng với chân GPIO đã chọn
      for (ui16 line = 0; line < 16; line++) {
        if (
          EXTI_Handle_Table[line] != NULL // Kiểm tra nếu có handle đã đăng ký cho line này
          && 
          EXTI_Handle_Table[line]->Line == Pin // Kiểm tra nếu line trong handle trùng với line đang xử lý
        ) {
          EXTI_Generic_IRQHandler(EXTI_Handle_Table[line]); // Gọi hàm xử lý ngắt chung với handle_param tương ứng
          break; // Dừng vòng lặp sau khi đã tìm thấy và xử lý ngắt cho line tương ứng
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
   *   - EXTI_Generic_IRQHandler() - Hàm xử lý ngắt gọi callback này
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT EXTI_RegisterCallback(
    EXTI_Handle_Param *handle_param,
    EXTI_Callback_Event_Param_Type callback_event_type,
    void (*callback_func)(void)
  ) {

    // Lưu trạng thái trả về, mặc định là STAT_OK và sẽ được cập nhật nếu có lỗi xảy ra
    
      RETR_STAT status = STAT_OK;
    
    // Kiểm tra con trỏ handle_param hợp lệ và con trỏ callback_func không NULL

      if (handle_param == NULL) {
        return STAT_ERROR;
      }

      if (callback_func == NULL) {
        return STAT_ERROR;
      }

    // Đăng ký hàm callback vào trường Callback của handle_param dựa vào loại sự kiện callback_event_type

      switch (callback_event_type) {
        case EXTI_COMMON_CB_ID:
          handle_param->Callback = callback_func; // Đăng ký hàm callback vào trường Callback của handle_param
          break;
        
        default:
          status = STAT_ERROR; // Nếu callback_event_type không hợp lệ thì trả về lỗi
          break;
      }
    
    // Kết thúc quy trình

      return status;
  }

  /*
   * Hàm đăng ký cấu trúc handle EXTI vào bảng quản lý các line EXTI.
   *
   * Tham số:
   *   handle_param - Con trỏ tới cấu trúc handle EXTI (chứa Line và Callback).
   *
   * Logic:
   *   - Kiểm tra con trỏ handle_param hợp lệ.
   *   - Kiểm tra giá trị tham số Line (0-15).
   *   - Lưu con trỏ handle_param vào bảng quản lý để sử dụng khi
   *     EXTI_Generic_IRQHandler() được gọi từ EXTI_IRQHandler().
   *   - Cho phép tìm kiếm nhanh handle tương ứng với mỗi line EXTI.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu đăng ký thành công, STAT_ERROR nếu có lỗi.
   *
   * Phụ thuộc ngoài module EXTI:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT EXTI_RegisterParam(EXTI_Handle_Param *handle_param) {

    // Kiểm tra con trỏ handle_param hợp lệ

      if (handle_param == NULL) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số Line hợp lệ (0-15)

      assert_param(handle_param->Line < 16); // Kiểm tra thông tin line EXTI hợp lệ (0-15)

    // Đăng ký con trỏ tới cấu trúc tham số vào bảng quản lý để sử dụng khi EXTI_Generic_IRQHandler() được gọi từ EXTI_IRQHandler()

      EXTI_Handle_Table[handle_param->Line] = handle_param; // Đăng ký con trỏ tới cấu trúc tham số vào bảng quản lý

    // Kết thúc quy trình

      return STAT_DONE;
  }

// Định nghĩa các hàm xử lý IRQ nhóm weak

  /*
   * Hàm xử lý ngắt EXTI line 0 (weak, có thể override trong user code).
   *
   * Logic:
   *   - Gọi hàm EXTI_IRQHandler() với line = 0.
   *   - Hàm này sẽ tìm kiếm trong bảng handle và gọi callback tương ứng.
   *   - User có thể override hàm này để implement xử lý tùy chỉnh.
   *
   * Kiểu trả về:
   *   Không có (void).
   */
  __weak void EXTI0_IRQHandler(void) {
    EXTI_IRQHandler((ui16)0); // Gọi hàm xử lý ngắt chung với line 0
  }

  /*
   * Hàm xử lý ngắt EXTI line 1 (weak, có thể override trong user code).
   *
   * Logic:
   *   - Gọi hàm EXTI_IRQHandler() với line = 1.
   *   - Hàm này sẽ tìm kiếm trong bảng handle và gọi callback tương ứng.
   *   - User có thể override hàm này để implement xử lý tùy chỉnh.
   *
   * Kiểu trả về:
   *   Không có (void).
   */
  __weak void EXTI1_IRQHandler(void) {
    EXTI_IRQHandler((ui16)1); // Gọi hàm xử lý ngắt chung với line 1
  }

  /*
   * Hàm xử lý ngắt EXTI line 2 (weak, có thể override trong user code).
   *
   * Logic:
   *   - Gọi hàm EXTI_IRQHandler() với line = 2.
   *   - Hàm này sẽ tìm kiếm trong bảng handle và gọi callback tương ứng.
   *   - User có thể override hàm này để implement xử lý tùy chỉnh.
   *
   * Kiểu trả về:
   *   Không có (void).
   */
  __weak void EXTI2_IRQHandler(void) {
    EXTI_IRQHandler((ui16)2); // Gọi hàm xử lý ngắt chung với line 2
  }

  /*
   * Hàm xử lý ngắt EXTI line 3 (weak, có thể override trong user code).
   *
   * Logic:
   *   - Gọi hàm EXTI_IRQHandler() với line = 3.
   *   - Hàm này sẽ tìm kiếm trong bảng handle và gọi callback tương ứng.
   *   - User có thể override hàm này để implement xử lý tùy chỉnh.
   *
   * Kiểu trả về:
   *   Không có (void).
   */
  __weak void EXTI3_IRQHandler(void) {
    EXTI_IRQHandler((ui16)3); // Gọi hàm xử lý ngắt chung với line 3
  }

  /*
   * Hàm xử lý ngắt EXTI line 4 (weak, có thể override trong user code).
   *
   * Logic:
   *   - Gọi hàm EXTI_IRQHandler() với line = 4.
   *   - Hàm này sẽ tìm kiếm trong bảng handle và gọi callback tương ứng.
   *   - User có thể override hàm này để implement xử lý tùy chỉnh.
   *
   * Kiểu trả về:
   *   Không có (void).
   */
  __weak void EXTI4_IRQHandler(void) {
    EXTI_IRQHandler((ui16)4); // Gọi hàm xử lý ngắt chung với line 4
  }

  /*
   * Hàm xử lý ngắt EXTI lines 5-9 (weak, có thể override trong user code).
   *
   * Logic:
   *   - Do EXTI lines 5-9 chia sẻ chung một IRQ vector, ta sẽ duyệt qua từng line.
   *   - Kiểm tra pending bit của mỗi line trong các line 5-9.
   *   - Nếu có pending bit, gọi EXTI_IRQHandler() với line tương ứng.
   *   - Cho phép xử lý đồng thời 5 line EXTI cụ thể trong cùng một IRQ.
   *   - User có thể override hàm này để implement xử lý tùy chỉnh.
   *
   * Kiểu trả về:
   *   Không có (void).
   */
  __weak void EXTI9_5_IRQHandler(void) {
    // Do EXTI lines 5-9 chia sẻ chung một IRQ, ta sẽ gọi hàm xử lý chung với line tương ứng
    for (ui16 line = 5; line <= 9; line++) {
      if (EXTI_REGS_PTR->EXTI_PR & (0x0001u << line)) { // Kiểm tra nếu pending bit của line EXTI được set
        EXTI_IRQHandler(line); // Gọi hàm xử lý ngắt chung với line tương ứng
      }
    }
  }

  /*
   * Hàm xử lý ngắt EXTI lines 10-15 (weak, có thể override trong user code).
   *
   * Logic:
   *   - Do EXTI lines 10-15 chia sẻ chung một IRQ vector, ta sẽ duyệt qua từng line.
   *   - Kiểm tra pending bit của mỗi line trong các line 10-15.
   *   - Nếu có pending bit, gọi EXTI_IRQHandler() với line tương ứng.
   *   - Cho phép xử lý đồng thời 6 line EXTI cụ thể trong cùng một IRQ.
   *   - User có thể override hàm này để implement xử lý tùy chỉnh.
   *
   * Kiểu trả về:
   *   Không có (void).
   */
  __weak void EXTI15_10_IRQHandler(void) {
    // Do EXTI lines 10-15 chia sẻ chung một IRQ, ta sẽ gọi hàm xử lý chung với line tương ứng
    for (ui16 line = 10; line <= 15; line++) {
      if (EXTI_REGS_PTR->EXTI_PR & (0x0001u << line)) { // Kiểm tra nếu pending bit của line EXTI được set
        EXTI_IRQHandler(line); // Gọi hàm xử lý ngắt chung với line tương ứng
      }
    }
  }
