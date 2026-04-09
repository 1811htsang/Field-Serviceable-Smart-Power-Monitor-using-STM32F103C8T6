# Requirement Analysis

Ghi chú này dựa trên các tài liệu tổng hợp ý tưởng thiết kế của [sys-des](/docs/notes/system-design-note.md) nhằm trình bày thống nhất các yêu cầu mà dự án đặt ra để đảm bảo hoạt động ổn định và hiệu quả của hệ thống.

## User Requirements

### Object

Đối tượng được xác định trong đề tài này chính là dữ liệu có được từ việc thu thập trên mạng lưới điện 3 pha trong công nghiệp.

Dữ liệu này có thể bao gồm các thông số như điện áp, dòng điện, tần số, và các đặc trưng khác liên quan đến hoạt động của hệ thống điện 3 pha nhằm cung cấp giải pháp:

- Giám sát năng lượng hiệu quả và chính xác.
- Có khả năng cập nhật logic đo đạc và hiệu chuẩn tùy thuộc vào từng môi trường và điều kiện hoạt động cụ thể.

### Input

Tín hiệu điện vật lý là nguồn điện 3 pha được thu thập thông qua các cảm biến và thiết bị đo đạc được kết nối với hệ thống. Dữ liệu này sẽ được truyền vào hệ thống để xử lý và phân tích nhằm đưa ra các dự đoán về tình trạng của hệ thống điện 3 pha.

Ngoài tín hiệu điện, lệnh điều khiển từ người dùng có thể cho phép tương tác với hệ thống để thực hiện các tác vụ như hiệu chỉnh, cấu hình, hoặc truy xuất dữ liệu đã được thu thập và phân tích.

### Output

Dữ liệu được trích xuất chính là giá trị hiệu dụng của áp, dòng, công suất tác dụng, phản kháng, biểu kiến, hệ số công suất và tần số của hệ thống điện 3 pha. Dữ liệu này sẽ được sử dụng để đánh giá và dự đoán tình trạng của hệ thống điện, giúp người dùng hoặc hệ thống tự động có thể đưa ra các biện pháp bảo trì hoặc điều chỉnh kịp thời để đảm bảo hoạt động ổn định và hiệu quả của hệ thống điện 3 pha.

## Functional Requirements

### Performance

Hệ thống được kỳ vọng hoạt động ở các thông số sau:

- Độ chính xác đạt tiêu chuẩn class 0.5S theo IEC 62053-22, đảm bảo rằng hệ thống có thể đo đạc và phân tích dữ liệu điện 3 pha với độ chính xác cao, giúp cung cấp thông tin đáng tin cậy cho việc giám sát và quản lý hệ thống điện.
- Cho phép xử lý ghi nhận dữ liệu từ cảm biến với độ trễ dưới 1ms theo cơ chế quản lý ngắt ưu tiên NVIC.
- Hỗ trợ đọc dữ liệu dạng sóng qua DMA để phục vụ phân tích sóng hài.
- Đảm bảo SPI đạt tốc độ ổn định ở mức 2.25MHz và UART có thể hỗ trợ XMODEM-CRC để truyền dữ liệu hiệu quả và đáng tin cậy giữa các thành phần của hệ thống.

### Operation Mode

Hệ thống sẽ hoạt động 2 chế độ chính:

- Chế độ bình thường - Normal mode: Trong chế độ này, thực hiện giám sát liên tục 24/7 và thực hiện sao lưu log định kỳ.
- Chế độ bảo vệ - Protection mode: Trong chế độ này, hệ thống sẽ tự động chuyển sang chế độ bảo vệ khi phát hiện các điều kiện bất thường hoặc nguy hiểm trong hệ thống điện 3 pha, giúp ngăn ngừa sự cố và bảo vệ thiết bị khỏi hư hỏng nghiêm trọng. Chế độ này có thể bao gồm việc tắt nguồn hoặc giảm tải để đảm bảo an toàn cho hệ thống điện và người dùng.
- Chế độ hiệu chỉnh - Calibration mode: Trong chế độ này, hệ thống sẽ được hiệu chỉnh để đảm bảo rằng các cảm biến và thiết bị thu thập dữ liệu hoạt động chính xác và cung cấp dữ liệu đáng tin cậy. Quá trình hiệu chỉnh có thể bao gồm việc điều chỉnh các tham số của cảm biến, kiểm tra và xác nhận chất lượng dữ liệu thu thập được, và đảm bảo rằng hệ thống sẵn sàng để hoạt động trong môi trường thực tế.
- Chế độ cập nhật - Update mode: Trong chế độ này, hệ thống sẽ cho phép cập nhật logic đo đạc và hiệu chuẩn tùy thuộc vào từng môi trường và điều kiện hoạt động cụ thể, giúp hệ thống có thể thích ứng với các thay đổi trong môi trường hoạt động và đảm bảo rằng dữ liệu thu thập được luôn chính xác và đáng tin cậy.

### Data Management

Hệ thống được kỳ vọng thiết kế và quản lý lưu trữ thông tin như sau:

- RTC Timestamp để đánh dấu thời gian chính xác của mỗi bản ghi dữ liệu, giúp theo dõi và phân tích dữ liệu theo thời gian một cách hiệu quả.
- Có cơ chế để quy định sử dụng Backup Registers để lưu trữ các thông tin quan trọng như trạng thái hệ thống, cấu hình, hoặc dữ liệu tạm thời cần thiết cho quá trình hoạt động của hệ thống, giúp đảm bảo rằng các thông tin này được bảo vệ và có thể truy xuất khi cần thiết.
- Thiết kế phân cấp rõ dung lượng sử dụng cho phần boot và phần lưu trữ chương trình đo đạc.

### Security

Hệ thống được kỳ vọng có các biện pháp bảo mật như TAMPER, MPU để chống lại các hành vi xâm nhập và bảo vệ dữ liệu, giúp đảm bảo rằng hệ thống có thể hoạt động an toàn và đáng tin cậy trong môi trường công nghiệp và bảo vệ thông tin quan trọng khỏi các mối đe dọa tiềm ẩn.

Bên cạnh đó, hệ thống cũng cần có khả năng phát hiện lỗi bên trong chính nó với CRC và có cơ chế tự reset bản thân với IWDG & WWDG nếu gặp lỗi phần cứng và phần mềm, giúp đảm bảo rằng hệ thống có thể hoạt động ổn định và đáng tin cậy trong thời gian dài mà không bị gián đoạn do các sự cố phần cứng hoặc phần mềm.

## Non-Functional Requirements

### Environmental

Hệ thống được kỳ vọng hoạt động ổn định trong môi trường có biến tần và động cơ lớn gây nhiễu điện từ mạnh. Điều này yêu cầu hệ thống phải được thiết kế để có khả năng chống nhiễu điện từ và đảm bảo rằng dữ liệu thu thập được không bị ảnh hưởng bởi các yếu tố nhiễu từ môi trường, giúp cung cấp thông tin đáng tin cậy cho việc giám sát và quản lý hệ thống điện 3 pha.

Đảm bảo thiết kế hệ thống có khả năng bảo vệ ESD và có cách ly quang để ngăn chặn các sự cố do nhiễu điện từ và đảm bảo an toàn cho hệ thống và người dùng.

### Power

Hệ thống được kỳ vọng hoạt động liên tục trong thời gian dài với nguồn điện ổn định, có khả năng tự reset bản thân với IWDG & WWDG nếu gặp lỗi phần cứng và phần mềm.

Ngoài ra nếu xảy ra trường hợp reset thì đảm bảo luôn lưu được trạng thái lỗi cũng như dữ liệu cuối cùng trước khi reset để có thể phân tích và khắc phục sự cố một cách hiệu quả, giúp đảm bảo rằng hệ thống có thể hoạt động ổn định và đáng tin cậy trong thời gian dài mà không bị gián đoạn do các sự cố phần cứng hoặc phần mềm.

Sử dụng các chế độ tiết kiệm năng lượng như Sleep mode, Stop mode, và Standby mode để giảm tiêu thụ năng lượng khi hệ thống không hoạt động hoặc ở trạng thái chờ, giúp kéo dài tuổi thọ pin và đảm bảo rằng hệ thống có thể hoạt động hiệu quả về mặt năng lượng trong thời gian dài.

## Constraints

Phần cứng được thiết kế giới hạn theo tiêu chuẩn của kit STM32F103C8T6 (64KB Flash + 20KB RAM).

Phần mềm đảm bảo tuân thủ thiết kế theo kiến trúc Bare-metal thuần túy để tối ưu dung lượng & tốc độ.

Việc kiểm chứng và đánh giá hiệu suất được ưu tiên với việc sử dụng MATLAB/Simulink để mô phỏng và phân tích dữ liệu cho SIL test, thiết bị mô phỏng tín hiệu điện 3 pha sử dụng MCP4728 cho HIL test, và sử dụng các công cụ như Logic Analyzer để phân tích giao tiếp SPI và UART nhằm đảm bảo rằng hệ thống hoạt động đúng theo thiết kế và đáp ứng các yêu cầu về hiệu suất và độ chính xác đã đề ra.

## Features

Có thể tuân theo lộ trình nâng cấp hỗ trợ cho giao thức Modbus RTU để mở rộng khả năng giao tiếp và tích hợp với các hệ thống giám sát và quản lý công nghiệp khác, giúp hệ thống có thể hoạt động hiệu quả trong môi trường công nghiệp và cung cấp thông tin đáng tin cậy cho việc giám sát và quản lý hệ thống điện 3 pha.
