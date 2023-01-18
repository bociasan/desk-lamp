#include <Arduino.h>
#include <StripBS.h>
#include <ServerHeader.h>
#include <credentials.h>
#include <ESP8266WiFiMulti.h>


#define pinStrip D1
#define useSerial true
// #define useSerial false

ESP8266WiFiMulti wifiMulti;

const char *ssid = LAMP_SSID;      // The name of the Wi-Fi network that will be created
const char *password = LAMP_PSWD; // The password required to connect to it, leave blank for an open network


#define k 0.02
#define changingDelay 15
#define maxBrightness 90

StripBS ledStrip(pinStrip, maxBrightness, changingDelay, k, useSerial);
unsigned long lastChange = 0;

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledStrip.getState()){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  // return String("???");
  return var;
}

String getStateMessage(){
  bool state = ledStrip.getState();
  float currentBrightness = ledStrip.getCurrentBrightness();
  float rawTargetBrightness = ledStrip.getTargetBrightness();
  float rawMaxBrightness = ledStrip.getMaxBrightness();
  float brightness100 = mapFloat(currentBrightness, 0, rawMaxBrightness, 0, 100);
  float targetBrightness100 = mapFloat(rawTargetBrightness, 0, rawMaxBrightness, 0, 100);
  float maxBrightness100 = mapFloat(rawMaxBrightness, 0, 255, 0, 100);
  String message = "{\"ledstrip\":{\"state\":" + String(state) 
                    + ",\"brightness\":"+ brightness100 
                    + ",\"currentBrightness\":" + currentBrightness
                    + ",\"rawMaxBrightness\":" + rawMaxBrightness
                    + ",\"rawTargetBrightness\":" + rawTargetBrightness
                    + ",\"targetBrightness100\":" + targetBrightness100
                    + ",\"maxBrightness100\":" + maxBrightness100
                    + "}}";
  return message;
}

String getDeviceAboutMessage(){
  return "{\"about\":{\"name\":\"" + device_name + "\", \"version\":\""+ device_version +"\", \"MAC\":\"" + WiFi.macAddress() + "\"}}";
}
void notifyClients() {
  String message = getStateMessage();
  ws.textAll(message);
  ledStrip.clearHasChanges();
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  String message = (char*)data;
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledStrip.toggleState();
    }

    if (message.indexOf("1s") >= 0) {
      int messageInt = message.substring(2).toInt();
      float maxBstrip = ledStrip.getMaxBrightness();
      float targetBrightness = mapFloat(messageInt, 0, 100, 0, maxBstrip);
      if (targetBrightness > 0){
        if (!ledStrip.getState()) ledStrip.toggleState();
        ledStrip.setTargetBrightness(targetBrightness);
      } else {
        ledStrip.turnOff();
      }

      Serial.printf("Target brightness from slider: %d, mapped: %f.\n", messageInt, targetBrightness);
    }

    if (message.indexOf("2s") >= 0) {
      int messageInt = message.substring(2).toInt();
      int newMaxBrightness = map(messageInt, 0, 100, 0, 255);
      ledStrip.setMaxBrightness(newMaxBrightness);
         
      Serial.printf("Max brightness from slider: %d, mapped: %d.\n", messageInt, newMaxBrightness);
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        ws.text(client->id(), getStateMessage());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void startWiFi()
{                              // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  WiFi.mode(WIFI_STA);
  // WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(device_name.c_str());
  //WiFi.softAP(ssid, password); // Start the access point
  // Serial.print("Access Point \"");
  // Serial.print(ssid);
  // Serial.println("\" started\r\n");


  wifiMulti.addAP(ANONYMUS_SSID, ANONYMUS_PSWD); // add Wi-Fi networks you want to connect to
  wifiMulti.addAP(BEAMLOGIC_SSID, BEAMLOGIC_PSWD);
  wifiMulti.addAP(PROIECT_SCR_SSID, PROIECT_SCR_PSWD);
  // wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
    
  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1)
  { // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  if (WiFi.softAPgetStationNum() == 0)
  { // If the ESP is connected to an AP
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID()); // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.print(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
  }
  else
  { // If a station is connected to the ESP SoftAP
    Serial.print("Station connected to ESP8266 AP");
  }
  Serial.println("\r\n");
}

void setup() {
  Serial.begin(115200);
  
  startWiFi();

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // request->send_P(200, "text/html", index_html, processor);
    request->send_P(200, "text/html", index_html);
  });
  
  server.on("/device-about", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/json", getDeviceAboutMessage().c_str());
  });

  // Start server
  server.begin();
}

#define EVERY_MS(x) \
  static uint32_t tmr;\
  bool flag = millis() - tmr >= (x);\
  if (flag) tmr = millis();\
  if (flag)

void loop() {
  ledStrip.tick();

  ws.cleanupClients();

  if(ledStrip.hasChanges()){
    EVERY_MS(100){
      if (ws.count() > 0){
      notifyClients();
      }
    }
  }
}