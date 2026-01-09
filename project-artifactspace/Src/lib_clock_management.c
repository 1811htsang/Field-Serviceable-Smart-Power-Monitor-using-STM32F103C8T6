/*
 * lib_clock_management.c
 *
 *  Created on: Jan 9, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung
#include <stdint.h>
#include <stdio.h>
#include "lib_keyword_define.h"
#include "lib_clock_management.h"

RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param *init_param) {
  
  if (DEBUG_MODE == ENABLE) {
    printf("RCC_CLK_Init, DBG1: Check Null pointer.\n");
  }

  if (init_param == NULL) {
    return STAT_ERROR;
  }

  if (DEBUG_MODE == ENABLE) {
    printf("RCC_CLK_Init, DBG2: Assert parameter.\n");
  }

  assert_param(IS_RCC_SYSCLK_SOURCE(init_param->CLK_Source));

  if (DEBUG_MODE == ENABLE) {
    printf("RCC_CLK_Init, DBG3: Setup procedure starts.\n");
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
      
      while (RCC_REGS_PTR->CR.HSIRDY == RESET) {
        // Chờ HSI sẵn sàng

        /**
         * Ghi chú:
         * Chỗ này sẽ có bổ sung hoạt động của IWDG
         * nhằm đảm bảo hệ thống không bị treo khi chờ HSi ổn định
         */
      }

      return STAT_OK;

      break;

    case 0x01ul: // HSE
      
      /**
       * Ghi chú:
       * Bật CSS trước để đảm bảo hệ thống được bảo vệ ngay 
       * khi HSE được kích hoạt
       */
      RCC_REGS_PTR->CR.CSSON = SET;

      RCC_REGS_PTR->CR.HSEON = SET; // Bật HSE

      while (RCC_REGS_PTR->CR.HSERDY == RESET) {
        // Chờ HSE sẵn sàng

        /**
         * Ghi chú:
         * Chỗ này sẽ có bổ sung hoạt động của IWDG
         * nhằm đảm bảo hệ thống không bị treo khi chờ HSE ổn định
         */
      }

      RCC_REGS_PTR->CFGR.SW = 0x01ul; // Chọn HSE làm nguồn SYSCLK

      if (RCC_REGS_PTR->CFGR.SWS != 0x01ul) {
        return STAT_ERROR; // Chuyển đổi nguồn SYSCLK thất bại
      }

      return STAT_OK;

      break;

    case 0x0Ful: // LSI
    
      RCC_REGS_PTR->CSR.LSION = SET; // Bật LSI  

      // Chờ LSI sẵn sàng
      while (RCC_REGS_PTR->CSR.LSIRDY == RESET) {}

      return STAT_OK;

      break;
    
    default:

      return STAT_ERROR;
      
      break;
  }

  /**
   * Ghi chú:
   * Ở đây sử dụng STAT_OK để biểu thị hàm thực thi thành công
   * Còn kết quả cụ thể của việc khởi tạo nguồn clock
   * sẽ được kiểm tra thông qua các flag trạng thái tương ứng
   * của switch bên trên.
   */

  return STAT_OK;
}

RETR_STAT RCC_CLK_DeInit(RCC_CLK_Init_Param *init_param) {

  /**
   * Ghi chú:
   * Ở đây sử dụng STAT_OK để biểu thị hàm thực thi thành công
   * Còn kết quả cụ thể của việc khởi tạo nguồn clock
   * sẽ được kiểm tra thông qua các flag trạng thái tương ứng
   * của switch bên trên.
   */

  return STAT_OK;
}

void RCC_CSS_Enable(void) {

}

void RCC_CSS_Disable(void) {

}

void RCC_NMI_IRQ_Handler(void) {

}

void RCC_GNR_IRQ_Handler(void) {

}

__weak void RCC_CSS_Callback(void) {

}
