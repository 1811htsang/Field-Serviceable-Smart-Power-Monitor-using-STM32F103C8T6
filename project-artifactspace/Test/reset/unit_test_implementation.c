/*
 * unit_test_implementation.c
 *
 *  Created on: Feb 7, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include <stdio.h>
  #include <assert.h>
	#include <string.h>
  #include "lib_keyword_def.h"
  #include "lib_condition_def.h"
  #include "lib_reset_def.h"
  #include "lib_reset_hal.h"
  #include "header_dependency.h"

// Khai báo ngoại vi giả cho mục đích unit test

  RCC_REGS_Typedef MOCK_RCC_REGS;
  
  __vo ui32 mock_aircr_reg; 
  __vo BLANK_REG* SCB_AIRCR_REG;
  

// Gọi ủy quyền các biến và hàm mock từ source_dependency.c

  /**
   * Ghi chú:
   * Module reset không có hàm nào phụ thuộc ngoài module nên không cần ủy quyền biến hay hàm mock.
   */

// Định nghĩa các hàm

  void setup() {
    /*
      Hàm này được gọi trước mỗi test case để khởi tạo lại trạng thái cho unit test
    */

    // Reset các cờ reset trong thanh ghi RCC giả về trạng thái chưa reset
    memset(&MOCK_RCC_REGS, 0, sizeof(MOCK_RCC_REGS));

    // Reset giá trị thanh ghi AIRCR giả về 0
    mock_aircr_reg = 0;
    SCB_AIRCR_REG = (BLANK_REG *)&mock_aircr_reg;
  }

  void PINRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi tín hiệu bên ngoài (PIN NRST) bằng cách thiết lập cờ PINRSTF trong thanh ghi RCC giả
    */

    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_PINRSTF_OCCURRED);
  }

  void PORRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi nguồn điện (POR) bằng cách thiết lập cờ PORRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_PORRSTF_OCCURRED);
  }

  void SFTRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi phần mềm (SFT) bằng cách thiết lập cờ SFTRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_SFTRSTF_OCCURRED);
  }

  void IWDGRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi watchdog độc lập (IWDG) bằng cách thiết lập cờ IWDGRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_IWDGRSTF_OCCURRED);
  } 

  void WWDGRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi watchdog cửa sổ (WWDG) bằng cách thiết lập cờ WWDGRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_WWDGRSTF_OCCURRED);
  } 

  void LPWRRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi chế độ điện áp thấp (Low Power) bằng cách thiết lập cờ LPWRRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_LPWRRSTF_OCCURRED);
  }

  void RMVF_set() {
    /*
      Hàm này mô phỏng việc xóa các cờ reset bằng cách ghi RMVF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_RMVF_SET);
  }

  void test_RST_SRC_Capture(RCC_RSTFLG_Typedef *reset_source) {
    /*
      Hàm này thực hiện gọi hàm RST_SRC_Capture và kiểm tra kết quả trả về
    */
    
    // Giả lập tín hiệu đã có reset xảy ra trước đó
    PINRSTF_set();
    PORRSTF_set();

    // Gọi hàm RST_SRC_Capture với con trỏ reset_source
    RST_SRC_Capture(reset_source);
  }

  void test_Capture_NullPointer_ShouldDoNothing() {
    setup();
    printf("TC1: Check Null Pointer...\n");
    
    // Gọi hàm RST_SRC_Capture với con trỏ NULL
    RST_SRC_Capture(NULL);

    // Nếu không bị crash thì test case đã thành công
    printf("-> PASSED\n");
  }
  
  void test_Capture_AllResetFlags_ShouldSetAllFlags(RCC_RSTFLG_Typedef *reset_source) {
    setup();
    printf("TC2: Capture All Reset Flags...\n");
    
    // Gọi hàm test để mô phỏng việc capture các cờ reset
    test_RST_SRC_Capture(reset_source);

    // Kiểm tra cờ trong đã được set đúng
    assert(__SET_FLAG_CHECK(reset_source->IsPinReset));
    assert(__SET_FLAG_CHECK(reset_source->IsPorReset));

    printf("-> PASSED\n");
  }

  void test_Capture_NoResetFlags_ShouldClearAllFlags(RCC_RSTFLG_Typedef *reset_source) {
    setup();
    printf("TC3: Capture No Reset Flags...\n");
    
    // Gọi hàm RST_SRC_Capture với con trỏ reset_source
    RST_SRC_Capture(reset_source);

    // Kiểm tra cờ trong đã được clear đúng
    assert(__RESET_FLAG_CHECK(reset_source->IsPinReset));
    assert(__RESET_FLAG_CHECK(reset_source->IsPorReset));
    assert(__RESET_FLAG_CHECK(reset_source->IsSftReset));
    assert(__RESET_FLAG_CHECK(reset_source->IsIwdgReset));
    assert(__RESET_FLAG_CHECK(reset_source->IsWwdgReset));
    assert(__RESET_FLAG_CHECK(reset_source->IsLowPwrReset));

    printf("-> PASSED\n");
  }

int main() {
    printf("\n--- RESET UNIT TEST ---\n");
    
    test_Capture_NullPointer_ShouldDoNothing();
    
    RCC_RSTFLG_Typedef reset_source;

    test_Capture_AllResetFlags_ShouldSetAllFlags(&reset_source);
    test_Capture_NoResetFlags_ShouldClearAllFlags(&reset_source);

    printf("\n--- ALL TESTS PASSED ---\n");
    return 0;
  }

