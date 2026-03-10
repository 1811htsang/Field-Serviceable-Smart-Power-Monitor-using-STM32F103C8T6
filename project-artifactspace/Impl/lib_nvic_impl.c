/*
 * lib_nvic_impl.c
 *
 *  Created on: Mar 1, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"
    #include "lib_nvic_def.h"
    #include "lib_nvic_hal.h"
    #include "header_dependency.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
  #include <string.h>

  #ifndef UNIT_TEST
    #include "generic/lib_keyword_def.h"
    #include "generic/lib_condition_def.h"
    #include "nvic/lib_nvic_def.h"
    #include "nvic/lib_nvic_hal.h"
  #endif

// Định nghĩa các hàm thành phần

  /*
   * Hàm cấu hình tổng quát cho một ngắt trong NVIC.
   *
   * Tham số:
   *   intr_param - Con trỏ tới cấu trúc tham số ngắt (Position, Priority, Status).
   *
   * Logic:
   *   - Kiểm tra con trỏ intr_param hợp lệ.
   *   - Kiểm tra các giá trị tham số (Position, Priority, Status).
   *   - Clear pending bit trước khi cấu hình để tránh ngắt không mong muốn.
   *   - Dựa vào Status, gọi hàm enable/disable ngắt:
   *       + INTR_STAT_DISABLE: Vô hiệu hóa ngắt
   *       + INTR_STAT_ENABLE: Kích hoạt ngắt
   *       + INTR_STAT_PENDING: Trả về STAT_BUSY (không hỗ trợ trong config)
   *   - Cấu hình priority cho ngắt.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu thành công, STAT_ERROR nếu lỗi, STAT_BUSY nếu pending.
   *
   * Phụ thuộc ngoài module NVIC:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - NVIC_INTR_Pending_Disable() - Hàm clear pending bit
   *   - NVIC_INTR_Activation_Enable() - Hàm enable ngắt
   *   - NVIC_INTR_Activation_Disable() - Hàm disable ngắt
   *   - NVIC_INTR_Priority_Config() - Hàm cấu hình priority
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT NVIC_INTR_Config(NVIC_INTR_Param *intr_param) {

    /**
     * Ghi chú:
     * Đánh dấu hàm này cần cân nhắc bổ sung khả năng config nhiều IRQ 
     * cùng lúc trong tương lai nếu cần thiết, 
     * hiện tại do thiết kế tài liệu chỉ yêu cầu config từng ngắt một 
     * nên ta sẽ giữ nguyên logic hiện tại.
     */

    // Kiểm tra con trỏ intr_param hợp lệ

      if (intr_param == NULL) {
        return STAT_ERROR;
      }
    
    // Kiểm tra các giá trị tham số đầu vào

      assert_param(IS_NVIC_IRQ_POS(intr_param->Position));
      assert_param(IS_NVIC_INTR_PRIORITY(intr_param->Priority));
      assert_param(IS_NVIC_INTR_STATUS(intr_param->Status));

    // Clear pending bit trước khi cấu hình để tránh ngắt không mong muốn

      NVIC_INTR_Pending_Disable(intr_param); // Trước khi config thì phải đảm bảo pending bit đang ở trạng thái disable để tránh việc ngắt được kích hoạt ngay sau khi config nếu có pending bit cũ chưa được clear

    // Dựa vào Status, gọi hàm enable/disable ngắt

      switch (intr_param->Status) {
        case INTR_STAT_DISABLE:
          NVIC_INTR_Activation_Disable(intr_param);
          break;
        
        case INTR_STAT_ENABLE:
          NVIC_INTR_Activation_Enable(intr_param);
          break;
        
        case INTR_STAT_PENDING: 

          /**
           * Ghi chú:
           * Đối với hàm Config thì phải sử dụng hàm NVIC_INTR_Pending_Enable để set pending bit, 
           * không sử dụng hàm cấu hình tổng quát NVIC_INTR_Config để tránh việc bị gọi đệ quy vô hạn.
           */

          return STAT_BUSY;
          break;
          
        default:
          return STAT_ERROR;
          break;
      }

    // Cấu hình priority cho ngắt

      NVIC_INTR_Priority_Config(intr_param);

    // Trả về trạng thái hoàn thành sau khi đã cấu hình xong ngắt

      return STAT_DONE;
  }
