/*
 * source_dependency.c
 *
 *  Created on: Mar 5, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include "lib_keyword_def.h"
  #include "header_dependency.h"

// Định nghĩa các hàm mock tương ứng

  /**
   * Ghi chú:
   * Do module NVIC không phụ thuộc vào các khai báo hoặc sử dụng hàm từ module khác nên
   * ở đây ta không cần định nghĩa hàm mock nào cả, 
   * chỉ cần đảm bảo rằng các hàm thành phần của module NVIC đã được định nghĩa đầy đủ trong lib_nvic_impl.c
   */
