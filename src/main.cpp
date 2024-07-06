

// ***
// the below _secret.h just define macros like:
// #define WIFI_SSID           "ssid"
// #define WIFI_PASSWORD       "password"
// ***
#include "_secret.h"



// #if defined(FOR_PICOW)
//   #define TFT_BL    21
//   #define TFT_CS    17
//   #define TFT_DC    16
//   #define TFT_SCLK  18
//   #define TFT_MOSI  19
//   #define TFT_RST   20
// #endif

// #if defined(FOR_PICOW)
//   //#define STORAGE_FS LittleFS
//   #define MAX_IMAGE_COUNT     10
// #elif defined(FOR_TCAMERAPLUS)
//   #include <FS.h>
//   #include <LittleFS.h>
//   //#define MAX_IMAGE_COUNT     20
//   //#define STORAGE_FS LittleFS
//   #define MAX_IMAGE_COUNT     10
// #endif

#include "tft_image_show/tft_image_show.ino"