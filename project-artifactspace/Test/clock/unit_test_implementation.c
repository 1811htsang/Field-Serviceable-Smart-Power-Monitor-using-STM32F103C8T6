/*
 * unit_test_implementation.c
 *
 *  Created on: Feb 2, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include <stdio.h>
  #include <assert.h>
	#include <string.h>
  #include "lib_keyword_def.h"
  #include "lib_condition_def.h"
  #include "lib_clock_def.h"
	#include "lib_clock_hal.h"
  #include "header_dependency.h"

// Khai báo ngoại vi giả cho mục đích unit test

  RCC_REGS_Typedef MOCK_RCC_REGS;

// Gọi ủy quyền các biến và hàm mock từ source_dependency.c

  extern RETR_STAT IWDG_Init_Expect;
  extern ui IWDG_Start_Called;
  extern ui IWDG_Reload_Called;

// Định nghĩa các hàm 

  /*
   * Khởi tạo lại trạng thái unit test cho driver clock.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Xóa toàn bộ thanh ghi RCC giả.
   *   - Đặt lại các biến mock của IWDG về trạng thái mặc định.
   *
   * Trả về:
   *   Không có.
   */
  void setup() {
    /*
      Hàm này được gọi trước mỗi test case để khởi tạo lại trạng thái cho unit test
    */

    // Reset thanh ghi RCC giả về các giá trị reset
    memset(&MOCK_RCC_REGS, 0, sizeof(MOCK_RCC_REGS));

    // Reset các biến giả lập trạng thái trả về của các hàm mock
    IWDG_Init_Expect = STAT_DONE;
    IWDG_Start_Called = 0;
    IWDG_Reload_Called = 0;
  }

  /*
   * Mô phỏng trạng thái HSI đã sẵn sàng.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set cờ HSIRDY trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void HSIRDY_ready_set() {
    /*
      Hàm này mô phỏng việc HSI đã sẵn sàng bằng cách thiết lập cờ HSIRDY trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CR |= RCC_CR_REG_HSIRDY_ON;
  }

  /*
   * Mô phỏng trạng thái HSE đã sẵn sàng.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set cờ HSERDY trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void HSERDY_ready_set() {
    /*
      Hàm này mô phỏng việc HSE đã sẵn sàng bằng cách thiết lập cờ HSERDY trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CR |= RCC_CR_REG_HSERDY_ON;
  }

  /*
   * Mô phỏng trạng thái LSI đã sẵn sàng.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Set cờ LSIRDY trong thanh ghi RCC giả.
   *
   * Trả về:
   *   Không có.
   */
  void LSI_ready_set() {
    /*
      Hàm này mô phỏng việc LSI đã sẵn sàng bằng cách thiết lập cờ LSIRDY trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CSR |= RCC_CSR_REG_LSIRDY_ON;
  }

  /*
   * Mô phỏng việc SYSCLK đã được chuyển sang HSE.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Clear trường SW.
   *   - Ghi giá trị nguồn HSE vào SW.
   *   - Set cờ trạng thái SWS tương ứng.
   *
   * Trả về:
   *   Không có.
   */
  void SYSCLK_HSE_switched() {
    /*
      Hàm này mô phỏng việc chuyển đổi SYSCLK sang HSE bằng cách thiết lập trường SWS trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CFGR &= ~RCC_CFGR_REG_SW_MASK; // Clear trường SW
    MOCK_RCC_REGS.CFGR |= RCC_SYSCLK_SOURCE_HSE; // Set trường SW để chọn HSE làm SYSCLK
    MOCK_RCC_REGS.CFGR |= RCC_CFGR_REG_SWS_HSE; // Set cờ trạng thái SYSCLK để phản ánh việc chuyển đổi thành công
  }

  /*
   * Mô phỏng việc SYSCLK đã được chuyển sang HSI.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Clear trường SW.
   *   - Ghi giá trị nguồn HSI vào SW.
   *   - Set cờ trạng thái SWS tương ứng.
   *
   * Trả về:
   *   Không có.
   */
  void SYSCLK_HSI_switched() {
    /*
      Hàm này mô phỏng việc chuyển đổi SYSCLK sang HSI bằng cách thiết lập trường SWS trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CFGR &= ~RCC_CFGR_REG_SW_MASK; // Clear trường SW
    MOCK_RCC_REGS.CFGR |= RCC_SYSCLK_SOURCE_HSI; // Set trường SW để chọn HSI làm SYSCLK
    MOCK_RCC_REGS.CFGR |= RCC_CFGR_REG_SWS_HSI; // Set cờ trạng thái SYSCLK để phản ánh việc chuyển đổi thành công
  }

  /*
   * Kiểm tra trường hợp RCC_CLK_Init nhận con trỏ NULL.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Gọi hàm với tham số NULL.
   *   - Xác nhận hàm trả về lỗi.
   *
   * Trả về:
   *   Không có.
   */
  void test_Init_NullPointer_ShouldReturnError() {
    setup();
    printf("TC1: Check Null Pointer...\n");
    assert(__ERROR_CHECK(RCC_CLK_Init(NULL, NULL)));
    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra khởi tạo clock với nguồn HSI thành công.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Mô phỏng HSI sẵn sàng.
   *   - Gọi RCC_CLK_Init với nguồn HSI.
   *   - Kiểm tra cờ ready được set đúng.
   *
   * Trả về:
   *   Không có.
   */
  void test_Init_HSI_Success_ShouldSetReadyFlag() {
    setup();
    printf("TC2: Init HSI Success Happy Path...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = RCC_SYSCLK_SOURCE_HSI }; // HSI
    RCC_RDYFLG_Typdef rdy_flg;

    // Mô phỏng HSI sẵn sàng ngay lập tức
    HSIRDY_ready_set();

    // Mô phỏng LSI sẵn sàng ngay lập tức
    LSI_ready_set();
    
    RETR_STAT result = RCC_CLK_Init(&param, &rdy_flg);
    
    assert(__OK_CHECK(result));
    assert(__SET_FLAG_CHECK(rdy_flg.HSI_RDY_FLG));
    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra khởi tạo clock với nguồn HSE thành công.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Mô phỏng HSE sẵn sàng.
   *   - Gọi RCC_CLK_Init với nguồn HSE.
   *   - Kiểm tra cờ ready được set đúng.
   *
   * Trả về:
   *   Không có.
   */
  void test_Init_HSE_Success_ShouldSetReadyFlag() {
    setup();
    printf("TC3: Init HSE Success Happy Path...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = RCC_SYSCLK_SOURCE_HSE }; // HSE
    RCC_RDYFLG_Typdef rdy_flg;

    // Mô phỏng HSE sẵn sàng ngay lập tức
    HSERDY_ready_set();

    // Mô phỏng LSI sẵn sàng ngay lập tức
    LSI_ready_set();

    // Mô phỏng HSI sẵn sàng ngay lập tức
    HSIRDY_ready_set();

    // Mô phỏng việc chuyển đổi SYSCLK sang HSE thành công
    SYSCLK_HSE_switched();
    
    RETR_STAT result = RCC_CLK_Init(&param, &rdy_flg);
    
    assert(__OK_CHECK(result));
    assert(__SET_FLAG_CHECK(rdy_flg.HSE_RDY_FLG));
    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra trường hợp nguồn clock không hợp lệ.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Gọi RCC_CLK_Init với nguồn không hợp lệ.
   *   - Xác nhận hàm trả về lỗi.
   *
   * Trả về:
   *   Không có.
   */
  void test_Init_InvalidSource_ShouldReturnError() {
    setup();
    printf("TC5: Init with Invalid Source -> Return Error...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = 0xFFul }; // Nguồn không hợp lệ
    RCC_RDYFLG_Typdef rdy_flg;
    
    RETR_STAT result = RCC_CLK_Init(&param, &rdy_flg);
    
    assert(__ERROR_CHECK(result));
    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra trường hợp HSE không sẵn sàng khi khởi tạo.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Không mô phỏng HSE ready.
   *   - Gọi RCC_CLK_Init với nguồn HSE.
   *   - Xác nhận hàm trả về lỗi.
   *
   * Trả về:
   *   Không có.
   */
  void test_Init_HSE_NotReady_ShouldReturnError() {
    setup();
    printf("TC6: HSE Not Ready -> Return Error...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = RCC_SYSCLK_SOURCE_HSE }; // HSE
    RCC_RDYFLG_Typdef rdy_flg;

    // Không mô phỏng HSE sẵn sàng
    
    RETR_STAT result = RCC_CLK_Init(&param, &rdy_flg);
    
    assert(__ERROR_CHECK(result));
    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra trường hợp con trỏ ready flag là NULL.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Gọi RCC_CLK_Init với rdy_flg NULL.
   *   - Xác nhận hàm trả về lỗi.
   *
   * Trả về:
   *   Không có.
   */
  void test_Init_NullRdyFlag_ShouldReturnError() {
    setup();
    printf("TC8: Null Ready Flag Pointer -> Return Error...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = RCC_SYSCLK_SOURCE_HSI }; // HSI
    
    RETR_STAT result = RCC_CLK_Init(&param, NULL);
    
    assert(__ERROR_CHECK(result));
    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra trường hợp deinit HSI khi hệ thống đang bận.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Gọi RCC_CLK_DeInit trong điều kiện HSI đang bận.
   *   - Xác nhận hàm trả về trạng thái BUSY.
   *
   * Trả về:
   *   Không có.
   */
  void test_DeInit_HSI_Busy_ShouldReturnBusy() {
    setup();
    printf("TC9: HSI Busy -> Return BUSY...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = RCC_SYSCLK_SOURCE_HSI }; // HSI
    RCC_RDYFLG_Typdef rdy_flg;

    // Mô phỏng HSI bận (không thể tắt)
    
    RETR_STAT result = RCC_CLK_DeInit(&param, &rdy_flg);
    
    assert(__BUSY_CHECK(result));
    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra deinit HSE thành công và tắt HSE đúng cách.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Mô phỏng chuyển SYSCLK sang HSI.
   *   - Gọi RCC_CLK_DeInit với nguồn HSE.
   *   - Kiểm tra bit HSEON đã được xóa.
   *
   * Trả về:
   *   Không có.
   */
  void test_DeInit_HSE_Success_ShouldTurnOffHSE() {
    setup();
    printf("TC10: DeInit HSE Success Happy Path...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = RCC_SYSCLK_SOURCE_HSE }; // HSE
    RCC_RDYFLG_Typdef rdy_flg;

    // Mô phỏng HSI sẵn sàng để chuyển SYSCLK về HSI trước khi tắt HSE
    HSIRDY_ready_set();

    // Mô phỏng việc chuyển đổi SYSCLK sang HSI thành công
    SYSCLK_HSI_switched();
    
    RETR_STAT result = RCC_CLK_DeInit(&param, &rdy_flg);
    
    assert(__DONE_CHECK(result));
    assert((MOCK_RCC_REGS.CR & RCC_CR_REG_HSEON_SET) == RESET); // Kiểm tra HSE đã tắt
    printf("-> PASSED\n");
  }

// Thực thi tất cả các test case

/*
 * Chạy toàn bộ test case của driver clock.
 *
 * Tham số:
 *   Không có.
 *
 * Logic:
 *   - In tiêu đề unit test.
 *   - Gọi lần lượt tất cả test case.
 *   - In trạng thái hoàn tất nếu mọi test đều qua.
 *
 * Trả về:
 *   int - 0 khi mọi test thành công.
 */
int main() {
    printf("\n--- CLOCK UNIT TEST ---\n");
    
    test_Init_NullPointer_ShouldReturnError();
    test_Init_HSI_Success_ShouldSetReadyFlag();
    test_Init_HSE_Success_ShouldSetReadyFlag();
    test_Init_InvalidSource_ShouldReturnError();
    test_Init_HSE_NotReady_ShouldReturnError();
    test_Init_NullRdyFlag_ShouldReturnError();
    test_DeInit_HSI_Busy_ShouldReturnBusy();
    test_DeInit_HSE_Success_ShouldTurnOffHSE();
    
    printf("----------------------------------------\n");
    printf("ALL TESTS PASSED!\n\n");
    return 0;
}
