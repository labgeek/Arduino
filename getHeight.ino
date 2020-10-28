/*
 * Program name:  getHeight.ino
 * Header file:  Font_Data.h
 * Author:  JD Durick - labgeek@gmail.com
 * Date:  10/28/2020
 * Version 0.1
 */

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Time.h>
#include "Font_Data.h"
#include <DS3231.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

DS3231 Clock;

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 13
#define DATA_PIN 11
#define CS_PIN 10
#define slider_pin A0 // OTA or OTB pin on slider
uint8_t scrollSpeed = 1; // default frame delay value
uint16_t scrollPause = 2000; // in milliseconds
#define BUF_SIZE 75
char curMessage[BUF_SIZE] = {
  ""
};
char newMessage[BUF_SIZE] = {
  "    Initial message goes here"
};
bool newMessageAvailable = true;
const uint8_t SPEED_IN = A5;
int slider_val; // used to hold the slider analog value
int slide_scroll_speed; // used when changing scroll speed
// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT; // how to align the text

#define trigPin A0
#define echoPin A1

#define SPEED_TIME 75 // speed of the transition
#define PAUSE_TIME 0
#define MAX_MESG 20

char testStr[10] = {
  ""
};
String heightStr = "";
String inchStr = "";

String feetStr = "";
String remainingStr = "";
String feet_inches = "";
String remStr = "";
char dest[25];
char buf1[10] = {
  ""
};
char buf2[10] = {
  ""
};
char buf3[10] = {
  ""
};
long duration;
int distance;
int height, inches, feet, inchesRemaining;

bool Century = false;
bool h12;
bool PM;
byte dd, mm, yyy;
uint16_t h, m, s;

char szTime[9]; // mm:ss\0
char szMesg[MAX_MESG + 1] = "";

uint8_t degC[] = {
  6,
  3,
  3,
  56,
  68,
  68,
  68
}; // Deg C
uint8_t degF[] = {
  6,
  3,
  3,
  124,
  20,
  20,
  4
}; // Deg F

char * mon2str(uint8_t mon, char * psz, uint8_t len)

// Get a label from PROGMEM into a char array
{
  static
  const __FlashStringHelper * str[] = {
    F("Jan"),
    F("Feb"),
    F("Mar"),
    F("Apr"),
    F("May"),
    F("Jun"),
    F("Jul"),
    F("Aug"),
    F("Sep"),
    F("Oct"),
    F("Nov"),
    F("Dec")
  };

  strncpy_P(psz, (const char PROGMEM * ) str[mon - 1], len);
  psz[len] = '\0';

  return (psz);
}

char * dow2str(uint8_t code, char * psz, uint8_t len) {
  static
  const __FlashStringHelper * str[] = {
    F("Sunday"),
    F("Monday"),
    F("Tuesday"),
    F("Wednesday"),
    F("Thursday"),
    F("Friday"),
    F("Saturday")
  };

  strncpy_P(psz, (const char PROGMEM * ) str[code - 1], len);
  psz[len] = '\0';

  return (psz);
}

void getTime(char * psz, bool f = true)
// Code for reading clock time
{
  s = Clock.getSecond();
  m = Clock.getMinute();
  h = Clock.getHour(h12, PM);
  // sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
  sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
  Serial.println(psz);

}

void getDate(char * psz)
// Code for reading clock date
{
  char szBuf[10];
  Serial.println("in getdate");
  dd = Clock.getDate();
  mm = Clock.getMonth(Century); //12
  yyy = Clock.getYear();
  sprintf(psz, "%d %s %04d", dd, mon2str(mm, szBuf, sizeof(szBuf) - 1), (yyy + 2000));

}

void setup() {
  Serial.begin(57600); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  P.begin();
  Wire.begin();
  P.displayText(newMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);

}

void loop() {
  static uint32_t lastTime = 0; // millis() memory
  static uint8_t display = 0; // current display mode
  static bool flasher = false; // seconds passing flasher

  //getTime(szMesg, flasher);
  // flasher = !flasher;

  measure();
  if (distance > 90) {
    Serial.println(distance);
    if (P.displayAnimate()) {
      if (newMessageAvailable) {
        //strcpy(curMessage, newMessage);
        flasher = true;
        getTime(szMesg, flasher);
        strcpy(curMessage, szMesg);
        delay(1000);
        P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
        newMessageAvailable = false;
      }
      P.displayReset();
    }

  }
  if (distance < 90) {
    // 229 is 8ft tall (7.5 feet)
    height = 229 - distance;
    inches = (height * 0.393701);
    heightStr = String(height, DEC) + "cm";
    inchStr = String(inches, DEC) + "in";

    feet = inches / 12;
    inchesRemaining = inches % 12;

    feetStr = String(feet, DEC) + "ft";
    remStr = String(inchesRemaining, DEC) + "in";

    heightStr.toCharArray(testStr, 75);
    inchStr.toCharArray(testStr, 75);

    feetStr.toCharArray(buf1, 10);
    remStr.toCharArray(buf2, 10);

    String final = feetStr + "." + remStr;
    final.toCharArray(buf3, 10);

    strcpy(dest, buf3);
    Serial.println(dest);

    //P.displayText(testStr, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayText(dest, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);

  }
  bool jd = P.displayAnimate();
  delay(100);
  newMessageAvailable = true;
  return;
}

void measure() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.017;

}
