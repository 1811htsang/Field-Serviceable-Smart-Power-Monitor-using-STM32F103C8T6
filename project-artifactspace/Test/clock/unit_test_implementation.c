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

  void setup() {
    /*
      Hàm này được gọi trước mỗi test case để khởi tạo lại trạng thái cho unit test
    */

    // Reset thanh ghi RCC giả về các giá trị reset
    MOCK_RCC_REGS.CR.HSION = RESET;
    MOCK_RCC_REGS.CR.HSIRDY = RESET; 
    MOCK_RCC_REGS.CR.HSEON = RESET;
    MOCK_RCC_REGS.CR.HSERDY = RESET; 
    MOCK_RCC_REGS.CR.CSSON = RESET;
    MOCK_RCC_REGS.CFGR.SW = 0x00;
    MOCK_RCC_REGS.CFGR.SWS = 0x00;
    MOCK_RCC_REGS.CSR.LSION = RESET;
    MOCK_RCC_REGS.CSR.LSIRDY = RESET;

    // Reset các biến giả lập trạng thái trả về của các hàm mock
    IWDG_Init_Expect = STAT_DONE;
    IWDG_Start_Called = 0;
    IWDG_Reload_Called = 0;
  }

  void HSIRDY_ready_set() {
    /*
      Hàm này mô phỏng việc HSI đã sẵn sàng bằng cách thiết lập cờ HSIRDY trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CR.HSIRDY = SET;
  }

  void HSERDY_ready_set() {
    /*
      Hàm này mô phỏng việc HSE đã sẵn sàng bằng cách thiết lập cờ HSERDY trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CR.HSERDY = SET;
  }

  void LSI_ready_set() {
    /*
      Hàm này mô phỏng việc LSI đã sẵn sàng bằng cách thiết lập cờ LSIRDY trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CSR.LSIRDY = SET;
  }

  void SYSCLK_HSE_switched() {
    /*
      Hàm này mô phỏng việc chuyển đổi SYSCLK sang HSE bằng cách thiết lập trường SWS trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CFGR.SWS = RCC_SYSCLK_SOURCE_HSE;
  }

  void SYSCLK_HSI_switched() {
    /*
      Hàm này mô phỏng việc chuyển đổi SYSCLK sang HSI bằng cách thiết lập trường SWS trong thanh ghi RCC giả
    */
    MOCK_RCC_REGS.CFGR.SWS = RCC_SYSCLK_SOURCE_HSI;
  }

  void test_Init_NullPointer_ShouldReturnError() {
    setup();
    printf("TC1: Check Null Pointer...\n");
    assert(__ERROR_CHECK(RCC_CLK_Init(NULL, NULL)));
    printf("-> PASSED\n");
  }

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

  void test_Init_LSI_Success_ShouldSetReadyFlag() {
    setup();
    printf("TC4: Init LSI Success Happy Path...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = RCC_IWDG_SOURCE_LSI }; // LSI
    RCC_RDYFLG_Typdef rdy_flg;

    // Mô phỏng LSI sẵn sàng ngay lập tức
    LSI_ready_set();
    
    RETR_STAT result = RCC_CLK_Init(&param, &rdy_flg);
    
    assert(__OK_CHECK(result));
    assert(__SET_FLAG_CHECK(rdy_flg.LSI_RDY_FLG));
    printf("-> PASSED\n");
  }

  void test_Init_InvalidSource_ShouldReturnError() {
    setup();
    printf("TC5: Init with Invalid Source -> Return Error...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = 0xFFul }; // Nguồn không hợp lệ
    RCC_RDYFLG_Typdef rdy_flg;
    
    RETR_STAT result = RCC_CLK_Init(&param, &rdy_flg);
    
    assert(__ERROR_CHECK(result));
    printf("-> PASSED\n");
  }

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

  void test_Init_LSI_NotReady_ShouldReturnError() {
    setup();
    printf("TC7: LSI Not Ready -> Return Error...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = RCC_IWDG_SOURCE_LSI }; // LSI
    RCC_RDYFLG_Typdef rdy_flg;

    // Không mô phỏng LSI sẵn sàng
    
    RETR_STAT result = RCC_CLK_Init(&param, &rdy_flg);
    
    assert(__ERROR_CHECK(result));
    printf("-> PASSED\n");
  }

  void test_Init_NullRdyFlag_ShouldReturnError() {
    setup();
    printf("TC8: Null Ready Flag Pointer -> Return Error...\n");
    
    RCC_CLK_Init_Param param = { .CLK_Source = RCC_SYSCLK_SOURCE_HSI }; // HSI
    
    RETR_STAT result = RCC_CLK_Init(&param, NULL);
    
    assert(__ERROR_CHECK(result));
    printf("-> PASSED\n");
  }

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
    assert(MOCK_RCC_REGS.CR.HSEON == RESET); // Kiểm tra HSE đã tắt
    printf("-> PASSED\n");
  }

// Thực thi tất cả các test case

int main() {
    printf("\n--- CLOCK UNIT TEST ---\n");
    
    test_Init_NullPointer_ShouldReturnError();
    test_Init_HSI_Success_ShouldSetReadyFlag();
    test_Init_HSE_Success_ShouldSetReadyFlag();
    test_Init_LSI_Success_ShouldSetReadyFlag();
    test_Init_InvalidSource_ShouldReturnError();
    test_Init_HSE_NotReady_ShouldReturnError();
    test_Init_LSI_NotReady_ShouldReturnError();
    test_Init_NullRdyFlag_ShouldReturnError();
    test_DeInit_HSI_Busy_ShouldReturnBusy();
    test_DeInit_HSE_Success_ShouldTurnOffHSE();
    
    printf("----------------------------------------\n");
    printf("ALL TESTS PASSED!\n\n");
    return 0;
}
