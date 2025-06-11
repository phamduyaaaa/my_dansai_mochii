#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- CÀI ĐẶT CƠ BẢN ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// --- BIẾN TRẠNG THÁI NHÂN VẬT ---
int eye_height = 36;
int eye_width = 32;
int eye_corner_radius = 12;
int space_between_eye = 10;
int left_eye_x, left_eye_y, right_eye_x, right_eye_y;
int pupil_x_offset = 0, pupil_y_offset = 0;

// <<< NÂNG CẤP MIỆNG ANIME: Dùng enum để code dễ đọc hơn >>>
enum MouthState { M_NEUTRAL, M_SMILE, M_SURPRISED, M_CAT_SMILE };
MouthState current_mouth_state = M_NEUTRAL;
int mouth_x, mouth_y;

// <<< NÂNG CẤP MIỆNG ANIME: "BỘ NÃO" CỦA NHÂN VẬT >>>
enum AIState { IDLE, PLAYFUL, SLEEPY, ASLEEP };
AIState currentState = IDLE;
long lastStateChangeTime = 0;
long currentStateDuration = 5000;
int targetX, targetY;


// --- CÁC HÀM VẼ VÀ HOẠT CẢNH ---

// <<< NÂNG CẤP MIỆNG ANIME: Hàm vẽ miệng với nhiều kiểu dáng >>>
void draw_mouth() {
  mouth_x = SCREEN_WIDTH / 2;
  mouth_y = SCREEN_HEIGHT / 2 + eye_height / 2 - 2;

  switch(current_mouth_state) {
    case M_NEUTRAL:
      display.drawFastHLine(mouth_x - 5, mouth_y, 10, SSD1306_WHITE);
      break;
    case M_SMILE:
      display.fillRoundRect(mouth_x - 15, mouth_y, 30, 8, 4, SSD1306_WHITE);
      display.fillRect(mouth_x - 15, mouth_y, 30, 5, SSD1306_BLACK);
      break;
    case M_SURPRISED:
      display.fillRoundRect(mouth_x - 8, mouth_y, 16, 16, 8, SSD1306_WHITE);
      break;
    case M_CAT_SMILE: // Miệng mèo :3
      display.drawLine(mouth_x - 7, mouth_y, mouth_x, mouth_y + 4, SSD1306_WHITE);
      display.drawLine(mouth_x, mouth_y + 4, mouth_x + 7, mouth_y, SSD1306_WHITE);
      break;
  }
}

void draw_anime_eyes() {
    display.fillRoundRect(left_eye_x - eye_width/2, left_eye_y - eye_height/2, eye_width, eye_height, eye_corner_radius, SSD1306_WHITE);
    display.fillRoundRect(right_eye_x - eye_width/2, right_eye_y - eye_height/2, eye_width, eye_height, eye_corner_radius, SSD1306_WHITE);
    int left_pupil_x = left_eye_x + pupil_x_offset, left_pupil_y = left_eye_y + pupil_y_offset;
    int right_pupil_x = right_eye_x + pupil_x_offset, right_pupil_y = right_eye_y + pupil_y_offset;
    display.fillCircle(left_pupil_x, left_pupil_y, 10, SSD1306_BLACK);
    display.fillCircle(right_pupil_x, right_pupil_y, 10, SSD1306_BLACK);
    display.fillCircle(left_pupil_x - 3, left_pupil_y - 3, 4, SSD1306_WHITE);
    display.fillCircle(left_pupil_x + 4, left_pupil_y + 4, 2, SSD1306_WHITE);
    display.fillCircle(right_pupil_x - 3, right_pupil_y - 3, 4, SSD1306_WHITE);
    display.fillCircle(right_pupil_x + 4, right_pupil_y + 4, 2, SSD1306_WHITE);
}

void draw_face() {
    display.clearDisplay();        
    draw_anime_eyes();
    draw_mouth();
    display.display();
}

void setup_face_position() {
  left_eye_x = SCREEN_WIDTH/2 - eye_width/2 - space_between_eye/2;
  left_eye_y = SCREEN_HEIGHT/2 - 5; // Dịch mắt lên chút
  right_eye_x = SCREEN_WIDTH/2 + eye_width/2 + space_between_eye/2;
  right_eye_y = SCREEN_HEIGHT/2 - 5;
}

void blink() {
  int original_h = eye_height;
  for(int h=original_h; h>2; h-=8) { eye_height=h; draw_face(); }
  for(int h=2; h<=original_h; h+=8) { eye_height=h; draw_face(); }
  eye_height = original_h;
  draw_face();
}

void sleep_eyes() {
  display.clearDisplay();
  display.drawFastHLine(left_eye_x - eye_width/2, SCREEN_HEIGHT/2, eye_width, SSD1306_WHITE);
  display.drawFastHLine(right_eye_x - eye_width/2, SCREEN_HEIGHT/2, eye_width, SSD1306_WHITE);
  display.display();
}

// --- HÀM SETUP VÀ LOOP CHÍNH ---

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  randomSeed(analogRead(0));
  setup_face_position();
  draw_face();
  lastStateChangeTime = millis();
}

void loop() {
  // --- KHỐI 1: LOGIC CHUYỂN ĐỔI TRẠNG THÁI ---
  if (millis() - lastStateChangeTime > currentStateDuration) {
    AIState newState;
    int chance = random(100);
    if (currentState == ASLEEP) {
        newState = IDLE; 
    } else {
        if (chance < 50) newState = IDLE;
        else if (chance < 85) newState = PLAYFUL;
        else newState = SLEEPY;
    }
    currentState = newState;
    lastStateChangeTime = millis();

    if (currentState == IDLE) currentStateDuration = random(3000, 7000);
    else if (currentState == PLAYFUL) currentStateDuration = random(2000, 5000);
    else if (currentState == SLEEPY) currentStateDuration = 4000;
    else if (currentState == ASLEEP) currentStateDuration = random(5000, 8000);
  }

  // --- KHỐI 2: LOGIC HÀNH VI DỰA TRÊN TRẠNG THÁI HIỆN TẠI ---
  switch(currentState) {
    case IDLE:
      current_mouth_state = M_NEUTRAL; // Miệng bình thường khi thư giãn
      if (random(100) < 10) { targetX = random(0, SCREEN_WIDTH); targetY = random(0, SCREEN_HEIGHT); }
      pupil_x_offset += (map(targetX, 0, SCREEN_WIDTH, -5, 5) - pupil_x_offset) * 0.2;
      pupil_y_offset += (map(targetY, 0, SCREEN_HEIGHT, -8, 8) - pupil_y_offset) * 0.2;
      if (random(1000) < 10) { blink(); }
      draw_face();
      delay(20);
      break;

    case PLAYFUL:
      // <<< NÂNG CẤP MIỆNG ANIME: Khi tinh nghịch, có thể cười hoặc làm miệng mèo >>>
      if(random(100) < 70) {
        current_mouth_state = M_SMILE;
      } else {
        current_mouth_state = M_CAT_SMILE;
      }
      if (random(100) < 20) { targetX = random(0, SCREEN_WIDTH); targetY = random(0, SCREEN_HEIGHT); }
      pupil_x_offset += (map(targetX, 0, SCREEN_WIDTH, -5, 5) - pupil_x_offset) * 0.2;
      pupil_y_offset += (map(targetY, 0, SCREEN_HEIGHT, -8, 8) - pupil_y_offset) * 0.2;
      if (random(1000) < 30) { blink(); }
      draw_face();
      delay(20);
      break;

    case SLEEPY:
      current_mouth_state = M_SURPRISED; // Miệng chữ 'o' khi ngáp
      for(int h=eye_height; h>2; h-=2) {
        display.clearDisplay();
        draw_mouth();
        display.fillRoundRect(left_eye_x-eye_width/2, left_eye_y-h/2, eye_width, h, eye_corner_radius, SSD1306_WHITE);
        display.fillRoundRect(right_eye_x-eye_width/2, right_eye_y-h/2, eye_width, h, eye_corner_radius, SSD1306_WHITE);
        display.display();
        delay(60);
      }
      currentState = ASLEEP;
      lastStateChangeTime = millis();
      currentStateDuration = random(5000, 8000);
      break;

    case ASLEEP:
      current_mouth_state = M_NEUTRAL;
      sleep_eyes();
      delay(100);
      break;
  }
}
