/*
 * lib_spi_impl.c
 *
 *  Created on: Mar 15, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"
    #include "lib_systick_def.h"
    #include "lib_systick_hal.h"
    #include "lib_spi_def.h"
    #include "lib_spi_hal.h"
    #include "lib_clock_def.h"
    #include "lib_clock_hal.h"
    #include "header_dependency.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
  #include <string.h>

  #ifndef UNIT_TEST
    #include "generic/lib_keyword_def.h"
    #include "generic/lib_condition_def.h"
    #include "systick/lib_systick_def.h"
    #include "systick/lib_systick_hal.h"
    #include "spi/lib_spi_def.h"
    #include "spi/lib_spi_hal.h"
    #include "clock/lib_clock_def.h"
    #include "clock/lib_clock_hal.h"
  #endif

// Khai báo hàm nội bộ

  static void SPI_WaitBeforeDisable(SPI_Handle_Param *hspi);
  
  static RETR_STAT SPI_WaitFlag(
    SPI_Handle_Param *hspi, 
    ui32 flag_mask, // Mặt nạ bit của cờ cần chờ
    ui32 desired_status, // Trạng thái mong muốn của cờ sau khi chờ (0 hoặc != 0)
    ui32 timeout, // Thời gian chờ tối đa tính bằng ms
    ui32 tickstart // Thời điểm bắt đầu chờ tính bằng tick của SysTick
  );

// Định nghĩa các hàm thành phần

  static void SPI_WaitBeforeDisable(SPI_Handle_Param *hspi) {
    ui32 direction = hspi->Init.Direction;
    ui32 mode = hspi->Init.Mode;

    // Trường hợp 1 & 2: Full-Duplex hoặc Transmit-only (2 dây hoặc 1 dây TX)

      if (
        direction == SPI_DIRECTION_2LINES 
        || 
        direction == SPI_DIRECTION_1LINE_TX
      ) {
        // 1. Đợi TXE = 1 (Bộ đệm truyền trống)
        while (!READ_BIT(hspi->Instance->SPI_SR, SPI_SR_TXE_MASK));
        // 2. Đợi BSY = 0 (Giao tiếp vật lý kết thúc)
        while (READ_BIT(hspi->Instance->SPI_SR, SPI_SR_BSY_MASK));
      }
    
    // Trường hợp 3: Master Receive-only (2 dây RXONLY hoặc 1 dây RX)

      else if (
        mode == SPI_MODE_MASTER 
        && 
        (
          direction == SPI_DIRECTION_2LINES_RXONLY 
          || 
          direction == SPI_DIRECTION_1LINE_RX
        )
      ) {
        /** 
         * Quy trình đặc biệt: Tắt SPE ngay sau khi nhận byte n-1.
         * Tuy nhiên, trong hàm DeInit tổng quát, ta thường đợi byte cuối cùng xong 
         * để tránh để lại dữ liệu rác cho lần khởi động sau.
         */
        while (!READ_BIT(hspi->Instance->SPI_SR, SPI_SR_RXNE_MASK));
        while (READ_BIT(hspi->Instance->SPI_SR, SPI_SR_BSY_MASK));
      }
    
    // Trường hợp 4: Slave Receive-only

      else {
        while (READ_BIT(hspi->Instance->SPI_SR, SPI_SR_BSY_MASK));
      }
  }

  static RETR_STAT SPI_WaitFlag(
    SPI_Handle_Param *hspi, 
    ui32 flag_mask, // Mặt nạ bit của cờ cần chờ
    ui32 desired_status, // Trạng thái mong muốn của cờ sau khi chờ (0 hoặc != 0)
    ui32 timeout, // Thời gian chờ tối đa tính bằng ms
    ui32 tickstart // Thời điểm bắt đầu chờ tính bằng tick của SysTick
  ) {

    // Khai báo biến điều chỉnh timeout

      __vo ui32 timeout_adjusted = 0u;
      ui32 elapsed = 0u;

    // Điều chỉnh timeout để bảo vệ khi Systick bị tắt

      /**
       * Ghi chú:
       * Điều chỉnh timeout để làm 1 Tick counter dự phòng khi Systick bị tắt, 
       * đảm bảo rằng hàm sẽ không bị treo vô hạn nếu Systick không hoạt động
       */

      timeout_adjusted = timeout * (1000u * 32u); 

    // Vòng lặp chờ cờ đạt trạng thái mong muốn hoặc timeout

      while ( // Đọc kiểm tra xem thanh ghi và mask có khớp với mask và trạng thái mong muốn hay không
        ((READ_BIT(hspi->Instance->SPI_SR, flag_mask) == flag_mask) ? SET: RESET) != desired_status
      ) {
        if (timeout != SYSTICK_LOAD_MAX_RELOAD_VALUE) {
          elapsed = SYSTICK_GetTick() - tickstart;
          if (
            (elapsed >= timeout)
            ||
            (timeout == 0u) // Trường hợp timeout ban đầu là 0, tức là không chờ, sẽ trả về timeout ngay lập tức
            ||
            (timeout_adjusted == 0u)
          ) {
            
            // Tắt SPI

              CLEAR_BIT(
                hspi->Instance->SPI_CR1,
                SPI_CR1_SPE_MASK
              );

            // Cập nhật trạng thái

              hspi->State = SPI_READY;

            // Cập nhật mã lỗi

              hspi->ErrorCode = SPI_ERROR_TIMEOUT;

            // Trả về lỗi timeout

              return STAT_TIMEOUT;

          }

          /**
           * Ghi chú:
           * Nếu Systick không hoạt động thì tắt timeout để trả về lỗi timeout ngay lập tức,
           */
          timeout_adjusted--;
        }
      }

    return STAT_OK;
  }

  RETR_STAT SPI_Init(SPI_Handle_Param *hspi) {

    // Kiểm tra tham số đầu vào hợp lệ

      if (hspi == NULL) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị các tham số hợp lệ

      assert_param(hspi->Instance != NULL); // Kiểm tra con trỏ tới bộ thanh ghi của ngoại vi SPI hợp lệ

      /**
       * Ghi chú:
       * Ở đây chỉ sử dụng SPI1 nên chỉ cần kiểm tra khác NULL là đủ.
       */

      assert_param(IS_SPI_MODE(hspi->Init.Mode)); 
      if (hspi->Init.Mode == SPI_MODE_MASTER) {
        assert_param(IS_SPI_BAUDRATEPRESCALER(hspi->Init.BaudRatePrescaler));
      } else {
        hspi->Init.BaudRatePrescaler = 0; 
        /**
         * Ghi chú:
         * - Chỉ khi ở chế độ Master mới cần kiểm tra giá trị BaudRatePrescaler 
         * vì chỉ Master mới có thể cấu hình baud rate.
         * - Ở chế độ Slave, BaudRatePrescaler sẽ không có ý nghĩa 
         * và sẽ được đặt mặc định là 0 để tránh gây nhầm lẫn 
         * hoặc lỗi khi sử dụng trong quá trình truyền nhận dữ liệu.
         */
      }

      assert_param(IS_SPI_DIRECTION(hspi->Init.Direction));
      assert_param(IS_SPI_DATASIZE(hspi->Init.DataSize));
      assert_param(IS_SPI_CPOL(hspi->Init.CLKPolarity));
      assert_param(IS_SPI_CPHA(hspi->Init.CLKPhase));
      assert_param(IS_SPI_NSS(hspi->Init.NSS));
      assert_param(IS_SPI_FIRSTBIT(hspi->Init.FirstBit));

      /**
       * Ghi chú:
       * Lưu ý rằng trong HAL của SPI,
       * sẽ có bổ sung thêm phần Lock để đảm bảo thread safety,
       * Lock này đảm bảo 1 thread truy cập vào SPI sẽ khóa tài nguyên này lại.
       * Tuy nhiên Lock này không phải là phần bắt buộc mà có thể sử dụng 
       * Mutex hoặc Semaphore với RTOS để quản lý truy cập tài nguyên SPI 
       * một cách hiệu quả hơn trong môi trường đa nhiệm.
       * Do đó, trong phần này sẽ không sử dụng Lock nội bộ 
       * mà sẽ để người dùng tự quản lý việc truy cập tài nguyên SPI 
       * thông qua cơ chế đồng bộ của RTOS nếu cần thiết.
       */
      
      #if (SPI_PUBLIC_CALLBACK_ENABLE == 1U)
        
        /**
         * Ghi chú:
         * Ở đây sẽ khởi tạo tất cả các con trỏ hàm callback 
         * về NULL để đảm bảo rằng nếu người dùng không đăng ký callback nào 
         * thì sẽ không có lỗi khi gọi hàm callback.
         * Thực hiện register callback sẽ được thực hiện thông qua hàm SPI_RegisterCallback
         * nếu người dùng muốn sử dụng callback thay vì hàm MSP_Init mặc định.
         */

        hspi->MSP_DeInit_Callback = NULL;
        hspi->Tx_Cplt_Callback = NULL;
        hspi->Rx_Cplt_Callback = NULL;
        hspi->TxRx_Cplt_Callback = NULL;
        hspi->Tx_HalfCplt_Callback = NULL;
        hspi->Rx_HalfCplt_Callback = NULL;
        hspi->TxRx_HalfCplt_Callback = NULL;
        hspi->Error_Callback = NULL;
        hspi->Abort_Callback = NULL;
        
        if (hspi->MSP_Init_Callback != NULL) {
          hspi->MSP_Init_Callback(hspi); // Gọi hàm callback khởi tạo MSP nếu đã đăng ký
        }
      #else
        SPI_MspInit(hspi); // Gọi hàm khởi tạo MSP mặc định nếu callback không được kích hoạt
      #endif
      
      /**
       * Ghi chú:
       * Báo cấu hình là Busy để thực hiện cấu hình thanh ghi.
       */
      hspi->State = SPI_BUSY;

    // Vô hiệu hóa SPI tạm thời

      CLEAR_BIT(
        hspi->Instance->SPI_CR1,
        SPI_CR1_SPE_MASK
      );

    // Khai báo thanh ghi tạm để tính toán cấu hình

      ui32 tmp_cr1 = 0;
      ui32 tmp_cr2 = 0;

      tmp_cr1 = (hspi->Init.Mode)          |
            (hspi->Init.Direction)         |
            (hspi->Init.DataSize)          |
            (hspi->Init.CLKPolarity)       |
            (hspi->Init.CLKPhase)          |
            (hspi->Init.BaudRatePrescaler) |
            (hspi->Init.FirstBit);

    // Kiểm tra NSS và cấu hình tương ứng

      if (hspi->Init.NSS == SPI_NSS_HARD_OUTPUT) {
        tmp_cr2 |= SPI_CR2_SSOE_MASK; // Kích hoạt SSOE để tự động quản lý tín hiệu NSS khi ở chế độ Master
      } else if (hspi->Init.NSS == SPI_NSS_SOFT) {
        tmp_cr1 |= SPI_CR1_SSM_MASK; // Kích hoạt SSM để quản lý tín hiệu NSS bằng phần mềm
        if (hspi->Init.Mode == SPI_MODE_MASTER) {
          tmp_cr1 |= SPI_CR1_SSI_MASK; // Master cần SSI=1
        }
      }

    // Nạp cấu hình vào thanh ghi SPI_CR1
    
      WRITE_REG(
        hspi->Instance->SPI_CR1,
        tmp_cr1
      );

    // Nạp cấu hình vào thanh ghi SPI_CR2

      WRITE_REG(
        hspi->Instance->SPI_CR2,
        tmp_cr2
      );

    // Kích hoạt SPI

      SET_BIT(
        hspi->Instance->SPI_CR1,
        SPI_CR1_SPE_MASK
      );

    // Set trạng thái 

      hspi->State = SPI_READY;
      hspi->ErrorCode = SPI_OK;

    return STAT_OK;
  }

  RETR_STAT SPI_DeInit(SPI_Handle_Param *hspi) {

    // Kiểm tra tham số đầu vào hợp lệ

      if (hspi == NULL) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị các tham số hợp lệ

      assert_param(hspi->Instance != NULL); // Kiểm tra con trỏ tới bộ thanh ghi của ngoại vi SPI hợp lệ

    // Báo cấu hình là Busy để thực hiện cấu hình

      hspi->State = SPI_BUSY;

    // Đợi dừng an toàn trước khi vô hiệu hóa
    
      SPI_WaitBeforeDisable(hspi);

    // Vô hiệu hóa SPI

      CLEAR_BIT(
        hspi->Instance->SPI_CR1,
        SPI_CR1_SPE_MASK
      );

    // Reset ngoại vi

      RCC_PCLK_Reset(SPI1); // Reset ngoại vi SPI1 thông qua RCC
    
    // Gọi MSP_DeInit

      #if (SPI_PUBLIC_CALLBACK_ENABLE == 1U)
        if (hspi->MSP_DeInit_Callback != NULL) {
          hspi->MSP_DeInit_Callback(hspi); // Gọi hàm callback giải phóng MSP nếu đã đăng ký
        }
      #else
        SPI_MspDeInit(hspi); // Gọi hàm giải phóng MSP mặc định nếu callback không được kích hoạt
      #endif

    // Cập nhật trạng thái Handler

      hspi->State = SPI_RESET;
      hspi->ErrorCode = SPI_OK;

    return STAT_DONE;
  }

  __weak void SPI_MSP_Init(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác khởi tạo MSP (MCU Specific Package) 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
  }

  __weak void SPI_MSP_DeInit(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác khởi tạo MSP (MCU Specific Package) 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
  }

  #if (SPI_PUBLIC_CALLBACK_ENABLE == 1U)
    RETR_STAT SPI_RegisterCallback(
      SPI_Handle_Param *hspi, 
      SPI_CallbackIDTypeDef CallbackID, 
      pSPI_CallbackTypeDef pCallback
    ) {

      // Lưu trạng thải trả về, mặc định là STAT_OK, sẽ được cập nhật nếu có lỗi xảy ra trong quá trình đăng ký callback
      
        RETR_STAT status = STAT_OK;
      
      // Kiểm tra con trỏ hspi hợp lệ và pCallback hợp lệ

        if (hspi == NULL || pCallback == NULL) {
          return STAT_ERROR;
        }

      // Kiểm tra trạng thái

        if (hspi->State == SPI_READY) { // Nếu ngoại vi đang ở trạng thái Ready thì mới cho phép đăng ký toàn bộ callback
          switch (CallbackID) {
            case SPI_TX_CPLT_CB_ID:
              hspi->Tx_Cplt_Callback = pCallback;
              break;
            case SPI_RX_CPLT_CB_ID:
              hspi->Rx_Cplt_Callback = pCallback;
              break;
            case SPI_TX_RX_CPLT_CB_ID:
              hspi->TxRx_Cplt_Callback = pCallback;
              break;
            case SPI_TX_HALF_CPLT_CB_ID:
              hspi->Tx_HalfCplt_Callback = pCallback;
              break;
            case SPI_RX_HALF_CPLT_CB_ID:
              hspi->Rx_HalfCplt_Callback = pCallback;
              break;
            case SPI_TX_RX_HALF_CPLT_CB_ID:
              hspi->TxRx_HalfCplt_Callback = pCallback;
              break;
            case SPI_ERROR_CB_ID:
              hspi->Error_Callback = pCallback;
              break;
            case SPI_ABORT_CB_ID:
              hspi->Abort_Callback = pCallback;
              break;
            case SPI_MSP_INIT_CB_ID:
              hspi->MSP_Init_Callback = pCallback;
              break;
            case SPI_MSP_DEINIT_CB_ID:
              hspi->MSP_DeInit_Callback = pCallback;
              break;
            default:
              hspi->ErrorCode = SPI_ERROR_INV_CALLBACK; // Cập nhật mã lỗi vào handle_param
              status = STAT_ERROR; // CallbackID không hợp lệ
              break;
          }
        }

        else if (hspi->State == SPI_RESET) { // Nếu ngoại vi đang ở trạng thái Reset thì chỉ cho phép đăng ký callback khởi tạo và giải phóng MSP
          switch (CallbackID) {
            case SPI_MSP_INIT_CB_ID:
              hspi->MSP_Init_Callback = pCallback;
              break;
            case SPI_MSP_DEINIT_CB_ID:
              hspi->MSP_DeInit_Callback = pCallback;
              break;
            default:
              hspi->ErrorCode = SPI_ERROR_INV_CALLBACK; // Cập nhật mã lỗi vào handle_param
              status = STAT_ERROR; // CallbackID không hợp lệ
              break;
          }
        } 
        
        else { // Ngoại vi đang ở trạng thái khác Ready hoặc Reset thì không cho phép đăng ký callback nào cả
          hspi->ErrorCode = SPI_ERROR_INV_CALLBACK; // Cập nhật mã lỗi vào handle_param
          status = STAT_ERROR; // Ngoại vi đang ở trạng thái không cho phép đăng ký callback
        }

      // Kết thúc quy trình đăng ký callback

        return status;
    }

    RETR_STAT SPI_UnRegisterCallback(
      SPI_Handle_Param *hspi, 
      SPI_CallbackIDTypeDef CallbackID
    ) {

      // Lưu trạng thải trả về, mặc định là STAT_OK, sẽ được cập nhật nếu có lỗi xảy ra trong quá trình đăng ký callback
      
        RETR_STAT status = STAT_OK;
      
      // Kiểm tra con trỏ hspi hợp lệ và pCallback hợp lệ

        if (hspi == NULL || pCallback == NULL) {
          return STAT_ERROR;
        }

      // Kiểm tra trạng thái

        if (hspi->State == SPI_READY) { // Nếu ngoại vi đang ở trạng thái Ready thì mới cho phép đăng ký toàn bộ callback mặc định
          switch (CallbackID) {
            case SPI_TX_CPLT_CB_ID:
              hspi->Tx_Cplt_Callback = Tx_Cplt_Callback;
              break;
            case SPI_RX_CPLT_CB_ID:
              hspi->Rx_Cplt_Callback = Rx_Cplt_Callback;
              break;
            case SPI_TX_RX_CPLT_CB_ID:
              hspi->TxRx_Cplt_Callback = TxRx_Cplt_Callback;
              break;
            case SPI_TX_HALF_CPLT_CB_ID:
              hspi->Tx_HalfCplt_Callback = Tx_HalfCplt_Callback;
              break;
            case SPI_RX_HALF_CPLT_CB_ID:
              hspi->Rx_HalfCplt_Callback = Rx_HalfCplt_Callback;
              break;
            case SPI_TX_RX_HALF_CPLT_CB_ID:
              hspi->TxRx_HalfCplt_Callback = TxRx_HalfCplt_Callback;
              break;
            case SPI_ERROR_CB_ID:
              hspi->Error_Callback = Error_Callback;
              break;
            case SPI_ABORT_CB_ID:
              hspi->Abort_Callback = Abort_Callback;
              break;
            case SPI_MSP_INIT_CB_ID:
              hspi->MSP_Init_Callback = MSP_Init_Callback;
              break;
            case SPI_MSP_DEINIT_CB_ID:
              hspi->MSP_DeInit_Callback = MSP_DeInit_Callback;
              break;
            default:
              hspi->ErrorCode = SPI_ERROR_INV_CALLBACK; // Cập nhật mã lỗi vào handle_param
              status = STAT_ERROR; // CallbackID không hợp lệ
              break;
          }
        }

        else if (hspi->State == SPI_RESET) { // Nếu ngoại vi đang ở trạng thái Reset thì chỉ cho phép đăng ký callback khởi tạo và giải phóng MSP mặc định
          switch (CallbackID) {
            case SPI_MSP_INIT_CB_ID:
              hspi->MSP_Init_Callback = MSP_Init_Callback;
              break;
            case SPI_MSP_DEINIT_CB_ID:
              hspi->MSP_DeInit_Callback = MSP_DeInit_Callback;
              break;
            default:
              hspi->ErrorCode = SPI_ERROR_INV_CALLBACK; // Cập nhật mã lỗi vào handle_param
              status = STAT_ERROR; // CallbackID không hợp lệ
              break;
          }
        } 
        
        else { // Ngoại vi đang ở trạng thái khác Ready hoặc Reset thì không cho phép đăng ký callback nào cả
          hspi->ErrorCode = SPI_ERROR_INV_CALLBACK; // Cập nhật mã lỗi vào handle_param
          status = STAT_ERROR; // Ngoại vi đang ở trạng thái không cho phép đăng ký callback
        }

      // Kết thúc quy trình đăng ký callback

        return status;
    }
  #endif

  void SPI_IRQHandler(SPI_Handle_Param *hspi) {
    
  }

  RETR_STAT SPI_Transmit(
    SPI_Handle_Param *hspi, 
    const ui8* pdata, 
    ui16 size, 
    ui32 timeout
  ) {

    // Kiểm tra tham số đầu vào hợp lệ

      if (hspi == NULL || pdata == NULL || size == 0) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị các tham số hợp lệ

      /**
       * Ghi chú:
       * - Phần cấu hình Direction đã được đảm bảo trong hàm Init 
       * nên sẽ không cần bổ sung khối kiểm tra để bật chế độ Direction
       * - Phần assertion cho timeout không cần thiết vì 
       * hàm sẽ xử lý timeout ở cả trường hợp timeout = 0.
       */

      assert_param(hspi->Instance != NULL); // Kiểm tra con trỏ tới bộ thanh ghi của ngoại vi SPI hợp lệ
      assert_param(
        hspi->Init.Direction == SPI_DIRECTION_2LINES 
        || 
        hspi->Init.Direction == SPI_DIRECTION_1LINE_TX
      ); // Chỉ cho phép truyền dữ liệu khi ở chế độ Full-Duplex hoặc Transmit-only

    // Khai báo quản lý thời gian

      ui32 tickstart = 0;

    // Khai báo lưu số lần truyền dữ liệu đã thực hiện

      ui16 initial_TxXferCount = size;

    // Kiểm tra trạng thái 

      if (hspi->State != SPI_READY) {
        return STAT_BUSY;
      }

    // Kiểm tra tham số hợp lệ

      if (pdata == NULL || size == 0u) {
        return STAT_ERROR;
      }

    // Cấu hình thông tin truyền nhận

      // Cập nhật trạng thái và lỗi
      hspi->State = SPI_BUSY_TX;
      hspi->ErrorCode = SPI_OK;

      // Cấu hình thông tin truyền 
      hspi->Tx_Buff_Ptr = (ui8*)pdata;
      hspi->Tx_Xfer_Size = size;
      hspi->Tx_Xfer_Count = size;

      // Cấu hình thông tin nhận
      /**
       * Ghi chú:
       * Mặc dù không set tham số cho RxBuf nhưng
       * khi hoạt động thì vẫn sẽ có sự tham gia của RxBuf
       * dẫn đến lỗi OVR nên bên dưới sẽ có xử lý để 
       * handle lỗi OVR. 
       */
      hspi->Rx_Buff_Ptr = (ui8*)NULL;
      hspi->Rx_Xfer_Size = 0u;
      hspi->Rx_Xfer_Count = 0u;
      
      // Cấu hình ISR
      hspi->TxISR = NULL;
      hspi->RxISR = NULL;

    /**
     * Ghi chú:
     * Ở khu vực này trong HAL sẽ có bổ sung thêm cấu hình Direction,
     * Tuy nhiên do trong khai báo của driver này đã đảm bảo cấu hình
     * và khai báo đầy đủ các tùy chọn Direction nên 
     * sẽ không cần phải cấu hình lại ở đây nữa.
     */

    // Kích hoạt SPI nếu chưa được kích hoạt 

      if (!READ_BIT(hspi->Instance->SPI_CR1, SPI_CR1_SPE_MASK)) {
        SET_BIT(
          hspi->Instance->SPI_CR1,
          SPI_CR1_SPE_MASK
        );
      }

    // Cập nhật thời điểm bắt đầu truyền dữ liệu để phục vụ cho việc kiểm tra timeout sau này

      tickstart = SYSTICK_GetTick();

    // Truyền dữ liệu

      switch (hspi->Init.DataSize)
      {
      case SPI_DATASIZE_16BIT:
        
        // Xử lý Preload

          /**
           * Ghi chú:
           * Thực hiện preload để Slave đảm bảo phản ứng tức thì 
           * khi Master gửi xung clock đầu tiên, dữ liệu Master
           * thu được không phải là dữ liệu rác.
           * Ngoài ra xử lý reload sẽ đảm bảo cover trường hợp
           * chỉ gửi 1 phần tử dữ liệu thì vẫn có dữ liệu hợp lệ để gửi đi
           * và loại bỏ kiểm tra cờ TXE dư thừa.
           */

          if (
            hspi->Init.Mode == SPI_MODE_SLAVE 
            || 
            initial_TxXferCount == 0x01u
          ) {
            hspi->Instance->SPI_DR = *((const ui16*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 16-bit
            hspi->Tx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer truyền đi (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
            hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã preload 1 phần tử)
          }

          // Truyền phần dữ liệu còn lại

            while (hspi->Tx_Xfer_Count > 0) {
              
              // Đợi cờ TXE
              
                if (
                  __DIFF_CHECK(
                    READ_BIT(hspi->Instance->SPI_SR, SPI_SR_TXE_MASK), 0
                  ) // Đảm bảo TXE != 0 báo TxBuf trống
                ) {
                  hspi->Instance->SPI_DR = *((const ui16*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 16-bit
                  hspi->Tx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer truyền đi (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
                  hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã truyền đi 1 phần tử)
                } else {

                  // Kiểm tra timeout

                    /**
                     * Ghi chú:
                     * Kiểm tra dựa trên 2 điều kiện:
                     * 1. Thời gian đã trôi qua kể từ khi bắt đầu đợi cờ TXE vượt quá giá trị timeout được chỉ định (SYSTICK_GetTick() - tickstart >= timeout) 
                     * và timeout không phải là giá trị đặc biệt SYSTICK_LOAD_MAX_RELOAD_VALUE (được sử dụng để biểu thị chờ vô hạn).
                     * 2. Giá trị timeout được chỉ định là 0, có nghĩa là không chờ đợi và trả về lỗi timeout ngay lập tức nếu cờ TXE không được set.
                     */

                    if (
                      (
                        ((SYSTICK_GetTick() - tickstart) >= timeout)
                        &&
                        (timeout != SYSTICK_LOAD_MAX_RELOAD_VALUE)
                      ) || (
                        timeout == 0x0u
                      )
                    ) {
                      hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình
                      hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
                      return STAT_TIMEOUT; // Truyền dữ liệu thất bại do timeout
                    }
                }
            }
        break;
      
      case SPI_DATASIZE_8BIT:

        // Xử lý Preload

          /**
           * Ghi chú:
           * Thực hiện preload để Slave đảm bảo phản ứng tức thì 
           * khi Master gửi xung clock đầu tiên, dữ liệu Master
           * thu được không phải là dữ liệu rác.
           * Ngoài ra xử lý reload sẽ đảm bảo cover trường hợp
           * chỉ gửi 1 phần tử dữ liệu thì vẫn có dữ liệu hợp lệ để gửi đi
           * và loại bỏ kiểm tra cờ TXE dư thừa.
           */

          if (
            hspi->Init.Mode == SPI_MODE_SLAVE 
            || 
            initial_TxXferCount == 0x01u
          ) {
            *((__vo ui8*)&hspi->Instance->SPI_DR) = *((const ui8*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 8-bit
            hspi->Tx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer truyền đi (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
            hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã preload 1 phần tử)
          }

          // Truyền phần dữ liệu còn lại

            while (hspi->Tx_Xfer_Count > 0) {
              
              // Đợi cờ TXE
              
                if (
                  __DIFF_CHECK(
                    READ_BIT(hspi->Instance->SPI_SR, SPI_SR_TXE_MASK), 0
                  ) // Đảm bảo TXE != 0 báo TxBuf trống
                ) {
                  *((__vo ui8*)&hspi->Instance->SPI_DR) = *((const ui8*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 8-bit
                  hspi->Tx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer truyền đi (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
                  hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã truyền đi 1 phần tử)
                } else {

                  // Kiểm tra timeout

                    /**
                     * Ghi chú:
                     * Kiểm tra dựa trên 2 điều kiện:
                     * 1. Thời gian đã trôi qua kể từ khi bắt đầu đợi cờ TXE vượt quá giá trị timeout được chỉ định (SYSTICK_GetTick() - tickstart >= timeout) 
                     * và timeout không phải là giá trị đặc biệt SYSTICK_LOAD_MAX_RELOAD_VALUE (được sử dụng để biểu thị chờ vô hạn).
                     * 2. Giá trị timeout được chỉ định là 0, có nghĩa là không chờ đợi và trả về lỗi timeout ngay lập tức nếu cờ TXE không được set.
                     */

                    if (
                      (
                        ((SYSTICK_GetTick() - tickstart) >= timeout)
                        &&
                        (timeout != SYSTICK_LOAD_MAX_RELOAD_VALUE)
                      ) || (
                        timeout == 0x0u
                      )
                    ) {
                      hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình
                      hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
                      return STAT_TIMEOUT; // Truyền dữ liệu thất bại do timeout
                    }
                }
            }
        break;

      default:
        hspi->ErrorCode = SPI_ERROR_DATASIZE; // Cập nhật mã lỗi vào handle_param
        hspi->State = SPI_RESET; // Cập nhật trạng thái về Reset để cho phép người dùng khởi tạo lại cấu hình
        return STAT_ERROR; // Kích thước dữ liệu không hợp lệ
        break;
      }

    // Kiểm tra timeout cờ TXE

      if (
        SPI_WaitFlag(
          hspi,
          SPI_SR_TXE_MASK,
          SET,
          timeout,
          tickstart
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
      }

    // Kiểm tra cờ BSY

      if (
        SPI_WaitFlag(
          hspi,
          SPI_SR_BSY_MASK,
          RESET,
          timeout,
          tickstart
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
      }

    // Xử lý cờ OVR nếu là truyền dữ liệu ở chế độ Full-Duplex

      /**
       * Ghi chú:
       * Đối với hàm Transmit ở bất kỳ chế độ nào,
       * thì cả TxBuf và RxBuf luôn hoạt động.
       * RxBuf sẽ thu dữ liệu rác làm cờ RXNE luôn được set 
       * sau khi truyền đi 1 phần tử dữ liệu.
       * Khi truyền ở lần thứ 2 mà dữ liệu RxBuf vẫn chưa đọc ra
       * thì cờ OVR sẽ được set, nếu không xử lý cờ OVR thì 
       * sẽ không thể tiếp tục truyền dữ liệu được nữa
       * vì cờ OVR sẽ ngăn cản việc set lại cờ TXE để báo TxBuf trống, 
       * dẫn đến tình trạng treo ở vòng đợi cờ TXE ở lần truyền thứ 2 trở đi.
        * Do đó, cần phải xử lý cờ OVR sau khi truyền xong dữ liệu 
        * để đảm bảo có thể tiếp tục truyền dữ liệu được nữa nếu cần thiết.
       */

      if (
        hspi->Init.Direction == SPI_DIRECTION_2LINES 
      ) {
        __vo ui32 tmp = 0x0u;
        tmp = hspi->Instance->SPI_DR; // Đọc DR
        tmp = hspi->Instance->SPI_SR; // Đọc SR để xóa cờ OVR
      }

    // Báo trạng thái hoàn thành

      hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình nếu cần thiết

    // Kiểm tra mã lỗi

      if (hspi->ErrorCode != SPI_OK) {
        return STAT_ERROR; // Truyền dữ liệu thất bại do lỗi đã được cập nhật vào handle_param
      }

    // Truyền dữ liệu thành công

      return STAT_OK;

  }

  RETR_STAT SPI_Receive(
    SPI_Handle_Param *hspi, 
    ui8* pdata, 
    ui16 size, 
    ui32 timeout
  ) {
    
  }

  RETR_STAT SPI_TransmitReceive(
    SPI_Handle_Param *hspi, 
    const ui8* pdata_tx, 
    ui8* pdata_rx, 
    ui16 size, 
    ui32 timeout
  ) {

  }

  RETR_STAT SPI_Transmit_IT(
    SPI_Handle_Param *hspi, 
    const ui8* pdata, 
    ui16 size
  ) {

  }

  RETR_STAT SPI_Receive_IT(
    SPI_Handle_Param *hspi, 
    ui8* pdata, 
    ui16 size
  ) {

  }

  RETR_STAT SPI_TransmitReceive_IT(
    SPI_Handle_Param *hspi, 
    const ui8* pdata_tx, 
    ui8* pdata_rx, 
    ui16 size
  ) {

  }

  RETR_STAT SPI_Transmit_DMA(
    SPI_Handle_Param *hspi, 
    const ui8* pdata, 
    ui16 size
  ) {

  }

  RETR_STAT SPI_Receive_DMA(
    SPI_Handle_Param *hspi, 
    ui8* pdata, 
    ui16 size
  ) {

  }

  RETR_STAT SPI_TransmitReceive_DMA(
    SPI_Handle_Param *hspi, 
    const ui8* pdata_tx, 
    ui8* pdata_rx, 
    ui16 size
  ) {

  }

  RETR_STAT SPI_DMA_Pause(SPI_Handle_Param *hspi) {

  }

  RETR_STAT SPI_DMA_Resume(SPI_Handle_Param *hspi) {

  }

  RETR_STAT SPI_DMA_Stop(SPI_Handle_Param *hspi) {

  }

  RETR_STAT SPI_Abort(SPI_Handle_Param *hspi) {

  }

  RETR_STAT SPI_Abort_IT(SPI_Handle_Param *hspi) {
    
  }

  SPI_STAT_Enum SPI_GetState(SPI_Handle_Param *hspi) {

  }

  ui32 SPI_GetError(SPI_Handle_Param *hspi) {

  }

// Định nghĩa các hàm callback weak mặc định (nếu được kích hoạt)

  __weak void Tx_Cplt_Callback(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác hoàn thành truyền dữ liệu 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
    return;
  }

  __weak void Rx_Cplt_Callback(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác hoàn thành nhận dữ liệu 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
    return;
  }

  __weak void TxRx_Cplt_Callback(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác hoàn thành truyền/nhận dữ liệu 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
    return;
  }

  __weak void Tx_HalfCplt_Callback(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác hoàn thành truyền một nửa dữ liệu 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
    return;
  }

  __weak void Rx_HalfCplt_Callback(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác hoàn thành nhận một nửa dữ liệu 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
    return;
  }

  __weak void TxRx_HalfCplt_Callback(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác hoàn thành truyền/nhận một nửa dữ liệu 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
    return;
  }

  __weak void Error_Callback(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác xử lý lỗi 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
    return;
  }

  __weak void Abort_Callback(SPI_Handle_Param *hspi) {
    /**
     * Ghi chú:
     * User có thể tự override hàm này 
     * để implement các thao tác hủy giao tiếp 
     * tùy thuộc vào thiết kế phần cứng của mình.
     */
    return;
  }

  
