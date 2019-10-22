#include <ESP8266WiFi.h>
#include "PubSubClient.h"
 
const char* ssid     = "bitraf24";
const char* password = "grimbadgerassault";
 
const char* broker = "mqtt.bitraf.no";

WiFiClient wfClient;
PubSubClient psClient(wfClient);

long lastTime;

void mqttCallback(const char *topic, byte *payload, unsigned length) {
  String topicString(topic);
  if (topicString != "/public/chime") {
    Serial.println("topic is not /public/chime");
    return;
  }

  if (!(2 <= length && length <= 5)) {
    Serial.println("payload length out of range (2 ~ 5)");
    return;
  }

  payload[length] = '\0';

  if (!('1' <= payload[0] && payload[0] <= '5')) {
    Serial.println("output number out of range (1 ~ 5)");
    return;
  }

  const unsigned pins[5] = {16, 5, 4, 0, 2};
  const unsigned p0 = payload[0] - '1';
  const unsigned pin = pins[p0];

  const unsigned onTime = atoi((const char *)(&payload[1]));
  
  if (!(0 <= length && length <= 500)) {
    Serial.println("onTime out of range (0 ~ 500)");
    return;
  }

  Serial.print("output ");
  Serial.print(payload[0] - '0');
  Serial.print(" (pin ");
  Serial.print(pin);
  Serial.print(") on for ");
  Serial.print(onTime);
  Serial.print(" ms");
  Serial.println("");

  digitalWrite(pin, HIGH);
  delay(onTime);
  digitalWrite(pin, LOW);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
 
  // We start by connecting to a WiFi network
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("connecting to broker ");
  Serial.println(broker);

  psClient.setServer(broker, 1883);
  psClient.setCallback(mqttCallback);

  psClient.connect("chime");
  psClient.subscribe("/public/chime");
}

int uptime = 0;
 
void loop() {
  if (!psClient.connected()) {
    Serial.println("not connected");
  }

  psClient.loop();

  long now = millis();
  if (10000 < now - lastTime) {
    lastTime = now;
    Serial.println("publish");
    
    //psClient.publish("/public/chime/help", "listening on /public/chime. format: PT[TTT], e.g. 1100: output 1 on for 100 ms.");

    psClient.publish("/public/chime/uptime", String(uptime).c_str());
    ++uptime;
  }
}
