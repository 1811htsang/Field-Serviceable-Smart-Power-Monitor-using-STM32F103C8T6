/*
 * lib_clock_impl.c
 *
 *  Created on: Jan 9, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #include <stdint.h>
  #include <stdio.h>
	#include <string.h>
  #include "generic/lib_keyword_def.h"
  #include "generic/lib_condition_def.h"
  #include "clock/lib_clock_def.h"
  #include "clock/lib_clock_hal.h"
  #include "iwdg/lib_iwdg_hal.h"

// Định nghĩa các hàm thành phần

  /*
   * Hàm khởi tạo và cấu hình nguồn clock hệ thống (HSI, HSE, LSI).
   *
   * Tham số:
   *   init_param - Con trỏ tới cấu trúc tham số khởi tạo (chọn nguồn clock).
   *   rdy_flg    - Con trỏ tới biến lưu trạng thái sẵn sàng của từng nguồn clock.
   *
   * Logic:
   *   - Kiểm tra con trỏ và giá trị tham số đầu vào.
   *   - Làm mới biến cờ trạng thái sẵn sàng.
   *   - Bật nguồn clock tương ứng, chờ sẵn sàng, xử lý watchdog nếu cần.
   *   - Lưu trạng thái sẵn sàng vào biến trả về.
   *   - Đối với HSE: bật CSS trước, chuyển SYSCLK sang HSE sau khi sẵn sàng.
   *   - Đối với HSI: không cần chuyển SYSCLK vì mặc định đã là HSI.
   *   - Đối với LSI: chỉ bật và chờ sẵn sàng.
   *   - Trong quá trình chờ HSI/HSE, nếu LSI chưa sẵn sàng thì khởi tạo LSI và IWDG để tránh treo hệ thống.
   *
   * Trả về:
   *   RETR_STAT - STAT_OK nếu thành công, STAT_ERROR nếu lỗi, STAT_DONE nếu hoàn tất quy trình.
   */
  RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param *init_param, RCC_RDYFLG_Typdef *rdy_flg) {
    
    // Kiểm tra con trỏ đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_CLK_Init, DBG1: Check Null pointer.\n");
    }
    if (__NULL_PTR_CHECK(init_param)) {
      return STAT_ERROR;
    }


    // Kiểm tra giá trị tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_CLK_Init, DBG2: Assert parameter.\n");
    }
    assert_param(
      IS_RCC_SYSCLK_SOURCE(init_param->CLK_Source) || 
      IS_RCC_IWDG_SOURCE(init_param->CLK_Source)
    );


    // Làm mới biến cờ trạng thái
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_CLK_Init, DBG3: Refresh Clock ready flags kit.\n");
    }
    memset(rdy_flg, 0, sizeof(RCC_RDYFLG_Typdef));


    // Bật nguồn clock tương ứng
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_CLK_Init, DBG4: Turn on clock.\n");
    }

    
    /**
     * Ghi chú:
     * Trong thiết phần cứng, việc kiểm tra hoạt động của HSI/HSE/LSI
     * được thực hiện thông qua các flag hoặc là ngắt.
     * Do đó, trong hàm khởi tạo này, ta chỉ cần thiết lập
     * và kiểm tra các flag tương ứng để đảm bảo nguồn clock được bật đúng cách.
     */
    switch (init_param->CLK_Source) {

      case 0x00ul: // HSI

        /**
         * Ghi chú:
         * Trong thiết kế của STM32F1, HSI luôn được bật từ lúc khởi động (POW reset)
         * nên được config sẵn là nguồn clock mặc định của SYSCLK.
         * Do đó, khởi tạo HSI chỉ cần đảm bảo HSI được bật đúng cách.
         */

        // Bật HSI
        RCC_REGS_PTR->CR.HSION = SET;

        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("RCC_CLK_Init, DBG5: Wait for ready flag.\n");
        }
        
        // Chờ HSI sẵn sàng
        while (__RESET_FLAG_CHECK(RCC_REGS_PTR->CR.HSIRDY)) {
          // Nếu LSI chưa sẵn sàng thì khởi tạo LSI và IWDG
          if (__NRDY_CHECK(RCC_IsLSIReady())) {
            
            /**
             * Ghi chú:
             * Nếu LSI chưa được bật, tiến hành khởi tạo LSI
             * để IWDG có thể hoạt động trong vòng chờ HSI ổn định
             * Bật IWDG init trong RCC_CLK_Init để đảm bảo khi LSI
             * được bật thì IWDG cũng được cấu hình đúng cách
             */

            // Khởi tạo LSI
            RCC_CLK_Init_Param rcc_lsi_init;
            rcc_lsi_init.CLK_Source = RCC_IWDG_SOURCE_LSI;
            RCC_RDYFLG_Typdef lsi_rdy_flg;
            if (RCC_CLK_Init(&rcc_lsi_init, &lsi_rdy_flg) != STAT_OK) {
              return STAT_ERROR;
            }
            // Khởi tạo và start IWDG
            IWDG_Init_Param iwdg_init = {
              .Prescaler = IWDG_PR_REG_PR_DIV_4,
              .Reload = IWDG_RLR_REG_RL_MAX
            };
            IWDG_Init(&iwdg_init);
            IWDG_Start();
          }

          /**
           * Ghi chú:
           * Ở đây, đã đảm bảo LSI sẵn sàng để IWDG hoạt động, 
           * chỉ cần thực hiện việc feed IWDG định kỳ trong vòng chờ HSI ổn định.
           */

          // Feed IWDG định kỳ trong vòng chờ
          IWDG_Reload();
        }

        /**
         * Ghi chú:
         * Lúc này cờ trạng thái sẵn sàng của HSI sẽ được thiết lập
         * Thực hiện lưu cờ trạng thái vào biến trả về
         */ 

        // Lưu trạng thái sẵn sàng HSI
        rdy_flg->HSI_RDY_FLG = SET;

        /**
         * Ghi chú:
         * Khi khởi động hệ thống HSI được chọn làm SYSCLK mặc định,
         * Do đó không cần phải thực hiện chuyển đổi nguồn SYSCLK
         */

        return STAT_OK;

        break;

      case 0x01ul: // HSE
        
        /**
         * Ghi chú:
         * Bật CSS trước để đảm bảo hệ thống được bảo vệ ngay 
         * khi HSE được kích hoạt
         */
        
        // Bật CSS trước khi bật HSE
        RCC_CSS_Enable();
        // Bật HSE
        RCC_REGS_PTR->CR.HSEON = SET;

        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("RCC_CLK_Init, DBG5: Wait for ready flag.\n");
        }

        // Chờ HSE sẵn sàng
        while (__RESET_FLAG_CHECK(RCC_REGS_PTR->CR.HSERDY)) {
          // Nếu LSI chưa sẵn sàng thì khởi tạo LSI và IWDG
          if (__NRDY_CHECK(RCC_IsLSIReady())) {
            
            /**
             * Ghi chú:
             * Nếu LSI chưa được bật, tiến hành khởi tạo LSI
             * để IWDG có thể hoạt động trong vòng chờ HSI ổn định
             * Bật IWDG init trong RCC_CLK_Init để đảm bảo khi LSI
             * được bật thì IWDG cũng được cấu hình đúng cách
             */

            // Khởi tạo LSI
            RCC_CLK_Init_Param rcc_lsi_init;
            rcc_lsi_init.CLK_Source = RCC_IWDG_SOURCE_LSI;
            RCC_RDYFLG_Typdef lsi_rdy_flg;
            if (!__OK_CHECK(RCC_CLK_Init(&rcc_lsi_init, &lsi_rdy_flg))) {
              return STAT_ERROR;
            }
            // Khởi tạo và start IWDG
            IWDG_Init_Param iwdg_init = {
              .Prescaler = IWDG_PR_REG_PR_DIV_4,
              .Reload = IWDG_RLR_REG_RL_MAX
            };
            IWDG_Init(&iwdg_init);
            IWDG_Start();
          }

          /**
           * Ghi chú:
           * Ở đây, đã đảm bảo LSI sẵn sàng để IWDG hoạt động, 
           * chỉ cần thực hiện việc feed IWDG định kỳ trong vòng chờ HSI ổn định.
           */

          // Feed IWDG định kỳ trong vòng chờ
          IWDG_Reload();
        }

        /**
         * Ghi chú:
         * Lúc này cờ trạng thái sẵn sàng của HSE sẽ được thiết lập
         * Thực hiện lưu cờ trạng thái vào biến trả về
         */

        // Lưu trạng thái sẵn sàng HSE
        rdy_flg->HSE_RDY_FLG = SET;

        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("RCC_CLK_Init, DBG5-1: Select SYSCLK.\n");
        }

        if (!__DONE_CHECK(RCC_SYSCLK_Switch(RCC_SYSCLK_SOURCE_HSE))) {
          return STAT_ERROR; // Chuyển đổi nguồn SYSCLK thất bại
        }

        return STAT_OK;

        break;

      case 0x0Ful: // LSI
      
        // Bật LSI
        RCC_REGS_PTR->CSR.LSION = SET;

        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("RCC_CLK_Init, DBG5: Wait for ready flag.\n");
        }

        // Chờ LSI sẵn sàng
        // Chờ LSI sẵn sàng
        while (__RESET_FLAG_CHECK(RCC_REGS_PTR->CSR.LSIRDY)) {}

        /**
         * Ghi chú:
         * Lúc này cờ trạng thái sẵn sàng của LSI sẽ được thiết lập
         * Thực hiện lưu cờ trạng thái vào biến trả về
         */

        // Lưu trạng thái sẵn sàng LSI
        rdy_flg->LSI_RDY_FLG = SET;

        return STAT_OK;

        break;
      
      default:

        return STAT_ERROR;
        
        break;
    }

    /**
     * Ghi chú:
     * Ở đây sử dụng STAT_DONE để biểu thị hàm thực thi thành công
     * Còn kết quả cụ thể của việc khởi tạo nguồn clock
     * sẽ được kiểm tra thông qua các flag trạng thái tương ứng
     * của switch bên trên.
     */

    // Kết thúc quy trình khởi tạo
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_CLK_Init, DBG6: Setup procedure done.\n");
    }

    return STAT_DONE;
  }

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
   */
  RETR_STAT RCC_SYSCLK_Switch(ul sysclk_source) {

    // Kiểm tra tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_SYSCLK_Switch, DBG1: Assert parameter.\n");
    }
    assert_param(IS_RCC_SYSCLK_SOURCE(sysclk_source));

    // Gán giá trị chọn nguồn SYSCLK
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_SYSCLK_Switch, DBG2: Switch SYSCLK source.\n");
    }
    RCC_REGS_PTR->CFGR.SW = sysclk_source;

    // Kiểm tra lại trạng thái đã chuyển đổi
    if (__DIFF_CHECK(RCC_REGS_PTR->CFGR.SWS, sysclk_source)) {
      return STAT_ERROR;
    }

    return STAT_DONE;
  }

  /*
   * Hàm de-initialize (tắt) nguồn clock hệ thống.
   *
   * Tham số:
   *   init_param - Con trỏ tới cấu trúc tham số nguồn clock cần deinit.
   *   rdy_flg    - Con trỏ tới biến lưu trạng thái sẵn sàng (được clear).
   *
   * Logic:
   *   - Kiểm tra con trỏ đầu vào hợp lệ.
   *   - Làm mới biến cờ trạng thái.
   *   - Với HSE: chuyển SYSCLK về HSI trước khi tắt HSE.
   *   - Với HSI: không thể tắt HSI (nguồn dự phòng), trả về STAT_BUSY.
   *   - Với LSI: không thể tắt LSI nếu đang dùng cho IWDG, trả về STAT_ERROR.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu thành công, STAT_ERROR nếu lỗi, STAT_BUSY nếu không thể tắt HSI.
   */
  RETR_STAT RCC_CLK_DeInit(RCC_CLK_Init_Param *init_param, RCC_RDYFLG_Typdef *rdy_flg) {

    /**
     * Ghi chú:
     * Ở đây sử dụng STAT_DONE để biểu thị hàm thực thi thành công
     * Còn kết quả cụ thể của việc khởi tạo nguồn clock
     * sẽ được kiểm tra thông qua các flag trạng thái tương ứng
     * của switch bên trên.
     */

    // Kiểm tra con trỏ đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_CLK_DeInit, DBG1: Check Null pointer.\n");
    }
    if (__NULL_PTR_CHECK(init_param)) {
      return STAT_ERROR;
    }

    // Kiểm tra giá trị tham số đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_CLK_DeInit, DBG2: Assert parameter.\n");
    }
    assert_param(
      IS_RCC_SYSCLK_SOURCE(init_param->CLK_Source) || 
      IS_RCC_IWDG_SOURCE(init_param->CLK_Source)
    );

    // Làm mới biến cờ trạng thái
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_CLK_DeInit, DBG3: Clear Clock ready flags kit.\n");
    }
    memset(rdy_flg, 0, sizeof(RCC_RDYFLG_Typdef));

    // Tắt nguồn clock tương ứng
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RCC_CLK_DeInit, DBG4: Turn off clock.\n");
    }

    /**
     * Ghi chú:
     * Ở đây, SYSCLK nếu đã select là HSE
     * Phải thực hiện chuyển đổi về HSI trước khi tắt HSE
     * Nghĩa là cần kiểm tra trạng thái hiện tại của SYSCLK
     * Nếu là HSE thì init clock về HSI trước
     */

    switch (init_param->CLK_Source) {
      case RCC_SYSCLK_SOURCE_HSI:
        
        /**
         * Ghi chú:
         * Do HSI là nguồn clock mặc định của hệ thống
         * Nên không thể tắt HSI khi có/không sử dụng làm nguồn SYSCLK
         * Vì HSI là nguồn clock dự phòng cho hệ thống
         */

        // Không thể tắt HSI (nguồn dự phòng)
        return STAT_BUSY;
        break;
      
      case RCC_SYSCLK_SOURCE_HSE:
        
        /**
         * Ghi chú:
         * Trước khi tắt HSE cần chuyển SYSCLK về HSI
         * Kiểm tra trạng thái của HSI
         * Nếu HSI chưa được bật thì tiến hành bật HSI trước
         */

        // Nếu HSI chưa sẵn sàng thì khởi tạo HSI trước khi tắt HSE
        if (__NRDY_CHECK(RCC_IsHSIReady())) {
          RCC_CLK_Init_Param rcc_hsi_init;
          rcc_hsi_init.CLK_Source = RCC_SYSCLK_SOURCE_HSI;
          RCC_RDYFLG_Typdef hsi_rdy_flg;
          if (RCC_CLK_Init(&rcc_hsi_init, &hsi_rdy_flg) != STAT_OK) {
            return STAT_ERROR;
          }
        }
        // Chuyển SYSCLK về HSI
        if (!__DONE_CHECK(RCC_SYSCLK_Switch(RCC_SYSCLK_SOURCE_HSI))) {
          return STAT_ERROR;
        }
        // Tắt HSE
        RCC_REGS_PTR->CR.HSEON = RESET;
        break;
      
      case RCC_IWDG_SOURCE_LSI:

        /**
         * Ghi chú:
         * Không thể tắt LSI sau khi đã khởi động IWDG
         * Do đó, nếu LSI đang được sử dụng làm nguồn clock cho IWDG
         * IWDG cũng không thể tắt
         */

        // Không thể tắt LSI nếu đang dùng cho IWDG
        return STAT_ERROR;
        break;

      default:
        // Tham số không hợp lệ
        return STAT_ERROR;
        break;
    }

    // Kết thúc quy trình deinit
    return STAT_DONE;
  }

  /*
   * Hàm bật Clock Security System (CSS) để bảo vệ khi HSE lỗi.
   * Không có tham số và không trả về giá trị.
   */
  void RCC_CSS_Enable(void) {
    RCC_REGS_PTR->CR.CSSON = SET;
  }

  /*
   * Hàm tắt Clock Security System (CSS).
   * Không có tham số và không trả về giá trị.
   */
  void RCC_CSS_Disable(void) {
    RCC_REGS_PTR->CR.CSSON = RESET;
  }

  /*
   * Hàm xử lý ngắt NMI do Clock Security System (CSS).
   *
   * Logic:
   *   - Kiểm tra cờ CSSF (Clock Security System Failure).
   *   - Nếu có lỗi clock (CSSF == SET):
   *       + Gọi callback RCC_CSS_Callback() để user xử lý lỗi.
   *       + Xóa cờ CSSF bằng cách ghi vào trường CSSC.
   *
   * Không có tham số và không trả về giá trị.
   */
  void RCC_NMI_IRQ_Handler(void) {

    if (__SET_FLAG_CHECK(RCC_REGS_PTR->CIR.CSSF)) {
      // Xử lý sự kiện lỗi clock (CSS Failure)
      RCC_CSS_Callback();
      // Xóa cờ CSSF với CSSC
      RCC_REGS_PTR->CIR.CSSC = SET;
    }

  }

  /*
   * Hàm callback yếu cho CSS (có thể override ở user code).
   * Không có tham số và không trả về giá trị.
   */
  __weak void RCC_CSS_Callback(void) {
    /**
     * Ghi chú:
     * Ở đây chỉ là hàm callback yếu,
     * Người dùng có thể override hàm này trong code của họ
     * để xử lý sự kiện lỗi clock theo nhu cầu cụ thể.
     */
  }

  /*
   * Hàm kiểm tra trạng thái sẵn sàng của nguồn clock HSI.
   *
   * Trả về:
   *   RETR_STAT - STAT_RDY nếu HSI đã sẵn sàng, STAT_NRDY nếu chưa.
   */
  RETR_STAT RCC_IsHSIReady(void) {
    if (__SET_FLAG_CHECK(RCC_REGS_PTR->CR.HSIRDY)) {
      return STAT_RDY;
    } else {
      return STAT_NRDY;
    }
  }

  /*
   * Hàm kiểm tra trạng thái sẵn sàng của nguồn clock HSE.
   *
   * Trả về:
   *   RETR_STAT - STAT_RDY nếu HSE đã sẵn sàng, STAT_NRDY nếu chưa.
   */
  RETR_STAT RCC_IsHSEReady(void) {
    if (__SET_FLAG_CHECK(RCC_REGS_PTR->CR.HSERDY)) {
      return STAT_RDY;
    } else {
      return STAT_NRDY;
    }
  }

  /*
   * Hàm kiểm tra trạng thái sẵn sàng của nguồn clock LSI.
   *
   * Trả về:
   *   RETR_STAT - STAT_RDY nếu LSI đã sẵn sàng, STAT_NRDY nếu chưa.
   */
  RETR_STAT RCC_IsLSIReady(void) {
    if (__SET_FLAG_CHECK(RCC_REGS_PTR->CSR.LSIRDY)) {
      return STAT_RDY;
    } else {
      return STAT_NRDY;
    }
  }
