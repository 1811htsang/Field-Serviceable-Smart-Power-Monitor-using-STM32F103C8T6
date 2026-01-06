Trong STM32 chia ra làm 2 loại watchdog là Independent Watchdog (IWDG) và Window Watchdog (WWDG).

# Independent Watchdog (IWDG)
IWDG sử dụng nguồn clock riêng biệt từ LSI và luôn hoạt động kể cả khi clock chính cho cả hệ thống bị lỗi.

IWDG được sử dụng như một tiến trình độc lập ngoài chương trình chính với ràng buộc chính xác về timing thấp.

Thông tin về kiến trúc nào trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 495.

## Tính năng chính
- Đếm lùi tự do
- Sử dụng nguồn clock RC độc lập có thể hoạt động ở chế độ Standby & Stop
- Reset khi giá trị đạt 0x0000

## Mô tả hoạt động
Điều kiện khởi động:
- `IWDG_KR` được ghi giá trị 0xCCCC để khởi động IWDG

Khởi đầu hoạt động:
- Bộ đếm bắt đầu đếm lùi từ giá trị reset 0xFFF

Tình trạng dừng:
- Bộ đếm đạt giá trị `0x0000`.
- Tín hiệu reset sinh ra.

Để tránh tín hiệu reset:
- Ghi giá trị `0xAAAA` vào `IWDG_KR` để tải lại giá trị ban đầu cho bộ đếm.
- `IWDG_RLR` xác định giá trị tải lại.

## Hardware watchdog
Tính năng này được kích hoạt thông qua option bits, giúp cho IWDG luôn hoạt động ngay sau khi khởi động hệ thống, tạo ra reset trừ khi IWDG_KR được ghi bởi chương trình trước khi bộ đếm đạt giá trị `0x0000`.

## Bảo vệ quyền truy cập thanh ghi
`IWDG_PR` và `IWDG_RLR` được bảo vệ để tránh ghi nhầm. Ghi 0x5555 vào `IWDG_KR` để mở khóa.

Lưu ý rằng, bất kỳ giá trị khác `0x5555` sẽ không mở khóa được.

Ngoài ra, IWDG cũng được thiết kế bổ sung 1 thanh ghi trạng thái để chỉ ra sự cập nhật prescaler hoặc reload value đang diễn ra.

## Chế độ debug
Trong chế độ debug, bộ đếm IWDG có thể được tạm dừng hoặc tiếp tục hoạt động dựa trên cấu hình trong thanh ghi DBG_IWDG_STOP.

## Thời gian timeout
Phụ thuộc vào giá trị prescaler và bit `PR[2:0]`.

Bảng tin này có trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 495, 496.

# Window Watchdog (WWDG)
Dùng để giám sát lỗi chương trình, thường được tại ra bởi các lỗi như vòng lặp vô hạn hoặc lỗi logic.

WWDG sẽ tạo ra các reset nếu bộ đếm không được tải lại trong một khoảng thời gian xác định, trừ khi chương trình refresh nội dung của bộ đếm giảm trước khi bit T6 được xóa.

Ngoài, WWDG cũng tạo ra reset nếu 7 bit giá trị của bộ đếm giảm trong các thanh ghi kiểm soát được làm mới trước khi bộ đếm đạt giá trị cửa sổ thấp hơn.

Nghĩa là bộ đếm chỉ refresh trong một cửa sổ thời gian xác định.

Kiểm chứng thông tin trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 500.

## Tính năng chính
- Bộ đếm lùi tự do khả lập trình
- Thực hiện reset có điều kiện, cụ thể
  - Reset khi giá trị bộ đếm lùi < 0x40
  - Reset nếu bộ đếm lùi reload nằm ngoài khung cửa sổ xác định
- Tạo ngắt sớm khi bộ đếm lùi đạt giá trị 0x40

## Mô tả hoạt động
Điều kiện khởi động:
- Set bit `WDGA` để khởi động `WWDG_CR`

Điều kiện reset:
- giá trị 7 bit của bộ đếm lùi trong khoảng (`0x40` (`W[6:0]`) -> `0x3F`)
- Phần mềm làm mới giá trị với giá trị bộ đếm mới > giá trị lưu trong khung cửa sổ.
- Bit WDGA được set và bit T6 được xóa (tức là giá trị bộ đếm < `0x3F`).

Để tránh reset:
- Phần mềm phải ghi vào thanh ghi `WWDG_CR` theo các khoảng thời gian đều đặn trong quá trình hoạt động bình thường để ngăn chặn việc MCU khởi động lại. 
- Hoạt động này chỉ nên xảy ra khi giá trị bộ đếm thấp hơn giá trị của thanh ghi cửa sổ. 
- Giá trị được lưu trong thanh ghi `WWDG_CR` phải nằm trong khoảng từ `0xFF` đến `0xC0`.
- Bộ đếm lùi phải reload khi giá trị bộ đếm lùi nằm trong khoảng (`W[6:0]` -> `0x3F`).

## Kích hoạt WWDG
Sau mỗi lần reset, WWDG được vô hiệu hóa. 

Để kích hoạt WWDG, cần ghi giá trị vào thanh ghi `WWDG_CR` với bit `WDGA` được set, sau đó sẽ không thể tắt WWDG trừ khi hệ thống được reset lại.

## Kiểm soát bộ đếm
Bộ đếm lùi hoạt động tự do nên dù không kích hoạt cũng sẽ đếm lùi. 

Do đó cần lưu ý rằng, khi enable WWDG thì set bit T6 ngay sau để tránh tạo ra reset tức thời.

Các bit `T[5:0]` chứa số lần tăng đại diện cho khoảng thời gian delay trước khi bộ đếm lùi đạt giá trị `0x3F` và tạo ra reset.

Thời gian này sẽ phụ thuộc vào trạng thái prescaler khi viết vào thanh ghi `WWDG_CR`.

Thanh ghi `WWDG_CFR` dùng để chứa chặn trên của khung cửa sổ.

Thông tin chi tiết về giản đồ hoạt động cơ bản của WWDG có trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 501, 502.

## EWI - Early Wakeup Interrupt
Chức năng này cho phép tạo ra ngắt để thực hiện các procedure an toàn cụ thể hoặc data logging trước khi WWDG tạo ra reset hệ thống.

Bật chức năng này trong thanh ghi `WWDG_CFR` bằng cách set bit `EWI`.

Khi bộ đếm lùi đạt giá trị `0x40`, một ngắt được tạo ra nếu chức năng EWI được bật.

Đối với một số ứng dụng, EWI có thể sử dụng để quản lý kiểm tra hệ thống từ phần mềm hoặc khôi phục/giảm hiệu suất hệ thống nhẹ nhàng mà không tạo ra reset WWDG. Trong trường hợp này, chương trình con xử lý ngắt (ISR) tương ứng cần phải nạp lại bộ đếm WWDG để tránh việc đặt lại WWDG, sau đó kích hoạt các hành động cần thiết.

Khi không thể xử lý ngắt EWI, ví dụ như do hệ thống bị khóa trong một tác vụ có độ ưu tiên cao hơn, thì tín hiệu đặt lại WWDG cuối cùng sẽ được tạo ra.

Ngắt EWI được xóa thông qua set 0 cho bit `EWIF` trong thanh ghi `WWDG_SR`.

Kiểm chứng thông tin trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 501, 502.

## Lập trình WWDG
Công thức tính giá trị thời gian timeout của WWDG:
$$T_{\text{WWDG}}=T_{\text{PCLK1}}\times 4096 \times 2^{\text{WDGTB[1:0]}} \times (\text{T[5:0]} + 1)$$
Trong đó:
- $T_{\text{WWDG}}$ là thời gian timeout của WWDG
- $T_{\text{PCLK1}}$ là chu kỳ clock của PCLK1
- 4096 là hằng số bộ chia
- $2^{\text{WDGTB[1:0]}}$ là giá trị prescaler từ thanh ghi `WWDG_CFR`
- $\text{T[5:0]}$ là giá trị bộ đếm lùi

Ví dụ bổ sung có trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf) trang 503.

## Chế độ debug
Trong chế độ debug, bộ đếm WWDG có thể được tạm dừng hoặc tiếp tục hoạt động dựa trên cấu hình trong thanh ghi `DBG_WWDG_STOP`.