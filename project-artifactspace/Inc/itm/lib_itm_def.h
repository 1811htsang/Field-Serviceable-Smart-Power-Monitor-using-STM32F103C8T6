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
      __vo BLANK_REG ITM_STIM[32];   // Tại offset 0x000 đến 0x7C, mỗi thanh ghi có kích thước 4 byte
      __vo BLANK_REG RESERVED0[864];
      __vo BLANK_REG ITM_TER;        // Tại offset 0xE00
      __vo BLANK_REG RESERVED1[15];  
      __vo BLANK_REG ITM_TPR;        // Tại offset 0xE40
      __vo BLANK_REG RESERVED2[15];
      __vo BLANK_REG ITM_TCR;        // Tại offset 0xE80
      

    } ITM_REGS_Typedef;


#endif /* LIB_ITM_DEF_H_ */
