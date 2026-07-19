#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"

// AI Thinker ESP32-CAM pin definitions
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0

#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {

  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP32-CAM COMPLETE TEST");
  Serial.println("==============================");

  // -----------------------------
  // PSRAM
  // -----------------------------
  if (psramFound()) {
    Serial.println("PSRAM: FOUND");
  } else {
    Serial.println("PSRAM: NOT FOUND");
  }

  // -----------------------------
  // CAMERA
  // -----------------------------
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size = FRAMESIZE_QVGA;
  config.fb_count = 1;
  config.jpeg_quality = 12;

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera Init FAILED (0x%x)\n", err);
  } else {

    Serial.println("Camera Init SUCCESS");

    sensor_t *s = esp_camera_sensor_get();

    if (s) {
      Serial.printf("Sensor PID : 0x%04X\n", s->id.PID);
      Serial.printf("Sensor VER : 0x%02X\n", s->id.VER);
      Serial.printf("MIDH       : 0x%02X\n", s->id.MIDH);
      Serial.printf("MIDL       : 0x%02X\n", s->id.MIDL);
    }

    camera_fb_t *fb = esp_camera_fb_get();

    if (fb) {
      Serial.printf("Capture OK (%u bytes)\n", fb->len);
      esp_camera_fb_return(fb);
    } else {
      Serial.println("Capture FAILED");
    }
  }

  Serial.println();

  // -----------------------------
  // SD CARD TEST
  // -----------------------------

  Serial.println("Trying SD in 4-bit mode...");

  bool mounted = SD_MMC.begin("/sdcard", false);

  if (!mounted) {

    Serial.println("4-bit FAILED");

    Serial.println("Trying SD in 1-bit mode...");

    mounted = SD_MMC.begin("/sdcard", true);

    if (!mounted) {
      Serial.println("1-bit FAILED");
      Serial.println("No SD card mounted.");
      return;
    }
  }

  Serial.println("SD Mounted!");

  uint8_t cardType = SD_MMC.cardType();

  switch (cardType) {

    case CARD_NONE:
      Serial.println("No SD card detected");
      break;

    case CARD_MMC:
      Serial.println("MMC");
      break;

    case CARD_SD:
      Serial.println("SDSC");
      break;

    case CARD_SDHC:
      Serial.println("SDHC/SDXC");
      break;

    default:
      Serial.println("Unknown");
  }

  uint64_t size = SD_MMC.cardSize() / (1024 * 1024);

  Serial.printf("Card Size: %llu MB\n", size);

  File file = SD_MMC.open("/test.txt", FILE_WRITE);

  if (!file) {
    Serial.println("Couldn't create file");
    return;
  }

  file.println("ESP32 SD TEST");

  file.close();

  Serial.println("Write SUCCESS");

  file = SD_MMC.open("/test.txt");

  if (!file) {
    Serial.println("Couldn't reopen file");
    return;
  }

  Serial.println("Contents:");

  while (file.available()) {
    Serial.write(file.read());
  }

  file.close();

  Serial.println();
  Serial.println("DONE");
}

void loop() {
}