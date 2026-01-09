# Giới thiệu
Phần ghi chú này trình bày các hướng dẫn nhằm chuẩn hóa quá trình viết mã nguồn và các hạng mục hỗ trợ cần lưu ý khi phát triển phần mềm. Mục tiêu là đảm bảo tính nhất quán, dễ bảo trì và nâng cao chất lượng mã nguồn trong các dự án phần mềm.

# Phương pháp tiếp cận
Đối với dự án này, ở thời điểm ngày 9/1/2025 là tiếp cận theo hướng Master Header File. 

Trong đó, phương pháp này tập trung vào việc tạo ra các file header chính (master header files) để quản lý và tổ chức các khai báo, định nghĩa và cấu trúc dữ liệu quan trọng trong dự án. Các file header chính này sẽ đóng vai trò như một điểm trung tâm để các phần khác của mã nguồn có thể tham chiếu và sử dụng.

Các header file chính này sẽ không bao gồm các cài đặt chi tiết của hàm hay logic phức tạp, mà thay vào đó sẽ tập trung vào việc cung cấp các khai báo và định nghĩa cần thiết để đảm bảo tính nhất quán và dễ dàng bảo trì mã nguồn.

Một điểm cần lưu ý chính, cách tiếp cận này là hardware specific và không áp dụng cho các thư viện bên thứ ba (third-party libraries), do đó vấn đề về scalability sẽ không được xét đến trong dự án này.

Tuy nhiên, trong tương lai nếu dự án phát triển lớn hơn và cần tích hợp các thư viện bên thứ ba, có thể xem xét việc áp dụng các phương pháp quản lý mã nguồn khác như modular programming hoặc sử dụng các công cụ quản lý phụ thuộc (dependency management tools) để đảm bảo tính mở rộng và khả năng tái sử dụng của mã nguồn.

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