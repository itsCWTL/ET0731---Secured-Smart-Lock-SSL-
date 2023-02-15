# ET0731---Secured-Smart-Lock-SSL- <br>
Welcome to Group 2's Internet of Things Security project! <br><br>
Group members: <br> Christine Wong Tien Lee <br>
               Seng Cheong Long Gabriel <br>
               Darren Tan Kai Jen <br><br>
Brief concept: In order to ensure that users can easily unlock their Secured Smart Lock (SSL) securely, we have implemented a number of security features and wireless technologies which are either performed behind the scenes, or when it requires user's inputs, is easy for them to use. Never undermine the harderend security that Se cured Smart Lock is able to provide. Using a combination of self-developed 128-bit AES CBC encryption algorithm to secure data, our smart lock also takes advantage of multi-factor authentication which includes features like biometric authentication and other important security implementations like destruction of useless data and session management. All will be discussed in the report and presentation slides that our team has constructed to discuss the security implementations in detail, as well as Google sites to market and demonstrate the Secured Smart Lock (SSL). Accompanying all of these are our self-developed codes of the Arduino's ESP32,keypad and stepper motor which function as the lock mechanism built into our prototype enclosure, as well as the Java code for our Android application which will help users unlock their door. <br><br>

Code Requirements:
Before running any code, there cetain requirements to take note. Firstly for the ESP32:<br>
1. Board: "DOIT ESP32 DEVKIT V1"
   Upload Speed: "115200"
   Flash Frequency: "80MHz"
   Partition Scheme: "No OTA (Large APP)"  // DEFAULT PARTITION SCHEME OF ESP32 IS INSUFFICIENT FOR SECURED SMART LOCK MEMORY USAGE => PROVIDE MORE MEMORY SPACE
2. Arduino IDE Libraries Used/Installed:
   <Keypad.h>
   <ESP32Servo.h>
   <BLEDevice.h>
   <BLEServer.h>
   <BLEUtils.h>
   <BLE2902.h>
   <WiFi.h>
   <iostream>
   <math.h>
   <string>
   <AES.h>    //
   <ThingSpeak.h>
      
Link to Google Sites: https://sites.google.com/view/securedsmartlockssl/home <br>
Link to second version of report (Final report): <a href="https://github.com/itsCWTL/ET0731---Secured-Smart-Lock-SSL-/blob/main/Grp2_ver1.pdf">Final report</a><br>
Link to presentation slides: <a href="https://github.com/itsCWTL/ET0731---Secured-Smart-Lock-SSL-/blob/main/Grp2_PPT.pptx">Powerpoint</a><br>
Link to the ESP32 arduino code: <a href="https://github.com/itsCWTL/ET0731---Secured-Smart-Lock-SSL-/blob/main/IOTS_Project_Group2_Code_Finalver.ino">Arduino code</a><br>
Link to the Door Lock android app: <a href="https://github.com/itsCWTL/ET0731---Secured-Smart-Lock-SSL-/blob/main/DoorUnlock.zip">Java code</a><br>
Link to first version of the report (Proposal): <a href="https://github.com/itsCWTL/ET0731---Secured-Smart-Lock-SSL-/blob/main/Grp2_ver1.pdf">Project porposal</a>
