/*
 * unit_test_implementation.c
 *
 *  Created on: Jan 21, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include <stdio.h>
  #include <assert.h>
	#include <string.h>
  #include "lib_keyword_def.h"
  #include "lib_iwdg_def.h"
	#include "lib_iwdg_hal.h"
  #include "header_dependency.h"

// Khai báo ngoại vi IWDG giả cho mục đích unit test

  IWDG_REGS_Typedef MOCK_IWDG_REGS;

// Gọi ủy quyền các biến và hàm mock từ source_dependency.c

  extern RETR_STAT RCC_IsLSIReady_Expect;
  extern RETR_STAT RCC_CLK_Init_Expect;
  extern RETR_STAT IWDG_IsPrescalerUpdated_Expect;
  extern RETR_STAT IWDG_IsReloadValueUpdated_Expect;

// Định nghĩa các hàm 

  void setup() {
    // Hàm thiết lập ban đầu cho mỗi test case
    // Có thể thêm các thao tác khởi tạo biến toàn cục ở đây nếu cần

    // Reset thanh ghi IWDG giả về các giá trị reset 
    MOCK_IWDG_REGS.KR.KEY = 0x0000;
    MOCK_IWDG_REGS.PR.PR = 0x0000;
    MOCK_IWDG_REGS.RLR.RL = 0x0FFF;
    MOCK_IWDG_REGS.SR.PVU = 0;

    RCC_IsLSIReady_Expect = STAT_RDY;
    RCC_CLK_Init_Expect = STAT_OK;
    IWDG_IsPrescalerUpdated_Expect = STAT_RDY;
    IWDG_IsReloadValueUpdated_Expect = STAT_RDY;
  }

  void test_Init_NullPointer_ShouldReturnError() {
    setup();
    printf("TC1: Check Null Pointer...\n");
    assert(IWDG_Init(NULL) == STAT_ERROR);
    printf("-> PASSED\n");
  }

  void test_Init_Success_ShouldSetRegisters() {
      setup();
      printf("TC2: Init Success Happy Path...\n");
      
      IWDG_Init_Param param = { .Prescaler = IWDG_PR_REG_PR_DIV_16, .Reload = 0x500 };
      
      RETR_STAT result = IWDG_Init(&param);
      
      assert(result == STAT_DONE);
      // Kiểm tra giá trị thực tế đã ghi vào thanh ghi giả (bitfield)
      assert(IWDG_REGS_PTR->PR.PR == IWDG_PR_REG_PR_DIV_16);
      assert(IWDG_REGS_PTR->RLR.RL == 0x500);
      printf("-> PASSED\n");
  }

  void test_Init_LSI_NotReady_ShouldTryToEnableLSI() {
    setup();
    printf("TC3: LSI Not Ready -> Enable LSI...");
    
    RCC_IsLSIReady_Expect = STAT_NRDY; // Giả lập LSI ban đầu chưa sẵn sàng
    RCC_CLK_Init_Expect = STAT_OK;     // Giả lập việc bật LSI thành công
    
    IWDG_Init_Param param = { .Prescaler = 0, .Reload = 100 };
    assert(IWDG_Init(&param) == STAT_DONE);
    printf("-> PASSED\n");
  }

  void test_Init_LSI_InitFail_ShouldReturnError() {
      setup();
      printf("TC4: LSI Init Failed -> Return Error...");
      
      RCC_IsLSIReady_Expect = STAT_NRDY;
      RCC_CLK_Init_Expect = STAT_ERROR;  // Giả lập bật LSI thất bại
      
      IWDG_Init_Param param = { .Prescaler = 0, .Reload = 100 };
      assert(IWDG_Init(&param) == STAT_ERROR);
      printf("-> PASSED\n");
  }

  void test_Init_PrescalerBusy_ShouldReturnNRDY() {
      setup();
      printf("TC5: Prescaler Busy -> Return NRDY...");
      
      IWDG_IsPrescalerUpdated_Expect = STAT_NRDY; // Giả lập thanh ghi PR đang bận
      
      IWDG_Init_Param param = { .Prescaler = 4, .Reload = 100 };
      assert(IWDG_Init(&param) == STAT_NRDY);
      printf("-> PASSED\n");
  }

// Thực thi tất cả các test case

int main() {
    printf("\n--- IWDG UNIT TEST ---\n");
    
    test_Init_NullPointer_ShouldReturnError();
    test_Init_Success_ShouldSetRegisters();
    test_Init_LSI_NotReady_ShouldTryToEnableLSI();
    test_Init_LSI_InitFail_ShouldReturnError();
    test_Init_PrescalerBusy_ShouldReturnNRDY();
    
    printf("----------------------------------------\n");
    printf("ALL TESTS PASSED!\n\n");
    return 0;
}
