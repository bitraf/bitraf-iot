
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>

#include <Msgflo.h>
#include <PubSubClient.h>

struct Config {
  const int ledPin = 14;
  const int buttonPin = 12;
  const String role = "button";

  const String wifiSsid = "jhaven";
  const String wifiPassword = "abcdefgh";

  const char *mqttHost = "192.168.43.85";
  const int mqttPort = 1883;

  const char *mqttUsername = "myuser";
  const char *mqttPassword = "mypassword";
} cfg;


const auto participant = msgflo::Participant("fosdem2017/Relay", cfg.role);

WiFiClient wifiClient; // used by WiFi
PubSubClient mqttClient;
msgflo::Engine *engine;
msgflo::OutPort *buttonPort;
msgflo::InPort *ledPort;

long nextButtonCheck = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.printf("Configuring wifi: %s\r\n", cfg.wifiSsid.c_str());
  WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPassword.c_str());

  mqttClient.setServer(cfg.mqttHost, cfg.mqttPort);
  mqttClient.setClient(wifiClient);

  const String clientId = cfg.role + WiFi.macAddress();

  engine = msgflo::pubsub::createPubSubClientEngine(participant, &mqttClient,
    clientId.c_str(), cfg.mqttUsername, cfg.mqttPassword);

  buttonPort = engine->addOutPort("button", "any", cfg.role+"/button");

  ledPort = engine->addInPort("led", "boolean", cfg.role+"/led",
  [](byte *data, int length) -> void {
      const std::string in((char *)data, length);
      const boolean on = (in == "1" || in == "true");
      digitalWrite(cfg.ledPin, on);
  });

  Serial.printf("Led pin: %d\r\n", cfg.ledPin);
  Serial.printf("Button pin: %d\r\n", cfg.buttonPin);

  pinMode(cfg.buttonPin, INPUT_PULLUP);
  pinMode(cfg.ledPin, OUTPUT);
}

void loop() {
  static bool connected = false;

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

  // TODO: check for statechange. If changed, send right away. Else only send every 3 seconds or so
  if (millis() > nextButtonCheck) {
    const bool pressed = digitalRead(cfg.buttonPin);
    buttonPort->send(pressed ? "true" : "false");
    nextButtonCheck += 100;
  }

}

