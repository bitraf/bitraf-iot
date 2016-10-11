#include "msgflo.h"
#include "utils.h"
#include "config.h"

#if not defined(BOX_ID) || \
    not defined(WIFI_SSID) || \
    not defined(WIFI_PASSWORD) || \
    not defined(MQTT_HOST) || \
    not defined(MQTT_PORT) || \
    not defined(MQTT_USERNAME) || \
    not defined(MQTT_PASSWORD)
#error Missing required config parameter. See config.h
#endif

#include <Arduino.h>
#include <ArduinoOTA.h>

void setupOta() {
  static fixed_interval_timer<1000> ota_blink;

  static int ota_color = 0;
  ArduinoOTA.onStart([]() {
    Serial.println("Starting OTA upgrade");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println();
    Serial.println("OTA upgrade complete");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  Serial.print("OTA Password: ");
  // if (cfg.otaPassword.length()) {
  //   Serial.println(cfg.otaPassword);
  //   ArduinoOTA.setPassword(cfg.otaPassword.c_str());
  // } else {
  Serial.println("Not set");
  // }

  ArduinoOTA.begin();
}

struct Config {
  const String id = BOX_ID;

  const int ledPin = 5;

  const String wifiSsid = WIFI_SSID;
  const String wifiPassword = WIFI_PASSWORD;

  const char *mqttHost = MQTT_HOST;
  const int mqttPort = MQTT_PORT;

  const char *mqttUsername = MQTT_USERNAME;
  const char *mqttPassword = MQTT_PASSWORD;
} cfg;

WiFiClient wifiClient;
PubSubClient mqttClient;
msgflo::Engine *engine;
msgflo::OutPort *buttonPort;

PinPoller<100, 0> button;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println();

  setupOta();

  Serial.printf("Configuring wifi: %s\r\n", cfg.wifiSsid.c_str());
  WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPassword.c_str());

  mqttClient.setServer(cfg.mqttHost, cfg.mqttPort);
  mqttClient.setClient(wifiClient);

  String clientId = "bitraf-button-";
  clientId += WiFi.macAddress();

  engine = msgflo::pubsub::createPubSubClientEngine(
             "bitraf-iot/BlinkButton",
             "Blinking Button",
             "lightbulb-o",
             &mqttClient, clientId.c_str(), cfg.mqttUsername, cfg.mqttPassword);

  buttonPort = engine->addOutPort("button-event", "any", "bitraf/button/" + cfg.id + "/event");

  Serial.printf("Led pin: %d\r\n", cfg.ledPin);
  Serial.printf("Button pin: %d\r\n", button.pin);
  pinMode(button.pin, INPUT);
  pinMode(cfg.ledPin, OUTPUT);
}

void loop() {
  static bool connected = false;

  ArduinoOTA.handle();

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

  button.loop();

  /*
  static fixed_interval_timer<3000> blink_timer;

  static bool on = false;
  if (blink_timer.expired()) {
    on = !on;
    Serial.printf("on=%d\r\n", on);
  }
  */

  if (button.changed()) {
    Serial.printf("button changed: ts=%u %d\r\n", millis(), button.state());
    buttonPort->send("!");
  }

  bool b = digitalRead(button.pin);
  digitalWrite(cfg.ledPin, !b);
}

