#include <SH1106.h>
#include <Preferences.h>

#include "asset.h"
#include "pitch.h"

#define MY_LED          5
#define BUTTON_PIN      23
#define BUTTON_PIN_1    34
#define BOOT_BUTTON_PIN 0 // Nút BOOT
#define BUZZER_PIN      18

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64

#define TUBE_DISTANCE   32
#define TUBE_WIDTH      6
#define PATH_WIDTH      30

// IIC default address is 0x3c
SH1106 display(0x3c, 21, 22);

// Preferences to read/write flash memory
Preferences preferences;

float tubeX[4];
int bottomTubeHeight[4];

unsigned int score = 0;
unsigned int highScore;
unsigned int gameState = 0;  // 0 - start, 1 - play, 2 - score

bool isFlyingUp = false;
bool isBuzzerOn = false;
bool increaseSpeed = false;
bool hasScored[4];
bool isMusicOn = false;

float birdX = 20.00;
float birdY = 28.00;
float speed = 0.01;

unsigned long keyPressTime = 0;

// trạng thái nút bấm
bool btn;

// danh sách các nốt nhạc
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// thời gina các nốt nhạc: 4 = 1/4 nốt nhạc, 8 = 1/8nốt nhạc, ...:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4};

int brightness = 0;    // mặc định độ sáng của đèn là 
int fadeAmount = 5;    // mỗi lần thay đổi độ sáng thì thay đổi với giá trị là bao nhiêu

void shortMusic(){
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // bây giờ ta đặt một nốt nhạc là 1 giây = 1000 mili giây
    // thì ta chia cho các thành phần noteDurations thì sẽ
    // được thời gian chơi các nốt nhạc
    // ví dụ: 4 => 1000/4; 8 ==> 1000/8 
    int noteDuration = 1000/noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote],noteDuration);

    // để phân biệt các nốt nhạc hãy delay giữa các nốt nhạc
    // một khoảng thời gian vừa phải. Ví dụ sau đây thực hiện tốt
    // điều đó: Ta sẽ cộng 30% và thời lượng của một nốt
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    
    //Ngừng phát nhạc để sau đó chơi nhạc tiếp!
    noTone(BUZZER_PIN);
  }
}

void setup() {
  // Chế độ hoạt động của chân Pin nối với Led:  Output
  pinMode(MY_LED, OUTPUT);
  
  // Chế độ hoạt động của chân Pin nối với nút bấm:  Input với điện trở kêo lên có sẵn trong MCU
  // Khi nhả: mạch điện ở phía nút bấm hở, nhưng điện trở kéo lên bên trong sẽ bảo đám mạch điện vẫn kín, và kéo về logic 1
  // Khi bấm: mạch điện ở phía nút bấm thông, tạo điện áp 0V tương ứng với logic 0
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN_1, INPUT); 
  pinMode(BUZZER_PIN, OUTPUT);

  shortMusic();

  preferences.begin("Flappy", false);
  // Get the high score, if the key does not exist, return a default value of 0
  highScore = preferences.getUInt("highScore", 0);
  // Close the Preferences
  preferences.end();

  display.init();
  
  // Phát còi khi khởi động
  // digitalWrite(BUZZER_PIN,1);
  // delay(200);
  // digitalWrite(BUZZER_PIN,0);

  
  
  

  // Initialize tubes on the right outside of the screen
  for(int i = 0; i < 4; i++) {
    tubeX[i] = 128 + i * TUBE_DISTANCE;
    bottomTubeHeight[i] = random (8, 32);
  }

  // Make all asset in the game is right side up
  display.flipScreenVertically();
}

void loop() {

  display.clear();

  int btn = digitalRead(BUTTON_PIN_1);
  // Hiển thị led tương ứng
  if (btn == HIGH)
    digitalWrite(MY_LED, HIGH);
  else{
    digitalWrite(MY_LED, LOW);
  }

  // //xuất giá trị độ sáng đèn LED
  // analogWrite(MY_LED, brightness);    
 
  // // thay đổi giá trị là đèn LED
  // brightness = brightness + fadeAmount;
 
  // // Đoạn code này có nghĩa nếu độ sáng == 0 hoặc bằng == 255 thì sẽ đổi chiều của biến thay đổi độ sáng. Ví dụ, nếu đèn từ sáng yếu --> sáng mạnh thì fadeAmount dương. Còn nếu đèn sáng mạnh --> sáng yếu thì fadeAmmount lúc này sẽ có giá trị âm
  // if (brightness == 0 || brightness == 255) {
  //   fadeAmount = -fadeAmount ;
  // }    
  // //đợi 30 mili giây để thấy sự thay đổi của đèn
  // //delay(30);                            

  // Display start screen
  if(gameState == 0) {  
    // Reinitialize in-game status
    birdY = 28.00;
    score = 0;
    speed = 0.01;

    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 4, "Flappy ");

    display.drawXbm(64, 0, Building_width, Building_height, Building);
    display.drawXbm(birdX, birdY, Flappy_width, Flappy_height, Flappy);
    display.setColor(WHITE);
    display.fillRect(0, SCREEN_HEIGHT - 5, SCREEN_WIDTH, 5);

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 44, "Press to start");

    // Reinitialize all tubes
    for(int i = 0; i < 4; i++) {
      tubeX[i] = 128 + ((i+1) * TUBE_DISTANCE);
      bottomTubeHeight[i] = random(8, 32);
      hasScored[i] = false;
    }

    if(digitalRead(BUTTON_PIN) == LOW) {
      gameState = 1;
      delay(50);
    }
  }

  //Display in-game screen
  else if (gameState == 1) {  

    //Display score
    display.setFont(ArialMT_Plain_10);
    display.drawString(3, 0, String(score));
   
    // Setup variables and flags if button is pressed
    if(digitalRead(BUTTON_PIN) == LOW) {
    keyPressTime = millis();
      isFlyingUp = true;
      isBuzzerOn = true;
    } 

    // Display bird and tubes
    display.setColor(WHITE);
    display.drawXbm(birdX, birdY, Flappy_width, Flappy_height, Flappy);

    for(int i = 0; i < 4; i++) {
      display.fillRect(tubeX[i], 0, TUBE_WIDTH, bottomTubeHeight[i]);
      display.fillRect(tubeX[i], bottomTubeHeight[i] + PATH_WIDTH, TUBE_WIDTH, SCREEN_HEIGHT - bottomTubeHeight[i] - PATH_WIDTH);
    }
  
    for(int i = 0; i < 4; i++) {
      //Move all tube to the left
      tubeX[i] -= speed;

      // If a tube pass the bird, add a point
      if(tubeX[i] < birdX && !hasScored[i]){
        score++;
        hasScored[i] = true;

        // Increase speed every 10 tubes
        if(score % 5 == 0){
          speed += 0.01;
        }
      }

      // If a tube pass the screen, reinitialize that tube on the right of the screen
      if(tubeX[i] + TUBE_WIDTH < 0){
        bottomTubeHeight[i] = random(8, 32);
        tubeX[i] = 128;
        hasScored[i] = false;
      }
    }

    // The bird will fly up for 80 milliseconds
    if((keyPressTime + 80) < millis()) {
      isFlyingUp = false;
    }

    // The buzzer will make sound for 10 milliseconds
    if((keyPressTime + 10) < millis()) {
      isBuzzerOn = false;
    }
  
    // Bird fly up (y-axis is reverted)
    if(isFlyingUp) {
      birdY -= 0.025;
    } 
    else {
      birdY += 0.015;
    }

    // Sound when click
    if(isBuzzerOn){
      digitalWrite(BUZZER_PIN,1);
    }
    else{
      digitalWrite(BUZZER_PIN,0);
    }

    if(birdY > 63 || birdY < 0) { // Check if out of bound on vertical axis
      // Ending sound
      digitalWrite(BUZZER_PIN,1);
      delay(200);
      digitalWrite(BUZZER_PIN,0); 
      delay(50);
      digitalWrite(BUZZER_PIN,1);
      delay(50);
      digitalWrite(BUZZER_PIN,0); 
      delay(50);
      digitalWrite(BUZZER_PIN,1);
      delay(50);
      digitalWrite(BUZZER_PIN,0); 

      if(score > highScore){
        highScore = score;

        // Write new high score to flash memory
        preferences.begin("Flappy", false);
        preferences.putUInt("highScore", highScore);
        preferences.end();
      }

      gameState = 2;
    }

    //Check for collision with tube
    for(int i = 0; i < 4; i++){
      if(tubeX[i] <= birdX + 7 && birdX + 7 <= tubeX[i] + 6) {
        if(birdY < bottomTubeHeight[i] || birdY + 8 > bottomTubeHeight[i] + PATH_WIDTH){
          // Ending sound
          digitalWrite(BUZZER_PIN,1);
          delay(200);
          digitalWrite(BUZZER_PIN,0); 
          delay(50);
          digitalWrite(BUZZER_PIN,1);
          delay(50);
          digitalWrite(BUZZER_PIN,0); 
          delay(50);
          digitalWrite(BUZZER_PIN,1);
          delay(50);
          digitalWrite(BUZZER_PIN,0); 

          if(score > highScore){
            highScore = score;

            // Write new high score to flash memory
            preferences.begin("Flappy", false);
            preferences.putUInt("highScore", highScore);
            preferences.end();
          }

          gameState = 2;

          delay(50);
        }
      }
    }

    // Display boundary
    display.drawRect(0, 0, 128, 64);
    //display.drawRect(0, 0, 256, 128);
  }

  // Display ending (score) screen
  else{ 
    if (!isMusicOn){
      isMusicOn = true;
      shortMusic();
    }
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Your score: " + String(score));
    display.drawString(0, 20, "High score: " + String(highScore));

    display.setFont(ArialMT_Plain_10);
    display.drawString(32, 44, "Click to restart");

    display.setFont(ArialMT_Plain_10);
    display.drawString(5, 54, "Click BOOT to reset score");

    if(digitalRead(BUTTON_PIN) == LOW){
      gameState = 0;
      delay(200);
    }

    // If BOOT button is pressed, reset high score in game and in the flash memory
    if(digitalRead(BOOT_BUTTON_PIN) == LOW){
      score = 0;
      highScore = 0;

      // Write high score (= 0) to flash memory
      preferences.begin("Flappy", false);
      preferences.putUInt("highScore", highScore);
      preferences.end();

      isMusicOn = false;
    }
  }
  
   display.display();
}