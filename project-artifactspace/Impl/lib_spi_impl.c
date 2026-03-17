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
    #include "spi/lib_spi_def.h"
    #include "spi/lib_spi_hal.h"
    #include "clock/lib_clock_def.h"
    #include "clock/lib_clock_hal.h"
  #endif

// Khai báo hàm nội bộ

  static void SPI_WaitBeforeDisable(SPI_Handle_Param *hspi);

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
      
      #if (SPI_CRC_ENABLE == 1U)
        assert_param(IS_SPI_CRCENABLE(hspi->Init.CRCCalculation));
        if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE) {
          assert_param(IS_SPI_CRCPOLYNOMIAL(hspi->Init.CRCPolynomial));
        } else {
          hspi->Init.CRCPolynomial = 0; 
        }
      #else
        hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
      #endif

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
            (hspi->Init.FirstBit)          |
            (hspi->Init.CRCCalculation);

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

    // Cấu hình CRC nếu được kích hoạt

      #if (SPI_CRC_ENABLE == 1U)
        if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE) {
          WRITE_REG(
            hspi->Instance->SPI_CRCPR,
            hspi->Init.CRCPolynomial
          );
        }
      #endif

    // Kích hoạt SPI

      SET_BIT(
        hspi->Instance->SPI_CR1,
        SPI_CR1_SPE_MASK
      );

    // Set trạng thái 

      hspi->State = SPI_READY;
      hspi->ErrorCode = SPI_OK;

    return STAT_DONE;
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

      assert_param(hspi->Instance != NULL); // Kiểm tra con trỏ tới bộ thanh ghi của ngoại vi SPI hợp lệ
      assert_param(hspi->State == SPI_READY); // Ngoại vi phải ở trạng thái Ready mới cho phép truyền dữ liệu
      assert_param(
        hspi->Init.Direction == SPI_DIRECTION_2LINES 
        || 
        hspi->Init.Direction == SPI_DIRECTION_1LINE_TX
      ); // Chỉ cho phép truyền dữ liệu khi ở chế độ Full-Duplex hoặc Transmit-only
      assert_param(size <= 0xFFFF); // Kích thước dữ liệu cần truyền phải nhỏ hơn hoặc bằng 65535 phần tử
      assert_param(timeout > 0); // Thời gian chờ phải lớn hơn 0

    // Khai báo quản lý thời gian

      ui32 tickstart = 0;

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

    // Reset CRC

      #if (SPI_CRC_ENABLE == 1U)
        if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE) {
          CLEAR_BIT(
            hspi->Instance->SPI_CR1,
            SPI_CR1_CRCEN_MASK
          );
        }
      #endif

    // Kích hoạt SPI nếu chưa được kích hoạt 

      if (!READ_BIT(hspi->Instance->SPI_CR1, SPI_CR1_SPE_MASK)) {
        SET_BIT(
          hspi->Instance->SPI_CR1,
          SPI_CR1_SPE_MASK
        );
      }

    // Truyền dữ liệu

      switch (hspi->Init.DataSize)
      {
      case SPI_DATASIZE_16BIT:
        /* code */
        break;
      
      case SPI_DATASIZE_8BIT:
        /* code */
        break;

      default:
        hspi->ErrorCode = SPI_ERROR_DATASIZE; // Cập nhật mã lỗi vào handle_param
        hspi->State = SPI_RESET; // Cập nhật trạng thái về Reset để cho phép người dùng khởi tạo lại cấu hình
        return STAT_ERROR; // Kích thước dữ liệu không hợp lệ
        break;
      }

    return STAT_DONE;

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

  
