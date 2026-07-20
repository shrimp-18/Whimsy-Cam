#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"


const char* ssid = "WhimsyCam";
const char* password = "12345678";
WebServer server(80);

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

void handleFrame()
{
    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb)
    {
        server.send(500, "text/plain", "Capture failed");
        return;
    }

    WiFiClient client = server.client();

    const int srcWidth = 320;
    const int dstWidth = 240;
    const int height = 240;

    const uint32_t outSize = dstWidth * height * 2;

    client.printf(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/octet-stream\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n\r\n",
        outSize);

    uint16_t *pixels = (uint16_t *)fb->buf;

    for (int y = 0; y < height; y++)
    {
        // Send the center 240 pixels (skip 40 on each side)
        client.write(
            (uint8_t *)&pixels[y * srcWidth + 40],
            dstWidth * 2);
    }

    client.flush();

    esp_camera_fb_return(fb);
}

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println("Booting...");

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

    config.xclk_freq_hz = 20000000;

    config.pixel_format = PIXFORMAT_RGB565;

    config.frame_size = FRAMESIZE_QVGA;

    config.jpeg_quality = 12;

    config.fb_count = 3;

    Serial.println("Initializing camera...");

    esp_err_t err = esp_camera_init(&config);

    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed: 0x%x\n", err);
        while (true)
            delay(1000);
    }

    Serial.println("Camera OK");

    sensor_t *s = esp_camera_sensor_get();

    s->set_whitebal(s, 1);
    s->set_awb_gain(s, 1);
    s->set_exposure_ctrl(s, 1);
    s->set_gain_ctrl(s, 1);

    s->set_brightness(s, 1);
    s->set_contrast(s, 1);
    s->set_saturation(s, 2);

    WiFi.mode(WIFI_AP);

    Serial.println("Starting AP...");

    WiFi.softAP(ssid, password);

    Serial.print("Camera IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/frame", HTTP_GET, handleFrame);

    server.begin();

    Serial.println("HTTP server started");
    Serial.println("Ready!");
}

void loop()
{
    server.handleClient();
}