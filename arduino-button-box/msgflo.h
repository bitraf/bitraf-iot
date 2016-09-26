#pragma once

#include <Arduino.h>

namespace msgflo {

class OutPort {
    virtual void send(const String &payload) = 0;
};

class Engine {
  protected:
    virtual ~Engine() {};
  public:
    virtual OutPort* addOutPort(const String &id, const String &type, const String &queue) = 0;

    virtual void loop() = 0;
};

}; // namespace msgflo

#include <PubSubClient.h>

namespace msgflo {
namespace pubsub {

Engine* createPubSubClientEngine(
  const String &component,
  const String &label,
  const String &icon,
  PubSubClient *client, const char *clientId);

}; // namespace pubsub
}; // namespace msgflo

