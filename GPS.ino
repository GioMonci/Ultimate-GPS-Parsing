#include <Adafruit_GPS.h>
#include "Wire.h"

#define GPSECHO false

// 'false' turn off echoing the GPS data to the Serial console
// 'true'  to debug and listen to the raw GPS sentences

#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);

uint32_t timer = millis();

void setup(){

  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic parsing test!");
  GPS.begin(9600); // 9600 NMEA is the default baud rate
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY); //uncomment for only "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // Don't go higher than 1 Hz
  GPS.sendCommand(PGCMD_ANTENNA); // Request updates on antenna status, comment out to stop
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE); // Ask for firmware version
}

void loop(){
  char debug_echo = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO) // Line 4, debug
    if (debug_echo) Serial.print(debug_echo);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.print(GPS.lastNMEA()); // sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // Every 2 seconds, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
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

