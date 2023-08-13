#include <Arduino.h>
#include <WiFi.h>

// #define SERIAL_DEBUG

#define WIFI_SSID "bar"
#define WIFI_PSK "wi9NNYara"

#define HTTP_TIMEOUT_MS 5000

#define PIN_LED 5
#define PIN_SWITCH 14

WiFiServer server(80);
bool state = false;
uint32_t requestStart;

void setup() {
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  while (!Serial) {
    delayMicroseconds(100);
  }
  Serial.println(F("Serial init!"));
#endif

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SWITCH, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  digitalWrite(PIN_SWITCH, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PSK);
  while (!WiFi.isConnected()) {
    delay(1000);
  }
#ifdef SERIAL_DEBUG
  Serial.println(F("WiFi init!"));
  Serial.println(WiFi.localIP().toString());
#endif

  server.begin();
#ifdef SERIAL_DEBUG
  Serial.println("HTTP init!");
#endif

  digitalWrite(PIN_LED, LOW);
}

void loop() {
  digitalWrite(PIN_SWITCH, state ? HIGH : LOW);

  auto client = server.available();
  if (!client) {
    return;
  }

  String request = "";
  String requestLine = "";
  requestStart = millis();
  while (client.connected() && millis() - requestStart <= HTTP_TIMEOUT_MS) {
    if (!client.available()) {
      continue;
    }

    char newChar = client.read();
    request += newChar;

    if (newChar == '\n') {
      if (requestLine.length() == 0) {
        client.println(F("HTTP/1.1 200 OK"));
        client.println(F("Content-type:text/html"));
        client.println(F("Connection: close"));
        client.println();

        if (request.indexOf("GET /on") >= 0) {
#ifdef SERIAL_DEBUG
          Serial.println(F("Turning relay on!"));
#endif
          state = true;
        } else if (request.indexOf("GET /off") >= 0) {
#ifdef SERIAL_DEBUG
          Serial.println(F("Turning relay off!"));
#endif
          state = false;
        }

        client.println(F("<!DOCTYPE html><html>"));
        client.println(
            F("<head><meta name=\"viewport\" content=\"width=device-width, "
              "initial-scale=1\"><style>"));
        client.println(
            F("html { font-family: sans-serif; margin: 0px auto; "
              "text-align: center; }"));
        client.println(F(
            ".button { background-color: #4CAF50; border: none; color: white; "
            "padding: 16px 40px; text-decoration: none; font-size: 30px; "
            "margin: 2px; cursor: pointer; } .button--off {background-color: "
            "#555555;}"));
        client.println(F("</style></head><body>"));
        client.println(F("<h1>Printer Light Control</h1>"));
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
