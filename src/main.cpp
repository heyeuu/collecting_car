#include <Arduino.h>
#include <AsyncTCP.h>
// #include <ESP32Servo.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Wire.h>

// #include "hardware/ultrasonic.hpp"
#include "hardware/car/car.hpp"
// #include "pins.hpp"
// Ultrasonic ultrasonic;
Car car;
// constexpr auto ssid = "Esp32ServerWifi";

constexpr auto ssid = "AllianceTeam2.4G";
const char* password = "12345678";

IPAddress localIP(192, 168, 1, 199);
IPAddress gateWay(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

bool ledState = 0;
const int ledPin = 2;

double joystickX = 0;
double joystickY = 0;
double distance = 0;

double leftVelocity = 0;
double rightVelocity = 0;

AsyncWebServer server(80);
AsyncWebSocket wsRobotCmd("/cmd");
AsyncWebSocket wsCameraStream("/stream");

void initWiFi();

void notifyClients();
void handleWebSocketMessage(void* arg, uint8_t* data, size_t len);
void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
    void* arg, uint8_t* data, size_t len);
void initWebSocket();
String processor(const String& var);
String replacePlaceholders(String html);

void handleRoot(AsyncWebServerRequest* request);
void handleStream(AsyncWebServerRequest* request);

void handleNotFound(AsyncWebServerRequest* request);

void setup() {
    Serial.begin(115200);
    car.turn(1, 0);
    // pinMode(ledPin, OUTPUT);
    // digitalWrite(ledPin, LOW);

    // initWiFi();

    // initWebSocket();
    // server.on("/", HTTP_GET, handleRoot);
    // server.onNotFound(handleNotFound);
    // server.begin();
    // digitalWrite(ledPin, HIGH);
    // Serial.print("ESP32 MAC 地址: ");
    // Serial.println(WiFi.macAddress());
    // Serial.println("ip:");
    // Serial.println(WiFi.localIP());
}

void loop() {
    wsRobotCmd.cleanupClients();
    // digitalWrite(ledPin, ledState);
}

void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.config(localIP, gateWay, subnet);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }

    // WiFi.mode(WIFI_AP);
    // WiFi.softAPConfig(IPAddress(192, 168, 233, 233), IPAddress(192, 168, 233, 0),
    //     IPAddress(255, 255, 255, 0));
    // WiFi.softAP(ssid);
}

void notifyClients() {
    String message = String("{\"xPoint\":") + String(joystickX) + ",\"yPoint\":" + String(joystickY) + ",\"leftVelocity\":" + String(leftVelocity) + ",\"rightVelocity\":" + String(rightVelocity) + "}";
    wsRobotCmd.textAll(message);
}

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        String message = (char*)data;

        if (message.startsWith("{") && message.endsWith("}")) {
            // Allocate the JSON document
            JsonDocument doc;

            // Deserialize the JSON document
            DeserializationError error = deserializeJson(doc, message);

            // Test if parsing succeeds
            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            } else {
                joystickX = doc["xPoint"];
                joystickY = doc["yPoint"];
                distance = doc["distance"];
            }
        }
        notifyClients();
    }
}

void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
    void* arg, uint8_t* data, size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
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
    } // when notify?
}

void initWebSocket() {
    wsRobotCmd.onEvent(onEvent);
    server.addHandler(&wsRobotCmd);
}

String processor(const String& var) {

    if (var == "X") {
        return String(joystickX, 2);
    }
    if (var == "Y") {
        return String(joystickY, 2);
    }

    if (var == "LEFT_VELOCITY") {
        return String(leftVelocity, 2);
    }
    if (var == "RIGHT_VELOCITY") {
        return String(rightVelocity, 2);
    }
    return String();
}

// String replacePlaceholders(String html) {
//     html.replace("{{STATE}}", processor("STATE"));
//     html.replace("{{X}}", processor("X"));
//     html.replace("{{Y}}", processor("Y"));

//     html.replace("{{LEFT_VELOCITY}}", processor("LEFT_VELOCITY"));
//     html.replace("{{RIGHT_VELOCITY}}", processor("RIGHT_VELOCITY"));

//     return html;
// }

void handleRoot(AsyncWebServerRequest* request) {
    // String html = web::index_html;
    // html = replacePlaceholders(html);
    // request->send(200, "text/html", html);
    // request->send_P(200, "text/html", index_html, processor);)
}

void handleNotFound(AsyncWebServerRequest* request) {
    request->send_P(404, "text/plain", "File Not Found");
}

void handleStream(AsyncWebServerRequest* request) { }