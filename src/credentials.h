#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include <ESP_WiFiManager_Lite.h>

#define ESP_WM_LITE_DEBUG_OUTPUT Serial

#define USE_LED_BUILTIN false
#define USING_MRD true
#define MULTIRESETDETECTOR_DEBUG false
#define MRD_TIMEOUT 10
#define MRD_ADDRESS 0

#if defined(ARDUINO_ESP32C3_DEV)
#define USE_LITTLEFS false
#define USE_SPIFFS true
#else
#define USE_LITTLEFS true
#define USE_SPIFFS false
#endif

#define TIMEOUT_RECONNECT_WIFI 10000L
#define RESET_IF_CONFIG_TIMEOUT true
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET 5
#define CONFIG_TIMEOUT 120000L
#define REQUIRE_ONE_SET_SSID_PW true
#define MAX_NUM_WIFI_RECON_TRIES_PER_LOOP 5
#define RESET_IF_NO_WIFI false
#define USE_DYNAMIC_PARAMETERS true
#define SCAN_WIFI_NETWORKS true
#define MANUAL_SSID_INPUT_ALLOWED true
#define MAX_SSID_IN_LIST 10
#define USING_BOARD_NAME true

#define MAX_PIN_LEN 2
#define MAX_PORT_LEN 6
#define MAX_NAME_LEN 15

bool LOAD_DEFAULT_CONFIG_DATA = false;

ESP_WM_LITE_Configuration defaultConfig = {"", "", "", "", "", "", 0};

char RelayPin[MAX_PIN_LEN + 1] = "14";
char HttpPort[MAX_PORT_LEN + 1] = "80";

MenuItem myMenuItems[] = {{"rp", "Relay Pin", RelayPin, MAX_PIN_LEN},
                          {"hp", "HTTP Port", HttpPort, MAX_PORT_LEN}};

uint16_t NUM_MENU_ITEMS = sizeof(myMenuItems) / sizeof(MenuItem);

#endif