#include <SH1106.h>
#include <Preferences.h>

#include "asset.h"
#include "pitch.h"

#define MY_LED_1          5
#define MY_LED_2          26

#define BUTTON_PIN_1      23
#define BUTTON_PIN_2      27
#define BOOT_BUTTON_PIN   0
#define BUZZER_PIN        18

#define SCREEN_WIDTH      128
#define SCREEN_HEIGHT     64

// Flappy
#define TUBE_DISTANCE     32
#define TUBE_WIDTH        6
#define PATH_WIDTH        30

// Dino
#define OBSTACLE_WIDTH    14
#define GROUND_HEIGHT     10
#define JUMP_HEIGHT       20
#define GRAVITY           0.025

// I2C default address is 0x3c
SH1106 display(0x3c, 21, 22);
Preferences preferences;

unsigned int FlappyScore = 0;
unsigned int FlappyHighScore;
unsigned int FlappyGameState = 0;

bool isFlyingUp = false;
bool isBuzzerOn = false;
bool FlappyHasScored[4];
bool isMusicOn = true;

float birdX = 20.0;
float birdY = 28.0;
float flappySpeed = 0.01;
float tubeX[4];
int bottomTubeHeight[4];

unsigned int DinoScore = 0;
unsigned int DinoHighScore;
unsigned int DinoGameState = 0;

bool isJumping = false;
bool canPush = true;
bool DinoHasScored[4];
bool isMusicOnDino = false;

float obstacleX[4];
float dinoX = 20.0;
float dinoY = 28.0;
float jumpSpeed = 0;
float dinoSpeed = 0.01;

unsigned long keyPressTime = 0;
int game = 0;

int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

void flappyBirdGame();
void dinoRunGame();
void displayFlappyStartScreen();
void playFlappyBird();
bool checkFlappyCollision();
void endFlappyGame();
void displayFlappyEndScreen();
void resetFlappyHighScore();
void displayDinoStartScreen();
void playDinoRun();
bool checkDinoCollision();
void endDinoGame();
void displayDinoEndScreen();
void resetDinoHighScore();

void shortMusic() {
    for (int thisNote = 0; thisNote < 8; thisNote++) {
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(BUZZER_PIN, melody[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(BUZZER_PIN);
    }
}

void setup() {
    pinMode(MY_LED_1, OUTPUT);
    pinMode(MY_LED_2, OUTPUT);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);
    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);

    shortMusic();

    preferences.begin("Dino", false);
    DinoHighScore = preferences.getUInt("highScore", 0);
    preferences.end();

    preferences.begin("Flappy", false);
    FlappyHighScore = preferences.getUInt("highScore", 0);
    preferences.end();

    display.init();

    for (int i = 0; i < 4; i++) {
        tubeX[i] = 128 + i * TUBE_DISTANCE;
        bottomTubeHeight[i] = random(8, 32);
        obstacleX[i] = 128 + i * 80;
    }

    display.flipScreenVertically();
}

void loop() {
    display.clear();

    if (digitalRead(BUTTON_PIN_2) == LOW) {
        game = (game + 1) % 2;
        delay(200);
    } 

    if (game == 0) {
      digitalWrite(MY_LED_1, HIGH);
      digitalWrite(MY_LED_2, LOW);
      flappyBirdGame();
    } else {
      digitalWrite(MY_LED_1, LOW);
      digitalWrite(MY_LED_2, HIGH);
      dinoRunGame();
    }


    display.display();
}

void flappyBirdGame() {
    if (FlappyGameState == 0) {
        displayFlappyStartScreen();
        if (digitalRead(BUTTON_PIN_1) == LOW) {
            FlappyGameState = 1;
            delay(50);
        }
    } else if (FlappyGameState == 1) {
        playFlappyBird();
    } else {
        displayFlappyEndScreen();

        if (digitalRead(BUTTON_PIN_1) == LOW) {
            FlappyGameState = 0;
            delay(200);
        }
        if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
            resetFlappyHighScore();
        }
    }
}

void dinoRunGame() {
    if (DinoGameState == 0) {
        displayDinoStartScreen();
        if (digitalRead(BUTTON_PIN_1) == LOW) {
            DinoGameState = 1;
            delay(50);
        }
    } else if (DinoGameState == 1) {
        playDinoRun();
    } else {
        displayDinoEndScreen();
      
        if (digitalRead(BUTTON_PIN_1) == LOW) {
            DinoGameState = 0;
            delay(200);
        }
        if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
            resetDinoHighScore();
        }
    }
}

void displayFlappyStartScreen() {
    birdY = 28.0;
    FlappyScore = 0;
    flappySpeed = 0.01;

    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 4, "Flappy ");
    display.drawXbm(64, 0, Building_width, Building_height, Building);
    display.drawXbm(birdX, birdY, Flappy_width, Flappy_height, Flappy);
    display.fillRect(0, SCREEN_HEIGHT - 5, SCREEN_WIDTH, 5);

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 44, "Press to start");

    for (int i = 0; i < 4; i++) {
        tubeX[i] = 128 + ((i + 1) * TUBE_DISTANCE);
        bottomTubeHeight[i] = random(8, 32);
        FlappyHasScored[i] = false;
    }
}

void playFlappyBird() {
    display.setFont(ArialMT_Plain_10);
    display.drawString(3, 0, String(FlappyScore));

    if (digitalRead(BUTTON_PIN_1) == LOW) {
        keyPressTime = millis();
        isFlyingUp = true;
        isBuzzerOn = true;
    }

    display.drawXbm(birdX, birdY, Flappy_width, Flappy_height, Flappy);
    for (int i = 0; i < 4; i++) {
        display.fillRect(tubeX[i], 0, TUBE_WIDTH, bottomTubeHeight[i]);
        display.fillRect(tubeX[i], bottomTubeHeight[i] + PATH_WIDTH, TUBE_WIDTH, SCREEN_HEIGHT - bottomTubeHeight[i] - PATH_WIDTH);
    }

    for (int i = 0; i < 4; i++) {
        tubeX[i] -= flappySpeed;
        if (tubeX[i] < birdX && !FlappyHasScored[i]) {
            FlappyScore++;
            FlappyHasScored[i] = true;
            if (FlappyScore % 5 == 0) {
                flappySpeed += 0.01;
            }
        }
        if (tubeX[i] + TUBE_WIDTH < 0) {
            bottomTubeHeight[i] = random(8, 32);
            tubeX[i] = 128;
            FlappyHasScored[i] = false;
        }
    }

    if ((keyPressTime + 80) < millis()) isFlyingUp = false;
    if ((keyPressTime + 10) < millis()) isBuzzerOn = false;

    birdY += isFlyingUp ? -0.025 : 0.015;
    digitalWrite(BUZZER_PIN, isBuzzerOn ? HIGH : LOW);

    if (birdY > 63 || birdY < 0 || checkFlappyCollision()) {
        endFlappyGame();
    }

    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

bool checkFlappyCollision() {
    for (int i = 0; i < 4; i++) {
        if (tubeX[i] <= birdX + 7 && birdX + 7 <= tubeX[i] + TUBE_WIDTH) {
            if (birdY < bottomTubeHeight[i] || birdY + 8 > bottomTubeHeight[i] + PATH_WIDTH) {
                return true;
            }
        }
    }
    return false;
}

void endFlappyGame() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(50);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
    delay(50);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);

    if (FlappyScore > FlappyHighScore) {
        FlappyHighScore = FlappyScore;
        preferences.begin("Flappy", false);
        preferences.putUInt("highScore", FlappyHighScore);
        preferences.end();
    }

    FlappyGameState = 2;
    delay(50);
}

void displayFlappyEndScreen() {
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Game over");
    display.drawString(0, 10, "Score:");
    display.drawString(55, 10, String(FlappyScore));
    display.drawString(0, 20, "Highscore:");
    display.drawString(55, 20, String(FlappyHighScore));
    display.drawString(0, 35, "Press to restart");
    display.drawString(0, 45, "Boot to reset");
}

void resetFlappyHighScore() {
    FlappyHighScore = 0;
    preferences.begin("Flappy", false);
    preferences.putUInt("highScore", FlappyHighScore);
    preferences.end();
    delay(200);
}

void displayDinoStartScreen() {
    dinoY = 28;
    DinoScore = 0;
    dinoSpeed = 0.05;

    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 4, "Dino Run");
    display.drawXbm(64, 0, Volcano_width, Volcano_height, Volcano);
    display.drawXbm(dinoX, dinoY, Dino_width, Dino_height, Dino);

    display.fillRect(0, SCREEN_HEIGHT - 5, SCREEN_WIDTH, 5);

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 44, "Press to start");

    for(int i = 0; i < 4; i++) {
      obstacleX[i] = 128 + i * 80;
      DinoHasScored[i] = false;
    }
}

void playDinoRun() {
    display.setFont(ArialMT_Plain_10);
    display.drawString(3, 0, String(DinoScore));

    if (digitalRead(BUTTON_PIN_1) == LOW && !isJumping && canPush) {
        keyPressTime = millis();
        jumpSpeed = 0.1;
        isJumping = true;
        canPush = false;
    }

    display.drawXbm(dinoX, dinoY, Dino_width, Dino_height, Dino);
    for (int i = 0; i < 4; i++) {
        display.drawXbm(obstacleX[i], SCREEN_HEIGHT - OBSTACLE_WIDTH, OBSTACLE_WIDTH, OBSTACLE_WIDTH, Cactus);
    }

    for (int i = 0; i < 4; i++) {
        obstacleX[i] -= dinoSpeed;
        if (obstacleX[i] < dinoX && !DinoHasScored[i]) {
            DinoScore++;
            DinoHasScored[i] = true;
            if (DinoScore % 5 == 0) {
                dinoSpeed += 0.01;
            }
        }
        if (obstacleX[i] + OBSTACLE_WIDTH < 0) {
            obstacleX[i] = obstacleX[(i + 3) % 4] + 80;
            DinoHasScored[i] = false;
        }
    }

    if((keyPressTime + 400) < millis()) {
      isJumping = false;
    }

    if(isJumping) {
      dinoY -= 0.08;
    }
    else {
      dinoY += 0.08;
      if(dinoY >= 49) {
        dinoY = 49;
        isJumping = false;
        canPush = true;
      }
    }

    if (checkDinoCollision()) {
        endDinoGame();
    }

    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

bool checkDinoCollision() {
    for (int i = 0; i < 4; i++) {
        if (obstacleX[i] <= dinoX + Dino_width && dinoX <= obstacleX[i] + OBSTACLE_WIDTH) {
            if (dinoY + Dino_height >= SCREEN_HEIGHT - OBSTACLE_WIDTH) {
                return true;
            }
        }
    }
    return false;
}

void endDinoGame() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(50);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
    delay(50);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);

    if (DinoScore > DinoHighScore) {
        DinoHighScore = DinoScore;
        preferences.begin("Dino", false);
        preferences.putUInt("highScore", DinoHighScore);
        preferences.end();
    }

    isMusicOnDino = false;
    DinoGameState = 2;
    delay(50);
}

void displayDinoEndScreen() {
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Game over");
    display.drawString(0, 10, "Score:");
    display.drawString(55, 10, String(DinoScore));
    display.drawString(0, 20, "Highscore:");
    display.drawString(55, 20, String(DinoHighScore));
    display.drawString(0, 35, "Press to restart");
    display.drawString(0, 45, "Boot to reset");
}

void resetDinoHighScore() {
    DinoHighScore = 0;
    preferences.begin("Dino", false);
    preferences.putUInt("highScore", DinoHighScore);
    preferences.end();
    delay(200);
}
