#include "DHT.h" // "DHT sensor library" by Adafruit.
#include "msgflo.h"
#include "utils.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#if not defined(BOX_ID) || \
    not defined(WIFI_SSID) || \
    not defined(WIFI_PASSWORD) || \
    not defined(MQTT_HOST) || \
    not defined(MQTT_PORT) || \
    not defined(MQTT_USERNAME) || \
    not defined(MQTT_PASSWORD)
#error Missing required config parameter. See config.h
#endif

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
msgflo::OutPort *temperaturePort;
msgflo::OutPort *humidityPort;

PinPoller<100, 0> button;

#ifdef USE_DHT
DHT dht(13, DHT22, 15);

static const unsigned int dht_sample_interval_ms = 5000;
static const unsigned int dht_samples_per_message = 60000 / dht_sample_interval_ms;
#endif

void dht22_setup() {
#ifdef USE_DHT
  dht.begin();
#endif
}

void dht22_loop() {
#ifdef USE_DHT
  static fixed_interval_timer<dht_sample_interval_ms> timer;
  static averager<float, dht_samples_per_message> temp_avg, hum_avg;

  if (timer.expired()) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    temp_avg += temp;
    hum_avg += hum;

    Serial.print("temp=");
    Serial.print(temp);
    Serial.print(", avg=");
    Serial.print(temp_avg.taste());
    Serial.println();

    if (temp_avg.full()) {
      float value = temp_avg.read();

      // if (isnan(value)) value = -1.0;

      Serial.print("temp_avg=");
      Serial.print(value);
      Serial.println();

      if (!isnan(value)) {
        Serial.print("temperaturePort: ");
        Serial.print((uint32_t)temperaturePort, HEX);
        Serial.print(", queue='");
        // Serial.println((uint32_t)temperaturePort);
        Serial.print(temperaturePort->queue());
        Serial.println("'");
        String msg;
        msg += value;
        temperaturePort->send(msg);
      }
    }

    if (hum_avg.full()) {
      float value = hum_avg.read();
      Serial.print("hum_avg=");
      Serial.print(value);
      Serial.println();

      if (!isnan(value)) {
        String msg;
        msg += value;
        humidityPort->send(msg);
      }
    }
  }
#endif
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("Bitraf Button. build: " __DATE__ " " __TIME__);

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

  buttonPort = engine->addOutPort("button-button", "any", "bitraf/button/" + cfg.id);
  temperaturePort = engine->addOutPort("button-temperature", "any", "bitraf/temperature/" + cfg.id);
  humidityPort = engine->addOutPort("button-humidity", "any", "bitraf/humidity/" + cfg.id);

  auto q = temperaturePort->queue();
  Serial.print("temperaturePort: ");
  Serial.println(q);

  Serial.printf("Led pin: %d\r\n", cfg.ledPin);
  Serial.printf("Button pin: %d\r\n", button.pin);
  pinMode(button.pin, INPUT);
  pinMode(cfg.ledPin, OUTPUT);

  dht22_setup();
}

void loop() {
  static bool connected = false;
  static fixed_interval_timer<100> xtimer;
  if (xtimer.expired()) {
    Serial.print("free heapX: ");
    Serial.println(ESP.getFreeHeap());
  }

  ArduinoOTA.handle();

  if (WiFi.status() == WL_CONNECTED) {
    if (!connected) {
      Serial.printf("Wifi connected: ip=%s\r\n", WiFi.localIP().toString().c_str());
    }
    connected = true;
    engine->loop();
  } else if (connected) {
    connected = false;
    Serial.println("Lost wifi connection.");
  }

  button.loop();

  /*
    static fixed_interval_timer<5000> blink_timer;
    if (blink_timer.expired()) {
    }
  */

  if (button.changed()) {
    Serial.printf("button changed: ts=%u %d\r\n", millis(), button.state());
    Serial.print("buttonPort: ");
    Serial.println((uint32_t)buttonPort);
    // buttonPort->send("!");
  }

  bool b = digitalRead(button.pin);
  digitalWrite(cfg.ledPin, !b);

  dht22_loop();
}

