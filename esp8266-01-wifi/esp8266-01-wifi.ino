#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "communication.h"


String cmd = "";
float speed = 0;
float distance = 0;


void setup() {
  Serial.begin(115200);    // FTDI serial monitor
  setupWiFi();             // Connect to WiFi
}

void loop() {
  // Check if data is available from Mega
  if (Serial.available()) {
    cmd = Serial.readStringUntil('\n'); // e.g., "speed:23.4,distance:15.6"
    Serial.println("Received from Mega: " + cmd);

    // Parse the received data
    int speedIndex = cmd.indexOf("speed:");
    int distanceIndex = cmd.indexOf("distance:");

    if (speedIndex != -1 && distanceIndex != -1) {
      String speedStr = cmd.substring(speedIndex + 6, cmd.indexOf(',', speedIndex));
      String distanceStr = cmd.substring(distanceIndex + 9);

      speed = speedStr.toFloat();
      distance = distanceStr.toFloat();

      // Debug
      Serial.println("Parsed Speed: " + String(speed));
      Serial.println("Parsed Distance: " + String(distance));

      // Send to server
      sendDataToServer(speed, distance);
    } else {
      // Serial.println("Invalid format");
    }
  }

//  delay(100);  // wait before next read

}
