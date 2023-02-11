//#define TS_ENABLE_SSL // For HTTPS SSL connection
#include <Keypad.h>
#include <ESP32Servo.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
//#include <WiFiClientSecure.h>
#include "ThingSpeak.h"
#include <iostream>
#include <math.h>
#include <string>
#include <AES.h>

//Initilize Timer
unsigned long startMillis = 0;  //some global variables available anywhere in the program
unsigned long currentMillis= 0;
bool timeoutrst = false;

//KEYPAD AND LOCK
String password1; // change your password here
String input_password;
boolean BLEVerified = false;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {27, 14, 12, 13}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {32, 33, 25, 26}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//servo motor
Servo myservo; 

int servoPin = 18;
int pos = 0;    // variable to store the servo position

char* host = "api.thingspeak.com";
char* readAPIKey  = "ABG5705DO6YQLGOW";
unsigned int ChannelID = 2010897;
unsigned int dataFieldOne = 1;
//const char* certificate = SECRET_TS_ROOT_CA;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
int txValue = 0;
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_TX "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a9"

//const char *ssid     = "ET0731-IOTS"; 
//const char *password = "iotset0731"; 
const char *ssid = "MEEP";
const char *password = "NOBODYWILLKNOW";


WiFiClient client;
//WiFiClientSecure client;
String aConst = "No";
String rxString;

//======================AES128 CBC Key and IV=============================
AES aes ;
//2b7e151628aed2a6abf7158809cf4f3c
byte key[] = {
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c, 
};
//000102030405060708090A0B0C0D0E0F
byte my_iv[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
};
//Initialize Ciphertext and decryption
byte cipher[16];
byte check[16] ;
//========================================================================

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

int64_t minute2micros(int minutes){ 
   return minutes*60000000; 
}

String decrypt(int bits, int blocks, String data)
{
  char const *arr = data.c_str();
  //char const encryptedmsg;
  auto getNum = [](char c){ return c > '9' ? c - 'a' + 10 : c - '0'; };
  byte *ptr = cipher;
  byte iv[16];
  byte succ = aes.set_key(key, bits);
  long t0 = micros();
  long t1 = micros()-t0;
  
  for(const char *idx = arr ; *idx ; ++idx, ++ptr ){
    *ptr = (getNum( *idx++ ) << 4) + getNum( *idx );
  }

  //Check converted byte values.
  for (byte b = 0 ; b < (blocks-1)*N_BLOCK ; b++)
  {
    byte val = cipher[b] ;
    Serial.print (val>>4, HEX) ; Serial.print (val&15, HEX) ;
  }
  Serial.println(" ");


  Serial.print ("set_key ") ; Serial.print (bits) ; Serial.print (" ->") ; Serial.print ((int) succ) ;
  Serial.print (" took ") ; Serial.print (t1) ; Serial.println ("us") ;
  t0 = micros();
  for (byte i = 0 ; i < 16 ; i++){
    iv[i] = my_iv[i] ;}
  succ = aes.cbc_decrypt (cipher, check, blocks, iv);
  t1 = micros()-t0 ;
  Serial.print ("decrypt") ; Serial.print (" ->") ; Serial.print ((int) succ) ;
  Serial.print (" took ") ; Serial.print (t1) ; Serial.println ("us") ;

  for (byte i = 0 ; i < (blocks-1)*N_BLOCK ; i++)
  {
    byte val = check[i] ;
    Serial.print (val>>4, HEX) ; Serial.print (val&15, HEX) ;
  }
  Serial.println(" ");
  for (byte i = 0 ; i < (blocks-1)*N_BLOCK ; i++)
  {
    byte val = check[i] ;
    Serial.print ((char)check[i]);
  }
  String msg;
  for(int i = 0; i <6;i++){
    msg = msg + (char)check[i];
  }
  return msg;
}


String readTSData( long TSChannel, unsigned int TSField ) {
  String data =  ThingSpeak.readStringField( TSChannel, TSField, readAPIKey );
  //Serial.print(data));
  Serial.println("Retrieving Data from ThingSpeak!");
  Serial.println( " Data read from ThingSpeak: ");
  Serial.println(data);
  password1= decrypt(128,2,data);
  Serial.println("Decrypted Message");
  Serial.println(password1);
  BLEVerified = true;
  return data;
}

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) { 
    std::string rxValue = pCharacteristic->getValue();

    if(rxValue.length() > 0) {
      int a = 0;
      Serial.println("START=RECIEVE");
      Serial.println("Recieved Value from BLE: ");

      for(int i = 0; i < rxValue.length(); i++){
        Serial.print(rxValue[i]);  
      }
      Serial.println();
      aConst = readTSData( ChannelID, dataFieldOne );
      for(int i = 0; i < rxValue.length(); i++){
        if(rxValue[i] == aConst[i]){
            a++;
        }
      }
      if(a == aConst.length()) {
        a = 0;
        int attemptcount = 0;
        startMillis = millis();
        
     while(BLEVerified == true){
        currentMillis = millis();
        if(currentMillis - startMillis >= 30000) {
          BLEVerified = false;
          startMillis = currentMillis;
        }    
        char customKey = customKeypad.getKey();
        delay(100);
        if(customKey){
          Serial.print(customKey);
      
          if (customKey == '*') {
            input_password = ""; // clear input password
          } else if (customKey == '#') {
            if (password1 == input_password) {
              Serial.println("The password is correct, ACCESS GRANTED!");
              BLEVerified = false;
              //servo motor
              for (pos = 90; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
                    // in steps of 1 degree
                    myservo.write(pos);    // tell servo to go to position in variable 'pos'
                    delay(5);             // waits 15ms for the servo to reach the position
              }
              delay(5000);
              for (pos = 180; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
                  myservo.write(pos);    // tell servo to go to position in variable 'pos'
                  delay(5);             // waits 15ms for the servo to reach the position
              }
      
            } else {
              Serial.println("The password is incorrect, ACCESS DENIED!");
              attemptcount = attemptcount + 1;
              if(attemptcount == 3)
                BLEVerified = false;
            }
            input_password = ""; // clear input password
          } else {
            input_password += customKey; // append new character to input password string
          }
        }
      }
     }
      Serial.println();
      Serial.println("END=RECIEVE");
      int connid = pServer->BLEServer::getConnId();
      pServer->BLEServer::disconnect(connid);
      aConst = "No";
    }
  }
};

void setup(void) {
  Serial.begin(115200); //Initialize Serial
  
  myservo.attach(servoPin);
  input_password.reserve(32);

  WiFi.mode(WIFI_STA);
  Serial.print("Attempting to Connect to WiFi");
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); //Attempting to Connect
  }
  Serial.println("Connected."); //Connected
  Serial.println(WiFi.localIP()); //Print Ip address
  
  //client.setCACert(certificate); 
  ThingSpeak.begin(client);
  
  // Create the BLE Device

  BLEDevice::init("Darrensucks");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharacteristic->setCallbacks(new MyCallbacks());
  
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
    // notify changed value
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)&value, 4);
        pCharacteristic->notify();
        value++;
        delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(5000); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        Serial.println("BLE Connected");
        //Set Timer
        
        startMillis = millis();
        timeoutrst = true;
        oldDeviceConnected = deviceConnected;
    }
    if(timeoutrst == true){
      currentMillis = millis();
      if(currentMillis - startMillis >= 60000 && aConst == "No") {
        int connid = pServer->BLEServer::getConnId();
        pServer->BLEServer::disconnect(connid);
        startMillis = currentMillis;
        timeoutrst = false;
      }
    }

}
