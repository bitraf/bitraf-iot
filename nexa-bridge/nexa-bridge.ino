#include "Tx433_Nexa.h"
#include "trygvis_arduino.h"

#include <WiFiManager.h> // Version 0.12.0
#include <PubSubClient.h>
#include <Wire.h>

using namespace trygvis::arduino;

String ch_nexa = "1010";

const int NEXA_PIN = D0;

WiFiManager wifiManager;
WiFiClient wifi_client;
PubSubClient mqtt;

const String mqtt_client_id = "bitraf-" + WiFi.macAddress();
String mqtt_host = "mqtt.bitraf.no";
String mqtt_username;
String mqtt_password;
int mqtt_port = 1883;
String instance_id;
int config_ok = 0;

bool save_config = false;

static const int CONFIG_PIN = D3;

void load_config() {
  with_spiffs spiffs;

  if (!spiffs.ok()) {
    return;
  }

  if (!SPIFFS.exists("config")) {
    Serial.println("No config file");
    return;
  }

  File f = SPIFFS.open("config", "r");
  if (!f) {
    Serial.println("Could not open config file");
    ESP.reset();
    delay(1000);
  }

  Serial.println("Reading config file");

  mqtt_host = f.readStringUntil('\n');
  Serial.println(mqtt_host);

  mqtt_port = f.readStringUntil('\n').toInt();
  Serial.println(mqtt_port);

  mqtt_username = f.readStringUntil('\n');
  Serial.println(mqtt_username);

  mqtt_password = f.readStringUntil('\n');
  Serial.println(mqtt_password);

  instance_id = f.readStringUntil('\n');
  Serial.println(instance_id);

  config_ok = f.readStringUntil('\n').toInt();
  f.close();
}

void write_config() {
  with_spiffs spiffs;

  if (!spiffs.ok()) {
    return;
  }

  File f = SPIFFS.open("config", "w");
  if (!f) {
    Serial.println("Could not open config file");
    ESP.reset();
    delay(1000);
  }

  f.print(mqtt_host);
  f.print('\n');
  f.print(mqtt_port);
  f.print('\n');
  f.print(mqtt_username);
  f.print('\n');
  f.print(mqtt_password);
  f.print('\n');
  f.print(instance_id);
  f.print('\n');
  f.print(1337);
  f.print('\n');

  f.close();

  Serial.println("wrote config file");
}

void nuke_config() {
  with_spiffs spiffs;

  if (!spiffs.ok()) {
    return;
  }

  SPIFFS.remove("config");
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("\n\nMQTT Nexa Bridge");

  mqtt.setCallback(mqttCallback);

  Serial.printf("MQTT Client id: %s\n", mqtt_client_id.c_str());

  pinMode(CONFIG_PIN, INPUT);

  load_config();

  if (config_ok != 1337) {
    run_wifi_manager();
  } else {
    WiFiManager wifiManager;
    wifiManager.autoConnect();
  }

  mqtt.setClient(wifi_client);
}

void save_config_callback()
{
  save_config = true;
}

void run_wifi_manager()
{
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(save_config_callback);

  WiFiManagerParameter custom_mqtt_host("mqtt_host", "MQTT host", mqtt_host.c_str(), 40);
  wifiManager.addParameter(&custom_mqtt_host);

  WiFiManagerParameter custom_mqtt_username("mqtt_username", "MQTT Username", mqtt_username.c_str(), 40);
  wifiManager.addParameter(&custom_mqtt_username);

  WiFiManagerParameter custom_mqtt_password("mqtt_password", "MQTT Password", mqtt_password.c_str(), 40);
  wifiManager.addParameter(&custom_mqtt_password);

  WiFiManagerParameter custom_instance_id("instance_id", "Instance (1...)", "", 40);
  wifiManager.addParameter(&custom_instance_id);

  if (!wifiManager.startConfigPortal(mqtt_client_id.c_str())) {
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  if (save_config) {
    mqtt_host = custom_mqtt_host.getValue();
    mqtt_username = custom_mqtt_username.getValue();
    mqtt_password = custom_mqtt_password.getValue();
    instance_id = String(custom_instance_id.getValue()).toInt();
    config_ok = 1337;
    write_config();
  }
}

void mqttCallback(const char* t, byte* payload, unsigned int length)
{
  String topic(t);
  String body;
  for (int i = 0; i < length; i++) {
    body += static_cast<char>(payload[i]);
  }

  Serial.printf("MQTT msg: %s\n", t);
  Serial.println(body);

  int i1 = topic.lastIndexOf('/');
  int i2 = topic.lastIndexOf('/', i1 - 1);

  if (i1 < 0 || i2 < 0) {
    return;
  }

  String part1 = topic.substring(i2 + 1, i1);
  String part2 = topic.substring(i1 + 1);

  Serial.printf("part1=%s\n", part1.c_str());
  Serial.printf("part2=%s\n", part2.c_str());

  if (part1 == "cmd") {
    String id;
    id.reserve(52);
    for (int i = 0; i < part2.length(); i++) {
      char c = part2[i];
      if (c == '0') {
        id += '0';
        id += '1';
      } else if (c == '1') {
        id += '1';
        id += '0';
      } else {
        Serial.printf("Bad cmd topic: %s\n", part2.c_str());
        return;
      }
    }

    if (id.length() != 52) {
      Serial.println("Invalid topic length, has to be 26 characters");
      return;
    }

    Serial.printf("ID: %s\n", id.c_str());

    Tx433_Nexa nexa(NEXA_PIN, id, ch_nexa);
    if (body == "off") {
      Serial.printf("Sending OFF to: %s\n", id.c_str());
      nexa.Device_Off(0);
    } else if (body == "on") {
      Serial.printf("Sending ON to: %s\n", id.c_str());
      nexa.Device_On(0);
    } else {
      Serial.printf("Bad payload: %s\n", body.c_str());
    }
  } else {
    Serial.printf("Unknown topic: %s, payload:\n", topic.c_str());
    Serial.println(body);
  }
}

void loop()
{
  if (digitalRead(CONFIG_PIN) == 0) {
    Serial.println("Reconfiguration requested");
    run_wifi_manager();
  }

  if (config_ok == 1337) {
    if (mqtt.connected()) {
      mqtt.loop();
    } else {
      Serial.printf("Connecting to %s:%d, %s/%s, client-id=%s\n", mqtt_host.c_str(), mqtt_port,
                    mqtt_username.c_str(), mqtt_password.c_str(),
                    mqtt_client_id.c_str());
      mqtt.setClient(wifi_client);
      mqtt.disconnect();
      mqtt.setServer(mqtt_host.c_str(), mqtt_port);
      String prefix = "bitraf/nexa/" + instance_id;
      String willTopic = prefix + "/online";
      if (mqtt.connect(mqtt_client_id.c_str(), mqtt_username.c_str(), mqtt_password.c_str(),
                       willTopic.c_str(), 0, 1, "0")) {
        Serial.printf("Connected to MQTT server, prefix=%s\n", prefix.c_str());
        mqtt.subscribe((prefix + "/cmd/#").c_str());
        mqtt.publish(willTopic.c_str(), "1");
      } else {
        Serial.printf("Connect failed: %d\n", mqtt.state());
        Serial.printf("Local IP: %s\n", WiFi.localIP().toString().c_str());
      }
    }
  }

  String line;
  static line_reader line_reader;
  if (line_reader.read(line)) {

  }
}

