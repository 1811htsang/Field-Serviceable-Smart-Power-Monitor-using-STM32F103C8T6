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
   * 
   * Phụ thuộc ngoài module Clock:
   *   - IWDG_Init_Param
   *   - IWDG_Init()
   *   - IWDG_Start()
   *   - IWDG_Reload()
   *   - IWDG_PR_REG_PR_DIV_4
   *   - IWDG_RLR_REG_RL_MAX
   */
  RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param *init_param, RCC_RDYFLG_Typdef *rdy_flg) {
    
    // Kiểm tra con trỏ đầu vào

      if (__NULL_PTR_CHECK(init_param)) {
        return STAT_ERROR;
      }
      if (__NULL_PTR_CHECK(rdy_flg)) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số đầu vào

      assert_param(
        IS_RCC_SYSCLK_SOURCE(init_param->CLK_Source) || 
        IS_RCC_IWDG_SOURCE(init_param->CLK_Source)
      );
    
    // Làm mới biến cờ trạng thái
    
      memset(rdy_flg, 0, sizeof(RCC_RDYFLG_Typdef));


    // Bật nguồn clock tương ứng

    
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

          /**
           * Ghi chú: 
           * Trong các thiết kế trước,
           * IWDG được sử dụng như 1 giải pháp đảm bảo an toàn trong trường hợp HSI không thể sẵn sàng,
           * tuy nhiên, sau khi thực hiện HIL test thì
           * IWDG không cần khởi động vì hệ thống tự động chọn HWDG để đảm bảo an toàn cho toàn hệ thống
           */

          // Bật HSI

            SET_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_HSION_SET);
          
          // Chờ HSI sẵn sàng

            while (
              __DIFF_CHECK(
                READ_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_HSIRDY_ON), 
                RCC_CR_REG_HSIRDY_ON
              )
            ) {
              
              /**
               * Ghi chú:
               * Ở đây không cần làm gì
               */

              #ifdef UNIT_TEST
                
                /**
                 * Ghi chú:
                 * Ở đây khu vực này sẽ giả lập việc HSI không thể sẵn sàng thì sẽ dừng lại,
                 * Do trong unit sẽ có các hàm giả lập việc HSI sẵn sàng nên 
                 * trường hợp HSI sẵn sàng sẽ bỏ qua vòng lặp này
                 */

                for (int i = 10; i >= 0; i--) {
                  
                }
                break;
                
              #endif

            }

          // Kiểm tra HSI đã sẵn sàng chưa

            if (
              __DIFF_CHECK(
                READ_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_HSIRDY_ON), 
                RCC_CR_REG_HSIRDY_ON
              )
            ) {
              return STAT_ERROR; // HSI không thể sẵn sàng
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
          
          /**
           * Ghi chú: 
           * Trong các thiết kế trước,
           * IWDG được sử dụng như 1 giải pháp đảm bảo an toàn trong trường hợp HSE không thể sẵn sàng,
           * tuy nhiên, sau khi thực hiện HIL test thì
           * IWDG không cần khởi động vì hệ thống tự động chọn HWDG để đảm bảo an toàn cho toàn hệ thống
           */
          
          // Bật CSS trước khi bật HSE

            RCC_CSS_Enable();

          // Bật HSE

            SET_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_HSEON_SET);

          // Chờ HSE sẵn sàng

            while (
              __DIFF_CHECK(
                READ_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_HSERDY_ON), 
                RCC_CR_REG_HSERDY_ON
              )
            ) {

              /**
               * Ghi chú:
               * Ở đây không cần làm gì.
               */

              #ifdef UNIT_TEST
                
                /**
                 * Ghi chú:
                 * Ở đây khu vực này sẽ giả lập việc HSE không thể sẵn sàng thì sẽ dừng lại,
                 * Do trong unit sẽ có các hàm giả lập việc HSE sẵn sàng nên 
                 * trường hợp HSE sẵn sàng sẽ bỏ qua vòng lặp này
                 */

                for (int i = 10; i >= 0; i--) {

                }
                break;
                
              #endif
            }

          // Kiểm tra HSE sẵn sàng hay chưa

            if (
              __DIFF_CHECK(
                READ_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_HSERDY_ON), 
                RCC_CR_REG_HSERDY_ON
              )
            ) {
              return STAT_ERROR; // HSE không thể sẵn sàng
            }

            /**
             * Ghi chú:
             * Lúc này cờ trạng thái sẵn sàng của HSE sẽ được thiết lập
             * Thực hiện lưu cờ trạng thái vào biến trả về
             */

            // Lưu trạng thái sẵn sàng HSE
            rdy_flg->HSE_RDY_FLG = SET;

          // Chuyển SYSCLK sang HSE sau khi HSE sẵn sàng
            
            if (!__DONE_CHECK(RCC_SYSCLK_Switch(RCC_SYSCLK_SOURCE_HSE))) {
              return STAT_ERROR; // Chuyển đổi nguồn SYSCLK thất bại
            }

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
   * 
   * Phụ thuộc ngoài module Clock: Không có
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

      if (__NULL_PTR_CHECK(init_param)) {
        return STAT_ERROR;
      }
      if (__NULL_PTR_CHECK(rdy_flg)) {
        return STAT_ERROR;
      }

    // Kiểm tra giá trị tham số đầu vào

      assert_param(IS_RCC_SYSCLK_SOURCE(init_param->CLK_Source));

    // Làm mới biến cờ trạng thái
      
      memset(rdy_flg, 0, sizeof(RCC_RDYFLG_Typdef));

    // Tắt nguồn clock tương ứng

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
              if (!__OK_CHECK(RCC_CLK_Init(&rcc_hsi_init, &hsi_rdy_flg))) {
                return STAT_ERROR;
              }
            }

          // Chuyển SYSCLK về HSI

            if (!__DONE_CHECK(RCC_SYSCLK_Switch(RCC_SYSCLK_SOURCE_HSI))) {
              return STAT_ERROR;
            }

          
          // Tắt HSE

            CLEAR_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_HSEON_SET);
            break;

        default:

            return STAT_ERROR;

            break;
      }

      // Kết thúc quy trình deinit
     
        return STAT_DONE;
  }

  /*
   * Hàm bật Clock Security System (CSS) để bảo vệ khi HSE lỗi.
   * Không có tham số và không trả về giá trị.
   * Phụ thuộc ngoài module Clock: Không có
   */
  stinl void RCC_CSS_Enable(void) {
    SET_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_CSSON_SET);
  }

  /*
   * Hàm tắt Clock Security System (CSS).
   * Không có tham số và không trả về giá trị.
   * Phụ thuộc ngoài module Clock: Không có
   */
  stinl void RCC_CSS_Disable(void) {
    CLEAR_BIT(RCC_REGS_PTR->CR, RCC_CR_REG_CSSON_SET);
  }

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

  /*
   * Hàm kích hoạt clock cho các ngoại vi (peripheral clock enable).
   *
   * Tham số:
   *   periph - Mã định danh ngoại vi cần kích hoạt clock (AFIO, GPIOA-G).
   *
   * Logic:
   *   - Kiểm tra giá trị tham số periph hợp lệ.
   *   - Dựa vào periph, set bit tương ứng trong thanh ghi RCC_APB2ENR:
   *       + AFIO: Bit 0
   *       + GPIOA: Bit 2
   *       + GPIOB: Bit 3
   *       + GPIOC: Bit 4
   *       + GPIOD: Bit 5
   *       + GPIOE: Bit 6
   *       + GPIOF: Bit 7
   *       + GPIOG: Bit 8
   *   - Trả về STAT_OK sau khi kích hoạt thành công.
   *   - Trả về STAT_ERROR nếu periph không hợp lệ.
   *
   * Trả về:
   *   RETR_STAT - STAT_OK nếu kích hoạt thành công, STAT_ERROR nếu lỗi.
   *
   * Phụ thuộc ngoài module Clock:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - SET_BIT() - Macro set bit thanh ghi
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT RCC_PCLK_Enable(ul periph) {

    // Kiểm tra tham số đầu vào

      assert_param(IS_RCC_PERIPH(periph));

    // Bật clock cho ngoại vi tương ứng
    
      switch (periph) {

        case AFIO:
          SET_BIT(RCC_REGS_PTR->RCC_APB2ENR, AFIO);
          return STAT_OK;
          break;

        case GPIOA:
          SET_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOA);
          return STAT_OK;
          break;
        
        case GPIOB:
          SET_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOB);
          return STAT_OK;
          break;
        
        case GPIOC:
          SET_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOC);
          return STAT_OK;
          break;
        
        case GPIOD:
          SET_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOD);
          return STAT_OK;
          break;

        case GPIOE:
          SET_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOE);
          return STAT_OK;
          break;

        case GPIOF:
          SET_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOF);
          return STAT_OK;
          break;

        case GPIOG:
          SET_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOG);
          return STAT_OK;
          break;

        default:
          return STAT_ERROR;
          break;
      }

    // Kết thúc quy trình kích hoạt

      return STAT_DONE;
  }

  /*
   * Hàm vô hiệu hóa clock cho các ngoại vi (peripheral clock disable).
   *
   * Tham số:
   *   periph - Mã định danh ngoại vi cần vô hiệu hóa clock (AFIO, GPIOA-G).
   *
   * Logic:
   *   - Kiểm tra giá trị tham số periph hợp lệ.
   *   - Dựa vào periph, clear bit tương ứng trong thanh ghi RCC_APB2ENR:
   *       + AFIO: Bit 0
   *       + GPIOA: Bit 2
   *       + GPIOB: Bit 3
   *       + GPIOC: Bit 4
   *       + GPIOD: Bit 5
   *       + GPIOE: Bit 6
   *       + GPIOF: Bit 7
   *       + GPIOG: Bit 8
   *   - Trả về STAT_OK sau khi vô hiệu hóa thành công.
   *   - Trả về STAT_ERROR nếu periph không hợp lệ.
   *
   * Trả về:
   *   RETR_STAT - STAT_OK nếu vô hiệu hóa thành công, STAT_ERROR nếu lỗi.
   *
   * Phụ thuộc ngoài module Clock:
   *   - assert_param() - Macro kiểm tra điều kiện
   *   - CLEAR_BIT() - Macro clear bit thanh ghi
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  RETR_STAT RCC_PCLK_Disable(ul periph) {

    // Kiểm tra tham số đầu vào

      assert_param(IS_RCC_PERIPH(periph));

    // Khóa nguồn clock

      switch (periph) {
        case AFIO:
          CLEAR_BIT(RCC_REGS_PTR->RCC_APB2ENR, AFIO);
          return STAT_OK;
          break;

        case GPIOA:
          CLEAR_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOA);
          return STAT_OK;
          break;
        
        case GPIOB:
          CLEAR_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOB);
          return STAT_OK;
          break;
        
        case GPIOC:
          CLEAR_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOC);
          return STAT_OK;
          break;
        
        case GPIOD:
          CLEAR_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOD);
          return STAT_OK;
          break;

        case GPIOE:
          CLEAR_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOE);
          return STAT_OK;
          break;

        case GPIOF:
          CLEAR_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOF);
          return STAT_OK;
          break;

        case GPIOG:
          CLEAR_BIT(RCC_REGS_PTR->RCC_APB2ENR, GPIOG);
          return STAT_OK;
          break;

        default:
          return STAT_ERROR;
          break;
      }

    // Kết thúc quá trình

      return STAT_DONE;
  }
