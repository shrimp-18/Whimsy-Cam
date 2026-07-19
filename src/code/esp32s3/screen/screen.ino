#define LGFX_USE_V1

#include <LovyanGFX.hpp>

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
      cfg.freq_write = 40000000;
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

void setup()
{
  tft.init();
  tft.setRotation(0);

  tft.fillScreen(TFT_RED);
  delay(1000);

  tft.fillScreen(TFT_GREEN);
  delay(1000);

  tft.fillScreen(TFT_BLUE);
  delay(1000);

  tft.fillScreen(TFT_WHITE);
  delay(1000);

  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.drawString("WHIMSY CAM", 40, 105);

  tft.drawCircle(120,120,100,TFT_CYAN);
}

void loop()
{
}