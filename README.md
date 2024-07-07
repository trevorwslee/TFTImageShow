# Simple Arduino Framework Raspberry Pi Pico / ESP32 TFT LCD Photo Frame Implementation with Photos Downloaded from the Internet via DumbDisplay

The target of this project is to implement a simple Arduino framework photos / images showing "photo frame" using Raspberry Pi Pico or ESP32 with photos / images downloaded from the Internet via the DumbDisplay app.

The microcontroller program here is developed in Arduino framework using VS Code and PlatformIO.

The simple remote UI for downloading photos / images from the Internet is realized with the help of the DumbDisplay Android app.
Please note that the downloaded image, be in in **Jpeg** or **PNG** format, will be transferring to the microcontroller board in **Jpeg** format scaled to site inside the TFT LCD screen. 

For Raspberry Pi Pico (WiFi), a ST7789 2.8 inch 280x320 SPI TFT LCD Module is attached to a Raspberry Pi PicoW board.
The TFT LCD module library is the `Adafruit-ST7735-Library` Arduino library. 

For ESP32, LiLyGo TCamera Plus Board is used. 
The TFT LCD module library is the `bodmer/TFT_eSPI` Arduino library.

In all cases, the **Jpeg** library used is the `bodmer/TJpg_Decoder` Arduino library.

A simple flash-based **LittleFS** file-system will be created for storing the download and transferred **Jpeg** photos / images.

The microcontroller board will have two running modes:

1) When connected to the DumbDisplay Android app, a simple UI will be provided for downloading images from some predefined sites,
   and save / transfer the downloaded image in **Jpeg** format to the microcontroller board.
   Note that the predefined sites is predefined in the sketch that you can conveniently change as desired.
2) When not connected to the DumbDisplay Android app, the microcontroller cycle through the saved **Jpeg** images and display them to the TFT LCD 
   screen one by one, like a simple "photo frame". Note that since the images are stored in **LittleFS**, they will survive even after reboot of the
   microcontroller board.

*Connect for the UI; disconnect to enjoy "photo frame" slide show.*         