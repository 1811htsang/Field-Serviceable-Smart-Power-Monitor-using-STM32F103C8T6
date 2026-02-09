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
// ... (Các định nghĩa struct của bạn giữ nguyên)

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

Sau khi khai báo header file với con trỏ thật và con trỏ giả, khi viết test dependency cho module IWDG, bạn có thể định nghĩa biến giả như sau:

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
