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

  // Khai báo cấu trúc tham số hàm khởi tạo

    #ifndef SPI_INIT_PARAM_TYPE
      #define SPI_INIT_PARAM_TYPE
        tdf_strc SPI_Init_Param { // Cấu trúc tham số để khởi tạo ngoại vi SPI
          ui32 Mode;         // Chọn chế độ hoạt động cho SPI (Master/Slave)
          ui32 Direction;    // Chọn chế độ truyền nhận dữ liệu (Full-duplex/Half-duplex/Receive-only)
          ui32 DataSize;     // Chọn kích thước dữ liệu (8-bit/16-bit)
          ui32 CLKPolarity;  // Chọn trạng thái của clock khi không truyền dữ liệu (Low/High)
          ui32 CLKPhase;     // Chọn cạnh hoạt động của clock để lấy mẫu dữ liệu (1st edge/2nd edge)
          ui32 NSS;         // Chọn chế độ quản lý tín hiệu Slave Select (Software/Hardware)
          ui32 BaudRatePrescaler; // Chọn hệ số chia tốc độ clock để tạo baud rate
          ui32 FirstBit;     // Chọn thứ tự bit khi truyền dữ liệu (MSB/LSB)
          ui32 CRCCalculation; // Chọn có kích hoạt tính toán CRC hay không (Enable/Disable)
          ui32 CRCPolynomial; // Chọn đa thức CRC nếu tính toán CRC được kích hoạt
        } SPI_Init_Param;
    #endif

  // Khai báo trạng thái trả về

    #ifndef SPI_RETR_ENUM_TYPE
      #define SPI_RETR_ENUM_TYPE
        tdf_enum SPI_RETR_Enum {
          SPI_OK = 0x00u,
          SPI_READY = 0x01u,
          SPI_BUSY = 0x02u,
          SPI_BUSY_TX = 0x03u,
          SPI_BUSY_RX = 0x04u,
          SPI_BUSY_TX_RX = 0x05u,
          SPI_ERROR = 0x06u,
          SPI_ABORT = 0x07u
        } SPI_RETR_Enum;
    #endif

  // Khai báo bộ tập hợp mã lỗi

    #ifndef SPI_ERR_ENUM_TYPE
      #define SPI_ERR_ENUM_TYPE
        tdf_enum SPI_ERR_Enum {
          SPI_OK = (ui32)0x0000u, // Không có lỗi
          SPI_ERROR_MODF = (ui32)(1u << SPI_SR_MODF_POS), // Lỗi MODF (Mode Fault)
          SPI_ERROR_CRC = (ui32)(1u << SPI_SR_CRCERR_POS), // Lỗi CRC
          SPI_ERROR_OVR = (ui32)(1u << SPI_SR_OVR_POS), // Lỗi Overrun
          SPI_ERROR_UDR = (ui32)(1u << SPI_SR_UDR_POS), // Lỗi Underrun
          SPI_ERROR_BSY = (ui32)(1u << SPI_SR_BSY_POS) // Lỗi Busy (thường xảy ra khi có lỗi khác và ngoại vi đang bận xử lý)
        } SPI_ERR_Enum;
    #endif

  // Khai báo khối quản lý tham số

    #ifndef SPI_HANDLE_PARAM_TYPE
      #define SPI_HANDLE_PARAM_TYPE
        tdf_strc SPI_Handle_Param { // Cấu trúc tham số để khởi tạo ngoại vi SPI
          SPI_REGS_Typedef *Instance; // Con trỏ tới bộ thanh ghi của ngoại vi SPI
          SPI_Init_Param Init; // Cấu trúc tham số khởi tạo ngoại vi SPI

          const ui8* Tx_Buff_Ptr; // Con trỏ tới buffer chứa dữ liệu cần truyền
          ui16 Tx_Xfer_Size; // Kích thước dữ liệu cần truyền (tính theo số lượng phần tử, không phải số byte)
          __vo ui16 Tx_Xfer_Count; // Biến đếm số lượng phần tử đã truyền được

          ui8* Rx_Buff_Ptr; // Con trỏ tới buffer chứa dữ liệu nhận được
          ui16 Rx_Xfer_Size; // Kích thước dữ liệu cần nhận (tính theo số lượng phần tử, không phải số byte)
          __vo ui16 Rx_Xfer_Count; // Biến đếm số lượng phần tử đã nhận được

          /**
           * Ghi chú:
           * - Tx_Buff_Ptr và Rx_Buff_Ptr sử dụng ui8* để đảm bảo truyền dữ liệu ở mức base 1-byte,
           * vì kích thước dữ liệu có thể là 8-bit hoặc 16-bit, nên việc sử dụng ui8* sẽ giúp dễ dàng xử lý dữ liệu theo từng byte.
           * - Không sử dụng void* để tránh mất tính rõ ràng về kiểu dữ liệu và để đảm bảo an toàn kiểu (type safety) trong quá trình truy cập dữ liệu.
           * - ở Tx sẽ bổ sung từ khóa const để đảm bảo rằng dữ liệu truyền đi sẽ không bị thay đổi trong quá trình truyền.
           */

          void (*RxISR)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm xử lý ngắt nhận dữ liệu
          void (*TxISR)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm xử lý ngắt truyền dữ liệu

          __vo SPI_RETR_Enum State; // Trạng thái hiện tại của ngoại vi SPI
          __vo ui32 ErrorCode; // Mã lỗi nếu có lỗi xảy ra

          #if (SPI_REGISTER_CALLBACK == 1U)
            void (*MSP_Init_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback khởi tạo MSP
            void (*MSP_DeInit_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback giải phóng MSP
            void (*Tx_Cplt_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback khi hoàn thành truyền dữ liệu
            void (*Rx_Cplt_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback khi hoàn thành nhận dữ liệu
            void (*TxRx_Cplt_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback khi hoàn thành truyền và nhận dữ liệu
            void (*Tx_HalfCplt_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback khi hoàn thành nửa quá trình truyền dữ liệu
            void (*Rx_HalfCplt_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback khi hoàn thành nửa quá trình nhận dữ liệu
            void (*TxRx_HalfCplt_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback khi hoàn thành nửa quá trình truyền và nhận dữ liệu
            void (*Error_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback khi có lỗi xảy ra
            void (*Abort_Callback)(struct SPI_Handle_Param *hspi); // Con trỏ tới hàm callback khi quá trình truyền hoặc nhận bị hủy bỏ
          #endif /* SPI_REGISTER_CALLBACK */
        } SPI_Handle_Param;
    #endif

  // Khai báo cấu trúc quản lý đa sự kiện

    #if (SPI_REGISTER_CALLBACK == 1U)
      tdf_enum SPI_Callback_Event_Param_Type {
        SPI_TX_CPLT_CB_ID = 0x00u, // ID callback khi hoàn thành truyền dữ liệu
        SPI_RX_CPLT_CB_ID = 0x01u, // ID callback khi hoàn thành nhận dữ liệu
        SPI_TX_RX_CPLT_CB_ID = 0x02u, // ID callback khi hoàn thành truyền và nhận dữ liệu
        SPI_TX_HALF_CPLT_CB_ID = 0x03u, // ID callback khi hoàn thành nửa quá trình truyền dữ liệu
        SPI_RX_HALF_CPLT_CB_ID = 0x04u, // ID callback khi hoàn thành nửa quá trình nhận dữ liệu
        SPI_TX_RX_HALF_CPLT_CB_ID = 0x05u, // ID callback khi hoàn thành nửa quá trình truyền và nhận dữ liệu
        SPI_ERROR_CB_ID = 0x06u, // ID callback khi có lỗi xảy ra
        SPI_ABORT_CB_ID = 0x07u, // ID callback khi quá trình truyền hoặc nhận bị hủy bỏ
        SPI_MSP_INIT_CB_ID = 0x08u, // ID callback khởi tạo MSP
        SPI_MSP_DEINIT_CB_ID = 0x09u // ID callback giải phóng MSP
      } SPI_CallbackIDTypeDef;

      typedef void (*pSPI_CallbackTypeDef)(SPI_Handle_Param *hspi); // Định nghĩa kiểu con trỏ hàm callback cho SPI
    #endif

  // Khai báo các kiểm tra nội bộ

    #define IS_SPI_MODE(MODE) (((MODE) == SPI_MODE_MASTER) || \
                              ((MODE) == SPI_MODE_SLAVE))
    #define IS_SPI_DIRECTION(DIRECTION) (((DIRECTION) == SPI_DIRECTION_2LINES) || \
                                      ((DIRECTION) == SPI_DIRECTION_2LINES_RXONLY) || \
                                      ((DIRECTION) == SPI_DIRECTION_1LINE))

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

    RETR_STAT SPI_Init(SPI_Handle_Param *spi_handle_param);
    RETR_STAT SPI_DeInit(SPI_Handle_Param *spi_handle_param);

    /**
     * Ghi chú:
     * Các hàm này sẽ được cấu hình tùy thuộc vào thiết kế phần cứng,
     * Hàm SPI_Init/SPI_DeInit bên trên sẽ đảm bảo cấu hình đúng logic.
     */
    void SPI_MSP_Init(SPI_Handle_Param *spi_handle_param);
    void SPI_MSP_DeInit(SPI_Handle_Param *spi_handle_param);

    RETR_STAT SPI_Transmit(
      SPI_Handle_Param *spi_handle_param, 
      const ui8* data_ptr, 
      ui16 size, 
      ui32 timeout
    );
    RETR_STAT SPI_Receive(
      SPI_Handle_Param *spi_handle_param, 
      ui8* data_ptr, 
      ui16 size, 
      ui32 timeout
    );
    RETR_STAT SPI_TransmitReceive(
      SPI_Handle_Param *spi_handle_param, 
      const ui8* tx_data_ptr, 
      ui8* rx_data_ptr, 
      ui16 size, 
      ui32 timeout
    );



#endif /* LIB_SPI_HAL_H_ */
