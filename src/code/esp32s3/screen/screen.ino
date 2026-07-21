#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <WiFi.h>

const char* ssid = "WhimsyCam";
const char* password = "12345678";
const int buttonPin = 2;
const char* host = "192.168.4.1";
const int port = 80;

// Must match the server's protocol.
#define CMD_FRAME    0x01
#define CMD_CAPTURE  0x02

bool takePhoto = false;
WiFiClient client;

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
uint16_t lines[240 * 16];

// Connects once and stays connected. Only reconnects if the socket
// has actually dropped - this is what avoids paying for a fresh TCP
// handshake on every single frame.
bool ensureConnected()
{
    if (client.connected())
        return true;

    Serial.println("Connecting...");
    if (!client.connect(host, port))
    {
        Serial.println("Failed");
        return false;
    }

    client.setNoDelay(true); // disable Nagle - cuts per-write latency
    return true;
}

// Blocks until exactly `len` bytes have been read into buf, or the
// connection drops.
bool readExact(uint8_t *buf, size_t len)
{
    size_t got = 0;
    while (got < len)
    {
        if (!client.connected())
            return false;

        int n = client.read(buf + got, len - got);
        if (n > 0)
            got += n;
    }
    return true;
}

bool requestFrame()
{
    uint8_t cmd = CMD_FRAME;
    client.write(&cmd, 1);

    tft.startWrite();
    for (int blockY = 0; blockY < 240; blockY += 16)
    {
        if (!readExact((uint8_t *)lines, sizeof(lines)))
        {
            tft.endWrite();
            return false;
        }
        tft.pushImage(0, blockY, 240, 16, lines);
    }
    tft.endWrite();
    return true;
}

void capturePhoto()
{
    uint8_t cmd = CMD_CAPTURE;
    client.write(&cmd, 1);

    uint8_t resp = 0x00;
    if (readExact(&resp, 1) && resp == 0x01)
        Serial.println("Photo saved!");
    else
        Serial.println("Capture failed");
}

void setup()
{
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP);
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
    static bool lastState = HIGH;
    bool state = digitalRead(buttonPin);
    if (lastState == HIGH && state == LOW)
        takePhoto = true;
    lastState = state;

    if (!ensureConnected())
    {
        delay(500);
        return;
    }

    if (takePhoto)
    {
        takePhoto = false;
        Serial.println("Taking photo...");
        capturePhoto();
    }

    if (!requestFrame())
    {
        Serial.println("Frame read failed, reconnecting...");
        client.stop();
        return;
    }
}
