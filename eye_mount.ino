#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Biến điều khiển
int demo_mode = 1;
static const int max_animation_index = 9; // <<< THAY ĐỔI: Tăng số hoạt cảnh lên
int current_animation_index = 0;


// Trạng thái tham chiếu (nghỉ) của mắt
int ref_eye_height = 30; // Giảm chiều cao mắt một chút để có không gian cho miệng
int ref_eye_width = 30;
int ref_space_between_eye = 15;
int ref_corner_radius = 8;

// Trạng thái hiện tại của con mắt
int left_eye_height = ref_eye_height;
int left_eye_width = ref_eye_width;
int left_eye_x, left_eye_y; // Sẽ được tính trong hàm center_eyes
int right_eye_x, right_eye_y;
int right_eye_height = ref_eye_height;
int right_eye_width = ref_eye_width;

// <<< THAY ĐỔI: Thêm các biến cho miệng >>>
int mouth_state = 0; // 0=Bình thường, 1=Cười, 2=Ngạc nhiên (chữ O)
int mouth_x;         // Vị trí X của miệng
int mouth_y;         // Vị trí Y của miệng
int mouth_w = 40;    // Chiều rộng miệng
int mouth_h = 8;     // Chiều cao miệng


// --- CÁC HÀM VẼ VÀ HOẠT CẢNH ---

// <<< THAY ĐỔI: Hàm vẽ miệng mới >>>
void draw_mouth() {
  switch(mouth_state) {
    case 0: // Miệng bình thường (một đường thẳng)
      display.drawFastHLine(mouth_x - mouth_w / 2, mouth_y, mouth_w, SSD1306_WHITE);
      break;
    case 1: // Miệng cười (một hình chữ nhật bo góc hẹp)
      display.drawRoundRect(mouth_x - mouth_w / 2, mouth_y, mouth_w, mouth_h, 4, SSD1306_WHITE);
      break;
    case 2: // Miệng ngạc nhiên (hình tròn)
      display.fillCircle(mouth_x, mouth_y + 4, 10, SSD1306_WHITE);
      break;
  }
}

// <<< THAY ĐỔI: Đổi tên hàm và tích hợp vẽ miệng >>>
void draw_face(bool update=true)
{
    display.clearDisplay();        
    
    // Vẽ mắt trái và phải
    int x = int(left_eye_x-left_eye_width/2);
    int y = int(left_eye_y-left_eye_height/2);
    display.fillRoundRect(x,y,left_eye_width,left_eye_height,ref_corner_radius,SSD1306_WHITE);
    
    x = int(right_eye_x-right_eye_width/2);
    y = int(right_eye_y-right_eye_height/2);
    display.fillRoundRect(x,y,right_eye_width,right_eye_height,ref_corner_radius,SSD1306_WHITE);    
    
    // Vẽ miệng
    draw_mouth();

    if(update)
    {
      display.display();
    }
}


void center_eyes(bool update=true)
{
  // Reset mắt về trung tâm và dịch lên trên một chút
  left_eye_height = ref_eye_height;
  left_eye_width = ref_eye_width;
  right_eye_height = ref_eye_height;
  right_eye_width = ref_eye_width;
  
  // Dịch tâm mắt lên trên để có chỗ cho miệng
  int center_y_offset = -8; 
  left_eye_x = SCREEN_WIDTH/2 - ref_eye_width/2 - ref_space_between_eye/2;
  left_eye_y = SCREEN_HEIGHT/2 + center_y_offset;
  right_eye_x = SCREEN_WIDTH/2 + ref_eye_width/2 + ref_space_between_eye/2;
  right_eye_y = SCREEN_HEIGHT/2 + center_y_offset;

  // <<< THAY ĐỔI: Reset miệng về trạng thái bình thường và căn giữa >>>
  mouth_state = 0;
  mouth_x = SCREEN_WIDTH / 2;
  mouth_y = SCREEN_HEIGHT / 2 + ref_eye_height / 2 + 5;
  
  draw_face(update);
}

void blink(int speed=12)
{
  // Giữ nguyên hàm blink
  int original_height = left_eye_height;
  for(int i=0;i<3;i++){
    left_eye_height -= speed;
    right_eye_height -= speed;    
    draw_face();
    delay(1);
  }
  for(int i=0;i<3;i++){
    left_eye_height += speed;
    right_eye_height += speed;
    draw_face();
    delay(1);
  }
  left_eye_height = original_height;
  right_eye_height = original_height;
  draw_face();
}

void sleep() {
  left_eye_height = 2;
  right_eye_height = 2;
  mouth_state = 0; // Miệng bình thường khi ngủ
  draw_face(true);  
}

void wakeup() {
  sleep();
  delay(500);
  for(int h=0; h <= ref_eye_height; h+=2){
    left_eye_height = h;
    right_eye_height = h;
    draw_face(true);
  }
}

// <<< THAY ĐỔI: Hàm happy_eye giờ sẽ làm miệng cười >>>
void happy_eye()
{
  center_eyes(false);
  mouth_state = 1; // Chuyển sang miệng cười
  
  // Vẫn giữ hiệu ứng mắt cười ^^
  int offset = ref_eye_height/2;
  for(int i=0; i<10; i++)
  {
    draw_face(false); // Vẽ lại cả mặt (đã có miệng cười)
    display.fillTriangle(left_eye_x-left_eye_width/2-1, left_eye_y+offset, left_eye_x+left_eye_width/2+1, left_eye_y+5+offset, left_eye_x-left_eye_width/2-1,left_eye_y+left_eye_height+offset,SSD1306_BLACK);
    display.fillTriangle(right_eye_x+right_eye_width/2+1, right_eye_y+offset, right_eye_x-left_eye_width/2-1, right_eye_y+5+offset, right_eye_x+right_eye_width/2+1,right_eye_y+right_eye_height+offset,SSD1306_BLACK);
    offset -= 2;
    display.display();
    delay(1);
  }
  delay(1000);
  center_eyes(true);
}

// <<< THAY ĐỔI: Tạo hoạt cảnh mới "Ngạc nhiên" >>>
void surprise()
{
  center_eyes(false);
  
  // Mắt mở to ra
  left_eye_height = ref_eye_height + 5;
  right_eye_height = ref_eye_height + 5;
  // Miệng hình chữ O
  mouth_state = 2;
  
  draw_face(true);
  delay(2000);
  center_eyes(true);
}

// Các hàm gốc khác giữ nguyên hoặc tạm thời vô hiệu hóa
void saccade(int direction_x, int direction_y){ blink(10); }
void move_right_big_eye() { center_eyes(); delay(500); }
void move_left_big_eye() { center_eyes(); delay(500); }


// --- HÀM SETUP VÀ LOOP CHÍNH ---

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  Serial.begin(115200);
  center_eyes(true);
}

void launch_animation_with_index(int animation_index)
{
  if(animation_index > max_animation_index) {
    animation_index = max_animation_index;
  }

  switch(animation_index)
  {
    case 0: wakeup(); break;
    case 1: center_eyes(true); break;
    case 2: move_right_big_eye(); break;
    case 3: move_left_big_eye(); break;
    case 4: blink(10); break;
    case 5: blink(20); break;
    case 6: happy_eye(); break;
    case 7: sleep(); break;
    case 8: saccade(0,0); break;
    case 9: surprise(); break; // <<< THAY ĐỔI: Thêm case mới
  }
}

void loop() {
  if(demo_mode == 1)
  {
    launch_animation_with_index(current_animation_index++);
    if(current_animation_index > max_animation_index) {
      current_animation_index = 0;
    }
    delay(500);
  }
  
  if(Serial.available()) {
    String data = Serial.readString();
    data.trim();
    char cmd = data[0];
    
    if(cmd == 'A') {
      demo_mode = 0;
      String arg = data.substring(1,data.length());
      int anim = arg.toInt();
      launch_animation_with_index(anim);
      Serial.print("Executed Anim: ");
      Serial.println(arg);
    }
  }
}
