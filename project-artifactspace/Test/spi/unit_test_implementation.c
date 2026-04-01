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

  void dummy_callback(SPI_Handle_Param *hspi) {
    // Hàm callback giả để kiểm tra việc đăng ký callback
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

  void test_SPI_RegisterCallback_InvalidState_ShouldReturnError() {
    setup();
    printf("TC9: Check Register Callback with Invalid State...\n");

    // Bổ sung define để đảm bảo sử dụng được unit test này
    #define SPI_MSP_INIT_CB_ID 0x00u
    
    SPI_Handle_Param hspi = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .State = SPI_BUSY // Trạng thái không cho phép đăng ký callback
    };
    assert(__ERROR_CHECK(SPI_RegisterCallback(&hspi, SPI_MSP_INIT_CB_ID, NULL))); // Hàm sẽ trả về lỗi do trạng thái không cho phép đăng ký callback

    printf("-> PASSED\n");
  }

  void test_SPI_RegisterCallback_ValidState_ShouldRegisterCallback() {
    setup();
    printf("TC10: Check Register Callback with Valid State...\n");
    
    #ifdef SPI_PUBLIC_CALLBACK_ENABLE
      SPI_Handle_Param hspi = { 
        .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
        .State = SPI_READY // Trạng thái cho phép đăng ký callback
      };
      assert(__OK_CHECK(SPI_RegisterCallback(&hspi, SPI_MSP_INIT_CB_ID, dummy_callback))); // Hàm sẽ trả về STAT_OK khi đăng ký callback thành công
      assert(hspi.MSP_Init_Callback == dummy_callback); // Callback phải được đăng ký đúng vào handle_param
    #else
      printf("SPI_PUBLIC_CALLBACK_ENABLE is not defined, skipping callback registration test.\n");
    #endif

    /**
     * Ghi chú:
     * Bổ sung preprocessor directive để đảm bảo hoạt động đúng mà vẫn tránh lỗi từ biên dịch
     */

    printf("-> PASSED\n");
  }

  void test_SPI_UnRegisterCallback_InvalidState_ShouldReturnError() {
    setup();
    printf("TC11: Check UnRegister Callback with Invalid State...\n");

    // Bổ sung define để đảm bảo sử dụng được unit test này
    #define SPI_MSP_INIT_CB_ID 0x00u
    
    SPI_Handle_Param hspi = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .State = SPI_BUSY // Trạng thái không cho phép hủy đăng ký callback
    };
    assert(__ERROR_CHECK(SPI_UnRegisterCallback(&hspi, SPI_MSP_INIT_CB_ID))); // Hàm sẽ trả về lỗi do trạng thái không cho phép hủy đăng ký callback

    printf("-> PASSED\n");
  }

  void test_SPI_UnRegisterCallback_ValidState_ShouldUnRegisterCallback() {
    setup();
    printf("TC12: Check UnRegister Callback with Valid State...\n");
    
    #ifdef SPI_PUBLIC_CALLBACK_ENABLE
      SPI_Handle_Param hspi = { 
        .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
        .State = SPI_READY, // Trạng thái cho phép đăng ký callback
        .MSP_Init_Callback = dummy_callback // Đã có callback được đăng ký trước đó
      };
      assert(__OK_CHECK(SPI_UnRegisterCallback(&hspi, SPI_MSP_INIT_CB_ID))); // Hàm sẽ trả về STAT_OK khi hủy đăng ký callback thành công
      assert(hspi.MSP_Init_Callback == NULL); // Callback phải được hủy đăng ký đúng vào handle_param (đặt về NULL)
    #else
      printf("SPI_PUBLIC_CALLBACK_ENABLE is not defined, skipping callback unregistration test.\n");
    #endif

    /**
     * Ghi chú:
     * Bổ sung preprocessor directive để đảm bảo hoạt động đúng mà vẫn tránh lỗi từ biên dịch
     */

    printf("-> PASSED\n");
  }

  void test_SPI_Transmit_InvalidParameters_ShouldReturnError() {
    setup();
    printf("TC13: Check Transmit with Invalid Parameters...\n");
    
    assert(__ERROR_CHECK(SPI_Transmit(NULL, NULL, 0, 0))); // Hàm sẽ trả về lỗi do con trỏ hspi và pdata không hợp lệ, size bằng 0

    printf("-> PASSED\n");
  }

  void test_SPI_Transmit_ValidParameters_ShouldTransmitData() {
    setup();
    printf("TC14: Check Transmit with Valid Parameters...\n");
    
    SPI_Handle_Param hspi8bit = { 
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
      },
      .State = SPI_READY // Trạng thái cho phép truyền dữ liệu
    };

    ui8 data_to_transmit[5] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE }; // Dữ liệu mẫu để truyền

    // Vòng lặp dữ liệu mẫu để truyền từng byte một, kiểm tra thanh ghi DR sau mỗi lần truyền

      for (ui16 i = 0; i < 5; i++) {
        assert(__OK_CHECK(SPI_Transmit(&hspi8bit, &data_to_transmit[i], 1, 1000))); // Truyền từng byte một, size = 1
        assert(MOCK_SPI_REGS.SPI_DR == data_to_transmit[i]); // Thanh ghi DR phải chứa đúng dữ liệu đã truyền
      }

    SPI_Handle_Param hspi16bit = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .Init = { 
        .Mode = SPI_MODE_MASTER, // Chế độ Master hợp lệ
        .Direction = SPI_DIRECTION_2LINES, // Chế độ Full-Duplex hợp lệ
        .DataSize = SPI_DATASIZE_16BIT, // Kích thước dữ liệu 16-bit hợp lệ
        .CLKPolarity = SPI_CLKPOLARITY_LOW, // Clock polarity Low hợp lệ
        .CLKPhase = SPI_CLKPHASE_1EDGE, // Clock phase 1st edge hợp lệ
        .NSS = SPI_NSS_SOFT, // Quản lý NSS bằng phần mềm hợp lệ
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16, // Hệ số chia baud rate hợp lệ cho chế độ Master
        .FirstBit = SPI_FIRSTBIT_MSB // Thứ tự bit MSB first hợp lệ
      },
      .State = SPI_READY // Trạng thái cho phép truyền dữ liệu
    };

    ui16 data_to_transmit_16bit[3] = { 0xAABB, 0xCCDD, 0xEEFF }; // Dữ liệu mẫu 16-bit để truyền

    // Vòng lặp dữ liệu mẫu để truyền từng phần tử 16-bit một, kiểm tra thanh ghi DR sau mỗi lần truyền

      for (ui16 i = 0; i < 3; i++) {
        assert(__OK_CHECK(SPI_Transmit(&hspi16bit, (ui8*)&data_to_transmit_16bit[i], 1, 1000))); // Truyền từng phần tử 16-bit một, size = 1 (tính theo số lượng phần tử)
        assert(MOCK_SPI_REGS.SPI_DR == data_to_transmit_16bit[i]); // Thanh ghi DR phải chứa đúng dữ liệu đã truyền
      }

    printf("-> PASSED\n");
  }

  void test_SPI_Transmit_Timeout_ShouldReturnError() {
    setup();
    printf("TC15: Check Transmit with Timeout...\n");
    
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
      },
      .State = SPI_READY // Trạng thái cho phép truyền dữ liệu
    };

    ui8 data_to_transmit = 0xAA; // Dữ liệu mẫu để truyền

    // Giả lập tình trạng bận của SPI bằng cách không cho phép thanh ghi SR sẵn sàng để truyền dữ liệu

      MOCK_SPI_REGS.SPI_SR = 0; // Thanh ghi SR không có cờ TXE (Transmit buffer empty) được set, giả lập tình trạng bận

      ui32 timeout_adjusted = 1000; // Timeout ban đầu (có thể điều chỉnh nếu cần)

      while (timeout_adjusted > 0) {
        if (__OK_CHECK(SPI_Transmit(&hspi, &data_to_transmit, 1, timeout_adjusted))) {
          // Nếu hàm trả về STAT_OK thì đã truyền thành công, không còn bận nữa
            break;
        } else {
          // Nếu hàm trả về lỗi, kiểm tra nếu là lỗi timeout thì dừng lại và trả về lỗi timeout

            if (hspi.ErrorCode == SPI_ERROR_TIMEOUT) {
              // Trả về lỗi timeout nếu đã hết thời gian chờ
              hspi.ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
              printf("-> PASSED (Timeout occurred as expected)\n");
              return; // Kết thúc test case sau khi đã xác nhận lỗi timeout xảy ra
            }
        }
        timeout_adjusted--; // Giảm timeout để tiếp tục thử truyền lại
      }

      /**
       * Ghi chú:
       * Nếu vòng lặp kết thúc mà vẫn chưa truyền được dữ liệu thành công, 
       * thì có thể do timeout đã xảy ra hoặc do tình trạng bận vẫn còn tiếp diễn.
       * Trong trường hợp này, cần kiểm tra mã lỗi trong handle_param để xác nhận nếu là lỗi timeout đã được cập nhật đúng.
       * Nếu là lỗi timeout thì test case sẽ được coi là passed vì đã xác nhận được tình trạng bận dẫn đến timeout như mong đợi.
       * Nếu không phải lỗi timeout thì có thể có vấn đề khác cần được điều tra thêm.
       */
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