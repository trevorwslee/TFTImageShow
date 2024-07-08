---
title: Simple Arduino Framework Photo Frame Implementation with Photos Downloaded from the Internet via DumbDisplay
description: Simple Arduino Framework Raspberry Pi Pico / ESP32 TFT LCD Photo Frame Implementation with Photos Downloaded from the Internet via DumbDisplay
tags: 'raspberrypipico, esp32, spitftlcd'
cover_image: ./imgs/MAIN.jpg
published: false
id: 1914271
---

# Simple Arduino Framework Raspberry Pi Pico / ESP32 TFT LCD Photo Frame Implementation with Photos Downloaded from the Internet via DumbDisplay

The target of this [project](https://github.com/trevorwslee/TFTImageShow) is to implement, mostly the software part, of a simple Arduino framework photos / images showing "photo frame" using Raspberry Pi Pico or ESP32 with photos / images downloaded from the Internet via DumbDisplay -- an Android app running on your Android phone.

The microcontroller program here is developed in Arduino framework using VS Code and PlatformIO, in the similar fashion as described by the post -- [A Way to Run Arduino Sketch With VSCode PlatformIO Directly](https://www.instructables.com/A-Way-to-Run-Arduino-Sketch-With-VSCode-PlatformIO/)

The simple remote UI for downloading photos / images from the Internet is realized with the help of the DumbDisplay Android app. For a brief description of DumbDisplay, you may want to refer to the post -- [Blink Test With Virtual Display, DumbDisplay](https://www.instructables.com/Blink-Test-With-Virtual-Display-DumbDisplay/)

Please note that
* The UI is driven by the microcontroller program. I.e., the control flow of the UI is programmed by the sketch.
* The downloaded image -- be it in **Jpeg** or **PNG** format -- will be transferred to the microcontroller board in **Jpeg** format, scaled to site inside the TFT LCD screen.

For Raspberry Pi Pico board (WiFi), a ST7789 2.8 inch 240x320 SPI TFT LCD screen is attached to a Raspberry Pi PicoW board.
The TFT LCD module library used is the `Adafruit-ST7735-Library` Arduino library.

For ESP32, LiLyGo TDisplay / TCamera Plus board is used. 
The TFT LCD module library use is the `bodmer/TFT_eSPI` Arduino library.

![](imgs/tft_image_show.gif)


In all cases, the **Jpeg** library used is the `bodmer/TJpg_Decoder` Arduino library.

A simple flash-based **LittleFS** file-system is allocated for storing the saved **Jpeg** images.

The microcontroller board has two running modes:

1) When connected to the DumbDisplay Android app (using WiFi), a simple UI is provided for downloading images from some predefined sites,
   as well as for transferring the downloaded image in **Jpeg** format to the microcontroller board.
   Note that the predefined sites is hardcoded in the sketch that you can conveniently change as desired by changing the sketch.
2) When not connected to the DumbDisplay Android app, the microcontroller cycles through the saved **Jpeg** images displaying them to the TFT LCD 
   screen one by one like a simple "photo frame". Note that since the images are stored in **LittleFS**, they will survive even after reboot of the
   microcontroller board.

***Connect for the UI; disconnect to enjoy "photo frame" slide show.***    

# The UI

|  |  |
|--|--|
|![](imgs/UI_00.png)|![](imgs/UI_01.png)|

The first time connected, an image download will be initiated automatically.
After downloading an image the image will be transferred to the microcontroller board in **Jpeg** format and be displayed to the TFT LCD screen.

You can choose to save the transferred image by clicking the ***💾Save*** button.
Notice that the [7-segment] *number* displayed next to the ***Saved🗂️*** label will be bumped up after saving.
The *number* indicates the number of images saved to the microcontroller's **LittleFS** storage.

If you so desired, you can turn on auto-save by clicking the ***Auto*** save button. (You turn off auto-save by clicking the button again.)

If you want to initiate another download of image, click on the canvas that shows the downloaded image.

If you want to delete all the saved images, double-click on the [7-segment] *number*.

After you are done with downloading and saving images, disconnect the microcontroller.
After disconnection, the "photo frame" slide show begins in the microcontroller side.

Anytime you want to change the saved images, reconnect to DumbDisplay Android app.

***Connect for the UI; disconnect to enjoy "photo frame" slide show.***    

# Wiring TFT LCD Module

For LiLyGo TDisplay / TCamera Plus board, the TFT LCD screen is in-built the microcontroller board; hence, no need for additional wiring.

For Raspberry Pi Pico board, as mentioned previously, a ST7789 2.8 inch 240x320 SPI TFT LCD module is used; hence, some wiring is necessary


![](imgs/TFT_WIRE.jpg)


|Raspberry Pi Pico|SPI TFT LCD |
|-----------------|------------|
| 3V3             | VCC        |
| GND             | GND        |
| GP21            | BL         |
| GP17            | CS         |
| GP16            | RS / DC    |
| GP18            | CLK / SCLK |
| GP19            | SDA / MOSI |
| GP20            | RST        |



# Developing and Building

As mentioned previously, the sketch will be developed using VS Code and PlatformIO.
Please clone the PlatformIO project [TFTImageShow](https://github.com/trevorwslee/TFTImageShow) GitHub repository.

The configurations for developing and building of the sketch in basically captured in the `platformio.ini` file
```
[env]
monitor_speed = 115200

[env:PICOW]  ; ensure long file name support ... git config --system core.longpaths true
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = rpipicow
framework = arduino
board_build.core = earlephilhower
board_build.filesystem = littlefs
board_build.filesystem_size = 1m
lib_deps =
    https://github.com/trevorwslee/Arduino-DumbDisplay
    https://github.com/adafruit/Adafruit-ST7735-Library.git
    https://github.com/adafruit/Adafruit-GFX-Library
    https://github.com/Bodmer/TJpg_Decoder.git 
    Wire
    SPI
    https://github.com/adafruit/Adafruit_BusIO 
build_flags =
    -D FOR_PICOW

[env:TDISPLAY]
platform = espressif32
board = esp32dev
framework = arduino
board_build.filesystem = littlefs
lib_deps =
    https://github.com/trevorwslee/Arduino-DumbDisplay
    bodmer/TFT_eSPI     ; Setup25_TTGO_T_Display
    bodmer/TJpg_Decoder
    LittleFS
build_flags =
    -D FOR_TDISPLAY

[env:TCAMERAPLUS]
platform = espressif32
board = esp32dev
framework = arduino
board_build.filesystem = littlefs
lib_deps =
    https://github.com/trevorwslee/Arduino-DumbDisplay
    bodmer/TFT_eSPI      ; modify User_Setup_Select.h ... Setup44_TTGO_CameraPlus
    bodmer/TJpg_Decoder
    LittleFS
    Wire
    SPI
    SPIFFS
build_flags =
    -D FOR_TCAMERAPLUS
```

***Please make sure you select the correct PlatformIO project environment*** -- `PICOW` / `TDISPLAY` / `TCAMERAPLUS`

For `PICOW`, the platform core is download from `https://github.com/maxgerhardt/platform-raspberrypi.git`.
(As far as I know, this is the only PlatformIO platform core that supports the use of Raspberry Pi PicoW WiFi capability.)

It might take a long time for PlatformIO to download and install it.
If PlatformIO fails to download and install the platform core, it might be that your system doesn't have long "file name" enabled, in such a case, try 
```
git config --system core.longpaths true
```  

For `TDISPLAY` that uses `bodmer/TFT_eSPI`, you will need to modify the installed `.pio/libdeps/TDISPLAY/TFT_eSPI/User_Set_Select.h`
to use `User_Setups/Setup25_TTGO_T_Display.h` rather than the default `User_Setup.h` like
```
...
//#include <User_Setup.h>           // Default setup is root library folder
...
#include <User_Setups/Setup25_TTGO_T_Display.h>    // Setup file for ESP32 and TTGO T-Display ST7789V SPI bus TFT
...

```  

For `TCAMERAPLUS` which also uses `bodmer/TFT_eSPI`, modify `User_Set_Select.h` similarly
```
...
//#include <User_Setup.h>           // Default setup is root library folder
...
#include <User_Setups/Setup44_TTGO_CameraPlus.h>   // Setup file for ESP32 and TTGO T-CameraPlus ST7789 SPI bus TFT    240x240
...

```  

The program entry point is `src/main.cpp`

```
// ***
// the below _secret.h just define macros like:
// #define WIFI_SSID           "your-wifi-ssid"
// #define WIFI_PASSWORD       "your-wifi-password"
// ***
#include "_secret.h"

#include "tft_image_show/tft_image_show.ino"
```

Notice there are two **included** files -- `_secret.h` and `tft_image_show/tft_image_show.ino` -- in the `src` directory

You will need to create the `_secret.h` with content like
```
#define WIFI_SSID           "your-wifi-ssid"
#define WIFI_PASSWORD       "your-wifi-password"
```

With these macros for accessing your WiFi, the microcontroller board will connect to DumbDisplay Android app using WiFi.
If you do not want to use WiFi, simply don't provide them.
In such a case, connection to DumbDisplay Android app is assumed to be using serial UART (slower) via an OTG adapter.
Please refer to the above mentioned post -- [Blink Test With Virtual Display, DumbDisplay](https://www.instructables.com/Blink-Test-With-Virtual-Display-DumbDisplay/)

# The Sketch

The sketch of the project is `tft_image_show/tft_image_show.ino`. You can [easily] customize some aspects of the sketch

```
...
// NEXT_S defines the delay (in seconds) to show next saved image
#define NEXT_S              5
...
// MAX_IMAGE_COUNT define that maximum number of images that can be saved
// set MAX_IMAGE_COUNT to 0 to force reformat the storage
#define MAX_IMAGE_COUNT     10
...
// getDownloadImageURL() returns a URL to download an image; add / remove sites as needed
// download image bigger than needed (on purpose)
const String urls[] = {
  String("https://loremflickr.com/") + String(2 * TFT_WIDTH) + String("/") + String(2 * TFT_HEIGHT),
  String("https://picsum.photos/") + String(2 * TFT_WIDTH) + String("/") + String(2 * TFT_HEIGHT),
};
const char* getDownloadImageURL() {
  int idx = random(2);
  return urls[idx].c_str();
}
...
```
* The slide show delay is defined by the macro `NEXT_S`, which default to 5 seconds
* The maximum number of saved images is defined by the macro `MAX_IMAGE_COUNT`, which default to 10.
  Note that if you set `MAX_IMAGE_COUNT`, flash and run the sketch, the **LittleFS** storage will be reformatted.
  For normal running, `MAX_IMAGE_COUNT` should be at lease 1.
* You can modify `urls` / `getDownloadImageURL()` to add / remove Internet sites for downloading images.  

# Sketch Highlight -- TFT LCD Library `Adafruit-ST7735-Library` 

Here is how `Adafruit-ST7735-Library` used in the sketch.

First a global `tft` object is defined like
```
  #define A_TFT_BL    21
  #define A_TFT_CS    17
  #define A_TFT_DC    16
  #define A_TFT_SCLK  18
  #define A_TFT_MOSI  19
  #define A_TFT_RST   20
  #define TFT_WIDTH   320
  #define TFT_HEIGHT  240
  #include <Adafruit_ST7789.h>
  Adafruit_ST7789 tft(A_TFT_CS, A_TFT_DC, A_TFT_RST);
```
Notice the pin assignments exactly match the wiring described previously.

Then in `setup()`
```
  pinMode(A_TFT_BL, OUTPUT);
  digitalWrite(A_TFT_BL, 1);  // light it up
  tft.init(240, 320, SPI_MODE0);
  tft.invertDisplay(false);
  tft.setRotation(1);
  tft.setSPISpeed(40000000);
```
* The back-light part is obvious.
* The TFT LCD screen size is 240x320.
* Why `SPI_MODE0` and other settings? Simply, they work for me. 

# Sketch Highlight -- TFT LCD Library `bodmer/TFT_eSPI` 

First a global `tft` object is defined like
```
  #include <TFT_eSPI.h>
  TFT_eSPI tft = TFT_eSPI();
```

Then in `setup()`
```
  tft.init();
  tft.setRotation(0);
```

# Sketch Highlight -- Jpeg Library `bodmer/TJpg_Decoder` 

Include the needed headers
```
#include <TJpg_Decoder.h>
```

In `setup()`
```
#if defined(TFT_ESPI_VERSION)
  TJpgDec.setSwapBytes(true);
#endif
  TJpgDec.setCallback(tft_output);
```
Why `setSwapBytes(true)`? Since it seems to work that way.

And here is the *callback* `tft_output`, which is mostly copied from an example of `bodmer/TJpg_Decoder`
```
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;
  // This function will clip the image block rendering automatically at the TFT boundaries
#if defined(TFT_ESPI_VERSION)
  tft.pushRect(x, y, w, h, bitmap);
#else
  tft.drawRGBBitmap(x, y, bitmap, w, h);
#endif
  // Return 1 to decode next block
  return 1;
}
```
Notice that in case of using `bodmer/TFT_eSPI`, the way is draw the decoded **Jpeg** chunk is 
```
  tft.pushRect(x, y, w, h, bitmap);
```
If `Adafruit-ST7735-Library` is use, the way to draw the decoded **Jpeg** chunk is
```
  tft.drawRGBBitmap(x, y, bitmap, w, h);
```

After setting `TJpgDec` up, a **Jpeg** image can be drawn like
```
  TJpgDec.drawJpg(x, y, jpegImage.bytes, jpegImage.byteCount);
```


# Sketch Highlight -- **LittleFS** 


Include the needed headers
```
#include <FS.h>
#include <LittleFS.h>
```

In `setup()`
```
 LittleFS.begin();
```

If you want to format the **LittleFS**, call `format()` like
```
  LittleFS.format()
```

**Jpeg** image can be saved like
```
  File f = LittleFS.open(fileName, "w");
  if (f) {
    f.println(currentJpegImage.width);
    f.println(currentJpegImage.height);
    f.println(currentJpegImage.byteCount);
    f.write(currentJpegImage.bytes, currentJpegImage.byteCount);
    f.close();
  }
```
Notice that not only the **Jpeg** image bytes got written to the file, the various metadata are saved first. (I.e. the file is not a normal **JPG** file, but a customized one.)

And **Jpeg** image can be read back like
```
  File f = LittleFS.open(fileName, "r");
  if (f) {
    int width = f.readStringUntil('\n').toInt();
    int height = f.readStringUntil('\n').toInt();
    int byteCount = f.readStringUntil('\n').toInt();
    uint8_t* bytes = new uint8_t[byteCount];
    f.readBytes((char*) bytes, byteCount);
    f.close();
    tempImage.width = width;
    tempImage.height = height;
    tempImage.byteCount = byteCount;
    tempImage.bytes = bytes;
  }
```


# Sketch Highlight -- DumbDisplay

Like all other use cases of using DumbDisplay, you first declare a global `DumbDisplay` object `dumbdisplay`
```
#if defined(WIFI_SSID)
  #include "wifidumbdisplay.h"
  DumbDisplay dumbdisplay(new DDWiFiServerIO(WIFI_SSID, WIFI_PASSWORD));
#else
  #include "dumbdisplay.h"
  DumbDisplay dumbdisplay(new DDInputOutput());
#endif
```
There, the new configured `DDWiFiServerIO` is one of the few ways to establish connection with DumbDisplay Android app.
Like in "no WiFi" case, Serial UART `DDInputOutput` is used.

Then, several global helper objects / pointers are declared
```
DDMasterResetPassiveConnectionHelper pdd(dumbdisplay);
GraphicalDDLayer* imageLayer;
LcdDDLayer* saveButtonLayer;
LcdDDLayer* autoSaveOptionLayer;
LcdDDLayer* savedCountLabelLayer;
SevenSegmentRowDDLayer* savedCountLayer;
SimpleToolDDTunnel* webImageTunnel;
ImageRetrieverDDTunnel* imageRetrieverTunnel = NULL;
```
* The `DDMasterResetPassiveConnectionHelper` global object `pdd` is a helper for managing connect and reconnection with DumbDisplay app.
* The `GraphicalDDLayer` pointer `imageLayer` is the `canvas` to which downloaded image is drawn to.
  Like other layers, they the layer is created later, in this case, when DumbDisplay app is created.
* The `LcdDDLayer` pointers `saveButtonLayer`, `autoSaveOptionLayer` and `savedCountLabelLayer` are for **save** button, *auto-save* button, and *saved-count* label respectively.
* The `SevenSegmentRowDDLayer` pointer `savedCountLayer` is for show the saved image count.
* The `SimpleToolDDTunnel` pointer `webImageTunnel` is for download image to DumbDisplay Android app purpose. It will be created together with other layers and `tunnels`.
* The `ImageRetrieverDDTunnel` pointer `imageRetrieverTunnel` is for retrieving the data of the downloaded image, in **Jpeg** format.

The life-cycle of the above DumbDisplay layers and "tunnels" are managed by the global `pdd` object, which will monitor connection and disconnection of
DumbDisplay app, calling appropriate user-defined functions as well as DumbDisplay functions in the appropriate time. It is cooperatively given "time slices" in the `loop()` block like
```
  void loop() {
    ...
    pdd.loop(initializeDD, updateDD);
    ...
  }
```

The `initializeDD` is the function defined in the sketch that is supposed to create the various layer and tunnel objects.
```
void initializeDD() {
  tft.fillScreen(COLOR_BG);
  // create a graphical layer for drawing the downloaded web image to
  imageLayer = dumbdisplay.createGraphicalLayer(2 * TFT_WIDTH, 2 * TFT_HEIGHT);
  ...
  // create a LCD layer for the save button
  saveButtonLayer = dumbdisplay.createLcdLayer(6, 2);
  ...
  // create a LCD layer for the auto save option
  autoSaveOptionLayer = dumbdisplay.createLcdLayer(6, 1);
  ...
  // create a LCD layer as the label for the number of saved images
  savedCountLabelLayer = dumbdisplay.createLcdLayer(8, 1);
  ...
  // create a 7-segment layer for showing the number of saved images
  savedCountLayer = dumbdisplay.create7SegmentRowLayer(2);
  ...
  // create a tunnel for downloading web image ... initially, no URL yet ... downloaded.png is the name of the image to save
  webImageTunnel = dumbdisplay.createImageDownloadTunnel("", "downloaded.png");
  ...
  // create a tunnel for retrieving JPEG image data from DumbDisplay app storage
  imageRetrieverTunnel = dumbdisplay.createImageRetrieverTunnel();
  // auto pin the layers
  dumbdisplay.configAutoPin(DDAutoPinConfig('V')
    .addLayer(imageLayer)
    .beginGroup('H')
      ...
    .endGroup()
    .build());
}
```

The `updateDD` is the function define in the sketch that is supposed to receive "time slices" to update / act-on the layer and tunnel objects.
```
  bool isFirstUpdate = !pdd.firstUpdated();
  bool updateUI = isFirstUpdate;
  if (autoSaveOptionLayer->getFeedback() != NULL) {
    // toggle auto save
    autoSave = !autoSave;
    updateUI = true;
  }
  if (updateUI) {
    if (autoSave) {
      autoSaveOptionLayer->writeLine("Auto✅️"); 
    } else {
      autoSaveOptionLayer->writeLine("Auto⛔");
    }
  }
  ...
  if (isFirstUpdate || state == NOTHING) {
    if (isFirstUpdate || imageLayer->getFeedback() != NULL) {
      // trigger download image
      saveButtonLayer->disabled(true);
      imageLayer->noBackgroundColor();
      state = DOWNLOADING_FOR_IMAGE;
    }
    return;
  }
  if (state == DOWNLOADING_FOR_IMAGE) {
    // set the URL to download web image 
    currentJpegImage.release();
    String url = getDownloadImageURL();
    webImageTunnel->reconnectTo(url);
    imageLayer->clear();
    imageLayer->write("downloading image ...");
    state = WAITING_FOR_IMAGE_DOWNLOADED;
    return;
  }
  if (state == WAITING_FOR_IMAGE_DOWNLOADED) {
    int result = webImageTunnel->checkResult();
    if (result == 1) {
      // web image downloaded ... retrieve JPEG data of the image
      imageRetrieverTunnel->reconnectForJpegImage("downloaded.png", TFT_WIDTH, TFT_HEIGHT);
      imageLayer->clear();
      imageLayer->drawImageFileFit("downloaded.png");
      state = RETRIEVING_IMAGE;
      retrieveStartMillis = millis();
    } else if (result == -1) {
      // failed to download the image
      imageLayer->clear();
      imageLayer->write("!!! failed to download image !!!");
      dumbdisplay.writeComment("XXX failed to download XXX");
      state = NOTHING;
    }
    return;
  }
  if (state == RETRIEVING_IMAGE) {
    // read the retrieve image (if it is available)
    DDJpegImage jpegImage;
    bool retrievedImage = imageRetrieverTunnel->readJpegImage(jpegImage);
    if (retrievedImage) {
      unsigned long retrieveTakenMillis = millis() - retrieveStartMillis;
      dumbdisplay.writeComment(String("* ") + jpegImage.width + "x" + jpegImage.height + " (" + String(jpegImage.byteCount / 1024.0) + " KB) in " + String(retrieveTakenMillis / 1000.0) + "s");
      if (jpegImage.isValid()) {
        ...
      } else {
        ...
      }
      ...
      state = NOTHING;
    }   
    return
  }  
```

Notice
* how layer "feedback" (e.g. clicking) is received using `getFeedback()`
* how the `webImageTunnel` "tunnel" is used to download image with call to `reconnectTo()`  
* how the `imageRetrieverTunnel` "tunnel" is used to initiate retrieving of download image data with call to `reconnectForJpegImage()`
* how the image data (`DDJpegImage`) is received via the tunnel `imageRetrieverTunnel` with call to `readJpegImage()`

The whole `updateDD` basically is a state-machine that handles the different states (`state`) of the UI processing:
- `NOTHING` -- just started, or finished download / saving of image; will wait for `imageLayer` being clicked to initiate an image
- `DOWNLOADING_FOR_IMAGE` -- this state could have been merged with previous stage; anyway, it reconnects `webImageTunnel` to activate an image download
- `WAITING_FOR_IMAGE_DOWNLOADED`  -- waiting for download image to complete; then will retrieve the download image to be displayed to the TFT LCD screen
- `RETRIEVING_IMAGE` -- retrieving the download image data to be transferred to the microcontroller; once retrieved, display the image to the TFT LCD screen



The slide show is carried out when "idle" (not connected to DumbDisplay app)  
```
void loop() {
  pdd.loop(initializeDD, updateDD);
  if (pdd.isIdle()) {
    if (pdd.justBecameIdle()) {
      // re-start slide show
      ...
    }
    unsigned long now = millis();
    if (now >= nextMillis) {
      if (MAX_IMAGE_COUNT > 0 && savedImageCount > 0) {
        ...
      } else {
        ...
      }
      ...
    }
  }
}
```

# Build and Upload

Build, upload the sketch and try it out! 

For WiFi connectivity, you will need to find out the IP address of the microcontroller board. Simply connect the microcontroller board with a Serial monitor (set to use baud rate 115200), you should see lines like
```
binded WIFI TrevorWireless
listening on 192.168.0.218:10201 ...
listening on 192.168.0.218:10201 ...
```

See that the IP address of the microcontroller board is printed out. 

On DumbDisplay Android app side

|||
|--|--|
|You will need the microcontroller's IP address to configure DumbDisplay Android app to connect it with your microcontroller|![](imgs/DD_WIFI.png)|

* Start the DumbDisplay app.
* Click on the Establish Connection icon.
* In the "establish connection" dialog, you should see the "add WIFI device" icon at the bottom right of the dialog. Click on it.
* A popup for you to enter WIFI IP will be shown. Enter the IP address of your ESP board as Network Host. Click OK when done.
* Back to the "establish connection" dialog, a new entry will be added, click on it to establish WIFI connection.


Have fun with it!


# Enjoy!

> Peace be with you!
> May God bless you!
> Jesus loves you!
> Amazing Grace!


