# Ghi chú về thiết kế hệ thống

## Giới thiệu

Ghi chú này tổng hợp các thiết kế hệ thống của dự án nhằm đảm bảo hoạt động ổn định và hiệu quả.

## Ý tưởng thiết kế

Thiết kế sử dụng sẽ lấy ý tưởng từ tài liệu [paper](../references/STM32-Based%20Home%20Automation%20and%20Energy%20Monitoring%20System%20with%20TFT%20Display.pdf) trang 6 nhằm đảm bảo tính ổn định và hiệu quả của hệ thống đã được kiểm chứng.

## Thay đổi thiết kế tương ứng

Từ thiết kế ban đầu, đã có một số thay đổi để phù hợp với yêu cầu của dự án:

- Thay màn TFT thành màn LCD 16x2 để giảm chi phí và đơn giản hóa giao diện người dùng.
- Bổ sung thêm tính năng Watchdog để đảm bảo hệ thống hoạt động ổn định và tự phục hồi khi gặp sự cố.
- Nâng cấp thêm khả năng cập nhật firmware qua UART sử dụng giao thức XMODEM, giúp việc bảo trì và nâng cấp hệ thống dễ dàng hơn trong môi trường thực tế. Hoặc sử dụng một thẻ microSD chứa firmware mới để cập nhật, giúp việc bảo trì và nâng cấp hệ thống dễ dàng hơn trong môi trường thực tế.
- Bổ sung nút điều khiển để người dùng có thể tương tác trực tiếp với thiết bị, ví dụ như start/stop đo lường hoặc reset thiết bị/cập nhật firmware.
- Thay thế ADE7757 thành ADE7758 nhằm hỗ trợ đo lường cả điện áp và dòng điện, giúp hệ thống có khả năng giám sát năng lượng toàn diện hơn.
- Bổ sung module MCP4728 để chuyển đổi điện 1 pha thành 3 pha, giúp hệ thống có khả năng giám sát năng lượng trên lưới điện công nghiệp giả lập, mở rộng phạm vi ứng dụng của hệ thống.

Trong tài liệu, nhóm tác giả thiết kế hệ thống nhằm sử dụng cho mục đích đo lường ở phạm vi mạng lưới điện dân dụng. Tuy nhiên, để sử dụng được trong môi trường công nghiệp, cần phải bổ sung thêm MCP4728 để đưa điện 1 pha vào hệ thống và chuyển đổi thành 3 pha để đo lường, giúp hệ thống có khả năng giám sát năng lượng trên lưới điện công nghiệp giả lập.
