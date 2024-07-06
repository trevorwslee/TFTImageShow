
#include "Arduino.h"



#if defined(WIFI_SSID)
  #include "wifidumbdisplay.h"
  DumbDisplay dumbdisplay(new DDWiFiServerIO(WIFI_SSID, WIFI_PASSWORD));
#else
  // for direct USB connection to phone
  // . via OTG -- see https://www.instructables.com/Blink-Test-With-Virtual-Display-DumbDisplay/
  // . via DumbDisplayWifiBridge -- see https://www.youtube.com/watch?v=0UhRmXXBQi8/
  #include "dumbdisplay.h"
  DumbDisplay dumbdisplay(new DDInputOutput());
#endif



#define NEXT_S              5

#define COLOR_BG            0xFFFF
#define COLOR_IDLE_BG       0x7BEF
#define COLOR_INVALID_BG    0xF800



#if defined(FOR_PICOW)  
  #define A_TFT_BL    21
  #define A_TFT_CS    17
  #define A_TFT_DC    16
  #define A_TFT_SCLK  18
  #define A_TFT_MOSI  19
  #define A_TFT_RST   20
  #define TFT_WIDTH   320
  #define TFT_HEIGHT  240
  #include <Adafruit_GFX.h>    // Core graphics library
  #include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
  Adafruit_ST7789 tft(A_TFT_CS, A_TFT_DC, A_TFT_RST);
#elif defined(FOR_TCAMERAPLUS)
  #include <TFT_eSPI.h>
  TFT_eSPI tft = TFT_eSPI();
#else
  #error board not supported
#endif


// set MAX_IMAGE_COUNT to 0 to force reformat the storage
#define MAX_IMAGE_COUNT  10


#include <FS.h>
#include <LittleFS.h>

#define STORAGE_FS       LittleFS


#include <TJpg_Decoder.h>

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
#if defined(A_TFT_CS)
  tft.drawRGBBitmap(x, y, bitmap, w, h);
#else
  tft.pushRect(x, y, w, h, bitmap);
#endif

  // Return 1 to decode next block
  return 1;
}



DDMasterResetPassiveConnectionHelper pdd(dumbdisplay);

GraphicalDDLayer* imageLayer;
LcdDDLayer* autoSaveOptionLayer;
LcdDDLayer* saveButtonLayer;
LcdDDLayer* savedCountLabelLayer;
SevenSegmentRowDDLayer* savedCountLayer;
SimpleToolDDTunnel* webImageTunnel;
ImageRetrieverDDTunnel* imageRetrieverTunnel = NULL;


int savedImageCount = 0;
int nextSaveImageIndex = 0;
int nextShowImageIndex = 0;

enum State {
  NOTHING,
  DOWNLOADING_FOR_IMAGE,
  WAITING_FOR_IMAGE_DOWNLOADED,
  RETRIEVING_IMAGE,
};


State state;
unsigned long retrieveStartMillis;

DDJpegImage currentJpegImage;
bool autoSave = false;


// download image bigger than needed (on purpose)
const String urls[] = {
  String("https://loremflickr.com/") + String(2 * TFT_WIDTH) + String("/") + String(2 * TFT_HEIGHT),
  String("https://picsum.photos/") + String(2 * TFT_WIDTH) + String("/") + String(2 * TFT_HEIGHT),
};
const char* getDownloadImageURL() {
  int idx = random(2);
  return urls[idx].c_str();
}


void initializeDD() {
  tft.fillScreen(COLOR_BG);

  // create a graphical layer for drawing the web image to
  imageLayer = dumbdisplay.createGraphicalLayer(2 * TFT_WIDTH, 2 * TFT_HEIGHT);
  imageLayer->border(10, "blue", "round");  
  imageLayer->padding(5);
  imageLayer->noBackgroundColor();
  imageLayer->setTextSize(18);
  imageLayer->enableFeedback("fl");

  saveButtonLayer = dumbdisplay.createLcdLayer(6, 2);
  saveButtonLayer->backgroundColor("ivory");
  saveButtonLayer->border(1, "blue", "raised");
  saveButtonLayer->padding(1);
  saveButtonLayer->writeCenteredLine("💾", 0);
  saveButtonLayer->writeCenteredLine("Save", 1);
  saveButtonLayer->enableFeedback("f");

  autoSaveOptionLayer = dumbdisplay.createLcdLayer(6, 1);
  autoSaveOptionLayer->backgroundColor("ivory");
  autoSaveOptionLayer->border(1, "blue", "raised");
  autoSaveOptionLayer->padding(1);
  autoSaveOptionLayer->enableFeedback("fl");

  savedCountLabelLayer = dumbdisplay.createLcdLayer(8, 1);
  savedCountLabelLayer->backgroundColor("azure");
  savedCountLabelLayer->border(1, "green", "hair");
  savedCountLabelLayer->padding(1);
  savedCountLabelLayer->writeLine("Saved🗂️");

  savedCountLayer = dumbdisplay.create7SegmentRowLayer(2);
  savedCountLayer->backgroundColor("azure");
  savedCountLayer->border(10, "red", "flat");
  savedCountLayer->padding(10);
  savedCountLayer->showNumber(savedImageCount, "0");
  savedCountLayer->enableFeedback();

  // create a tunnel for downloading web image ... initially, no URL yet ... downloaded.png is the name of the image to save
  webImageTunnel = dumbdisplay.createImageDownloadTunnel("", "downloaded.png");

  // create a tunnel for retrieving JPEG image data from DumbDisplay app storage
  imageRetrieverTunnel = dumbdisplay.createImageRetrieverTunnel();

  dumbdisplay.configAutoPin(DDAutoPinConfig('V')
    .addLayer(imageLayer)
    .beginGroup('H')
      .addLayer(saveButtonLayer)
      .beginGroup('V')
        .beginGroup('H')
          .addLayer(savedCountLabelLayer)
          .addLayer(savedCountLayer)
        .endGroup()  
        .addLayer(autoSaveOptionLayer)
      .endGroup()
    .endGroup()
    .build());
}

String _formatImageName(int index) {
  return "/img_" + String(index);
}
String formatImageFileName(int index) {
  return _formatImageName(index) + ".jpeg";
}
String formatImageMetaFileName(int index) {
  return _formatImageName(index) + ".txt";
}

void saveCurrentImage() {
  if (currentJpegImage.isValid()) {
    String fileName = formatImageFileName(nextSaveImageIndex);
    File f = STORAGE_FS.open(fileName, "w");
    if (f) {
      f.println(currentJpegImage.width);
      f.println(currentJpegImage.height);
      f.println(currentJpegImage.byteCount);
      f.write(currentJpegImage.bytes, currentJpegImage.byteCount);
      f.close();
      dumbdisplay.logToSerial(String("! written image to  [") + fileName + "]");
    } else {
      dumbdisplay.log(String("unable to open file [") + fileName + "] for writing");
    }
    savedImageCount = min(savedImageCount + 1, MAX_IMAGE_COUNT);
    nextSaveImageIndex = (nextSaveImageIndex + 1) % MAX_IMAGE_COUNT; 
    dumbdisplay.logToSerial("$ savedImageCount: " + String(savedImageCount));
    savedCountLayer->showNumber(savedImageCount, "0");
    if (true) {
      currentJpegImage.release();
    }
  }
}

void deleteAllSavedImage() {
  for (int i = 0; i < savedImageCount; i++) {
    String fileName = formatImageFileName(i);
    if (STORAGE_FS.exists(fileName)) {
      STORAGE_FS.remove(fileName);
    }
  }
  savedImageCount = 0;
  nextSaveImageIndex = 0;
  dumbdisplay.log("!!! Deleted all saved images !!!");
  savedCountLayer->showNumber(savedImageCount, "0");
}

DDJpegImage& getSavedImage(DDJpegImage& tempImage) {
  String fileName = formatImageFileName(nextShowImageIndex);
  File f = STORAGE_FS.open(fileName, "r");
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
  } else {
    dumbdisplay.log(String("unable to open file [") + fileName + "] for reading");
  }
  return tempImage;
}


void updateDD(bool isFirstUpdate) {
  bool updateUI = isFirstUpdate;

  if (autoSaveOptionLayer->getFeedback() != NULL) {
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

  if (state == NOTHING) {
    if (saveButtonLayer->getFeedback() != NULL) {
      saveButtonLayer->disabled(true);
      saveCurrentImage();
    }
  }

  const DDFeedback* fb = savedCountLayer->getFeedback();
  if (state == NOTHING) {
    if (fb != NULL && fb->type == DOUBLECLICK) {
      if (state == NOTHING) {
        savedCountLayer->flash();
        deleteAllSavedImage();
      }
    }
  }

  if (isFirstUpdate || state == NOTHING) {
    if (isFirstUpdate || imageLayer->getFeedback() != NULL) {
      saveButtonLayer->disabled(true);
      imageLayer->noBackgroundColor();
      state = DOWNLOADING_FOR_IMAGE;
    }
    return;
  }

  if (state == DOWNLOADING_FOR_IMAGE) {
    // set the URL to download web image ... using a bit different size so that web image will be different 
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
    DDJpegImage jpegImage;
    bool retrievedImage = imageRetrieverTunnel->readJpegImage(jpegImage);
    if (retrievedImage) {
      unsigned long retrieveTakenMillis = millis() - retrieveStartMillis;
      dumbdisplay.writeComment(String("* ") + jpegImage.width + "x" + jpegImage.height + " (" + String(jpegImage.byteCount / 1024.0) + " KB) in " + String(retrieveTakenMillis / 1000.0) + "s");
      if (jpegImage.isValid()) {
        saveButtonLayer->disabled(autoSave);
        imageLayer->backgroundColor("blue");
        tft.fillScreen(COLOR_BG);
        int x = (TFT_WIDTH - jpegImage.width) / 2;
        int y = (TFT_HEIGHT - jpegImage.height) / 2;
        TJpgDec.drawJpg(x, y, jpegImage.bytes, jpegImage.byteCount);
        if (MAX_IMAGE_COUNT > 0) {
          jpegImage.transferTo(currentJpegImage);
          if (autoSave) {
            saveCurrentImage();
          } else {
            saveButtonLayer->disabled(false);            
          }
        }
      } else {
        imageLayer->backgroundColor("red");
        tft.fillScreen(COLOR_INVALID_BG);
      }
      state = NOTHING;
    }

    return;
  }
}

void setup() {
  Serial.begin(115200);

#if defined(FOR_PICOW)  
  pinMode(A_TFT_BL, OUTPUT);
  digitalWrite(A_TFT_BL, 1);  // light it up
  tft.init(240, 320, SPI_MODE0);
  tft.invertDisplay(false);
  tft.setRotation(1);
  tft.setSPISpeed(40000000);
#endif  

#if !defined(A_TFT_CS)  
  tft.init();
  tft.setRotation(0);
#endif

  TJpgDec.setJpgScale(1);
#if !defined(A_TFT_CS)
  TJpgDec.setSwapBytes(true);
#endif
  TJpgDec.setCallback(tft_output);

  tft.fillScreen(COLOR_INVALID_BG);
}

bool initializeStorageFS = false;

unsigned long nextMillis = 0;

void loop() {
  if (!initializeStorageFS) {
    savedImageCount = 0;
    dumbdisplay.logToSerial("Initialized STORAGE_FS");
    if (MAX_IMAGE_COUNT > 0 && STORAGE_FS.begin()) {
      dumbdisplay.logToSerial("... existing STORAGE_FS ...");
      for (int i = 0; i < MAX_IMAGE_COUNT; i++) {
          String fileName = formatImageFileName(i);
          if (STORAGE_FS.exists(fileName)) {
            savedImageCount++;
          } else {
            break;
          }
      }
    } else {
      // if (!STORAGE_FS.format()) {
      //   dumbdisplay.logToSerial("Unable to format(), aborting");
      //   delay(2000);
      //   return;
      // }
      if (!STORAGE_FS.begin()) {
        dumbdisplay.logToSerial("Unable to begin(), aborting\n");
        delay(2000);
        return;
      }
      if (!STORAGE_FS.format()) {
        dumbdisplay.logToSerial("Unable to format(), aborting");
        delay(2000);
        return;
      }
    }
    dumbdisplay.logToSerial("STORAGE_FS initialized");
    initializeStorageFS = true;
  }

 pdd.loop([](){
    // **********
    // *** initializeCallback ***
    // **********
    initializeDD();
  }, [](){
    // **********
    // *** updateCallback ***
    // **********
    updateDD(!pdd.firstUpdated());
  }, [](){
    // **********
    // *** disconnectedCallback ***
    // **********
  });
  if (pdd.isIdle()) {
    if (pdd.justBecameIdle()) {
      dumbdisplay.logToSerial("# idle");
      tft.fillScreen(COLOR_IDLE_BG);
      nextShowImageIndex = 0;
      nextMillis = millis();
    }
    unsigned long now = millis();
    if (now >= nextMillis) {
      if (MAX_IMAGE_COUNT > 0 && savedImageCount > 0) {
        DDJpegImage dummyImage;
        DDJpegImage& jpegImage = getSavedImage(dummyImage);
        dumbdisplay.logToSerial("# nextShowImageIndex: " + String(nextShowImageIndex));
       if (jpegImage.isValid()) {
          tft.fillScreen(COLOR_IDLE_BG);
          int x = (TFT_WIDTH - jpegImage.width) / 2;
          int y = (TFT_HEIGHT - jpegImage.height) / 2;
          TJpgDec.drawJpg(x, y, jpegImage.bytes, jpegImage.byteCount);
        } else {
          tft.fillScreen(COLOR_INVALID_BG);
        }
        nextShowImageIndex = (nextShowImageIndex + 1) % savedImageCount;
      } else {
        dumbdisplay.logToSerial("# ...");
      }
      nextMillis = now + NEXT_S * 1000;
    }
  }
}
