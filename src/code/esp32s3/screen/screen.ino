#define LGFX_USE_V1

#include <LovyanGFX.hpp>
#include <WiFi.h>
const char* ssid = "WhimsyCam";
const char* password = "12345678";

const char* host = "192.168.4.1";
const int port = 80;

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_GC9A01 _panel;
  lgfx::Bus_SPI _bus;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus.config();

      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 80000000;
      cfg.freq_read  = 16000000;

      cfg.pin_sclk = 18;
      cfg.pin_mosi = 17;
      cfg.pin_miso = -1;
      cfg.pin_dc   = 5;

      _bus.config(cfg);
      _panel.setBus(&_bus);
    }

    {
      auto cfg = _panel.config();

      cfg.pin_cs           = 16;
      cfg.pin_rst          = 4;
      cfg.pin_busy         = -1;

      cfg.memory_width     = 240;
      cfg.memory_height    = 240;
      cfg.panel_width      = 240;
      cfg.panel_height     = 240;

      cfg.offset_x         = 0;
      cfg.offset_y         = 0;

      cfg.offset_rotation  = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits  = 1;
      cfg.readable         = false;
      cfg.invert           = true;
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       = false;

      _panel.config(cfg);
    }

    setPanel(&_panel);
  }
};

LGFX tft;
uint16_t lines[240*16];
void setup()
{
  Serial.begin(115200);

  tft.init();
  tft.setSwapBytes(false);
  tft.setRotation(0);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Connecting...", 40, 100);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected!");

  tft.fillScreen(TFT_GREEN);
  tft.drawString("WiFi OK", 60, 110);
}

void loop()
{
    WiFiClient client;

    Serial.println("Connecting...");

    if (!client.connect(host, port))
    {
        Serial.println("Failed");
        delay(1000);
        return;
    }

    client.print(
        "GET /frame HTTP/1.1\r\n"
        "Host: 192.168.4.1\r\n"
        "Connection: close\r\n\r\n");

    // Skip HTTP header
    String lineStr;

    while (true)
    {
        lineStr = client.readStringUntil('\n');
        lineStr.trim();

        if (lineStr.length() == 0)
            break;
    }

   tft.startWrite();

for (int blockY = 0; blockY < 240; blockY += 16)
{
    // Receive 16 full rows (320 pixels each)
    uint8_t* ptr = (uint8_t*)lines;
    int bytesLeft = 240 * 16 * 2;

    while (bytesLeft > 0)
    {
        int n = client.read(ptr, bytesLeft);

        if (n > 0)
        {
            ptr += n;
            bytesLeft -= n;
        }
    }

    // Crop into a packed 240x16 buffer
    static uint16_t cropped[240 * 16];

    for (int r = 0; r < 16; r++)
    {
        memcpy(
            &cropped[r * 240],
            &lines[r * 320 + 40],
            240 * sizeof(uint16_t)
        );
    }

    tft.pushImage(
    0,
    blockY,
    240,
    16,
    lines
);
}

tft.endWrite();

    client.stop();

    Serial.println("Frame drawn");

}