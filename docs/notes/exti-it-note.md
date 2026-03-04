# Ghi chú về EXTI và IT

## IT

### Giới thiệu

Các ngắt ngoại vi chính đều được quản lý bởi NVIC, trong đó lên tới 81 ngắt khác nhau kèm theo mức ưu tiên khả lập trình từ 0-15.

Bên cạnh đó, tài liệu cũng cung cấp bổ sung thông tin về bảng vector ngắt để giúp người đọc hiểu rõ hơn về cách thức hoạt động của các ngắt và cách chúng được xử lý trong hệ thống. Bảng này liệt kê tất cả các ngắt có sẵn, cùng với mô tả chức năng và mức ưu tiên tương ứng của chúng.

Dựa theo tài liệu và các nhu cầu sử dụng của dự án, các ngoại vi được chỉ định có sử dụng ngắt bao gồm:

- Reset (ưu tiên -3, không thay đổi)
- NMI (ưu tiên -2, không thay đổi) được sử dụng bởi CSS trên driver clock.
- HardFault (ưu tiên -1, không thay đổi) để catch tất cả các lớp lỗi.
- BusFault (ưu tiên 1, thay đổi được) để catch lỗi pre-fetch và lỗi truy cập bộ nhớ.
- UsageFault (ưu tiên 2, thay đổi được) để catch lỗi liên quan đến việc sử dụng CPU, như lỗi chia cho 0 hoặc lỗi truy cập bộ nhớ không hợp lệ.
- Debug Monitor (ưu tiên 3, thay đổi được) để catch ngắt liên quan đến debug.
- WWDG (vị trí 0, ưu tiên 7, thay đổi được) để catch lỗi liên quan đến window watchdog.
- RCC (vị trí 5, ưu tiên 12, thay đổi được) để catch lỗi liên quan đến các ngắt RCC chung.
- EXTI[0:4] (vị trí 6-10, ưu tiên 13-17, thay đổi được) để catch lỗi liên quan đến các ngắt EXTI line 0 tới 4.
- EXTI[9:5] (vị trí 23, ưu tiên 30, thay đổi được) để catch lỗi liên quan đến các ngắt EXTI line 5 tới 9.
- EXTI[15:10] (vị trí 40, ưu tiên 47, thay đổi được) để catch lỗi liên quan đến các ngắt EXTI line 10 tới 15.
- I2C1_EV (vị trí 31, ưu tiên 38, thay đổi được) để catch ngắt liên quan đến sự kiện I2C1.
- I2C1_ER (vị trí 32, ưu tiên 39, thay đổi được) để catch ngắt liên quan đến lỗi I2C1.
- SPI1 (vị trí 35, ưu tiên 42, thay đổi được) để catch ngắt chung liên quan đến SPI1.
- USART1 (vị trí 37, ưu tiên 44, thay đổi được) để catch ngắt chung liên quan đến USART1.

Kiểm chứng thông tin trong [rm0008](../references/rm0008-stm32-f101-f102-f103-f105-f107-reference-manual.pdf).

### Thiết kế mapping các ngắt thường

Theo tài liệu, CMSIS cung cấp cơ chế ánh xạ trong đó chia thành 3 nhóm theo số vị trí ngắt:

- Nhóm 1: 0-31, sử dụng các thanh ghi ISER0, ICER0, ISPR0, ICPR0, IABR0.
- Nhóm 2: 32-63, sử dụng các thanh ghi ISER1, ICER1, ISPR1, ICPR1, IABR1.
- Nhóm 3: 64-95, sử dụng các thanh ghi ISER2, ICER2, ISPR2, ICPR2, IABR2.

Đối với thanh ghi IPR, CMSIS thiết kế theo ý tưởng xếp chồng từng 8-bit thay thế cho 21 thanh ghi IPR0-20, tạo thành một mảng IPR[0:67] để quản lý mức ưu tiên của tất cả các ngắt. Điều này giúp đơn giản hóa việc truy cập và quản lý mức ưu tiên của các ngắt trong hệ thống.

Tuy nhiên, nếu không sử dụng thiết kế IPR trên, ta vẫn có thể thao tác với công thức như sau:

```math
IRQPriority << ( (8 * byte_offset) + (8 - NOT_IMPLEMENTED_OFFSET = 4) )
```

Trong đó, `byte_offset` là số byte cần dịch chuyển để đến đúng vị trí của ngắt trong thanh ghi IPR, và `NOT_IMPLEMENTED_OFFSET` là một hằng số được sử dụng để điều chỉnh vị trí của ngắt trong trường hợp có những ngắt không được triển khai hoặc không có sẵn trên vi điều khiển cụ thể. Công thức này giúp xác định chính xác vị trí của ngắt trong thanh ghi IPR để thiết lập mức ưu tiên một cách chính xác.

Nếu sử dụng theo thiết kế IPR trên, ta có thể truy cập trực tiếp vào mảng IPR[0:67] để thiết lập mức ưu tiên cho các ngắt mà không cần phải tính toán byte offset, giúp đơn giản hóa quá trình quản lý ngắt trong hệ thống.

### Thiết kế mapping các ngắt hệ thống

Các ngắt hệ thống như Reset, NMI, HardFault, được thiết kế với mức ưu tiên âm (-3 đến -1) để đảm bảo rằng chúng luôn được xử lý trước các ngắt ngoại vi khác. Điều này giúp đảm bảo rằng các lỗi nghiêm trọng và các sự kiện quan trọng luôn được ưu tiên xử lý ngay lập tức, giảm thiểu nguy cơ.

Tuy nhiên, vẫn có 1 số ngắt đặc biệt như Memory Management, BusFault, UsageFault, Debug Monitor được thiết kế với mức ưu tiên dương (1 đến 3) để cho phép người dùng có thể điều chỉnh mức ưu tiên của chúng nếu cần thiết. Điều này cung cấp sự linh hoạt trong việc quản lý ngắt hệ thống, cho phép người dùng tùy chỉnh mức ưu tiên của các ngắt này dựa trên yêu cầu cụ thể của ứng dụng hoặc hệ thống mà họ đang phát triển.

Theo tài liệu, CMSIS đề xuất hướng thiết kế tối giản hoạt động thông qua việc sử dụng mảng SHP[0:12] thay thế cho nhóm thanh ghi SHPR1-3, giúp đơn giản hóa việc quản lý mức ưu tiên của các ngắt hệ thống. Mảng SHP này cho phép người dùng dễ dàng truy cập và thiết lập mức ưu tiên cho các ngắt hệ thống mà không cần phải lo lắng về việc tính toán byte offset hoặc vị trí cụ thể của từng ngắt trong thanh ghi SHPR. Điều này giúp tăng tính linh hoạt và hiệu quả trong việc quản lý ngắt hệ thống trong các ứng dụng sử dụng CMSIS.

### Lưu ý

Trong tất cả các thiết kế mapping này, tất cả các ngắt đều được thiết kế với yêu cầu truy cập có quyền ưu tiên (privileged access) để đảm bảo rằng chỉ có các phần của hệ thống có quyền truy cập cao mới có thể thiết lập hoặc thay đổi mức ưu tiên của các ngắt, giúp tăng cường bảo mật và ổn định của hệ thống. Điều này đảm bảo rằng các ngắt quan trọng và các sự kiện hệ thống luôn được xử lý một cách an toàn và hiệu quả, giảm thiểu nguy cơ bị tấn công hoặc lỗi do việc thiết lập mức ưu tiên không đúng cách.

Trong đó cần làm rõ:

Unprivilieged được định nghĩa là phần mềm:

- Quyền truy cập hạn chế đối với lệnh MSR, MRS, CPS.
- Không được phép truy cập vào Sys-Tim, NVIC hoặc SCB.
- Có thể giới hạn quyền truy cập đối với các ngoại vi và bộ nhớ.

Privileged được định nghĩa là phần mềm:

- Có thể truy cập đầy đủ vào tất cả các lệnh và truy cập và tất cả tài nguyên.

Ở Thread mode, Để kiểm tra thì xem trong nhóm thanh ghi CONTROL để xác định việc xử lý phần mềm đang ở chế độ privileged hay unprivileged.

Ở Handler mode, phần mềm luôn ở chế độ privileged. Điều này đảm bảo rằng các ngắt và các sự kiện hệ thống luôn được xử lý với quyền truy cập đầy đủ, giúp tăng cường bảo mật và ổn định của hệ thống.

Chỉ có phần mềm được quyền mới có thể ghi vào nhóm thanh ghi CONTROL để thay đổi quyền hạn thực thi phần mềm ở Thread mode, giúp đảm bảo rằng chỉ có các phần của hệ thống có quyền truy cập cao mới có thể thay đổi quyền hạn của phần mềm, tăng cường bảo mật và ổn định của hệ thống.

Để thực hiện quyền ưu tiên cần dùng lệnh SVC để chuyển quyền hoạt động sang chế độ privileged, sau đó mới có thể thiết lập mức ưu tiên cho các ngắt hoặc thực hiện các thao tác yêu cầu quyền truy cập cao khác. Điều này đảm bảo rằng chỉ có phần mềm được quyền mới có thể thực hiện các thao tác quan trọng liên quan đến quản lý ngắt và hệ thống, giúp tăng cường bảo mật và ổn định của hệ thống.

Ghi chú: Theo tìm hiểu thì ARM Cortex-M3 chia ra hoạt động ở 2 chế độ là Thread mode và Handler mode, trong đó khi khởi tạo chương trình và dự án thì phần mềm sẽ hoạt động ở Thread mode với quyền ưu tiên privileged sẵn có thể truy cập đầy đủ vào tất cả các lệnh và tài nguyên. Tuy nhiên, khi có ngắt xảy ra, phần mềm sẽ chuyển sang Handler mode để xử lý ngắt, trong đó phần mềm luôn ở chế độ privileged để đảm bảo rằng các ngắt và các sự kiện hệ thống luôn được xử lý với quyền truy cập đầy đủ, giúp tăng cường bảo mật và ổn định của hệ thống.

Kiểm chứng thông tin trong 2 nguồn sau:

- [pm0056](../references/pm0056-stm32-f10-f20-f21-l1-cortex-m3-programming-manual.pdf)
- [arm-access-level](https://www.iotality.com/armcm-access-levels/)
