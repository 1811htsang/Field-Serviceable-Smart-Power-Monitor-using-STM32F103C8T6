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
  __vo BLANK_REG_32B* SCB_AIRCR_REG;
  

// Gọi ủy quyền các biến và hàm mock từ source_dependency.c

  /**
   * Ghi chú:
   * Module reset không có hàm nào phụ thuộc ngoài module nên không cần ủy quyền biến hay hàm mock.
   */

// Định nghĩa các hàm

  /*
   * Khởi tạo lại trạng thái unit test cho driver reset.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Xóa thanh ghi RCC giả.
   *   - Đưa thanh ghi AIRCR giả về trạng thái mặc định.
   *
   * Trả về:
   *   Không có.
   */
  void setup() {
    /*
      Hàm này được gọi trước mỗi test case để khởi tạo lại trạng thái cho unit test
    */

    // Reset các cờ reset trong thanh ghi RCC giả về trạng thái chưa reset
    memset(&MOCK_RCC_REGS, 0, sizeof(MOCK_RCC_REGS));

    // Reset giá trị thanh ghi AIRCR giả về 0
    mock_aircr_reg = 0;
    SCB_AIRCR_REG = (BLANK_REG_32B *)&mock_aircr_reg;
  }

  /*
   * Mô phỏng cờ reset do tín hiệu ngoài (PIN NRST).
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set cờ PINRSTF trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void PINRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi tín hiệu bên ngoài (PIN NRST) bằng cách thiết lập cờ PINRSTF trong thanh ghi RCC giả
    */

    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_PINRSTF_OCCURRED);
  }

  /*
   * Mô phỏng cờ reset do nguồn điện (POR).
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set cờ PORRSTF trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void PORRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi nguồn điện (POR) bằng cách thiết lập cờ PORRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_PORRSTF_OCCURRED);
  }

  /*
   * Mô phỏng cờ reset do phần mềm (SFT).
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set cờ SFTRSTF trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void SFTRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi phần mềm (SFT) bằng cách thiết lập cờ SFTRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_SFTRSTF_OCCURRED);
  }

  /*
   * Mô phỏng cờ reset do watchdog độc lập (IWDG).
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set cờ IWDGRSTF trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void IWDGRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi watchdog độc lập (IWDG) bằng cách thiết lập cờ IWDGRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_IWDGRSTF_OCCURRED);
  } 

  /*
   * Mô phỏng cờ reset do watchdog cửa sổ (WWDG).
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set cờ WWDGRSTF trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void WWDGRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi watchdog cửa sổ (WWDG) bằng cách thiết lập cờ WWDGRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_WWDGRSTF_OCCURRED);
  } 

  /*
   * Mô phỏng cờ reset do chế độ điện áp thấp.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set cờ LPWRRSTF trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void LPWRRSTF_set() {
    /*
      Hàm này mô phỏng việc reset bởi chế độ điện áp thấp (Low Power) bằng cách thiết lập cờ LPWRRSTF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_LPWRRSTF_OCCURRED);
  }

  /*
   * Mô phỏng thao tác xóa các cờ reset.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set bit RMVF trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void RMVF_set() {
    /*
      Hàm này mô phỏng việc xóa các cờ reset bằng cách ghi RMVF trong thanh ghi RCC giả
    */
    SET_BIT(MOCK_RCC_REGS.CSR, RCC_CSR_REG_RMVF_SET);
  }

  /*
   * Thiết lập điều kiện đầu vào cho bài test capture nguồn reset.
   *
   * Tham số:
   *   reset_source - Con trỏ tới cấu trúc lưu cờ reset cần kiểm tra.
   *
   * Logic:
   *   - Mô phỏng trạng thái reset đã xảy ra.
   *   - Gọi RST_SRC_Capture để lấy giá trị cờ reset.
   *
   * Trả về:
   *   Không có.
   */
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

  /*
   * Kiểm tra trường hợp RST_SRC_Capture nhận con trỏ NULL.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Gọi RST_SRC_Capture với con trỏ NULL.
   *   - Xác nhận chương trình không bị crash.
   *
   * Trả về:
   *   Không có.
   */
  void test_Capture_NullPointer_ShouldDoNothing() {
    setup();
    printf("TC1: Check Null Pointer...\n");
    
    // Gọi hàm RST_SRC_Capture với con trỏ NULL
    RST_SRC_Capture(NULL);

    // Nếu không bị crash thì test case đã thành công
    printf("-> PASSED\n");
  }
  
  /*
   * Kiểm tra capture đầy đủ các cờ reset đã được mô phỏng.
   *
   * Tham số:
   *   reset_source - Con trỏ tới cấu trúc lưu cờ reset cần kiểm tra.
   *
   * Logic:
   *   - Mô phỏng trạng thái reset xảy ra.
   *   - Gọi helper capture.
   *   - Xác nhận các cờ được set đúng.
   *
   * Trả về:
   *   Không có.
   */
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

  /*
   * Kiểm tra capture khi không có cờ reset nào được set.
   *
   * Tham số:
   *   reset_source - Con trỏ tới cấu trúc lưu cờ reset cần kiểm tra.
   *
   * Logic:
   *   - Gọi RST_SRC_Capture khi không mô phỏng reset nào.
   *   - Xác nhận toàn bộ cờ đều ở trạng thái reset.
   *
   * Trả về:
   *   Không có.
   */
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

/*
 * Chạy toàn bộ test case của driver reset.
 *
 * Tham số:
 *   Không có.
 *
 * Logic:
 *   - In tiêu đề unit test.
 *   - Chạy các test case theo thứ tự.
 *   - Trả về mã thành công nếu tất cả test đều qua.
 *
 * Trả về:
 *   int - 0 khi mọi test thành công.
 */
int main() {
    printf("\n--- RESET UNIT TEST ---\n");
    
    test_Capture_NullPointer_ShouldDoNothing();
    
    RCC_RSTFLG_Typedef reset_source;

    test_Capture_AllResetFlags_ShouldSetAllFlags(&reset_source);
    test_Capture_NoResetFlags_ShouldClearAllFlags(&reset_source);

    printf("\n--- ALL TESTS PASSED ---\n");
    return 0;
  }

