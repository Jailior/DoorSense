#include <WiFi.h>
#include <string>

#include "esp_camera.h"
#include "config.h"
#include "imageprocessing.h"
#include "protocol.hpp"

#define CAMERA_MODEL_XIAO_ESP32S3

#include "camera_pins.h"

void configureCameraPins(camera_config_t* config);

const int16_t port = 12345;
const int16_t interval = 85;
const uint32_t IMAGE_SIZE = 240 * 240; // 240x240 pixels

uint32_t nextTime; 
WiFiClient client;

// Debugging configuration
#define ifdebug if(DEBUG)
const bool DEBUG = false;
unsigned long max_capture_time = 0;
unsigned long max_detect_time = 0;
unsigned long max_send_time = 0;

void setup() {
  // Serial port for debugging
  Serial.begin(115200);
  delay(1000);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Connect to server
  if (!client.connect(SERVER_IP, port)) {
    Serial.println("Connection to server failed");
    return;
  }
  Serial.println("Connected to server");
  
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("Initializing camera...");

  camera_config_t config;
  configureCameraPins(&config);
  config.xclk_freq_hz = 20000000;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = FRAMESIZE_240X240;

#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  Serial.println("Camera initialized.");

  nextTime = millis();
}

void loop() {
  
  static camera_fb_t *fb;

  unsigned long start, t_capture, t_detect, t_send;

  if (millis() - nextTime >= interval) {
    nextTime += interval;

    ifdebug start = millis();
  
    // Get Image
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }
    ifdebug t_capture = millis();

    // Detect edges
    int edgeCount = ImageProcessing::detectEdges(fb->buf, fb->width, fb->height);
    ifdebug Serial.printf("Detected %d edge pixels\n", edgeCount);
    ifdebug t_detect = millis();

    // Release the frame buffer
    esp_camera_fb_return(fb);

    // Send edge count
    uint32_t edge_count = htonl(edgeCount); // Convert to network byte order
    client.write((const char *)&edge_count, sizeof(edge_count));
    ifdebug t_send = millis();

    ifdebug {
      Serial.printf("Timing: Capture: %lu ms, Detect: %lu ms, Send: %lu ms\n", 
                    t_capture - start, t_detect - t_capture, t_send - t_detect);
      if (t_capture - start > max_capture_time) {
        max_capture_time = t_capture - start;
      }
      if (t_detect - t_capture > max_detect_time) {
        max_detect_time = t_detect - t_capture;
      }
      if (t_send - t_detect > max_send_time) {
        max_send_time = t_send - t_detect;
      }
      Serial.printf("Max times: Capture: %lu ms, Detect: %lu ms, Send: %lu ms\n", 
                    max_capture_time, max_detect_time, max_send_time);
    }

    // Check for server connection
    if (!client.connected()) {
      Serial.println("Server disconnected");
      delay(2000);
      client.connect(SERVER_IP, port);
    }
  }
}

/* Function Definitions */

void configureCameraPins(camera_config_t* config) {
  config->ledc_channel = LEDC_CHANNEL_0;
  config->ledc_timer   = LEDC_TIMER_0;
  config->pin_d0       = Y2_GPIO_NUM;
  config->pin_d1       = Y3_GPIO_NUM;
  config->pin_d2       = Y4_GPIO_NUM;
  config->pin_d3       = Y5_GPIO_NUM;
  config->pin_d4       = Y6_GPIO_NUM;
  config->pin_d5       = Y7_GPIO_NUM;
  config->pin_d6       = Y8_GPIO_NUM;
  config->pin_d7       = Y9_GPIO_NUM;
  config->pin_xclk     = XCLK_GPIO_NUM;
  config->pin_pclk     = PCLK_GPIO_NUM;
  config->pin_vsync    = VSYNC_GPIO_NUM;
  config->pin_href     = HREF_GPIO_NUM;
  config->pin_sccb_sda = SIOD_GPIO_NUM;
  config->pin_sccb_scl = SIOC_GPIO_NUM;
  config->pin_pwdn     = PWDN_GPIO_NUM;
  config->pin_reset    = RESET_GPIO_NUM;
}