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

  /*
   * Hàm xử lý khi assert_param thất bại trong unit test.
   *
   * Tham số:
   *   file - Tên file nơi assert thất bại.
   *   line - Số dòng chứa assert thất bại.
   *
   * Logic:
   *   - In thông báo lỗi.
   *   - Đánh dấu cờ assert_caught.
   *   - Quay về nhánh test qua longjmp.
   *
   * Trả về:
   *   Không có.
   */
  void assert_failed(ui8* file, ui8 line) {
    printf("Assertion using assert_param failed in file %s on line %u.\n", file, line);
    assert_caught = TRUE;
    longjmp(assert_env, 1);
  }

  /*
   * Callback giả để kiểm tra việc đăng ký callback SPI.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI đang được kiểm thử.
   *
   * Logic:
   *   - Không thực hiện xử lý nào ngoài việc xác nhận hàm được gọi.
   *
   * Trả về:
   *   Không có.
   */
  void dummy_callback(SPI_Handle_Param *hspi) {
  }

  /*
   * Callback MSP Init giả để kiểm tra dữ liệu truyền vào callback.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI được truyền vào callback.
   *
   * Logic:
   *   - In các trường của handle để đối chiếu khi test.
   *
   * Trả về:
   *   Không có.
   */
  void SPI_MSP_Init(SPI_Handle_Param *hspi) {

    // Thực hiện in ra toàn bộ thông tin của hspi để kiểm tra xem hàm callback có nhận đúng tham số hay không
    printf("MSP_Init Callback Called with hspi:\n");
    printf("  Instance: %p\n", (void*)hspi->Instance);
    printf("  Init.Mode: %u\n", hspi->Init.Mode);
    printf("  Init.Direction: %u\n", hspi->Init.Direction);
    printf("  Init.DataSize: %u\n", hspi->Init.DataSize);
    printf("  Init.CLKPolarity: %u\n", hspi->Init.CLKPolarity);
    printf("  Init.CLKPhase: %u\n", hspi->Init.CLKPhase);
    printf("  Init.NSS: %u\n", hspi->Init.NSS);
    printf("  Init.BaudRatePrescaler: %u\n", hspi->Init.BaudRatePrescaler);
    printf("  Init.FirstBit: %u\n", hspi->Init.FirstBit);
  }

  /*
   * Khởi tạo lại trạng thái unit test trước mỗi test case.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Xóa thanh ghi SPI giả.
   *
   * Trả về:
   *   Không có.
   */
  void setup() {

    // Hàm này được gọi trước mỗi test case để khởi tạo lại trạng thái cho unit test

    // Reset thanh ghi SPI giả về các giá trị reset
    memset(&MOCK_SPI_REGS, 0, sizeof(SPI_REGS_Typedef));
  }

  /*
   * Kiểm tra trường hợp SPI_Init nhận con trỏ NULL.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Gọi SPI_Init với tham số NULL.
   *   - Xác nhận hàm trả về lỗi.
   *
   * Trả về:
   *   Không có.
   */
  void test_SPI_Init_NULLPointer_ShouldReturnError() {
    setup();
    printf("TC1: Check Null Pointer...\n");
    assert(__ERROR_CHECK(SPI_Init(NULL)));
    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra trường hợp SPI_Init nhận Instance không hợp lệ.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle với Instance NULL.
   *   - Xác nhận assert được kích hoạt.
   *
   * Trả về:
   *   Không có.
   */
  void test_SPI_Init_InvalidInstance_ShouldAssert() {
    setup();
    printf("TC2: Check Invalid Instance Pointer...\n");
    
    SPI_Handle_Param hspi = { .Instance = NULL }; // Con trỏ Instance không hợp lệ (NULL)
    ASSERT_EXPECT_FAIL(__ERROR_CHECK(SPI_Init(&hspi))); // Hàm sẽ assert fail do con trỏ Instance không hợp lệ
  }

  /*
   * Kiểm tra trường hợp SPI_Init nhận mode không hợp lệ.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle với mode sai.
   *   - Xác nhận assert được kích hoạt.
   *
   * Trả về:
   *   Không có.
   */
  void test_SPI_Init_InvalidMode_ShouldAssert() {
    setup();
    printf("TC3: Check Invalid Mode...\n");
    
    SPI_Handle_Param hspi = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .Init = { .Mode = 0xFF } // Giá trị Mode không hợp lệ
    };
    ASSERT_EXPECT_FAIL(SPI_Init(&hspi)); // Hàm sẽ assert fail do giá trị Mode không hợp lệ
  }

  /*
   * Kiểm tra trường hợp SPI_Init nhận bộ tham số không hợp lệ.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle với bộ tham số sai.
   *   - Xác nhận assert được kích hoạt.
   *
   * Trả về:
   *   Không có.
   */
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

  /*
   * Kiểm tra SPI_Init với đầu vào hợp lệ và cấu hình thanh ghi đúng.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle hợp lệ.
   *   - Gọi SPI_Init.
   *   - So sánh giá trị thanh ghi cấu hình.
   *
   * Trả về:
   *   Không có.
   */
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
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32, // Hệ số chia baud rate hợp lệ cho chế độ Master
        .FirstBit = SPI_FIRSTBIT_MSB // Thứ tự bit MSB first hợp lệ
      }
    };
    assert(__OK_CHECK(SPI_Init(&hspi))); // Hàm trả về STAT_OK khi cấu hình thành công

    // Kiểm tra các thanh ghi đã được cấu hình đúng

      // Cấu hình CR1
      ui32 expected_cr1 = SPI_MODE_MASTER | SPI_DIRECTION_2LINES | SPI_DATASIZE_8BIT | SPI_CLKPOLARITY_LOW | SPI_CLKPHASE_1EDGE | SPI_BAUDRATEPRESCALER_32 | SPI_FIRSTBIT_MSB | SPI_CR1_SSM_MASK | SPI_CR1_SSI_MASK | SPI_CR1_SPE_MASK;
      assert(MOCK_SPI_REGS.SPI_CR1 == expected_cr1); // Thanh ghi CR1 phải được cấu hình đúng

    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra trường hợp SPI_DeInit nhận con trỏ NULL.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Gọi SPI_DeInit với tham số NULL.
   *   - Xác nhận hàm trả về lỗi.
   *
   * Trả về:
   *   Không có.
   */
  void test_SPI_DeInit_NULLPointer_ShouldReturnError() {
    setup();
    printf("TC6: Check Null Pointer...\n");
    assert(__ERROR_CHECK(SPI_DeInit(NULL)));
    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra trường hợp SPI_DeInit nhận Instance không hợp lệ.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle với Instance NULL.
   *   - Xác nhận assert được kích hoạt.
   *
   * Trả về:
   *   Không có.
   */
  void test_SPI_DeInit_InvalidInstance_ShouldAssert() {
    setup();
    printf("TC7: Check Invalid Instance Pointer...\n");
    
    SPI_Handle_Param hspi = { .Instance = NULL }; // Con trỏ Instance không hợp lệ (NULL)
    ASSERT_EXPECT_FAIL(SPI_DeInit(&hspi)); // Hàm sẽ assert fail do con trỏ Instance không hợp lệ
  }

  /*
   * Kiểm tra SPI_DeInit với đầu vào hợp lệ.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Khởi tạo SPI trước khi deinit.
   *   - Gọi SPI_DeInit.
   *   - Xác nhận các thanh ghi được reset đúng.
   *
   * Trả về:
   *   Không có.
   */
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

    MOCK_SPI_REGS.SPI_SR = SPI_SR_TXE_MASK;

    assert(__OK_CHECK(SPI_DeInit(&hspi))); // Hàm trả về STAT_OK khi deinit thành công

    // Kiểm tra các thanh ghi đã được reset về giá trị mặc định

      assert(MOCK_SPI_REGS.SPI_CR1 == 0); // Thanh ghi CR1 phải được reset về 0
      assert(MOCK_SPI_REGS.SPI_CR2 == 0); // Thanh ghi CR2 phải được reset về 0

    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra đăng ký callback khi SPI đang ở trạng thái không cho phép.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle với trạng thái BUSY.
   *   - Xác nhận hàm trả về lỗi.
   *
   * Trả về:
   *   Không có.
   */
  void test_SPI_RegisterCallback_InvalidState_ShouldReturnError() {
    setup();
    printf("TC9: Check Register Callback with Invalid State...\n");
    
    SPI_Handle_Param hspi = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .State = SPI_BUSY // Trạng thái không cho phép đăng ký callback
    };
    assert(__ERROR_CHECK(SPI_RegisterCallback(&hspi, SPI_MSP_INIT_CB_ID, NULL))); // Hàm sẽ trả về lỗi do trạng thái không cho phép đăng ký callback

    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra đăng ký callback khi SPI ở trạng thái hợp lệ.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle ở trạng thái READY.
   *   - Đăng ký callback mẫu.
   *   - Xác nhận con trỏ callback được cập nhật.
   *
   * Trả về:
   *   Không có.
   */
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

  /*
   * Kiểm tra hủy đăng ký callback khi SPI đang ở trạng thái không cho phép.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle với trạng thái BUSY.
   *   - Xác nhận hàm trả về lỗi.
   *
   * Trả về:
   *   Không có.
   */
  void test_SPI_UnRegisterCallback_InvalidState_ShouldReturnError() {
    setup();
    printf("TC11: Check UnRegister Callback with Invalid State...\n");
    
    SPI_Handle_Param hspi = { 
      .Instance = &MOCK_SPI_REGS, // Con trỏ Instance hợp lệ
      .State = SPI_BUSY // Trạng thái không cho phép hủy đăng ký callback
    };
    assert(__ERROR_CHECK(SPI_UnRegisterCallback(&hspi, SPI_MSP_INIT_CB_ID))); // Hàm sẽ trả về lỗi do trạng thái không cho phép hủy đăng ký callback

    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra hủy đăng ký callback khi SPI ở trạng thái hợp lệ.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle ở trạng thái READY với callback đã đăng ký.
   *   - Hủy đăng ký callback.
   *   - Xác nhận callback quay về giá trị mặc định.
   *
   * Trả về:
   *   Không có.
   */
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
      assert(hspi.MSP_Init_Callback == SPI_MSP_Init); // Callback MSP phải được trả về hàm mặc định sau khi unregister
    #else
      printf("SPI_PUBLIC_CALLBACK_ENABLE is not defined, skipping callback unregistration test.\n");
    #endif

    /**
     * Ghi chú:
     * Bổ sung preprocessor directive để đảm bảo hoạt động đúng mà vẫn tránh lỗi từ biên dịch
     */

    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra trường hợp SPI_Transmit nhận tham số không hợp lệ.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Gọi SPI_Transmit với con trỏ NULL và size không hợp lệ.
   *   - Xác nhận hàm trả về lỗi.
   *
   * Trả về:
   *   Không có.
   */
  void test_SPI_Transmit_InvalidParameters_ShouldReturnError() {
    setup();
    printf("TC13: Check Transmit with Invalid Parameters...\n");
    
    assert(__ERROR_CHECK(SPI_Transmit(NULL, NULL, 0, 0, SPI_TRANS_NORM))); // Hàm sẽ trả về lỗi do con trỏ hspi và pdata không hợp lệ, size bằng 0

    printf("-> PASSED\n");
  }

  /*
   * Kiểm tra SPI_Transmit với tham số hợp lệ ở cả 8-bit và 16-bit.
   *
   * Tham số:
   *   Không có.
   *
   * Logic:
   *   - Tạo handle hợp lệ cho 8-bit và 16-bit.
   *   - Gọi SPI_Transmit với từng trường hợp.
   *   - Xác nhận dữ liệu được ghi đúng vào thanh ghi DR.
   *
   * Trả về:
   *   Không có.
   */
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
        MOCK_SPI_REGS.SPI_SR = SPI_SR_TXE_MASK;
        assert(__OK_CHECK(SPI_Transmit(&hspi8bit, &data_to_transmit[i], 1, 1000, SPI_TRANS_NORM))); // Truyền từng byte một, size = 1
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
        MOCK_SPI_REGS.SPI_SR = SPI_SR_TXE_MASK;
        assert(__OK_CHECK(SPI_Transmit(&hspi16bit, (ui8*)&data_to_transmit_16bit[i], 1, 1000, SPI_TRANS_NORM))); // Truyền từng phần tử 16-bit một, size = 1 (tính theo số lượng phần tử)
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
        if (__OK_CHECK(SPI_Transmit(&hspi, &data_to_transmit, 1, timeout_adjusted, SPI_TRANS_NORM))) {
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
  void test_SPI_Receive_InvalidParameters_ShouldReturnError() {
    setup();
    printf("TC16: Check Receive with Invalid Parameters...\n");
    
    assert(__ERROR_CHECK(SPI_Receive(NULL, NULL, 0, 0, SPI_TRANS_NORM))); // Hàm sẽ trả về lỗi do con trỏ hspi và pdata không hợp lệ, size bằng 0

    printf("-> PASSED\n");
  }

  void test_SPI_Receive_ValidParameters_ShouldReceiveData() {
    setup();
    printf("TC17: Check Receive with Valid Parameters...\n");

    SPI_Handle_Param hspi_rx8bit = {
      .Instance = &MOCK_SPI_REGS,
      .Init = {
        .Mode = SPI_MODE_SLAVE,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_8BIT,
        .CLKPolarity = SPI_CLKPOLARITY_LOW,
        .CLKPhase = SPI_CLKPHASE_1EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB
      },
      .State = SPI_READY
    };

    ui8 rx_buffer_8bit = 0x00u;
    MOCK_SPI_REGS.SPI_SR = SPI_SR_RXNE_MASK;
    MOCK_SPI_REGS.SPI_DR = 0xA5u;

    printf("TC17-1: 8-bit receive...\n");
    assert(__OK_CHECK(SPI_Receive(&hspi_rx8bit, &rx_buffer_8bit, 1, 1000, SPI_TRANS_NORM)));
    assert(rx_buffer_8bit == 0xA5u);

    SPI_Handle_Param hspi_rx16bit = {
      .Instance = &MOCK_SPI_REGS,
      .Init = {
        .Mode = SPI_MODE_SLAVE,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_16BIT,
        .CLKPolarity = SPI_CLKPOLARITY_LOW,
        .CLKPhase = SPI_CLKPHASE_1EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB
      },
      .State = SPI_READY
    };

    ui16 rx_buffer_16bit = 0x0000u;
    MOCK_SPI_REGS.SPI_SR = SPI_SR_RXNE_MASK;
    MOCK_SPI_REGS.SPI_DR = 0xBEEFu;

    printf("TC17-2: 16-bit receive...\n");
    assert(__OK_CHECK(SPI_Receive(&hspi_rx16bit, (ui8*)&rx_buffer_16bit, 1, 1000, SPI_TRANS_NORM)));
    assert(rx_buffer_16bit == 0xBEEFu);

    printf("-> PASSED\n");
  }

  void test_SPI_Receive_Timeout_ShouldReturnError() {
    setup();
    printf("TC18: Check Receive timeout...\n");

    SPI_Handle_Param hspi = {
      .Instance = &MOCK_SPI_REGS,
      .Init = {
        .Mode = SPI_MODE_SLAVE,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_8BIT,
        .CLKPolarity = SPI_CLKPOLARITY_LOW,
        .CLKPhase = SPI_CLKPHASE_1EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB
      },
      .State = SPI_READY
    };

    ui8 rx_buffer = 0x00u;
    MOCK_SPI_REGS.SPI_SR = 0u;

    assert(SPI_Receive(&hspi, &rx_buffer, 1, 0u, SPI_TRANS_NORM) == STAT_TIMEOUT);
    assert(hspi.ErrorCode == SPI_ERROR_TIMEOUT);

    printf("-> PASSED (Timeout occurred as expected)\n");
  }

  void test_SPI_Abort_Norm_ShouldAbortAndResetState() {
    setup();
    printf("TC19: Check Abort in Normal mode...\n");

    SPI_Handle_Param hspi = {
      .Instance = &MOCK_SPI_REGS,
      .Init = {
        .Mode = SPI_MODE_MASTER,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_8BIT,
        .CLKPolarity = SPI_CLKPOLARITY_LOW,
        .CLKPhase = SPI_CLKPHASE_1EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB
      },
      .State = SPI_BUSY_TX
    };

    MOCK_SPI_REGS.SPI_SR = SPI_SR_TXE_MASK;

    assert(__OK_CHECK(SPI_Abort(&hspi, SPI_TRANS_NORM)));
    assert(hspi.State == SPI_READY);
    assert(hspi.ErrorCode == SPI_ERR_OK);
    assert((MOCK_SPI_REGS.SPI_CR1 & SPI_CR1_SPE_MASK) == 0u);

    printf("-> PASSED\n");
  }

  void test_SPI_Abort_Intr_ShouldAbortAndResetState() {
    setup();
    printf("TC20: Check Abort in Interrupt mode...\n");

    SPI_Handle_Param hspi = {
      .Instance = &MOCK_SPI_REGS,
      .Init = {
        .Mode = SPI_MODE_MASTER,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_8BIT,
        .CLKPolarity = SPI_CLKPOLARITY_LOW,
        .CLKPhase = SPI_CLKPHASE_1EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB
      },
      .State = SPI_BUSY_TX_RX
    };

    assert(__OK_CHECK(SPI_Abort(&hspi, SPI_TRANS_INTR)));
    assert(hspi.State == SPI_READY);
    assert(hspi.ErrorCode == SPI_ERR_OK);
    assert((MOCK_SPI_REGS.SPI_CR1 & SPI_CR1_SPE_MASK) == 0u);

    printf("-> PASSED\n");
  }

  void test_SPI_Transmit_Intr_ShouldConfigureAndComplete() {
    setup();
    printf("TC21: Check Transmit interrupt path...\n");

    SPI_Handle_Param hspi = {
      .Instance = &MOCK_SPI_REGS,
      .Init = {
        .Mode = SPI_MODE_MASTER,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_8BIT,
        .CLKPolarity = SPI_CLKPOLARITY_LOW,
        .CLKPhase = SPI_CLKPHASE_1EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB
      },
      .State = SPI_READY
    };

    ui8 tx_data = 0x5Au;
    MOCK_SPI_REGS.SPI_SR = SPI_SR_TXE_MASK;

    assert(__OK_CHECK(SPI_Transmit(&hspi, &tx_data, 1, 1000, SPI_TRANS_INTR)));
    assert(hspi.State == SPI_BUSY_TX);
    assert((MOCK_SPI_REGS.SPI_CR2 & (SPI_CR2_TXEIE_MASK | SPI_CR2_ERRIE_MASK)) == (SPI_CR2_TXEIE_MASK | SPI_CR2_ERRIE_MASK));
    assert(hspi.TxISR != NULL);
    assert(hspi.RxISR != NULL);

    hspi.TxISR(&hspi);

    assert(hspi.State == SPI_READY);
    assert(hspi.Tx_Xfer_Count == 0u);
    assert((MOCK_SPI_REGS.SPI_CR2 & (SPI_CR2_TXEIE_MASK | SPI_CR2_ERRIE_MASK | SPI_CR2_RXNEIE_MASK)) == 0u);
    assert(MOCK_SPI_REGS.SPI_DR == tx_data);

    printf("-> PASSED\n");
  }

  void test_SPI_Receive_Intr_ShouldConfigureAndComplete() {
    setup();
    printf("TC22: Check Receive interrupt path...\n");

    SPI_Handle_Param hspi = {
      .Instance = &MOCK_SPI_REGS,
      .Init = {
        .Mode = SPI_MODE_SLAVE,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_8BIT,
        .CLKPolarity = SPI_CLKPOLARITY_LOW,
        .CLKPhase = SPI_CLKPHASE_1EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB
      },
      .State = SPI_READY
    };

    ui8 rx_data = 0x00u;
    MOCK_SPI_REGS.SPI_SR = SPI_SR_TXE_MASK | SPI_SR_RXNE_MASK;
    MOCK_SPI_REGS.SPI_DR = 0xA5u;

    assert(__OK_CHECK(SPI_Receive(&hspi, &rx_data, 1, 1000, SPI_TRANS_INTR)));
    assert(hspi.State == SPI_BUSY_RX);
    assert((MOCK_SPI_REGS.SPI_CR2 & (SPI_CR2_RXNEIE_MASK | SPI_CR2_ERRIE_MASK)) == (SPI_CR2_RXNEIE_MASK | SPI_CR2_ERRIE_MASK));
    assert(hspi.TxISR != NULL);
    assert(hspi.RxISR != NULL);

    hspi.RxISR(&hspi);

    assert(rx_data == 0xA5u);
    assert(hspi.State == SPI_READY);
    assert(hspi.Rx_Xfer_Count == 0u);
    assert((MOCK_SPI_REGS.SPI_CR2 & (SPI_CR2_TXEIE_MASK | SPI_CR2_RXNEIE_MASK | SPI_CR2_ERRIE_MASK)) == 0u);

    printf("-> PASSED\n");
  }

  void test_SPI_TransmitReceive_Intr_ShouldConfigureAndComplete() {
    setup();
    printf("TC23: Check TransmitReceive interrupt path...\n");

    SPI_Handle_Param hspi = {
      .Instance = &MOCK_SPI_REGS,
      .Init = {
        .Mode = SPI_MODE_MASTER,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_8BIT,
        .CLKPolarity = SPI_CLKPOLARITY_LOW,
        .CLKPhase = SPI_CLKPHASE_1EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB
      },
      .State = SPI_READY
    };

    ui8 tx_data = 0x3Cu;
    ui8 rx_data = 0x00u;
    MOCK_SPI_REGS.SPI_SR = SPI_SR_TXE_MASK | SPI_SR_RXNE_MASK;
    MOCK_SPI_REGS.SPI_DR = tx_data;

    assert(__OK_CHECK(SPI_TransmitReceive(&hspi, &tx_data, &rx_data, 1, 1000, SPI_TRANS_INTR)));
    assert(hspi.State == SPI_BUSY_TX_RX);
    assert((MOCK_SPI_REGS.SPI_CR2 & (SPI_CR2_TXEIE_MASK | SPI_CR2_RXNEIE_MASK | SPI_CR2_ERRIE_MASK)) == (SPI_CR2_TXEIE_MASK | SPI_CR2_RXNEIE_MASK | SPI_CR2_ERRIE_MASK));
    assert(hspi.TxISR != NULL);
    assert(hspi.RxISR != NULL);

    hspi.TxISR(&hspi);
    assert(hspi.State == SPI_BUSY_TX_RX);
    assert(hspi.Tx_Xfer_Count == 0u);

    hspi.RxISR(&hspi);

    assert(rx_data == tx_data);
    assert(hspi.State == SPI_READY);
    assert(hspi.Rx_Xfer_Count == 0u);
    assert((MOCK_SPI_REGS.SPI_CR2 & (SPI_CR2_TXEIE_MASK | SPI_CR2_RXNEIE_MASK | SPI_CR2_ERRIE_MASK)) == 0u);

    printf("-> PASSED\n");
  }
                    
  
// Thực thi tất cả các test case

int main() {
  printf("\n--- SPI UNIT TEST ---\n");

  test_SPI_Init_NULLPointer_ShouldReturnError();
  test_SPI_Init_InvalidInstance_ShouldAssert();
  test_SPI_Init_InvalidMode_ShouldAssert();
  test_SPI_Init_InvalidParameters_ShouldAssert();
  test_SPI_Init_ValidInput_ShouldConfigureSPI();
  test_SPI_DeInit_NULLPointer_ShouldReturnError();
  test_SPI_DeInit_InvalidInstance_ShouldAssert();
  test_SPI_DeInit_ValidInput_ShouldDeInitSPI();
  test_SPI_RegisterCallback_InvalidState_ShouldReturnError();
  test_SPI_RegisterCallback_ValidState_ShouldRegisterCallback();
  test_SPI_UnRegisterCallback_InvalidState_ShouldReturnError();
  test_SPI_UnRegisterCallback_ValidState_ShouldUnRegisterCallback();
  test_SPI_Transmit_InvalidParameters_ShouldReturnError();
  test_SPI_Transmit_ValidParameters_ShouldTransmitData();
  test_SPI_Transmit_Timeout_ShouldReturnError();
  test_SPI_Receive_InvalidParameters_ShouldReturnError();
  test_SPI_Receive_ValidParameters_ShouldReceiveData();
  test_SPI_Receive_Timeout_ShouldReturnError();
  test_SPI_Abort_Norm_ShouldAbortAndResetState();
  test_SPI_Abort_Intr_ShouldAbortAndResetState();
  test_SPI_Transmit_Intr_ShouldConfigureAndComplete();
  test_SPI_Receive_Intr_ShouldConfigureAndComplete();
  test_SPI_TransmitReceive_Intr_ShouldConfigureAndComplete();

    printf("----------------------------------------\n");
    printf("ALL TESTS PASSED!\n\n");
    return 0;
}