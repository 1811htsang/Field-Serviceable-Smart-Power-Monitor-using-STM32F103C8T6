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
  	#include "generic/lib_condition_def.h";
    #include "nvic/lib_nvic_def.h";
    #include "nvic/lib_nvic_hal.h";
  #endif

// Định nghĩa các hàm thành phần

  void NVIC_INTR_Activation_Enable(NVIC_INTR_Param *intr_param) {

    /**
     * Ghi chú:
     * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
     * nên ở đây ta có thể yên tâm rằng việc set bit trong ISER sẽ không gây ra lỗi.
     */

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("NVIC_INTR_Activation_Enable, DBG1: Enabling NVIC interrupt for IRQ_POS %u.\n", intr_param->Position);
    }

      if (intr_param->Position < 32) {
        NVIC_REGS_PTR->NVIC_ISER[0] |= (0x0001u << intr_param->Position); // Set bit tương ứng trong ISER để kích hoạt ngắt
      } else if (intr_param->Position < 64) {
        NVIC_REGS_PTR->NVIC_ISER[1] |= (0x0001u << (intr_param->Position - 32)); // Set bit tương ứng trong ISER để kích hoạt ngắt
      } else {
        NVIC_REGS_PTR->NVIC_ISER[2] |= (0x0001u << (intr_param->Position - 64)); // Set bit tương ứng trong ISER để kích hoạt ngắt
      }

      return;
  }

  void NVIC_INTR_Activation_Disable(NVIC_INTR_Param *intr_param) {

    /**
     * Ghi chú:
     * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
     * nên ở đây ta có thể yên tâm rằng việc set bit trong ICER sẽ không gây ra lỗi.
     */

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("NVIC_INTR_Activation_Disable, DBG1: Disabling NVIC interrupt for IRQ_POS %u.\n", intr_param->Position);
    }

      if (intr_param->Position < 32) {
        NVIC_REGS_PTR->NVIC_ICER[0] |= (0x0001u << intr_param->Position); // Set bit tương ứng trong ICER để vô hiệu hóa ngắt
      } else if (intr_param->Position < 64) {
        NVIC_REGS_PTR->NVIC_ICER[1] |= (0x0001u << (intr_param->Position - 32)); // Set bit tương ứng trong ICER để vô hiệu hóa ngắt
      } else {
        NVIC_REGS_PTR->NVIC_ICER[2] |= (0x0001u << (intr_param->Position - 64)); // Set bit tương ứng trong ICER để vô hiệu hóa ngắt
      }

      return;
  }

  void NVIC_INTR_Pending_Enable(NVIC_INTR_Param *intr_param) {
    
    /**
     * Ghi chú:
     * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
     * nên ở đây ta có thể yên tâm rằng việc set bit trong ISPR sẽ không gây ra lỗi.
     */

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("NVIC_INTR_Pending_Enable, DBG1: Setting pending bit for NVIC interrupt with IRQ_POS %u.\n", intr_param->Position);
    }
    
      if (intr_param->Position < 32) {
        NVIC_REGS_PTR->NVIC_ISPR[0] |= (0x0001u << intr_param->Position); // Set bit tương ứng trong ISPR để kích hoạt pending bit
      } else if (intr_param->Position < 64) {
        NVIC_REGS_PTR->NVIC_ISPR[1] |= (0x0001u << (intr_param->Position - 32)); // Set bit tương ứng trong ISPR để kích hoạt pending bit
      } else {
        NVIC_REGS_PTR->NVIC_ISPR[2] |= (0x0001u << (intr_param->Position - 64)); // Set bit tương ứng trong ISPR để kích hoạt pending bit
      }

      return;
  }

  void NVIC_INTR_Pending_Disable(NVIC_INTR_Param *intr_param) {

    /**
     * Ghi chú:
     * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
     * nên ở đây ta có thể yên tâm rằng việc set bit trong ICPR sẽ không gây ra lỗi.
     */

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("NVIC_INTR_Pending_Disable, DBG1: Clearing pending bit for NVIC interrupt with IRQ_POS %u.\n", intr_param->Position);
    }

      if (intr_param->Position < 32) {
        NVIC_REGS_PTR->NVIC_ICPR[0] |= (0x0001u << intr_param->Position); // Set bit tương ứng trong ICPR để clear pending bit
      } else if (intr_param->Position < 64) {
        NVIC_REGS_PTR->NVIC_ICPR[1] |= (0x0001u << (intr_param->Position - 32)); // Set bit tương ứng trong ICPR để clear pending bit
      } else {
        NVIC_REGS_PTR->NVIC_ICPR[2] |= (0x0001u << (intr_param->Position - 64)); // Set bit tương ứng trong ICPR để clear pending bit
      }

      return;
  }

  void NVIC_INTR_Priority_Config(NVIC_INTR_Param *intr_param) {

    /**
     * Ghi chú:
     * Do ở hàm cấu hình tổng quát đã có kiểm tra tham số đầu vào và đã đảm bảo rằng pending bit đang ở trạng thái disable,
     * nên ở đây ta có thể yên tâm rằng việc set bit trong IPR sẽ không gây ra lỗi.
     */

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("NVIC_INTR_Priority_Config, DBG1: Configuring priority for IRQ_POS %u with priority %u.\n", intr_param->Position, intr_param->Priority);
    }

      NVIC_REGS_PTR->NVIC_IPRE[intr_param->Position] = (intr_param->Priority & 0x0Fu) << 4; 
      
      /**
       * Ghi chú:
       * Set 4 bit cao của byte tương ứng trong IPR để cấu hình mức ưu tiên ngắt, 
       * lưu ý rằng trong thiết kế tài liệu mức ưu tiên chỉ sử dụng loại preempt 
       * và bỏ qua sub-preempt nên ta sẽ không sử dụng 4 bit thấp của byte trong IPR.
       * Thực hiện and với mask 0x0F để đảm bảo rằng chỉ có 4 bit thấp của intr_param->Priority được sử dụng, tránh việc giá trị priority vượt quá 15u
       */

      return;
    
  }

  RETR_STAT NVIC_INTR_Config(NVIC_INTR_Param *intr_param) {

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("NVIC_INTR_Config, DBG1: Check NULL input.\n");
    }

      if (intr_param == NULL) {
        if (__DEBUG_GET_MODE(ENABLE)) {
          printf("NVIC_INTR_Config, ERR: Null pointer detected.\n");
        }
        return STAT_ERROR;
      }
    
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("NVIC_INTR_Config, DBG2: Assert parameter.\n");
    }

      assert_param(IS_NVIC_IRQ_POS(intr_param->Position));
      assert_param(IS_NVIC_INTR_PRIORITY(intr_param->Priority));
      assert_param(IS_NVIC_INTR_STATUS(intr_param->Status));

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf(
        "NVIC_INTR_Config, DBG3: Configuring NVIC interrupt for IRQ_POS %u with priority %u and status %u.\n", 
        intr_param->Position, 
        intr_param->Priority, 
        intr_param->Status
      );
    }

      NVIC_INTR_Pending_Disable(intr_param); // Trước khi config thì phải đảm bảo pending bit đang ở trạng thái disable để tránh việc ngắt được kích hoạt ngay sau khi config nếu có pending bit cũ chưa được clear

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

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("NVIC_INTR_Config, DBG4: Configure priority for IRQ_POS %u with priority %u.\n", intr_param->Position, intr_param->Priority);
    }

      NVIC_INTR_Priority_Config(intr_param);

      return STAT_DONE;
  }

  ui32 NVIC_INTR_GetActivation(IRQ_POS_Enum_Type position) {
    
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

  ui32 NVIC_INTR_GetPending(IRQ_POS_Enum_Type position) {
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
