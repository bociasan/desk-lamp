#include <Arduino.h>
#include <StripBS.h>
#include <ServerHeader.h>

#define pinStrip D1
#define useSerial true
// #define useSerial false

#define k 0.02
#define maxBrightness 90
#define changingDelay 15

StripBS ledStrip(pinStrip, maxBrightness, changingDelay, k, useSerial);
bool needNotify = false;

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
  return String();
}

String getStateMessage(){
  bool state = ledStrip.getState();
  float rawBrightness = ledStrip.getCurrentBrightness();
  float brightness = trunc(rawBrightness*100/maxBrightness);
  String message = "{\"ledstrip\":{\"state\":" + String(state) + ",\"brightness\":" + brightness+ ",\"rawBrightness\":" + rawBrightness+"}}";
  return message;
}

void notifyClients() {
  // bool state = ledStrip.getState();
  // float rawBrightness = ledStrip.getCurrentBrightness();
  // float brightness = trunc(rawBrightness*100/maxBrightness);
  // String message = "{\"ledstrip\":{\"state\":" + String(state) + ",\"brightness\":" + brightness+ ",\"rawBrightness\":" + rawBrightness+"}}";

  ws.textAll(getStateMessage());
  ledStrip.clearHasChanges();
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledStrip.toggleState();
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        // String message = getStateMessage();
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



void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
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
    if (ws.count() > 0){
      notifyClients();
    }
  }
}

