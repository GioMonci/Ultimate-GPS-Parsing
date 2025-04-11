#include <Adafruit_GPS.h>
#include "Wire.h"

#define GPSECHO false

#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);

uint32_t timer = millis();

void printTwoDigits(uint8_t number) {
  if (number < 10) Serial.print('0');
  Serial.print(number, DEC);
}

void setup(){
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic parsing test!");
  
  GPS.begin(9600); // 9600 NMEA is the default baud rate
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY); // uncomment for "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update
  GPS.sendCommand(PGCMD_ANTENNA); // Request antenna status updates
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE); // Request firmware version
}

void loop(){
  unsigned long currentMillis = millis();

  // Read GPS input and optionally echo for debugging
  char c = GPS.read();
  if (GPSECHO && c) {
    Serial.print(c);
  }
  
  // Process a complete NMEA sentence
  if (GPS.newNMEAreceived()) {
    char *nmea = GPS.lastNMEA(); // Store sentence once
    Serial.print(nmea);
    if (!GPS.parse(nmea)) return; // If parsing fails, wait for next sentence
  }

  // Every 2 seconds, print the current time, date, and fix data
  if (currentMillis - timer > 2000) {
    timer = currentMillis;
    Serial.print("\nTime: ");
    printTwoDigits(GPS.hour); Serial.print(':');
    printTwoDigits(GPS.minute); Serial.print(':');
    printTwoDigits(GPS.seconds); Serial.print('.');
    
    // Format milliseconds to three digits
    if (GPS.milliseconds < 10)
      Serial.print("00");
    else if (GPS.milliseconds < 100)
      Serial.print("0");
    Serial.println(GPS.milliseconds);

    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
    }
  }
}
