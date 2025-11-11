// // /* 10 FPS */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>

// TFT Pins
#define TFT_CS  5
#define TFT_DC  2
#define TFT_RST 4
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Wi-Fi
const char* ssid = "ESP32-CAM-AP";
const char* password = "12345678";

const char* camIP = "192.168.4.1";
const int camPort = 80;

// Frame Size
#define WIDTH  160
#define HEIGHT 120
#define FRAME_SIZE (WIDTH*HEIGHT*2) // RGB565

// Frame Rate Control
unsigned long lastFrameTime = 0;
const int frameInterval = 100; // 10 FPS

// Chunk size (2 KB)
#define CHUNK_SIZE 2048
uint8_t chunkBuf[CHUNK_SIZE];

// TFT row buffer
uint16_t rowBuf[WIDTH];

// Setup
void setup() {
  Serial.begin(115200);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

// Loop
void loop() {
  if(WiFi.status() != WL_CONNECTED) return;
  if(millis() - lastFrameTime < frameInterval) return;
  lastFrameTime = millis();

  WiFiClient client;
  if(!client.connect(camIP, camPort)){
    Serial.println("Failed to connect to ESP32-CAM!");
    delay(500);
    return;
  }

  // Read frame row by row
  for(int row = 0; row < HEIGHT; row++){
    int bytesRead = 0;
    while(bytesRead < WIDTH*2){
      int len = client.read(chunkBuf, min(CHUNK_SIZE, WIDTH*2 - bytesRead));
      if(len > 0){
        for(int i = 0; i < len/2; i++){
          rowBuf[i + bytesRead/2] = ((uint16_t)chunkBuf[i*2] << 8) | chunkBuf[i*2+1];
        }
        bytesRead += len;
        yield(); // prevent watchdog
      }
    }

    // Push row to TFT using pushColor
    tft.setAddrWindow(0, row, WIDTH-1, row);
    for(int x=0; x<WIDTH; x++){
      tft.pushColor(rowBuf[x]);
    }
  }

  client.stop();
}





/* 20-25 fps */
