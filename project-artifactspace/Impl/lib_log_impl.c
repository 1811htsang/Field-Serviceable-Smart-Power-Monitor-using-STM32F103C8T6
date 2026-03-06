/*
 * lib_log_impl.c
 *
 *  Created on: Mar 6, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng

  #ifdef UNIT_TEST
    #include "lib_log_def.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
  #include <stdarg.h>

  #ifndef UNIT_TEST
    #include "log/lib_log_def.h"
  #endif

// Khai báo từ khóa

  static const char level_chars[] = {'N', 'E', 'W', 'I', 'D', 'V'};

// Định nghĩa các hàm thành phần

  void LOG_Write(LOG_Enum level, const char *tag, const char *format, ...) {

    // 1. Kiểm tra mức log trước khi xử lý để tiết kiệm thời gian CPU
    if (level > LOG_GLOBAL_LEVEL || level == LOG_LEVEL_NONE) {
        return;
    }

    // 2. In phần Header (mức log, tag, content) (không dùng màu)
    printf("%c [%s]: ", level_chars[level], tag);

    // 3. Xử lý nội dung chính bằng vprintf
    va_list args;
    va_start(args, format); // Bắt đầu lấy danh sách tham số sau biến format
    vprintf(format, args);  // Hàm tiêu chuẩn chuyển đổi format + va_list thành chuỗi in ra
    va_end(args);           // Giải phóng danh sách tham số

    // 4. Kết thúc dòng log bằng ký tự xuống dòng
    printf("\n");
  }