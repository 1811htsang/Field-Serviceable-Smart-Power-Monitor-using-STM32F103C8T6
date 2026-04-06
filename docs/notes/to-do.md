# Tổng quan

Lộ trình triển khai chi tiết, đi từ

  1. **Nền tảng** (Drivers)
  2. **Cốt lõi** (Bootloader Logic)
  3. **Giao thức** (Protocol)
  4. **Công cụ** (Host Tool)

Có thể cân nhắc bổ sung việc triển khai Unity Test Framework để viết Unit Test cho các Driver thuận tiện hơn.

## Giai đoạn Nền tảng

- [x] Thiết kế khung dự án (Project Structure)
- [x] Bổ sung các tài liệu căn bản của dự án
- [x] Tìm hiểu đầy đủ về Clock System
- [x] Tìm hiểu về reset và khởi động STM32
- [x] Tìm hiểu về Watchdog Timer
- [x] Tìm hiểu về Unit Testing trong Embedded
- [x] Viết Header & Source Inclusion cho GCC Compiler
- [x] Tìm hiểu sử dụng GDB cho Unit Test
- [x] Hoàn thiện ghi chú về sử dụng GDB cho Unit Test
- [x] Hoàn thiện ghi chú về thiết kế mã nguồn hỗ trợ Unit Test
- [x] Viết Driver Clock (Clock Control)
- [x] Viết Driver IWDG (Independent Watchdog)
- [x] Viết Driver Reset (Reset Control)
- [x] Viết Unit Test cho Driver IWDG
- [x] Viết Unit Test cho Driver Clock
- [x] Viết Unit Test cho Driver Reset
- [x] Tìm hiểu về GPIO và AFIO
- [x] Viết Driver GPIO
- [x] Viết Unit Test cho Driver GPIO
- [x] Viết Driver AFIO
- [x] Tìm hiểu về NVIC và EXTI
- [x] Viết Driver EXTI
- [x] Viết Unit Test cho Driver EXTI
- [x] Viết Driver NVIC
- [x] Viết Unit Test cho Driver NVIC
- [x] Triển khai các driver lên mạch thực tế để kiểm tra hoạt động
- [x] Viết Driver SysTick cho khả năng tạo delay và quản lý thời gian với SPI
- [ ] Viết Driver SPI cho khả năng giao tiếp với ADE7758
- [ ] Viết Unit Test cho Driver SPI
- [ ] Viết Driver I2C cho khả năng giao tiếp với LCD 16x2, MCP4728
- [ ] Viết Unit Test cho Driver I2C
- [ ] Viết Driver UART cho khả năng giao tiếp với máy tính qua cổng UART
- [ ] Viết Unit Test cho Driver UART
- [ ] Triển khai Driver lên mạch thực tế

## Giai đoạn Cốt lõi

- [ ] Phân vùng bộ nhớ
- [ ] Viết Driver Flash Internal
- [ ] Viết hàm Jump to Application

## Giai đoạn Giao thức

- [ ] Triển khai giao thức XMODEM
- [ ] Triển khai XMODEM Receive
- [ ] Xử lý Timeout

## Giai đoạn Công cụ

- [ ] Viết Python script gửi file qua XMODEM
- [ ] So sánh kích thước

## Danh sách Commit chủ chốt

- [ ] feat(rcc): setup system clock to 72MHz bare-metal
- [ ] feat(uart): implement polling uart driver
- [ ] feat(flash): implement internal flash erase and write
- [ ] feat(boot): implement jump to application logic
- [ ] feat(xmodem): integrate xmodem protocol for firmware reception
- [ ] chore(linker): update linker scripts for bootloader memory map
- [ ] tool(python): add script to flash firmware via uart
