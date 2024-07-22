// -------------------------------------------------------------------------------------
// H.Drees - 2022-12-18
// ESP8266MOD12F
//
// NTPClient von Fabrice Weinberg
// Adafruit SSD1306
// -------------------------------------------------------------------------------------

#include "Led.h"

#include "wifiData.h"

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_BUILTIN_PIN 2  // PIN für Status-LED

// Define NTP Client to get time
// Zeitverschiebung UTC <-> MEZ (Winterzeit) = 3600 Sekunden (1 Stunde)
// Zeitverschiebung UTC <-> MEZ (Sommerzeit) = 7200 Sekunden (2 Stunden)
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][3] = { "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa" };

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

Led LedBuiltIn(2);

void setup() {
  Serial.begin(9600);

  // auf serielle Verbindung warten
  while (!Serial) {;}

  Serial.println("Setup NTP Clock ...");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();

  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.cp437(true);                  // Use full 256 char 'Code Page 437' font

  display.setCursor(0, 0);
  display.print("Setup NTP Clock ...");
  display.display();

  pinMode(LED_BUILTIN_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN_PIN, HIGH);

  char wifiConnect[40];
  display.setCursor(0, 12);
  display.print("Connecting to WLAN:");
  display.setCursor(0, 22);
  display.print(WIFI_SSID);
  display.display();

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();

  Serial.println("... finished");

  display.setCursor(0, 34);
  display.print("... setup finished");
  display.display();

  delay(500);
}

void loop() {
  timeClient.update();

  Serial.print("epoch: ");
  Serial.println(timeClient.getEpochTime());

  display.clearDisplay();

  display.setCursor(0, 0);
  char epochText[40];
  sprintf(epochText, "NTP-Uhr: %lu", timeClient.getEpochTime());
  display.print(epochText);

  display.setCursor(0, 12);
  char wifi[40];
  sprintf(wifi, "WiFi: %s", WIFI_SSID);
  display.print(wifi);

  display.setCursor(0, 24);
  display.print(getCurrentDate());

  display.setCursor(0, 36);
  display.print(getCurrentTime());

  display.display();

  LedBuiltIn.toggle();

  delay(1000);
}
// -------------------------------------------------------------------------------------
String getCurrentDate() {
  time_t epochTime = timeClient.getEpochTime();
  //Get a time structure
  struct tm *ptm = gmtime((time_t *)&epochTime);

  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;

  return String(daysOfTheWeek[ptm->tm_wday]) + ", " + String(monthDay) + "." + String(currentMonth) + "." + String(currentYear);
}
// -------------------------------------------------------------------------------------
String getCurrentTime() {
  return timeClient.getFormattedTime();
}
// -------------------------------------------------------------------------------------
