/*
 * lib_spi_hal.h
 *
 *  Created on: Mar 15, 2026
 *      Author: shanghuang
 */

#ifndef LIB_SPI_HAL_H_
  #define LIB_SPI_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "spi/lib_spi_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_spi_def.h"
    #endif
  
  // Khai báo quản lý sử dụng CRC

    #ifndef SPI_CRC_ENABLE
      #define SPI_CRC_ENABLE 0U // Mặc định không kích hoạt tính năng CRC
    #endif

  // Khai báo cấu trúc tham số hàm khởi tạo

    #ifndef SPI_INIT_PARAM_TYPE
      #define SPI_INIT_PARAM_TYPE
        tdf_strc SPI_Init_Param { // Cấu trúc tham số để khởi tạo ngoại vi SPI
          ui32 Mode;              // Chọn chế độ hoạt động cho SPI (Master/Slave)
          ui32 Direction;         // Chọn chế độ truyền nhận dữ liệu (Full-duplex/Half-duplex/Receive-only)
          ui32 DataSize;          // Chọn kích thước dữ liệu (8-bit/16-bit)
          ui32 CLKPolarity;       // Chọn trạng thái của clock khi không truyền dữ liệu (Low/High)
          ui32 CLKPhase;          // Chọn cạnh hoạt động của clock để lấy mẫu dữ liệu (1st edge/2nd edge)
          ui32 NSS;               // Chọn chế độ quản lý tín hiệu Slave Select (Software/Hardware)
          ui32 BaudRatePrescaler; // Chọn hệ số chia tốc độ clock để tạo baud rate
          ui32 FirstBit;          // Chọn thứ tự bit khi truyền dữ liệu (MSB/LSB)
          ui32 CRCCalculation;    // Chọn có kích hoạt tính toán CRC hay không (Enable/Disable)
          ui32 CRCPolynomial;     // Chọn đa thức CRC nếu tính toán CRC được kích hoạt
        } SPI_Init_Param;
    #endif

    /**
     * Ghi chú:
     * - Mode ảnh hưởng bit 2
     * - Direction ảnh hưởng bit 15, 14, 10
     * - DataSize ảnh hưởng bit 11
     * - CLKPolarity ảnh hưởng bit 1
     * - CLKPhase ảnh hưởng bit 0
     * - NSS ảnh hưởng bit 9, 8, 2 (SSOE - CR2)
     * - BaudRatePrescaler ảnh hưởng bit 5:3
     * - FirstBit ảnh hưởng bit 7
     * - CRCCalculation ảnh hưởng bit 13
     */

  // Khai báo trạng thái trả về

    #ifndef SPI_STAT_ENUM_TYPE
      #define SPI_STAT_ENUM_TYPE
        tdf_enum SPI_STAT_Enum {
          SPI_OK          = 0x00u,
          SPI_READY       = 0x01u,
          SPI_BUSY        = 0x02u,
          SPI_BUSY_TX     = 0x03u,
          SPI_BUSY_RX     = 0x04u,
          SPI_BUSY_TX_RX  = 0x05u,
          SPI_ERROR       = 0x06u,
          SPI_ABORT       = 0x07u
        } SPI_STAT_Enum;
    #endif

  // Khai báo bộ tập hợp mã lỗi

    #ifndef SPI_ERR_ENUM_TYPE
      #define SPI_ERR_ENUM_TYPE
        tdf_enum SPI_ERR_Enum {
          SPI_OK = (ui32)0x0000u,                           // Không có lỗi
          SPI_ERROR_MODF = (ui32)(1u << SPI_SR_MODF_POS),   // Lỗi MODF (Mode Fault)
          SPI_ERROR_CRC = (ui32)(1u << SPI_SR_CRCERR_POS),  // Lỗi CRC
          SPI_ERROR_OVR = (ui32)(1u << SPI_SR_OVR_POS),     // Lỗi Overrun
          SPI_ERROR_UDR = (ui32)(1u << SPI_SR_UDR_POS),     // Lỗi Underrun
          SPI_ERROR_BSY = (ui32)(1u << SPI_SR_BSY_POS)      // Lỗi Busy (thường xảy ra khi có lỗi khác và ngoại vi đang bận xử lý)
        } SPI_ERR_Enum;
    #endif

  // Khai báo cấu trúc quản lý đa sự kiện và callback (nếu được kích hoạt)

    #if (SPI_PUBLIC_CALLBACK_ENABLE == 1U)

      // Bộ quản lý ID callback cho các sự kiện khác nhau của SPI
      tdf_enum SPI_Callback_Event_Param_Type {
        SPI_TX_CPLT_CB_ID = 0x00u,          // ID callback khi hoàn thành truyền dữ liệu
        SPI_RX_CPLT_CB_ID = 0x01u,          // ID callback khi hoàn thành nhận dữ liệu
        SPI_TX_RX_CPLT_CB_ID = 0x02u,       // ID callback khi hoàn thành truyền và nhận dữ liệu
        SPI_TX_HALF_CPLT_CB_ID = 0x03u,     // ID callback khi hoàn thành nửa quá trình truyền dữ liệu
        SPI_RX_HALF_CPLT_CB_ID = 0x04u,     // ID callback khi hoàn thành nửa quá trình nhận dữ liệu
        SPI_TX_RX_HALF_CPLT_CB_ID = 0x05u,  // ID callback khi hoàn thành nửa quá trình truyền và nhận dữ liệu
        SPI_ERROR_CB_ID = 0x06u,            // ID callback khi có lỗi xảy ra
        SPI_ABORT_CB_ID = 0x07u,            // ID callback khi quá trình truyền hoặc nhận bị hủy bỏ
        SPI_MSP_INIT_CB_ID = 0x08u,         // ID callback khởi tạo MSP
        SPI_MSP_DEINIT_CB_ID = 0x09u        // ID callback giải phóng MSP
      } SPI_CallbackIDTypeDef;

      // Định nghĩa khai báo thống nhất cho các hàm callback của SPI
      typedef void (*pSPI_CallbackTypeDef)(SPI_Handle_Param *hspi); // Định nghĩa kiểu con trỏ hàm callback cho SPI
    #endif
  
    /**
     * Ghi chú:
     * Trong khai báo thiết kế này, chúng ta đã sử dụng các thành phần sau:
     * - Init_Param: Chứa tham số cấu hình cho khởi tạo SPI
     * - Handle_Param: Chứa tham số quản lý trạng thái, quá trình truyền nhận và callback của SPI
     * - CallbackIDTypeDef: Định nghĩa ID cho các sự kiện callback khác nhau của SPI
     * - pSPI_CallbackTypeDef: Định nghĩa kiểu con trỏ hàm callback cho SPI
     * 
     * Trong Handle_Param chúng ta đã có RxISR và TxISR để quản lý nội bộ xử lý ngắt truyền và nhận dữ liệu,
     * Việc bổ sung thêm các callback khác như Tx_Cplt_Callback, Rx_Cplt_Callback,... 
     * sẽ giúp người dùng có thể dễ dàng đăng ký các hàm callback 
     * cho các sự kiện khác nhau của SPI một cách linh hoạt và tiện lợi hơn. Ngoài ra,
     * các bộ quản lý callback bên trong Handle_Param đóng vai trò là bảng quản lý callback thay thế tương tự như bên EXTI,
     * giúp cho việc gọi các hàm callback trở nên dễ dàng và rõ ràng hơn trong quá trình phát triển và sử dụng thư viện SPI này.
     * Khai báo pSPI_CallbackTypeDef sẽ giúp chúng ta có một kiểu dữ liệu thống nhất để quản lý các hàm callback của SPI,
     * giúp cho việc đăng ký và gọi các hàm callback trở nên dễ dàng và rõ ràng hơn trong quá trình phát triển 
     * và sử dụng thư viện SPI này.
     */

  // Khai báo khối quản lý tham số

    #ifndef SPI_HANDLE_PARAM_TYPE
      #define SPI_HANDLE_PARAM_TYPE
        tdf_strc SPI_Handle_Param {   // Cấu trúc tham số để khởi tạo ngoại vi SPI
          SPI_REGS_Typedef *Instance; // Con trỏ tới bộ thanh ghi của ngoại vi SPI
          SPI_Init_Param Init;        // Cấu trúc tham số khởi tạo ngoại vi SPI

          // Quản lý phía truyền dữ liệu
          const ui8* Tx_Buff_Ptr;     // Con trỏ tới buffer chứa dữ liệu cần truyền
          ui16 Tx_Xfer_Size;          // Kích thước dữ liệu cần truyền (tính theo số lượng phần tử, không phải số byte)
          __vo ui16 Tx_Xfer_Count;    // Biến đếm số lượng phần tử đã truyền được

          // Quản lý phía nhận dữ liệu
          ui8* Rx_Buff_Ptr;           // Con trỏ tới buffer chứa dữ liệu nhận được
          ui16 Rx_Xfer_Size;          // Kích thước dữ liệu cần nhận (tính theo số lượng phần tử, không phải số byte)
          __vo ui16 Rx_Xfer_Count;    // Biến đếm số lượng phần tử đã nhận được

          /**
           * Ghi chú:
           * - Tx_Buff_Ptr và Rx_Buff_Ptr sử dụng ui8* để đảm bảo truyền dữ liệu ở mức base 1-byte,
           * vì kích thước dữ liệu có thể là 8-bit hoặc 16-bit, nên việc sử dụng ui8* sẽ giúp dễ dàng xử lý dữ liệu theo từng byte.
           * - Không sử dụng void* để tránh mất tính rõ ràng về kiểu dữ liệu và để đảm bảo an toàn kiểu (type safety) trong quá trình truy cập dữ liệu.
           * - ở Tx sẽ bổ sung từ khóa const để đảm bảo rằng dữ liệu truyền đi sẽ không bị thay đổi trong quá trình truyền.
           */

          // Quản lý ngắt và callback
          void (*RxISR)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm xử lý ngắt nhận dữ liệu
          void (*TxISR)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm xử lý ngắt truyền dữ liệu

          __vo SPI_STAT_Enum State; // Trạng thái hiện tại của ngoại vi SPI
          __vo ui32 ErrorCode;      // Mã lỗi nếu có lỗi xảy ra

          // Quản lý callback cho các sự kiện khác nhau của SPI (nếu được kích hoạt)
          #if (SPI_PUBLIC_CALLBACK_ENABLE == 1U)
            pSPI_CallbackTypeDef MSP_Init_Callback;     // Con trỏ tới hàm callback khởi tạo MSP
            pSPI_CallbackTypeDef MSP_DeInit_Callback;   // Con trỏ tới hàm callback giải phóng MSP
            pSPI_CallbackTypeDef Tx_Cplt_Callback;      // Con trỏ tới hàm callback khi hoàn thành truyền dữ liệu
            pSPI_CallbackTypeDef Rx_Cplt_Callback;      // Con trỏ tới hàm callback khi hoàn thành nhận dữ liệu
            pSPI_CallbackTypeDef TxRx_Cplt_Callback;    // Con trỏ tới hàm callback khi hoàn thành truyền và nhận dữ liệu
            pSPI_CallbackTypeDef Tx_HalfCplt_Callback;  // Con trỏ tới hàm callback khi hoàn thành nửa quá trình truyền dữ liệu
            pSPI_CallbackTypeDef Rx_HalfCplt_Callback;    // Con trỏ tới hàm callback khi hoàn thành nửa quá trình nhận dữ liệu
            pSPI_CallbackTypeDef TxRx_HalfCplt_Callback;  // Con trỏ tới hàm callback khi hoàn thành nửa quá trình truyền và nhận dữ liệu
            pSPI_CallbackTypeDef Error_Callback;          // Con trỏ tới hàm callback khi có lỗi xảy ra
            pSPI_CallbackTypeDef Abort_Callback;          // Con trỏ tới hàm callback khi quá trình truyền hoặc nhận bị hủy bỏ
          #endif 
        } SPI_Handle_Param;
    #endif

  // Khai báo các kiểm tra nội bộ

    #define IS_SPI_MODE(MODE) (((MODE) == SPI_MODE_MASTER) || \
                              ((MODE) == SPI_MODE_SLAVE))
    #define IS_SPI_DIRECTION(DIRECTION) (((DIRECTION) == SPI_DIRECTION_2LINES) || \
                                      ((DIRECTION) == SPI_DIRECTION_2LINES_RXONLY) || \
                                      ((DIRECTION) == SPI_DIRECTION_1LINE_RX) || \
                                      ((DIRECTION) == SPI_DIRECTION_1LINE_TX))

    #define IS_SPI_DATASIZE(SIZE) (((SIZE) == SPI_DATASIZE_8BIT) || \
                                  ((SIZE) == SPI_DATASIZE_16BIT))

    #define IS_SPI_CPOL(CPOL) (((CPOL) == SPI_CLKPOLARITY_LOW) || \
                                  ((CPOL) == SPI_CLKPOLARITY_HIGH))

    #define IS_SPI_CPHA(CPHA) (((CPHA) == SPI_CLKPHASE_1EDGE) || \
                                  ((CPHA) == SPI_CLKPHASE_2EDGE))

    #define IS_SPI_NSS(NSS) (((NSS) == SPI_NSS_SOFT) || \
                              ((NSS) == SPI_NSS_HARD_INPUT) || \
                              ((NSS) == SPI_NSS_HARD_OUTPUT))
    
    #define IS_SPI_BAUDRATEPRESCALER(PRESCALER) (((PRESCALER) == SPI_BAUDRATEPRESCALER_2) || \
                                                ((PRESCALER) == SPI_BAUDRATEPRESCALER_4) || \
                                                ((PRESCALER) == SPI_BAUDRATEPRESCALER_8) || \
                                                ((PRESCALER) == SPI_BAUDRATEPRESCALER_16) || \
                                                ((PRESCALER) == SPI_BAUDRATEPRESCALER_32) || \
                                                ((PRESCALER) == SPI_BAUDRATEPRESCALER_64) || \
                                                ((PRESCALER) == SPI_BAUDRATEPRESCALER_128) || \
                                                ((PRESCALER) == SPI_BAUDRATEPRESCALER_256))

    #define IS_SPI_FIRSTBIT(FIRSTBIT) (((FIRSTBIT) == SPI_FIRSTBIT_MSB) || \
                                      ((FIRSTBIT) == SPI_FIRSTBIT_LSB))

    #define IS_SPI_CRCENABLE(CRCCALCULATION) (((CRCCALCULATION) == SPI_CRCCALCULATION_DISABLE) || \
                                                ((CRCCALCULATION) == SPI_CRCCALCULATION_ENABLE))

    #define IS_SPI_CRCPOLYNOMIAL(CRCPOLYNOMIAL) (((CRCPOLYNOMIAL) >= 0x1U) && \
                                                ((CRCPOLYNOMIAL) <= 0xFFFFU) && \
                                                (((CRCPOLYNOMIAL) & 0x1U) != 0U))

  // Khai báo các hàm thành phần

    // >> Các hàm khởi tạo và giải phóng ngoại vi
    RETR_STAT SPI_Init(SPI_Handle_Param *hspi);
    RETR_STAT SPI_DeInit(SPI_Handle_Param *hspi);

    // >> Hàm khởi tạo và giải phóng MSP (MCU Specific Package)
    /**
     * Ghi chú:
     * Các hàm này sẽ được cấu hình tùy thuộc vào thiết kế phần cứng,
     * Hàm SPI_Init/SPI_DeInit bên trên sẽ đảm bảo cấu hình đúng logic.
     */
    void SPI_MSP_Init(SPI_Handle_Param *hspi);
    void SPI_MSP_DeInit(SPI_Handle_Param *hspi);

    // >> Hàm quản lý đăng ký callback (nếu được kích hoạt)
    #if (SPI_PUBLIC_CALLBACK_ENABLE == 1U)
      RETR_STAT SPI_RegisterCallback(
        SPI_Handle_Param *hspi, 
        SPI_CallbackIDTypeDef CallbackID, 
        pSPI_CallbackTypeDef pCallback
      );
      RETR_STAT SPI_UnRegisterCallback(
        SPI_Handle_Param *hspi, 
        SPI_CallbackIDTypeDef CallbackID
      );
    #endif

    // >> Hàm xử lý ngắt SPI (ISR)
    void SPI_IRQHandler(SPI_Handle_Param *hspi);

    // >> Các hàm truyền nhận dữ liệu cơ bản (blocking mode)
    RETR_STAT SPI_Transmit(
      SPI_Handle_Param *hspi, 
      const ui8* pdata, 
      ui16 size, 
      ui32 timeout
    );
    RETR_STAT SPI_Receive(
      SPI_Handle_Param *hspi, 
      ui8* pdata, 
      ui16 size, 
      ui32 timeout
    );
    RETR_STAT SPI_TransmitReceive(
      SPI_Handle_Param *hspi, 
      const ui8* pdata_tx, 
      ui8* pdata_rx, 
      ui16 size, 
      ui32 timeout
    );

    // >> Các hàm truyền nhận dữ liệu không đồng bộ (non-blocking mode) sử dụng ngắt
    RETR_STAT SPI_Transmit_IT(
      SPI_Handle_Param *hspi, 
      const ui8* pdata, 
      ui16 size
    );
    RETR_STAT SPI_Receive_IT(
      SPI_Handle_Param *hspi, 
      ui8* pdata, 
      ui16 size
    );
    RETR_STAT SPI_TransmitReceive_IT(
      SPI_Handle_Param *hspi, 
      const ui8* pdata_tx, 
      ui8* pdata_rx, 
      ui16 size
    );

    // >> Các hàm truyền nhận dữ liệu không đồng bộ (non-blocking mode) sử dụng DMA
    RETR_STAT SPI_Transmit_DMA(
      SPI_Handle_Param *hspi, 
      const ui8* pdata, 
      ui16 size
    );
    RETR_STAT SPI_Receive_DMA(
      SPI_Handle_Param *hspi, 
      ui8* pdata, 
      ui16 size
    );
    RETR_STAT SPI_TransmitReceive_DMA(
      SPI_Handle_Param *hspi, 
      const ui8* pdata_tx, 
      ui8* pdata_rx, 
      ui16 size
    );

    // >> Các hàm kiểm soát quá trình truyền nhận dữ liệu DMA
    RETR_STAT SPI_DMA_Pause(SPI_Handle_Param *hspi);
    RETR_STAT SPI_DMA_Resume(SPI_Handle_Param *hspi);
    RETR_STAT SPI_DMA_Stop(SPI_Handle_Param *hspi);

    // >> Các hàm quản lý abort quá trình truyền nhận dữ liệu
    RETR_STAT SPI_Abort(SPI_Handle_Param *hspi);
    RETR_STAT SPI_Abort_IT(SPI_Handle_Param *hspi);

#endif /* LIB_SPI_HAL_H_ */
