#ifndef CONSTS_HPP_
#define CONSTS_HPP_

#define MAX_ANT_COUNT     10
#define MAX_RULE_LENGTH   20

// Timeout[ms]
#define TIMEOUT_MS_SEMAPHORE 5000
#define TIMEOUT_MS_WIFICONN  30000

// Display size
#define DISPLAY_HEIGHT 280
#define DISPLAY_WIDTH  240

// Colors
#define COLOR_BACKGROUND    0x0000
#ifdef GRAD_RGB_PALETTE
#define COLOR_PALETTE_START 0xf81f
#define COLOR_PALETTE_MID   0x03e0
#define COLOR_PALETTE_END   0xffe0
#else
#define COLOR_PALETTE_START 0x8f71
#define COLOR_PALETTE_END   0xfdb7
#endif
#define COLOR_FOREGROUND    0xFFFF
#define COLOR_FOREGROUND_OK 0xffe0
#define COLOR_FOREGROUND_NG 0xf800

// Pin numbers
#define PIN_TFT_CS   17
#define PIN_TFT_RST  22
#define PIN_TFT_DC   28
#define PIN_TFT_MOSI 19
#define PIN_TFT_SCK  18
#define PIN_TFT_BL   16
#define PIN_BUTTON   20
#define PIN_VOLUME   27

// Network
#define SERVER_PORT 8888
#define STASSID "XXX"
#define STAPSK  "XXX"

#endif
