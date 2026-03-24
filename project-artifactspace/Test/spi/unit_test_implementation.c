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
  #include "lib_spi_def.h"
  #include "lib_spi_hal.h"
  #include "header_dependency.h"

// Khai báo ngoại vi giả cho mục đích unit test

  SPI_REGS_Typedef MOCK_SPI_REGS;

// Khai báo cờ ngữ cảnh cho việc bắt assert trong unit test

  jmp_buf assert_env;
  ui8 assert_caught = FALSE; // Cờ để theo dõi xem assert fail đã được bắt hay chưa

// Định nghĩa các hàm 

  void assert_failed(ui8* file, ui8 line) {
    printf("Assertion using assert_param failed in file %s on line %u.\n", file, line);
    assert_caught = TRUE;
    longjmp(assert_env, 1);
  }

  void test_SPI_Init_NULLPointer_ShouldReturnError() {
    setup();
    printf("TC1: Check Null Pointer...\n");
    assert(__ERROR_CHECK(SPI_Init(NULL)));
    printf("-> PASSED\n");
  }

  void test_SPI_Init_InvalidInstance_ShouldAssert() {
    setup();
    printf("TC2: Check Invalid Instance Pointer...\n");
    
    SPI_Handle_Param hspi = { .Instance = NULL }; // Con trỏ Instance không hợp lệ (NULL)
    ASSERT_EXPECT_FAIL(SPI_Init(&hspi)); // Hàm sẽ assert fail do con trỏ Instance không hợp lệ
  }

  void test_SPI_Init_InvalidMode_ShouldAssert() {
    setup();
    printf("TC3: Check Invalid Mode...\n");
    
    SPI_Handle_Param hspi = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .Init = { .Mode = 0xFF } // Giá trị Mode không hợp lệ
    };
    ASSERT_EXPECT_FAIL(SPI_Init(&hspi)); // Hàm sẽ assert fail do giá trị Mode không hợp lệ
  }

  void test_SPI_Init_InvalidParameters_ShouldAssert() {
    setup();
    printf("TC4: Check Invalid Parameters...\n");
    
    SPI_Handle_Param hspi = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .Init = { 
        .Mode = SPI_MODE_MASTER, // Chế độ Master hợp lệ
        .BaudRatePrescaler = 0xFF // Giá trị BaudRatePrescaler không hợp lệ cho chế độ Master
      }
    };
    ASSERT_EXPECT_FAIL(SPI_Init(&hspi)); // Hàm sẽ assert fail do giá trị BaudRatePrescaler không hợp lệ cho chế độ Master
  }

  void test_SPI_Init_ValidInput_ShouldConfigureSPI() {
    setup();
    printf("TC5: Valid Input -> Configure SPI...\n");
    
    SPI_Handle_Param hspi = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .Init = { 
        .Mode = SPI_MODE_MASTER, // Chế độ Master hợp lệ
        .Direction = SPI_DIRECTION_2LINES, // Chế độ Full-Duplex hợp lệ
        .DataSize = SPI_DATASIZE_8BIT, // Kích thước dữ liệu 8-bit hợp lệ
        .CLKPolarity = SPI_CLKPOLARITY_LOW, // Clock polarity Low hợp lệ
        .CLKPhase = SPI_CLKPHASE_1EDGE, // Clock phase 1st edge hợp lệ
        .NSS = SPI_NSS_SOFT, // Quản lý NSS bằng phần mềm hợp lệ
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16, // Hệ số chia baud rate hợp lệ cho chế độ Master
        .FirstBit = SPI_FIRSTBIT_MSB // Thứ tự bit MSB first hợp lệ
      }
    };
    assert(__DONE_CHECK(SPI_Init(&hspi))); // Hàm trả về STAT_OK khi cấu hình thành công

    // Kiểm tra các thanh ghi đã được cấu hình đúng

      // Cấu hình CR1
      ui32 expected_cr1 = SPI_MODE_MASTER | SPI_DIRECTION_2LINES | SPI_DATASIZE_8BIT | SPI_CLKPOLARITY_LOW | SPI_CLKPHASE_1EDGE | SPI_BAUDRATEPRESCALER_16 | SPI_FIRSTBIT_MSB | SPI_CR1_SSM_MASK | SPI_CR1_SSI_MASK;
      assert(MOCK_SPI_REGS.SPI_CR1 == expected_cr1); // Thanh ghi CR1 phải được cấu hình đúng

    printf("-> PASSED\n");
  }

  void setup() {

    // Hàm này được gọi trước mỗi test case để khởi tạo lại trạng thái cho unit test

    // Reset thanh ghi SPI giả về các giá trị reset
    memset(&MOCK_SPI_REGS, 0, sizeof(SPI_REGS_Typedef));
  }

  void test_SPI_DeInit_NULLPointer_ShouldReturnError() {
    setup();
    printf("TC6: Check Null Pointer...\n");
    assert(__ERROR_CHECK(SPI_DeInit(NULL)));
    printf("-> PASSED\n");
  }

  void test_SPI_DeInit_InvalidInstance_ShouldAssert() {
    setup();
    printf("TC7: Check Invalid Instance Pointer...\n");
    
    SPI_Handle_Param hspi = { .Instance = NULL }; // Con trỏ Instance không hợp lệ (NULL)
    ASSERT_EXPECT_FAIL(SPI_DeInit(&hspi)); // Hàm sẽ assert fail do con trỏ Instance không hợp lệ
  }

  void test_SPI_DeInit_ValidInput_ShouldDeInitSPI() {
    setup();
    printf("TC8: Valid Input -> DeInit SPI...\n");
    
    SPI_Handle_Param hspi = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .Init = { 
        .Mode = SPI_MODE_MASTER, // Chế độ Master hợp lệ
        .Direction = SPI_DIRECTION_2LINES, // Chế độ Full-Duplex hợp lệ
        .DataSize = SPI_DATASIZE_8BIT, // Kích thước dữ liệu 8-bit hợp lệ
        .CLKPolarity = SPI_CLKPOLARITY_LOW, // Clock polarity Low hợp lệ
        .CLKPhase = SPI_CLKPHASE_1EDGE, // Clock phase 1st edge hợp lệ
        .NSS = SPI_NSS_SOFT, // Quản lý NSS bằng phần mềm hợp lệ
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16, // Hệ số chia baud rate hợp lệ cho chế độ Master
        .FirstBit = SPI_FIRSTBIT_MSB // Thứ tự bit MSB first hợp lệ
      }
    };
    assert(__OK_CHECK(SPI_Init(&hspi))); // Đầu tiên khởi tạo thành công để có thể deinit

    assert(__OK_CHECK(SPI_DeInit(&hspi))); // Hàm trả về STAT_OK khi deinit thành công

    // Kiểm tra các thanh ghi đã được reset về giá trị mặc định

      assert(MOCK_SPI_REGS.SPI_CR1 == 0); // Thanh ghi CR1 phải được reset về 0
      assert(MOCK_SPI_REGS.SPI_CR2 == 0); // Thanh ghi CR2 phải được reset về 0

    printf("-> PASSED\n");
  }
  

// Thực thi tất cả các test case

int main() {
    printf("\n--- NVIC UNIT TEST ---\n");
    
    
    printf("\n");
    
    printf("\n");
    
    printf("\n");

    printf("\n");
    
    printf("\n");
    
    printf("\n");
    
    printf("\n");
    
    printf("\n");
    
    printf("\n");

    printf("\n");

    printf("\n");

    printf("----------------------------------------\n");
    printf("ALL TESTS PASSED!\n\n");
    return 0;
}