// By Thor

#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>

#include <Msgflo.h>
#include <PubSubClient.h>

#include "./config.h"

struct Config {
  const int pirPin = D5;
  const String role = "public/motionsensor/lab";

  const String wifiSsid = CFG_WIFI_SSID;
  const String wifiPassword = CFG_WIFI_PASSWORD;

  const char *mqttHost = CFG_MQTT_HOST;
  const int mqttPort = 1883;

  const char *mqttUsername = CFG_MQTT_USER;
  const char *mqttPassword = CFG_MQTT_PASSWORD;
} cfg;


const auto participant = msgflo::Participant("bitraf/PirMotionSensor", cfg.role);

WiFiClient wifiClient; // used by WiFi
PubSubClient mqttClient;
msgflo::Engine *engine;
msgflo::OutPort *pirPort;

bool val = 0;
bool pirState = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.printf("Configuring wifi: %s\r\n", cfg.wifiSsid.c_str());
  WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPassword.c_str());

  mqttClient.setServer(cfg.mqttHost, cfg.mqttPort);
  mqttClient.setClient(wifiClient);

  const String clientId = cfg.role + WiFi.macAddress();

  engine = msgflo::pubsub::createPubSubClientEngine(participant, &mqttClient,
    clientId.c_str());

  pirPort = engine->addOutPort("motion", "boolean", cfg.role+"/motion");


  Serial.printf("Motion pin: %d\r\n", cfg.pirPin);

  pinMode(cfg.pirPin, INPUT);
}

void loop() {
  static bool connected = false;

  if (WiFi.status() == WL_CONNECTED) {
    engine->loop(); // Run Msgflo/MQTT
    
    if (!connected) {
      Serial.printf("Wifi connected: ip=%s\r\n", WiFi.localIP().toString().c_str());
   
      // Send first state right away
      pirState = digitalRead(cfg.pirPin);
      if (pirState == HIGH) {
        Serial.println("Initial state: HIGH");
        pirPort->send("true");
      } else {
        Serial.println("Initial state: LOW");
        pirPort->send("false");
      }
    }
    connected = true;
    
  } else {
    if (connected) {
      connected = false;
      Serial.println("Lost wifi connection.");
    }
  }
  val = digitalRead(cfg.pirPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
     if (pirState == LOW) {
        // we have just turned on
        Serial.println("LOW to HIGH");
        pirPort->send("true");
        // We only want to send on the output change, not state
        pirState = HIGH;
      }
  } else {
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("HIGH to LOW");
      pirPort->send("false");
      // We only want to send on the output change, not state
      pirState = LOW;
    }
  }     
}

