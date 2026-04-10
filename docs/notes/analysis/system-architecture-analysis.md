# System Architecture Analysis

Ghi chú này dựa trên các tài liệu tổng hợp ý tưởng thiết kế của [sys-des](/docs/notes/system-design-note.md) và các phân tích yêu cầu rõ ràng của [req-ana](requirement-analysis.md) nhằm trình bày thống nhất các phân tích về kiến trúc hệ thống mà dự án đặt ra để đảm bảo hoạt động ổn định và hiệu quả của hệ thống.

## Required Components

Dựa trên các yêu cầu đã được phân tích, hệ thống sẽ cần phải bao gồm các thành phần chính sau:

- MCU STM32F103C8T6 để quản lý luồng dữ liệu, tính toán logic, quản lý giao thức XMODEM và điều khiển FSM toàn cục.
- IC ADE7758 để đo lường chính xác các thông số điện năng như công suất, điện áp, dòng điện và tần số.
- IC ZMCT103C để bảo vệ và cách ly mạch đo lường khỏi nguồn điện áp cao, đảm bảo an toàn cho hệ thống.
- Optocoupler 4N35SM để cách ly tín hiệu giữa mạch đo lường và MCU, đảm bảo an toàn và giảm nhiễu.
- Mạch ổn áp L7805CV để cung cấp nguồn 5V ổn định cho các linh kiện điện tử trong hệ thống.
- Transitor BC547 để điều khiển dòng điện trong mạch điện tử, hỗ trợ việc đóng cắt nguồn điện và điều khiển các thành phần khác.
- MCP4728 để chuyển đổi tín hiệu điện 1 pha thành 3 pha, giúp hệ thống có khả năng giám sát năng lượng trên lưới điện công nghiệp giả lập.
- TDA2030 để khuếch đại tín hiệu điện 3 pha thành tín hiệu điện 3 pha có công suất lớn hơn, giúp hệ thống có khả năng giám sát năng lượng trên lưới điện công nghiệp giả lập.
- LCD 1602 để hiển thị thông tin đo lường điện năng và trạng thái của hệ thống.
- Module relay để điều khiển tải điện từ vi điều khiển, mô phỏng việc đóng cắt nguồn điện và kiểm tra hoạt động của hệ thống trong các tình huống khác nhau.

## Clock Tree Design

Để đảm bảo hoạt động ổn định và hiệu quả của hệ thống, thiết kế clock tree sẽ được thực hiện như sau:

- Nguồn xung chính - SYSCLK dùng thạch anh ngoài HSE 8Mhz để đảm bảo độ chính xác và ổn định cho hoạt động của MCU.
- Từ nguồn xung chính, sẽ sử dụng PLL để nhân lên tần số lên mức tối đa 72Mhz, đảm bảo hiệu suất xử lý cao cho các tác vụ tính toán và quản lý dữ liệu.
- AHB bus sẽ chạy ở tốc độ tối đa 72Mhz để đảm bảo tốc độ truyền dữ liệu nhanh giữa các thành phần trong hệ thống.
- APB1 bus sẽ chạy ở tốc độ tối đa 36Mhz để đảm bảo hiệu suất cho các thiết bị ngoại vi như ADC, UART, I2C và SPI.
- APB2 bus sẽ chạy ở tốc độ tối đa 72Mhz để đảm bảo hiệu suất cho các thiết bị ngoại vi như GPIO, USART và ADC.
- LSI clock để dành riêng cho IWDG nhằm đảm bảo hoạt động ổn định của watchdog timer, giúp hệ thống tự động khởi động lại trong trường hợp gặp sự cố hoặc treo máy.
- CSS clock để giám sát nguồn xung chính, đảm bảo hệ thống có thể phát hiện và xử lý các lỗi liên quan đến nguồn xung một cách hiệu quả.
