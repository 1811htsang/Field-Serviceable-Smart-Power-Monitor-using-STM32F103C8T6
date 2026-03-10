/*
 * unit_test_implementation.c
 *
 *  Created on: Mar 5, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng cho unit test

  #include <stdio.h>
  #include <assert.h>
  #include <string.h>
  #include "lib_keyword_def.h"
  #include "lib_condition_def.h"
  #include "lib_test_util.h"
  #include "lib_nvic_def.h"
  #include "lib_nvic_hal.h"
  #include "header_dependency.h"

// Khai báo ngoại vi giả cho mục đích unit test

  NVIC_REGS_Typedef MOCK_NVIC_REGS;

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

    // Hàm này được gọi trước mỗi test case để khởi tạo lại trạng thái cho unit test

    // Reset thanh ghi NVIC giả về các giá trị reset
    memset(&MOCK_NVIC_REGS, 0, sizeof(NVIC_REGS_Typedef));
  }

  void test_NVIC_INTR_Config_NULLPointer_ShouldReturnError() {
    setup();
    printf("TC1: Check Null Pointer...\n");
    assert(__ERROR_CHECK(NVIC_INTR_Config(NULL)));
    printf("-> PASSED\n");
  }

  void test_NVIC_INTR_Config_InvalidPosition_ShouldReturnError() {
    setup();
    printf("TC2: Check Invalid Position...\n");
    
    NVIC_INTR_Param intr_param = { .Position = 96 }; // Giá trị Position không hợp lệ (>= 96)
    ASSERT_EXPECT_FAIL(NVIC_INTR_Config(&intr_param)); // Hàm sẽ assert fail do Position vượt quá giới hạn
  }

  void test_NVIC_INTR_Config_ValidInput_ShouldConfigureInterrupt() {
    setup();
    printf("TC3: Valid Input -> Configure Interrupt...\n");
    
    NVIC_INTR_Param intr_param = { .Position = 10, .Priority = 5, .Status = INTR_STAT_ENABLE };
    assert(NVIC_INTR_Config(&intr_param) == STAT_DONE); // Hàm trả về STAT_DONE khi cấu hình thành công

    // Kiểm tra thanh ghi ISER đã được set bit tương ứng
    assert((MOCK_NVIC_REGS.NVIC_ISER[0] & (0x0001u << 10)) != 0); // Bit 10 của ISER[0] phải được set

    // Kiểm tra thanh ghi IPR đã được cấu hình mức ưu tiên đúng
    assert((MOCK_NVIC_REGS.NVIC_IPRE[10] & 0xF0) == (5 << 4)); // Mức ưu tiên 5 phải được ghi vào 4 bit cao của byte IPR[10]
    printf("-> PASSED\n");
  }

  void test_NVIC_INTR_Config_PendingStatus_ShouldReturnBusy() {
    setup();
    printf("TC4: Pending Status -> Return Busy...\n");
    
    NVIC_INTR_Param intr_param = { .Position = 20, .Priority = 3, .Status = INTR_STAT_PENDING };
    assert(__BUSY_CHECK(NVIC_INTR_Config(&intr_param))); // Hàm trả về STAT_BUSY khi Status là PENDING
    printf("-> PASSED\n");
  }

  void test_NVIC_INTR_Config_DisableStatus_ShouldDisableInterrupt() {
    setup();
    printf("TC5: Disable Status -> Disable Interrupt...\n");
    
    // Trước tiên, kích hoạt ngắt để sau đó kiểm tra việc vô hiệu hóa
    NVIC_INTR_Param intr_param_enable = { .Position = 15, .Priority = 2, .Status = INTR_STAT_ENABLE };
    assert(NVIC_INTR_Config(&intr_param_enable) == STAT_DONE); // Kích hoạt ngắt thành công

    // Bây giờ, vô hiệu hóa ngắt
    NVIC_INTR_Param intr_param_disable = { .Position = 15, .Priority = 2, .Status = INTR_STAT_DISABLE };
    assert(NVIC_INTR_Config(&intr_param_disable) == STAT_DONE); // Vô hiệu hóa ngắt thành công

    // Kiểm tra thanh ghi ICER đã được set bit tương ứng để vô hiệu hóa ngắt
    assert((MOCK_NVIC_REGS.NVIC_ICER[0] & (0x0001u << 15)) != 0); // Bit 15 của ICER[0] phải được set để vô hiệu hóa ngắt
    printf("-> PASSED\n");
  }

  void test_NVIC_INTR_Config_InvalidStatus_ShouldReturnError() {
    setup();
    printf("TC6: Invalid Status -> Return Error...\n");
    
    NVIC_INTR_Param intr_param = { .Position = 5, .Priority = 1, .Status = INTR_STAT_UNF }; // Giá trị Status không hợp lệ
    assert(__ERROR_CHECK(NVIC_INTR_Config(&intr_param))); // Hàm trả về STAT_ERROR khi Status không hợp lệ
    printf("-> PASSED\n");
  }

  void test_NVIC_INTR_GetActivation_ShouldReturnCorrectStatus() {
    setup();
    printf("TC7: Get Activation Status -> Return Correct Status...\n");
    
    // Kích hoạt ngắt ở Position 25
    NVIC_INTR_Param intr_param = { .Position = 25, .Priority = 0, .Status = INTR_STAT_ENABLE };
    assert(__DONE_CHECK(NVIC_INTR_Config(&intr_param))); // Kích hoạt ngắt thành công

    // Kiểm tra trạng thái kích hoạt của ngắt
    assert(NVIC_INTR_GetActivation(25) == 1); // Ngắt ở Position 25 phải đang được kích hoạt (return 1)
    assert(NVIC_INTR_GetActivation(26) == 0); // Ngắt ở Position 26 phải không được kích hoạt (return 0)
  }

  void test_NVIC_INTR_GetActivation_InvalidPosition_ShouldReturn0() {
    setup();
    printf("TC8: Get Activation Status with Invalid Position -> Return 0...\n");
    
    // Kiểm tra trạng thái kích hoạt của ngắt với Position không hợp lệ
    assert(NVIC_INTR_GetActivation(100) == 0); // Position 100 không hợp lệ, hàm nên trả về 0
    printf("-> PASSED\n");
  }

  void test_NVIC_INTR_GetActivation_MultipleInterrupts_ShouldReturnCorrectStatus() {
    setup();
    printf("TC9: Get Activation Status with Multiple Interrupts -> Return Correct Status...\n");
    
    // Kích hoạt ngắt ở Position 5 và 40
    NVIC_INTR_Param intr_param1 = { .Position = 5, .Priority = 0, .Status = INTR_STAT_ENABLE };
    NVIC_INTR_Param intr_param2 = { .Position = 40, .Priority = 0, .Status = INTR_STAT_ENABLE };
    assert(__DONE_CHECK(NVIC_INTR_Config(&intr_param1))); // Kích hoạt ngắt ở Position 5 thành công
    assert(__DONE_CHECK(NVIC_INTR_Config(&intr_param2))); // Kích hoạt ngắt ở Position 40 thành công

    // Kiểm tra trạng thái kích hoạt của các ngắt
    assert(NVIC_INTR_GetActivation(5) == 1); // Ngắt ở Position 5 phải đang được kích hoạt (return 1)
    assert(NVIC_INTR_GetActivation(40) == 1); // Ngắt ở Position 40 phải đang được kích hoạt (return 1)
    assert(NVIC_INTR_GetActivation(10) == 0); // Ngắt ở Position 10 phải không được kích hoạt (return 0)
    printf("-> PASSED\n");
  }

  void test_NVIC_INTR_GetPending_ShouldReturnCorrectStatus() {
    setup();
    printf("TC10: Get Pending Status -> Return Correct Status...\n");
    
    // Set pending bit cho ngắt ở Position 12
    NVIC_INTR_Param intr_param = { .Position = 12, .Priority = 0, .Status = INTR_STAT_PENDING };
    assert(__BUSY_CHECK(NVIC_INTR_Config(&intr_param))); // Cấu hình với Status PENDING sẽ trả về STAT_BUSY

    // Cấu hình pending
    NVIC_INTR_Pending_Enable(&intr_param); // Set pending bit cho ngắt ở Position 12

    // Kiểm tra trạng thái pending của ngắt
    assert(NVIC_INTR_GetPending(12) == 1); // Ngắt ở Position 12 phải đang ở trạng thái pending (return 1)
    assert(NVIC_INTR_GetPending(13) == 0); // Ngắt ở Position 13 phải không đang ở trạng thái pending (return 0)
    printf("-> PASSED\n");
  }

  void test_NVIC_INTR_GetPending_InvalidPosition_ShouldReturn0() {
    setup();
    printf("TC11: Get Pending Status with Invalid Position -> Return 0...\n");
    
    // Kiểm tra trạng thái pending của ngắt với Position không hợp lệ
    assert(NVIC_INTR_GetPending(100) == 0); // Position 100 không hợp lệ, hàm nên trả về 0
    printf("-> PASSED\n");
  }

// Thực thi tất cả các test case

int main() {
    printf("\n--- NVIC UNIT TEST ---\n");
    
    test_NVIC_INTR_Config_NULLPointer_ShouldReturnError();
    printf("\n");
    test_NVIC_INTR_Config_InvalidPosition_ShouldReturnError();
    printf("\n");
    test_NVIC_INTR_Config_ValidInput_ShouldConfigureInterrupt();
    printf("\n");

    test_NVIC_INTR_Config_PendingStatus_ShouldReturnBusy();
    printf("\n");
    test_NVIC_INTR_Config_DisableStatus_ShouldDisableInterrupt();
    printf("\n");
    test_NVIC_INTR_Config_InvalidStatus_ShouldReturnError();
    printf("\n");

    test_NVIC_INTR_GetActivation_ShouldReturnCorrectStatus();
    printf("\n");
    test_NVIC_INTR_GetActivation_InvalidPosition_ShouldReturn0();
    printf("\n");
    test_NVIC_INTR_GetActivation_MultipleInterrupts_ShouldReturnCorrectStatus();
    printf("\n");

    test_NVIC_INTR_GetPending_ShouldReturnCorrectStatus();
    printf("\n");
    test_NVIC_INTR_GetPending_InvalidPosition_ShouldReturn0();
    printf("\n");

    printf("----------------------------------------\n");
    printf("ALL TESTS PASSED!\n\n");
    return 0;
}