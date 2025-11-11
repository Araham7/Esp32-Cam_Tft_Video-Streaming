# ESP32-CAM 10 FPS Video Streaming Projects

This document describes two projects that enable **10 FPS video streaming** using an ESP32-CAM module. One project streams the video to a TFT display via Wi-Fi, and the other sets up an ESP32-CAM as an access point to stream raw RGB565 frames over TCP.

---

## **Project 1: ESP32-CAM → TFT Display Streaming**

### **Overview**
This project allows an ESP32 microcontroller with a 1.8" TFT display to receive video frames from an ESP32-CAM over Wi-Fi and display them in real-time at 10 FPS.

---

### **Hardware**
- ESP32-CAM (AI Thinker)
- ESP32 (with TFT 1.8" ST7735 display)
- SPI connection between ESP32 and TFT
- Wi-Fi network for communication

---

### **Libraries**
```cpp
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
```

---

### **TFT Setup**
```cpp
#define TFT_CS  5
#define TFT_DC  2
#define TFT_RST 4
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
```

---

### **Wi-Fi Configuration**
```cpp
const char* ssid = "ESP32-CAM-AP";
const char* password = "12345678";
const char* camIP = "192.168.4.1";
const int camPort = 80;
```

---

### **Frame Settings**
- Resolution: 160×120 (QQVGA)
- Frame buffer: RGB565
- Frame rate: 10 FPS

```cpp
#define WIDTH  160
#define HEIGHT 120
#define FRAME_SIZE (WIDTH*HEIGHT*2)
const int frameInterval = 100; // 100ms = 10 FPS
```

---

### **Loop Logic**
1. Connect to Wi-Fi
2. Connect to ESP32-CAM via TCP
3. Read frame data row by row in chunks (2 KB)
4. Convert chunk data to `uint16_t` RGB565 format
5. Push each row to TFT using `pushColor()`
6. Repeat every 100ms (~10 FPS)

```cpp
for(int row = 0; row < HEIGHT; row++){
    int bytesRead = 0;
    while(bytesRead < WIDTH*2){
        int len = client.read(chunkBuf, min(CHUNK_SIZE, WIDTH*2 - bytesRead));
        if(len > 0){
            for(int i = 0; i < len/2; i++){
                rowBuf[i + bytesRead/2] = ((uint16_t)chunkBuf[i*2] << 8) | chunkBuf[i*2+1];
            }
            bytesRead += len;
            yield();
        }
    }
    tft.setAddrWindow(0, row, WIDTH-1, row);
    for(int x=0; x<WIDTH; x++){
        tft.pushColor(rowBuf[x]);
    }
}
```

---

## **Project 2: ESP32-CAM Raw RGB Streaming Server**

### **Overview**
This project sets up an **ESP32-CAM as an Access Point** and streams raw RGB565 frames over TCP. The client (another ESP32 or PC) can receive these frames and display them.

---

### **Hardware**
- ESP32-CAM (AI Thinker)
- Optional: Any client that can receive TCP frames (ESP32, Raspberry Pi, etc.)

---

### **Libraries**
```cpp
#include "esp_camera.h"
#include <WiFi.h>
```

---

### **Camera Pin Configuration (AI Thinker)**
```cpp
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
```

---

### **Wi-Fi Access Point**
```cpp
const char* ssid = "ESP32-CAM-AP";
const char* password = "12345678";
WiFiServer server(80);
```

---

### **Camera Initialization**
- Frame size: QQVGA (160×120)
- Pixel format: RGB565 (raw)
- Single frame buffer
- 20 MHz XCLK

```cpp
camera_config_t config;
config.xclk_freq_hz = 20000000;
config.pixel_format = PIXFORMAT_RGB565;
config.frame_size = FRAMESIZE_QQVGA;
config.fb_count = 1;
esp_camera_init(&config);
```

---

### **Streaming Logic**
1. Accept TCP client connections
2. Capture raw RGB565 frames using `esp_camera_fb_get()`
3. Send optional frame size header
4. Send raw frame data
5. Repeat at ~25 FPS or configured delay

```cpp
void streamRGB(WiFiClient client){
    digitalWrite(LED_GPIO_NUM, HIGH);
    camera_fb_t *fb;
    while(client.connected()){
        fb = esp_camera_fb_get();
        if(!fb) continue;

        uint32_t frameSize = 160 * 120 * 2;
        client.write((uint8_t*)&frameSize, 4);
        client.write(fb->buf, fb->len);

        esp_camera_fb_return(fb);
        delay(40); // ~25 FPS
        yield();
    }
    digitalWrite(LED_GPIO_NUM, LOW);
}
```

---

### **Setup & Loop**
```cpp
void setup(){
    Serial.begin(115200);
    pinMode(LED_GPIO_NUM, OUTPUT);
    digitalWrite(LED_GPIO_NUM, LOW);

    startCamera();
    WiFi.softAP(ssid, password);
    server.begin();
}

void loop(){
    WiFiClient client = server.available();
    if(client) streamRGB(client);
}
```

---

## **Key Notes**
- **Project 1**: Client (ESP32 + TFT) fetches frames from ESP32-CAM via Wi-Fi.
- **Project 2**: ESP32-CAM acts as a server streaming raw RGB565 frames.
- Both projects use **RGB565 format** to reduce memory usage.
- Frame rate control ensures smooth video without overwhelming the microcontroller.
- `yield()` prevents watchdog timer resets during blocking operations.

---

## **References**
- [ESP32-CAM AI Thinker Pinout](https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/)
- [Adafruit ST7735 Library](https://github.com/adafruit/Adafruit-ST7735-Library)
- [ESP32 Camera Web Server Example](https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/Camera/CameraWebServer)

