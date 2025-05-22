#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>



// communication.h

#ifndef COMMUNICATION_H
#define COMMUNICATION_H


// Function prototypes
void sendDataToServer(float speed, float distance);
void setupWiFi();
#endif
