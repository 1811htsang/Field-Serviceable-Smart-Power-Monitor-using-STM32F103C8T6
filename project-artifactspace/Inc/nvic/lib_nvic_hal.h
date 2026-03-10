/*
 * lib_nvic_hal.h
 *
 *  Created on: Mar 1, 2026
 *      Author: shanghuang
 */

#ifndef LIB_NVIC_HAL_H_
  #define LIB_NVIC_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "nvic/lib_nvic_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_nvic_def.h"
    #endif

    #include <stdint.h>

  // Khai báo bộ quản lý số IRQ

    #ifndef IRQ_POS_ENUM_TYPE
      #define IRQ_POS_ENUM_TYPE
        tdf_enum IRQ_POS_Enum_Type {
          NVIC_IRQ_POS_WWDG      = (ui16)0,
          NVIC_IRQ_POS_RCC       = (ui16)5,
          NVIC_IRQ_POS_EXTI0     = (ui16)6,
          NVIC_IRQ_POS_EXTI1     = (ui16)7,
          NVIC_IRQ_POS_EXTI2     = (ui16)8,
          NVIC_IRQ_POS_EXTI3     = (ui16)9,
          NVIC_IRQ_POS_EXTI4     = (ui16)10,
          NVIC_IRQ_POS_EXTI9_5   = (ui16)23,
          NVIC_IRQ_POS_EXTI15_10 = (ui16)40,
          NVIC_IRQ_POS_I2C1_EV   = (ui16)31,
          NVIC_IRQ_POS_I2C1_ER   = (ui16)32,
          NVIC_IRQ_POS_SPI1      = (ui16)35,
          NVIC_IRQ_POS_USART1    = (ui16)37
        } IRQ_POS_Enum_Type;
    #endif

  // Khai báo kiểu dữ liệu trả về

    #ifndef NVIC_INTR_STATUS_TYPE
      #define NVIC_INTR_STATUS_TYPE
      tdf_enum NVIC_INTR_Status_Type {
        INTR_STAT_DISABLE = (ui16)0xB0,
        INTR_STAT_ENABLE  = (ui16)0xB1,
        INTR_STAT_PENDING = (ui16)0xB2,
        INTR_STAT_UNF     = (ui16)0xFF
      } NVIC_INTR_Status_Type;
    #endif

  // Khai báo cấu trúc quản lý thông số ngắt

    #ifndef NVIC_INTR_PARAM_TYPE
      #define NVIC_INTR_PARAM_TYPE
        tdf_strc NVIC_INTR_Param { // Cấu trúc tham số để cấu hình ngắt cấp hệ thống trong NVIC
          IRQ_POS_Enum_Type Position;      // Chọn IRQ_POS cần xử lý
          ui16 Priority;                   // Chọn mức ưu tiên ngắt (0-15, 0 là ưu tiên cao nhất). Lưu ý rằng trong thiết kế tài liệu, mức ưu tiên chỉ sử dụng loại preempt và bỏ qua sub-preempt 
          NVIC_INTR_Status_Type Status;    // Chọn trạng thái kích hoạt ngắt (0: Disable, 1: Enable, 2: Pending)
        } NVIC_INTR_Param;
    #endif

  // Khai báo các kiểm tra tham số đầu vào nội bộ

    #define IS_NVIC_IRQ_POS(POS) (((POS) == NVIC_IRQ_POS_WWDG) || \
                                  ((POS) == NVIC_IRQ_POS_RCC) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI0) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI1) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI2) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI3) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI4) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI9_5) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI15_10) || \
                                  ((POS) == NVIC_IRQ_POS_I2C1_EV) || \
                                  ((POS) == NVIC_IRQ_POS_I2C1_ER) || \
                                  ((POS) == NVIC_IRQ_POS_SPI1) || \
                                  ((POS) == NVIC_IRQ_POS_USART1))
    
    #define IS_NVIC_INTR_STATUS(STATUS) (((STATUS) == INTR_STAT_DISABLE) || \
                                  ((STATUS) == INTR_STAT_ENABLE) || \
                                  ((STATUS) == INTR_STAT_PENDING))

    #define IS_NVIC_INTR_PRIORITY(PRIORITY) (((PRIORITY) <= 15u))

  // Khai báo các hàm thành phần

    // >> Hàm cấu hình tổng quát trạng thái ngắt cấp hệ thống kích hoạt

      RETR_STAT NVIC_INTR_Config(NVIC_INTR_Param *intr_param);

    // >> Hàm cấu hình trạng thái kích hoạt của ngắt cấp hệ thống

      /*
       * Hàm kích hoạt (enable) một ngắt cụ thể trong NVIC.
       *
       * Tham số:
       *   intr_param - Con trỏ tới cấu trúc tham số ngắt (chứa Position).
       *
       * Logic:
       *   - Xác định thanh ghi ISER tương ứng dựa vào Position:
       *       + Position < 32: Sử dụng ISER[0]
       *       + Position < 64: Sử dụng ISER[1]
       *       + Position >= 64: Sử dụng ISER[2]
       *   - Set bit tương ứng trong ISER để kích hoạt ngắt.
       *   - Hàm này giả định pending bit đã được clear trước đó.
       *
       * Trả về:
       *   Không có (void).
       *
       * Phụ thuộc ngoài module NVIC:
       *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
       */
      stinl void NVIC_INTR_Activation_Enable(NVIC_INTR_Param *intr_param) {

        /**
         * Ghi chú:
         * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
         * nên ở đây ta có thể yên tâm rằng việc set bit trong ISER sẽ không gây ra lỗi.
         */

        if (intr_param->Position < 32) {
          NVIC_REGS_PTR->NVIC_ISER[0] |= (0x0001u << intr_param->Position); // Set bit tương ứng trong ISER để kích hoạt ngắt
        } else if (intr_param->Position < 64) {
          NVIC_REGS_PTR->NVIC_ISER[1] |= (0x0001u << (intr_param->Position - 32)); // Set bit tương ứng trong ISER để kích hoạt ngắt
        } else {
          NVIC_REGS_PTR->NVIC_ISER[2] |= (0x0001u << (intr_param->Position - 64)); // Set bit tương ứng trong ISER để kích hoạt ngắt
        }

        return;
      }

      /*
       * Hàm vô hiệu hóa (disable) một ngắt cụ thể trong NVIC.
       *
       * Tham số:
       *   intr_param - Con trỏ tới cấu trúc tham số ngắt (chứa Position).
       *
       * Logic:
       *   - Xác định thanh ghi ICER tương ứng dựa vào Position:
       *       + Position < 32: Sử dụng ICER[0]
       *       + Position < 64: Sử dụng ICER[1]
       *       + Position >= 64: Sử dụng ICER[2]
       *   - Set bit tương ứng trong ICER để vô hiệu hóa ngắt.
       *   - Hàm này giả định pending bit đã được clear trước đó.
       *
       * Trả về:
       *   Không có (void).
       *
       * Phụ thuộc ngoài module NVIC:
       *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
       */
      stinl void NVIC_INTR_Activation_Disable(NVIC_INTR_Param *intr_param) {

        /**
         * Ghi chú:
         * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
         * nên ở đây ta có thể yên tâm rằng việc set bit trong ICER sẽ không gây ra lỗi.
         */

        if (intr_param->Position < 32) {
          NVIC_REGS_PTR->NVIC_ICER[0] |= (0x0001u << intr_param->Position); // Set bit tương ứng trong ICER để vô hiệu hóa ngắt
        } else if (intr_param->Position < 64) {
          NVIC_REGS_PTR->NVIC_ICER[1] |= (0x0001u << (intr_param->Position - 32)); // Set bit tương ứng trong ICER để vô hiệu hóa ngắt
        } else {
          NVIC_REGS_PTR->NVIC_ICER[2] |= (0x0001u << (intr_param->Position - 64)); // Set bit tương ứng trong ICER để vô hiệu hóa ngắt
        }

        return;
      }

    // >> Hàm cấu hình trạng thái pending của ngắt cấp hệ thống

      /*
       * Hàm set pending bit cho một ngắt cụ thể trong NVIC.
       *
       * Tham số:
       *   intr_param - Con trỏ tới cấu trúc tham số ngắt (chứa Position).
       *
       * Logic:
       *   - Xác định thanh ghi ISPR tương ứng dựa vào Position:
       *       + Position < 32: Sử dụng ISPR[0]
       *       + Position < 64: Sử dụng ISPR[1]
       *       + Position >= 64: Sử dụng ISPR[2]
       *   - Set bit tương ứng trong ISPR để kích hoạt pending bit.
       *   - Pending bit cho phép ngắt được đặt vào hàng đợi chờ xử lý.
       *
       * Trả về:
       *   Không có (void).
       *
       * Phụ thuộc ngoài module NVIC:
       *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
       */
      stinl void NVIC_INTR_Pending_Enable(NVIC_INTR_Param *intr_param) {

        /**
         * Ghi chú:
         * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
         * nên ở đây ta có thể yên tâm rằng việc set bit trong ISPR sẽ không gây ra lỗi.
         */

        if (intr_param->Position < 32) {
          NVIC_REGS_PTR->NVIC_ISPR[0] |= (0x0001u << intr_param->Position); // Set bit tương ứng trong ISPR để kích hoạt pending bit
        } else if (intr_param->Position < 64) {
          NVIC_REGS_PTR->NVIC_ISPR[1] |= (0x0001u << (intr_param->Position - 32)); // Set bit tương ứng trong ISPR để kích hoạt pending bit
        } else {
          NVIC_REGS_PTR->NVIC_ISPR[2] |= (0x0001u << (intr_param->Position - 64)); // Set bit tương ứng trong ISPR để kích hoạt pending bit
        }

        return;
      }

      /*
       * Hàm clear pending bit cho một ngắt cụ thể trong NVIC.
       *
       * Tham số:
       *   intr_param - Con trỏ tới cấu trúc tham số ngắt (chứa Position).
       *
       * Logic:
       *   - Xác định thanh ghi ICPR tương ứng dựa vào Position:
       *       + Position < 32: Sử dụng ICPR[0]
       *       + Position < 64: Sử dụng ICPR[1]
       *       + Position >= 64: Sử dụng ICPR[2]
       *   - Set bit tương ứng trong ICPR để clear pending bit.
       *   - Clear pending bit là bước quan trọng trước khi cấu hình ngắt.
       *
       * Trả về:
       *   Không có (void).
       *
       * Phụ thuộc ngoài module NVIC:
       *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
       */
      stinl void NVIC_INTR_Pending_Disable(NVIC_INTR_Param *intr_param) {

        /**
         * Ghi chú:
         * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
         * nên ở đây ta có thể yên tâm rằng việc set bit trong ICPR sẽ không gây ra lỗi.
         */

        if (intr_param->Position < 32) {
          NVIC_REGS_PTR->NVIC_ICPR[0] |= (0x0001u << intr_param->Position); // Set bit tương ứng trong ICPR để clear pending bit
        } else if (intr_param->Position < 64) {
          NVIC_REGS_PTR->NVIC_ICPR[1] |= (0x0001u << (intr_param->Position - 32)); // Set bit tương ứng trong ICPR để clear pending bit
        } else {
          NVIC_REGS_PTR->NVIC_ICPR[2] |= (0x0001u << (intr_param->Position - 64)); // Set bit tương ứng trong ICPR để clear pending bit
        }

        return;
      }

    // >> Hàm cấu hình mức ưu tiên của ngắt cấp hệ thống

      /*
       * Hàm cấu hình mức ưu tiên (priority) cho một ngắt cụ thể trong NVIC.
       *
       * Tham số:
       *   intr_param - Con trỏ tới cấu trúc tham số ngắt (chứa Position và Priority).
       *
       * Logic:
       *   - Ghi giá trị priority vào 4 bit cao của byte tương ứng trong IPR.
       *   - Sử dụng mask 0x0F để đảm bảo chỉ sử dụng 4 bit cao của Priority.
       *   - Dịch trái 4 bit để ghi vào nửa trên của byte (chỉ dùng preempt priority).
       *   - Giá trị priority hợp lệ: 0-15 (thấp hơn = ưu tiên cao hơn).
       *
       * Trả về:
       *   Không có (void).
       *
       * Phụ thuộc ngoài module NVIC:
       *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
       */
      stinl void NVIC_INTR_Priority_Config(NVIC_INTR_Param *intr_param) {

        /**
         * Ghi chú:
         * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
         * nên ở đây ta có thể yên tâm rằng việc set bit trong IPR sẽ không gây ra lỗi.
         */

        NVIC_REGS_PTR->NVIC_IPRE[intr_param->Position] = (ui8)((intr_param->Priority & 0x0Fu) << 4);

        /**
         * Ghi chú:
         * Set 4 bit cao của byte tương ứng trong IPR để cấu hình mức ưu tiên ngắt,
         * lưu ý rằng trong thiết kế tài liệu mức ưu tiên chỉ sử dụng loại preempt
         * và bỏ qua sub-preempt nên ta sẽ không sử dụng 4 bit thấp của byte trong IPR.
         * Thực hiện and với mask 0x0F để đảm bảo rằng chỉ có 4 bit cao của intr_param->Priority được sử dụng, tránh việc giá trị priority vượt quá 15u
         */

        return;
      }

    // >> Hàm kiểm tra trạng thái ngắt cấp hệ thống

      /*
       * Hàm kiểm tra trạng thái kích hoạt (activation) của một ngắt.
       *
       * Tham số:
       *   position - Vị trí ngắt (IRQ number) cần kiểm tra (0-67).
       *
       * Logic:
       *   - Xác định thanh ghi ISER tương ứng dựa vào position.
       *   - Đọc bit tương ứng trong ISER để kiểm tra trạng thái.
       *   - Trả về 1 nếu ngắt đang được kích hoạt, 0 nếu không.
       *
       * Trả về:
       *   ui32 - 1 nếu ngắt được enable, 0 nếu disable.
       *
       * Phụ thuộc ngoài module NVIC: Không có
       */
      stinl ui32 NVIC_INTR_GetActivation(IRQ_POS_Enum_Type position) {

        ui32 result = 0;

        if (position < 32) {
          result = (NVIC_REGS_PTR->NVIC_ISER[0] & (0x0001u << position)) != 0; // Kiểm tra bit tương ứng trong ISER để xác định trạng thái kích hoạt ngắt
        } else if (position < 64) {
          result = (NVIC_REGS_PTR->NVIC_ISER[1] & (0x0001u << (position - 32))) != 0; // Kiểm tra bit tương ứng trong ISER để xác định trạng thái kích hoạt ngắt
        } else {
          result = (NVIC_REGS_PTR->NVIC_ISER[2] & (0x0001u << (position - 64))) != 0; // Kiểm tra bit tương ứng trong ISER để xác định trạng thái kích hoạt ngắt
        }

        return result;
      }

    // >> Hàm kiểm tra trạng thái pending của ngắt cấp hệ thống

      /*
       * Hàm kiểm tra trạng thái pending của một ngắt.
       *
       * Tham số:
       *   position - Vị trí ngắt (IRQ number) cần kiểm tra (0-67).
       *
       * Logic:
       *   - Xác định thanh ghi ISPR tương ứng dựa vào position.
       *   - Đọc bit tương ứng trong ISPR để kiểm tra trạng thái pending.
       *   - Trả về 1 nếu ngắt đang pending, 0 nếu không.
       *   - Pending bit cho biết ngắt đang chờ được xử lý.
       *
       * Trả về:
       *   ui32 - 1 nếu ngắt đang pending, 0 nếu không.
       *
       * Phụ thuộc ngoài module NVIC: Không có
       */
      stinl ui32 NVIC_INTR_GetPending(IRQ_POS_Enum_Type position) {

        ui32 result = 0;

        if (position < 32) {
          result = (NVIC_REGS_PTR->NVIC_ISPR[0] & (0x0001u << position)) != 0; // Kiểm tra bit tương ứng trong ISPR để xác định trạng thái pending ngắt
        } else if (position < 64) {
          result = (NVIC_REGS_PTR->NVIC_ISPR[1] & (0x0001u << (position - 32))) != 0; // Kiểm tra bit tương ứng trong ISPR để xác định trạng thái pending ngắt
        } else {
          result = (NVIC_REGS_PTR->NVIC_ISPR[2] & (0x0001u << (position - 64))) != 0; // Kiểm tra bit tương ứng trong ISPR để xác định trạng thái pending ngắt
        }

        return result;
      }

#endif /* LIB_NVIC_HAL_H_ */
