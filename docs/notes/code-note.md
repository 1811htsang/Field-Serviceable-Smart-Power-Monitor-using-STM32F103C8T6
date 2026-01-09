# Giới thiệu
Phần ghi chú này trình bày các hướng dẫn nhằm chuẩn hóa quá trình viết mã nguồn và các hạng mục hỗ trợ cần lưu ý khi phát triển phần mềm. Mục tiêu là đảm bảo tính nhất quán, dễ bảo trì và nâng cao chất lượng mã nguồn trong các dự án phần mềm.

# Phương pháp tiếp cận
Quá trình viết mã nguồn nên tuân theo các nguyên tắc sau:
- Tính nhất quán: Sử dụng cùng một phong cách viết mã nguồn, bao gồm cách đặt tên biến, hàm và cấu trúc thư mục.
- Tính dễ đọc: Viết mã nguồn sao cho dễ hiểu, sử dụng các chú thích hợp lý để giải thích các phần phức tạp.
- Tính tái sử dụng: Thiết kế mã nguồn sao cho các thành phần có thể tái sử dụng trong các dự án khác nhau.
- Tính kiểm thử: Đảm bảo mã nguồn có thể được kiểm thử dễ dàng thông qua các unit test và integration test.

Ngoài ra, một lưu ý cần tránh trong thiết kế chính là sử dụng các master header file để chứa tất cả các khai báo. Điều này có thể dẫn đến việc phụ thuộc lẫn nhau giữa các module và làm tăng độ phức tạp của mã nguồn.

Nên đảm bảo rằng các header riêng có thể tự chứa tất cả các khai báo cần thiết để hoạt động độc lập, giúp giảm thiểu sự phụ thuộc và tăng tính mô-đun của mã nguồn.

# Cấu trúc mã nguồn
Trong quá trình thiết kế mã nguồn, sẽ tồn tại rất nhiều file mã nguồn khác nhau, bao gồm các file header (.h) và file cài đặt (.c). 

Cụ thể hơn đối với các file header, chúng thường chứa các khai báo hàm, định nghĩa kiểu dữ liệu, hằng số và macro.

# Cấu trúc header file
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
  - Khai báo các định nghĩa bit sử dụng trên từng thanh ghi
  - Khai báo cấu trúc tham số hàm khởi tạo nếu có
  - Khai báo các kiểm tra tham số hàm khởi tạo nếu có
  - Khai báo prototype hàm