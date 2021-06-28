/*
*  Tally Light Receiver and Display
*  switches determine camera number
*  green led for pre-stage
*  red led for active
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#define RXPin 2
#define TXPin 3
#define setPin 4
#define redPin 5
#define greenPin 6
#define redPin2 9
#define greenPin2 10
const int brightnessPin = A7;

int iFeedSwPin[] = {A0,A1};
int iFSIdx;
int iFEEDSWMAX = 2;
int iFeedVal = 0;
int iFSVal;
String cFeedValList[] = {"1", "2", "3", "4"};
String cFeedVal = "1";

SoftwareSerial HC12(TXPin, RXPin);         // HC-12 TX Pin, HC-12 RX Pin
byte incomingByte;
String readBuffer = "";
String kRED = "R";
String kGREEN = "G";
String kOFF = "O";
String sCurrColor = kOFF;

int iRedSet = LOW;
int iGreenSet = LOW;
int iRedMax = 255;
int iGreenMax = iRedMax / 4;

void setup() {
                  // Open serial port to computer
  HC12.begin(9600);                     // Open serial port to HC12
  pinMode(setPin, OUTPUT);
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  digitalWrite(setPin, LOW);
  delay(250);
  HC12.write("AT+DEFAULT");
  delay(250);
  digitalWrite(setPin, HIGH);          // HC-12 normal, transparent mode
  for (iFSIdx = 0; iFSIdx < iFEEDSWMAX; iFSIdx++) {
    pinMode(iFeedSwPin[iFSIdx], INPUT_PULLUP);
  }

  for (iFSIdx = 0; iFSIdx < 5; iFSIdx++) {
    analogWrite(greenPin, iGreenMax); 
    analogWrite(greenPin2, iGreenMax);
    analogWrite(redPin, LOW);
    analogWrite(redPin2, LOW);
    delay(1000);
    analogWrite(greenPin, LOW); 
    analogWrite(greenPin2, LOW);
    analogWrite(redPin, iRedMax);
    analogWrite(redPin2, iRedMax);
    delay(1000);
  }
  analogWrite(greenPin, iRedMax); 
  analogWrite(greenPin2, iRedMax);
  delay(1000);
  analogWrite(greenPin, LOW); 
  analogWrite(greenPin2, LOW);
  analogWrite(redPin, LOW);
  analogWrite(redPin2, LOW);
}

void loop() {
  delay(100);

  iFeedVal = 0;
  for (iFSIdx = 0; iFSIdx < iFEEDSWMAX; iFSIdx++) {
    if (!digitalRead(iFeedSwPin[iFSIdx]) == HIGH) {
      iFSVal = 1;
    } else {
      iFSVal = 0;
    }
    iFeedVal = (iFeedVal * 2) + iFSVal;
  }

  cFeedVal = cFeedValList[iFeedVal];

  while (HC12.available()) {             // If HC-12 has data
    incomingByte = HC12.read();          // Store each icoming byte from HC-12
    readBuffer += char(incomingByte);
  }
//  delay(100);
  if (readBuffer.substring(0,1) == cFeedVal) {
    iRedSet = LOW;
    iGreenSet = LOW;
    sCurrColor = readBuffer.substring(1,2);
    if (sCurrColor == kRED) {
      iRedSet = iRedMax;
      iGreenSet = LOW;
    } else {
      if (sCurrColor == kGREEN) {
          iRedSet = LOW;
          iGreenSet = iGreenMax;
      }
    }
    analogWrite(greenPin, iGreenSet);
    analogWrite(redPin, iRedSet);
    analogWrite(greenPin2, iGreenSet);
    analogWrite(redPin2, iRedSet);
  }

  readBuffer = "";

}