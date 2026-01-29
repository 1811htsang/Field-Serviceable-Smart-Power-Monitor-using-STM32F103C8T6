/*
 * lib_iwdg_impl.c
 *
 *  Created on: Jan 14, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
  #include "header_dependency.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
  #include "generic/lib_keyword_def.h"
  #include "generic/lib_condition_def.h"
  #include "iwdg/lib_iwdg_def.h"
  #include "iwdg/lib_iwdg_hal.h"

  #ifndef UNIT_TEST
  #include "clock/lib_clock_hal.h"
  #endif

// Định nghĩa các hàm thành phần

  /*
   * Hàm khởi tạo và cấu hình watchdog độc lập (IWDG).
   *
   * Tham số:
   *   init_param - Con trỏ tới cấu trúc tham số khởi tạo (Prescaler, Reload).
   *
   * Logic:
   *   - Kiểm tra con trỏ và giá trị tham số đầu vào.
   *   - Đảm bảo LSI đã sẵn sàng (bật nếu cần).
   *   - Bật quyền ghi, cấu hình prescaler và reload, kiểm tra trạng thái cập nhật.
   *   - Tắt quyền ghi sau khi cấu hình.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu thành công, STAT_ERROR nếu lỗi, STAT_NRDY nếu chưa cập nhật xong.
   */
  RETR_STAT IWDG_Init(IWDG_Init_Param *init_param) {

    // Kiểm tra con trỏ đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("IWDG_Init, DBG1: Check Null pointer.\n");
    }
    if (__NULL_PTR_CHECK(init_param)) {
      return STAT_ERROR;
    }


    // Kiểm tra giá trị tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("IWDG_Init, DBG2: Assert parameter.\n");
    }
    assert_param(IS_IWDG_PRESCALER(init_param->Prescaler));
    assert_param(IS_IWDG_RELOAD(init_param->Reload));


    // Bật LSI nếu chưa được bật
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("IWDG_Init, DBG3: Check LSI ready.\n");
    }
    if (__NRDY_CHECK(RCC_IsLSIReady())) {
      RCC_CLK_Init_Param rcc_lsi_init;
      rcc_lsi_init.CLK_Source = RCC_IWDG_SOURCE_LSI;
      RCC_RDYFLG_Typdef rdy_flg;
      if (!__OK_CHECK(RCC_CLK_Init(&rcc_lsi_init, &rdy_flg))) {
        return STAT_ERROR;
      }
    }


    // Kích hoạt quyền ghi vào các thanh ghi cấu hình IWDG
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("IWDG_Init, DBG4: Enable write access.\n");
    }
    IWDG_EnableWriteAccess();


    // Cấu hình bộ chia tần số
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("IWDG_Init, DBG5: Set prescaler.\n");
    }
    if (__RDY_CHECK(IWDG_IsPrescalerUpdated())) {
      IWDG_REGS_PTR->PR.PR = init_param->Prescaler;
    } else {
      IWDG_DisableWriteAccess();
      return STAT_NRDY;
    }


    // Cấu hình giá trị nạp lại
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("IWDG_Init, DBG6: Set reload value.\n");
    }
    if (__RDY_CHECK(IWDG_IsReloadValueUpdated())) {
      IWDG_REGS_PTR->RLR.RL = init_param->Reload;
    } else {
      IWDG_DisableWriteAccess();
      return STAT_NRDY;
    }


    // Tắt quyền ghi vào các thanh ghi cấu hình IWDG
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("IWDG_Init, DBG7: Disable write access.\n");
    }
    IWDG_DisableWriteAccess();

    
    // Kết thúc quy trình khởi tạo
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("IWDG_Init, DBG8: Setup procedure done.\n");
    }
    return STAT_DONE;
  }

  /*
   * Hàm de-initialize (reset) watchdog độc lập (IWDG).
   *
   * Tham số:
   *   init_param - Không sử dụng, chỉ để tương thích giao diện.
   *
   * Logic:
   *   - Reload bộ đếm về giá trị mặc định.
   *   - Khóa quyền ghi thanh ghi cấu hình.
   *
   * Trả về:
   *   RETR_STAT - Luôn trả về STAT_DONE.
   */
  RETR_STAT IWDG_DeInit(IWDG_Init_Param *init_param) {
    // Reload bộ đếm IWDG về giá trị mặc định
    IWDG_Reload();

    // Khóa quyền ghi vào các thanh ghi cấu hình IWDG
    IWDG_DisableWriteAccess();

    return STAT_DONE;
  }

  /*
   * Hàm khởi động watchdog độc lập (IWDG).
   * Ghi giá trị KEY_START vào thanh ghi KR để bắt đầu hoạt động.
   * Không có tham số và không trả về giá trị.
   */
  void IWDG_Start(void) {
    IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_START;
  }

  /*
   * Hàm tải lại (feed) bộ đếm watchdog độc lập (IWDG).
   * Ghi giá trị KEY_RELOAD_COUNTER vào thanh ghi KR để reset bộ đếm.
   * Không có tham số và không trả về giá trị.
   */
  void IWDG_Reload(void) {
    IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_RELOAD_COUNTER;
  }

  /*
   * Hàm kiểm tra trạng thái cập nhật giá trị reload của IWDG.
   *
   * Logic:
   *   - Nếu bit RVU (Reload Value Update) đang SET, việc cập nhật đang diễn ra.
   *   - Nếu bit RVU RESET, đã cập nhật xong.
   *
   * Trả về:
   *   RETR_STAT - STAT_RDY nếu đã cập nhật xong, STAT_NRDY nếu đang cập nhật.
   */
  RETR_STAT IWDG_IsReloadValueUpdated(void) {
    if (IWDG_REGS_PTR->SR.RVU == SET) {
      return STAT_NRDY;
    }
    return STAT_RDY;
  }

  /*
   * Hàm kiểm tra trạng thái cập nhật prescaler của IWDG.
   *
   * Logic:
   *   - Nếu bit PVU (Prescaler Value Update) đang SET, việc cập nhật đang diễn ra.
   *   - Nếu bit PVU RESET, đã cập nhật xong.
   *
   * Trả về:
   *   RETR_STAT - STAT_RDY nếu đã cập nhật xong, STAT_NRDY nếu đang cập nhật.
   */
  RETR_STAT IWDG_IsPrescalerUpdated(void) {
    if (IWDG_REGS_PTR->SR.PVU == SET) {
      return STAT_NRDY;
    }
    return STAT_RDY;
  }

  /*
   * Hàm bật quyền ghi vào thanh ghi cấu hình IWDG.
   * Ghi giá trị KEY_ENABLE_ACCESS vào thanh ghi KR.
   * Không có tham số và không trả về giá trị.
   */
  void IWDG_EnableWriteAccess(void) {
    IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_ENABLE_ACCESS;
  }

  /*
   * Hàm tắt quyền ghi vào thanh ghi cấu hình IWDG.
   * Ghi giá trị KEY_DISABLE_ACCESS vào thanh ghi KR.
   * Không có tham số và không trả về giá trị.
   */
  void IWDG_DisableWriteAccess(void) {
    IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_DISABLE_ACCESS;
  }

  /*
   * Hàm xử lý sự kiện reset do IWDG.
   * (Chưa triển khai, placeholder cho mở rộng driver reset hoặc callback user).
   * Không có tham số và không trả về giá trị.
   */
  void IWDG_ResetEvent_Catch(void) {
    // TODO: Bổ sung xử lý sự kiện reset do IWDG nếu cần
  }
