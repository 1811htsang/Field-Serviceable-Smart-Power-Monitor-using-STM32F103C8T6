/*
 * lib_clock_impl.c
 *
 *  Created on: Jan 9, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
		#include "lib_keyword_def.h"
		#include "lib_condition_def.h"
		#include "lib_clock_def.h"
		#include "lib_clock_hal.h"
    #include "header_dependency.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
	#include <string.h>

  #ifndef UNIT_TEST
		#include "generic/lib_keyword_def.h"
  	#include "generic/lib_condition_def.h"
  	#include "clock/lib_clock_def.h"
  	#include "clock/lib_clock_hal.h"
    #include "iwdg/lib_iwdg_hal.h"
  #endif

// Định nghĩa các hàm thành phần

  /*
   * Hàm khởi tạo nguồn clock hệ thống (HSI, HSE, LSI).
   *
   * Tham số:
   *   init_param - Con trỏ tới cấu trúc tham số khởi tạo clock (chọn nguồn clock).
   *   rdy_flg    - Con trỏ tới biến lưu trạng thái sẵn sàng của từng nguồn clock.
   *
   * Logic chính:
   *   - Kiểm tra con trỏ đầu vào hợp lệ.
   *   - Kiểm tra giá trị nguồn clock hợp lệ (HSI, HSE, LSI).
   *   - Làm mới biến cờ trạng thái sẵn sàng.
   *   - Bật nguồn clock tương ứng, chờ sẵn sàng, xử lý watchdog nếu cần.
   *   - Lưu trạng thái sẵn sàng vào biến trả về.
   *   - Đối với HSE: bật CSS trước, chuyển SYSCLK sang HSE sau khi sẵn sàng.
   *   - Đối với HSI: không cần chuyển SYSCLK vì mặc định đã là HSI.
   *   - Đối với LSI: chỉ bật và chờ sẵn sàng.
   *   - Trong quá trình chờ HSI/HSE, nếu LSI chưa sẵn sàng thì khởi tạo LSI và IWDG để tránh treo hệ thống.
   *
   * Trả về:
   *   RETR_STAT - STAT_OK nếu thành công, STAT_ERROR nếu lỗi tham số hoặc khởi tạo, STAT_DONE nếu hoàn tất quy trình.
   */
  RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param *init_param, RCC_RDYFLG_Typdef *rdy_flg) {
    
    if (DEBUG_MODE == ENABLE) {
      printf("RCC_CLK_Init, DBG1: Check Null pointer.\n");
    }

    if (init_param == NULL) {
      return STAT_ERROR;
    }

    if (DEBUG_MODE == ENABLE) {
      printf("RCC_CLK_Init, DBG2: Assert parameter.\n");
    }

    assert_param(
      IS_RCC_SYSCLK_SOURCE(init_param->CLK_Source) || 
      IS_RCC_IWDG_SOURCE(init_param->CLK_Source)
    );

    if (DEBUG_MODE == ENABLE) {
      printf("RCC_CLK_Init, DBG3: Refresh Clock ready flags kit.\n");
    }

    memset(rdy_flg, 0, sizeof(RCC_RDYFLG_Typdef));

    if (DEBUG_MODE == ENABLE) {
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

        RCC_REGS_PTR->CR.HSION = SET; // Bật HSI

        if (DEBUG_MODE == ENABLE) {
          printf("RCC_CLK_Init, DBG5: Wait for ready flag.\n");
        }
        
        while (RCC_REGS_PTR->CR.HSIRDY == RESET) {
          // Chờ HSI sẵn sàng

          if (RCC_IsLSIReady() != STAT_RDY) {
            
            /**
             * Ghi chú:
             * Nếu LSI chưa được bật, tiến hành khởi tạo LSI
             * để IWDG có thể hoạt động trong vòng chờ HSI ổn định
             * Bật IWDG init trong RCC_CLK_Init để đảm bảo khi LSI
             * được bật thì IWDG cũng được cấu hình đúng cách
             */

            RCC_CLK_Init_Param rcc_lsi_init;
            rcc_lsi_init.CLK_Source = RCC_IWDG_SOURCE_LSI;
            RCC_RDYFLG_Typdef lsi_rdy_flg;
            if (RCC_CLK_Init(&rcc_lsi_init, &lsi_rdy_flg) != STAT_OK) {
              return STAT_ERROR;
            }

            IWDG_Init_Param iwdg_init = {
              .Prescaler = IWDG_PR_REG_PR_DIV_4, // Giá trị prescaler là 4
              .Reload = IWDG_RLR_REG_RL_MAX      // Giá trị reload là max
            };

            IWDG_Init(&iwdg_init);
            IWDG_Start();
          }

          /**
           * Ghi chú:
           * Ở đây, đã đảm bảo LSI sẵn sàng để IWDG hoạt động, 
           * chỉ cần thực hiện việc feed IWDG định kỳ trong vòng chờ HSI ổn định.
           */

          IWDG_Reload();
        }

        /**
         * Ghi chú:
         * Lúc này cờ trạng thái sẵn sàng của HSI sẽ được thiết lập
         * Thực hiện lưu cờ trạng thái vào biến trả về
         */ 

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
        
        RCC_CSS_Enable();

        RCC_REGS_PTR->CR.HSEON = SET; // Bật HSE

        if (DEBUG_MODE == ENABLE) {
          printf("RCC_CLK_Init, DBG5: Wait for ready flag.\n");
        }

        while (RCC_REGS_PTR->CR.HSERDY == RESET) {
          // Chờ HSE sẵn sàng

          if (RCC_IsLSIReady() != STAT_RDY) {
            
            /**
             * Ghi chú:
             * Nếu LSI chưa được bật, tiến hành khởi tạo LSI
             * để IWDG có thể hoạt động trong vòng chờ HSI ổn định
             * Bật IWDG init trong RCC_CLK_Init để đảm bảo khi LSI
             * được bật thì IWDG cũng được cấu hình đúng cách
             */

            RCC_CLK_Init_Param rcc_lsi_init;
            rcc_lsi_init.CLK_Source = RCC_IWDG_SOURCE_LSI;
            RCC_RDYFLG_Typdef lsi_rdy_flg;
            if (RCC_CLK_Init(&rcc_lsi_init, &lsi_rdy_flg) != STAT_OK) {
              return STAT_ERROR;
            }

            IWDG_Init_Param iwdg_init = {
              .Prescaler = IWDG_PR_REG_PR_DIV_4, // Giá trị prescaler là 4
              .Reload = IWDG_RLR_REG_RL_MAX      // Giá trị reload là max
            };

            IWDG_Init(&iwdg_init);
            IWDG_Start();
          }

          /**
           * Ghi chú:
           * Ở đây, đã đảm bảo LSI sẵn sàng để IWDG hoạt động, 
           * chỉ cần thực hiện việc feed IWDG định kỳ trong vòng chờ HSI ổn định.
           */

          IWDG_Reload();
        }

        /**
         * Ghi chú:
         * Lúc này cờ trạng thái sẵn sàng của HSE sẽ được thiết lập
         * Thực hiện lưu cờ trạng thái vào biến trả về
         */

        rdy_flg->HSE_RDY_FLG = SET;

        if (DEBUG_MODE == ENABLE) {
          printf("RCC_CLK_Init, DBG5-1: Select SYSCLK.\n");
        }

        if (RCC_SYSCLK_Switch(RCC_SYSCLK_SOURCE_HSE) != STAT_DONE) {
          return STAT_ERROR; // Chuyển đổi nguồn SYSCLK thất bại
        }

        return STAT_OK;

        break;

      case 0x0Ful: // LSI
      
        RCC_REGS_PTR->CSR.LSION = SET; // Bật LSI  

        if (DEBUG_MODE == ENABLE) {
          printf("RCC_CLK_Init, DBG5: Wait for ready flag.\n");
        }

        // Chờ LSI sẵn sàng
        while (RCC_REGS_PTR->CSR.LSIRDY == RESET) {}

        /**
         * Ghi chú:
         * Lúc này cờ trạng thái sẵn sàng của LSI sẽ được thiết lập
         * Thực hiện lưu cờ trạng thái vào biến trả về
         */

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

    if (DEBUG_MODE == ENABLE) {
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
   * Logic chính:
   *   - Kiểm tra tham số đầu vào hợp lệ.
   *   - Gán giá trị chọn nguồn SYSCLK vào thanh ghi cấu hình.
   *   - Kiểm tra lại trạng thái đã chuyển đổi thành công chưa.
   *
   * Trả về:
   *   RETR_STAT - STAT_DONE nếu thành công, STAT_ERROR nếu chuyển đổi thất bại.
   */
  RETR_STAT RCC_SYSCLK_Switch(ul sysclk_source) {

    if (DEBUG_MODE == ENABLE) {
      printf("RCC_SYSCLK_Switch, DBG1: Assert parameter.\n");
    }

    assert_param(IS_RCC_SYSCLK_SOURCE(sysclk_source));

    if (DEBUG_MODE == ENABLE) {
      printf("RCC_SYSCLK_Switch, DBG2: Switch SYSCLK source.\n");
    }
    
    RCC_REGS_PTR->CFGR.SW = sysclk_source; // Chọn HSE làm nguồn SYSCLK

    if (RCC_REGS_PTR->CFGR.SWS != sysclk_source) {
      return STAT_ERROR; // Chuyển đổi nguồn SYSCLK thất bại
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
   * Logic chính:
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

    if (DEBUG_MODE == ENABLE) {
      printf("RCC_CLK_DeInit, DBG1: Check Null pointer.\n");
    }

    if (init_param == NULL) {
      return STAT_ERROR;
    }

    if (DEBUG_MODE == ENABLE) {
      printf("RCC_CLK_DeInit, DBG2: Assert parameter.\n");
    }

    assert_param(
      IS_RCC_SYSCLK_SOURCE(init_param->CLK_Source) || 
      IS_RCC_IWDG_SOURCE(init_param->CLK_Source)
    );

    if (DEBUG_MODE == ENABLE) {
      printf("RCC_CLK_DeInit, DBG3: Clear Clock ready flags kit.\n");
    }

    memset(rdy_flg, 0, sizeof(RCC_RDYFLG_Typdef));

    if (DEBUG_MODE == ENABLE) {
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

      return STAT_BUSY;
      break;
    
    case RCC_SYSCLK_SOURCE_HSE:
      
      /**
       * Ghi chú:
       * Trước khi tắt HSE cần chuyển SYSCLK về HSI
       * Kiểm tra trạng thái của HSI
       * Nếu HSI chưa được bật thì tiến hành bật HSI trước
       */

      if (RCC_IsHSIReady() != STAT_RDY) {
        RCC_CLK_Init_Param rcc_hsi_init;
        rcc_hsi_init.CLK_Source = RCC_SYSCLK_SOURCE_HSI;
        RCC_RDYFLG_Typdef hsi_rdy_flg;
        if (RCC_CLK_Init(&rcc_hsi_init, &hsi_rdy_flg) != STAT_OK) {
          return STAT_ERROR;
        }
      }

      if (RCC_SYSCLK_Switch(RCC_SYSCLK_SOURCE_HSI) != STAT_DONE) {
        return STAT_ERROR; // Chuyển đổi nguồn SYSCLK thất bại
      }

      RCC_REGS_PTR->CR.HSEON = RESET; // Tắt HSE
      break;
    
    case RCC_IWDG_SOURCE_LSI:

      /**
       * Ghi chú:
       * Không thể tắt LSI sau khi đã khởi động IWDG
       * Do đó, nếu LSI đang được sử dụng làm nguồn clock cho IWDG
       * IWDG cũng không thể tắt
       */

      return STAT_ERROR;
      break;

    default:
      return STAT_ERROR;
      break;
    }

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
   * Hàm xử lý ngắt NMI do CSS (chưa triển khai, placeholder cho mở rộng).
   * Không có tham số và không trả về giá trị.
   */
  void RCC_NMI_IRQ_Handler(void) {

  }

  /*
   * Hàm xử lý ngắt chung RCC (chưa triển khai, placeholder cho mở rộng).
   * Không có tham số và không trả về giá trị.
   */
  void RCC_GNR_IRQ_Handler(void) {

  }

  /*
   * Hàm callback yếu cho CSS (có thể override ở user code).
   * Không có tham số và không trả về giá trị.
   */
  __weak void RCC_CSS_Callback(void) {

  }

  /*
   * Hàm kiểm tra trạng thái sẵn sàng của nguồn clock HSI.
   *
   * Trả về:
   *   RETR_STAT - STAT_RDY nếu HSI đã sẵn sàng, STAT_NRDY nếu chưa.
   */
  RETR_STAT RCC_IsHSIReady(void) {
    if (RCC_REGS_PTR->CR.HSIRDY == SET) {
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
    if (RCC_REGS_PTR->CR.HSERDY == SET) {
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
    if (RCC_REGS_PTR->CSR.LSIRDY == SET) {
      return STAT_RDY;
    } else {
      return STAT_NRDY;
    }
  }
