/*
 * lib_reset_hal.h
 *
 *  Created on: Jan 29, 2026
 *      Author: shanghuang
 */

#ifndef LIB_RESET_HAL_H_
  #define LIB_RESET_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "generic/lib_condition_def.h"
      #include "reset/lib_reset_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_condition_def.h"
      #include "lib_reset_def.h"
    #endif

  // Khai báo cấu trúc catch reset

    #ifndef RCC_RSTFLG_TYPEDEF
      #define RCC_RSTFLG_TYPEDEF
        tdf_strc RCC_RSTFLG_Typedef {
          ui IsPinReset;    // Cờ đánh dấu reset bởi tín hiệu bên ngoài (PIN NRST)
          ui IsPorReset;    // Cờ đánh dấu reset bởi nguồn điện (POR)
          ui IsSftReset;    // Cờ đánh dấu reset bởi phần mềm (SFT)
          ui IsIwdgReset;   // Cờ đánh dấu reset bởi watchdog độc lập (IWDG)
          ui IsWwdgReset;   // Cờ đánh dấu reset bởi watchdog cửa sổ (WWDG)
          ui IsLowPwrReset; // Cờ đánh dấu reset bởi chế độ điện áp thấp (Low Power)
        } RCC_RSTFLG_Typedef;
    #endif

  // Khai báo các hàm thành phần

    // >> Hàm thực hiện reset phần mềm hệ thống
    /*
     * Hàm thực hiện reset hệ thống bằng phần mềm (Software System Reset).
     *
     * Tham số: Không có.
     *
     * Logic:
     *   - Ghi giá trị VECTKEY vào thanh ghi SCB_AIRCR để mở khóa ghi.
     *   - Set bit SYSRESETREQ trong SCB_AIRCR để yêu cầu reset hệ thống.
     *   - Vào vòng lặp vô hạn chờ hệ thống reset.
     *   - Sau khi reset, hệ thống sẽ khởi động lại từ đầu.
     *
     * Trả về:
     *   Không có (void) - Hàm không bao giờ trả về vì hệ thống sẽ reset.
     *
     * Phụ thuộc ngoài module Reset:
     *   - SET_BIT() - Macro set bit thanh ghi
     *   - SCB_AIRCR_REG_PTR - Con trỏ tới thanh ghi AIRCR của SCB
     *   - SCB_AIRCR_REG_VECTKEY_SET - Giá trị key để mở khóa ghi
     *   - SCB_AIRCR_REG_SYSRESETREQ_SET - Bit yêu cầu reset hệ thống
     */
    stinl void RST_SYS_SW_Reset(void) {

      // Ghi giá trị VECTKEY và SYSRESETREQ vào thanh ghi AIRCR để thực hiện reset phần mềm

        SET_BIT(*SCB_AIRCR_REG_PTR, SCB_AIRCR_REG_VECTKEY_SET);
        SET_BIT(*SCB_AIRCR_REG_PTR, SCB_AIRCR_REG_SYSRESETREQ_SET); // Yêu cầu reset hệ thống

      // Chờ đợi cho đến khi hệ thống được reset

        while (1) {
          // Vòng lặp vô hạn chờ reset xảy ra
        }
    }

    // >> Hàm catch reset event sau khi khởi động lại hệ thống
    /*
     * Hàm capture (ghi lại) nguồn gây ra reset hệ thống sau khi khởi động lại.
     *
     * Tham số:
     *   reset_source - Con trỏ tới cấu trúc lưu trữ các cờ reset.
     *
     * Logic:
     *   - Kiểm tra con trỏ reset_source hợp lệ.
     *   - Đọc các bit cờ reset từ thanh ghi RCC_CSR:
     *       + PINRSTF: Reset từ chân NRST
     *       + PORRSTF: Power-On Reset
     *       + SFTRSTF: Software Reset
     *       + IWDGRSTF: Independent Watchdog Reset
     *       + WWDGRSTF: Window Watchdog Reset
     *       + LPWRRSTF: Low-Power Reset
     *   - Lưu trạng thái các cờ vào cấu trúc reset_source.
     *   - Ghi bit RMVF để clear (xóa) tất cả các cờ reset.
     *
     * Trả về:
     *   Không có (void).
     *
     * Phụ thuộc ngoài module Reset:
     *   - __NULL_PTR_CHECK() - Macro kiểm tra con trỏ NULL
     *   - READ_BIT() - Macro đọc bit thanh ghi
     *   - SET_BIT() - Macro set bit thanh ghi
     *   - RCC_REGS_PTR - Con trỏ tới cấu trúc thanh ghi RCC
     *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
     */
    stinl void RST_SRC_Capture(RCC_RSTFLG_Typedef *reset_source) {

      // Kiểm tra con trỏ đầu vào

        if (__NULL_PTR_CHECK(reset_source)) {
          return;
        }

      // Đọc trạng thái các cờ reset từ RCC_CSR_REG và gán vào cấu trúc reset_source

        reset_source->IsPinReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_PINRSTF_OCCURRED) == RCC_CSR_REG_PINRSTF_OCCURRED) ? SET : RESET;
        reset_source->IsPorReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_PORRSTF_OCCURRED) == RCC_CSR_REG_PORRSTF_OCCURRED) ? SET : RESET;
        reset_source->IsSftReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_SFTRSTF_OCCURRED) == RCC_CSR_REG_SFTRSTF_OCCURRED) ? SET : RESET;
        reset_source->IsIwdgReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_IWDGRSTF_OCCURRED) == RCC_CSR_REG_IWDGRSTF_OCCURRED) ? SET : RESET;
        reset_source->IsWwdgReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_WWDGRSTF_OCCURRED) == RCC_CSR_REG_WWDGRSTF_OCCURRED) ? SET : RESET;
        reset_source->IsLowPwrReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_LPWRRSTF_OCCURRED) == RCC_CSR_REG_LPWRRSTF_OCCURRED) ? SET : RESET;

      // Xóa các cờ reset đã đọc bằng cách ghi RMVF

        SET_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_RMVF_SET);
    }

#endif /* LIB_RESET_HAL_H_ */
