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
- [ ] Viết Driver GPIO
- [ ] Viết Driver SPI
- [ ] Viết Driver I2C
- [ ] Viết Driver USART (Polling/Interrupt Mode)
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
