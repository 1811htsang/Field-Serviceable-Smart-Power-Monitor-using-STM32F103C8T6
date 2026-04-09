# Ghi chú về SPI

## Giới thiệu

SPI (Serial Peripheral Interface) là giao diện nối tiếp đồng bộ, cho phép truyền thông nối tiếp giữa vi điều khiển và các thiết bị ngoại vi hoặc giữa các vi điều khiển với nhau.

Hỗ trợ truyền nhận toàn song công (full-duplex) hoặc bán song công (half-duplex/simplex).

Có thể cấu hình làm Master (thiết bị chủ, cung cấp xung nhịp) hoặc Slave (thiết bị tớ).

## Tính năng của SPI trên STM32F103C8T6

Baud rate Master tối đa đạt $$f_{PCLK}/2$$ . Có 8 bộ chia prescaler cho Master.

Hỗ trợ khung dữ liệu 8-bit hoặc 16-bit.

Có thể lập trình gửi bit MSB (bit trọng số lớn nhất) trước hoặc LSB trước.

Tích hợp bộ tính toán và kiểm tra mã CRC bằng phần cứng để đảm bảo độ tin cậy.

Hỗ trợ chế độ đa chủ (Multimaster).

Hỗ trợ DMA (Direct Memory Access) để giảm tải cho CPU.

## Cấu hình chân SPI

- MISO (Master In / Slave Out): Truyền dữ liệu từ Slave về Master.
- MOSI (Master Out / Slave In): Truyền dữ liệu từ Master đến Slave.
- SCK (Serial Clock): Chân xung nhịp đồng bộ do Master điều khiển.
- NSS (Slave Select): Chân chọn thiết bị Slave.
- Quản lý phần mềm (SSM=1): Trạng thái NSS được quyết định bởi bit SSI trong thanh ghi `SPI_CR1`.
- Quản lý phần cứng (SSM=0):
  - NSS Output: Master giữ chân thấp khi truyền thông.
  - NSS Input: Slave bị chọn khi chân này ở mức thấp.

## Pha xung và cực xung

Người dùng có thể chọn 4 mối quan hệ thời gian thông qua bit CPOL và CPHA trong thanh ghi SPI_CR1:

- CPOL (Clock Polarity): Quy định mức logic của SCK khi nghỉ.
  - CPOL = 0: SCK ở mức thấp khi không có dữ liệu.
  - CPOL = 1: SCK ở mức cao khi không có dữ liệu.
- CPHA (Clock Phase): Quy định cạnh xung nhịp nào sẽ bắt (capture) dữ liệu.
  - CPHA = 0: Dữ liệu được chốt ở cạnh đầu tiên của chu kỳ xung nhịp.
  - CPHA = 1: Dữ liệu được chốt ở cạnh thứ hai của chu kỳ xung nhịp.

## Định dạng khung dữ liệu

- Kích thước khung: Được cấu hình qua bit DFF trong `SPI_CR1`. Chọn 0 cho khung 8-bit hoặc 1 cho khung 16-bit.
- Thứ tự truyền: Cấu hình qua bit LSBFIRST. Chọn 0 để truyền MSB trước, chọn 1 để truyền LSB trước.

## Quy trình truyền nhận dữ liệu

- Truyền: Dữ liệu được ghi vào Tx Buffer thông qua thanh ghi `SPI_DR`. Khi cờ TXE (Transmit buffer empty) được đặt, dữ liệu chuyển vào Shift Register để đẩy ra chân MOSI (Master) hoặc MISO (Slave).
- Nhận: Dữ liệu từ chân vào Shift Register và chuyển sang Rx Buffer. Khi quá trình hoàn tất, cờ RXNE (Receive buffer not empty) được đặt để báo hiệu CPU/DMA có thể đọc dữ liệu từ SPI_DR.
- Lưu ý: Trong chế độ toàn song công (full-duplex), việc truyền và nhận diễn ra đồng thời.

## Cờ trạng thái và báo lỗi SPI

Các trạng thái chính nằm trong thanh ghi SPI_SR:

- TXE (Transmit buffer empty): Bằng 1 khi bộ đệm truyền trống.
- RXNE (Receive buffer not empty): Bằng 1 khi có dữ liệu mới trong bộ đệm nhận.
- BSY (Busy flag): Bằng 1 khi SPI đang thực hiện truyền thông hoặc bộ đệm truyền không trống.
- OVR (Overrun): Lỗi xảy ra khi dữ liệu mới nhận được trong khi dữ liệu cũ chưa được đọc.
- MODF (Mode Fault): Xảy ra khi thiết bị đang làm Master nhưng chân NSS bị kéo xuống thấp (xung đột Master).
- CRCERR: Cờ báo lỗi khi giá trị CRC nhận được không khớp với giá trị tính toán.

## Ngắt SPI

Các sự kiện có thể kích hoạt ngắt (cấu hình qua SPI_CR2):

- TXEIE: Ngắt khi bộ đệm truyền trống.
- RXNEIE: Ngắt khi bộ đệm nhận có dữ liệu.
- ERRIE: Ngắt khi xảy ra các lỗi (OVR, MODF, CRCERR).

## Nguyên tắc vô hiệu hóa SPI

Để tránh làm hỏng dữ liệu đang truyền dở, cần tuân thủ:

- Đợi RXNE = 1 để nhận byte cuối cùng.
- Đợi TXE = 1.
- Đợi BSY = 0.
- Tắt SPI bằng cách xóa bit SPE trong thanh ghi SPI_CR1.

## Tích hợp DMA với SPI

- Cho phép truyền dữ liệu tốc độ cao mà không cần can thiệp liên tục từ CPU.
- TXDMAEN: Khi được bật, yêu cầu DMA được tạo ra mỗi khi cờ TXE lên 1.
- RXDMAEN: Khi được bật, yêu cầu DMA được tạo ra mỗi khi cờ RXNE lên 1.
- Trong chế độ truyền nhận bằng DMA, CPU chỉ cần xử lý khi nhận được tín hiệu hoàn tất từ bộ điều khiển DMA.

## Nguyên tắc cấu hình SPI slave

1. Thiết lập DFF (8/16-bit).
2. Chọn CPOL và CPHA đồng bộ với Master.
3. Thiết lập khung dữ liệu (MSB/LSB first).
4. Cấu hình chân NSS (phần cứng hoặc phần mềm).
5. Xóa bit MSTR trong SPI_CR1 để chọn chế độ Slave.
6. Bật bit SPE để kích hoạt ngoại vi.

## Nguyên tắc cấu hình SPI master

1. Chọn tốc độ truyền qua bit BR[2:0] trong SPI_CR1.
2. Thiết lập CPOL và CPHA.
3. Thiết lập DFF và LSBFIRST.
4. Cấu hình quản lý chân NSS (dùng bit SSOE nếu cần xuất tín hiệu chọn Slave).
5. Đặt bit MSTR và bit SPE trong SPI_CR1.
6. Bắt đầu truyền thông bằng cách ghi dữ liệu vào thanh ghi SPI_DR.

## Lưu ý

Đối với cấu hình CPHA và CPOL, đảm bảo cả slave & master phải đồng bộ để tránh lỗi truyền thông. Nếu không đồng bộ, dữ liệu có thể bị đọc sai hoặc mất mát. Thông thường nếu master là vi điều khiển và slave là cảm biến thì sẽ phụ thuộc vào yêu cầu của cảm biến để chọn CPOL/CPHA phù hợp.

Đối với cấu hình SPI cho ADE7758, tài liệu xác định CPOL = 1 và CPHA = 0, nghĩa là SCK ở mức cao khi nghỉ và dữ liệu được chốt vào cạnh đầu tiên của chu kỳ xung nhịp. Điều này cần được đảm bảo trong cấu hình SPI của vi điều khiển để giao tiếp chính xác với ADE7758. Ngoài ra, ADE7758 chỉ hoạt động ở xung nhịp 5MHz đến 15MHz và chân CS giữ thấp trong suốt quá trình truyền 1 lệnh.

| Thông số   | Giá trị             | Ý nghĩa                          |
|------------|---------------------|----------------------------------|
| Mode       | Master              | STM32 điều khiển nhịp đo         |
| Direction  | 2 Lines Full Duplex | Truyền và nhận đồng thời         |
| Data Size  | 8 Bits              | ADE7758 làm việc theo byte       |
| CPOL       | High (1)            | SCLK nghỉ ở mức cao              |
| CPHA       | 1 Edge (0)          | Sample ở sườn xuống              |
| Baudrate   | < 2 MHz             | Đảm bảo ADE7758 kịp xử lý        |
| NSS        | Soft (GPIO)         | Tự điều khiển chân CS thủ công   |
