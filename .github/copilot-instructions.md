# Giới thiệu tổng quan
Đây là tài liệu git Hub Copilot Instructions. Nó cung cấp hướng dẫn cụ thể về cách sử dụng GitHub Copilot trong bối cảnh của dự án này.

**Tên dự án:** Field-Serviceable Smart Power Monitor using STM32F103C8T6

**Mô tả dự án:** Dự án này nhằm phát triển một thiết bị giám sát năng lượng thông minh có thể sửa chữa tại chỗ, sử dụng vi điều khiển STM32F103C8T6. Thiết bị sẽ theo dõi mức tiêu thụ năng lượng, cung cấp dữ liệu thời gian thực và cho phép người dùng thực hiện các sửa chữa hoặc nâng cấp mà không cần thay thế toàn bộ thiết bị.

Cấu trúc dự án hiện tại bao gồm các thành phần chính sau:
```
STM32/
├── .github/                      # Cấu hình GitHub và hướng dẫn Copilot
├── docs/                         # Thư mục tài liệu dự án
│   ├── notes/                    # Ghi chú phát triển và lộ trình
│   └── references/               # Tài liệu tham khảo kỹ thuật
├── project-artifactspace/        # Không gian hoạt động của dự án chứa mã nguồn và tài nguyên
│   ├── HAL_Sample_Driver/        # Ví dụ về driver HAL cho STM32 trích xuất từ STM32CubeIDE
│   ├── Impl/                     # Thư mục chứa các triển khai chi tiết của thư viện
│   ├── Inc/                      # Thư mục chứa các file header (.h)
│   ├── Src/                      # Thư mục chứa các file nguồn (.c)
│   ├── Test/                     # Thư mục chứa các file kiểm thử cho dự án
├── project-halartifactspace/     # Không gian phần cứng trừu tượng do STM32CubeIDE tạo ra
│   ├── Core/                     # Thư mục chứa các file lõi của dự án
│   │   ├── Inc/                  # Thư mục chứa các file header (.h)
│   │   └── Src/                  # Thư mục chứa các file nguồn (.c)
│   ├── Drivers/                  # Thư mục chứa các triển khai chi tiết của thư viện mẫu
│   │   ├── CMSIS/                # Thư mục chứa các triển khai theo tiêu chuẩn CMSIS
│   │   └── STM32F1xx_HAL_Driver/ # Thư mục chứa các driver HAL cho STM32F1xx
│   ├── Inc/                      # Thư mục chứa các file header (.h)
│   ├── Src/                      # Thư mục chứa các file nguồn (.c)
│   ├── Test/                     # Thư mục chứa các file kiểm thử cho dự án
├── project-obsidianspace/        # Không gian hoạt động Obsidian cho ghi chú và tài liệu
│   ├── references/               # Thư mục tài liệu tham khảo kỹ thuật
│   ├── OBSIDIAN-README.md        # Hướng dẫn sử dụng Obsidian cho dự án
│   └── notes/                    # Thư mục ghi chú Obsidian
├── README.md                     # Tài liệu giới thiệu tổng quan về dự án
└── .gitignore                    # File cấu hình gitignore
```

**Lưu ý:** 
- README.md và OBSIDIAN-README.md ở các nhánh khác nhau trên Github chưa hoàn toàn đồng bộ nội dung. 
- Thư mục project-artifactspace/ chứa mã nguồn chính của dự án, trong khi project-halartifactspace/ là không gian phần cứng trừu tượng do STM32CubeIDE tạo ra.
- Tập trung vào `docs/notes/to-do.md` để theo dõi lộ trình phát triển chi tiết và các nhiệm vụ đã hoàn thành cũng như chưa hoàn thành.

# Vai trò của GitHub Copilot trong dự án
Github Copilot sẽ hỗ trợ chuẩn hóa mã nguồn, cấu trúc thư mục, và quy trình làm việc trong dự án này. Các hướng dẫn cụ thể được cung cấp trong các phần tiếp theo nhằm đảm bảo rằng mã nguồn và tài liệu được duy trì một cách nhất quán và dễ hiểu.

Github Copilot sẽ không thực hiện các thay đổi trực tiếp vào mã nguồn hoặc tài liệu mà không có sự giám sát của con người. Thay vào đó, nó sẽ cung cấp các đề xuất và hướng dẫn dựa trên các tiêu chuẩn đã được thiết lập trong tài liệu này.

# Tiêu chuẩn commit
Tiêu chuẩn commit này nhằm đảm bảo rằng tất cả các commit trong dự án đều rõ ràng, nhất quán và dễ hiểu. Việc tuân thủ các hướng dẫn này sẽ giúp cải thiện khả năng theo dõi lịch sử thay đổi và hỗ trợ làm việc nhóm hiệu quả hơn.

## Giới thiệu
Đối với dự án này, tập trung vào tuân thủ tiêu chuẩn *Conventional commits* để đảm bảo lịch sử cam kết rõ ràng và nhất quán.

## Công thức chuẩn
Cấu trúc một commit tuân theo định dạng sau:
```
Type(Scope): Subject
```

Trong đó:
- **Type** để chỉ định loại thay đổi được thực hiện.
- **Scope** là tùy chọn và chỉ định phần cụ thể của mã bị ảnh hưởng. (Có thể chỉ rõ các file bị ảnh hưởng)
- **Subject** là mô tả ngắn gọn về thay đổi. (Sử dụng *imperative mood*)

### Đối với nhóm tính năng mới
Sử dụng loại commit `feat` để biểu thị việc hoạt động với các tính năng trên mã nguồn.

Các thao tác ở nhóm commit này là
- **Implement**: Triển khai một chức năng hoàn chỉnh (dùng nhiều nhất).
  - Ví dụ: *feat(uart): implement XMODEM protocol for file transfer*
- **Integrate**: Tích hợp thư viện hoặc module bên ngoài.
  - Ví dụ: *feat(rtos): integrate FreeRTOS kernel v10.4*
- **Introduce**: Giới thiệu một cấu hình hoặc kiến trúc mới.
  - Ví dụ: *feat(bsp): introduce linker script for dual-bank flash*
- **Expose**: Mở API cho layer khác sử dụng.
  - Ví dụ: *feat(i2s): expose DMA interrupt callback functions*
- Ngoài ra có thể sử dụng các thao tác khác tùy theo ngữ cảnh.

### Đối với nhóm tối ưu , tái cấu trúc
Sử dụng loại commit `refactor` để biểu thị việc tái cấu trúc mã nguồn mà không thêm tính năng mới hoặc sửa lỗi.

Các thao tác ở nhóm commit này là
- **Optimize**: Tối ưu hóa hiệu năng/bộ nhớ.
  - Ví dụ: *perf(isr): optimize interrupt latency by moving code to IRAM*
  - Ví dụ: *perf(flash): reduce code size by bypassing HAL libraries*
- **Refactor**: Viết lại code cho sạch hơn mà không đổi tính năng.
  - Ví dụ: *refactor(uart): decouple hardware dependency from logic layer*
- **Simplify**: Đơn giản hóa logic phức tạp.
  - Ví dụ: *refactor(fsm): simplify state transition logic*

### Đối với nhóm tài liệu
Sử dụng loại commit `docs` để biểu thị việc cập nhật tài liệu.

Các thao tác ở nhóm commit này là
- **Document**: Viết tài liệu code.
  - Ví dụ: *docs(readme): document wiring diagram for ESP32 and Mic*
- **Configure**: Thay đổi cài đặt hệ thống.
  - Ví dụ: *chore(cmake): configure build output directory*
- **Update**: Cập nhật thông thường (hạn chế dùng cho code logic, dùng cho docs/tool).
  - Ví dụ: *docs(contributing): update commit guidelines*
- **Add**: Thêm tài liệu mới.
  - Ví dụ: *docs(api): add API reference for UART driver*

### Đối với nhóm cấu trúc quản lý phiên bản
Sử dụng loại commit `struct` để biểu thị việc thay đổi cấu trúc quản lý phiên bản mà không ảnh hưởng đến mã nguồn.

Các thao tác ở nhóm commit này là
- **Bump**: Tăng phiên bản phần mềm.
- **Merge**: Hợp nhất các nhánh quản lý phiên bản.
- **Release**: Đánh dấu phát hành phiên bản mới.
- **Tag**: Gắn thẻ phiên bản trong hệ thống quản lý mã nguồn.

Trong nhóm này, ta thường sử dụng thao tác **Merge** để hợp nhất nhánh `docs` và `feat` nhằm đồng bộ tài liệu và mã nguồn. Ví dụ như sau:
```
struct(docs, feat): Merge branch 'docs' into 'feat' to revise code guidelines to match new standards
```

Trong đó:
- Nhánh `X` nào được merge vào nhánh `Y` hiện tại sẽ là toán tử đầu vào của `struct(X, Y)` và `Merge branch 'X' into 'Y' to` trong phần mô tả.
- Phần nội dung mô tả sau `to` sẽ nêu rõ mục đích của việc merge này.

## Lưu ý bổ sung
1. **Nguyên tắc "Nhánh nào việc nấy" (Separation of Concerns)**

    Mặc dù đã đồng bộ (merge) hai nhánh, nhưng mục đích ban đầu của chúng vẫn khác nhau:
    - Nhánh `feat`: Tập trung phát triển tính năng (Code là chính).
    - Nhánh `docs`: Tập trung viết tài liệu (Markdown, hướng dẫn là chính).

    Nếu bạn đang ở nhánh `feat` mà lại sửa tài liệu:
    Sau này khi bạn tạo Pull Request (PR) để đưa code vào nhánh chính (`main` hoặc `develop`), cái PR đó sẽ bị lẫn lộn giữa "Code tính năng" và "Cập nhật tài liệu". Người review sẽ khó theo dõi.

2. **Hiện tượng "File rác" khi chuyển nhánh (Working Directory)**

    Có một điểm quan trọng: Nếu bạn tạo một file mới nhưng *chưa commit*, khi bạn dùng `git checkout` sang nhánh khác, file đó sẽ "đi theo" bạn sang nhánh mới.

    *Nguy cơ:* Bạn định thêm tài liệu vào nhánh `docs`, nhưng quên không checkout, bạn tạo file ở `feat`. Sau đó bạn nhớ ra và checkout sang `docs`, file đó vẫn nằm đó. Nếu bạn không cẩn thận `git add`, bạn sẽ commit nhầm những thứ không liên quan.

3. **Quy trình làm việc (Workflow) khuyến nghị**

    Để tránh nhập nhằng, bạn nên áp dụng quy trình sau:

    *Kịch bản A: Bạn muốn bổ sung Tài liệu*
    1.  `git checkout docs` (Chuyển hẳn sang nhánh tài liệu).
    2.  Thực hiện viết tài liệu, tạo file mới.
    3.  `git add` và `git commit`.
    4.  Nếu muốn nhánh `feat` cũng có tài liệu mới này: `git checkout feat` -> `git merge docs`.

    *Kịch bản B: Bạn muốn bổ sung Code*
    1.  `git checkout feat`.
    2.  Viết code, tạo file code mới.
    3.  `git add` và `git commit`.
    4.  (Thường thì nhánh `docs` không cần chứa code đang phát triển, nên bạn không cần merge ngược lại).

4. **Mẹo xử lý khi "Lỡ quên" (Sử dụng Git Stash)**

    Nếu bạn đang code dở ở nhánh `feat` nhưng đột nhiên sếp bảo cập nhật tài liệu gấp ở nhánh `docs`:

    1.  *Lưu tạm code đang dở:*
        ```bash
        git stash
        ```
    2.  *Chuyển sang nhánh docs và làm việc:*
        ```bash
        git checkout docs
        # Sửa tài liệu...
        git add .
        git commit -m "Update docs"
        ```
    3.  *Quay lại nhánh feat và lấy lại code đang làm dở:*
        ```bash
        git checkout feat
        git stash pop
        ```

## Sửa đổi commit message
Để sửa đổi commit message của commit gần nhất, bạn có thể sử dụng lệnh:
```bash
git commit --amend
```

Nếu commit đã push lên remote, bạn cần force push sau khi sửa đổi:
```bash
git push --force
```

# Tiêu chuẩn gitignore
Tiêu chuẩn gitignore này nhằm đảm bảo rằng các tệp và thư mục không cần thiết hoặc nhạy cảm không bị theo dõi bởi hệ thống kiểm soát phiên bản Git. Việc tuân thủ các hướng dẫn này sẽ giúp giữ cho kho mã nguồn sạch sẽ, bảo mật và dễ quản lý hơn.

## Giới thiệu
gitignore là một tệp cấu hình đặc biệt trong Git, dùng để chỉ định các tệp và thư mục mà bạn muốn Git bỏ qua, không theo dõi sự thay đổi của chúng. Điều này rất hữu ích để tránh việc commit các tệp tạm thời, tệp cấu hình cá nhân, hoặc các tệp sinh ra trong quá trình biên dịch mà không cần thiết phải đưa vào kho mã nguồn.

## Cách sử dụng .gitignore
1. **Tạo tệp .gitignore**: Tạo một tệp có tên `.gitignore` ở thư mục gốc của dự án Git của bạn.
2. **Thêm quy tắc vào .gitignore**: Mỗi dòng trong tệp `.gitignore` đại diện cho một mẫu tệp hoặc thư mục mà bạn muốn Git bỏ qua. Ví dụ:
   - `*.log` sẽ bỏ qua tất cả các tệp có phần mở rộng `.log`.
   - `temp/` sẽ bỏ qua toàn bộ thư mục `temp` và tất cả các tệp bên trong nó.
   - `config/*.json` sẽ bỏ qua tất cả các tệp `.json` trong thư mục `config`.

## Các quy tắc phổ biến
- Sử dụng `#` để thêm chú thích trong tệp `.gitignore`.
- Dùng dấu `/` để chỉ định thư mục cụ thể.
- Dùng dấu `*` làm ký tự đại diện cho bất kỳ chuỗi ký tự nào.
- Dùng dấu `!` để phủ định một quy tắc, tức là theo dõi tệp hoặc thư mục đó ngay cả khi nó bị bỏ qua bởi quy tắc trước đó.

## Nguyên tắc sử dụng .gitignore trong dự án
Cấu trúc của 1 file gitignore tiêu chuẩn cho dự án này như sau:

```
# Ignore + tên file hoặc thư mục hoặc kiểu file 1
<Liệt các file hoặc thư mục hoặc kiểu file cần ignore theo quy tắc đã nêu ở trên>

# Ignore + tên file hoặc thư mục hoặc kiểu file 2
<Liệt các file hoặc thư mục hoặc kiểu file cần ignore theo quy tắc đã nêu ở trên>
```


# Cấu trúc làm việc với project-artifactspace
Dưới đây là hướng dẫn về cách tổ chức và làm việc với không gian dự án `project-artifactspace/`, nơi chứa mã nguồn và tài nguyên chính của dự án.

Cấu trúc thư mục nên tuân thủ trong trường hợp bổ sung các ngoại vi mới như sau:
```
project-artifactspace/
├── HAL_Sample_Driver/                    # Ví dụ về driver HAL cho STM32 trích xuất từ STM32CubeIDE
├── Impl/                                 # Thư mục chứa các triển khai chi tiết của thư viện
│   ├── lib_a_impl.c                      # File nguồn triển khai chi tiết của ngoại vi A
│   └── lib_b_impl.c                      # File nguồn triển khai chi tiết của ngoại vi B
├── Inc/                                  # Thư mục chứa các file header (.h)
│   ├── generic/                          # Thư mục chứa các cấu hình chung về từ khóa và kiểu dữ liệu
│   │   └── lib_keyword_def.h             # File định nghĩa cấu hình từ khóa
│   ├── A/                                # Thư mục chứa các file header của ngoại vi A
│   │   ├── lib_a_def.h                   # File định nghĩa cấu hình của ngoại vi A
│   │   └── lib_a_hal.h                   # File khai báo hàm sử dụng của ngoại vi A
│   ├── B/                                # Thư mục chứa các triển khai chi tiết của ngoại vi B
│   │   ├── lib_b_def.h                   # File định nghĩa cấu hình của ngoại vi B
│   │   └── lib_b_hal.h                   # File khai báo hàm sử dụng của ngoại vi B
│   └── .../                              # Các ngoại vi khác tương tự
├── Src/                                  # Thư mục chứa các file nguồn (.c)
│   ├── main.c                            # File nguồn chính của dự án
│   ├── syscalls.c                        # File lời gọi hệ thống
│   └── sysmem.c                          # File quản lý bộ nhớ hệ thống
└── Test/                                 # Thư mục chứa các file kiểm thử cho dự án
    ├── A/                                # Thư mục chứa các file kiểm thử của ngoại vi A
    │   ├── header_dependency.h           # File định nghĩa phụ thuộc cho kiểm thử ngoại vi A
    │   ├── source_dependency.h           # File nguồn phụ thuộc cho kiểm thử ngoại vi A
    │   └── unit_test_implementation.c    # File kiểm thử đơn vị cho ngoại vi A
    ├── B/                                # Thư mục chứa các file kiểm thử của ngoại vi B
    │   ├── header_dependency.h           # File định nghĩa phụ thuộc cho kiểm thử ngoại vi B
    │   ├── source_dependency.h           # File nguồn phụ thuộc cho kiểm thử ngoại vi B
    │   └── unit_test_implementation.c    # File kiểm thử đơn vị cho ngoại vi B
    └── .../                              # Các ngoại vi khác tương tự
```

## Cấu trúc folder 
Khi bổ sung ngoại vi mới, cần tuân thủ cấu trúc file như sau:
```
project-artifactspace/
├── Inc/
│   └── <Peripheral_Name>/                # Thư mục chứa các file header của ngoại vi mới
│       ├── lib_<peripheral>_def.h        # File định nghĩa cấu hình của ngoại vi mới
│       └── lib_<peripheral>_hal.h        # File khai báo hàm sử dụng của ngoại vi mới
├── Impl/
│   └── lib_<peripheral>_impl.c          # File nguồn triển khai chi tiết của ngoại vi mới
└── Test/
    └── <Peripheral_Name>/                # Thư mục chứa các file kiểm thử của ngoại vi mới
        ├── header_dependency.h           # File định nghĩa phụ thuộc cho kiểm thử ngoại vi mới
        ├── source_dependency.h           # File nguồn phụ thuộc cho kiểm thử ngoại vi mới
        └── unit_test_implementation.c    # File kiểm thử đơn vị cho ngoại vi mới
```

## Cấu trúc file
Cấu trúc file khi bổ sung ngoại vi mới cần tuân thủ tùy thuộc vào loại `impl`, `def`, `hal`, và `unit_test`.

### Cấu trúc file def
Cấu trúc file định nghĩa cấu hình ngoại vi mới (`lib_<peripheral>_def.h`) như sau:
```c
/*
 * lib_<peripheral>_def.h
 *
 *  Created on: <Month> <Day>, <Year>
 *      Author: shanghuang
 */

#ifndef LIB_<PERIPHERAL>_DEF_H_
	#define LIB_<PERIPHERAL>_DEF_H_

	// Khai báo các thư viện sử dụng chung

	// Khai báo địa chỉ ngoại vi

	// Khai báo địa chỉ thanh ghi cụ thể

	// Khai báo cấu trúc thanh ghi

	// >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

	// Khai báo các định nghĩa bit cần sử dụng trên các thanh ghi

#endif /* LIB_<PERIPHERAL>_DEF_H_ */
```

**Luu ý:** 

- Thay `<PERIPHERAL>` bằng tên ngoại vi viết hoa toàn bộ, và `<peripheral>` bằng tên ngoại vi viết thường trong toàn bộ file.
- Trong 1 số ngoại vi có thể sẽ không sử dụng tới phần khai báo cấu trúc thanh ghi hoặc địa chỉ thanh ghi cụ thể, thì có thể bỏ qua phần đó.

### Cấu trúc file hal
Cấu trúc file khai báo hàm sử dụng ngoại vi mới (`lib_<peripheral>_hal.h`) như sau:
```c
/*
 * lib_<peripheral>_hal.h
 *
 *  Created on: <Month> <Day>, <Year>
 *      Author: shanghuang
 */

#ifndef LIB_<PERIPHERAL>_HAL_H_
  #define LIB_<PERIPHERAL>_HAL_H_

  // Khai báo các thư viện sử dụng chung

  // Khai báo cấu trúc tham số hàm khởi tạo (nếu có)  

	// Khai báo các cờ trả về có thời hạn (nếu có)

  // Khai báo các kiểm tra tham số đầu vào nội bộ (nếu có)

  // Khai báo IQRHandler (nếu có)

  // Khai báo các hàm thành phần

#endif /* LIB_<PERIPHERAL>_HAL_H_ */
```

### Cấu trúc file impl
Cấu trúc file triển khai chi tiết ngoại vi mới (`lib_<peripheral>_impl.c`) như sau:
```c
/*
 * lib_iwdg_impl.c
 *
 *  Created on: Jan 14, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  /*
    Bổ sung các preprocessor directives cần thiết cho unit test
  */

// Định nghĩa các hàm thành phần

```

### Cấu trúc file header dependency
Cấu trúc file định nghĩa phụ thuộc cho kiểm thử ngoại vi mới (`header_dependency.h`) như sau:
```c
/*
 * header_dependency.h
 *
 *  Created on: <Month> <Day>, <Year>
 *      Author: shanghuang
 */

#ifndef <PERIPHERAL>_HEADER_DEPENDENCY_H_
  #define <PERIPHERAL>_HEADER_DEPENDENCY_H_

  // Khai báo các thư viện chung cho unit test
  
  // Khai báo các dependency definition từ các file header cần thiết

    // >> Từ lib_a_def/hal

    // >> Từ lib_b_def/hal

  // Khai báo các dependency function từ các file header cần thiết ngoài module đang test

#endif /* <PERIPHERAL>_HEADER_DEPENDENCY_H_ */
```

### Cấu trúc file source dependency
Cấu trúc file nguồn phụ thuộc cho kiểm thử ngoại vi mới (`source_dependency.h`) như sau:
```c
/*
 * source_dependency.c
 *
 *  Created on: <Month> <Day>, <Year>
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  /*
    Thông thường chỉ cần include file header dependency
  */

// Khai báo các dependency của các hàm cần định nghĩa

// Định nghĩa các hàm mock tương ứng
```

### Cấu trúc file unit test implementation
Cấu trúc file kiểm thử đơn vị cho ngoại vi mới (`unit_test_implementation.c`) như sau:
```c
/*
 * unit_test_implementation.c
 *
 *  Created on: Jan 21, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  /*
    Ở unit_test_implementation.c này, 
    cần include file header dependency và file của ngoại vi đang test
  */

// Khai báo ngoại vi giả cho mục đích unit test

// Gọi ủy quyền các biến và hàm mock từ source_dependency.c

// Định nghĩa các hàm 

  void setup() {
    /*
      Hàm này được gọi trước mỗi test case để khởi tạo lại trạng thái cho unit test
    */
  }

// Thực thi tất cả các test case

int main() {
    printf("\n--- <PERIPHERAL> UNIT TEST ---\n");
    
    /*
      Gọi lần lượt tất cả các hàm test case ở đây
    */
    
    printf("----------------------------------------\n");
    printf("ALL TESTS PASSED!\n\n");
    return 0;
}
```

## Code style
Khi viết mã nguồn trong `project-artifactspace/`, cần tuân thủ các nguyên tắc về phong cách mã nguồn sau để đảm bảo tính nhất quán và dễ đọc:
1. **Đặt tên biến và hàm:**
  - Sử dụng kiểu đặt tên `snake_case` cho biến và hàm.
  - Tên biến và hàm nên có ý nghĩa rõ ràng, phản ánh chức năng hoặc mục đích sử dụng.
2. **Cấu trúc thanh ghi:**
  - Cấu trúc thanh ghi nên được định nghĩa rõ ràng với các trường hợp sử dụng cụ thể.
  - BIT_NAME viết hoa toàn bộ.
  - Sử dụng dấu gạch dưới (_) để phân tách các từ trong tên BIT_NAME.
  - BIT_WIDTH là số nguyên biểu thị số bit của trường.
  - Ưu tiên sử dụng các từ khóa rút gọn được định nghĩa trong `lib_keyword_def.h` để đảm bảo tính nhất quán.
    ```c
    tdf_strc IWDG_KR_REG {
		__vo ui KEY : 16;
		__vo ui RESERVED : 16;
	} IWDG_KR_REG_Typedef;
    ```
  - Cấu trúc thanh ghi theo mẫu:
    ```c
    tdf_strc <REGISTER_NAME>_REG {
		__vo ui <BIT_NAME> : <BIT_WIDTH>;
	} <REGISTER_NAME>_REG_Typedef;
    ```
    Cấm sử dụng loại sau:
    ```c
    tdf_strc <REGISTER_NAME>_REG 
    {
		__vo ui <BIT_NAME> : <BIT_WIDTH>;
	} <REGISTER_NAME>_REG_Typedef;
    ```
3. **Định dạng mã nguồn:**
  - Với mỗi khoảng trống giữa 2 comment dạng `//` thì thiết kế như sau:
    ```c
    // Comment dòng 1

      /*
        Code block cách ra 1 dòng trống so với comment dòng 1 và comment dòng 2
      */

    // Comment dòng 2
    ```
  - Nếu tham số truyền vào hàm quá dài, cần ngắt dòng sao cho mỗi tham số nằm trên một dòng riêng biệt và thụt vào 1 tab so với tên hàm.
    ```c
    function_name(
      parameter_1,
      parameter_2,
      parameter_3
    ) {
      // Function body
    }
    ```
  - Sử dụng khoảng trắng hợp lý để tăng tính dễ đọc (ví dụ: sau dấu phẩy, trước và sau toán tử).

# Hướng dẫn viết comment trong mã nguồn
Khi viết comment trong mã nguồn của `project-artifactspace/`, cần tuân thủ các nguyên tắc sau để đảm bảo tính nhất quán và dễ hiểu:
1. **Loại comment sử dụng:**
  - Sử dụng comment dạng `/* ... */` cho các mô tả hàm, cấu trúc dữ liệu và phần giải thích phức tạp.
  - Sử dụng comment dạng `//` cho các ghi chú ngắn gọn, chú thích dòng và giải thích logic đơn giản.
2. **Vị trí đặt comment:**
  - Đặt comment ngay trước phần mã mà nó giải thích hoặc mô tả.
  - Đối với comment dòng, đặt nó ở cuối dòng mã nếu nó ngắn gọn và không làm rối mắt.
3. **Nội dung comment:**
  - Comment nên rõ ràng, ngắn gọn và tập trung vào việc giải thích "tại sao" thay vì "cái gì".
  - Tránh viết comment thừa thãi hoặc lặp lại những gì mã đã thể hiện rõ ràng.
4. **Cấu trúc comment hàm:**
  - Mỗi hàm nên có một comment mô tả chức năng, tham số đầu vào và giá trị trả về (nếu có) theo mẫu sau:
    ```c
    /*
     * Hàm khởi tạo ngoại vi IWDG.
     *
     * Tham số:
     *   prescaler - Giá trị chia tần số.
     *   reload - Giá trị nạp lại.
     *
     * Trả về:
     *   Không có.
     */
    void function_name(type parameter_1, type parameter_2) {
      // Body của hàm
    }
    ```
5. **Cấu trúc comment cho cấu trúc dữ liệu:**
  - Mỗi cấu trúc dữ liệu nên có một comment mô tả mục đích và các trường bên trong theo mẫu sau:
    ```c
    /*
     * Cấu trúc thanh ghi IWDG_KR.
     *
     * Trường:
     *   KEY - Mã khóa.
     *   RESERVED - Dự phòng.
     */
    tdf_strc IWDG_KR_REG {
      __vo ui KEY : 16;
      __vo ui RESERVED : 16;
    } IWDG_KR_REG_Typedef;
    ```

# Hướng dẫn về quản lý lỗi và logging
Khi quản lý lỗi và logging trong mã nguồn của `project-artifactspace/`, cần tuân thủ các nguyên tắc sau để đảm bảo tính nhất quán và hiệu quả trong việc theo dõi và xử lý lỗi:
1. **Cách thức quản lý lỗi:**
  - Sử dụng các mã lỗi (error codes) để biểu thị các loại lỗi khác nhau.
  - Mỗi hàm nên trả về một mã lỗi để cho biết trạng thái thực thi (thành công hoặc thất bại).
  - Định nghĩa các mã lỗi trong một tệp header riêng biệt để dễ dàng quản lý và sử dụng lại, đã có trong `lib_keyword_def.h`.
2. **Cách thức logging:**
  - Sử dụng các hàm logging để ghi lại thông tin về trạng thái hệ thống như `printf(...)` hoặc các hàm logging tùy chỉnh.
  - Ghi lại các thông tin quan trọng như lỗi, cảnh báo và thông tin trạng thái hệ thống.
  - Định dạng thông điệp logging nên rõ ràng và dễ hiểu, bao gồm thời gian, mức độ nghiêm trọng và mô tả chi tiết.
3. **Mức độ logging:**
  - Định nghĩa các mức độ logging như DEBUG, INFO, WARNING, ERROR để phân loại thông điệp.
  - Chỉ ghi lại các thông điệp phù hợp với mức độ hiện tại của hệ thống để tránh quá tải thông tin.

# Quy tắc về Review mã nguồn
Quy tắc review mã nguồn này nhằm đảm bảo rằng tất cả các thay đổi mã nguồn trong dự án đều được kiểm tra kỹ lưỡng, nhất quán và tuân thủ các tiêu chuẩn đã đề ra. Việc tuân thủ các hướng dẫn này sẽ giúp cải thiện chất lượng mã nguồn, giảm thiểu lỗi và hỗ trợ làm việc nhóm hiệu quả hơn.

## Giới thiệu
Quy trình review mã nguồn (code review) là một bước quan trọng trong quá trình phát triển phần mềm, giúp đảm bảo rằng mã nguồn được viết ra đáp ứng các tiêu chuẩn chất lượng và phù hợp với yêu cầu dự án. Quy trình này thường bao gồm việc xem xét mã nguồn bởi các thành viên khác trong nhóm phát triển trước khi mã được hợp nhất vào nhánh chính của dự án.

Ở đây, Github Copilot sẽ là người thực hiện review mã nguồn cục bộ dựa trên các tiêu chuẩn đã được thiết lập trong tài liệu này. Tuy nhiên, các đề xuất của Copilot cần được con người xem xét và phê duyệt trước khi áp dụng vào mã nguồn chính thức.

## Tiêu chí review
Khi thực hiện review mã nguồn, cần chú ý đến các tiêu chí sau:
1. **Tuân thủ tiêu chuẩn mã nguồn:** Đảm bảo rằng mã nguồn tuân thủ các tiêu chuẩn về phong cách mã nguồn, cấu trúc thư mục và quy trình làm việc đã được thiết lập trong dự án.
2. **Tính rõ ràng và dễ hiểu:** Mã nguồn nên được viết một cách rõ ràng, dễ đọc và dễ hiểu. Các biến, hàm và cấu trúc dữ liệu nên có tên rõ ràng và mô tả đầy đủ.
3. **Hiệu suất và tối ưu hóa:** Đánh giá mã nguồn về hiệu suất và tối ưu hóa, đảm bảo rằng mã không gây ra các vấn đề về hiệu suất hoặc sử dụng tài nguyên không cần thiết.
4. **Hoạt động đúng chức năng:** Kiểm tra xem mã nguồn có thực hiện đúng chức năng như mong đợi hay không, bao gồm cả việc xử lý các trường hợp ngoại lệ và lỗi.
5. **Bảo mật:** Đánh giá mã nguồn về các vấn đề bảo mật tiềm ẩn, đảm bảo rằng mã không chứa các lỗ hổng bảo mật hoặc các thực hành không an toàn.
6. **Kiểm thử:** Đảm bảo rằng mã nguồn đi kèm với các bài kiểm thử phù hợp và các bài kiểm thử này đã được thực hiện và vượt qua thành công.

# Tiêu chuẩn thiết kế CMakeList.txt
Tiêu chuẩn thiết kế CMakeList.txt này nhằm đảm bảo rằng các tệp CMakeLists.txt trong dự án đều được viết một cách nhất quán, rõ ràng và dễ hiểu. Việc tuân thủ các hướng dẫn này sẽ giúp cải thiện khả năng quản lý dự án, giảm thiểu lỗi và hỗ trợ làm việc nhóm hiệu quả hơn.

## Giới thiệu
CMake là một công cụ quản lý xây dựng mã nguồn mở, giúp tự động hóa quá trình biên dịch và liên kết mã nguồn thành các tệp thực thi hoặc thư viện. Tệp CMakeLists.txt là nơi chứa các lệnh và cấu hình cần thiết để CMake hiểu cách xây dựng dự án.

## Cấu trúc tệp CMakeLists.txt
Khi thiết kế tệp CMakeLists.txt, cần tuân thủ cấu trúc sau:
```txt
# Cấu trúc các tệp nguồn và thư viện như sau
<Khai báo các tệp sử dụng trong dự án>

# Cấu trúc lệnh GCC ban đầu:
<
  Khai báo lệnh GCC ban đầu sử dụng trong dự án
  Ví dụ mẫu dưới đây:
  # gcc 
  # -DUNIT_TEST 
  # -I ..\..\Inc\generic\ 
  # -I ..\..\Inc\iwdg\ 
  # -I ..\..\Inc\clock\ 
  # -I ..\iwdg\ 
  # -g -o .\unit_test_implementation 
  # -o .\unit_test_implementation.exe 
  #    .\unit_test_implementation.c 
  #    .\source_dependency.c 
  #    ..\..\Impl\lib_iwdg_impl.c
>

# Khai báo phiên bản CMake tối thiểu
cmake_minimum_required(VERSION <Phiên_bản_CMake_tối_thiểu>) 

# Khai báo tên dự án và ngôn ngữ lập trình
project(<Tên dự án> C)

# Định nghĩa các file nguồn .c sử dụng trong dự án
set(SOURCES
  <Liệt kê các tệp nguồn .c sử dụng trong dự án>
)

# Cấu hình để luôn tạo ra file Debug (tương đương flag -g)
set(CMAKE_BUILD_TYPE Debug)

# Tạo mục tiêu thực thi
add_executable(${PROJECT_NAME} ${SOURCES})

# Định nghĩa các Macro (tương đương flag -D)
target_compile_definitions(${PROJECT_NAME} PRIVATE <Liệt kê các Macro cần định nghĩa>)

# Cấu hình các đường dẫn Include (tương đương các tham số -I)
target_include_directories(${PROJECT_NAME} PRIVATE
  <Liệt kê các đường dẫn Include cần cấu hình>
)
```

## Lưu ý bổ sung
1. Giữa mỗi comment phải cách ra một dòng trống so với khối lệnh bên dưới.
2. Sử dụng dấu gạch ngang `-` thay vì dấu gạch dưới `_` cho các tham số trong lệnh CMake.
3. Sử dụng dấu cách hợp lý để tăng tính dễ đọc (ví dụ: sau dấu phẩy, trước và sau toán tử).
4. Indentation sử dụng 2 spaces cho mỗi cấp độ tuân thủ cấu trúc.

## Cần chú ý
Ở thời điểm hiện tại CMake được sử dụng trong dự án nhằm phục vụ cho việc biên dịch các file kiểm thử đơn vị (unit test) trong thư mục `project-artifactspace/Test/`. Các tệp CMakeLists.txt được đặt trong từng thư mục con tương ứng với từng ngoại vi cụ thể để quản lý việc biên dịch các bài kiểm thử đơn vị cho ngoại vi đó.

Do đó việc đặt tên, tuân thủ theo nguyên tắc `<Ngoại vi, viết thường toàn bộ>_unit_test`
