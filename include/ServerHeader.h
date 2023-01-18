#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SiteHeader.h>


String device_version = "v1.1";
String device_name = "Wemos-Smart-Lamp";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");