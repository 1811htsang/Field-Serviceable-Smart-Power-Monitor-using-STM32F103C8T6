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

// Khai báo biến quản lý thời gian

	__vo ui32 ms_ticks = 0;

// Khai báo hàm nội bộ

  // >> Hàm vô hiệu hóa với polling
  sta void SPI_DisablePolling(SPI_Handle_Param *hspi);
  
  // >> Hàm chờ cờ với timeout, trả về STAT_OK nếu cờ đạt trạng thái mong muốn, trả về STAT_TIMEOUT nếu timeout xảy ra
  sta RETR_STAT SPI_FlagTimeout(
    SPI_Handle_Param *hspi, 
    ui32 flag_mask,      // Mặt nạ bit của cờ cần chờ
    ui32 desired_status, // Trạng thái mong muốn của cờ sau khi chờ (0 hoặc != 0)
    ui32 timeout,        // Thời gian chờ tối đa tính bằng ms
    ui32 tickstart       // Thời điểm bắt đầu chờ tính bằng tick của SysTick
  );

  // >> API nội bộ sử dụng cho norm mode
  sta RETR_STAT SPI_Transmit_Norm(
    SPI_Handle_Param *hspi, const ui8* pdata, 
    ui16 size, ui32 timeout
  );
  sta RETR_STAT SPI_Receive_Norm(
    SPI_Handle_Param *hspi, ui8* pdata, 
    ui16 size, ui32 timeout
  );
  sta RETR_STAT SPI_TransmitReceive_Norm(
    SPI_Handle_Param *hspi, 
    const ui8* pdata_tx, ui8* pdata_rx, 
    ui16 size, ui32 timeout
  );

  // >> API nội bộ sử dụng cho intr mode
  sta RETR_STAT SPI_Transmit_Intr(
    SPI_Handle_Param *hspi, 
    const ui8 *pdata, ui16 size
  );
  sta RETR_STAT SPI_Receive_Intr(
    SPI_Handle_Param *hspi, 
    ui8 *pdata, ui16 size
  );
  sta RETR_STAT SPI_TransmitReceive_Intr(
    SPI_Handle_Param *hspi, 
    const ui8 *ptx, ui8 *prx, 
    ui16 size
  );

  // >> API nội bộ sử dụng cho abort mode
  sta RETR_STAT SPI_Abort_Norm(SPI_Handle_Param *hspi);
  sta RETR_STAT SPI_Abort_Intr(SPI_Handle_Param *hspi);

  // >> API nội bộ cho xử lý ISR 1 line TX/RX
  sta void SPI_1lineTxISR_8BIT(SPI_Handle_Param *hspi);
  sta void SPI_1lineTxISR_16BIT(SPI_Handle_Param *hspi);
  sta void SPI_1lineRxISR_8BIT(SPI_Handle_Param *hspi);
  sta void SPI_1lineRxISR_16BIT(SPI_Handle_Param *hspi);

  // >> API nội bộ cho xử lý ISR 2 line TX/RX
  sta void SPI_2lineTxISR_8BIT(SPI_Handle_Param *hspi);
  sta void SPI_2lineTxISR_16BIT(SPI_Handle_Param *hspi);
  sta void SPI_2lineRxISR_8BIT(SPI_Handle_Param *hspi);
  sta void SPI_2lineRxISR_16BIT(SPI_Handle_Param *hspi);

  // >> API nội bộ cho đóng giao tiếp và thu thập trạng thái cuối cùng khi kết thúc quá trình truyền nhận
  sta void SPI_CloseTx_ISR(SPI_Handle_Param *hspi);
  sta void SPI_CloseRx_ISR(SPI_Handle_Param *hspi);
  sta void SPI_CloseRxTx_ISR(SPI_Handle_Param *hspi);

  // >> API nội bộ cho thu thập trạng thái cuối cùng và callback khi kết thúc quá trình truyền nhận
  sta RETR_STAT SPI_EndTxTransaction(SPI_Handle_Param *hspi, ui32 timeout, ui32 tickstart);
  sta RETR_STAT SPI_EndRxTransaction(SPI_Handle_Param *hspi, ui32 timeout, ui32 tickstart);
  sta RETR_STAT SPI_EndRxTxTransaction(SPI_Handle_Param *hspi, ui32 timeout, ui32 tickstart);

// Định nghĩa các hàm callback weak mặc định (nếu được kích hoạt)

  /*
   * Callback mặc định khi truyền SPI hoàn tất.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI đang được sử dụng.
   *
   * Logic:
   *   - Cho phép ứng dụng ghi đè để xử lý hậu truyền dữ liệu.
   *
   * Trả về:
   *   Không có.
   */
  __weak void Tx_Cplt_Callback(SPI_Handle_Param *hspi) {
    return;
  }

  /*
   * Callback mặc định khi nhận SPI hoàn tất.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI đang được sử dụng.
   *
   * Logic:
   *   - Cho phép ứng dụng ghi đè để xử lý hậu nhận dữ liệu.
   *
   * Trả về:
   *   Không có.
   */
  __weak void Rx_Cplt_Callback(SPI_Handle_Param *hspi) {
    return;
  }

  /*
   * Callback mặc định khi truyền và nhận SPI hoàn tất.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI đang được sử dụng.
   *
   * Logic:
   *   - Cho phép ứng dụng ghi đè để xử lý hậu truyền/nhận dữ liệu.
   *
   * Trả về:
   *   Không có.
   */
  __weak void TxRx_Cplt_Callback(SPI_Handle_Param *hspi) {
    return;
  }

  /*
   * Callback mặc định khi truyền SPI đạt mốc một nửa.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI đang được sử dụng.
   *
   * Logic:
   *   - Cho phép ứng dụng ghi đè để xử lý mốc giữa của quá trình truyền.
   *
   * Trả về:
   *   Không có.
   */
  __weak void Tx_HalfCplt_Callback(SPI_Handle_Param *hspi) {
    return;
  }

  /*
   * Callback mặc định khi nhận SPI đạt mốc một nửa.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI đang được sử dụng.
   *
   * Logic:
   *   - Cho phép ứng dụng ghi đè để xử lý mốc giữa của quá trình nhận.
   *
   * Trả về:
   *   Không có.
   */
  __weak void Rx_HalfCplt_Callback(SPI_Handle_Param *hspi) {
    return;
  }

  /*
   * Callback mặc định khi truyền và nhận SPI đạt mốc một nửa.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI đang được sử dụng.
   *
   * Logic:
   *   - Cho phép ứng dụng ghi đè để xử lý mốc giữa của quá trình truyền/nhận.
   *
   * Trả về:
   *   Không có.
   */
  __weak void TxRx_HalfCplt_Callback(SPI_Handle_Param *hspi) {
    return;
  }

  /*
   * Callback mặc định khi SPI phát sinh lỗi.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI đang được sử dụng.
   *
   * Logic:
   *   - Cho phép ứng dụng ghi đè để xử lý lỗi giao tiếp.
   *
   * Trả về:
   *   Không có.
   */
  __weak void Error_Callback(SPI_Handle_Param *hspi) {
    return;
  }

  /*
   * Callback mặc định khi SPI bị hủy giao tiếp.
   *
   * Tham số:
   *   hspi - Con trỏ tới handle SPI đang được sử dụng.
   *
   * Logic:
   *   - Cho phép ứng dụng ghi đè để xử lý trạng thái sau khi abort.
   *
   * Trả về:
   *   Không có.
   */
  __weak void Abort_Callback(SPI_Handle_Param *hspi) {
    return;
  }

// Định nghĩa các hàm thành phần

  sta void SPI_DisablePolling(SPI_Handle_Param *hspi) {

    // Xác định chế độ truyền nhận để biết cần chờ cờ nào trước khi tắt SPI

      ui32 direction = hspi->Init.Direction;
      ui32 mode = hspi->Init.Mode;

    // Trường hợp 1 & 2: Full-Duplex hoặc Transmit-only (2 dây TX/RX hoặc 1 dây TX)

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
    
    // Trường hợp 3: Master Receive-only (2 dây RX hoặc 1 dây RX)

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
        // 1. Đợi RXNE = 1 (Bộ đệm nhận đầy)
        while (!READ_BIT(hspi->Instance->SPI_SR, SPI_SR_RXNE_MASK));
        // 2. Đợi BSY = 0 (Giao tiếp vật lý kết thúc)
        while (READ_BIT(hspi->Instance->SPI_SR, SPI_SR_BSY_MASK));
      }
    
    // Trường hợp 4: Slave Receive-only

      else {
        // Chỉ cần đợi BSY = 0 (Giao tiếp vật lý kết thúc) vì Slave sẽ không có cờ TXE hoặc RXNE để chờ
        while (READ_BIT(hspi->Instance->SPI_SR, SPI_SR_BSY_MASK));
      }
  }

  sta RETR_STAT SPI_FlagTimeout(
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
          elapsed = SYSTICK_GetTick() - tickstart; // Tính thời gian đã trôi qua kể từ khi bắt đầu chờ
          if (
            (elapsed >= timeout) // Nếu thời gian đã trôi qua lớn hơn hoặc bằng timeout ban đầu thì trả về lỗi timeout
            ||
            (timeout == 0u) // Trường hợp timeout ban đầu là 0, tức là không chờ, sẽ trả về timeout ngay lập tức
            ||
            (timeout_adjusted == 0u) // Nếu timeout đã được điều chỉnh về 0 do Systick bị tắt thì trả về timeout ngay lập tức
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
          timeout_adjusted--; // Giảm timeout đã điều chỉnh để tiếp tục chờ nếu Systick vẫn hoạt động
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

        hspi->MSP_Init_Callback = NULL;
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
        } else {
          SPI_MSP_Init(hspi); // Fallback về weak callback mặc định nếu chưa đăng ký public callback
        }
      #else
        SPI_MSP_Init(hspi); // Gọi hàm khởi tạo MSP mặc định nếu callback không được kích hoạt
      #endif
      
      /**
       * Ghi chú:
       * Báo cấu hình là Busy để thực hiện cấu hình thanh ghi.
       */
      hspi->State = SPI_BUSY;

    // Vô hiệu hóa SPI tạm thời

      SPI_Disable(hspi);

    /**
     * Ghi chú:
     * Quy trình cấu hình SPI cho master và Slave đều tuân thủ các quá trình như sau:
     * - Cấu hình CPOL & CPHA
     * - Cấu hình DFF
     * - Cấu hình NSS
     * - Cấu hình SPE
     * 
     * Trong đó, cụ thể hơn đối với Slave:
     * - Khi dùng NSS Hardware thì SSM = 0, SSI = 0, SSOE = 0
     * - Khi dùng NSS Software thì SSM = 1, SSI = 0, SSOE = 0
     * - Xóa MSTR để đảm bảo ở chế độ Slave.
     * 
     * Đối với Master:
     * - Cấu hình thêm baud rate với BR[2:0]
     * - Khi dùng NSS Hardware Input thì NSS = 1
     * - Khi dùng NSS Software Input thì SSM = 1, SSI = 1, SSOE = 0
     * - Khi dùng NSS Software Output thì SSM = 1, SSI = 1, SSOE = 1
     * 
     * Lưu ý rằng việc cấu hình các bit này sẽ được thực hiện thông qua việc ghi vào thanh ghi CR1 và CR2,
     * trong đó một số bit sẽ được cấu hình trong CR1 (như CPOL, CPHA, DFF, SSM, SSI) 
     * và một số bit sẽ được cấu hình trong CR2 (như SSOE).
     * Việc tách biệt này giúp cho quá trình cấu hình trở nên rõ ràng hơn và dễ dàng quản lý hơn.
     */

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
          tmp_cr1 |= SPI_CR1_SSI_MASK; // Master cần SSI=1, Slave thì xóa SSI 
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

      SPI_Enable(hspi);

    // Set trạng thái 

      hspi->State = SPI_READY;
      hspi->ErrorCode = SPI_ERR_OK;

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
    
      SPI_DisablePolling(hspi);

    // Vô hiệu hóa SPI

      SPI_Disable(hspi);

    // Reset ngoại vi

      RCC_PCLK_Reset(SPI1); // Reset ngoại vi SPI1 thông qua RCC
    
    // Gọi MSP_DeInit

      #if (SPI_PUBLIC_CALLBACK_ENABLE == 1U)
        if (hspi->MSP_DeInit_Callback != NULL) {
          hspi->MSP_DeInit_Callback(hspi); // Gọi hàm callback giải phóng MSP nếu đã đăng ký
        }
      #else
        SPI_MSP_DeInit(hspi); // Gọi hàm giải phóng MSP mặc định nếu callback không được kích hoạt
      #endif

    // Cập nhật trạng thái Handler

      hspi->State = SPI_RESET;
      hspi->ErrorCode = SPI_ERR_OK;

    return STAT_OK;
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
      
      // Kiểm tra con trỏ hspi hợp lệ

        if (hspi == NULL) {
          return STAT_ERROR;
        }

      // Kiểm tra trạng thái

        if (hspi->State == SPI_READY) { // Nếu ngoại vi đang ở trạng thái Ready thì mới cho phép xóa đăng ký callback
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
              hspi->MSP_Init_Callback = SPI_MSP_Init;
              break;
            case SPI_MSP_DEINIT_CB_ID:
              hspi->MSP_DeInit_Callback = SPI_MSP_DeInit;
              break;
            default:
              hspi->ErrorCode = SPI_ERROR_INV_CALLBACK; // Cập nhật mã lỗi vào handle_param
              status = STAT_ERROR; // CallbackID không hợp lệ
              break;
          }
        }

        else if (hspi->State == SPI_RESET) { // Nếu ngoại vi đang ở trạng thái Reset thì chỉ cho phép xóa đăng ký callback khởi tạo và giải phóng MSP
          switch (CallbackID) {
            case SPI_MSP_INIT_CB_ID:
              hspi->MSP_Init_Callback = SPI_MSP_Init;
              break;
            case SPI_MSP_DEINIT_CB_ID:
              hspi->MSP_DeInit_Callback = SPI_MSP_DeInit;
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

      // Kết thúc quy trình xóa đăng ký callback

        return status;
    }
  #endif

  void SPI_IRQHandler(SPI_Handle_Param *hspi) {
    
    /**
     * Ghi chú:
     * Phần này sẽ được implement bên cạnh với phần truyền sử dụng Interrupt.
     */

  }

  RETR_STAT SPI_Transmit_Norm(
    SPI_Handle_Param *hspi, 
    const ui8* pdata, 
    ui16 size, 
    ui32 timeout
  ) {

    // Kiểm tra tham số đầu vào hợp lệ

      /**
       * Ghi chú:
       * Phần này được kiểm tra ở Dispatcher nên sẽ không cần kiểm tra lại ở đây nữa.
       */

    // Kiểm tra giá trị các tham số hợp lệ

      /**
       * Ghi chú:
       * - Phần cấu hình Direction đã được đảm bảo trong hàm Init 
       * nên sẽ không cần bổ sung khối kiểm tra để bật chế độ Direction
       * - Phần assertion cho timeout không cần thiết vì 
       * hàm sẽ xử lý timeout ở cả trường hợp timeout = 0.
       */

      assert_param(hspi->Instance != NULL); // Kiểm tra con trỏ tới bộ thanh ghi của ngoại vi SPI hợp lệ
      assert_param(IS_FD(hspi) || IS_HDTX(hspi)); // TX+RX || TX-only

    // Khai báo quản lý thời gian

      ui32 tickstart = 0;

    // Khai báo lưu số lần truyền dữ liệu đã thực hiện

      ui16 initial_TxXferCount = size;

    // Đảm bảo kiểm tra SPI ready trước khi bắt đầu truyền dữ liệu

      if (hspi->State != SPI_READY) {
        return STAT_BUSY;
      }

    // Cấu hình thông tin truyền nhận

      /**
       * Ghi chú:
       * size này nghĩa là số phần tử dữ liệu cần truyền, không phải số byte,
       * kích thước phần tử dữ liệu sẽ được xác định bởi cấu hình DataSize trong Init.
       * Do đó, khi cập nhật con trỏ buffer truyền đi và số lượng phần tử cần truyền 
       * thì cần phải tính toán dựa trên kích thước phần tử dữ liệu để đảm bảo truyền đúng dữ liệu 
       * và tránh lỗi tràn bộ đệm hoặc truyền dữ liệu rác.
       */

      // Cập nhật trạng thái và lỗi

        hspi->State = SPI_BUSY_TX; // Báo trạng thái đang bận truyền dữ liệu, sẽ được cập nhật lại khi kết thúc quá trình truyền
        hspi->ErrorCode = SPI_ERR_OK;

      // Cấu hình thông tin truyền 

        hspi->Tx_Buff_Ptr = (ui8*)pdata;
        hspi->Tx_Xfer_Size = size;
        hspi->Tx_Xfer_Count = size;

      // Cấu hình thông tin nhận

        hspi->Rx_Buff_Ptr = (ui8*)NULL;
        hspi->Rx_Xfer_Size = 0u;
        hspi->Rx_Xfer_Count = 0u;

        /**
         * Ghi chú:
         * Mặc dù không set tham số cho RxBuf nhưng
         * khi hoạt động thì vẫn sẽ có sự tham gia của RxBuf
         * dẫn đến lỗi OVR nên bên dưới sẽ có xử lý để 
         * handle lỗi OVR. 
         */
      
      // Cấu hình ISR

        hspi->TxISR = NULL;
        hspi->RxISR = NULL;

        /**
         * Ghi chú:
         * Do chế độ polling nên sẽ không cần cấu hình ISR,
         * tuy nhiên vẫn cần đảm bảo rằng con trỏ hàm ISR được set về NULL
         */

      // Cấu hình Direction

        /**
         * Ghi chú:
         * Ở khu vực này trong HAL sẽ có bổ sung thêm cấu hình Direction,
         * Tuy nhiên do trong khai báo của driver này đã đảm bảo cấu hình
         * và khai báo đầy đủ các tùy chọn Direction nên 
         * sẽ không cần phải cấu hình lại ở đây nữa.
         */

    // Kích hoạt SPI nếu chưa được kích hoạt 

      if (__DIFF_CHECK(SPI_SPE_GET(),SPI_CR1_SPE_MASK)) {
        SPI_SPE_ENABLE();
      }

    // Cập nhật thời điểm bắt đầu truyền dữ liệu để phục vụ cho việc kiểm tra timeout sau này

      tickstart = SYSTICK_GetTick();

    // Truyền dữ liệu

      switch (hspi->Init.DataSize) {
        case SPI_DATASIZE_16BIT:
          
          // Xử lý Preload = Truyền mồi để đảm bảo Slave phản ứng kịp thời với xung clock đầu tiên của Master

            /**
             * Ghi chú:
             * Thực hiện preload để Slave đảm bảo phản ứng tức thì 
             * khi Master gửi xung clock đầu tiên, dữ liệu Master
             * thu được không phải là dữ liệu rác.
             * Ngoài ra xử lý reload sẽ đảm bảo cover trường hợp
             * chỉ gửi 1 phần tử dữ liệu thì vẫn có dữ liệu hợp lệ để gửi đi
             * và loại bỏ kiểm tra cờ TXE dư thừa.
             */

            if (IS_SLAVE(hspi) || initial_TxXferCount == 0x01u) {
              hspi->Instance->SPI_DR = *((const ui16*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 16-bit
              hspi->Tx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer truyền đi (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
              hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã preload 1 phần tử)
            }

          // Truyền phần dữ liệu còn lại

            while (hspi->Tx_Xfer_Count > 0) {
              
              // Đợi cờ TXE
              
                if (__DIFF_CHECK(SPI_TXE_GET(), 0)) { // Đảm bảo TXE != 0 báo TxBuf trống
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
                        ((SYSTICK_GetTick() - tickstart) >= timeout) // Nếu thời gian đã trôi qua lớn hơn hoặc bằng timeout ban đầu thì trả về lỗi timeout
                        &&
                        (timeout != SYSTICK_LOAD_MAX_RELOAD_VALUE) // Trường hợp timeout ban đầu là giá trị đặc biệt biểu thị chờ vô hạn thì không trả về timeout ngay lập tức mà tiếp tục chờ
                      ) || (
                        timeout == 0x0u // Trường hợp timeout ban đầu là 0, tức là không chờ, sẽ trả về timeout ngay lập tức nếu cờ TXE không được set
                      )
                    ) {
                      hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình
                      hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
                      return STAT_TIMEOUT; // Truyền dữ liệu thất bại do timeout
                    }
                }
            }

          break;
        
        case SPI_DATASIZE_8BIT: // Xử lý tương tự như trường hợp 16-bit nhưng với kích thước dữ liệu là 8-bit

          // Xử lý Preload = Truyền mồi để đảm bảo Slave phản ứng kịp thời với xung clock đầu tiên của Master

            /**
             * Ghi chú:
             * Thực hiện preload để Slave đảm bảo phản ứng tức thì 
             * khi Master gửi xung clock đầu tiên, dữ liệu Master
             * thu được không phải là dữ liệu rác.
             * Ngoài ra xử lý reload sẽ đảm bảo cover trường hợp
             * chỉ gửi 1 phần tử dữ liệu thì vẫn có dữ liệu hợp lệ để gửi đi
             * và loại bỏ kiểm tra cờ TXE dư thừa.
             */

            if (IS_SLAVE(hspi) || initial_TxXferCount == 0x01u) {
              *((__vo ui8*)&hspi->Instance->SPI_DR) = *((const ui8*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 8-bit
              hspi->Tx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer truyền đi (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
              hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã preload 1 phần tử)
            }

            // Truyền phần dữ liệu còn lại

              while (hspi->Tx_Xfer_Count > 0) {
                
                // Đợi cờ TXE
                
                  if (__DIFF_CHECK(SPI_TXE_GET(), 0)) { // Đảm bảo TXE != 0 báo TxBuf trống
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

    // Kiểm tra kết thúc transaction

      if (SPI_EndTxTransaction(hspi, timeout, tickstart) != STAT_OK) {
        return STAT_TIMEOUT; // Truyền dữ liệu thất bại do timeout khi chờ kết thúc transaction
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

      if (IS_FD(hspi)) { // Chỉ xử lý cờ OVR nếu ở chế độ Full-Duplex vì chỉ có chế độ này mới có sự tham gia của RxBuf
        {
          __vo ui32 tmp = 0x0u;
          tmp = hspi->Instance->SPI_DR; // Đọc DR
          tmp = hspi->Instance->SPI_SR; // Đọc SR để xóa cờ OVR
          tmp = 0u;
        }
      }

    // Báo trạng thái hoàn thành

      hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình nếu cần thiết

    // Kiểm tra mã lỗi

      if (hspi->ErrorCode != SPI_ERR_OK) {
        return STAT_ERROR; // Truyền dữ liệu thất bại do lỗi đã được cập nhật vào handle_param
      }

    // Truyền dữ liệu thành công

      return STAT_OK;

  }

  RETR_STAT SPI_Receive_Norm(
    SPI_Handle_Param *hspi, 
    ui8* pdata, 
    ui16 size, 
    ui32 timeout
  ) {
    
    // Kiểm tra tham số đầu vào hợp lệ

      /**
       * Ghi chú:
       * Phần này được kiểm tra ở Dispatcher nên sẽ không cần kiểm tra lại ở đây nữa.
       */

    // Khai báo quản lý thời gian

      ui32 tickstart = 0;

    // Đảm bảo kiểm tra SPI ready trước khi bắt đầu nhận dữ liệu

      if (hspi->State != SPI_READY) {
        return STAT_BUSY;
      }

    // Kiểm tra chế độ và tạo xung

      if (IS_MASTER(hspi) && IS_FD(hspi)) {
        hspi->State = SPI_BUSY_RX; // Cập nhật trạng thái về Busy_RX để báo đang nhận dữ liệu
        return SPI_TransmitReceive_Norm( // Hàm này được gọi để sử dụng data tạo xung và thực hiện chức năng như SPI_Receive
          hspi, pdata, pdata,
          size, timeout
        );
      }

    // Đánh dấu timing

      tickstart = SYSTICK_GetTick();

    // Cấu hình thông tin truyền nhận

      // Cập nhật trạng thái và lỗi
      
        hspi->State = SPI_BUSY_RX;
        hspi->ErrorCode = SPI_ERR_OK;

      // Cấu hình thông tin nhận

        hspi->Rx_Buff_Ptr = (ui8*)pdata;
        hspi->Rx_Xfer_Size = size;
        hspi->Rx_Xfer_Count = size;

      // Cấu hình thông tin truyền 

        hspi->Tx_Buff_Ptr = (ui8*)NULL;
        hspi->Tx_Xfer_Size = 0u;
        hspi->Tx_Xfer_Count = 0u;

        /**
         * Ghi chú:
         * Mặc dù không set tham số cho TxBuf nhưng
         * khi hoạt động thì vẫn sẽ có sự tham gia của TxBuf
         * dẫn đến việc tạo xung clock để Slave có thể gửi dữ liệu,
         * do đó sẽ có xử lý để đảm bảo TxBuf vẫn hoạt động 
         * ngay cả khi không có dữ liệu thực tế để truyền đi.
         */

      // Cấu hình ISR

        hspi->TxISR = NULL;
        hspi->RxISR = NULL;

        /**
         * Ghi chú:
         * Do chế độ polling nên sẽ không cần cấu hình ISR
         */

    // Kích hoạt SPI nếu chưa được kích hoạt 

      if ( // Nếu SPI chưa được kích hoạt (SPE = 0) thì mới kích hoạt
        __DIFF_CHECK(SPI_SPE_GET(),SPI_CR1_SPE_MASK) 
      ) {
        SPI_SPE_ENABLE();
      }

    // Nhận dữ liệu

      switch (hspi->Init.DataSize) {
        case SPI_DATASIZE_16BIT:

          // Nhận dữ liệu

            while (hspi->Rx_Xfer_Count > 0u) {

              // Đợi cờ RXNE

                if (SPI_RXNE_GET()) { // Đảm bảo RXNE != 0 báo RxBuf có dữ liệu
                  *((ui16*)hspi->Rx_Buff_Ptr) = hspi->Instance->SPI_DR; // Đọc dữ liệu ràng buộc casting 16-bit
                  hspi->Rx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer nhận vào (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
                  hspi->Rx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần nhận (giảm đi 1 phần tử vì đã nhận được 1 phần tử)
                } else {

                  // Kiểm tra timeout

                    /**
                     * Ghi chú:
                     * Kiểm tra dựa trên 2 điều kiện:
                     * 1. Thời gian đã trôi qua kể từ khi bắt đầu đợi cờ RXNE vượt quá giá trị timeout được chỉ định (SYSTICK_GetTick() - tickstart >= timeout) 
                     * và timeout không phải là giá trị đặc biệt SYSTICK_LOAD_MAX_RELOAD_VALUE (được sử dụng để biểu thị chờ vô hạn).
                     * 2. Giá trị timeout được chỉ định là 0, có nghĩa là không chờ đợi và trả về lỗi timeout ngay lập tức nếu cờ RXNE không được set.
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
                      return STAT_TIMEOUT; // Nhận dữ liệu thất bại do timeout
                    }
                }
            }

          break;
        
        case SPI_DATASIZE_8BIT:

          // Nhận dữ liệu

            while (hspi->Rx_Xfer_Count > 0u) {

              // Đợi cờ RXNE

                if (SPI_RXNE_GET()) { // Đảm bảo RXNE != 0 báo RxBuf có dữ liệu
                  *((ui8*)hspi->Rx_Buff_Ptr) = *((__vo ui8*)&hspi->Instance->SPI_DR); // Đọc dữ liệu ràng buộc casting 8-bit
                  hspi->Rx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer nhận vào (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
                  hspi->Rx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần nhận (giảm đi 1 phần tử vì đã nhận được 1 phần tử)
                } else {

                  // Kiểm tra timeout

                    /**
                     * Ghi chú:
                     * Kiểm tra dựa trên 2 điều kiện:
                     * 1. Thời gian đã trôi qua kể từ khi bắt đầu đợi cờ RXNE vượt quá giá trị timeout được chỉ định (SYSTICK_GetTick() - tickstart >= timeout) 
                     * và timeout không phải là giá trị đặc biệt SYSTICK_LOAD_MAX_RELOAD_VALUE (được sử dụng để biểu thị chờ vô hạn).
                     * 2. Giá trị timeout được chỉ định là 0, có nghĩa là không chờ đợi và trả về lỗi timeout ngay lập tức nếu cờ RXNE không được set.
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
                      return STAT_TIMEOUT; // Nhận dữ liệu thất bại do timeout
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

    // Tắt SPI để đảm bảo không nhận thêm dữ liệu

      if (
        IS_MASTER(hspi)
        &&
        (IS_FD(hspi) || IS_HDRX(hspi))
      ) {
        SPI_SPE_DISABLE();
      }

    // Báo trạng thái hoàn thành

      hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình nếu cần thiết

    // Kiểm tra kết thúc transaction

      if (SPI_EndRxTransaction(hspi, timeout, tickstart) != STAT_OK) {
        return STAT_TIMEOUT; // Nhận dữ liệu thất bại do timeout khi chờ kết thúc transaction
      }

    // Kiểm tra mã lỗi

      if (hspi->ErrorCode != SPI_ERR_OK) {
        return STAT_ERROR; // Nhận dữ liệu thất bại do lỗi đã được cập nhật vào handle_param
      }

    return STAT_OK; // Nhận dữ liệu thành công
  }

  RETR_STAT SPI_TransmitReceive_Norm(
    SPI_Handle_Param *hspi, 
    const ui8* pdata_tx, 
    ui8* pdata_rx, 
    ui16 size, 
    ui32 timeout
  ) {

    // Kiểm tra tham số đầu vào hợp lệ

      /**
       * Ghi chú:
       * Phần này được kiểm tra ở Dispatcher nên sẽ không cần kiểm tra lại ở đây nữa.
       */

    // Khai báo quản lý thời gian

      ui32 tickstart = 0;

    // Khai báo lưu trạng thái cục bộ

      ui32 tmp_state = hspi->State;
      ui32 tmp_mode = hspi->Init.Mode;
      ui32 tmp_direction = hspi->Init.Direction;

    // Khai báo lưu số lần truyền dữ liệu đã thực hiện

      ui16 initial_TxXferCount = size;

    // Khai báo biến xác định chuyển đổi TX và RX

      ui8 is_TxPhase = 1u; // Biến cờ để xác định đang ở pha truyền (1) hay pha nhận (0), mặc định bắt đầu ở pha truyền

    // Kiểm tra chế độ truyền

      assert_param(IS_FD(hspi)); // Chỉ cho phép truyền nhận dữ liệu khi ở chế độ Full-Duplex

    // Kiểm tra trạng thái

      if (!( // Nếu không thỏa mãn các điều kiện sau thì trả về trạng thái bận
        tmp_state == SPI_READY 
        || ( // Không thỏa mãn ở Full-Duplex Master với RX đang bận thì không thực thi
          (tmp_state == SPI_BUSY_RX) 
          && 
          (tmp_mode == SPI_MODE_MASTER) 
          && 
          (tmp_direction == SPI_DIRECTION_2LINES)
        )
      )) {
        return STAT_BUSY;
      }

    // Đảm bảo không overwrite state nếu sử dụng hàm này sau khi gọi SPI_Receive

      if (hspi->State != SPI_BUSY_RX) {
        hspi->State = SPI_BUSY_TX_RX; // Cập nhật trạng thái về Busy_TX_RX để báo đang truyền nhận dữ liệu
      }

      /**
       * Ghi chú:
       * Trường này đảm bảo ở 2 nhánh sau:
       * - Nếu sử dụng hàm này khi mới khởi tạo thì cập nhật để báo đang ở trạng thái truyền nhận dữ liệu
       * - Nếu có xảy ra RX transaction trước đó ở chế độ Full-Duplex Master thì 
       * vẫn cho phép tiếp tục sử dụng hàm này mà không cần cập nhật lại trạng thái vì đã ở trạng thái bận nhận dữ liệu, chỉ cần chuyển sang trạng thái bận truyền nhận dữ liệu mà không cần phải cập nhật lại trạng thái từ Busy_RX về Busy_TX_RX để tránh việc overwrite trạng thái Busy_RX vốn đã báo đang có transaction RX đang diễn ra, đảm bảo có thể tiếp tục thực hiện chức năng truyền nhận dữ liệu mà không bị gián đoạn do việc cập nhật lại trạng thái.
       */

    // Cấu hình thông tin truyền nhận

      // Cập nhật lỗi

        hspi->ErrorCode = SPI_ERR_OK;

      // Cấu hình thông tin truyền 

        hspi->Tx_Buff_Ptr = (const ui8*)pdata_tx; // enforce const để đảm bảo dữ liệu truyền đi không bị thay đổi
        hspi->Tx_Xfer_Size = size;
        hspi->Tx_Xfer_Count = size;

      // Cấu hình thông tin nhận

        hspi->Rx_Buff_Ptr = (ui8*)pdata_rx;
        hspi->Rx_Xfer_Size = size;
        hspi->Rx_Xfer_Count = size;

      // Cấu hình ISR

        hspi->TxISR = NULL;
        hspi->RxISR = NULL;

    // Kích hoạt SPI nếu chưa được kích hoạt 

      if (
        __DIFF_CHECK(
          SPI_SPE_GET(),
          SPI_CR1_SPE_MASK
        ) // Nếu SPI chưa được kích hoạt (SPE = 0) thì mới kích hoạt
      ) {
        SPI_SPE_ENABLE();
      }

    // Cập nhật thời điểm bắt đầu truyền nhận dữ liệu để phục vụ cho việc kiểm tra timeout sau này

      tickstart = SYSTICK_GetTick();

    // Truyền nhận dữ liệu

      switch (hspi->Init.DataSize) {
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
              IS_SLAVE(hspi)
              || 
              (initial_TxXferCount == 0x01u)
            ) {
              hspi->Instance->SPI_DR = *((const ui16*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 16-bit
              hspi->Tx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer truyền đi (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
              hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã preload 1 phần tử)
              is_TxPhase = 0u; // Chuyển sang pha nhận sau preload để cho phép đọc lại dữ liệu của khung đầu tiên
            }

          // Xử lý truyền nhận

            while (
              (hspi->Tx_Xfer_Count > 0) 
              || 
              (hspi->Rx_Xfer_Count > 0)
            ) {
              // Kiểm tra cờ TXE

                if (
                  SPI_TXE_GET() // Đảm bảo TXE != 0 báo TxBuf trống
                  &&
                  (hspi->Tx_Xfer_Count > 0) // Chỉ truyền dữ liệu khi vẫn còn dữ liệu cần truyền
                  &&
                  is_TxPhase // Chỉ truyền dữ liệu khi đang ở pha truyền
                ) {
                  hspi->Instance->SPI_DR = *((const ui16*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 16-bit
                  hspi->Tx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer truyền đi (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
                  hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã truyền đi 1 phần tử)
                  is_TxPhase = 0u; // Chuyển sang pha nhận sau khi truyền đi 1 phần tử dữ liệu
                }

              // Kiểm tra cờ RXNE

                if (
                  SPI_RXNE_GET() // Đảm bảo RXNE != 0 báo RxBuf có dữ liệu
                  &&
                  (hspi->Rx_Xfer_Count > 0) // Chỉ nhận dữ liệu khi vẫn còn dữ liệu cần nhận
                  &&
                  !is_TxPhase // Chỉ nhận dữ liệu khi đang ở pha nhận
                ) { 
                  *((ui16*)hspi->Rx_Buff_Ptr) = hspi->Instance->SPI_DR; // Đọc dữ liệu ràng buộc casting 16-bit
                  hspi->Rx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer nhận vào (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
                  hspi->Rx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần nhận (giảm đi 1 phần tử vì đã nhận được 1 phần tử)
                  is_TxPhase = 1u; // Chuyển sang pha truyền sau khi nhận được 1 phần tử dữ liệu
                }

              // Kiểm tra timeout

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
                  return STAT_TIMEOUT; // Nhận dữ liệu thất bại do timeout
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
              IS_SLAVE(hspi)
              || 
              (initial_TxXferCount == 0x01u)
            ) {
              *((__vo ui8*)hspi->Instance->SPI_DR) = *((const ui8*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 8-bit
              hspi->Tx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer truyền đi (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
              hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã preload 1 phần tử)
              is_TxPhase = 0u; // Chuyển sang pha nhận sau preload để cho phép đọc lại dữ liệu của khung đầu tiên
            }

          // Xử lý truyền nhận

            while (
              (hspi->Tx_Xfer_Count > 0) 
              || 
              (hspi->Rx_Xfer_Count > 0)
            ) {
              // Kiểm tra cờ TXE

                if (
                  SPI_TXE_GET() // Đảm bảo TXE != 0 báo TxBuf trống
                  &&
                  (hspi->Tx_Xfer_Count > 0) // Chỉ truyền dữ liệu khi vẫn còn dữ liệu cần truyền
                  &&
                  is_TxPhase // Chỉ truyền dữ liệu khi đang ở pha truyền
                ) {
                  *((__vo ui8*)hspi->Instance->SPI_DR) = *((const ui8*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 8-bit
                  hspi->Tx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer truyền đi (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
                  hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã truyền đi 1 phần tử)
                  is_TxPhase = 0u; // Chuyển sang pha nhận sau khi truyền đi 1 phần tử dữ liệu
                }

              // Kiểm tra cờ RXNE

                if (
                  SPI_RXNE_GET() // Đảm bảo RXNE != 0 báo RxBuf có dữ liệu
                  &&
                  (hspi->Rx_Xfer_Count > 0) // Chỉ nhận dữ liệu khi vẫn còn dữ liệu cần nhận
                  &&
                  !is_TxPhase // Chỉ nhận dữ liệu khi đang ở pha nhận
                ) { 
                  *((ui8*)hspi->Rx_Buff_Ptr) = hspi->Instance->SPI_DR; // Đọc dữ liệu ràng buộc casting 8-bit
                  hspi->Rx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer nhận vào (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
                  hspi->Rx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần nhận (giảm đi 1 phần tử vì đã nhận được 1 phần tử)
                  is_TxPhase = 1u; // Chuyển sang pha truyền sau khi nhận được 1 phần tử dữ liệu
                }

              // Kiểm tra timeout

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
                  return STAT_TIMEOUT; // Nhận dữ liệu thất bại do timeout
                }
            }

          break;

        default:
          hspi->ErrorCode = SPI_ERROR_DATASIZE; // Cập nhật mã lỗi vào handle_param
          hspi->State = SPI_RESET; // Cập nhật trạng thái về Reset để cho phép người dùng khởi tạo lại cấu hình
          return STAT_ERROR; // Kích thước dữ liệu không hợp lệ
          break;
      }

      /**
       * Ghi chú:
       * Vì sao phải có cơ chế ping-pong khi nhận, không làm TX/RX “độc lập”?
       * - SPI là đồng bộ theo xung clock từ Master,
       * Không có clock thì Slave không thể đẩy bit ra MISO.
       * - Mỗi lần Master dịch 1 bit ra MOSI thì đồng thời cũng dịch 1 bit vào MISO,
       * Nghĩa là về phần cứng, TX và RX luôn ghép cặp theo từng khung.
       * - Nếu chỉ “chờ RXNE” mà không tạo clock (bằng ghi DR) sẽ treo vì dữ liệu không bao giờ đến.
       * - Nếu chỉ TX liên tục mà không đọc RX kịp, RX buffer đầy gây OVR; 
       * khi OVR set thì luồng truyền tiếp theo có thể bị kẹt/timeout.
       * - Ping-pong giải quyết cả hai:
       *  - Đảm bảo luôn có clock để nhận.
       *  - Đảm bảo đọc RX ngay sau mỗi nhịp TX để tránh OVR.
       *  - Giữ tiến trình full-duplex ổn định theo từng frame.
       * - Nói ngắn gọn: “Receive-only” trên SPI Master 2-lines là khái niệm ở API, còn ở mức bus vật lý thì vẫn phải phát dữ liệu giả để kéo clock, nên bắt buộc dùng mô hình transmit-receive theo kiểu ping-pong.
       */

      // Kiểm tra kết thúc transaction

        if (SPI_EndRxTxTransaction(hspi, timeout, tickstart) != STAT_OK) {
          return STAT_TIMEOUT; // Truyền nhận dữ liệu thất bại do timeout khi chờ kết thúc transaction
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
          IS_FD(hspi) // Chỉ xử lý cờ OVR nếu ở chế độ Full-Duplex vì chỉ có chế độ này mới có sự tham gia của RxBuf 
        ) {
          {
            __vo ui32 tmp = 0x0u;
            tmp = hspi->Instance->SPI_DR; // Đọc DR
            tmp = hspi->Instance->SPI_SR; // Đọc SR để xóa cờ OVR
            tmp = 0u;
          }
        }

      // Báo trạng thái hoàn thành

        hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình nếu cần thiết

      // Kiểm tra mã lỗi

        if (hspi->ErrorCode != SPI_ERR_OK) {
          return STAT_ERROR; // Truyền dữ liệu thất bại do lỗi đã được cập nhật vào handle_param
        }

      // Truyền dữ liệu thành công

        return STAT_OK;
  }

  sta RETR_STAT SPI_Transmit_Intr(
    SPI_Handle_Param *hspi, 
    const ui8 *pdata, 
    ui16 size
  ) {

    // Kiểm tra tham số đầu vào hợp lệ

      /**
       * Ghi chú:
       * Phần này được kiểm tra ở Dispatcher nên sẽ không cần kiểm tra lại ở đây nữa.
       */

    // Đảm bảo kiểm tra SPI ready trước khi bắt đầu truyền dữ liệu

      if (hspi->State != SPI_READY) {
        return STAT_BUSY;
      }

    // Kiểm tra direction

      if (!(IS_FD(hspi) || IS_HDTX(hspi))) {
        return STAT_ERROR; // Chỉ cho phép truyền dữ liệu khi ở chế độ Full-Duplex hoặc Half-Duplex TX
      }

      /**
       * Ghi chú:
       * Trong HAL tham khảo sẽ có thêm phần xử lý đường truyền 1 line, 
       * nhưng ở đây vì đã kiểm tra direction ở trên nên chỉ cần bỏ qua.
       */

    // Cấu hình thông tin truyền nhận

      // Cập nhật trạng thái và lỗi

        hspi->State = SPI_BUSY_TX; // Cập nhật trạng thái về Busy_TX để báo đang truyền dữ liệu
        hspi->ErrorCode = SPI_ERR_OK;

      // Cấu hình thông tin truyền 

        hspi->Tx_Buff_Ptr = (const ui8*)pdata; // enforce const để đảm bảo dữ liệu truyền đi không bị thay đổi
        hspi->Tx_Xfer_Size = size;
        hspi->Tx_Xfer_Count = size;

      // Cấu hình thông tin nhận 
      
        hspi->Rx_Buff_Ptr = (ui8*)NULL;
        hspi->Rx_Xfer_Size = 0u;
        hspi->Rx_Xfer_Count = 0u;

        /**
         * Ghi chú:
         * Mặc dù không set tham số cho RxBuf nhưng
         * khi hoạt động thì vẫn sẽ có sự tham gia của RxBuf
         * dẫn đến việc thu dữ liệu rác làm cờ RXNE luôn được set 
         * sau khi truyền đi 1 phần tử dữ liệu, do đó sẽ có xử lý để đảm bảo RxBuf vẫn hoạt động 
         * ngay cả khi không có dữ liệu thực tế để nhận vào.
         */

      // Cấu hình ISR

        hspi->TxISR = SPI_1lineTxISR_8BIT; // Gán ISR truyền dữ liệu hoàn thành chung cho cả 2 chế độ Full-Duplex và Half-Duplex TX vì cả 2 chế độ này đều có cùng cách thức hoạt động của TxBuf
        hspi->RxISR = SPI_1lineRxISR_8BIT; // Gán ISR nhận dữ liệu hoàn thành chung cho cả 2 chế độ Full-Duplex và Half-Duplex TX vì cả 2 chế độ này đều có cùng cách thức hoạt động của RxBuf
        
        /**
         * Ghi chú:
         * Do hàm truyền là non-blocking với interrupt nên 
         * sẽ cần cấu hình ISR để xử lý khi truyền dữ liệu hoàn thành,
         * mặc dù là TX nhưng vẫn gán cho Rx_ISR để 
         * không bị lỗi. 
         * Ngoài ra ở bên dưới sẽ có gán ISR theo DFF 
         * để đảm bảo ISR phù hợp với kích thước dữ liệu được cấu hình,
         * nên là ở khu vực này sẽ gán tạm ISR.
         */

    // Gán ISR theo DFF

      if (IS_16BIT(hspi)) {
        hspi->TxISR = SPI_1lineTxISR_16BIT;
      } else {
        hspi->TxISR = SPI_1lineTxISR_8BIT;
      }

    // Kích hoạt SPI nếu chưa được kích hoạt 

      if (
        __DIFF_CHECK(
          SPI_SPE_GET(),
          SPI_CR1_SPE_MASK
        ) // Nếu SPI chưa được kích hoạt (SPE = 0) thì mới kích hoạt
      ) {
        SPI_SPE_ENABLE();
      }

    // Kích hoạt interrupt TXE 

      SPI_TXE_ENABLE();

    // Kích hoạt interrupt Error 

      SPI_ERR_ENABLE();

    // Trả về trạng thái đã khởi động truyền dữ liệu bằng interrupt

      return STAT_OK;

  }

  sta RETR_STAT SPI_Receive_Intr(
    SPI_Handle_Param *hspi, 
    ui8 *pdata, 
    ui16 size
  ) {

    // Kiểm tra tham số đầu vào hợp lệ

      /**
       * Ghi chú:
       * Phần này được kiểm tra ở Dispatcher nên sẽ không cần kiểm tra lại ở đây nữa.
       */

    // Đảm bảo kiểm tra SPI ready trước khi bắt đầu nhận dữ liệu

    // Kiểm tra direction

      if (IS_FD(hspi) && IS_MASTER(hspi)) {

        // Cập nhật trạng thái về Busy_RX để báo đang nhận dữ liệu

          hspi->State = SPI_BUSY_RX; 

        // Sử dụng hàm TransmitReceive_Intr để tạo xung clock và nhận dữ liệu

          return SPI_TransmitReceive_Intr(
            hspi, (const ui8*)pdata, pdata, size
          );

          /**
           * Ghi chú:
           * Phần truyền thì vẫn phải truyền vào dữ liệu để tạo xung clock cho Slave
           */
      }

    // Cấu hình thông tin truyền nhận

      // Cập nhật trạng thái và lỗi

        hspi->State = SPI_BUSY_RX; // Cập nhật trạng thái về Busy_RX để báo đang nhận dữ liệu
        hspi->ErrorCode = SPI_ERR_OK;

      // Cấu hình thông tin nhận

        hspi->Rx_Buff_Ptr = (ui8*)pdata;
        hspi->Rx_Xfer_Size = size;
        hspi->Rx_Xfer_Count = size;

      // Cấu hình thông tin truyền 
      
        hspi->Tx_Buff_Ptr = (ui8*)NULL;
        hspi->Tx_Xfer_Size = 0u;
        hspi->Tx_Xfer_Count = 0u;

        /**
         * Ghi chú:
         * Mặc dù không set tham số cho TxBuf nhưng
         * khi hoạt động thì vẫn sẽ có sự tham gia của TxBuf
         * dẫn đến việc tạo xung clock để Slave có thể gửi dữ liệu,
         * do đó sẽ có xử lý để đảm bảo TxBuf vẫn hoạt động 
         * ngay cả khi không có dữ liệu thực tế để truyền đi.
         */

      // Cấu hình ISR

        hspi->TxISR = SPI_1lineTxISR_8BIT; 
        hspi->RxISR = SPI_1lineRxISR_8BIT;

        /**
         * Ghi chú:
         * Do hàm nhận là non-blocking với interrupt nên 
         * sẽ cần cấu hình ISR để xử lý khi nhận dữ liệu hoàn thành,
         * mặc dù là RX nhưng vẫn gán cho Tx_ISR để 
         * không bị lỗi.
         * Ngoài ra ở bên dưới sẽ có gán ISR theo DFF
         * để đảm bảo ISR phù hợp với kích thước dữ liệu được cấu hình,
         * nên là ở khu vực này sẽ gán tạm ISR.
         */

    // Gán ISR theo DFF

      if (IS_16BIT(hspi)) {
        hspi->RxISR = SPI_1lineRxISR_16BIT;
      } else {
        hspi->RxISR = SPI_1lineRxISR_8BIT;
      }

    // Kích hoạt SPI nếu chưa được kích hoạt 

      if (
        __DIFF_CHECK(
          SPI_SPE_GET(),
          SPI_CR1_SPE_MASK
        ) // Nếu SPI chưa được kích hoạt (SPE = 0) thì mới kích hoạt
      ) {
        SPI_SPE_ENABLE();
      }

    // Kích hoạt interrupt RXNE

      SPI_RXNE_ENABLE();

    // Kích hoạt interrupt Error

      SPI_ERR_ENABLE();
    
    // Trả về trạng thái đã khởi động nhận dữ liệu bằng interrupt

      return STAT_OK;
  }

  sta RETR_STAT SPI_TransmitReceive_Intr(
    SPI_Handle_Param *hspi, 
    const ui8 *ptx, ui8 *prx, 
    ui16 size
  ) {

    // Kiểm tra tham số đầu vào hợp lệ

      /**
       * Ghi chú:
       * Phần này được kiểm tra ở Dispatcher nên sẽ không cần kiểm tra lại ở đây nữa.
       */
    
    // Lưu cấu hình tạm thời

      ui32 tmp_mode = hspi->Init.Mode;
      ui32 tmp_direction = hspi->Init.Direction;
      ui32 tmp_state = hspi->State;

    // Kiểm tra direction

      if (tmp_direction != SPI_DIRECTION_2LINES) {
        return STAT_ERROR; // Chỉ cho phép truyền nhận dữ liệu khi ở chế độ Full-Duplex
      }

    // Kiểm tra trạng thái và cấu hình để đảm bảo có thể sử dụng hàm TransmitReceive_Intr ngay sau khi gọi hàm Receive_Intr mà không bị lỗi busy

      if (!(
        tmp_state == SPI_READY 
        ||
        (
          (tmp_state == SPI_BUSY_RX) 
          && 
          (tmp_mode == SPI_MODE_MASTER) 
          && 
          (tmp_direction == SPI_DIRECTION_2LINES)
        )
      )) {
        return STAT_BUSY;
      }

    // Cấu hình thông tin truyền nhận

      // Cập nhật trạng thái và lỗi

        if (tmp_state != SPI_BUSY_RX) {
          hspi->State = SPI_BUSY_TX_RX; // Cập nhật trạng thái về Busy_TX_RX để báo đang truyền nhận dữ liệu
        }
        hspi->ErrorCode = SPI_ERR_OK;

      // Cấu hình thông tin truyền 

        hspi->Tx_Buff_Ptr = (const ui8*)ptx; // enforce const để đảm bảo dữ liệu truyền đi không bị thay đổi
        hspi->Tx_Xfer_Size = size;
        hspi->Tx_Xfer_Count = size;

      // Cấu hình thông tin nhận

        hspi->Rx_Buff_Ptr = (ui8*)prx;
        hspi->Rx_Xfer_Size = size;
        hspi->Rx_Xfer_Count = size;

      // Cấu hình ISR

        hspi->TxISR = SPI_2lineTxISR_8BIT;
        hspi->RxISR = SPI_2lineRxISR_8BIT;

         /**
         * Ghi chú:
         * Do hàm truyền nhận là non-blocking với interrupt nên 
         * sẽ cần cấu hình ISR để xử lý khi truyền nhận dữ liệu hoàn thành,
         * mặc dù là TX/RX nhưng vẫn gán cho Rx_ISR/Tx_ISR để 
         * không bị lỗi.
         * Ngoài ra ở bên dưới sẽ có gán ISR theo DFF
         * để đảm bảo ISR phù hợp với kích thước dữ liệu được cấu hình,
         * nên là ở khu vực này sẽ gán tạm ISR.
         */
    
    // Gán ISR theo DFF

      if (hspi->Init.DataSize == SPI_DATASIZE_16BIT) {
        hspi->TxISR = SPI_2lineTxISR_16BIT;
        hspi->RxISR = SPI_2lineRxISR_16BIT;
      } else {
        hspi->TxISR = SPI_2lineTxISR_8BIT;
        hspi->RxISR = SPI_2lineRxISR_8BIT;
      }

    // Kích hoạt SPI nếu chưa được kích hoạt 

      if (
        __DIFF_CHECK(
          SPI_SPE_GET(),
          SPI_CR1_SPE_MASK
        ) // Nếu SPI chưa được kích hoạt (SPE = 0) thì mới kích hoạt
      ) {
        SPI_SPE_ENABLE();
      }

    // Kích hoạt interrupt TXE 

      SPI_TXE_ENABLE();

    // Kích hoạt interrupt RXNE

      SPI_RXNE_ENABLE();

    // Kích hoạt interrupt Error

      SPI_ERR_ENABLE();
    
    // Trả về trạng thái đã khởi động nhận dữ liệu bằng interrupt

      return STAT_OK;

  }

  sta void SPI_1lineTxISR_8BIT(SPI_Handle_Param *hspi) {

    // Lưu data vào DR để tạo xung clock

      *((__vo ui8*)&hspi->Instance->SPI_DR) = *((const ui8*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 8-bit
      hspi->Tx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer truyền đi (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
      hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã truyền đi 1 phần tử)

    // Kiểm tra nếu đã truyền xong hết dữ liệu thì tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

      if (hspi->Tx_Xfer_Count == 0u) {
        SPI_CloseTx_ISR(hspi); // Gọi xử lý đóng 1line_TxISR để tắt interrupt TXE và thực hiện các thao tác hoàn thành truyền dữ liệu nếu cần thiết
      }
  }

  sta void SPI_1lineTxISR_16BIT(SPI_Handle_Param *hspi) {

    // Lưu data vào DR để tạo xung clock

      hspi->Instance->SPI_DR = *((const ui16*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 16-bit
      hspi->Tx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer truyền đi (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
      hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã truyền đi 1 phần tử)

    // Kiểm tra nếu đã truyền xong hết dữ liệu thì tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

      if (hspi->Tx_Xfer_Count == 0u) {
        SPI_CloseTx_ISR(hspi); // Gọi xử lý đóng 1line_TxISR để tắt interrupt TXE và thực hiện các thao tác hoàn thành truyền dữ liệu nếu cần thiết
      }
  }

  sta void SPI_1lineRxISR_8BIT(SPI_Handle_Param *hspi) {

    // Đọc data từ DR

      *((ui8*)hspi->Rx_Buff_Ptr) = hspi->Instance->SPI_DR; // Đọc dữ liệu ràng buộc casting 8-bit
      hspi->Rx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer nhận vào (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
      hspi->Rx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần nhận (giảm đi 1 phần tử vì đã nhận được 1 phần tử)

    // Kiểm tra nếu đã nhận xong hết dữ liệu thì tắt interrupt RXNE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để nhận

      if (hspi->Rx_Xfer_Count == 0u) {
        SPI_CloseRx_ISR(hspi); // Gọi xử lý đóng 1line_RxISR để tắt interrupt RXNE và thực hiện các thao tác hoàn thành nhận dữ liệu nếu cần thiết
      }
  }

  sta void SPI_1lineRxISR_16BIT(SPI_Handle_Param *hspi) {

    // Đọc data từ DR

      *((ui16*)hspi->Rx_Buff_Ptr) = hspi->Instance->SPI_DR; // Đọc dữ liệu ràng buộc casting 16-bit
      hspi->Rx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer nhận vào (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
      hspi->Rx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần nhận (giảm đi 1 phần tử vì đã nhận được 1 phần tử)

    // Kiểm tra nếu đã nhận xong hết dữ liệu thì tắt interrupt RXNE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để nhận

      if (hspi->Rx_Xfer_Count == 0u) {
        SPI_CloseRx_ISR(hspi); // Gọi xử lý đóng 1line_RxISR để tắt interrupt RXNE và thực hiện các thao tác hoàn thành nhận dữ liệu nếu cần thiết
      }
  }

  sta RETR_STAT SPI_EndTxTransaction(
    SPI_Handle_Param *hspi, 
    ui32 timeout, 
    ui32 tickstart
  ) {
    // Đợi cờ TXE

      if (
        SPI_FlagTimeout(
          hspi,SPI_SR_TXE_MASK,SET,
          timeout,tickstart
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
        return STAT_TIMEOUT; // Truyền dữ liệu thất bại do timeout
      }

    // Đợi cờ BSY

      if (
        SPI_FlagTimeout(
          hspi,SPI_SR_BSY_MASK,RESET,
          timeout,tickstart
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
        return STAT_TIMEOUT; // Truyền dữ liệu thất bại do timeout
      }

    return STAT_OK; // Truyền dữ liệu thành công
  }

  sta RETR_STAT SPI_EndRxTransaction(
    SPI_Handle_Param *hspi, 
    ui32 timeout, 
    ui32 tickstart
  ) {
    // Kiểm tra trường hợp Master Receiver

      if (
        IS_MASTER(hspi) 
        && 
        (IS_FDRX(hspi) || IS_HDRX(hspi))
      ) {
        SPI_SPE_DISABLE(); // Tắt SPI để Master Receiver có thể đọc dữ liệu cuối cùng mà không bị thiếu xung clock, tránh tình trạng treo khi chờ cờ RXNE ở lần nhận cuối cùng
      }

    // Với trường hợp Master Receiver, kiểm tra RXNE cuối truyền

      if (IS_MASTER(hspi) && IS_FDRX(hspi)) {
        if (
          SPI_FlagTimeout(
            hspi,SPI_SR_RXNE_MASK,RESET,
            timeout,tickstart
          ) != STAT_OK
        ) {
          hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
          return STAT_TIMEOUT; // Nhận dữ liệu thất bại do timeout
        }
      } else { // Với các trường hợp khác, chỉ cần kiểm tra cờ BSY để đảm bảo đã kết thúc hoàn toàn giao tiếp
        if (
          SPI_FlagTimeout(
            hspi,SPI_SR_BSY_MASK,RESET,
            timeout,tickstart
          ) != STAT_OK
        ) {
          hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
          return STAT_TIMEOUT; // Nhận dữ liệu thất bại do timeout
        }
      }

    return STAT_OK; // Nhận dữ liệu thành công
  }

  sta RETR_STAT SPI_EndRxTxTransaction(
    SPI_Handle_Param *hspi, 
    ui32 timeout, 
    ui32 tickstart
  ) {

    // Tắt INTR-TXE
      
      SPI_TXE_DISABLE(); // Tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

    // Tắt INTR-ERR
    
      SPI_ERR_DISABLE(); // Tắt interrupt Error để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền dữ liệu

    // Tắt INTR-RXNE

      SPI_RXNE_DISABLE(); // Tắt interrupt RXNE để tránh bị gọi lại ISR liên tục khi đã hoàn thành nhận dữ liệu

    // Đợi cờ TXE

      if (
        SPI_FlagTimeout(
          hspi,SPI_SR_TXE_MASK,SET,
          timeout,tickstart
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
        return STAT_TIMEOUT; // Truyền dữ liệu thất bại do timeout
      }

    // Đợi cờ BSY

      if (
        SPI_FlagTimeout(
          hspi,SPI_SR_BSY_MASK,RESET,
          timeout,tickstart
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
        return STAT_TIMEOUT; // Truyền dữ liệu thất bại do timeout
      }

    return STAT_OK; // Truyền dữ liệu thành công
  }

  sta void SPI_CloseTx_ISR(SPI_Handle_Param *hspi) {
    if (
      SPI_FlagTimeout( // Đợi cờ TXE
        hspi,SPI_SR_TXE_MASK,SET,
        SYSTICK_LOAD_MAX_RELOAD_VALUE, // Không timeout, đợi mãi cho đến khi cờ TXE được set
        SYSTICK_GetTick()
      ) == STAT_OK
    ) { // Nếu cờ TXE trả về trong timeout

      // Tắt INTR-TXE
      
        SPI_TXE_DISABLE(); // Tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

      // Tắt INTR-ERR
      
        SPI_ERR_DISABLE(); // Tắt interrupt Error để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền dữ liệu

      // Kiểm tra transaction đã hoàn thành hay chưa

        if (
          SPI_EndTxTransaction(
            hspi,
            SYSTICK_LOAD_MAX_RELOAD_VALUE, // Không timeout, đợi mãi cho đến khi transaction kết thúc hoàn toàn
            SYSTICK_GetTick()
          ) != STAT_OK
        ) {
          hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
        }

      // Kiểm tra xóa cờ OVR nếu là truyền dữ liệu ở chế độ Full-Duplex

        if (
          IS_FD(hspi) // Chỉ xử lý cờ OVR nếu ở chế độ Full-Duplex vì chỉ có chế độ này mới có sự tham gia của RxBuf
        ) {
          {
            __vo ui32 tmp = 0x0u;
            tmp = hspi->Instance->SPI_DR; // Đọc DR
            tmp = hspi->Instance->SPI_SR; // Đọc SR để xóa cờ OVR
            tmp = 0u;
          }
        }

      // Báo trạng thái hoàn thành

        hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình nếu cần thiết

      // Kiểm tra sử dụng callback hoàn thành truyền dữ liệu

        if (hspi->ErrorCode != SPI_ERR_OK) {
          #if (SPI_PUBLIC_CALLBACK_ENABLE == 1u) 
            if (hspi->Error_Callback != NULL) {
              hspi->Error_Callback(hspi); // Gọi callback xử lý lỗi nếu có lỗi xảy ra trong quá trình truyền dữ liệu
            } else {
              Error_Callback(hspi);
            }
          #else
            Error_Callback(hspi); // Gọi callback xử lý lỗi mặc định nếu có lỗi xảy ra trong quá trình truyền dữ liệu
          #endif
        } else {
          #if (SPI_PUBLIC_CALLBACK_ENABLE == 1u) 
            if (hspi->Tx_Cplt_Callback != NULL) {
              hspi->Tx_Cplt_Callback(hspi); // Gọi callback xử lý hoàn thành truyền dữ liệu
            } else {
              Tx_Cplt_Callback(hspi);
            }
          #else
            Tx_Cplt_Callback(hspi); // Gọi callback xử lý hoàn thành truyền dữ liệu mặc định 
          #endif
        }

    } else { // Nếu cờ TXE không trả về trong timeout 
      
      // Tắt INTR-TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền dù đã timeout

        SPI_TXE_DISABLE(); // Tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

      // Tắt INTR-ERR để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền dữ liệu dù đã timeout

        SPI_ERR_DISABLE(); // Tắt interrupt Error để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền dữ liệu

      // Cập nhật lỗi timeout vào handle_param

        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param

      // Báo trạng thái hoàn thành dù có lỗi để user có thể thực hiện các thao tác sau khi truyền xong nếu cần thiết

        hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình nếu cần thiết

      // Kiểm tra sử dụng callback hoàn thành truyền dữ liệu

        #if (SPI_PUBLIC_CALLBACK_ENABLE == 1u) 
          if (hspi->Error_Callback != NULL) {
            hspi->Error_Callback(hspi); // Gọi callback xử lý lỗi nếu có lỗi xảy ra trong quá trình truyền dữ liệu
          } else {
            Error_Callback(hspi);
          }
        #else
          Error_Callback(hspi); // Gọi callback xử lý lỗi mặc định nếu có lỗi xảy ra trong quá trình truyền dữ liệu
        #endif
    }
  }

  sta void SPI_CloseRx_ISR(SPI_Handle_Param *hspi) {

    // Tắt INTR-RXNE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để nhận

      SPI_RXNE_DISABLE(); // Tắt interrupt RXNE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để nhận

    // Tắt INTR-ERR để tránh bị gọi lại ISR liên tục khi đã hoàn thành nhận dữ liệu

      SPI_ERR_DISABLE(); // Tắt interrupt Error để tránh bị gọi lại ISR liên tục khi đã hoàn thành nhận dữ liệu

    // Kiểm tra transaction đã hoàn thành hay chưa

      if (
        SPI_EndRxTransaction(
          hspi,
          SYSTICK_LOAD_MAX_RELOAD_VALUE, // Không timeout, đợi mãi cho đến khi transaction kết thúc hoàn toàn
          SYSTICK_GetTick()
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
      }

    // Xóa cờ OVR nếu là truyền dữ liệu ở chế độ Full-Duplex

      if (
        hspi->Init.Direction == SPI_DIRECTION_2LINES 
      ) {
        {
          __vo ui32 tmp = 0x0u;
          tmp = hspi->Instance->SPI_DR; // Đọc DR
          tmp = hspi->Instance->SPI_SR; // Đọc SR để xóa cờ OVR
          tmp = 0u;
        }
      }

    // Báo trạng thái hoàn thành

      hspi->State = SPI_READY; // Cập nhật trạng thái về Ready để cho phép người dùng khởi tạo lại cấu hình nếu cần thiết

    // Kiểm tra sử dụng callback hoàn thành nhận dữ liệu

      if (hspi->ErrorCode != SPI_ERR_OK) {
        #if (SPI_PUBLIC_CALLBACK_ENABLE == 1u) 
          if (hspi->Error_Callback != NULL) {
            hspi->Error_Callback(hspi); // Gọi callback xử lý lỗi nếu có lỗi xảy ra trong quá trình nhận dữ liệu
          } else {
            Error_Callback(hspi);
          }
        #else
          Error_Callback(hspi); // Gọi callback xử lý lỗi mặc định nếu có lỗi xảy ra trong quá trình nhận dữ liệu
        #endif
      } else {
        #if (SPI_PUBLIC_CALLBACK_ENABLE == 1u) 
          if (hspi->Rx_Cplt_Callback != NULL) {
            hspi->Rx_Cplt_Callback(hspi); // Gọi callback xử lý hoàn thành nhận dữ liệu
          } else {
            Rx_Cplt_Callback(hspi);
          }
        #else
          Rx_Cplt_Callback(hspi); // Gọi callback xử lý hoàn thành nhận dữ liệu mặc định 
        #endif
      }

  }

  sta void SPI_CloseRxTx_ISR(SPI_Handle_Param *hspi) {

    // Tắt INTR-TXE
      
      SPI_TXE_DISABLE(); // Tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

    // Tắt INTR-ERR
    
      SPI_ERR_DISABLE(); // Tắt interrupt Error để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền dữ liệu

    // Tắt INTR-RXNE để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền nhận dữ liệu

      SPI_RXNE_DISABLE(); // Tắt interrupt RXNE để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền nhận dữ liệu

    // Kiểm tra cờ TXE để đảm bảo đã truyền xong hết dữ liệu

      if (
        SPI_FlagTimeout(
          hspi,SPI_SR_TXE_MASK,SET,
          SYSTICK_LOAD_MAX_RELOAD_VALUE, // Không timeout, đợi mãi cho đến khi cờ TXE được set
          SYSTICK_GetTick()
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
      }
    
    // Kiểm tra hoàn thành transaction hay chưa

      if (
        SPI_EndRxTxTransaction(
          hspi,
          SYSTICK_LOAD_MAX_RELOAD_VALUE, // Không timeout, đợi mãi cho đến khi transaction kết thúc hoàn toàn
          SYSTICK_GetTick()
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
      }
    
    // Xóa cờ OVR nếu là truyền dữ liệu ở chế độ Full-Duplex

      if (
        IS_FD(hspi)
      ) {
        {
          __vo ui32 tmp = 0x0u;
          tmp = hspi->Instance->SPI_DR; // Đọc DR
          tmp = hspi->Instance->SPI_SR; // Đọc SR để xóa cờ OVR
          tmp = 0u;
        }
      }

    // Báo trạng thái hoàn thành

      hspi->State = SPI_READY;

    // Kiểm tra sử dụng callback hoàn thành truyền nhận dữ liệu

      if (hspi->ErrorCode != SPI_ERR_OK) {
        #if (SPI_PUBLIC_CALLBACK_ENABLE == 1u) 
          if (hspi->Error_Callback != NULL) {
            hspi->Error_Callback(hspi); // Gọi callback xử lý lỗi nếu có lỗi xảy ra trong quá trình truyền nhận dữ liệu
          } else {
            Error_Callback(hspi);
          }
        #else
          Error_Callback(hspi); // Gọi callback xử lý lỗi mặc định nếu có lỗi xảy ra trong quá trình truyền nhận dữ liệu
        #endif
      } else {
        #if (SPI_PUBLIC_CALLBACK_ENABLE == 1u) 
          if (hspi->TxRx_Cplt_Callback != NULL) {
            hspi->TxRx_Cplt_Callback(hspi); // Gọi callback xử lý hoàn thành truyền nhận dữ liệu
          } else {
            TxRx_Cplt_Callback(hspi);
          }
        #else
          TxRx_Cplt_Callback(hspi); // Gọi callback xử lý hoàn thành truyền nhận dữ liệu mặc định 
        #endif
      }
  }

  sta void SPI_2lineTxISR_8BIT(SPI_Handle_Param *hspi) {
    // Lưu data vào DR để tạo xung clock

      *((__vo ui8*)&hspi->Instance->SPI_DR) = *((const ui8*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 8-bit
      hspi->Tx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer truyền đi (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
      hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã truyền đi 1 phần tử)

    // Kiểm tra nếu đã truyền xong hết dữ liệu thì tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

      if (hspi->Tx_Xfer_Count == 0u) {

        // Tắt INTR-TXE

          SPI_TXE_DISABLE(); // Tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

        // Kiểm tra RxXferCount để khớp cặp hoàn tất truyền nhận dữ liệu

          if (hspi->Rx_Xfer_Count == 0u) {

            /**
             * Ghi chú:
             * Ở khu vực này thì Tx_Xfer_Count đã được đảm bảo bằng 0
             */

            SPI_CloseRxTx_ISR(hspi); // Gọi xử lý đóng RxTxISR để tắt interrupt RXNE và thực hiện các thao tác hoàn thành truyền nhận dữ liệu nếu cần thiết
          }
      }
  }

  sta void SPI_2lineTxISR_16BIT(SPI_Handle_Param *hspi) {
    // Lưu data vào DR để tạo xung clock

      hspi->Instance->SPI_DR = *((const ui16*)hspi->Tx_Buff_Ptr); // Nạp dữ liệu ràng buộc casting 16-bit
      hspi->Tx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer truyền đi (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
      hspi->Tx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần truyền (giảm đi 1 phần tử vì đã truyền đi 1 phần tử)

    // Kiểm tra nếu đã truyền xong hết dữ liệu thì tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

      if (hspi->Tx_Xfer_Count == 0u) {

        // Tắt INTR-TXE

          SPI_TXE_DISABLE(); // Tắt interrupt TXE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để truyền

        // Tắt INTR-ERR để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền dữ liệu

          SPI_ERR_DISABLE(); // Tắt interrupt Error để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền dữ liệu

        // Kiểm tra RxXferCount để khớp cặp hoàn tất truyền nhận dữ liệu

          if (hspi->Rx_Xfer_Count == 0u) {
            SPI_CloseRxTx_ISR(hspi); // Gọi xử lý đóng RxTxISR để tắt interrupt RXNE và thực hiện các thao tác hoàn thành truyền nhận dữ liệu nếu cần thiết
          }
      }
  }

  sta void SPI_2lineRxISR_8BIT(SPI_Handle_Param *hspi) {
    // Đọc data từ DR

      *((ui8*)hspi->Rx_Buff_Ptr) = hspi->Instance->SPI_DR; // Đọc dữ liệu ràng buộc casting 8-bit
      hspi->Rx_Buff_Ptr += sizeof(ui8); // Cập nhật con trỏ buffer nhận vào (tăng lên 1 byte vì kích thước dữ liệu là 8-bit)
      hspi->Rx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần nhận (giảm đi 1 phần tử vì đã nhận được 1 phần tử)

    // Kiểm tra nếu đã nhận xong hết dữ liệu thì tắt interrupt RXNE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để nhận

      if (hspi->Rx_Xfer_Count == 0u) {

        // Tắt INTR-RXNE

          SPI_RXNE_DISABLE(); // Tắt interrupt RXNE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để nhận

        // Kiểm tra TxXferCount để khớp cặp hoàn tất truyền nhận dữ liệu

          if (hspi->Tx_Xfer_Count == 0u) {
            SPI_CloseRxTx_ISR(hspi); // Gọi xử lý đóng RxTxISR để tắt interrupt TXE và thực hiện các thao tác hoàn thành truyền nhận dữ liệu nếu cần thiết
          }
      }
  }

  sta void SPI_2lineRxISR_16BIT(SPI_Handle_Param *hspi) {
    // Đọc data từ DR

      *((ui16*)hspi->Rx_Buff_Ptr) = hspi->Instance->SPI_DR; // Đọc dữ liệu ràng buộc casting 16-bit
      hspi->Rx_Buff_Ptr += sizeof(ui16); // Cập nhật con trỏ buffer nhận vào (tăng lên 2 byte vì kích thước dữ liệu là 16-bit)
      hspi->Rx_Xfer_Count--; // Cập nhật lại số lượng phần tử cần nhận (giảm đi 1 phần tử vì đã nhận được 1 phần tử)

    // Kiểm tra nếu đã nhận xong hết dữ liệu thì tắt interrupt RXNE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để nhận

      if (hspi->Rx_Xfer_Count == 0u) {

        // Tắt INTR-RXNE

          SPI_RXNE_DISABLE(); // Tắt interrupt RXNE để tránh bị gọi lại ISR liên tục khi không còn dữ liệu để nhận

        // Tắt INTR-ERR để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền nhận dữ liệu

          SPI_ERR_DISABLE(); // Tắt interrupt Error để tránh bị gọi lại ISR liên tục khi đã hoàn thành truyền nhận dữ liệu

        // Kiểm tra TxXferCount để khớp cặp hoàn tất truyền nhận dữ liệu

          if (hspi->Tx_Xfer_Count == 0u) {
            SPI_CloseRxTx_ISR(hspi); // Gọi xử lý đóng RxTxISR để tắt interrupt TXE và thực hiện các thao tác hoàn thành truyền nhận dữ liệu nếu cần thiết
          }
      }
  }

  sta RETR_STAT SPI_Abort_Norm(SPI_Handle_Param *hspi) {

    // Chỉ cho phép abort khi ngoại vi đang thực sự bận hoặc đang ở trạng thái abort

      if (
        hspi->State == SPI_READY
        ||
        hspi->State == SPI_RESET
      ) {
        return STAT_ERROR;
      }

    // Cập nhật trạng thái và lỗi

      hspi->State = SPI_ABORT;
      hspi->ErrorCode = SPI_ERR_OK;

    // Tắt toàn bộ ngắt SPI để ngăn transaction mới chen vào

      CLEAR_BIT(
        hspi->Instance->SPI_CR2,
        SPI_CR2_TXEIE_MASK | SPI_CR2_RXNEIE_MASK | SPI_CR2_ERRIE_MASK
      );

    // Đảm bảo khung cuối cùng kết thúc trước khi tắt SPI

      if (
        SPI_FlagTimeout(
          hspi,
          SPI_SR_TXE_MASK,
          SET,
          SYSTICK_LOAD_MAX_RELOAD_VALUE,
          SYSTICK_GetTick()
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT;
      }

      if (
        SPI_FlagTimeout(
          hspi,
          SPI_SR_BSY_MASK,
          RESET,
          SYSTICK_LOAD_MAX_RELOAD_VALUE,
          SYSTICK_GetTick()
        ) != STAT_OK
      ) {
        hspi->ErrorCode = SPI_ERROR_TIMEOUT;
      }

    // Tắt SPI và xóa dữ liệu treo/OVR

      SPI_Disable(hspi);

      {
        __vo ui32 tmp = 0x0u;
        tmp = hspi->Instance->SPI_DR;
        tmp = hspi->Instance->SPI_SR;
        tmp = 0u;
      }

    // Reset thông tin truyền nhận và ISR nội bộ

      hspi->Tx_Xfer_Count = 0u;
      hspi->Rx_Xfer_Count = 0u;
      hspi->Tx_Xfer_Size = 0u;
      hspi->Rx_Xfer_Size = 0u;
      hspi->TxISR = NULL;
      hspi->RxISR = NULL;

    // Hoàn tất trạng thái

      hspi->State = SPI_READY;

    // Callback abort hoàn tất

      #if (SPI_PUBLIC_CALLBACK_ENABLE == 1u)
        if (hspi->Abort_Callback != NULL) {
          hspi->Abort_Callback(hspi);
        } else {
          Abort_Callback(hspi);
        }
      #else
        Abort_Callback(hspi);
      #endif

    // Trả về kết quả

      if (hspi->ErrorCode != SPI_ERR_OK) {
        return STAT_TIMEOUT;
      }

    return STAT_OK;
  }

  sta RETR_STAT SPI_Abort_Intr(SPI_Handle_Param *hspi) {

    // Chỉ cho phép abort khi ngoại vi đang thực sự bận hoặc đang ở trạng thái abort

      if (
        hspi->State == SPI_READY
        ||
        hspi->State == SPI_RESET
      ) {
        return STAT_ERROR;
      }

    // Cập nhật trạng thái và lỗi

      hspi->State = SPI_ABORT;
      hspi->ErrorCode = SPI_ERR_OK;

    // Tắt toàn bộ ngắt SPI ngay lập tức

      CLEAR_BIT(
        hspi->Instance->SPI_CR2,
        SPI_CR2_TXEIE_MASK | SPI_CR2_RXNEIE_MASK | SPI_CR2_ERRIE_MASK
      );

    // Tắt SPI và flush dữ liệu treo

      SPI_Disable(hspi);

      {
        __vo ui32 tmp = 0x0u;
        tmp = hspi->Instance->SPI_DR;
        tmp = hspi->Instance->SPI_SR;
        tmp = 0u;
      }

    // Reset thông tin truyền nhận và ISR nội bộ

      hspi->Tx_Xfer_Count = 0u;
      hspi->Rx_Xfer_Count = 0u;
      hspi->Tx_Xfer_Size = 0u;
      hspi->Rx_Xfer_Size = 0u;
      hspi->TxISR = NULL;
      hspi->RxISR = NULL;

    // Hoàn tất trạng thái

      hspi->State = SPI_READY;

    // Callback abort hoàn tất

      #if (SPI_PUBLIC_CALLBACK_ENABLE == 1u)
        if (hspi->Abort_Callback != NULL) {
          hspi->Abort_Callback(hspi);
        } else {
          Abort_Callback(hspi);
        }
      #else
        Abort_Callback(hspi);
      #endif

    return STAT_OK;
  }

  RETR_STAT SPI_Abort(SPI_Handle_Param *hspi, SPI_TRANS_Enum trans) {

    // Kiểm tra tham số đầu vào hợp lệ

      if (hspi == NULL) {
        return STAT_ERROR;
      }

    // Lựa chọn hàm abort phù hợp dựa trên trans

      switch (trans) {
        case SPI_TRANS_NORM:
          return SPI_Abort_Norm(hspi);

        case SPI_TRANS_INTR:
          return SPI_Abort_Intr(hspi);

        default:
          return STAT_ERROR;
          break;
      }

    return STAT_ERROR;
  }

  RETR_STAT SPI_Transmit(
    SPI_Handle_Param *hspi, 
    const ui8* pdata, 
    ui16 size, 
    ui32 timeout,
    SPI_TRANS_Enum trans
  ) {
    // Kiểm tra tham số đầu vào hợp lệ

      if (hspi == NULL || pdata == NULL || size == 0u) {
        return STAT_ERROR;
      }

      /**
       * Ghi chú:
       * Ở dispatcher chỉ thực hiện kiểm tra NULL,
       * các kiểm tra chi tiết hơn về tham số 
       * sẽ được thực hiện ở các hàm truyền dữ liệu cụ thể 
       * để đảm bảo tính linh hoạt trong việc sử dụng hàm Transmit 
       * cho nhiều mục đích khác nhau 
       * tùy thuộc vào thiết kế phần cứng của user.
       */

    // Lựa chọn hàm truyền dữ liệu phù hợp dựa trên trans

      switch (trans) {
        case SPI_TRANS_NORM: // Chế độ truyền dữ liệu bình thường (blocking)
          return SPI_Transmit_Norm(hspi, pdata, size, timeout);

        case SPI_TRANS_INTR: // Chế độ truyền dữ liệu bằng interrupt (non-blocking)
          return SPI_Transmit_Intr(hspi, pdata, size);
        
        default: // Trường hợp trans không hợp lệ
          return STAT_ERROR; // Trả về lỗi do trans không hợp lệ
          break;
      }

    // Trường hợp mặc định đã return ở trên
    return STAT_ERROR;
  }

  RETR_STAT SPI_Receive(
    SPI_Handle_Param *hspi, 
    ui8* pdata, 
    ui16 size, 
    ui32 timeout,
    SPI_TRANS_Enum trans
  ) {
    // Kiểm tra tham số đầu vào hợp lệ

      if (hspi == NULL || pdata == NULL || size == 0u) {
        return STAT_ERROR;
      }

      /**
       * Ghi chú:
       * Ở dispatcher chỉ thực hiện kiểm tra NULL,
       * các kiểm tra chi tiết hơn về tham số 
       * sẽ được thực hiện ở các hàm nhận dữ liệu cụ thể 
       * để đảm bảo tính linh hoạt trong việc sử dụng hàm Receive 
       * cho nhiều mục đích khác nhau 
       * tùy thuộc vào thiết kế phần cứng của user.
       */

    // Lựa chọn hàm nhận dữ liệu phù hợp dựa trên trans

      switch (trans) {
        case SPI_TRANS_NORM: // Chế độ truyền dữ liệu bình thường (blocking)
          return SPI_Receive_Norm(hspi, pdata, size, timeout);

        case SPI_TRANS_INTR: // Chế độ truyền dữ liệu bằng interrupt (non-blocking)
          return SPI_Receive_Intr(hspi, pdata, size);
        
        default: // Trường hợp trans không hợp lệ
          return STAT_ERROR; // Trả về lỗi do trans không hợp lệ
          break;
      }

    // Trường hợp mặc định đã return ở trên
    return STAT_ERROR;
  }

  RETR_STAT SPI_TransmitReceive(
    SPI_Handle_Param *hspi, 
    const ui8* pdata_tx, 
    ui8* pdata_rx, 
    ui16 size, 
    ui32 timeout,
    SPI_TRANS_Enum trans
  ) {
    // Kiểm tra tham số đầu vào hợp lệ

      if (hspi == NULL || pdata_tx == NULL || pdata_rx == NULL || size == 0u) {
        return STAT_ERROR;
      }

      /**
       * Ghi chú:
       * Ở dispatcher chỉ thực hiện kiểm tra NULL,
       * các kiểm tra chi tiết hơn về tham số 
       * sẽ được thực hiện ở các hàm truyền nhận dữ liệu cụ thể 
       * để đảm bảo tính linh hoạt trong việc sử dụng hàm TransmitReceive 
       * cho nhiều mục đích khác nhau 
       * tùy thuộc vào thiết kế phần cứng của user.
       */

    // Lựa chọn hàm truyền nhận dữ liệu phù hợp dựa trên trans

      switch (trans) {
        case SPI_TRANS_NORM: // Chế độ truyền dữ liệu bình thường (blocking)
          return SPI_TransmitReceive_Norm(hspi, pdata_tx, pdata_rx, size, timeout);

        case SPI_TRANS_INTR: // Chế độ truyền dữ liệu bằng interrupt (non-blocking)
          return SPI_TransmitReceive_Intr(hspi, pdata_tx, pdata_rx, size);
        
        default: // Trường hợp trans không hợp lệ
          return STAT_ERROR; // Trả về lỗi do trans không hợp lệ
          break;
      }

    // Trường hợp mặc định đã return ở trên
    return STAT_ERROR;
  }

  
