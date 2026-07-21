#include <WiFi.h>
#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"

const char* ssid = "WhimsyCam";
const char* password = "12345678";

const int tcpPort = 80;
WiFiServer tcpServer(tcpPort);
WiFiClient tcpClient;

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

// --- Minimal binary protocol ---------------------------------------
// Client sends a single command byte, server replies:
//   CMD_FRAME   (0x01) -> raw 240x240 RGB565 bytes (115200 bytes), no header
//   CMD_CAPTURE (0x02) -> saves a BMP to SD, replies with 1 status byte
//                          (0x01 = ok, 0x00 = fail)
// The connection is left open between commands so we never pay for a
// fresh TCP handshake per frame.
#define CMD_FRAME    0x01
#define CMD_CAPTURE  0x02

void sendFrame(WiFiClient &client)
{
    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb)
    {
        // Nothing to stream - drop the connection so the client notices
        // instead of blocking forever waiting for bytes that never come.
        client.stop();
        return;
    }

    const int srcWidth = 320;
    const int dstWidth = 240;
    const int height = 240;

    uint16_t *pixels = (uint16_t *)fb->buf;

    for (int y = 0; y < height; y++)
    {
        client.write(
            (const uint8_t *)&pixels[y * srcWidth + 40],
            dstWidth * 2);
    }

    esp_camera_fb_return(fb);
}

bool savePhotoBMP()
{
    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb)
    {
        Serial.println("Capture failed");
        return false;
    }

    static int photoNum = 1;

    char filename[32];
    sprintf(filename, "/IMG%04d.bmp", photoNum++);

    File file = SD_MMC.open(filename, FILE_WRITE);

    if (!file)
    {
        Serial.println("Couldn't create file");
        esp_camera_fb_return(fb);
        return false;
    }

    const int width = 320;
    const int height = 240;

    const uint32_t rowSize = (width * 3 + 3) & ~3;
    const uint32_t imageSize = rowSize * height;
    const uint32_t fileSize = 54 + imageSize;

    uint8_t header[54] = {
        'B','M',
        0,0,0,0,
        0,0,0,0,
        54,0,0,0,
        40,0,0,0,
        0,0,0,0,
        0,0,0,0,
        1,0,
        24,0,
        0,0,0,0,
        0,0,0,0,
        0x13,0x0B,0,0,
        0x13,0x0B,0,0,
        0,0,0,0,
        0,0,0,0
    };

    header[2] = fileSize;
    header[3] = fileSize >> 8;
    header[4] = fileSize >> 16;
    header[5] = fileSize >> 24;

    header[18] = width;
    header[19] = width >> 8;
    header[20] = width >> 16;
    header[21] = width >> 24;

    header[22] = height;
    header[23] = height >> 8;
    header[24] = height >> 16;
    header[25] = height >> 24;

    header[34] = imageSize;
    header[35] = imageSize >> 8;
    header[36] = imageSize >> 16;
    header[37] = imageSize >> 24;

    file.write(header, 54);

    uint16_t *pixels = (uint16_t *)fb->buf;

    uint8_t row[rowSize];

    for (int y = height - 1; y >= 0; y--)
    {
        int idx = 0;

        for (int x = 0; x < width; x++)
        {
            uint16_t p = pixels[y * width + x];
            p = (p >> 8) | (p << 8);

            uint8_t r = ((p >> 11) & 0x1F) << 3;
            uint8_t g = ((p >> 5) & 0x3F) << 2;
            uint8_t b = (p & 0x1F) << 3;

            row[idx++] = b;
            row[idx++] = g;
            row[idx++] = r;
        }

        while (idx < rowSize)
            row[idx++] = 0;

        file.write(row, rowSize);
    }

    file.close();
    esp_camera_fb_return(fb);

    Serial.print("Saved ");
    Serial.println(filename);

    return true;
}

void handleTcp()
{
    // Accept a new client if we don't currently have one connected.
    if (!tcpClient || !tcpClient.connected())
    {
        tcpClient = tcpServer.available();
        return;
    }

    if (tcpClient.available() > 0)
    {
        uint8_t cmd = tcpClient.read();

        switch (cmd)
        {
            case CMD_FRAME:
                sendFrame(tcpClient);
                break;

            case CMD_CAPTURE:
            {
                bool ok = savePhotoBMP();
                uint8_t resp = ok ? 0x01 : 0x00;
                tcpClient.write(&resp, 1);
                break;
            }

            default:
                // Unknown command byte - drop the connection rather than
                // risk getting permanently out of sync with the client.
                tcpClient.stop();
                break;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    delay(2000);

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

    if (!SD_MMC.begin())
    {
        Serial.println("SD Mount Failed");
        while (true)
            delay(1000);
    }

    Serial.println("SD Ready");

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

    tcpServer.begin();
    tcpServer.setNoDelay(true); // disable Nagle - cuts per-write latency

    savePhotoBMP();

    Serial.println("TCP server started");
    Serial.println("Ready!");
}

void loop()
{
    handleTcp();
}
