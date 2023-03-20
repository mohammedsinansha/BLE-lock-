//Code modified from wybiral/BLE_Scan.ino https://gist.github.com/wybiral/2a96c1d1605af7efa11b690586c4b13e
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>



const char *tags[] = {
  "a1:b3:c1:f1:09:d1", //Pet1
  "eb:49:f2:2c:00:00", //Pet2
  "c3:48:d1:ed:00:00", //Pet3
  "fe:d6:99:97:00:00", //Pet4
  "e9:d0:7d:db:00:00", //Pet5
  "f7:13:9c:3f:00:00", //Spare
};
// Wires White == Reset, Brown == Touch enable debug, Grey == program CMD
int len = (sizeof (tags) / sizeof (*tags)); // how many elements in array
int x; // generic loop counter

unsigned long previousMillis = 0;
const long DoorOpenDuration = 10000;
int debug = 0;
const int TriggerPIN = 2; //Red
//pin 2 for ESP32es
const int CUTOFF = -60;
String detectedMAC = " ";
int TagAllowed = 0;
int bestSignal = -99;

//touch buttons
#define TOUCH_PIN T4 //connected to P13 Brown
int touch_value = 100;

void setup() {
  //Initiate Serial communication.
  Serial.begin(9600);
  pinMode(TriggerPIN, OUTPUT); //Red 
  BLEDevice::init("");
  Serial.print("System Ready, trigger pin is ");
  Serial.println (TriggerPIN); 
  
 
}

void loop() {
  getTouch();
  getClosestTag();

      if (TagAllowed == 1){    
        Serial.print(detectedMAC);
        Serial.print(" ");
        Serial.println(bestSignal);
        digitalWrite(TriggerPIN, HIGH);
     }
     //check if it's time to close the door
     unsigned long currentMillis = millis();
     if (currentMillis - previousMillis >= DoorOpenDuration) {
       TagAllowed = 0;
       detectedMAC = " ";
       getClosestTag();
       digitalWrite(TriggerPIN, LOW); //Close the door
       previousMillis = currentMillis;
    }
 
}
void getTouch(){
  touch_value = touchRead(TOUCH_PIN);
   Serial.println(touch_value); // get value using T0
  if (touch_value < 20){
     Serial.println("touch detected");
     if (debug == 0) {
     debug = 1;
     Serial.println("Debug On");
     } else {
      debug = 0;
      Serial.println("Debug Off");
     }

  }
}
void getClosestTag(){
  BLEScan *scan = BLEDevice::getScan();
  scan->setActiveScan(true);
  BLEScanResults results = scan->start(1);
  bestSignal = CUTOFF;
  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice device = results.getDevice(i);
      int rssi = device.getRSSI();
      if (debug ==1){
        detectedMAC = (device.getAddress().toString().c_str());
        Serial.print("I see this device ");
        Serial.print(detectedMAC);
        Serial.print(" ");
        Serial.println(rssi);
      }
    if (rssi > bestSignal) {
      bestSignal = rssi;
      detectedMAC = (device.getAddress().toString().c_str());
      
      for (x = 0; x <len; x ++){
        if (detectedMAC == tags [ x ]){
          TagAllowed = 1;
          break;
        }
      }
    }
  }
}