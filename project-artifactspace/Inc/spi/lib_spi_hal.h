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

    #ifndef SPI_RETR
      #define SPI_RETR
        tdf_enum SPI_RETR_Enum {
          SPI_OK = 0x00u,
          SPI_READY = 0x01u,
          SPI_ERROR = 0x01u,
          SPI_BUSY = 0x02u,
          SPI_BUSY_TX = 0x03u,
          SPI_BUSY_RX = 0x04u,
          SPI_BUSY_TX_RX = 0x05u,
          SPI_ERROR = 0x06u,
          SPI_ABORT = 0x07u
        } SPI_RETR_Enum;
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

          __vo SPI_RETR_Enum State; // Trạng thái hiện tại của ngoại vi SPI
          __vo ui32 ErrorCode; // Mã lỗi nếu có lỗi xảy ra
        } SPI_Handle_Param;
    #endif

#endif /* LIB_SPI_HAL_H_ */
