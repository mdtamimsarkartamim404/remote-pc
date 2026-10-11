// LovyanGFX display layer for ESP32-2432S028R ("CYD", plain ESP32, NO PSRAM)
// 2.8" 240x320 ST7789 (use -DCYD_ILI9341 for the ILI9341 variant), landscape = 320x240.
//
// The original UI was laid out for 480x320.  Rather than rewrite 3000 lines of coordinates,
// the Canvas (page sprite) scales every logical 480x320 coordinate to the real 320x240 panel
// and maps the fonts to smaller ones.  The Display (real panel, used for media) is NOT scaled.
#pragma once
#define LGFX_USE_V1
#include <Arduino.h>
#include <LovyanGFX.hpp>

// ---- pins: fixed on the ESP32-2432S028R ----
#define PIN_TFT_SCK  14
#define PIN_TFT_MOSI 13
#define PIN_TFT_MISO 12
#define PIN_TFT_DC    2
#define PIN_TFT_CS   15
#define PIN_TFT_RST  -1          // tied to EN
#ifndef PIN_TFT_BL
#define PIN_TFT_BL   21          // some CYD revisions use 27: build with -DPIN_TFT_BL=27
#endif
#ifndef DISP_SPI_HZ
#define DISP_SPI_HZ 40000000     // ST7789 on the CYD is usually clean at 40 MHz; try 27000000 / 20000000 if noisy
#endif
#ifndef DISP_RGB_ORDER
#define DISP_RGB_ORDER 0
#endif
#ifndef DISP_INVERT
#define DISP_INVERT 0            // your CYD showed a NEGATIVE picture with 1, so 0 is the default (use env color_invert for the other way)
#endif

// logical UI size (what main.cpp draws in) and real panel size
#define UI_W 480
#define UI_H 320
// 8 = RGB332 sprite (76.8 KB) fits next to WiFi on a no-PSRAM ESP32; 16 (153.6 KB) normally does not
#ifndef SPRITE_DEPTH
#define SPRITE_DEPTH 8
#endif

class LGFX_Panel : public lgfx::LGFX_Device {
#ifdef CYD_ILI9341
  lgfx::Panel_ILI9341 _panel;
#else
  lgfx::Panel_ST7789 _panel;
#endif
  lgfx::Bus_SPI _bus;
  lgfx::Light_PWM _light;
public:
  LGFX_Panel() {
    { auto c = _bus.config();
      c.spi_host = SPI2_HOST;           // HSPI: TFT on 14/13/12, the SD card uses VSPI
      c.spi_mode = 0;
      c.freq_write = DISP_SPI_HZ;
      c.freq_read = 16000000;
      c.spi_3wire = false;
      c.use_lock = true;
      c.dma_channel = SPI_DMA_CH_AUTO;
      c.pin_sclk = PIN_TFT_SCK;
      c.pin_mosi = PIN_TFT_MOSI;
      c.pin_miso = -1;                  // read-back not used (GPIO12 is a strapping pin anyway)
      c.pin_dc = PIN_TFT_DC;
      _bus.config(c);
      _panel.setBus(&_bus); }
    { auto c = _panel.config();
      c.pin_cs = PIN_TFT_CS;
      c.pin_rst = PIN_TFT_RST;
      c.pin_busy = -1;
      c.panel_width = 240;
      c.panel_height = 320;
      c.offset_x = 0; c.offset_y = 0;
      c.offset_rotation = 0;
      c.readable = false;
      c.invert = DISP_INVERT;
      c.rgb_order = DISP_RGB_ORDER;
      c.dlen_16bit = false;
      c.bus_shared = false;
      _panel.config(c); }
    { auto c = _light.config();
      c.pin_bl = PIN_TFT_BL;
      c.invert = false;
      c.freq = 5000;
      c.pwm_channel = 7;
      _light.config(c);
      _panel.setLight(&_light); }
    setPanel(&_panel);
  }
};

// colour names (only if the library did not already define them)
#ifndef TFT_BLACK
#define TFT_BLACK 0x0000
#endif
#ifndef TFT_NAVY
#define TFT_NAVY 0x000F
#endif
#ifndef TFT_DARKGREY
#define TFT_DARKGREY 0x7BEF
#endif
#ifndef TFT_LIGHTGREY
#define TFT_LIGHTGREY 0xD69A
#endif
#ifndef TFT_BLUE
#define TFT_BLUE 0x001F
#endif
#ifndef TFT_GREEN
#define TFT_GREEN 0x07E0
#endif
#ifndef TFT_CYAN
#define TFT_CYAN 0x07FF
#endif
#ifndef TFT_RED
#define TFT_RED 0xF800
#endif
#ifndef TFT_MAGENTA
#define TFT_MAGENTA 0xF81F
#endif
#ifndef TFT_YELLOW
#define TFT_YELLOW 0xFFE0
#endif
#ifndef TFT_WHITE
#define TFT_WHITE 0xFFFF
#endif
#ifndef TFT_ORANGE
#define TFT_ORANGE 0xFDA0
#endif

// TFT_eSPI-style datum numbers
#undef TL_DATUM
#undef TC_DATUM
#undef TR_DATUM
#undef ML_DATUM
#undef MC_DATUM
#undef MR_DATUM
#undef BL_DATUM
#undef BC_DATUM
#undef BR_DATUM
#define TL_DATUM 0
#define TC_DATUM 1
#define TR_DATUM 2
#define ML_DATUM 4
#define MC_DATUM 5
#define MR_DATUM 6
#define BL_DATUM 8
#define BC_DATUM 9
#define BR_DATUM 10

// =====================================================================
//  DEVICE COLOUR THEMES: every named colour (TFT_BLACK, TFT_CYAN ...) used by the UI is remapped.
//  order: BLACK NAVY DARKGREY LIGHTGREY BLUE GREEN CYAN RED MAGENTA YELLOW WHITE ORANGE
// =====================================================================
#define C565(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))
#define THEME_COUNT 7
extern bool gThemeActive;
extern uint8_t gTheme;
static const char* const THEME_NAMES[THEME_COUNT] = {"Classic", "Ocean", "Amber terminal", "Matrix", "Sunset", "Mono", "High contrast"};
static const uint16_t THEME_TAB[THEME_COUNT][12] = {
  {0},   // 0 = Classic (no remap)
  {C565(2,12,28),  C565(0,48,96),   C565(90,120,150), C565(190,215,235), C565(40,120,255), C565(0,220,170),  C565(80,220,255),  C565(255,100,110), C565(170,130,255), C565(255,220,110), C565(235,248,255), C565(255,170,90)},
  {C565(0,0,0),    C565(60,30,0),   C565(140,90,20),  C565(220,170,80),  C565(255,150,0),  C565(255,190,40),  C565(255,200,60),  C565(255,80,40),   C565(255,120,60),  C565(255,220,100), C565(255,215,140), C565(255,140,20)},
  {C565(0,0,0),    C565(0,40,0),    C565(0,110,40),   C565(120,230,140), C565(0,160,80),   C565(0,255,70),    C565(120,255,180), C565(255,70,70),   C565(180,255,100), C565(220,255,80),  C565(210,255,215), C565(200,255,40)},
  {C565(18,6,28),  C565(70,20,80),  C565(140,100,150),C565(235,200,210), C565(120,100,255), C565(120,230,150), C565(255,150,180), C565(255,80,90),   C565(255,100,200), C565(255,200,90),  C565(255,240,235), C565(255,140,60)},
  {C565(0,0,0),    C565(40,40,40),  C565(120,120,120),C565(200,200,200), C565(150,150,150), C565(220,220,220), C565(255,255,255), C565(255,70,70),   C565(190,190,190), C565(240,240,240), C565(255,255,255), C565(255,170,60)},
  {C565(0,0,0),    C565(0,0,0),     C565(180,180,180),C565(255,255,255), C565(90,180,255),  C565(0,255,0),     C565(0,255,255),   C565(255,60,60),   C565(255,0,255),   C565(255,255,0),   C565(255,255,255), C565(255,165,0)},
};
static inline uint16_t themeMap(uint16_t c) {
  if (!gTheme || !gThemeActive) return c;
  const uint16_t* t = THEME_TAB[gTheme];
  switch (c) {
    case 0x0000: return t[0];  case 0x000F: return t[1];  case 0x7BEF: return t[2];  case 0xD69A: return t[3];
    case 0x001F: return t[4];  case 0x07E0: return t[5];  case 0x07FF: return t[6];  case 0xF800: return t[7];
    case 0xF81F: return t[8];  case 0xFFE0: return t[9];  case 0xFFFF: return t[10]; case 0xFDA0: return t[11];
  }
  return c;
}

extern bool gDirty;     // set by Canvas drawing calls (sprite needs sending to the panel)

// SC = true  : coordinates/fonts are given in the logical 480x320 UI and scaled to the 320x240 panel
template <class B, bool TRACK, bool SC>
class TftApi : public B {
  struct FM { const lgfx::IFont* f; float mx, my; };
  static FM fontFor(int f) {
    if (!SC) {                                  // real panel: same fonts as before
      switch (f) {
        case 2: return {&lgfx::fonts::Font2, 1.0f, 1.0f};
        case 4: return {&lgfx::fonts::Font4, 1.0f, 1.0f};
        case 6: return {&lgfx::fonts::Font6, 1.0f, 1.0f};
        case 7: return {&lgfx::fonts::Font7, 1.0f, 1.0f};
        case 8: return {&lgfx::fonts::Font8, 1.0f, 1.0f};
        default: return {&lgfx::fonts::Font0, 1.0f, 1.0f};
      }
    }
#ifdef CYD_SMOOTH_FONTS                         // smooth DejaVu fonts: nicer, but ~25% wider than the scaled layout -> text can overlap
    switch (f) {
      case 2: return {&lgfx::fonts::DejaVu9, 1.0f, 1.0f};
      case 4: return {&lgfx::fonts::DejaVu12, 1.0f, 1.0f};
      case 6: return {&lgfx::fonts::DejaVu24, 1.0f, 1.0f};
      case 7: return {&lgfx::fonts::DejaVu24, 1.0f, 1.0f};
      case 8: return {&lgfx::fonts::DejaVu40, 1.0f, 1.0f};
      default: return {&lgfx::fonts::Font0, 1.0f, 1.0f};
    }
#else                                           // DEFAULT: original fonts scaled by EXACTLY the layout factors (x*2/3, y*3/4)
    // -> every string is as wide as in the 480x320 design, so nothing can overlap.
    const float SX = 2.0f / 3.0f, SY = 0.75f;
    switch (f) {
      case 2: return {&lgfx::fonts::Font2, SX, SY};
      case 4: return {&lgfx::fonts::Font4, SX, SY};
      case 6: return {&lgfx::fonts::Font6, SX, SY};
      case 7: return {&lgfx::fonts::Font7, SX, SY};
      case 8: return {&lgfx::fonts::Font8, SX, SY};
      default: return {&lgfx::fonts::Font0, 1.0f, 1.0f};
    }
#endif
  }
  static inline void mark() { if (TRACK) gDirty = true; }
  // logical -> physical (rounded)
  static inline int32_t X(int32_t v) { return SC ? (v * 2 + 1) / 3 : v; }
  static inline int32_t Y(int32_t v) { return SC ? (v * 3 + 2) / 4 : v; }
  static inline int32_t R(int32_t v) { return SC ? (v * 2 + 1) / 3 : v; }
  // extent that avoids gaps between neighbouring scaled rectangles (never collapses to 0)
  static inline int32_t WX(int32_t x, int32_t w) { int32_t r = X(x + w) - X(x); return (w > 0 && r < 1) ? 1 : r; }
  static inline int32_t HY(int32_t y, int32_t h) { int32_t r = Y(y + h) - Y(y); return (h > 0 && r < 1) ? 1 : r; }
  static inline uint16_t TC(uint32_t c) { return SC ? themeMap((uint16_t)c) : (uint16_t)c; }
  float tsz_ = 1.0f;
public:
  using B::B;
  // LovyanGFX datum values: top_left=0, top_center=1, top_right=2, middle_left=4, middle_center=5,
  // middle_right=6, bottom_left=16, bottom_center=17, bottom_right=18
  void setTextDatum(int d) {
    static const uint8_t map[11] = {0, 1, 2, 0, 4, 5, 6, 0, 16, 17, 18};
    B::setTextDatum((lgfx::textdatum::textdatum_t)map[d < 0 || d > 10 ? 0 : d]);
  }
  void setTextSize(float s) { tsz_ = s; }
  void setTextPadding(uint32_t p) { B::setTextPadding(SC ? (uint32_t)X((int32_t)p) : p); }
  size_t drawString(const char* s, int32_t x, int32_t y, int font = 1) {
    mark();
    FM m = fontFor(font);
    B::setFont(m.f);
    B::setTextSize(m.mx * tsz_, m.my * tsz_);
    return B::drawString(s, X(x), Y(y));
  }
  size_t drawString(const String& s, int32_t x, int32_t y, int font = 1) { return drawString(s.c_str(), x, y, font); }
  void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c) { mark(); B::fillRect(X(x), Y(y), WX(x, w), HY(y, h), TC(c)); }
  void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t c) { mark(); B::drawRect(X(x), Y(y), WX(x, w), HY(y, h), TC(c)); }
  void fillScreen(uint32_t c) { mark(); B::fillScreen(TC(c)); }
  void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t c) { mark(); B::drawFastHLine(X(x), Y(y), WX(x, w), TC(c)); }
  void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t c) { mark(); B::drawFastVLine(X(x), Y(y), HY(y, h), TC(c)); }
  void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t c) { mark(); B::drawLine(X(x0), Y(y0), X(x1), Y(y1), TC(c)); }
  void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t c) { mark(); B::drawCircle(X(x), Y(y), R(r), TC(c)); }
  void fillCircle(int32_t x, int32_t y, int32_t r, uint32_t c) { mark(); B::fillCircle(X(x), Y(y), R(r), TC(c)); }
  void setTextColor(uint32_t fg, uint32_t bg) { B::setTextColor(TC(fg), TC(bg)); }
  void setTextColor(uint32_t fg) { B::setTextColor(TC(fg)); }
  // RGB565 pixels in native (little-endian) order, as produced by TJpg_Decoder / AnimatedGIF / the web panel.
  void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t* d) {
    B::pushImage(x, y, w, h, (const lgfx::rgb565_t*)d);
  }
  void setSwapBytes(bool) {}
};
#ifdef CYD_USE_SPRITE
// page sprite in RAM (needs ~77 KB free internal RAM: can starve WiFi/web server on a no-PSRAM ESP32)
class Display : public TftApi<LGFX_Panel, false, false> {};
class Canvas : public TftApi<lgfx::LGFX_Sprite, true, true> {
public:
  explicit Canvas(lgfx::LovyanGFX* parent) : TftApi<lgfx::LGFX_Sprite, true, true>(parent) {}
};
#else
// DEFAULT on the CYD: pages are drawn straight to the panel (no RAM sprite) -> ~77 KB more free heap for WiFi + web UI
class Display : public TftApi<LGFX_Panel, true, true> {};
#endif
