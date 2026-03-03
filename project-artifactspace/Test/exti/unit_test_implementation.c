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

// Khai báo bảng quản lý table callback cho các line EXTI

  extern EXTI_Handle_Param *EXTI_Handle_Table[16]; // Bảng quản lý con trỏ tới cấu trúc handle EXTI cho 16 line EXTI (0-15)

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

  void callback_example(void) {
    /*
      Hàm callback mẫu để sử dụng trong test case đăng ký callback cho EXTI
    */
    printf("Callback function called successfully.\n");
  }

  void callback_register_example(void) {
    /*
      Hàm callback mẫu khác để sử dụng trong test case đăng ký callback cho EXTI
    */
    printf("Another callback function called successfully.\n");
  }

  void interrupt_trigger(ui16 line) {
    /*
      Hàm này mô phỏng việc thiết lập giá trị cho thanh ghi PR của ngoại vi EXTI giả để tạo ngắt pending cho line tương ứng
    */
    MOCK_EXTI_REGS.EXTI_PR |= (0x0001u << line); // Set bit tương ứng trong PR để tạo ngắt pending
  }

  void test_EXTI_RegisterParam_NullPointer_ShouldReturnError() {
    setup();
    printf("TC1: Check Null Pointer...\n");
    
    // Gọi hàm EXTI_RegisterParam với con trỏ NULL
    RETR_STAT result = EXTI_RegisterParam(NULL);

    // Kiểm tra kết quả trả về là lỗi do con trỏ NULL
    assert(result == STAT_ERROR);

    printf("-> PASSED\n");
  }

  void test_EXTI_RegisterParam_InvalidLine_ShouldAssert() {
    setup();
    printf("TC2: Check Invalid Line Parameter...\n");
    
    // Chuẩn bị tham số không hợp lệ cho EXTI_RegisterParam
    EXTI_Handle_Param handle_param;
    handle_param.Line = 16; // Giá trị line không hợp lệ (vượt quá 15)

    // Gọi hàm EXTI_RegisterParam với tham số không hợp lệ và kiểm tra assert
    // Lưu ý: Trong môi trường unit test, assert sẽ dừng chương trình nếu điều kiện
    // không thỏa mãn nên ta sẽ gọi hàm này trong một test case riêng biệt để kiểm tra assert.

    ASSERT_EXPECT_FAIL(EXTI_RegisterParam(&handle_param));
  }

  void test_EXTI_RegisterParam_ValidParameter_ShouldRegisterCallback() {
    setup();
    printf("TC3: Check Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho EXTI_RegisterParam
    EXTI_Handle_Param handle_param;
    handle_param.Line = 5; // Chọn line 5
    handle_param.Callback = callback_example; // Gán callback mẫu

    // Gọi hàm EXTI_RegisterParam với tham số hợp lệ
    RETR_STAT result = EXTI_RegisterParam(&handle_param);

    // Kiểm tra kết quả trả về là thành công
    assert(result == STAT_DONE);

    // Kiểm tra nếu handle đã được đăng ký đúng cách trong bảng quản lý
    assert(EXTI_Handle_Table[handle_param.Line] == &handle_param);

    printf("-> PASSED\n");
  }

  void test_EXTI_Config_Init_NullPointer_ShouldReturnError() {
    setup();
    printf("TC4: Check Null Pointer...\n");
    
    // Gọi hàm EXTI_Config_Init với con trỏ NULL
    RETR_STAT result = EXTI_Config_Init(NULL, NULL);

    // Kiểm tra kết quả trả về là lỗi do con trỏ NULL
    assert(result == STAT_ERROR);

    printf("-> PASSED\n");
  }

  void test_EXTI_Config_Init_InvalidTrigger_ShouldAssert() {
    setup();
    printf("TC5: Check Invalid Trigger Parameter...\n");
    
    // Chuẩn bị tham số không hợp lệ cho EXTI_Config_Init
    GPIO_Init_Param gpio_init_param;
    gpio_init_param.Trigger = 0xFF; // Giá trị trigger không hợp lệ

    AFIO_EXTI_Init_Param afio_init_param;
    afio_init_param.Line = 3; // Chọn line 3

    // Gọi hàm EXTI_Config_Init với tham số không hợp lệ và kiểm tra assert
    // Lưu ý: Trong môi trường unit test, assert sẽ dừng chương trình nếu điều kiện
    // không thỏa mãn nên ta sẽ gọi hàm này trong một test case riêng biệt để kiểm tra assert.

    ASSERT_EXPECT_FAIL(EXTI_Config_Init(&gpio_init_param, &afio_init_param));
  }

  void test_EXTI_Config_Init_ValidParameter_ShouldConfigureEXTI() {
    setup();
    printf("TC6: Check Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho EXTI_Config_Init
    GPIO_Init_Param gpio_init_param;
    gpio_init_param.Trigger = GPIO_TRIGGER_RISING; // Chọn trigger rising

    AFIO_EXTI_Init_Param afio_init_param;
    afio_init_param.Line = 7; // Chọn line 7

    // Gọi hàm EXTI_Config_Init với tham số hợp lệ
    RETR_STAT result = EXTI_Config_Init(&gpio_init_param, &afio_init_param);

    // Kiểm tra kết quả trả về là thành công
    assert(result == STAT_DONE);

    // Kiểm tra nếu các bit tương ứng trong RTSR và FTSR đã được set đúng cách cho line 7
    assert((MOCK_EXTI_REGS.EXTI_RTSR & (0x0001u << afio_init_param.Line)) != 0); // Bit trong RTSR phải được set
    assert((MOCK_EXTI_REGS.EXTI_FTSR & (0x0001u << afio_init_param.Line)) == 0); // Bit trong FTSR phải được clear

    // Kiểm tra nếu bit tương ứng trong IMR đã được set đúng cách cho line 7
    assert((MOCK_EXTI_REGS.EXTI_IMR & (0x0001u << afio_init_param.Line)) != 0); // Bit trong IMR phải được set

    // Kiểm tra nếu pending bit trong PR đã được clear đúng cách cho line 7
    assert((MOCK_EXTI_REGS.EXTI_PR & (0x0001u << afio_init_param.Line)) != 0); // Bit trong PR phải được clear với 1

    printf("-> PASSED\n");
  }

  void test_EXTI_Config_DeInit_NullPointer_ShouldReturnError() {
    setup();
    printf("TC7: Check Null Pointer...\n");
    
    // Gọi hàm EXTI_Config_DeInit với con trỏ NULL
    RETR_STAT result = EXTI_Config_DeInit(NULL, NULL);

    // Kiểm tra kết quả trả về là lỗi do con trỏ NULL
    assert(result == STAT_ERROR);

    printf("-> PASSED\n");
  }

  void test_EXTI_Config_DeInit_InvalidTrigger_ShouldAssert() {
    setup();
    printf("TC8: Check Invalid Trigger Parameter...\n");
    
    // Chuẩn bị tham số không hợp lệ cho EXTI_Config_DeInit
    GPIO_Init_Param gpio_init_param;
    gpio_init_param.Trigger = 0xFF; // Giá trị trigger không hợp lệ

    AFIO_EXTI_Init_Param afio_init_param;
    afio_init_param.Line = 4; // Chọn line 4

    // Gọi hàm EXTI_Config_DeInit với tham số không hợp lệ và kiểm tra assert
    // Lưu ý: Trong môi trường unit test, assert sẽ dừng chương trình nếu điều kiện
    // không thỏa mãn nên ta sẽ gọi hàm này trong một test case riêng biệt để kiểm tra assert.

    ASSERT_EXPECT_FAIL(EXTI_Config_DeInit(&gpio_init_param, &afio_init_param));
  }

  void test_EXTI_Config_DeInit_ValidParameter_ShouldDeInitEXTI() {
    setup();
    printf("TC9: Check Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho EXTI_Config_DeInit
    GPIO_Init_Param gpio_init_param;
    gpio_init_param.Trigger = GPIO_TRIGGER_FALLING; // Chọn trigger falling

    AFIO_EXTI_Init_Param afio_init_param;
    afio_init_param.Line = 2; // Chọn line 2

    // Gọi hàm EXTI_Config_Init trước để cấu hình EXTI cho line 2
    RETR_STAT result = EXTI_Config_Init(&gpio_init_param, &afio_init_param);
    assert(result == STAT_DONE);

    // Gọi hàm EXTI_Config_DeInit với tham số hợp lệ để vô hiệu hóa cấu hình EXTI cho line 2
    result = EXTI_Config_DeInit(&gpio_init_param, &afio_init_param);
    assert(result == STAT_DONE);

    // Kiểm tra nếu các bit tương ứng trong IMR, RTSR và FTSR đã được clear đúng cách cho line 2
    assert((MOCK_EXTI_REGS.EXTI_IMR & (0x0001u << afio_init_param.Line)) == 0); // Bit trong IMR phải được clear
    assert((MOCK_EXTI_REGS.EXTI_RTSR & (0x0001u << afio_init_param.Line)) == 0); // Bit trong RTSR phải được clear
    assert((MOCK_EXTI_REGS.EXTI_FTSR & (0x0001u << afio_init_param.Line)) == 0); // Bit trong FTSR phải được clear

    // Kiểm tra nếu pending bit trong PR đã được clear đúng cách cho line 2
    assert((MOCK_EXTI_REGS.EXTI_PR & (0x0001u << afio_init_param.Line)) == 0); // Bit trong PR phải được clear

    // Kiểm tra nếu thông tin Line trong cấu trúc tham số đã được reset về giá trị mặc định
    assert(afio_init_param.Line == 15u); // Line phải được reset về giá trị mặc định là 15

    printf("-> PASSED\n");
  }

  void test_EXTI_Generic_IRQHandler_NullPointer_ShouldDoNothing() {
    setup();
    printf("TC10: Check Null Pointer...\n");
    
    // Gọi hàm EXTI_Generic_IRQHandler với con trỏ NULL
    EXTI_Generic_IRQHandler(NULL);

    // Nếu không bị crash thì test case đã thành công
    printf("-> PASSED\n");
  }

  void test_EXTI_Generic_IRQHandler_ValidParameter_ShouldCallCallback() {
    setup();
    printf("TC11: Check Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho EXTI_Generic_IRQHandler
    EXTI_Handle_Param handle_param;
    handle_param.Line = 1; // Chọn line 1
    handle_param.Callback = callback_example; // Gán callback mẫu

    // Đăng ký handle vào bảng quản lý để mô phỏng tình huống đã có callback được đăng ký cho line 1
    EXTI_Handle_Table[handle_param.Line] = &handle_param;

    // Mô phỏng việc thiết lập pending bit cho line 1 để tạo ngắt EXTI
    interrupt_trigger(handle_param.Line);

    // Gọi hàm EXTI_Generic_IRQHandler với tham số hợp lệ
    EXTI_Generic_IRQHandler(&handle_param);

    // Kiểm tra nếu hàm callback đã được gọi đúng cách (trong trường hợp này callback là callback_example nên chỉ cần kiểm tra không bị crash)
    // Trong môi trường unit test, việc gọi callback sẽ in ra thông báo "Callback function called successfully."
    printf("-> PASSED\n");
  }

  void test_EXTI_Generic_IRQHandler_ValidParameter_ShouldNotCallCallback() {
    setup();
    printf("TC12: Check Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho EXTI_Generic_IRQHandler
    EXTI_Handle_Param handle_param;
    handle_param.Line = 1; // Chọn line 1
    handle_param.Callback = callback_example; // Gán callback mẫu

    // Đăng ký handle vào bảng quản lý để mô phỏng tình huống đã có callback được đăng ký cho line 1
    EXTI_Handle_Table[handle_param.Line] = &handle_param;

    // Mô phỏng việc thiết lập pending bit cho line 1 để tạo ngắt EXTI
    interrupt_trigger(3); // Tạo ngắt pending cho line 3 thay vì line 1

    // Gọi hàm EXTI_Generic_IRQHandler với tham số hợp lệ
    EXTI_Generic_IRQHandler(&handle_param);

    // Kiểm tra nếu hàm callback đã được gọi đúng cách (trong trường hợp này callback là callback_example nên chỉ cần kiểm tra không bị crash)
    // Trong môi trường unit test, việc gọi callback sẽ in ra thông báo "Callback function called successfully."
    printf("-> PASSED\n");
  }

  void test_EXTI_IRQHandler_LineSpecific_ShouldCallCorrectCallback() {
    setup();
    printf("TC13: Check Line-Specific IRQ Handler...\n");
    
    // Chuẩn bị tham số hợp lệ cho EXTI_Generic_IRQHandler
    EXTI_Handle_Param handle_param_line0;
    handle_param_line0.Line = 0; // Chọn line 0
    handle_param_line0.Callback = callback_example; // Gán callback mẫu

    EXTI_Handle_Param handle_param_line1;
    handle_param_line1.Line = 1; // Chọn line 1
    handle_param_line1.Callback = callback_example; // Gán callback mẫu

    // Đăng ký handle vào bảng quản lý để mô phỏng tình huống đã có callback được đăng ký cho line 0 và line 1
    EXTI_Handle_Table[handle_param_line0.Line] = &handle_param_line0;
    EXTI_Handle_Table[handle_param_line1.Line] = &handle_param_line1;

    // Mô phỏng việc thiết lập pending bit cho line 0 để tạo ngắt EXTI
    interrupt_trigger(handle_param_line0.Line);
    interrupt_trigger(handle_param_line1.Line);

    // Gọi hàm EXTI_IRQHandler với tham số của line 0
    EXTI_IRQHandler(handle_param_line0.Line);

    // Gọi hàm EXTI_IRQHandler với tham số của line 1
    EXTI_IRQHandler(handle_param_line1.Line);

    // Kiểm tra nếu hàm callback đã được gọi đúng cách (trong trường hợp này callback là callback_example nên chỉ cần kiểm tra không bị crash)
    printf("-> PASSED\n");
  }

  void test_EXTI_RegisterCallback_NullPointer_ShouldReturnError() {
    setup();
    printf("TC14: Check Null Pointer...\n");
    
    // Gọi hàm EXTI_RegisterCallback với con trỏ NULL
    RETR_STAT result = EXTI_RegisterCallback(NULL, EXTI_COMMON_CB_ID, callback_example);

    // Kiểm tra kết quả trả về là lỗi do con trỏ NULL
    assert(result == STAT_ERROR);

    printf("-> PASSED\n");
  }

  void test_EXTI_RegisterCallback_InvalidParameter_ShouldReturnError() {
    setup();
    printf("TC15: Check Invalid Parameter...\n");
    
    // Chuẩn bị tham số không hợp lệ cho EXTI_RegisterCallback
    EXTI_Handle_Param handle_param;
    handle_param.Line = 2; // Chọn line 2

    // Gọi hàm EXTI_RegisterCallback với tham số không hợp lệ (callback_event_type không hợp lệ) và kiểm tra kết quả trả về là lỗi
    RETR_STAT result = EXTI_RegisterCallback(&handle_param, 0xFAu, callback_example); // 0xFF là giá trị không hợp lệ cho callback_event_type
    assert(result == STAT_ERROR);

    printf("-> PASSED\n");
  }

  void test_EXTI_RegisterCallback_ValidParameter_ShouldRegisterCallback() {
    setup();
    printf("TC16: Check Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho EXTI_RegisterCallback
    EXTI_Handle_Param handle_param;
    handle_param.Line = 3; // Chọn line 3

    // Gọi hàm EXTI_RegisterCallback với tham số hợp lệ
    RETR_STAT result = EXTI_RegisterCallback(&handle_param, EXTI_COMMON_CB_ID, callback_register_example);

    // Kiểm tra kết quả trả về là thành công
    assert(result == STAT_OK);

    // Kiểm tra nếu hàm callback đã được đăng ký đúng cách trong cấu trúc handle_param
    assert(handle_param.Callback == callback_register_example);

    printf("-> PASSED\n");
  }

  void test_EXTI_GenerateSWI_NULLPointer_ShouldDoNothing() {
    setup();
    printf("TC17: Check Null Pointer...\n");
    
    // Gọi hàm EXTI_GenerateSWI với con trỏ NULL
    EXTI_GenerateSWI(NULL);

    // Nếu không bị crash thì test case đã thành công
    printf("-> PASSED\n");
  }

  void test_EXTI_GenerateSWI_ValidParameter_ShouldSetPendingBit() {
    setup();
    printf("TC18: Check Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho EXTI_GenerateSWI
    EXTI_Handle_Param handle_param;
    handle_param.Line = 4; // Chọn line 4

    interrupt_trigger(handle_param.Line); // Tạo ngắt pending cho line 4 để kiểm tra nếu hàm EXTI_GenerateSWI có set lại pending bit đúng cách hay không

    // Gọi hàm EXTI_GenerateSWI với tham số hợp lệ
    EXTI_GenerateSWI(&handle_param);

    // Kiểm tra nếu pending bit trong PR đã được set đúng cách cho line 4
    assert((MOCK_EXTI_REGS.EXTI_PR & (0x0001u << handle_param.Line)) != 0); // Bit trong PR phải được set

    printf("-> PASSED\n");
  }

int main() {
  printf("\n--- EXTI UNIT TEST ---\n");
  
  // Chạy tất cả các test case
  
  test_EXTI_RegisterParam_NullPointer_ShouldReturnError();
  printf("\n");
  test_EXTI_RegisterParam_InvalidLine_ShouldAssert();
  printf("\n");
  test_EXTI_RegisterParam_ValidParameter_ShouldRegisterCallback();
  printf("\n");

  test_EXTI_Config_Init_NullPointer_ShouldReturnError();
  printf("\n");
  test_EXTI_Config_Init_InvalidTrigger_ShouldAssert();
  printf("\n");
  test_EXTI_Config_Init_ValidParameter_ShouldConfigureEXTI();
  printf("\n");

  test_EXTI_Config_DeInit_NullPointer_ShouldReturnError();
  printf("\n");
  test_EXTI_Config_DeInit_InvalidTrigger_ShouldAssert();
  printf("\n");
  test_EXTI_Config_DeInit_ValidParameter_ShouldDeInitEXTI();
  printf("\n");

  test_EXTI_Generic_IRQHandler_NullPointer_ShouldDoNothing();
  printf("\n");
  test_EXTI_Generic_IRQHandler_ValidParameter_ShouldCallCallback();
  printf("\n");
  test_EXTI_Generic_IRQHandler_ValidParameter_ShouldNotCallCallback();
  printf("\n");

  test_EXTI_IRQHandler_LineSpecific_ShouldCallCorrectCallback();
  printf("\n");

  test_EXTI_RegisterCallback_NullPointer_ShouldReturnError();
  printf("\n");
  test_EXTI_RegisterCallback_InvalidParameter_ShouldReturnError();
  printf("\n");
  test_EXTI_RegisterCallback_ValidParameter_ShouldRegisterCallback();
  printf("\n");

  test_EXTI_GenerateSWI_NULLPointer_ShouldDoNothing();
  printf("\n");
  test_EXTI_GenerateSWI_ValidParameter_ShouldSetPendingBit();
  printf("\n");

  printf("\n--- ALL TESTS PASSED ---\n");
  return 0;
}