# Games in ESP32 Arduino

![game](https://github.com/HungHyperX/GamesInESP32-BTLNhung/assets/136741851/aec5e780-e426-4735-95d3-63e418ca2b2e)



## Giới thiệu
&emsp;Đề tài triển khai trò chơi Flappy Bird và Dino Run trên ESP32 bằng cách sử dụng kiến trúc Arduino. Trò chơi được thiết kế để chạy trên màn hình SSD1306 Oled 1,3 inch, mang lại trải nghiệm thú vị và tương tác trên nền tảng ESP32.

Các tính năng chính bao gồm : 
- Điều khiển : Người chơi sử dụng nút bấm hoặc cảm biến đo khoảng cách để khiến nhân vật nhảy lên 
- Màn hình OLED : trò chơi được thiết kế để hiển thị trên màn hình OLED 1.3 inches. 
- Chuyển trò chơi : Có thể lựa chọn giữa 2 trò chơi Flappy Bird và Dino 
- Điểm cao : Lưu High Score của người chơi và điểm cao có thể được Reset

## Hướng dẫn cài đặt và chơi

1. Clone repository về máy.

2. Mở project trên Arduino IDE

3. Cài đặt các thư viện cần thiết bằng Arduino Library Manager

4. Cấu hình dự án bằng cách chọn board và cổng thích hợp.

5. Tải lên ESP32.

6. Chọn 1 trong 2 game: Flappy Bird hoặc Dino Run

7. Điều khiển chim  hoặc khủng long bằng cách sử dụng các nút hoặc cảm biến được chỉ định.

8. Cố gắng vượt qua các đường ống hoặc cây xương rồng và ghi càng nhiều điểm càng tốt.

Link video hướng dẫn: 
https://www.youtube.com/watch?v=JgbkK8cggGQ&t=2s

## Danh sách linh kiện

- ESP32 Development Board.
- PCB Đục Lỗ 7x9cm 2 Lớp (Phíp Thủy Tinh) 
- Màn hình SSD1306 Oled tương thích với ESP32.
- 2 Nút nhấn bốn chân
- Còi chíp 5V
- 2 Đèn LED đơn và 2 điện trở
- Cảm Biến Siêu Âm SRF04
- Mạch sạc 5V
- 1 công tắc gạt 5 chân (nhưng chỉ dùng 3 chân)
- 1 pin 3.8V

## Sơ đồ nguyên lý

![image](https://github.com/HungHyperX/GamesInESP32-BTLNhung/assets/136741851/170443e1-9d3e-4394-997e-8fea894a8980)


- Màn hình của SH1106 nối với ESP 32 (SCL nối với chân 22, SDA nối với chân 21 của ESP32).
- Nút bấm thứ nhất nối với chân 23, nút bấm thứ hai nối với chân 27.
- Cảm biến đo khoảng cách: chân TRIG nối với chân 33, chân ECHO nối với chân 32 của ESP32.
- Còi có một chân nối với chân 18 của ESP32, chân còn lại nối đất.
- Đèn led thứ nhất nối với chân 5, đèn led thứ hai nối với chân 26 của ESP32, chân còn lại nối đất.

![image](https://github.com/HungHyperX/GamesInESP32-BTLNhung/assets/137110849/03d5a613-ba45-4890-9ea4-2948dfb6a1b9)

Sơ đồ mạch sạc pin:
- 2 chân B- và B+ nối với pin
- Chân dương nối với chân VIN (5V) của ESP32 Development Board đi qua công tắc, công tắc sử dụng để cấp/dừng cấp nguồn điện cho ESP32
- Chân âm nối đất  


## Thư viện cài đặt
- [ThingPulse OLED SSD1306](https://github.com/ThingPulse/esp8266-oled-ssd1306.git): SSD1306 Oled display library for ESP8266 and ESP32.
- [Preferences](https://github.com/vshymanskyy/Preferences): Library to store the high score in the internal flash filesystem of ESP32.

## Thiết kế phần mềm

- Trạng thái nút bấm với điện trở kéo lên: khi nút không bấm sẽ mặc định ở mức HIGH, và khi nút được nhấn, nó sẽ nối đất(GND), làm cho nút bấm ở mức LOW(0V)
- Còi chíp:  Khi được cấp nguồn điện, mạch dao động bên trong còi chíp sẽ tạo ra một tín hiệu điện với tần số cố định. Tín hiệu dao động này được áp vào đĩa áp điện, làm cho đĩa co giãn theo tần số của tín hiệu. Đĩa co giãn làm màng rung dao động, tạo ra sóng âm, phát ra ngoài không khí dưới dạng âm thanh. 
- Cảm biến siêu âm: dùng 1 cảm biến siêu âm SR-04 để đo khoảng cách với khoảng cách (theo cm) là giá trị trả về của hàm int GetDistance(), nếu khoảng cách trả về nhỏ hơn hoặc bằng 4cm thì flappy có thể bay lên
- Các icon, hình ảnh trong game là các file .xbm, chứa trong file header asset.h, sử dụng hàm drawXBM() trong thư viện ThingPulse OLED SSD1306, truyền vào vị trí render hình (x, y) tự chọn và width, height, char[] image trong dữ liệu .xbm ở trên
- Máy có 2 game, việc chuyển đổi giữa các game thể hiện qua biến int game (0 là flappy bird, 1 là dino run)
- Mỗi game có 3 trạng thái, thể hiện qua các biến unsigned int FlappyGameState và unsigned int DinoGameState (0 - start, 1 - playing, 2 - end) 
- Dino:
  + Hành động nhảy của khủng long thể hiện qua biến isJumping = true và dinoY -= 0.08: giảm tọa độ y của khủng long đi 0.08 đơn vị. Điều này làm cho khủng long di chuyển lên trên màn hình. Ngược lại đối với khi khủng long rơi
  + Va chạm xảy ra khi một trong các cạnh của khủng long (thể hiện thông qua các biến Dino_width, Dino_height, dinoX, dinoY) đè lên một cạnh nào đó của chướng ngại vật (thể hiện thông qua các biến obstacleX[i], OBSTACLE_WIDTH, SCREEN_HEIGHT - 10)
- Flappy Bird:
  + Khi người chơi nhấn nút hoặc cảm biến khoảng cách nhận được khoảng cách nhỏ hơn hoặc bằng 4cm, biến isFlyingUp được đặt thành true và chim sẽ bay lên.Tọa độ y của chim (birdY) sẽ giảm đi 0.025, mô phỏng hành động chim bay lên.
  + Khi không nhấn nút hoặc cảm biến không nhận được khoảng cách nhỏ hơn hoặc bằng 4cm, biến isFlyingUp được đặt thành false và chim sẽ rơi xuống.Tọa độ y của chim (birdY) sẽ tăng lên 0.015, mô phỏng hành động chim rơi xuống đất.
  + Hàm checkFlappyCollision() kiểm tra xem chim có va chạm với các ống hay không.
  + Khi chim đụng vào một trong các cạnh của ống (thể hiện bằng các biến tubeX[i], TUBE_WIDTH, bottomTubeHeight[i], PATH_WIDTH), va chạm xảy ra.
  + Mỗi khi chim vượt qua một ống, điểm số (FlappyScore) tăng lên.
  + Sau mỗi 5 điểm, tốc độ di chuyển của các ống tăng lên một chút.
## Nhóm sinh viên thực hiện
- Nguyễn Đình Tuấn Đạt - MSSV: 20215562
- Phạm Lưu Minh Hùng - MSSV: 20215586
- Phạm Anh Tuấn - MSSV: 20215661
- Đỗ Đào Phúc - MSSV: 20210684
- Nguyễn Trọng Nhật - MSSV: 20215625

## Nguồn tham khảo
- Original Flappy Bird game by Dong Nguyen.

- Original Google's Dinosaur Run game.

- SSD1306 library by ThingPulse.

- Preferences library by Volodymyr Shymanskyy.

- Flappy Bird : https://github.com/nonameex1sts/FlappyBird_ESP32
