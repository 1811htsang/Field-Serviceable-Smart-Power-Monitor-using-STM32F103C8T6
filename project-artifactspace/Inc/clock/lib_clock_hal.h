/*
 * lib_clock_hal.h
 *
 *  Created on: Jan 14, 2026
 *      Author: shanghuang
 */

#ifndef LIB_CLOCK_HAL_H_
  #define LIB_CLOCK_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "generic/lib_condition_def.h"
      #include "clock/lib_clock_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_condition_def.h"
      #include "lib_clock_def.h"
    #endif

  // Khai báo cấu trúc tham số hàm khởi tạo

    #ifndef RCC_CLK_INIT_PARAM_TYPE
      #define RCC_CLK_INIT_PARAM_TYPE
      tdf_strc RCC_CLK_Init_Param{
        ul CLK_Source; // Chọn nguồn clock hệ thống
      } RCC_CLK_Init_Param;
    #endif

  // Khai báo các cờ trả về có thời hạn 

    #ifndef RCC_RDYFLG_TYPEDEF
      #define RCC_RDYFLG_TYPEDEF
      tdf_strc RCC_RDYFLG_Typdef {
        __vo ui HSI_RDY_FLG : 1;
        __vo ui HSE_RDY_FLG : 1;
        __vo ui LSI_RDY_FLG : 1;
      } RCC_RDYFLG_Typdef;
    #endif

    /**
     * Ghi chú:
     * Do các cờ trạng thái này sẽ được thiết lập về 0 trong 
     * N chu kỳ hoạt động tiếp theo của hệ thống,
     * Do đó cần có biến lưu trữ trạng thái này
     * để người dùng có thể kiểm tra trạng thái khởi tạo nguồn clock
     */

  // Khai báo các kiểm tra tham số đầu vào nội bộ

    #define IS_RCC_SYSCLK_SOURCE(SOURCE) (((SOURCE) == RCC_SYSCLK_SOURCE_HSI) || \
                                          ((SOURCE) == RCC_SYSCLK_SOURCE_HSE))
    #define IS_RCC_IWDG_SOURCE(SOURCE) (((SOURCE) == RCC_IWDG_SOURCE_LSI))

    #define IS_RCC_PERIPH(PERIPH) (((PERIPH) == AFIO) || \
                                            ((PERIPH) == GPIOA) || \
                                            ((PERIPH) == GPIOB) || \
                                            ((PERIPH) == GPIOC) || \
                                            ((PERIPH) == GPIOD) || \
                                            ((PERIPH) == GPIOE) || \
                                            ((PERIPH) == GPIOF) || \
                                            ((PERIPH) == GPIOG))

  // Khai báo các hàm thành phần

    // >> Hàm khởi tạo clock hệ thống
    RETR_STAT RCC_CLK_Init(
      RCC_CLK_Init_Param *init_param, 
      RCC_RDYFLG_Typdef *rdy_flg
    );

    // >> Hàm de-initialize clock hệ thống
    RETR_STAT RCC_CLK_DeInit(
      RCC_CLK_Init_Param *init_param, 
      RCC_RDYFLG_Typdef *rdy_flg
    );

    // >> Hàm hoạt hóa ngoại vi
    RETR_STAT RCC_PCLK_Enable(ul periph);

    // >> Hàm vô hiệu hóa ngoại vi
    RETR_STAT RCC_PCLK_Disable(ul periph);

    // >> Hàm chuyển đổi SYSCLK
    /*
     * Hàm chuyển đổi nguồn SYSCLK của hệ thống.
     *
     * Tham số:
     *   sysclk_source - Nguồn clock muốn chuyển sang (HSI hoặc HSE).
     *
     * Logic:
     *   - Kiểm tra tham số đầu vào hợp lệ.
     *   - Gán giá trị chọn nguồn SYSCLK vào thanh ghi cấu hình.
     *   - Kiểm tra lại trạng thái đã chuyển đổi thành công chưa.
     *
     * Trả về:
     *   RETR_STAT - STAT_DONE nếu thành công, STAT_ERROR nếu chuyển đổi thất bại.
     *
     * Phụ thuộc ngoài module Clock: Không có
     */
    stinl RETR_STAT RCC_SYSCLK_Switch(ui32 sysclk_source) {

      // Kiểm tra giá trị tham số đầu vào

        assert_param(IS_RCC_SYSCLK_SOURCE(sysclk_source));

      // Gán giá trị chọn nguồn SYSCLK vào thanh ghi cấu hình

        SET_BIT(RCC_REGS_PTR->CFGR, sysclk_source);

      // Kiểm tra lại trạng thái đã chuyển đổi

        /**
         * Ghi chú:
         * Điều kiện kiểm tra là
         * đọc lại thanh ghi cấu hình để kiểm tra xem nguồn SYSCLK đã được chuyển đổi thành công chưa
         * Nếu bit SWS (System Clock Switch Status) không phản ánh đúng nguồn SYSCLK đã chọn thì có thể coi là chuyển đổi thất bại
         */

        switch (sysclk_source) {
          case RCC_SYSCLK_SOURCE_HSI:

            if (
              __DIFF_CHECK(
                READ_BIT(RCC_REGS_PTR->CFGR, RCC_CFGR_REG_SWS_HSI),
                RCC_CFGR_REG_SWS_HSI
              )
            ) {
              return STAT_ERROR;
            }
            break;

          case RCC_SYSCLK_SOURCE_HSE:
            if (
              __DIFF_CHECK(
                READ_BIT(RCC_REGS_PTR->CFGR, RCC_CFGR_REG_SWS_HSE),
                RCC_CFGR_REG_SWS_HSE
              )
            ) {
              return STAT_ERROR;
            }
            break;

          default:
            return STAT_ERROR;
            break;
        }

      // Kết thúc quy trình chuyển đổi

        return STAT_DONE;
    }

    // >> Hàm bật CSS
    /*
     * Hàm bật Clock Security System (CSS) để bảo vệ khi HSE lỗi.
     * Không có tham số và không trả về giá trị.
     * Phụ thuộc ngoài module Clock: Không có
     */
    stinl void RCC_CSS_Enable(void) {
      SET_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_CSSON_SET);
    }

    // >> Hàm tắt CSS
    /*
     * Hàm tắt Clock Security System (CSS).
     * Không có tham số và không trả về giá trị.
     * Phụ thuộc ngoài module Clock: Không có
     */
    stinl void RCC_CSS_Disable(void) {
      CLEAR_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_CSSON_SET);
    }

    // >> Hàm kiểm tra clock HSI sẵn sàng
    /*
     * Hàm kiểm tra trạng thái sẵn sàng của nguồn clock HSI.
     *
     * Trả về:
     *   RETR_STAT - STAT_RDY nếu HSI đã sẵn sàng, STAT_NRDY nếu chưa.
     *
     * Phụ thuộc ngoài module Clock: Không có
     */
    stinl RETR_STAT RCC_IsHSIReady(void) {
      if (
        __DIFF_CHECK(
          READ_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_HSION_SET),
          RCC_CR_REG_HSION_SET
        )
      ) {
        return STAT_NRDY;
      } else {
        return STAT_RDY;
      }
    }

    // >> Hàm kiểm tra clock HSE sẵn sàng
    /*
     * Hàm kiểm tra trạng thái sẵn sàng của nguồn clock HSE.
     *
     * Trả về:
     *   RETR_STAT - STAT_RDY nếu HSE đã sẵn sàng, STAT_NRDY nếu chưa.
     *
     * Phụ thuộc ngoài module Clock: Không có
     */
    stinl RETR_STAT RCC_IsHSEReady(void) {
      if (
        __DIFF_CHECK(
          READ_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_HSEON_SET),
          RCC_CR_REG_HSEON_SET
        )
      ) {
        return STAT_NRDY;
      } else {
        return STAT_RDY;
      }
    }

#endif /* LIB_CLOCK_HAL_H_ */
