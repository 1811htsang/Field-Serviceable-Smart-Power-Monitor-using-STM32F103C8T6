/*
 * lib_itm_impl.c
 *
 *  Created on: Mar 10, 2026
 *      Author: shanghuang
 */

// >> Thư viện triển khai các hàm điều khiển ITM

  #include "stdint.h"
  #include "stdio.h"
  #include "generic/lib_keyword_def.h"
  #include "generic/lib_condition_def.h"
  #include "itm/lib_itm_def.h"
  #include "itm/lib_itm_ex_def.h"

// Định nghĩa các hàm thành phần

  void ITM_Init(void) {

    // Bật nguồn clock quản lý bởi DEMCR để đảm bảo ITM hoạt động bình thường

      SET_BIT(CRDBG_DEMCR, (1u << 24)); // Set bit TRCENA trong thanh ghi DEMCR để bật clock cho ITM

    // Cấu hình TPIU để hỗ trợ truyền dữ liệu qua SWO (Serial Wire Output) 

      TPIU_SPPR = 0x00000002; // Cấu hình SPPR để chọn protocol SWO NRZ
      TPIU_ACPR = 35u; // Cấu hình ACPR để thiết lập baud rate cho SWO (SYSCLK là 72MHz và baud rate là 2MHz)

    // Kiểm tra BUSY flag trong thanh ghi SR để đảm bảo ITM không bận trước khi khởi động

      /**
       * Ghi chú:
       * - Kiểm tra xem kết quả đảo trả về của READ_BIT(ITM_REGS_PTR->ITM_TCR, (1u << 23)) có trùng với (1u << 23) không
       * - Nếu trùng, nghĩa là BUSY flag đang được set, tức là ITM đang bận và không thể khởi động được, 
       * nên sẽ tiếp tục vòng lặp để chờ đến khi BUSY flag được clear.
       */

      while (!__DIFF_CHECK(READ_BIT(ITM_REGS_PTR->ITM_TCR, (1u << 23)), (1u << 23))) { 
        // Không làm gì ngoài việc chờ đợi BUSY flag được clear
      }

    // Thiết lập các thanh ghi cần thiết để khởi động ITM port 1

      SET_BIT(ITM_REGS_PTR->ITM_TCR, (1u << 0)); // Enable ITM
      SET_BIT(ITM_REGS_PTR->ITM_TER, (1u << 1)); // Enable stimulus port 1

      /**
       * Ghi chú:
       * - Trong cấu hình gốc, khi khởi động hệ thống
       * thì sẽ được set sẵn quyền privileged 
       * nên không cần phải set quyền truy cập vào các thanh ghi của ITM 
       * thông qua thanh ghi LAR.
       * - Dựa theo cấu hình của HAL Header Cortex-M3 
       * thì không có các cấu hình khởi động mà phụ thuộc vào kiểm tra
       * thanh ghi TCR và TER. Do đó, hàm khởi động này tham chiếu
       * theo cấu hình của HAL Header Cortex-M3 
       * để đảm bảo tính tương thích và hiệu quả trong quá trình khởi động ITM.
       * - Trên cấu hình Debug của STM32CubeIDE thì Port 0 được sử dụng cho SWO, 
       * do đó, chỉ cần khởi động Port 1 để hỗ trợ printf qua ITM.
       */
  }

  void ITM_SendChar(ui32 ch) {

    // Kiểm tra xem ITM và stimulus port 1 đã sẵn sàng để gửi dữ liệu chưa

      if (
        READ_BIT(ITM_REGS_PTR->ITM_TCR, (1u << 0)) != 0 && // Kiểm tra ITM enable
        READ_BIT(ITM_REGS_PTR->ITM_TER, (1u << 1)) != 0 // Kiểm tra stimulus port 1 enable
      ) {
        
        while (ITM_REGS_PTR->ITM_STIM[1] == 0UL) { // Kiểm tra FIFO của stimulus port 1
          // Bổ sung 1 timeout để tránh trường hợp vòng lặp vô hạn nếu FIFO luôn đầy
          // Có thể sử dụng một biến đếm hoặc timer để thực hiện timeout
          // Ví dụ sử dụng biến đếm đơn giản:
          static ui32 timeout_counter = 0;
          timeout_counter++;
          if (timeout_counter > 10000) { // Giả sử timeout sau 10 ngàn lần kiểm tra
            return; // Thoát khỏi hàm nếu timeout xảy ra
          }
        }
        ITM_REGS_PTR->ITM_STIM[1] = ch; // Gửi ký tự qua stimulus port 1

      }
      return;
  }

  void ITM_SendString(const char *str) {

    while (*str) {
      ITM_SendChar((ui32)(*str)); // Gửi từng ký tự trong chuỗi qua ITM
      str++;
    }

  }

