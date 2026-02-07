# Giới thiệu
GPIO (General Purpose Input/Output) là một thành phần quan trọng trong các vi điều khiển, cho phép giao tiếp với các thiết bị ngoại vi bằng cách cấu hình các chân (pin) của vi điều khiển thành các chân vào hoặc ra.

# Cấu hình GPIO
- Input floating
- Input pull-up/down
- Analog
- Output push-pull/open-drain
- Alternate function push-pull/open-drain

Mỗi cổng đều có thể được cấu hình độc lập cho từng chân nhưng tất cả đều phải sử dụng ở 32 bit - 1 word. Tuy nhiên, có thể sử dụng `GPIOx_BSRR` và `GPIOx_BRR` để thao tác với đọc/ghi từng cấp độ bit nhằm tránh xử lý ngắt xảy ra khi thao tác với các chân khác.

Việc thao tác chân này được cấu hình thông qua các thanh ghi:
- `GPIOx_CRL` (Control Register Low): Cấu hình các chân từ 0 đến 7.
- `GPIOx_CRH` (Control Register High): Cấu hình các chân từ 8 đến 15.
- `GPIOx_IDR` (Input Data Register): Đọc trạng thái của các chân vào.
- `GPIOx_ODR` (Output Data Register): Ghi trạng thái của các chân ra.

