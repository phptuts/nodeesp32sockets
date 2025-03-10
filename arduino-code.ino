#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ESP32Servo.h>

const char* ssid = "Noisebridge";
const char* password = "noisebridge";
const char* websocket_server = "Replace with local ip";  // in the terminal type ifconfig
const int websocket_port = 3500;

WebSocketsClient webSocket;
Servo myServo;
const int servoPin = 23;  // Set to your servo pin

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    String message;  // Declare the variable outside the switch
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("WebSocket Disconnected!");
            break;
        case WStype_CONNECTED:
            Serial.println("WebSocket Connected!");
            break;
        case WStype_TEXT: {
            String message = String((char*)payload);  // Convert payload to String
            Serial.printf("Received command: %s\n", message.c_str());

            // Ensure the message starts with "angle-"
            if (message.startsWith("angle-")) {
                String angleStr = message.substring(6); // Extract number after "angle-"
                int angle = angleStr.toInt(); // Convert extracted part to integer

                if (angle >= 0 && angle <= 180) {  // Ensure valid angle
                    myServo.write(angle);
                    Serial.printf("Servo moved to %d degrees\n", angle);
                } else {
                    Serial.println("Invalid angle received");
                }
            } else if (message == "HIGH_FIVE") {
              myServo.write(0);
              delay(100);
              myServo.write(90);
              delay(3000);
              myServo.write(0);
            }
            else {
                Serial.println("Invalid message format");
            }
            break;
        }

        case WStype_ERROR:
            Serial.println("WebSocket Error!");
            break;

        default:
            Serial.println("Unknown WebSocket event");
            break;
    }
}

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");

    myServo.attach(servoPin);

    webSocket.begin(websocket_server, websocket_port, "/");
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
}
