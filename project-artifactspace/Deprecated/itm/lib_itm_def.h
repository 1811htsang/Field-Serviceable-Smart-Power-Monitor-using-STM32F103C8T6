/*
 * lib_itm_def.h
 *
 *  Created on: Mar 7, 2026
 *      Author: shanghuang
 */

#ifndef LIB_ITM_DEF_H_
  #define LIB_ITM_DEF_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdio.h>
    #include <stdint.h>
    #include "generic/lib_keyword_def.h"

  // Khai báo địa chỉ ngoại vi

    #define ITM_REGS_BASEADDR 0xE0000000ul

  // Khai báo cấu trúc thanh ghi

    tdf_strc ITM_REGS {
      __vo BLANK_REG_32B ITM_STIM[32];   // Tại offset 0x000 đến 0x7C
      __vo BLANK_REG_32B RESERVED0[864];
      __vo BLANK_REG_32B ITM_TER;        // Tại offset 0xE00
      __vo BLANK_REG_32B RESERVED1[15];  
      __vo BLANK_REG_32B ITM_TPR;        // Tại offset 0xE40
      __vo BLANK_REG_32B RESERVED2[15];
      __vo BLANK_REG_32B ITM_TCR;        // Tại offset 0xE80
      __vo BLANK_REG_32B RESERVED3[29];
      __vo BLANK_REG_32B ITM_IWR;        // Tại offset 0xEF8
      __vo BLANK_REG_32B ITM_IRR;        // Tại offset 0xEFC
      __vo BLANK_REG_32B ITM_IMCR;       // Tại offset 0xF00
      __vo BLANK_REG_32B RESERVED4[43];
      __vo BLANK_REG_32B ITM_LAR;        // Tại offset 0xFB0
      __vo BLANK_REG_32B ITM_LSR;        // Tại offset 0xFB4
      __vo BLANK_REG_32B RESERVED5[6];
      __vo BLANK_REG_32B ITM_PID4;       // Tại offset 0xFD0
      __vo BLANK_REG_32B ITM_PID5;       // Tại offset 0xFD4
      __vo BLANK_REG_32B ITM_PID6;       // Tại offset 0xFD8
      __vo BLANK_REG_32B ITM_PID7;       // Tại offset 0xFDC
      __vo BLANK_REG_32B ITM_PID0;       // Tại offset 0xFE0
      __vo BLANK_REG_32B ITM_PID1;       // Tại offset 0xFE4
      __vo BLANK_REG_32B ITM_PID2;       // Tại offset 0xFE8
      __vo BLANK_REG_32B ITM_PID3;       // Tại offset 0xFEC
      __vo BLANK_REG_32B ITM_CID0;       // Tại offset 0xFF0
      __vo BLANK_REG_32B ITM_CID1;       // Tại offset 0xFF4
      __vo BLANK_REG_32B ITM_CID2;       // Tại offset 0xFF8
      __vo BLANK_REG_32B ITM_CID3;       // Tại offset 0xFFC

      /**
       * Ghi chú:
       * - Đối với 2 thanh ghi LAR và LSR thuộc quyền hạn quản lý trong tài liệu 
       * Arm® CoreSight™ Architecture Specification dùng để kiểm soát quyền truy cập vào các thanh ghi khác của ITM, 
       * nên chúng được chung ở cấu trúc để dễ dàng quản lý và sử dụng trong các hàm liên quan đến quyền truy cập.
       * - Các thanh ghi IWR, IRR VÀ ICMR là các thanh ghi cấp cao nằm trong quyền hạn quản lý
       * của các nhà sản xuất chip nên không cần đụng đến trong quá trình sử dụng.
       */

    } ITM_REGS_Typedef;

  // >> Tạo con trỏ phần cứng thật

    #define ITM_REGS_PTR ((ITM_REGS_Typedef *) ITM_REGS_BASEADDR)

  // Khai báo các hàm điều khiển ITM sẽ được triển khai trong lib_itm_impl.c

    void ITM_Init(void);
    void ITM_SendChar(ui32 ch);
    void ITM_SendString(const char *str);

#endif /* LIB_ITM_DEF_H_ */
