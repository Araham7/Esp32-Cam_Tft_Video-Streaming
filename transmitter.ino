/* 10fps */

// #include "esp_camera.h"
// #include <WiFi.h>

// // ===== Camera Model: AI THINKER =====
// #define PWDN_GPIO_NUM 32
// #define RESET_GPIO_NUM -1
// #define XCLK_GPIO_NUM 0
// #define SIOD_GPIO_NUM 26
// #define SIOC_GPIO_NUM 27
// #define Y9_GPIO_NUM 35
// #define Y8_GPIO_NUM 34
// #define Y7_GPIO_NUM 39
// #define Y6_GPIO_NUM 36
// #define Y5_GPIO_NUM 21
// #define Y4_GPIO_NUM 19
// #define Y3_GPIO_NUM 18
// #define Y2_GPIO_NUM 5
// #define VSYNC_GPIO_NUM 25
// #define HREF_GPIO_NUM 23
// #define PCLK_GPIO_NUM 22

// #define LED_GPIO_NUM 4

// // ===== Wi-Fi =====
// const char* ssid = "ESP32-CAM-AP";
// const char* password = "12345678";

// WiFiServer server(80);

// // ===== Camera Init =====
// void startCamera() {
//   camera_config_t config;
//   config.ledc_channel = LEDC_CHANNEL_0;
//   config.ledc_timer = LEDC_TIMER_0;
//   config.pin_d0 = Y2_GPIO_NUM;
//   config.pin_d1 = Y3_GPIO_NUM;
//   config.pin_d2 = Y4_GPIO_NUM;
//   config.pin_d3 = Y5_GPIO_NUM;
//   config.pin_d4 = Y6_GPIO_NUM;
//   config.pin_d5 = Y7_GPIO_NUM;
//   config.pin_d6 = Y8_GPIO_NUM;
//   config.pin_d7 = Y9_GPIO_NUM;
//   config.pin_xclk = XCLK_GPIO_NUM;
//   config.pin_pclk = PCLK_GPIO_NUM;
//   config.pin_vsync = VSYNC_GPIO_NUM;
//   config.pin_href = HREF_GPIO_NUM;
//   config.pin_sscb_sda = SIOD_GPIO_NUM;
//   config.pin_sscb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn = PWDN_GPIO_NUM;
//   config.pin_reset = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 20000000;
//   config.pixel_format = PIXFORMAT_RGB565; // RAW RGB565
//   config.frame_size = FRAMESIZE_QQVGA;    // 160x120
//   config.jpeg_quality = 10;
//   config.fb_count = 1;

//   if(esp_camera_init(&config) != ESP_OK){
//     Serial.println("Camera init failed");
//     while(true);
//   }
// }

// // ===== Stream RGB Frame =====
// void streamRGB(WiFiClient client){
//   digitalWrite(LED_GPIO_NUM, HIGH);
//   camera_fb_t *fb;
//   while(client.connected()){
//     fb = esp_camera_fb_get();
//     if(!fb) continue;

//     // Send raw RGB565 data
//     client.write(fb->buf, fb->len);

//     esp_camera_fb_return(fb);

//     delay(100); // ~10 FPS
//     yield();    // prevent watchdog reset
//   }
//   digitalWrite(LED_GPIO_NUM, LOW);
// }

// // ===== Setup =====
// void setup(){
//   Serial.begin(115200);
//   pinMode(LED_GPIO_NUM, OUTPUT);
//   digitalWrite(LED_GPIO_NUM, LOW);

//   startCamera();
//   WiFi.softAP(ssid, password);
//   Serial.println("AP started!");
//   Serial.print("IP: "); Serial.println(WiFi.softAPIP());

//   server.begin();
// }

// // ===== Loop =====
// void loop(){
//   WiFiClient client = server.available();
//   if(!client) return;

//   streamRGB(client);
// }



#include "esp_camera.h"
#include <WiFi.h>

// ===== Camera Model: AI THINKER =====
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define LED_GPIO_NUM 4

// ===== Access Point =====
const char* ssid = "ESP32-CAM-AP";
const char* password = "12345678";

WiFiServer server(80);

// ===== Camera Init =====
void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565; // RAW RGB565
  config.frame_size = FRAMESIZE_QQVGA;    // 160x120
  config.fb_count = 1;                     // Single buffer

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    while(true);
  }
}

// ===== Stream RGB Frame =====
void streamRGB(WiFiClient client){
  digitalWrite(LED_GPIO_NUM, HIGH);
  camera_fb_t *fb;
  uint16_t width = 160;
  uint16_t height = 120;
  uint32_t frameSize = width * height * 2;

  while(client.connected()){
    fb = esp_camera_fb_get();
    if(!fb) continue;

    // Send frame size header (optional)
    client.write((uint8_t*)&frameSize, 4);

    // Send raw RGB565 frame
    client.write(fb->buf, fb->len);

    esp_camera_fb_return(fb);
    delay(40); // ~25 FPS
    yield();    // prevent watchdog reset
  }
  digitalWrite(LED_GPIO_NUM, LOW);
}

// ===== Setup =====
void setup(){
  Serial.begin(115200);
  pinMode(LED_GPIO_NUM, OUTPUT);
  digitalWrite(LED_GPIO_NUM, LOW);

  startCamera();
  WiFi.softAP(ssid, password);
  Serial.println("AP started!");
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  server.begin();
}

// ===== Main Loop =====
void loop(){
  WiFiClient client = server.available();
  if(!client) return;

  Serial.println("Client connected, streaming raw RGB...");
  streamRGB(client);
  Serial.println("Client disconnected.");
}
