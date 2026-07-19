
/*
  ESP32-CAM GC2145 -> Save RGB565 frame as BMP
  NOTE:
  This sketch captures a frame and writes it to /photo.bmp.
  It assumes the frame is RGB565. BMP is written as 24-bit BGR.

  Tested configuration:
  - AI Thinker ESP32-CAM
  - GC2145 sensor
  - SD_MMC
*/

#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"

// AI Thinker pin definitions
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

void write32(File &f, uint32_t v){
  f.write((uint8_t*)&v,4);
}
void write16(File &f, uint16_t v){
  f.write((uint8_t*)&v,2);
}

void setup() {
  Serial.begin(115200);
  delay(1500);

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
  config.jpeg_quality = 12;
  config.fb_count = 1;

 esp_err_t err = esp_camera_init(&config);

if (err != ESP_OK){
  Serial.printf("Camera init failed: 0x%x\n", err);
  return;
}

Serial.println("Camera initialized!");

sensor_t *s = esp_camera_sensor_get();

s->set_whitebal(s, 1);
s->set_awb_gain(s, 1);
s->set_exposure_ctrl(s, 1);
s->set_gain_ctrl(s, 1);

s->set_brightness(s, 1);
s->set_contrast(s, 1);
s->set_saturation(s, 2);

s->set_special_effect(s, 0);

if (s) {
  Serial.println("===== SENSOR INFO =====");
  Serial.printf("PID        : 0x%04X\n", s->id.PID);
  Serial.printf("VER        : 0x%02X\n", s->id.VER);
  Serial.printf("MIDH       : 0x%02X\n", s->id.MIDH);
  Serial.printf("MIDL       : 0x%02X\n", s->id.MIDL);
  Serial.printf("Framesize  : %d\n", s->status.framesize);

  // Print the pixel format that the camera driver is actually using
  pixformat_t fmt = s->pixformat;
  Serial.printf("PixelFormat enum: %d\n", fmt);

  switch (fmt) {
    case PIXFORMAT_RGB565:
      Serial.println("PixelFormat: RGB565");
      break;
    case PIXFORMAT_YUV422:
      Serial.println("PixelFormat: YUV422");
      break;
    case PIXFORMAT_GRAYSCALE:
      Serial.println("PixelFormat: GRAYSCALE");
      break;
    case PIXFORMAT_JPEG:
      Serial.println("PixelFormat: JPEG");
      break;
    case PIXFORMAT_RGB888:
      Serial.println("PixelFormat: RGB888");
      break;
    case PIXFORMAT_RAW:
      Serial.println("PixelFormat: RAW");
      break;
    default:
      Serial.println("PixelFormat: UNKNOWN");
      break;
  }

  Serial.println("=======================");
}

  if(!SD_MMC.begin()){
    Serial.println("SD mount failed");
    return;
  }

  camera_fb_t *fb = esp_camera_fb_get();
  if(!fb){
    Serial.println("Capture failed");
    return;
  }

  const int w = fb->width;
  const int h = fb->height;

  File file = SD_MMC.open("/photo.bmp", FILE_WRITE);
  if(!file){
    Serial.println("Couldn't create photo.bmp");
    esp_camera_fb_return(fb);
    return;
  }

  uint32_t rowSize = ((24*w +31)/32)*4;
  uint32_t imgSize = rowSize*h;
  uint32_t fileSize = 54 + imgSize;

  // BMP header
  file.write('B'); file.write('M');
  write32(file,fileSize);
  write32(file,0);
  write32(file,54);

  // DIB header
  write32(file,40);
  write32(file,w);
  write32(file,h);
  write16(file,1);
  write16(file,24);
  write32(file,0);
  write32(file,imgSize);
  write32(file,2835);
  write32(file,2835);
  write32(file,0);
  write32(file,0);

  uint8_t pad[3]={0,0,0};
  int padding=rowSize-(w*3);

  uint16_t *pix=(uint16_t*)fb->buf;

  // BMP stores bottom-up
  for(int y=h-1;y>=0;y--){
    for(int x=0;x<w;x++){
      uint16_t p = pix[y*w+x];

// Swap the two bytes
p = (p >> 8) | (p << 8);
      uint8_t r=((p>>11)&0x1F)<<3;
      uint8_t g=((p>>5)&0x3F)<<2;
      uint8_t b=(p&0x1F)<<3;
      file.write(b);
      file.write(g);
      file.write(r);
    }
    if(padding) file.write(pad,padding);
  }

  file.close();
  esp_camera_fb_return(fb);

  Serial.println("photo.bmp saved to SD card!");
}

void loop(){}
