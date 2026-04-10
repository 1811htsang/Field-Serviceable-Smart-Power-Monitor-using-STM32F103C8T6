# Ghi chú về I2C

## Giới thiệu

- Định nghĩa: I2C (Inter-Integrated Circuit) là giao diện bus nối tiếp dùng để kết nối giữa vi điều khiển và các ngoại vi.
- Khả năng: Hỗ trợ đa chủ (Multimaster), kiểm soát mọi trình tự, giao thức, trọng tài (arbitration) và thời gian trên bus.
- Chế độ tốc độ:
  - Standard mode (Sm): Lên đến 100 kHz.
  - Fast mode (Fm): Lên đến 400 kHz.
- Tính tương thích: Có khả năng hoạt động với SMBus 2.0 và PMBus.

## Tính năng của I2C trên STM32F103C8T6

- Chế độ Master và Slave.
- Hỗ trợ địa chỉ 7-bit và 10-bit; có khả năng nhận diện 2 địa chỉ Slave đồng thời (Dual addressing).
- Hỗ trợ General Call (gọi tổng quát).
- Có bộ lọc nhiễu Analog (Analog noise filter).
- Có chức năng kiểm tra lỗi gói tin (PEC - Packet Error Checking).
- Hỗ trợ DMA để giảm tải cho CPU.
- Clock stretching (Kéo dài xung nhịp) để đồng bộ tốc độ giữa các thiết bị.

## Cấu hình chân I2C

- SCL (Serial Clock): Chân xung nhịp.
- SDA (Serial Data): Chân dữ liệu.
- Cấu hình vật lý: Các chân này cần được cấu hình ở chế độ Alternate Function Open-Drain.

## Lựa chọn chế độ

Ngoại vi I2C hoạt động ở 4 chế độ chính:

1. Slave transmitter (Bộ truyền tớ).
2. Slave receiver (Bộ nhận tớ).
3. Master transmitter (Bộ truyền chủ).
4. Master receiver (Bộ nhận chủ).

Mặc định thiết bị hoạt động ở chế độ Slave.

## Thiết kế hoạt động của I2C Slave TX (Truyền)

1. Nhận địa chỉ: Sau khi khớp địa chỉ, bit `ADDR` được set.
2. Xóa ADDR: Được thực hiện bằng cách đọc thanh ghi `SR1` sau đó đọc `SR2`.
3. Gửi dữ liệu: Slave gửi byte từ thanh ghi `DR` ra bus SDA.
4. Cờ trạng thái: Bit `TxE` (Data register empty) được set khi thanh ghi dữ liệu trống. Nếu phần mềm không nạp dữ liệu kịp, bit `BTF` (Byte Transfer Finished) sẽ được set và SCL bị kéo thấp (stretching) cho đến khi `DR` được ghi.

Ghi chú:

- EV3 trong tài liệu xảy ra nhằm đảm bảo khi Data từ DR sang SDA thì cờ trạng thái `TXE = 1` để phần mềm có thể nạp dữ liệu tiếp theo vào DR mà không làm khựng lại quá trình truyền.

## Thiết kế hoạt động của I2C Slave RX (Nhận)

1. Nhận địa chỉ: Tương tự Slave TX, bit `ADDR` được set khi khớp địa chỉ.
2. Nhận dữ liệu: Dữ liệu từ bus SDA vào thanh ghi `DR`.
3. Cờ trạng thái: Bit `RxNE` (Data register not empty) được set sau mỗi byte nhận được. Nếu không đọc `DR` kịp, bit `BTF` được set và SCL bị kéo thấp cho đến khi dữ liệu được đọc ra.

Ghi chú:

- EV2 cũng có chức năng tương tự EV3.

## Thiết kế hoạt động của I2C Slave khi đóng giao tiếp

- Sau khi nhận byte cuối cùng, Master gửi điều kiện Stop.
- Giao diện I2C Slave phát hiện điều kiện này và set bit `STOPF` trong thanh ghi `SR1`.
- Xóa `STOPF` bằng cách đọc `SR1` rồi ghi vào thanh ghi `CR1`.

## Thiết kế hoạt động của I2C Master TX (Truyền)

1. Khởi tạo: Gửi điều kiện Start bằng cách set bit `START` trong `CR1`. Đợi bit `SB` (Start bit) lên 1.
2. Gửi địa chỉ: Ghi địa chỉ Slave vào thanh ghi `DR`. Đợi bit `ADDR` lên 1 và xóa nó.
3. Truyền dữ liệu: Ghi dữ liệu vào `DR`. Đợi cờ `TxE` hoặc `BTF`.
4. Kết thúc: Set bit `STOP` để gửi điều kiện Stop.

Ghi chú:

- Nếu không ghi `DR` kịp khi `TXE = 1` sau đoạn truyền dữ liệu trước đó, bit `BTF` được set và SCL bị kéo thấp cho đến khi dữ liệu được ghi ra `DR` .
- EV8 cũng có chức năng tương tự EV2.
- Đối với địa chỉ 10-bit thì sẽ bổ sung packet header trước khi gửi địa chỉ.

Quá trình đóng giao tiếp:

- Đợi `BTF = 1`.
- Set bit `STOP` để gửi điều kiện Stop.
- Master tự động quay về Slave.

## Thiết kế hoạt động của I2C Master RX (Nhận)

1. Khởi tạo: Gửi điều kiện Start, đợi bit `SB`.
2. Gửi địa chỉ: Ghi địa chỉ Slave (kèm bit R/W = 1). Đợi bit `ADDR`.
3. Nhận dữ liệu: Đợi cờ `RxNE` để đọc dữ liệu từ `DR`.
4. Phản hồi: Master gửi ACK sau mỗi byte nhận được, trừ byte cuối cùng sẽ gửi NACK trước khi gửi điều kiện Stop.

Ghi chú:

- EV7 cũng có chức năng tương tự EV2.

Quá trình đóng giao tiếp nếu nhận nhiều byte:

- Đợi đến khi nhận byte thứ `N-2`.
- Khi nhận byte thứ `N-1`, Master gửi NACK bằng cách xóa bit ACK trong `CR1`, set `STOP` để gửi điều kiện Stop.
- Đợi bit `RxNE` để đọc byte cuối cùng từ `DR`.

Quá trình đóng giao tiếp nếu nhận 1 byte:

- Tại EV6, Master gửi NACK bằng cách xóa bit ACK trong `CR1`.
- Xóa cờ `ADDR` bằng cách đọc `SR1` và `SR2`.
- Set bit `STOP` để gửi điều kiện Stop.
- Đợi bit `RxNE` để đọc byte duy nhất từ `DR`.

## Nguyên tắc làm việc với SCL của Master

- Master chịu trách nhiệm tạo xung SCL thông qua thanh ghi I2C_CCR.
- Hardware tự động kiểm tra trạng thái chân SCL trên bus. Nếu SCL bị kéo thấp (bởi Slave), Master sẽ dừng đếm thời gian cho đến khi SCL được thả nổi (logic 1). Điều này đảm bảo an toàn cho tính năng Clock Stretching.

## Điều kiện chấp nhận giao tiếp của Master

- Master chỉ bắt đầu giao tiếp khi bit BUSY (trong thanh ghi `SR2`) bằng 0 (bus đang rảnh).
- Nếu `BUSY = 1`, việc set bit `START` sẽ tạo ra một điều kiện khởi động lại (ReStart) thay vì một điều kiện Start mới hoàn toàn.

## Quy trình truyền địa chỉ Slave từ Master

- Khởi động (Sự kiện EV5)
  - Hành động: Thiết lập bit `START` trong thanh ghi I2C_CR1.
  - Kiểm tra: Đợi cờ `SB` (Start Bit) trong I2C_SR1 lên 1.
  - Ý nghĩa: Xác nhận Master đã chiếm được bus thành công.
- Gửi địa chỉ vào thanh ghi dữ liệu (DR)
  - Ngay sau khi cờ `SB` được xóa (bằng cách đọc `SR1` và ghi vào `DR`), Master thực hiện gửi địa chỉ:
  - Chế độ 7-bit:
    - Ghi 1 byte vào thanh ghi `DR`.
    - Cấu trúc byte: [7 bit địa chỉ] + [bit R/W].
      - bit `R/W = 0`: Master muốn Ghi (Transmitter).
      - bit `R/W = 1`: Master muốn Đọc (Receiver).
  - Chế độ 10-bit:
    - Bước 1: Gửi Header `11110xx0` (xx là 2 bit cao của địa chỉ). Đợi cờ `ADD10`.
    - Bước 2: Gửi byte thứ hai (8 bit thấp của địa chỉ).
- Xác nhận địa chỉ (Sự kiện EV6)
  - Dấu hiệu: Cờ `ADDR` (Address sent) trong thanh ghi `SR1` được set lên 1.
  - Ý nghĩa: Slave có địa chỉ tương ứng đã phản hồi bằng một xung ACK.
  - Thao tác xóa cờ ADDR (Bắt buộc):
    - Đọc thanh ghi `SR1`.
    - Đọc thanh ghi `SR2`.
  - Lưu ý: Nếu không đọc `SR2`, xung nhịp SCL sẽ bị kéo thấp vĩnh viễn (Clock Stretching).
- Kiểm tra hướng truyền nhận (TRA bit)
  - Sau khi xóa cờ ADDR, người dùng có thể kiểm tra bit TRA trong thanh ghi `SR2` để xác nhận Master đang thực sự ở chế độ truyền hay nhận (phụ thuộc vào bit R/W đã gửi).
- Trường hợp đặc biệt (NACK)
  - Nếu không có Slave nào phản hồi (NACK), cờ `AF` (Acknowledge Failure) sẽ bật trong `SR1`. Master phải thiết lập bit `STOP` hoặc `START` lại để kết thúc hoặc thử lại.

## Quản lý thông báo lỗi giao tiếp trên I2C

Các cờ lỗi chính trong thanh ghi `SR1`:

- BERR (Bus Error): Phát hiện điều kiện Start/Stop ở vị trí không hợp lệ trong khung truyền.
- AF (Acknowledge Failure): Không nhận được bit ACK từ thiết bị nhận.
- ARLO (Arbitration Lost): Mất quyền ưu tiên trong hệ thống đa chủ.
- OVR (Overrun/Underrun): Xảy ra khi clock stretching bị tắt và dữ liệu không được xử lý kịp thời.

## Ngắt I2C

Hệ thống ngắt được chia làm 2 kênh vector:

1. it_event (Event Interrupt): Xảy ra khi có các sự kiện truyền thông thành công như `SB`, `ADDR`, `ADD10`, `STOPF`, `BTF`, `RxNE`, `TxE`.
2. it_error (Error Interrupt): Xảy ra khi có lỗi như `BERR`, `ARLO`, `AF`, `OVR`, `TIMEOUT`, `PECERR`.

## Nguyên tắc cấu hình I2C slave

1. Cấu hình xung nhịp ngoại vi trong thanh ghi `I2C_CR2` (tối thiểu 2 MHz cho Sm, 4 MHz cho Fm).
2. Thiết lập địa chỉ riêng (Own Address) trong thanh ghi `OAR1` (và `OAR2` nếu cần).
3. Bật ngoại vi bằng cách set bit `PE` (Peripheral Enable) trong `I2C_CR1`.

## Nguyên tắc cấu hình I2C master

1. Cấu hình tần số đầu vào trong thanh ghi `I2C_CR2`.
2. Tính toán và thiết lập tốc độ bus (Standard/Fast) trong thanh ghi I2C_CCR.
3. Thiết lập thời gian vọt (Maximum Rise Time) trong thanh ghi I2C_TRISE.
4. Bật bit `PE` trong `I2C_CR1`.
5. Sử dụng bit `START` để bắt đầu chiếm quyền điều khiển bus.
