
#include <Arduino.h>

#include <FS.h>
#include <LittleFS.h>
//#include <SDFS.h>


void setup() {
  Serial.begin(115200);
  Serial.println("***** start *****");
}

bool initialized = false;


void loop() {
  Serial.println("... wait ...");
  delay(5000);

  if (!initialized) {
      Serial.println("***** begin *****");
    Serial.println("***** format *****");
    if (!LittleFS.begin(true)) {
      Serial.println("***** format *****");
      if (!LittleFS.format()) {
        Serial.printf("Unable to format(), aborting\n");
        return;
      }
      if (!LittleFS.begin()) {
        Serial.printf("Unable to begin(), aborting\n");
        return;
      }
    }
    Serial.println("Filesystem initialized");
    initialized = true;
  }

  size_t totalBytes = LittleFS.totalBytes();
  Serial.printf("=== LittleFS Total bytes: %d\n", totalBytes);

  File testFile = LittleFS.open(F("/testCreate.txt"), "w");
 
  if (testFile){
    Serial.println("Write file content!");
    testFile.print("Here is the test text");
    testFile.close();
  }else{
    Serial.println("Problem on create file!");
  }
  testFile = LittleFS.open(F("/testCreate.txt"), "r");
  if (testFile){
    Serial.println("Read file content!");
    /**
     * File derivate from Stream so you can use all Stream method
     * readBytes, findUntil, parseInt, println etc
     */
    Serial.println(testFile.readString());
    testFile.close();
  }else{
    Serial.println("Problem on read file!");
  }  
}
 
 