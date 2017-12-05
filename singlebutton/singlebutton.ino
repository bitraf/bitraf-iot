#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>

#include <Msgflo.h>
#include <PubSubClient.h>

#include "./config.h"
#include "./ledcontrol.hpp"

// uncomment the device you're using
//#define TYPE_NODEMCU 1
#define TYPE_WEMOSD1 2

// uncomment the location for the button
//#define ROLE_LAB 1
//#define ROLE_OFFICE3RDFLOOR 2
#define ROLE_WORKSHOP 3

struct Config {
  #if defined( TYPE_NODEMCU )
    const int ledPin = 14;
    const int buttonPin = 12;
  #elif defined( TYPE_WEMOSD1 )
    const int ledPin = D1;
    const int buttonPin = D2;
  #endif

  #if defined( ROLE_LAB )
    const String role = "bitraf/guestbutton/lab";
  #elif defined( ROLE_OFFICE3RDFLOOR )
    const String role = "bitraf/openbutton/office3rdfloor";
  #elif defined( ROLE_WORKSHOP )
    const String role = "bitraf/openbutton/workshop";
  #endif

  const String wifiSsid = CFG_WIFI_SSID;
  const String wifiPassword = CFG_WIFI_PASSWORD;

  const char *mqttHost = CFG_MQTT_HOST;
  const int mqttPort = 1883;

  const char *mqttUsername = CFG_MQTT_USER;
  const char *mqttPassword = CFG_MQTT_PASSWORD;
} cfg;


const auto participant = msgflo::Participant("bitraf/LedButton", cfg.role);

WiFiClient wifiClient; // used by WiFi
PubSubClient mqttClient;
msgflo::Engine *engine;
msgflo::OutPort *buttonPort;
msgflo::InPort *ledPort;

ledcontrol::Parameters ledParameters;
ledcontrol::State ledState;

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.printf("Configuring wifi: %s\r\n", cfg.wifiSsid.c_str());
  WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPassword.c_str());
  WiFi.mode(WIFI_STA);

  mqttClient.setServer(cfg.mqttHost, cfg.mqttPort);
  mqttClient.setClient(wifiClient);

  const String clientId = cfg.role + WiFi.macAddress();

  engine = msgflo::pubsub::createPubSubClientEngine(participant, &mqttClient,
    clientId.c_str(), cfg.mqttUsername, cfg.mqttPassword);

  buttonPort = engine->addOutPort("button", "any", cfg.role+"/button");

  ledPort = engine->addInPort("led", "boolean", cfg.role+"/led",
  [](byte *data, int length) -> void {
      const std::string in((char *)data, length);
      ledParameters = ledcontrol::parse(in);
  });

  Serial.printf("Led pin: %d\r\n", cfg.ledPin);
  Serial.printf("Button pin: %d\r\n", cfg.buttonPin);

  pinMode(cfg.buttonPin, INPUT_PULLUP);
  pinMode(cfg.ledPin, OUTPUT);
  analogWriteRange(255);
}

void loop() {
  static bool connected = false;
  static bool was_pressed = false;
  static long nextUpdate = 0;

  if (WiFi.status() == WL_CONNECTED) {
    if (!connected) {
      Serial.printf("Wifi connected: ip=%s\r\n", WiFi.localIP().toString().c_str());
    }
    connected = true;
    engine->loop();
  } else {
    if (connected) {
      connected = false;
      Serial.println("Lost wifi connection.");
    }
  }

  // LED control
  const int pwm = ledcontrol::next(ledState, ledParameters, millis());
  if (pwm >= 0) {
    analogWrite(cfg.ledPin, pwm);
  }

  // Send immediately on state change, else periodically  
  const bool pressed = !digitalRead(cfg.buttonPin);
  if (pressed != was_pressed) {
    buttonPort->send(pressed ? "true" : "false");
  }
  was_pressed = pressed;
  if (millis() > nextUpdate) {
    buttonPort->send(pressed ? "true" : "false");
    nextUpdate += 30*1000;
  }

}

