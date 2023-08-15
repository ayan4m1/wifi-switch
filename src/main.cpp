#include <Arduino.h>
#include <WiFi.h>

// uncomment this to enable serial debug logging
// #define SERIAL_DEBUG

// you MUST modify these values to match your network
#define WIFI_SSID "bar"
#define WIFI_PSK "wi9NNYara"

// you MUST modify this value to match your wiring
#define RELAY_PIN 14

// if your board does not provide an LED_BUILTIN, specify the LED pin here
#ifndef LED_BUILTIN
#define LED_BUILTIN 5
#endif

// shouldn't need to change these
#define HTTP_TIMEOUT_MS 5000
#define HTTP_PORT 80

// uncomment this to invert the logical output of RELAY_PIN
// #define INVERT_RELAY_TRIGGER

// uncomment this if your LED turns on with logic level HIGH
// #define INVERT_LED

WiFiServer server(HTTP_PORT);
bool state = false;

void setup() {
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  while (!Serial) {
    delayMicroseconds(100);
  }
  Serial.println(F("Serial init!"));
#endif

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
#ifndef INVERT_LED
  digitalWrite(LED_BUILTIN, HIGH);
#else
  digitalWrite(LED_BUILTIN, LOW);
#endif
#ifndef INVERT_RELAY_TRIGGER
  digitalWrite(RELAY_PIN, LOW);
#else
  digitalWrite(RELAY_PIN, HIGH);
#endif
#ifdef SERIAL_DEBUG
  Serial.println(F("Logic init!"));
#endif

  WiFi.begin(WIFI_SSID, WIFI_PSK);
  while (!WiFi.isConnected()) {
    delay(1000);
  }
#ifdef SERIAL_DEBUG
  Serial.println(F("WiFi init!"));
  Serial.println("IP Address: " + WiFi.localIP().toString());
#endif

  server.begin();
#ifdef SERIAL_DEBUG
  Serial.println("HTTP init!");
#endif

#ifndef INVERT_LED
  digitalWrite(LED_BUILTIN, LOW);
#else
  digitalWrite(LED_BUILTIN, HIGH);
#endif
}

void loop() {
#ifndef INVERT_RELAY_TRIGGER
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
#else
  digitalWrite(RELAY_PIN, state ? LOW : HIGH);
#endif

  auto client = server.available();
  if (!client) {
    return;
  }

  String header = "";
  String requestLine = "";
  uint32_t requestStart = millis();
  while (client.connected() && millis() - requestStart <= HTTP_TIMEOUT_MS) {
    if (!client.available()) {
      continue;
    }

    char newChar = client.read();
    header += newChar;

    if (newChar == '\n') {
      if (requestLine.length() == 0) {
        client.println(F("HTTP/1.1 200 OK"));
        client.println(F("Content-type:text/html"));
        client.println(F("Connection: close"));
        client.println();

        if (header.indexOf("GET /on") >= 0) {
#ifdef SERIAL_DEBUG
          Serial.println(F("Turning relay on!"));
#endif
          state = true;
        } else if (header.indexOf("GET /off") >= 0) {
#ifdef SERIAL_DEBUG
          Serial.println(F("Turning relay off!"));
#endif
          state = false;
        }

        client.println(F(
            "<!DOCTYPE html>\n"
            "<html><head><meta name=\"viewport\" content=\"width=device-width, "
            "initial-scale=1\"><style>\n"
            "html { font-family: sans-serif; margin: 0px auto; "
            "text-align: center; }\n"
            ".button { background-color: #4CAF50; border: none; color: white; "
            "padding: 16px 40px; text-decoration: none; font-size: 30px; "
            "margin: 2px; cursor: pointer; } .button--off {background-color: "
            "#555555;}\n"
            "</style></head><body>\n"
            "<h1>Printer Light Control</h1>\n"));
        client.printf("<p>Current state: %s</p>\n", state ? "ON" : "OFF");
        if (state) {
          client.println(
              F("<p><a href=\"/off\"><button class=\"button "
                "button--off\">Turn Off</button></a></p>"));
        } else {
          client.println(
              F("<p><a href=\"/on\"><button "
                "class=\"button\">Turn On</button></a></p>"));
        }
        client.println(F("</body></html>"));
        client.println();

#ifdef SERIAL_DEBUG
        Serial.println(F("Replied to client!"));
#endif

        break;
      } else {
        requestLine = "";
      }
    } else if (newChar != '\r') {
      requestLine += newChar;
    }
  }

  client.stop();
}
