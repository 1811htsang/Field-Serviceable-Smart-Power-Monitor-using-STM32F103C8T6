# Giới thiệu

GPIO (General Purpose Input/Output) là một thành phần quan trọng trong các vi điều khiển, cho phép giao tiếp với các thiết bị ngoại vi bằng cách cấu hình các chân (pin) của vi điều khiển thành các chân vào hoặc ra.

## Cấu hình GPIO

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

Kiểm chứng thông tin trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 159.

## Phân biệt giữa Push-Pull và Open-Drain

- Push-Pull: Chân có thể xuất tín hiệu cao hoặc thấp một cách trực tiếp.
- Open-Drain: Chân chỉ có thể kéo xuống mức thấp, cần một điện trở kéo lên (pull-up resistor) để đạt mức cao.

## Thiết kế phần cứng GPIO

Trong thiết kế phần cứng (Fig 13 trang 160), từ bộ output-control sẽ có 2 đầu nối, 1 nối P-MOS, 1 nối N-MOS.

### Chế độ Push-Pull

Xuất mức 1:

- Bật P-MOS, tắt N-MOS -> Chân ra mức cao (Vcc)
- Dòng điện chạy từ VDD qua tải đến chân ra.

Xuất mức 0:

- Tắt P-MOS, bật N-MOS -> Chân ra mức thấp (GND)
- Dòng điện chạy từ chân ra qua N-MOS đến GND.

### Chế độ Open-Drain

Xuất mức 1:

- Cả P-MOS và N-MOS đều tắt -> Chân ra ở trạng thái nổi (floating)
- Cần có điện trở kéo lên (pull-up resistor) để đạt mức cao.

Xuất mức 0:

- Tắt P-MOS, bật N-MOS -> Chân ra mức thấp (GND)
- Dòng điện chạy từ chân ra qua N-MOS đến GND.

### Nhận xét

PMOS đóng vai trò là công tắc đầu vào tích cực thấp - đầu ra tích cực cao, nghĩa là cho nguồn VDD cần kiểm soát, khi ngõ điều khiển C = 0 thì PMOS dẫn, khi C = 1 thì PMOS ngắt.

NMOS đóng vai trò là công tắc đầu vào tích cực cao - đầu ra tích cực thấp, nghĩa là cho nguồn GND cần kiểm soát, khi ngõ điều khiển C = 1 thì NMOS dẫn, khi C = 0 thì NMOS ngắt.

Xét trong thiết kế mạch, PMOS luôn ưu tiên nối vào VDD và NMOS luôn ưu tiên nối vào GND vì nếu ngược lại, khi NMOS ưu tiên nối vào VDD (giả sử 3.3V) thì ngõ điều khiển phải có điện áp lớn hơn 3.3V (ví dụ 5V) để NMOS dẫn, điều này không thực tế trong thiết kế mạch số sử dụng điện áp thấp hiện nay. Tương tự, nếu PMOS nối vào GND thì ngõ điều khiển phải có điện áp âm để PMOS dẫn, điều này cũng không thực tế.

## Đặc tính cổng sử dụng cần lưu ý

Sau khi reset, tất cả các chân GPIO đều ở chế độ Input floating (`CNFx[1:0] = 01, MODEx[1:0] = 00`).

Ngoài ra, các chân JTAG đều được cấu hình sẵn ở chế độ Input PU/PD như sau:

- PA15: JTDI (Input PU)
- PA14: JTCK (Input PD)
- PA13: JTMS (Input PU)
- PB4: NJTRST (Input PU)

Khi thiết kế cấu hình đầu ra, giá trị sẽ được ghi vào thanh ghi `GPIOx_ODR` để xác định mức logic ban đầu của chân, có thể sử dụng các chế độ Output Push-Pull hoặc Open-Drain tùy theo yêu cầu của ứng dụng.

Thanh ghi `GPIOx_IDR` chỉ đọc trạng thái dữ liệu ghi vào ở mỗi chu kỳ clock của APB2.

## Thao tác mức bit trên GPIO

Theo tài liệu RM0008 trang 161, có thể thao tác mức bit trên GPIO bằng cách sử dụng thanh ghi `GPIOx_BSRR` và `GPIOx_BRR` mà có thể bỏ qua vô hiệu hóa ngắt.

## Ngắt ngoại trên GPIO

Tất cả các chân đều có thể được sử dụng làm ngắt ngoại.

Tuy nhiên cần lưu ý rằng các cổng đều phải cấu hình ở chế độ input.

## Hoán đổi chức năng

Trong thiết kế chức năng, GPIO cho phép cấu hình thay đổi các cổng nhằm phục vụ đầu ra đầu vào của các ngoại vi khác nhau như UART, SPI, I2C, ADC, DAC, PWM, v.v.

**Ghi chú:** AF là Alternate Function.

Đối với các chức năng đầu vào, cấu hình chân ở chế độ AF Input và pin đầu vào phải được điều khiển bởi bên phía thiết bị ngoài.

Đối với các chức năng đầu ra, cấu hình chân ở chế độ AF Output và pin đầu ra được điều khiển bởi bộ ngoại vi bên trong vi điều khiển.

Với các chức năng 2 chiều, chân có thể được cấu hình ở chế độ AF Open-Drain hoặc Push-Pull tùy theo yêu cầu của giao thức truyền thông, chế độ Input sẽ cấu hình ở Floating.

Với các cổng được cấu hình là AF Output thì các thanh ghi lưu dữ liệu đầu ra sẽ bị vô hiệu hóa và connect tới tín hiệu đầu ra của bộ ngoại vi tương ứng trên chip.

Nếu phần mềm cấu hình chân là AF Output nhưng bộ ngoại vi không được kích hoạt, chân sẽ ở trạng thái không chỉ định (undefined state).

## Cơ chế khóa cổng

Cơ chế khóa cổng (GPIO Lock) được sử dụng để ngăn chặn việc thay đổi cấu hình của các chân GPIO sau khi đã thiết lập. Điều này đặc biệt hữu ích trong các ứng dụng yêu cầu độ tin cậy cao, nơi mà cấu hình GPIO không nên bị thay đổi do lỗi phần mềm hoặc các sự kiện không mong muốn.

Kiểm chứng thông tin trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 162.

## Cấu hình GPIO cho từng ngoại vi

Kiểm chứng trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) từ trang 166 đến trang 170 để biết cấu hình GPIO cho từng ngoại vi cụ thể như USART, SPI, I2C, TIM, ADC, DAC, CAN, USB, v.v.

Tập trung vào phần SPI, I2C ở trang 181.
