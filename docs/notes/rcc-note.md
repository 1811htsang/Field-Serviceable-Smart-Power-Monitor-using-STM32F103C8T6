# Reset
Chia ra làm 3 loại:
- System reset
- Power reset
- Backup domain reset

## System reset
Reset tất cả thanh ghi về giá trị mặc định, ngoại trừ thanh ghi trong khu vực CSR và Backup domain. 

**Lưu ý**: System reset này không có nghĩa là reset tất cả mọi thứ về trạng thái ban đầu như lúc mới cấp nguồn (Power-on reset) mà chỉ thực hiện reset các thanh ghi trong hệ thống ngoại trừ các thanh ghi trong khu vực CSR và Backup domain.

Chỉ xảy ra reset với các trường hợp:
1. NRST pin bị kéo thấp (này là external)
2. Window watchdog timeout (WWGD reset)
3. Independent watchdog timeout (IWDG reset)
4. Software reset (SW reset)
5. Low-power management reset (LPM reset)

Các nguồn này có thể xác định thông qua kiểm tra RCC_CSR.

### Software reset
Bật bit SYSRESETREQ trong thanh ghi SCB_AIRCR để yêu cầu một system reset.(Yêu cầu quyền truy cập viết vào thanh ghi này)

Cụ thể hơn phải ghi `0x5FA` vào trường `VECTKEY[31:16]` và `1` vào trường `SYSRESETREQ[2]` của thanh ghi SCB_AIRCR để được chấp nhận.

Kiểm chứng thông tin này trong [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 90 và [pm0056-stm32](/docs/references/pm0056-stm32-f10-f20-f21-l1-cortex-m3-programming-manual.pdf) trang 134.

### LPM reset
Xảy ra khi:
1. Hệ thống thoát khỏi chế độ Standby. 
   
   *Loại này thực hiện thông qua reset bit nRST_STDBBY trong User Option Bytes. Trong trường hợp này, khi chuỗi entry vào chế độ Standby được thực hiện, hệ thống sẽ tự động reset.*
2. Hệ thống thoát khỏi chế độ Stop. 
   
   *Loại này thực hiện thông qua reset bit nRST_STOP trong User Option Bytes. Trong trường hợp này, khi chuỗi entry vào chế độ Stop được thực hiện, hệ thống sẽ tự động reset.*

Kiểm chứng thông tin này trong [pm0075-stm32f10xxx](/docs/references/pm0075-stm32f10xxx-flash-programming-manual.pdf) trang 19-20-21.

## Power reset
Chỉ xảy ra trong 2 events:
1. POR/PDR reset: Bật nguồn hoặc khôi phục nguồn.
2. Thoát Standby mode.

Power reset sẽ reset tất cả thanh ghi về giá trị mặc định ngoại trừ Backup domain.

Các nguồn event reset này phụ thuộc vào chân NRST và giữ mức thấp trong delay phase. 

Vector của RST_SVC_RTN giữ cố định ở địa chỉ `0x00000004` trong suốt quá trình Power reset.

Tín hiệu để thực hiện reset từ bên ngoài là thông qua chân NRST, khi nhấn giữ NRST (*tức là NRST đang mức thấp*), xung reset được tạo ra. Bất kể trường hợp reset từ chân NRST hay từ bên trong, đều sẽ có khoảng delay tối thiểu 20 micro-s với mỗi tín hiệu. 

Kiểm chứng thông tin này [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 91.

## Backup domain reset
Chỉ xảy ra khi có tín hiệu SWRST hoặc VDD/VBAT bật lên sau khi bị tắt trước đó.

## Quy trình reset khi sử dụng chân NRST (cần lưu ý lại)
1. NRST pin bị kéo thấp
2. `RCC_CSR->PINRSTF = 1;` // Cờ từ RCC_CSR
3. Thông báo reset tự động đến các module
4. Hiển thị tín hiệu reset trên pin / console log nếu cần thiết
5. `RCC_CSR->RMVF = 1;` // Xóa cờ reset
6. NRST pin trở về mức cao

**Lưu ý**: Trong schematics và mạch thật thì sẽ có 1 chân R (Reset) và 1 nút Reset riêng biệt. Chưa làm rõ thông tin về tín hiệu NRST sẽ thuộc về chân R hay nút Reset.

**Bổ sung**: Sau khi tìm hiểu các tài liệu trực tuyến, có thể rút ra kết luận như sau:
- Chân NRST là chân vật lý trên MCU, dùng để nhận tín hiệu reset từ bên ngoài (như nút reset, mạch reset,...)
- Nút Reset là nút nhấn được thiết kế sẵn trên board mạch, kết nối với chân NRST để tạo tín hiệu reset.

Kiểm chứng thông tin này trong [đây](https://dev.to/carolineee/what-is-the-r-pin-on-the-stm32-blue-pill-1ma5).

Do đó, khi nhấn nút Reset, nó sẽ kéo chân NRST xuống mức thấp, từ đó kích hoạt quá trình reset trên MCU mà không cần bổ sung mạch reset phức tạp.

## Quy trình reset khi sử dụng Software reset
1. `SCB_AIRCR->VECTKEY[31:16] = 0x5FA` // Khóa ghi để mở quyền truy cập viết vào thanh ghi AIRCR
2. `SCB_AIRCR->SYSRESETREQ[2] = 1;` // Yêu cầu system reset

**Lưu ý**: Do có các báo cáo bổ sung về ARM Cortex-M4 lỗi khi thực hiện Software reset (*thông qua sử dụng hàm `NVIC_SystemReset()`*), nên cần kiểm tra kỹ lưỡng trong quá trình triển khai bằng việc đảm bảo không xảy ra quá trình thao tác với bộ nhớ trong khi thực hiện Software reset, hoặc ràng buộc các thao tác này phải hoàn thành trước khi gọi hàm reset.

Kiểm chứng thông tin tại [đây](https://www.systemonchips.com/arm-cortex-m4-system-reset-failure-via-nvic_systemreset-function/).


# Clock System trên STM32F1xx
Các src CLK chính sử dụng trong RCC:
- HSI
- LSI
- HSE
- LSE

Trong đa số trường hợp sử dụng HSI/HSE làm nguồn CLK chính cho hệ thống.

## CLK tree
Kiểm tra trong trng:
- [prod-stm32](/docs/references/production-stm32-f103x8-f103xb-datasheet.pdf) trang 12
- [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 93

## HSI
Kiểm tra trong các nguồn:
- [prod-stm32](/docs/references/production-stm32-f103x8-f103xb-datasheet.pdf) trang 54
- [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 95

Nguồn này được tạo ra từ bộ oscillator 8MHz nội bộ, có thể sử dụng như nguồn CLK chính cho hệ thống hoặc làm nguồn CLK cho PLL.

Không cần dùng clock ngoài, thời gian khởi động nhanh nhưng độ chính xác thấp kể cả có hiệu chỉnh.

Nguồn này có thể làm nguồn dự phòng trong trường hợp HSE bị lỗi
-> Ưu tiên sử dụng HSE làm nguồn CLK chính cho hệ thống, nếu HSE lỗi thì chuyển sang HSI.

NSX đã hiệu chỉnh sẵn tần số HSI trong nhà máy, sai số tối đa ±1% ở nhiệt độ 25°C. Giá trị hiệu chỉnh nằm trong thanh ghi `RCC_CR->HSITRIM[7:0]`.

Nguyên tắc sử dụng bộ HSI:
- Bật HSI: đặt HSION bit trong RCC_CR (`RCC_CR->HSION = 1`)
- Chờ HSI sẵn sàng: kiểm tra HSIRDY bit trong RCC_CR (`RCC_CR->HSIRDY == 1`)

## HSE
Kiểm tra trong các nguồn:
- [prod-stm32](/docs/references/production-stm32-f103x8-f103xb-datasheet.pdf) trang 52

Nguồn này sử dụng bộ thạch anh ngoài cho tần số từ 4MHz đến 16MHz, cho ra tần số chính xác cao hơn HSI.

Nguyên tắc sử dụng bộ HSE:
- Bật HSE: đặt HSEON bit trong RCC_CR (`RCC_CR->HSEON = 1`)
- Chờ HSE sẵn sàng: kiểm tra HSERDY bit trong RCC_CR (`RCC_CR->HSERDY == 1`)
  - Ngắt có thể tạo ra nếu enable trong RCC_CIR 


## SYSCLK select
Kiểm tra trong nguồn [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 97

Nguyên tắc lựa chọn nguồn SYSCLK:
- Sau system reset, HSI được chọn làm nguồn SYSCLK mặc định. 
- Muốn chuyển qua nguồn khác thì chỉ có thể khi nguồn được target đó stable sau startup delay hoặc PLL đã lock.
- Nếu chọn nguồn chưa stable thì switch diễn ra sau khi nguồn đó stable.
- Kiểm tra nguồn SYSCLK hiện tại trong RCC_CR.

Lưu ý: Nếu đã có nguồn clock khác đang sử dụng trực tiếp hoặc thông qua PLL thì không thể stop 

## Clock Security System (CSS)
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
1. SYSCLK tự động chuyển sang HSI và tắt HSE.
2. Thực hiện các bước giải quyết.

Trong trường hợp `HSE làm nguồn cho PLL, PLL làm nguồn SYSCLK`, khi HSE lỗi:
1. PLL tắt.
2. SYSCLK tự động chuyển sang HSI.
3. Thực hiện các bước giải quyết.

## MCO
Kiểm tra trong nguồn [rm0008-stm32](/docs/references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 98

MCO (Microcontroller Clock Output) dùng để xuất một trong các nguồn clock nội bộ ra chân GPIO để sử dụng bên ngoài.

Nguồn có thể xuất ra chân MCO bao gồm:
- SYSCLK
- HSI
- HSE
- PLL/2

### Quy trình select và khởi động nguồn clock chính cho hệ thống
0. reset STM32, lúc này nguồn SYSCLK là HSI
1. reset RCC_CR, RCC_CFGR, RCC_CIR về mặc định
2. `RCC_CFGR->SW = 00;` // Chọn HSI làm nguồn SYSCLK tạm thời
3. `RCC_CR->CSSON = 1;` // Bật CSS để giám sát HSE trước
4. `RCC_CIR->HSERDYIE = 1;` // Enable ngắt HSE ready
5. `RCC_CR->HSEON = 1;` // Bật HSE lên
6. Chờ HSE ready
7. Khi HSE ready
   - `RCC_CIR->HSEDYF = 1;` // Cờ từ hệ thống ngắt  
   - `RCC_CR->HSERRDY = 1;` // Cờ từ RCC_CR
8. `RCC_CFGR->SW = 01;` // Chuyển SYSCLK sang HSE
9. `RCC_CIR->HSERDYC = 1;` // Xóa cờ ngắt HSE ready
10. Tiếp tục cấu hình các nguồn clock khác nếu cần thiết

### Quy trình xử lý khi HSE lỗi
1. Xảy ra lỗi HSE
   -  Nghĩa là khi xảy ra lỗi HSE, HSE tự động tắt
   -  `RCC_CR->CSSON = 0;` // Tắt CSS để tránh lặp ngắt
   -  `RCC_CR->HSEON = 0;` // HSE tắt
   -  `RCC_CR->HSERDY = 0;` // Cờ HSE ready tắt sau khi HSEON = 0 trong 6 chu kỳ
   -  `RCC_CFGR->SW = 00;` // Chuyển SYSCLK sang HSI
2. `RCC_CIR->CSSF = 1;` // Cờ từ hệ thống ngắt
3. `RCC_CIR->HSIDYIE = 1;` // Enable ngắt HSI ready
4. `RCC_CR->HSION = 1;` // Ép bật HSI lên nếu chưa bật
5. Chờ HSI ready
6. Khi HSI ready
   - `RCC_CIR->HSIRDYF = 1;` // Cờ từ hệ thống ngắt
   - `RCC_CR->HSIRDY = 1;` // Cờ từ RCC_CR
7. `RCC_CIR->HSIRDYC = 1;` // Xóa cờ ngắt HSI ready
8. Thực hiện các bước xử lý khác nếu cần thiết

   