Các src CLK chính sử dụng trong RCC:
- HSI
- LSI
- HSE
- LSE

Trong đa số trường hợp sử dụng HSI/HSE làm nguồn CLK chính cho hệ thống.

# CLK tree
Kiểm tra trong trng:
- [prod-stm32](/docs/references/production-stm32-f103x8-f103xb-datasheet.pdf) trang 12
- [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 93

# HSI
Kiểm tra trong các nguồn:
- [prod-stm32](/docs/references/production-stm32-f103x8-f103xb-datasheet.pdf) trang 54
- [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 95

Nguồn này được tạo ra từ bộ oscillator 8MHz nội bộ, có thể sử dụng như nguồn CLK chính cho hệ thống hoặc làm nguồn CLK cho PLL.

Không cần dùng clock ngoài, thời gian khởi động nhanh nhưng độ chính xác thấp kể cả có hiệu chỉnh.

Nguồn này có thể làm nguồn dự phòng trong trường hợp HSE bị lỗi
-> Ưu tiên sử dụng HSE làm nguồn CLK chính cho hệ thống, nếu HSE lỗi thì chuyển sang HSI.

NSX đã hiệu chỉnh sẵn tần số HSI trong nhà máy, sai số tối đa ±1% ở nhiệt độ 25°C. Giá trị hiệu chỉnh nằm trong thanh ghi RCC_CR->HSITRIM[7:0].

Nguyên tắc sử dụng bộ HSI:
- Bật HSI: đặt HSION bit trong RCC_CR (RCC_CR->HSION = 1)
- Chờ HSI sẵn sàng: kiểm tra HSIRDY bit trong RCC_CR (RCC_CR->HSIRDY == 1)

# HSE
Kiểm tra trong các nguồn:
- [prod-stm32](/docs/references/production-stm32-f103x8-f103xb-datasheet.pdf) trang 52

Nguồn này sử dụng bộ thạch anh ngoài cho tần số từ 4MHz đến 16MHz, cho ra tần số chính xác cao hơn HSI.

Nguyên tắc sử dụng bộ HSE:
- Bật HSE: đặt HSEON bit trong RCC_CR (RCC_CR->HSEON = 1)
- Chờ HSE sẵn sàng: kiểm tra HSERDY bit trong RCC_CR (RCC_CR->HSERDY == 1)
  - Ngắt có thể tạo ra nếu enable trong RCC_CIR 


# SYSCLK select
Kiểm tra trong nguồn [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 97

Nguyên tắc lựa chọn nguồn SYSCLK:
- Sau system reset, HSI được chọn làm nguồn SYSCLK mặc định. 
- Muốn chuyển qua nguồn khác thì chỉ có thể khi nguồn được target đó stable sau startup delay hoặc PLL đã lock.
- Nếu chọn nguồn chưa stable thì switch diễn ra sau khi nguồn đó stable.
- Kiểm tra nguồn SYSCLK hiện tại trong RCC_CR.

Lưu ý: Nếu đã có nguồn clock khác đang sử dụng trực tiếp hoặc thông qua PLL thì không thể stop 

# Clock Security System (CSS)
Kiểm tra trong nguồn [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 97

CSS có thể kích hoạt phần mềm, enable sau startup delay của HSE, disable khi HSE tắt.

Nếu HSE lỗi, hướng giải quyết như sau:
1. HSE tự động tắt
2. Ngắt gửi đến TIM1-8
3. Ngắt chuyên biệt CSS (CSSI) gửi đến phần mềm cho MCU thực hiện các thao tác cần thiết.

Lưu ý: 
1. CSSI gắn liền với ngắt không mask (NMI).
2. Chỉ khi `CSS được bật` và `HSE lỗi` thì `CSSI xảy ra` và `NMI xảy ra`. NMI xử lý vô thời hạn cho tới khi bit `CSSI pending` được xóa. *(Điều này có nghĩa là NMI ISR phải xóa CSSI thông qua bit CSSC trong RCC_CIR)*

Trong trường hợp `HSE sử dụng làm nguồn trực tiếp hoặc gián tiếp`, khi HSE lỗi:
1. `SYSCLK tự động chuyển sang HSI` và `tắt HSE`.
2. Thực hiện các bước giải quyết.

Trong trường hợp `HSE làm nguồn cho PLL, PLL làm nguồn SYSCLK`, khi HSE lỗi:
1. `PLL tắt`.
2. `SYSCLK tự động chuyển sang HSI`.
3. Thực hiện các bước giải quyết.
