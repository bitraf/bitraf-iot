#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "./config.h"


WiFiClient client;


Adafruit_MQTT_Client mqtt(&client, CFG_MQTT_HOST, CFG_MQTT_SERVERPORT, CFG_MQTT_USER, CFG_MQTT_PASSWORD);
const int analogInPin = A0; 
int sensorValue = 0;


Adafruit_MQTT_Publish strom = Adafruit_MQTT_Publish(&mqtt, CFG_MQTT_USER "public/current/shopbot");

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("MQTT START"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(CFG_WIFI_SSID);

  WiFi.begin(CFG_WIFI_SSID, CFG_WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

uint32_t x=0;

void loop() {

  MQTT_connect();

  int value = readCurrent();

  // Now we can publish stuff!
  Serial.print(F("\nSending current val"));
  Serial.print(value);
  Serial.print("...");
 // if (! photocell.publish(value)) {
 //   Serial.println(F("Failed"));
 // } else {
 //   Serial.println(F("OK!"));
//  }
    //   delay(60000);  
       delay(600);  
}

int readCurrent(){
    int sensorValue = 0;
  for(int x = 0; x < 100; x++){
     sensorValue+= analogRead(analogInPin);
     delay(2);
  }
  return sensorValue /100;
}



void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
