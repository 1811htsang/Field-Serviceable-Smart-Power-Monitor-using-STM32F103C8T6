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
    #include "spi/lib_spi_def.h"
    #include "spi/lib_spi_hal.h"
  #endif

// Định nghĩa các hàm thành phần

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
        // Các callback sẽ được khởi tạo mặc định là NULL, người dùng có thể đăng ký sau khi hàm Init thành công
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
      // 
      return STAT_DONE;
    }

    RETR_STAT SPI_UnRegisterCallback(
      SPI_Handle_Param *hspi, 
      SPI_CallbackIDTypeDef CallbackID
    ) {
      return STAT_DONE;
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

  
