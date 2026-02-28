/*
 * lib_test_util.h
 *
 *  Created on: Feb 28, 2026
 *      Author: shanghuang
 */

#ifndef LIB_TEST_UTIL_H_
  #define LIB_TEST_UTIL_H_

  // Khai báo thư viện

    #include <stdio.h>
    #include <setjmp.h>
    #include "lib_keyword_def.h"

  // Khai báo ủy quyền các biến dùng chung cho việc bắt assert trong unit test

    extern jmp_buf assert_env;
    extern ui8 assert_caught;

  // Khai báo các hàm dùng chung cho việc bắt assert trong unit test

    #define ASSERT_EXPECT_FAIL(code_to_test) \
      do { \
          assert_caught = 1; \
          if (setjmp(assert_env) == 0) { \
              code_to_test; \
              /* Nếu chạy đến đây mà không nhảy vào longjmp nghĩa là thoát lỗi */ \
              printf("-> ASSERT FAILED (Expected assert failure)\n"); \
          } else { \
              /* Nhảy vào đây nghĩa là Assert đã hoạt động đúng */ \
              printf("-> ASSERT PASSED (Expected assert failure)\n"); \
          } \
          assert_caught = 0; \
      } while(0)


#endif /* LIB_TEST_UTIL_H_ */
