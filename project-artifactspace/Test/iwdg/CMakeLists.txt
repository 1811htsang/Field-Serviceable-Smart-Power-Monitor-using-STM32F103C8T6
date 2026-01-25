# Cấu trúc các tệp nguồn và thư viện như sau:
# - unit_test_implementation.c: Tệp nguồn chính cho các bài kiểm tra đơn vị
# - source_dependency.c: Tệp nguồn cho thư viện phụ thuộc
# - header_dependency.h: Tệp tiêu đề cho thư viện phụ thuộc
# - ..\..\Inc\generic\lib_keyword_def.h: Tệp tiêu đề chung cho định nghĩa từ khóa
# - ..\..\Inc\iwdg\lib_iwdg_def.h: Tệp tiêu đề chung cho định nghĩa iwdg
# - ..\..\Inc\iwdg\lib_iwdg_hal.h: Tệp tiêu đề chung cho giao diện phần cứng iwdg
# - ..\..\Impl\iwdg\lib_iwdg_impl.c: Tệp nguồn cho triển khai giao diện phần cứng iwdg
# - ..\..\Inc\clock\lib_clock_def.h: Tệp tiêu đề chung cho định nghĩa clock
# - ..\..\Inc\clock\lib_clock_hal.h: Tệp tiêu đề chung cho giao diện phần cứng clock
# - ..\..\Impl\clock\lib_clock_impl.c: Tệp nguồn cho triển khai giao diện phần cứng clock

# Cấu trúc lệnh GCC ban đầu:
# gcc 
# -DUNIT_TEST 
# -I ..\..\Inc\generic\ 
# -I ..\..\Inc\iwdg\ 
# -I ..\..\Inc\clock\ 
# -I ..\iwdg\ 
# -g -o .\unit_test_implementation 
# -o .\unit_test_implementation.exe 
#    .\unit_test_implementation.c 
#    .\source_dependency.c 
#    ..\..\Impl\lib_iwdg_impl.c

# Khai báo phiên bản CMake tối thiểu
cmake_minimum_required(VERSION 4.2.2) #Yêu cầu phiên bản CMake tối thiểu

# Khai báo tên dự án và ngôn ngữ lập trình
project(iwdg_unit_test C) #Đặt tên dự án và ngôn ngữ lập trình

# Định nghĩa các file nguồn .c sử dụng trong dự án
set(SOURCES
    unit_test_implementation.c
    source_dependency.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Impl/lib_iwdg_impl.c
)

# Cấu hình để luôn tạo ra file Debug (tương đương flag -g)
set(CMAKE_BUILD_TYPE Debug)

# Tạo mục tiêu thực thi
add_executable(${PROJECT_NAME} ${SOURCES}) #Chỉ định tệp nguồn để biên dịch

# Định nghĩa các Macro (tương đương flag -D)
target_compile_definitions(${PROJECT_NAME} PRIVATE UNIT_TEST)

# Cấu hình các đường dẫn Include (tương đương các tham số -I)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Inc/generic
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Inc/iwdg
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Inc/clock
  ${CMAKE_CURRENT_SOURCE_DIR}
)


