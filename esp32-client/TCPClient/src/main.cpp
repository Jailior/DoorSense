#include "esp_camera.h"
#include "config.h"
#include "imageprocessing.h"
#include <WiFi.h>

#define CAMERA_MODEL_XIAO_ESP32S3

#include "camera_pins.h"

void configureCameraPins(camera_config_t* config);

const int16_t port = 12345;

WiFiClient client;

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
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Best option for face detection/recognition
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

  // Get Image
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  Serial.printf("Image width: %u, height: %u, len: %u\n", fb->width, fb->height, fb->len);

  uint8_t *out = (uint8_t *) malloc(fb->len);
  if (!out) {
    Serial.println("Failed to allocate memory for image");
    esp_camera_fb_return(fb);
    return;
  }

  // Convert to grayscale
  // ImageProcessing::convertToGrayscale(fb->buf, fb->width, fb->height, out);
  // Serial.println("Image converted to grayscale");

  // Detect edges
  ImageProcessing::detectEdges(fb->buf, fb->width, fb->height, out);
  Serial.println("Image converted to edges image");

  // Send Image Length
  uint32_t img_len = fb->len;
  img_len = htonl(img_len); // Convert to network byte order
  client.write((const char *)&img_len, sizeof(img_len));

  // Send image data
  size_t bytes_sent = client.write(out, fb->len);
  Serial.printf("Sent %u of %u bytes\n", bytes_sent, ntohl(img_len));

  // Release the frame buffer
  esp_camera_fb_return(fb);
  client.stop();
  Serial.println("Client disconnected");
}

void loop() {
  // Nothing to do here
  delay(1000);
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