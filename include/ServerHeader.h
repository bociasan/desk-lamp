#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SiteHeader.h>

const char* ssid = "Anonymus";
const char* password = "camera18";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");