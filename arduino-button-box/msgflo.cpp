#include "msgflo.h"

#include <vector>

namespace msgflo {
namespace pubsub {

static
void printMqttState(int state) {
  switch (state) {
    case MQTT_CONNECTION_TIMEOUT:
      Serial.println("Disconnected: connection timeout");
      break;
    case MQTT_CONNECTION_LOST:
      Serial.println("Disconnected: connection lost");
      break;
    case MQTT_CONNECT_FAILED:
      Serial.println("Disconnected: connect failed");
      break;
    case MQTT_DISCONNECTED:
      Serial.println("Disconnected: disconnected");
      break;
    case MQTT_CONNECTED:
      Serial.println("Disconnected: connected");
      break;
    case MQTT_CONNECT_BAD_PROTOCOL:
      Serial.println("Disconnected: bad protocol");
      break;
    case MQTT_CONNECT_BAD_CLIENT_ID:
      Serial.println("Disconnected: bad client id");
      break;
    case MQTT_CONNECT_UNAVAILABLE:
      Serial.println("Disconnected: unavailable");
      break;
    case MQTT_CONNECT_BAD_CREDENTIALS:
      Serial.println("Disconnected: bad credentials");
      break;
    case MQTT_CONNECT_UNAUTHORIZED:
      Serial.println("Disconnected: unauthorized");
      break;
  }
}

static void globalCallback(const char* topic, byte* payload, unsigned int length);

class Publisher {
  public:
    virtual void send(const String &queue, const String &payload) = 0;
};

struct PubSubOutPort : public OutPort {
  public:
    const String id;
    const String type;
    const String queue;
    Publisher *publisher;

    PubSubOutPort(const String &id, const String &type, const String &queue, Publisher *publisher) :
      id(id), type(type), queue(queue), publisher(publisher) {
    }

    void toJson(String &s) const {
      s += "{\"id\": \"";
      s += id;
      s += "\", \"type\": \"";
      s += type;
      s += "\", \"queue\": \"";
      s += queue;
      s += "\"}";
    }

    void send(const String &payload) {
      publisher->send(queue, payload);
    }
};

static
const char *discoveryTopic = "fbp";

class PubSubClientEngine : public Engine, public Publisher {

    const String component;
    const String label;
    const String icon;

    PubSubClient *mqtt;
    const char *clientId;

    std::vector<PubSubOutPort> outPorts;
  public:
    void send(const String &queue, const String &payload) {
      mqtt->publish(queue.c_str(), payload.c_str());
    }

    PubSubClientEngine(
      const String &component, const String &label, const String &icon,
      PubSubClient *mqtt, const char *clientId): component(component), label(label), icon(icon), mqtt(mqtt), clientId(clientId) {
      mqtt->setCallback(&globalCallback);
    }

    OutPort* addOutPort(const String &id, const String &type, const String &queue) {
      outPorts.emplace_back(id, type, queue, this);
      return &outPorts[outPorts.size() - 1];
    }

    void callback(const char* topic, byte* payload, unsigned int length) {
    }

    void onConnected() {
    // fbp {"protocol":"discovery","command":"participant","payload":{"component":"dlock13/DoorLock","label":"Open the door","icon":"lightbulb-o","inports":[{"queue":"/bitraf/door/boxy4/open","type":"object","id":"open"}],"outports":[],"role":"boxy4","id":"boxy4"}}
      String discoveryMessage =
        "{"
        "\"protocol\": \"discovery\","
        "\"command\": \"participant\","
        "\"payload\": {\"component\": \"";
      discoveryMessage += component;
      discoveryMessage += "\", \"label\": \"";
      discoveryMessage += label;
      discoveryMessage += "\", \"icon\": \"";
      discoveryMessage += icon;
      discoveryMessage += "\", \"outports\": [";
      for (auto &p : outPorts) {
        p.toJson(discoveryMessage);
      }
      discoveryMessage += "]}}";
      Serial.println("discoveryMessage");
      Serial.println(discoveryMessage);
      mqtt->publish(discoveryTopic, discoveryMessage.c_str(), false);
    }

    void loop() {
      mqtt->loop();

      if (!mqtt->connected()) {
        int state = mqtt->state();

        Serial.print("Connecting...");
        // This is blocking
        if (mqtt->connect(clientId)) {
          Serial.println("ok");
          onConnected();
        } else {
          Serial.println("failed");
          printMqttState(mqtt->state());
        }
      }
    }
};

uint8_t instanceBytes[sizeof(PubSubClientEngine)];
static PubSubClientEngine *instance = nullptr;

static void globalCallback(const char* topic, byte* payload, unsigned int length) {
  instance->callback(topic, payload, length);
}

Engine *createPubSubClientEngine(
  const String &component,
  const String &label,
  const String &icon,
  PubSubClient* mqtt, const char *clientId) {
  if (instance != nullptr) {
    Serial.println("Double initialization of msgflo engine.");
    return instance;
  }

  instance = new (instanceBytes) PubSubClientEngine(component, label, icon, mqtt, clientId);
  return instance;
}

};
};

