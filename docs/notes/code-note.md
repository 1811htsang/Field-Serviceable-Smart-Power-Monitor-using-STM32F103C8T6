# Giới thiệu

Phần ghi chú này trình bày các hướng dẫn nhằm chuẩn hóa quá trình viết mã nguồn và các hạng mục hỗ trợ cần lưu ý khi phát triển phần mềm. Mục tiêu là đảm bảo tính nhất quán, dễ bảo trì và nâng cao chất lượng mã nguồn trong các dự án phần mềm.

## Phương pháp tiếp cận

Quá trình viết mã nguồn nên tuân theo các nguyên tắc sau:

- Tính nhất quán: Sử dụng cùng một phong cách viết mã nguồn, bao gồm cách đặt tên biến, hàm và cấu trúc thư mục.
- Tính dễ đọc: Viết mã nguồn sao cho dễ hiểu, sử dụng các chú thích hợp lý để giải thích các phần phức tạp.
- Tính tái sử dụng: Thiết kế mã nguồn sao cho các thành phần có thể tái sử dụng trong các dự án khác nhau.
- Tính kiểm thử: Đảm bảo mã nguồn có thể được kiểm thử dễ dàng thông qua các unit test và integration test.

Ngoài ra, một lưu ý cần tránh trong thiết kế chính là sử dụng các master header file để chứa tất cả các khai báo. Điều này có thể dẫn đến việc phụ thuộc lẫn nhau giữa các module và làm tăng độ phức tạp của mã nguồn.

Nên đảm bảo rằng các header riêng có thể tự chứa tất cả các khai báo cần thiết để hoạt động độc lập, giúp giảm thiểu sự phụ thuộc và tăng tính mô-đun của mã nguồn.

## Cấu trúc mã nguồn

Trong quá trình thiết kế mã nguồn, sẽ tồn tại rất nhiều file mã nguồn khác nhau, bao gồm các file header (.h) và file cài đặt (.c).

Cụ thể hơn đối với các file header, chúng thường chứa các khai báo hàm, định nghĩa kiểu dữ liệu, hằng số và macro.

## Cấu trúc header file

Trong các header file, cần tuân thủ các quy tắc sau:

- Luôn có header guard để tránh việc include nhiều lần. Trong đó, tên header guard nên trùng tên với tên file, viết hoa và sử dụng dấu gạch dưới thay cho dấu chấm. Ví dụ

  ```c
  #ifndef LIB_CLOCK_MANAGEMENT_H
  #define LIB_CLOCK_MANAGEMENT_H
  // Nội dung header file
  #endif // LIB_CLOCK_MANAGEMENT_H
  ```

- Các hạng mục nên có trong header file bao gồm:
  - Khai báo các thư viện sử dụng chung
  - Khai báo địa chỉ ngoại vi sử dụng
  - Khai báo địa chỉ thanh ghi cụ thể
  - Khai báo cấu trúc & con trỏ thanh ghi
  - Khai báo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi (*)
  - Khai báo các định nghĩa bit sử dụng trên từng thanh ghi
  - Khai báo cấu trúc tham số hàm khởi tạo nếu có
  - Khai báo các kiểm tra tham số hàm khởi tạo nếu có
  - Khai báo prototype hàm

(*) **Lưu ý**:

Trong header file, cần phân biệt rõ ràng giữa con trỏ phần cứng thật và con trỏ phần cứng giả để hỗ trợ việc kiểm thử đơn vị (unit testing).

Cấu trúc header file khi khai báo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi như sau:

```c
// ... (Các định nghĩa struct của ta giữ nguyên)

#ifndef UNIT_TEST
  #define IWDG_REGS_PTR ((IWDG_REGS_Typedef *) IWDG_REGS_BASEADDR)
#else
  extern IWDG_REGS_Typedef Mock_IWDG; 
  #define IWDG_REGS_PTR (&Mock_IWDG)
#endif
```

Trong đó:

- Khi không trong chế độ kiểm thử đơn vị (UNIT_TEST không được định nghĩa), con trỏ phần cứng thật được định nghĩa trỏ tới địa chỉ cơ sở của ngoại vi.
- Khi trong chế độ kiểm thử đơn vị (UNIT_TEST được định nghĩa), con trỏ phần cứng giả trỏ tới một biến giả (Mock_IWDG) được khai báo bên ngoài, cho phép mô phỏng hành vi của ngoại vi trong quá trình kiểm thử.

Ví dụ:

Sau khi khai báo header file với con trỏ thật và con trỏ giả, khi viết test dependency cho module IWDG, ta có thể định nghĩa biến giả như sau:

```c
IWDG_REGS_Typedef Mock_IWDG = {0}; // Khởi tạo biến giả với tất cả giá trị bằng 0 nhằm dễ dàng kiểm tra
```

Hoặc có thể sử dụng `memset` để khởi tạo biến giả:

```c
IWDG_REGS_Typedef Mock_IWDG;
memset(&Mock_IWDG, 0, sizeof(IWDG_REGS_Typedef)); 
// Khởi tạo biến giả với tất cả giá trị bằng 0 nhằm dễ dàng kiểm tra
```

Tuy nhiên cũng cần lưu ý rằng giá trị reset ban đầu của biến giả không phải lúc nào cũng là 0, do đó cần thiết lập các giá trị ban đầu phù hợp với yêu cầu kiểm thử.

Ghi chú:

- Bổ sung các thông tin về khai thác gdb trong quá trình debug mã nguồn và unit test.
- Bổ sung các thông tin về thiết kế mã nguồn hỗ trợ unit test.

## Logic thiết kế giữa Reset, Clock và try-catch reset event

Trong quá trình thiết kế hệ thống nhúng, việc quản lý reset và clock là rất quan trọng để đảm bảo hệ thống hoạt động ổn định. Dưới đây là một số logic thiết kế cơ bản giữa Reset, Clock và việc xử lý sự kiện reset (try-catch reset event):

- Khi reset event xảy ra, hệ thống sẽ thực hiện reset ngay lập tức để đưa tất cả các module về trạng thái ban đầu (ngoại trừ WWDG) và khởi động lại quá trình khởi tạo hệ thống.
- Sau khi khởi động lại, các cờ báo reset sẽ được kiểm tra để xác định nguyên nhân gây ra reset. Ở đây cần lưu ý rằng, phần cứng sẽ tự phân biệt được giữa lần hoạt động bình thường và lần hoạt động do reset bằng cách sử dụng tín hiệu từ cấp nguồn (Power-on Reset).

Suy ra, trình tự khi thiết kế là như sau:

- Kiểm tra các cờ báo reset để xác định hoạt động
- Khởi tạo clock và watchdog cho hệ thống
- Thực hiện các chức năng chính của hệ thống

Ở đây, nếu có thể thì bổ sung thêm các thanh ghi BKP (Backup Power) để lưu trữ trạng thái trước khi reset, giúp hệ thống có thể phục hồi nhanh chóng sau khi reset hoặc cung cấp thông tin chi tiết về nguyên nhân reset cũng như ngắt việc phải reset liên tục trong các trường hợp lỗi phần mềm.

## Logic thiết kế giữa GPIO, AFIO và EXTI

Trong thiết kế driver của STM32, ta sẽ thấy có sự liên kết phụ thuộc giữa GPIO, AFIO và EXTI nhằm đảm bảo tính linh hoạt và tiện lợi khi sử dụng HAL Driver của STM32.

Tuy nhiên, sự phụ thuộc này sẽ gây ra sự khó khăn và phá vỡ nguyên tắc đơn nhiệm với các hàm init phải chứa logic của các ngoại vi khác. Do đó, cần thiết kế lại để tách biệt các phần này ra khỏi nhau, giúp giảm sự phụ thuộc và tăng tính mô-đun của mã nguồn.

Cụ thể hơn, ta có thể thiết kế lại như sau:

- GPIO sẽ chỉ chịu trách nhiệm cấu hình chân GPIO, bao gồm chế độ, cấu hình và tốc độ.
- AFIO sẽ chỉ chịu trách nhiệm cấu hình chuyển đổi chức năng (Alternate Function) cho các chân GPIO và cấu hình Line của EXTI.
- EXTI sẽ chỉ chịu trách nhiệm cấu hình ngắt ngoại (External Interrupt) cho các chân GPIO đã được cấu hình bởi AFIO.
- NVIC sẽ chỉ chịu trách nhiệm cấu hình mức ưu tiên và kích hoạt ngắt cho các ngắt đã được cấu hình bởi EXTI và các ngắt khác của hệ thống.

## Logic thiết kế giữa cấu hình GPIO và EXTI

Trên thiết kế cấu hình GPIO, STM32 sử dụng hướng thiết kế cấu hình hỗ trợ đa chân thông qua việc sử dụng bitmask (ví dụ: `GPIO_PIN_0 | GPIO_PIN_1`) để cấu hình nhiều chân cùng lúc, giúp đơn giản hóa quá trình cấu hình và tăng tính tiện lợi khi cần cấu hình nhiều chân cùng một lúc.

Tuy nhiên, việc áp dụng vòng lặp để cấu hình đa chân trên EXTI là một vấn đề gây tranh cãi về mặt kiến trúc. Câu trả lời là **CÓ THỂ** làm được để đồng nhất với GPIO, nhưng **KHÔNG KHUYẾN KHÍCH** sử dụng cho các ứng dụng thực tế phức tạp.

Dưới đây là phân tích chi tiết tại sao EXTI khác với GPIO và cách nên tiếp cận:

### Sự khác biệt bản chất giữa GPIO và EXTI

- GPIO khi cấu hình `GPIO_PIN_0 | GPIO_PIN_1` là `Output`, cả hai chân này thường có đặc điểm giống hệt nhau (cùng tốc độ, cùng kiểu đẩy kéo). Chúng thuộc về cùng một Port.
- EXTI (Sự kiện riêng biệt): Ngắt thường là các sự kiện độc lập.
  - Chân PA0 có thể là nút bấm (ngắt cạnh lên).
  - Chân PA1 có thể là tín hiệu từ cảm biến (ngắt cả hai cạnh).
  - Ràng buộc Port đảm bảo Mỗi EXTI Line chỉ có thể nhận tín hiệu từ **một Port duy nhất** tại một thời điểm (thông qua `AFIO_EXTICR`,  không thể cấu hình đồng thời PA0 và PB0 vì chúng dùng chung Line 0.

### Tại sao vòng lặp trong EXTI lại gây khó khăn cho Driver?

Nếu thiết kế hàm `EXTI_Init` hỗ trợ Bitmask (`PIN_0 | PIN_1`), ta sẽ gặp 3 vấn đề sau:

#### Vấn đề về Callback (Đăng ký logic)

Mỗi Line cần một Callback riêng. Nếu ta Init 5 chân cùng lúc trong 1 hàm, ta sẽ truyền Callback vào như thế nào?

- Nếu truyền 1 con trỏ hàm: Cả 5 chân dùng chung 1 logic (thường không thực tế).
- Nếu truyền mảng con trỏ hàm: Code sẽ cực kỳ phức tạp và dễ lỗi bộ nhớ.

#### Nguy cơ xung đột Port ngầm

Giả sử người dùng gọi: `EXTI_Init(GPIOA, PIN_0 | PIN_1, ...)`.
Sau đó lại gọi: `EXTI_Init(GPIOB, PIN_0 | PIN_2, ...)`.
Vòng lặp trong Driver sẽ vô tình ghi đè cấu hình của Line 0 từ Port A sang Port B. Nếu ta cấu hình từng chân riêng lẻ, việc kiểm soát xung đột này (bằng các lệnh `assert` hoặc kiểm tra trạng thái) sẽ minh bạch hơn rất nhiều.

#### Hiệu suất thực thi

Ngắt thường không được khởi tạo hàng loạt như GPIO. Trong một dự án, ta thường chỉ có 2-3 nguồn ngắt EXTI. Việc viết một vòng lặp quét 16 bit để cấu hình cho 1 chân duy nhất là một sự lãng phí tài nguyên (dù nhỏ).
