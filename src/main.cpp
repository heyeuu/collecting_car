#include <Arduino.h>
#include <AsyncTCP.h>
// #include <ESP32Servo.h>
#include "hardware/motor/motor.hpp"
#include "web_server/index.hh"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Wire.h>
#include <iostream>
// #include "hardware/ultrasonic.hpp"
// #include "pins.hpp"

// Ultrasonic ultrasonic;
auto motor1 = Motor(1);

constexpr auto wifi_name = "Esp32ServerWifi";

// constexpr auto wifi_name = "AllianceTeam2.4G";
// const char* password = "12345678";

// bool ledState = 0;
// const int ledPin = 2;

double joystick_x = 0;
double joystick_y = 0;

double left_velocity = 0;
double right_velocity = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket wsRobotCmd("/cmd");
AsyncWebSocket wsCameraStream("/stream");

void notifyClients();
void handleWebSocketMessage(void* arg, uint8_t* data, size_t len);
void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
    void* arg, uint8_t* data, size_t len);
void initWebSocket();
String processor(const String& var);
String replacePlaceholders(String html);

void handle_root(AsyncWebServerRequest* request);
void handle_stream(AsyncWebServerRequest* request);

void handle_not_find(AsyncWebServerRequest* request);

void setup() {
    // Serial port for debugging purposes
    Serial.begin(115200);

    // pinMode(ledPin, OUTPUT);
    // digitalWrite(ledPin, LOW);

    motor1.initialize(2, 0, 16, 1);
    // WiFi.mode(WIFI_STA);
    // WiFi.begin(wifi_name, password);

    // Connect to Wi-Fi
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 168, 233, 233), IPAddress(192, 168, 233, 0),
        IPAddress(255, 255, 255, 0));

    WiFi.softAP(wifi_name);
    // Print ESP Local IP Address
    // Serial.println(WiFi.localIP());

    initWebSocket();

    // Route for root / web page
    server.on("/", HTTP_GET, handle_root);
    // Start server
    server.begin();
}

void loop() {
    wsRobotCmd.cleanupClients();
    // digitalWrite(ledPin, ledState);
    delay(500);
}

void notifyClients() {
    String message = String("{\"ledState\":") + String(ledState) + ",\"xPoint\":" + String(joystick_x) + ",\"yPoint\":" + String(joystick_y) + ",\"leftVelocity\":" + String(left_velocity) + ",\"rightVelocity\":" + String(right_velocity) + "}";
    wsRobotCmd.textAll(message);
}

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        String message = (char*)data;
        if (strcmp((char*)data, "toggle") == 0) {
            ledState = !ledState;
            notifyClients();
        }
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
                joystick_x = doc["xPoint"];
                joystick_y = doc["yPoint"];
                notifyClients();
            }
        }
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
    }
}

void initWebSocket() {
    wsRobotCmd.onEvent(onEvent);
    server.addHandler(&wsRobotCmd);
}

String processor(const String& var) {
    Serial.println(var);
    if (var == "STATE") {
        return ledState ? "ON" : "OFF";
    }
    if (var == "X") {
        return String(joystick_x, 2);
    }
    if (var == "Y") {
        return String(joystick_y, 2);
    }

    if (var == "LEFT_VELOCITY") {
        return String(left_velocity, 2);
    }
    if (var == "RIGHT_VELOCITY") {
        return String(right_velocity, 2);
    }
    return String();
}

String replacePlaceholders(String html) {
    html.replace("{{STATE}}", processor("STATE"));
    html.replace("{{X}}", processor("X"));
    html.replace("{{Y}}", processor("Y"));

    html.replace("{{LEFT_VELOCITY}}", processor("LEFT_VELOCITY"));
    html.replace("{{RIGHT_VELOCITY}}", processor("RIGHT_VELOCITY"));

    return html;
}

void handle_root(AsyncWebServerRequest* request) {
    String html = web::index_html;
    html = replacePlaceholders(html);
    request->send(200, "text/html", html);
    // request->send_P(200, "text/html", index_html, processor);)
}

void handleNotFound(AsyncWebServerRequest* request) {
    request->send_P(404, "text/plain", "File Not Found");
}

void handle_stream(AsyncWebServerRequest* request) { }