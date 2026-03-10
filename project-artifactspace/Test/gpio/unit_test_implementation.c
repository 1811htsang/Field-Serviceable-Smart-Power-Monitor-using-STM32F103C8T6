/*
 * unit_test_implementation.c
 *
 *  Created on: Feb 26, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include <stdio.h>
  #include <assert.h>
	#include <string.h>
  #include <setjmp.h>
  #include "lib_keyword_def.h"
  #include "lib_condition_def.h"
  #include "lib_gpio_def.h"
  #include "lib_gpio_hal.h"
  #include "lib_test_util.h"
  #include "header_dependency.h"

// Khai báo ngoại vi giả cho mục đích unit test

  GPIO_REGS_Typedef MOCK_GPIOA_REGS;
  GPIO_REGS_Typedef MOCK_GPIOB_REGS;
  GPIO_REGS_Typedef MOCK_GPIOC_REGS;
  GPIO_REGS_Typedef MOCK_GPIOD_REGS;
  GPIO_REGS_Typedef MOCK_GPIOE_REGS;
  GPIO_REGS_Typedef MOCK_GPIOF_REGS;
  GPIO_REGS_Typedef MOCK_GPIOG_REGS;

// Gọi ủy quyền các biến và hàm mock từ source_dependency.c

  /**
   * Ghi chú:
   * Module gpio không có hàm nào phụ thuộc ngoài module nên không cần ủy quyền biến hay hàm mock.
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

    // Reset các ngoại vi GPIO về các giá trị reset
    memset(&MOCK_GPIOA_REGS, 0, sizeof(GPIO_REGS_Typedef));
    memset(&MOCK_GPIOB_REGS, 0, sizeof(GPIO_REGS_Typedef));
    memset(&MOCK_GPIOC_REGS, 0, sizeof(GPIO_REGS_Typedef));
    memset(&MOCK_GPIOD_REGS, 0, sizeof(GPIO_REGS_Typedef));
    memset(&MOCK_GPIOE_REGS, 0, sizeof(GPIO_REGS_Typedef));
    memset(&MOCK_GPIOF_REGS, 0, sizeof(GPIO_REGS_Typedef));
    memset(&MOCK_GPIOG_REGS, 0, sizeof(GPIO_REGS_Typedef));

    // Thiết lập lại giá trị reset cho các thanh ghi của ngoại vi GPIO giả
    MOCK_GPIOA_REGS.GPIO_CRL = 0x44444444;
    MOCK_GPIOA_REGS.GPIO_CRH = 0x44444444;
    MOCK_GPIOB_REGS.GPIO_CRL = 0x44444444;
    MOCK_GPIOB_REGS.GPIO_CRH = 0x44444444;
    MOCK_GPIOC_REGS.GPIO_CRL = 0x44444444;
    MOCK_GPIOC_REGS.GPIO_CRH = 0x44444444;
    MOCK_GPIOD_REGS.GPIO_CRL = 0x44444444;
    MOCK_GPIOD_REGS.GPIO_CRH = 0x44444444;
    MOCK_GPIOE_REGS.GPIO_CRL = 0x44444444;
    MOCK_GPIOE_REGS.GPIO_CRH = 0x44444444;
    MOCK_GPIOF_REGS.GPIO_CRL = 0x44444444;
    MOCK_GPIOF_REGS.GPIO_CRH = 0x44444444;
    MOCK_GPIOG_REGS.GPIO_CRL = 0x44444444;
    MOCK_GPIOG_REGS.GPIO_CRH = 0x44444444;
  }

  void GPIOx_IDR_data_set(GPIO_REGS_Typedef *GPIOx, ui16 data) {
    /*
      Hàm này mô phỏng việc thiết lập giá trị cho thanh ghi IDR của ngoại vi GPIO giả
    */
    GPIOx->GPIO_IDR = data;
  }

  void test_GPIO_Init_NullPointer_ShouldReturnError(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC1: Check Null Pointer...\n");
    
    // Gọi hàm GPIO_Init với con trỏ NULL
    RETR_STAT result = GPIO_Init(NULL, NULL);

    // Kiểm tra kết quả trả về là lỗi do con trỏ NULL
    assert(result == STAT_ERROR);

    printf("-> PASSED\n");
  }

  void test_GPIO_Init_InvalidParameter_ShouldAssert(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC2: Check Invalid Parameter...\n");
    
    // Chuẩn bị tham số không hợp lệ cho GPIO_Init
    GPIO_Init_Param init_param;
    init_param.Pin = 0x0000; // Không chọn chân nào
    init_param.Mode = 0xFF; // Chế độ không hợp lệ
    init_param.Pull = 0xFF; // Giá trị pull không hợp lệ

    // Gọi hàm GPIO_Init với tham số không hợp lệ và kiểm tra assert
    // Lưu ý: Trong môi trường unit test, assert sẽ dừng chương trình nếu điều kiện
    // không thỏa mãn, nên ta sẽ gọi hàm này trong một test case riêng biệt để kiểm tra assert.
    ASSERT_EXPECT_FAIL(GPIO_Init(GPIOx, &init_param));
  }

  void test_GPIO_Init_ValidParameter_ShouldConfigureGPIO(GPIO_REGS_Typedef *GPIOx, u Mode) {
    setup();
    printf("TC3: Check Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho GPIO_Init
    GPIO_Init_Param init_param;
    init_param.Pin = GPIO_PIN_15; // Chọn chân 0
    init_param.Mode = Mode; // Chế độ output push-pull 10MHz
    init_param.Pull = GPIO_NOPULL; // Không kéo lên hay xuống

    // Gọi hàm GPIO_Init với tham số hợp lệ
    RETR_STAT result = GPIO_Init(GPIOx, &init_param);

    // Kiểm tra kết quả trả về là thành công
    assert(result == STAT_DONE);

    printf("-> PASSED\n");
  }

  void test_GPIO_Init_MultiplePins_ShouldConfigureAllSelectedPins(GPIO_REGS_Typedef *GPIOx, u Mode) {
    setup();
    printf("TC3.1: Check Multiple Pins...\n");
    
    // Chuẩn bị tham số hợp lệ cho GPIO_Init với nhiều chân được chọn
    GPIO_Init_Param init_param;
    init_param.Pin = GPIO_PIN_10 | GPIO_PIN_6 | GPIO_PIN_2; // Chọn chân 10, 6 và 2
    init_param.Mode = Mode; // Chế độ Input pull-up
    init_param.Pull = GPIO_PULLUP; // Kéo lên

    // Gọi hàm GPIO_Init với tham số hợp lệ
    RETR_STAT result = GPIO_Init(GPIOx, &init_param);

    // Kiểm tra kết quả trả về là thành công
    assert(result == STAT_DONE);

    printf("-> PASSED\n");
  }

  void test_GPIO_DeInit_NullPointer_ShouldReturnError(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC4: Check Null Pointer...\n");
    
    // Gọi hàm GPIO_DeInit với con trỏ NULL
    RETR_STAT result = GPIO_DeInit(NULL, 1u);

    // Kiểm tra kết quả trả về là lỗi do con trỏ NULL
    assert(result == STAT_ERROR);

    printf("-> PASSED\n");
  }

  void test_GPIO_DeInit_InvalidParameter_ShouldAssert(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC5: Check Invalid Parameter...\n");
    
    // Gọi hàm GPIO_DeInit với tham số không hợp lệ và kiểm tra assert
    // Lưu ý: Trong môi trường unit test, assert sẽ dừng chương trình nếu điều kiện
    // không thỏa mãn, nên ta sẽ gọi hàm này trong một test case riêng biệt để kiểm tra assert.
    ASSERT_EXPECT_FAIL(GPIO_DeInit(GPIOx, 0x0000)); // Không chọn chân nào
  }

  void test_GPIO_DeInit_ValidParameter_ShouldResetGPIO(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC6: Check Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho GPIO_DeInit
    ui16 Pin = 0x0001; // Chọn chân 0

    // Gọi hàm GPIO_DeInit với tham số hợp lệ
    RETR_STAT result = GPIO_DeInit(GPIOx, Pin);

    // Kiểm tra kết quả trả về là thành công
    assert(result == STAT_DONE);

    printf("-> PASSED\n");
  }

  void test_GPIO_DeInit_MultiplePins_ShouldResetAllSelectedPins(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC6.1: Check Multiple Pins...\n");
    
    // Chuẩn bị tham số hợp lệ cho GPIO_DeInit với nhiều chân được chọn
    ui16 Pin = GPIO_PIN_10 | GPIO_PIN_6 | GPIO_PIN_2; // Chọn chân 10, 6 và 2

    // Gọi hàm GPIO_DeInit với tham số hợp lệ
    RETR_STAT result = GPIO_DeInit(GPIOx, Pin);

    // Kiểm tra kết quả trả về là thành công
    assert(result == STAT_DONE);

    printf("-> PASSED\n");
  }

  void test_GPIO_ReadPin_NullPointer_ShouldReturnError(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC7: Check Read Pin with Null Pointer...\n");
    
    // Gọi hàm GPIO_ReadPin với con trỏ NULL
    RETR_STAT result = GPIO_ReadPin(NULL, 0x0001);

    // Kiểm tra kết quả trả về là lỗi do con trỏ NULL
    assert(result == GPIO_PIN_UNF);

    printf("-> PASSED\n");
  }

  void test_GPIO_ReadPin_InvalidParameter_ShouldAssert(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC8: Check Read Pin with Invalid Parameter...\n");
    
    // Gọi hàm GPIO_ReadPin với tham số không hợp lệ và kiểm tra assert
    // Lưu ý: Trong môi trường unit test, assert sẽ dừng chương trình nếu điều kiện
    // không thỏa mãn, nên ta sẽ gọi hàm này trong một test case riêng biệt để kiểm tra assert.
    ASSERT_EXPECT_FAIL(GPIO_ReadPin(GPIOx, 0x0000)); // Không chọn chân nào
  }
  
  void test_GPIO_ReadPin_ValidParameter_ShouldReturnPinState(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC9: Check Read Pin with Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho GPIO_ReadPin
    ui16 Pin = GPIO_PIN_14; // Chọn chân 14

    // Thiết lập giá trị cho thanh ghi IDR để mô phỏng trạng thái chân GPIO
    GPIOx_IDR_data_set(GPIOx, 0x4501); // Chân 14 ở mức cao

    // Gọi hàm GPIO_ReadPin với tham số hợp lệ và kiểm tra kết quả trả về là GPIO_PIN_SET
    PIN_RETR_Enum result = GPIO_ReadPin(GPIOx, Pin);
    assert(result == GPIO_PIN_SET);

    // Thiết lập lại giá trị cho thanh ghi IDR để mô phỏng trạng thái chân GPIO
    GPIOx_IDR_data_set(GPIOx, 0x0000); // Chân 14 ở mức thấp

    // Gọi hàm GPIO_ReadPin với tham số hợp lệ và kiểm tra kết quả trả về là GPIO_PIN_RESET
    result = GPIO_ReadPin(GPIOx, Pin);
    assert(result == GPIO_PIN_RESET);

    printf("-> PASSED\n");
  }

  void test_GPIO_WritePin_NullPointer_ShouldDoNothing(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC10: Check Write Pin with Null Pointer...\n");
    
    // Gọi hàm GPIO_WritePin với con trỏ NULL
    GPIO_WritePin(NULL, GPIO_PIN_7, GPIO_PIN_SET);

    // Nếu không bị crash thì test case đã thành công
    printf("-> PASSED\n");
  }

  void test_GPIO_WritePin_InvalidParameter_ShouldAssert(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC11: Check Write Pin with Invalid Parameter...\n");
    
    // Gọi hàm GPIO_WritePin với tham số không hợp lệ và kiểm tra assert
    // Lưu ý: Trong môi trường unit test, assert sẽ dừng chương trình nếu điều kiện
    // không thỏa mãn, nên ta sẽ gọi hàm này trong một test case riêng biệt để kiểm tra assert.
    ASSERT_EXPECT_FAIL(GPIO_WritePin(GPIOx, 0x0000, GPIO_PIN_SET)); // Không chọn chân nào
  }

  void test_GPIO_WritePin_ValidParameter_ShouldSetODR(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC12: Check Write Pin with Valid Parameter...\n");
    
    // Chuẩn bị tham số hợp lệ cho GPIO_WritePin
    ui16 Pin = GPIO_PIN_11; // Chọn chân 11

    // Gọi hàm GPIO_WritePin để đưa chân 11 lên mức cao
    GPIO_WritePin(GPIOx, Pin, GPIO_PIN_SET);

    // Kiểm tra giá trị ODR đã được set đúng cách (chân 11 ở mức cao)
    assert((GPIOx->GPIO_ODR & Pin) == Pin);

    // Gọi hàm GPIO_WritePin để đưa chân 11 về mức thấp
    GPIO_WritePin(GPIOx, Pin, GPIO_PIN_RESET);

    // Kiểm tra giá trị ODR đã được reset đúng cách (chân 11 ở mức thấp)
    assert((GPIOx->GPIO_ODR & Pin) == 0x0000);

    printf("-> PASSED\n");
  }

  void test_GPIO_TogglePin_NullPointer_ShouldDoNothing(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC13: Check Null Pointer...\n");
    
    // Gọi hàm GPIO_TogglePin với con trỏ NULL
    GPIO_TogglePin(NULL, 0x0001);

    // Nếu không bị crash thì test case đã thành công
    printf("-> PASSED\n");
  }

  void test_GPIO_TogglePin_InvalidParameter_ShouldAssert(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC14: Check Invalid Parameter...\n");
    
    // Gọi hàm GPIO_TogglePin với tham số không hợp lệ và kiểm tra assert
    // Lưu ý: Trong môi trường unit test, assert sẽ dừng chương trình nếu điều kiện
    // không thỏa mãn, nên ta sẽ gọi hàm này trong một test case riêng biệt để kiểm tra assert.
    ASSERT_EXPECT_FAIL(GPIO_TogglePin(GPIOx, 0x0000)); // Không chọn chân nào
  }

  void test_GPIO_TogglePin_ShouldToggleODR(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC15: Check Toggle Pin...\n");
    
    // Chuẩn bị tham số hợp lệ cho GPIO_Init để cấu hình chân 0 là output
    GPIO_Init_Param init_param;
    init_param.Pin = GPIO_PIN_9; // Chọn chân 9
    init_param.Mode = GPIO_MODE_OUTPUT_10MHz_PP; // Chế độ output push-pull 10MHz
    init_param.Pull = GPIO_NOPULL; // Không kéo lên hay xuống

    // Gọi hàm GPIO_Init để cấu hình chân 0
    RETR_STAT result = GPIO_Init(GPIOx, &init_param);
    assert(result == STAT_DONE);

    // Gọi hàm GPIO_TogglePin để toggle chân 0
    GPIO_TogglePin(GPIOx, GPIO_PIN_9);

    // Kiểm tra giá trị ODR đã được toggle đúng cách (từ 0x0000 sang 0x0001)
    assert((GPIOx->GPIO_ODR & GPIO_PIN_9) == GPIO_PIN_9);

    // Gọi hàm GPIO_TogglePin lần nữa để toggle chân 0 về mức thấp
    GPIO_TogglePin(GPIOx, GPIO_PIN_9);

    // Kiểm tra giá trị ODR đã được toggle đúng cách (từ 0x0001 về 0x0000)
    assert((GPIOx->GPIO_ODR & GPIO_PIN_9) == (ui32)0);

    printf("-> PASSED\n");
  }

  void test_GPIO_LockPin_NullPointer_ShouldReturnError(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC16: Check Null Pointer...\n");
    
    // Gọi hàm GPIO_LockPin với con trỏ NULL
    RETR_STAT result = GPIO_LockPin(NULL, 0x0001);

    // Kiểm tra kết quả trả về là lỗi do con trỏ NULL
    assert(result == STAT_ERROR);

    printf("-> PASSED\n");
  }

  void test_GPIO_LockPin_InvalidParameter_ShouldAssert(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC17: Check Invalid Parameter...\n");
    
    // Gọi hàm GPIO_LockPin với tham số không hợp lệ và kiểm tra assert
    // Lưu ý: Trong môi trường unit test, assert sẽ dừng chương trình nếu điều kiện
    // không thỏa mãn, nên ta sẽ gọi hàm này trong một test case riêng biệt để kiểm tra assert.
    ASSERT_EXPECT_FAIL(GPIO_LockPin(GPIOx, 0x0000)); // Không chọn chân nào
  }

  void test_GPIO_LockPin_ShouldSetLCKR(GPIO_REGS_Typedef *GPIOx) {
    setup();
    printf("TC18: Check Lock Pin...\n");
    
    // Chuẩn bị tham số hợp lệ cho GPIO_Init để cấu hình chân 0 là output
    GPIO_Init_Param init_param;
    init_param.Pin = GPIO_PIN_0; // Chọn chân 0
    init_param.Mode = GPIO_MODE_OUTPUT_10MHz_PP; // Chế độ output push-pull 10MHz
    init_param.Pull = GPIO_NOPULL; // Không kéo lên hay xuống

    // Gọi hàm GPIO_Init để cấu hình chân 0
    RETR_STAT result = GPIO_Init(GPIOx, &init_param);
    assert(result == STAT_DONE);

    // Gọi hàm GPIO_LockPin để khóa chân 0
    result = GPIO_LockPin(GPIOx, GPIO_PIN_0);
    assert(result == STAT_DONE);

    // Kiểm tra giá trị LCKR đã được set đúng cách (bit tương ứng với chân 0 được set)
    assert((GPIOx->GPIO_LCKR & GPIO_PIN_0) == GPIO_PIN_0);

    printf("-> PASSED\n");
  }

int main() {
  printf("\n--- GPIO UNIT TEST ---\n");
  
  // Chạy tất cả các test case
  test_GPIO_Init_NullPointer_ShouldReturnError(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_Init_InvalidParameter_ShouldAssert(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_Init_ValidParameter_ShouldConfigureGPIO(&MOCK_GPIOA_REGS, GPIO_MODE_OUTPUT_10MHz_PP);
  printf("\n");
  test_GPIO_Init_MultiplePins_ShouldConfigureAllSelectedPins(&MOCK_GPIOA_REGS, GPIO_MODE_INPUT_PU_PD);
  printf("\n");
  test_GPIO_DeInit_NullPointer_ShouldReturnError(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_DeInit_InvalidParameter_ShouldAssert(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_DeInit_ValidParameter_ShouldResetGPIO(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_DeInit_MultiplePins_ShouldResetAllSelectedPins(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_ReadPin_NullPointer_ShouldReturnError(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_ReadPin_InvalidParameter_ShouldAssert(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_ReadPin_ValidParameter_ShouldReturnPinState(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_WritePin_NullPointer_ShouldDoNothing(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_WritePin_InvalidParameter_ShouldAssert(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_WritePin_ValidParameter_ShouldSetODR(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_TogglePin_NullPointer_ShouldDoNothing(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_TogglePin_InvalidParameter_ShouldAssert(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_TogglePin_ShouldToggleODR(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_LockPin_NullPointer_ShouldReturnError(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_LockPin_InvalidParameter_ShouldAssert(&MOCK_GPIOA_REGS);
  printf("\n");
  test_GPIO_LockPin_ShouldSetLCKR(&MOCK_GPIOA_REGS);
  printf("\n");

  printf("\n--- ALL TESTS PASSED ---\n");
  return 0;
}
