//BHB

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Pins
const int relayPin = 25;
const int buzzerPin = 26;

// Server and WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool relayState = false;
unsigned long buzzerTimer = 0;
bool buzzerOn = false;

// HTML Page with embedded JS WebSocket client
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Relay & Buzzer WebSocket Control</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    button { padding: 20px; font-size: 24px; cursor: pointer; }
    #status { margin-top: 20px; font-size: 18px; }
  </style>
</head>
<body>
  <h1>Relay & Buzzer Control (WebSocket)</h1>
  <button id="toggleBtn">Loading...</button>
  <div id="status"></div>

<script>
  let ws = new WebSocket(`ws://${location.host}/ws`);

  ws.onopen = () => {
    console.log('WebSocket connected');
    ws.send(JSON.stringify({action: "getState"}));
  };

  ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    if (data.relay !== undefined) {
      document.getElementById('toggleBtn').innerText = data.relay ? 'Turn OFF' : 'Turn ON';
      document.getElementById('status').innerText = data.relay ? 'Relay is ON. Buzzer beeping...' : 'Relay is OFF. Buzzer silent.';
    }
  };

  ws.onclose = () => {
    console.log('WebSocket disconnected');
    document.getElementById('toggleBtn').innerText = 'Disconnected';
  };

  document.getElementById('toggleBtn').onclick = () => {
    ws.send(JSON.stringify({action: "toggle"}));
  };
</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  StaticJsonDocument<200> doc;
  doc["relay"] = relayState;
  String jsonStr;
  serializeJson(doc, jsonStr);
  ws.textAll(jsonStr);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0; // Null terminate string
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, (char*)data);
      if (!error) {
        const char* action = doc["action"];
        if (strcmp(action, "toggle") == 0) {
          relayState = !relayState;
          digitalWrite(relayPin, relayState ? HIGH : LOW);
          buzzerOn = false;
          buzzerTimer = millis();
          notifyClients();
        } else if (strcmp(action, "getState") == 0) {
          notifyClients();
        }
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  digitalWrite(buzzerPin, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

void loop() {
  // Buzzer beep logic when relay ON
  if (relayState) {
    unsigned long currentMillis = millis();
    if (currentMillis - buzzerTimer >= 500) { // 500 ms beep cycle
      buzzerOn = !buzzerOn;
      digitalWrite(buzzerPin, buzzerOn ? HIGH : LOW);
      buzzerTimer = currentMillis;
    }
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}
