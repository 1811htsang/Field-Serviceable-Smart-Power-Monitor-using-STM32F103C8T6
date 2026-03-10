# Khai báo các tệp sử dụng trong dự án
# gcc 
# -DUNIT_TEST 
# -I ../../Inc/generic/ 
# -I ../../Inc/clock/ 
# -I ./ 
# -g -o ./unit_test_implementation 
# -o ./unit_test_implementation.exe 
#    ./unit_test_implementation.c 
#    ./source_dependency.c 
#    ../../Impl/lib_reset_impl.c

cmake_minimum_required(VERSION 3.10)

project(reset_unit_test C)

# Định nghĩa các file nguồn .c sử dụng trong dự án
set(SOURCES
  unit_test_implementation.c
  source_dependency.c
  ../../Impl/lib_reset_impl.c
)

# Cấu hình để luôn tạo ra file Debug (tương đương flag -g)
set(CMAKE_BUILD_TYPE Debug)

# Tạo mục tiêu thực thi
add_executable(${PROJECT_NAME} ${SOURCES})

# Định nghĩa các Macro (tương đương flag -D)
target_compile_definitions(${PROJECT_NAME} PRIVATE UNIT_TEST)

# Cấu hình các đường dẫn Include (tương đương các tham số -I)
target_include_directories(${PROJECT_NAME} PRIVATE
  ../../Inc/generic/
  ../../Inc/reset/
  ../../Inc/clock/
  ./
)


