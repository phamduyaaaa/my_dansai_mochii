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
int max_pupil_offset = eye_width/2 - 10;

enum MouthState { M_NEUTRAL, M_SMILE, M_SURPRISED, M_CAT_SMILE, M_SAD, M_GRIN, M_TONGUE_OUT, M_WOBBLY };
MouthState current_mouth_state = M_NEUTRAL;
int mouth_x, mouth_y;

// "BỘ NÃO" CỦA NHÂN VẬT
enum AIState { IDLE, PLAYFUL, SLEEPY, ASLEEP };
AIState currentState = IDLE;
long lastStateChangeTime = 0;
long currentStateDuration = 5000;
int targetX, targetY;
int shake_x = 0, shake_y = 0;


// --- CÁC HÀM VẼ VÀ HOẠT CẢNH ---

void draw_mouth() {
  mouth_x = SCREEN_WIDTH / 2 + shake_x;
  mouth_y = SCREEN_HEIGHT / 2 + eye_height / 2 - 2 + shake_y;

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
    case M_CAT_SMILE:
      display.drawLine(mouth_x - 7, mouth_y, mouth_x, mouth_y + 4, SSD1306_WHITE);
      display.drawLine(mouth_x, mouth_y + 4, mouth_x + 7, mouth_y, SSD1306_WHITE);
      break;
    case M_WOBBLY:
      display.drawLine(mouth_x - 10, mouth_y + 2, mouth_x - 5, mouth_y - 2, SSD1306_WHITE);
      display.drawLine(mouth_x - 5, mouth_y - 2, mouth_x + 5, mouth_y + 2, SSD1306_WHITE);
      display.drawLine(mouth_x + 5, mouth_y + 2, mouth_x + 10, mouth_y - 2, SSD1306_WHITE);
      break;
    // Các case khác có thể thêm ở đây nếu muốn
  }
}

void draw_anime_eyes() {
    display.fillRoundRect(left_eye_x + shake_x - eye_width/2, left_eye_y + shake_y - eye_height/2, eye_width, eye_height, eye_corner_radius, SSD1306_WHITE);
    display.fillRoundRect(right_eye_x + shake_x - eye_width/2, right_eye_y + shake_y - eye_height/2, eye_width, eye_height, eye_corner_radius, SSD1306_WHITE);
    
    int lp_x = left_eye_x + shake_x + pupil_x_offset;
    int lp_y = left_eye_y + shake_y + pupil_y_offset;
    int rp_x = right_eye_x + shake_x + pupil_x_offset;
    int rp_y = right_eye_y + shake_y + pupil_y_offset;
    
    display.fillCircle(lp_x, lp_y, 10, SSD1306_BLACK);
    display.fillCircle(rp_x, rp_y, 10, SSD1306_BLACK);
    display.fillCircle(lp_x - 3, lp_y - 3, 4, SSD1306_WHITE);
    display.fillCircle(lp_x + 4, lp_y + 4, 2, SSD1306_WHITE);
    display.fillCircle(rp_x - 3, rp_y - 3, 4, SSD1306_WHITE);
    display.fillCircle(rp_x + 4, rp_y + 4, 2, SSD1306_WHITE);
}

void draw_swirl_eye(int x, int y, int size) {
  int step = 3;
  for (int i = 0; i < size / 2; i += step) {
    display.drawRect(x - i, y - i, 2 * i, 2 * i, SSD1306_WHITE);
  }
}

void draw_face() {
    display.clearDisplay();        
    draw_anime_eyes();
    draw_mouth();
    display.display();
}

void setup_face_position() {
  left_eye_x = SCREEN_WIDTH/2 - eye_width/2 - space_between_eye/2;
  left_eye_y = SCREEN_HEIGHT/2 - 5;
  right_eye_x = SCREEN_WIDTH/2 + eye_width/2 + space_between_eye/2;
  right_eye_y = SCREEN_HEIGHT/2 - 5;
  pupil_x_offset = 0;
  pupil_y_offset = 0;
}

void blink() {
  int original_h = eye_height;
  for(int h=original_h; h>2; h-=8) { eye_height=h; draw_face(); }
  for(int h=2; h<=original_h; h+=8) { eye_height=h; draw_face(); }
  eye_height = original_h;
}

void sleep_eyes() {
  display.clearDisplay();
  display.drawFastHLine(left_eye_x - eye_width/2, SCREEN_HEIGHT/2, eye_width, SSD1306_WHITE);
  display.drawFastHLine(right_eye_x - eye_width/2, SCREEN_HEIGHT/2, eye_width, SSD1306_WHITE);
  display.display();
}

void dizzy_animation() {
  long start_time = millis();
  current_mouth_state = M_WOBBLY;

  while (millis() - start_time < 4000) {
    display.clearDisplay();
    shake_x = random(-3, 4);
    shake_y = random(-3, 4);
    draw_swirl_eye(left_eye_x + shake_x, left_eye_y + shake_y, 30);
    draw_swirl_eye(right_eye_x + shake_x, right_eye_y + shake_y, 30);
    draw_mouth();
    display.display();
    delay(50);
  }
  shake_x = 0; shake_y = 0;
  setup_face_position(); // Reset lại vị trí
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
    AIState newState; int chance = random(100);
    if(currentState==ASLEEP){newState=IDLE;}else{if(chance<50)newState=IDLE;else if(chance<85)newState=PLAYFUL;else newState=SLEEPY;}
    currentState=newState; lastStateChangeTime=millis();
    if(currentState==IDLE)currentStateDuration=random(3000,6000);else if(currentState==PLAYFUL)currentStateDuration=random(2000,5000);else if(currentState==SLEEPY)currentStateDuration=4000;else if(currentState==ASLEEP)currentStateDuration=random(5000,8000);
  }

  // --- KHỐI 2: LOGIC HÀNH VI ---
  switch(currentState) {
    case IDLE:
      current_mouth_state = M_NEUTRAL;
      if (random(100) < 10) { targetX = random(0, SCREEN_WIDTH); targetY = random(0, SCREEN_HEIGHT); }
      pupil_x_offset += (map(targetX, 0, SCREEN_WIDTH, -max_pupil_offset, max_pupil_offset) - pupil_x_offset) * 0.1;
      pupil_y_offset += (map(targetY, 0, SCREEN_HEIGHT, -8, 8) - pupil_y_offset) * 0.1;
      if (random(1000) < 10) { blink(); }
      draw_face();
      break;

    case PLAYFUL:
      if(random(100) < 10){ current_mouth_state = (random(2) == 0) ? M_SMILE : M_CAT_SMILE; }
      if (random(100) < 20) { targetX = random(0, SCREEN_WIDTH); targetY = random(0, SCREEN_HEIGHT); }
      pupil_x_offset += (map(targetX, 0, SCREEN_WIDTH, -max_pupil_offset, max_pupil_offset) - pupil_x_offset) * 0.2;
      pupil_y_offset += (map(targetY, 0, SCREEN_HEIGHT, -8, 8) - pupil_y_offset) * 0.2;
      
      if (random(1000) < 15) {
        dizzy_animation();
      } else if (random(1000) < 40) {
        blink();
      }
      draw_face();
      break;

    case SLEEPY:
      current_mouth_state = M_SURPRISED;
      for(int h=eye_height; h>2; h-=2){
        display.clearDisplay();
        draw_mouth();
        display.fillRoundRect(left_eye_x - eye_width/2, left_eye_y-h/2, eye_width, h, eye_corner_radius, SSD1306_WHITE);
        display.fillRoundRect(right_eye_x - eye_width/2, right_eye_y-h/2, eye_width, h, eye_corner_radius, SSD1306_WHITE);
        display.display();
        delay(60);
      }
      currentState = ASLEEP;
      lastStateChangeTime = millis();
      currentStateDuration = random(5000, 8000);
      break;

    case ASLEEP:
      sleep_eyes();
      delay(100);
      break;
  }
  delay(20);
}
