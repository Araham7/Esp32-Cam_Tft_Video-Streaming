# ESP32-CAM 10 FPS Video Streaming Projects

This document explains how two projects work that enable **10 FPS video streaming** using an ESP32-CAM module.

---

## **Project 1: ESP32-CAM → TFT Display Streaming**

### **How It Works**

1. The ESP32-CAM captures video frames (160×120 resolution, RGB565 format).
2. The frames are sent over Wi-Fi to an ESP32 microcontroller connected to a TFT display.
3. The ESP32 reads the frame row by row and pushes it to the TFT display.
4. Frame rate control ensures video is displayed smoothly at 10 FPS.

### **Hardware**

* ESP32-CAM (AI Thinker)
* ESP32 with 1.8" TFT ST7735 display
* SPI connection
* Wi-Fi network

### **Flow**

* Connect to Wi-Fi.
* Connect to the ESP32-CAM TCP server.
* Read frame in chunks and push to TFT.
* Repeat every 100ms for the next frame.

---

## **Project 2: ESP32-CAM Raw RGB Streaming Server**

### **How It Works**

1. ESP32-CAM is set up as an Access Point.
2. A TCP client connects (ESP32, Raspberry Pi, etc.).
3. ESP32-CAM captures frames (160×120, RGB565) and sends the raw data to the client.
4. The client can receive and display the frames.
5. Delay and `yield()` are used to prevent watchdog resets.

### **Flow**

* Start ESP32-CAM as an AP.
* Server listens on port 80.
* Frames are continuously sent once a client connects.
* Optional frame size header is sent so the client knows how much data to receive.

---

## **Summary**

* **Project 1**: ESP32-CAM streams video to an ESP32 TFT display.
* **Project 2**: ESP32-CAM acts as a server streaming raw RGB frames.
* RGB565 format is memory-efficient and frame rate control ensures smooth video.
