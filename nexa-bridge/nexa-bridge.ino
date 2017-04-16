#include "Tx433_Nexa.h"
#include "trygvis_arduino.h"
#include "bitraf_iot.h"

#include <WiFiManager.h> // Version 0.12.0
#include <PubSubClient.h>
#include <Wire.h>

using namespace trygvis::arduino;

String ch_nexa = "1010";

// Configuration parameters. These are written to the config file
static String mqtt_host = "mqtt.bitraf.no";
static int mqtt_port = 1883;
static String mqtt_username;
static String mqtt_password;
static String mqtt_topic_prefix = "bitraf";
static String instance_name;
static int radio_rx_pin = D0;
static int radio_tx_pin = D3;
static int config_pin = D3;
int config_magic;

bool save_config = false;

WiFiManager wifiManager;
WiFiClient wifi_client;
PubSubClient mqtt;

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

  mqtt_topic_prefix = f.readStringUntil('\n');
  Serial.println(mqtt_topic_prefix);

  instance_name = f.readStringUntil('\n');
  Serial.println(instance_name);

  radio_rx_pin = f.readStringUntil('\n').toInt();
  Serial.println(radio_tx_pin);

  radio_tx_pin = f.readStringUntil('\n').toInt();
  Serial.println(radio_rx_pin);

  config_magic = f.readStringUntil('\n').toInt();
  Serial.println(config_magic);
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
  f.print(mqtt_topic_prefix);
  f.print('\n');
  f.print(instance_name);
  f.print('\n');
  f.print(radio_rx_pin);
  f.print('\n');
  f.print(radio_tx_pin);
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
  Serial.println("\n\nBitraf IOT");

  mqtt.setCallback(mqttCallback);

  pinMode(config_pin, INPUT);

  load_config();

  if (instance_name.length() == 0) {
    instance_name = bitraf_iot::generate_instance_name();
  }

  mqtt.setClient(wifi_client);

  if (config_magic != 1337) {
    run_wifi_manager();
  } else {
    WiFiManager wifiManager;
    wifiManager.autoConnect();
  }

}

void save_config_callback()
{
  save_config = true;
}

void run_wifi_manager()
{
  String configuration_ssid = "bitraf-iot-" + instance_name;

  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(save_config_callback);

  WiFiManagerParameter custom_mqtt_host("mqtt_host", "MQTT host", mqtt_host.c_str(), 40);
  wifiManager.addParameter(&custom_mqtt_host);

  WiFiManagerParameter custom_mqtt_username("mqtt_username", "MQTT Username", mqtt_username.c_str(), 40);
  wifiManager.addParameter(&custom_mqtt_username);

  WiFiManagerParameter custom_mqtt_password("mqtt_password", "MQTT Password", mqtt_password.c_str(), 40);
  wifiManager.addParameter(&custom_mqtt_password);

  WiFiManagerParameter custom_mqtt_topic_prefix("mqtt_topic_prefix", "MQTT Topic Prefix", mqtt_topic_prefix.c_str(), 40);
  wifiManager.addParameter(&custom_mqtt_topic_prefix);

  WiFiManagerParameter custom_radio_rx_pin("radio_rx_pin", "Radio RX pin", String(radio_rx_pin).c_str(), 3);
  wifiManager.addParameter(&custom_radio_rx_pin);

  WiFiManagerParameter custom_instance_name("instance_name", "Instance name", instance_name.c_str(), 40);
  wifiManager.addParameter(&custom_instance_name);

  WiFiManagerParameter custom_radio_tx_pin("radio_tx_pin", "Radio TX pin", String(radio_tx_pin).c_str(), 3);
  wifiManager.addParameter(&custom_radio_tx_pin);

  if (!wifiManager.startConfigPortal(configuration_ssid.c_str())) {
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  if (save_config) {
    mqtt_host = custom_mqtt_host.getValue();
    mqtt_username = custom_mqtt_username.getValue();
    mqtt_password = custom_mqtt_password.getValue();
    mqtt_topic_prefix = custom_mqtt_topic_prefix.getValue();
    instance_name = custom_instance_name.getValue();
    config_magic = 1337;
    write_config();
  }
}

void mqttCallback(const char* t, byte* payload, unsigned int length)
{
  String topic(t);
  String body;
  for (unsigned int i = 0; i < length; i++) {
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
    for (unsigned int i = 0; i < part2.length(); i++) {
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

    Tx433_Nexa nexa(radio_tx_pin, id, ch_nexa);
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

static void mqtt_loop() {
  if (config_magic != 1337) {
    return;
  }

  static fixed_interval_timer<1000> connect_timer;

  if (mqtt.connected()) {
    mqtt.loop();
    connect_timer.reset();
  }

  if (connect_timer.expired()) {
    const char *u = nullptr_if_empty(mqtt_username);
    const char *p = nullptr_if_empty(mqtt_password);
    const char *client_id = instance_name.c_str();
    Serial.printf("Connecting to %s:%d, %s/%s, client-id=%s\n", mqtt_host.c_str(), mqtt_port,
                  u ? u : "not set", p ? p : "not set",
                  client_id ? client_id : "not set");
    mqtt.setClient(wifi_client);
    mqtt.disconnect();
    mqtt.setServer(mqtt_host.c_str(), mqtt_port);
    String prefix = mqtt_topic_prefix + "/nexa/" + instance_name;
    String online_topic = prefix + "/online";
    if (mqtt.connect(client_id, u, p,
                     online_topic.c_str(), 0, 1, "0")) {
      Serial.printf("Connected to MQTT server, prefix=%s\n", prefix.c_str());
      mqtt.publish(online_topic.c_str(), "1", true);
      mqtt.subscribe((prefix + "/cmd/#").c_str());
    } else {
      Serial.printf("Connect failed: %d\n", mqtt.state());
      Serial.printf("Local IP: %s\n", WiFi.localIP().toString().c_str());
    }
  }
}

void loop()
{
  if (digitalRead(config_pin) == 0) {
    Serial.println("Reconfiguration requested");
    run_wifi_manager();
  }

  mqtt_loop();

  String line;
  static line_reader line_reader;
  if (line_reader.read(line)) {

  }
}

