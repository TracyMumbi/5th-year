#include "communication.h"
// Wi-Fi credentials
const char* ssid = "iPhone";          // Replace with your WiFi SSID
const char* password = "password12345";  // Replace with your WiFi password
void setupWiFi() {
  WiFi.begin(ssid, password);
   Serial.print("/Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
     Serial.print(".");
  }
   Serial.println("Connected to/ WiFi!");
   Serial.println(WiFi.localIP());  // Print the local IP address
}

// Function to send data to the server
void sendDataToServer(float speed, float distance) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    String serverUrl = "http://172.20.10.3:3000/parameters"; // Replace with your server URL

    // Create a JSON object to send
    String jsonData = "{";
    jsonData += "\"speed\":" + String(speed) + ",";
    jsonData += "\"distance\":" + String(distance);
    jsonData += "}";

    // Debugging
     Serial.println("Sending data: " + jsonData);

    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Set timeout
    http.setTimeout(5000); // 5 seconds timeout

    // Send the data to the server using POST
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
       Serial.print("Server Response: ");
       Serial.println(response);
    } else {
       Serial.print("Error sending data: ");
       Serial.println(httpResponseCode);  // Debugging error codes
    }

    http.end();
  } else {
    // Serial.println("WiFi not connected!");
  }
}
