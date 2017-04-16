#pragma once

#include "esp8266_peri.h"
#include <Arduino.h>
#include <array>
#include "bitraf_iot_words.h"

namespace bitraf_iot {

int get_random(int from_inclusive, int to_exclusive) {
  uint32_t r = RANDOM_REG32;
  Serial.printf("r=%u\n", r);
  return from_inclusive + r % (to_exclusive - from_inclusive);
}

template<typename T, size_t N>
const T& pick_random(const T(&data)[N]) {
  return data[get_random(0, N)];
}

String generate_instance_name() {
  String s;
  s.reserve(50);
  s += pick_random(adjectives);
  s += "-";
  s += pick_random(nouns);
  return s;
}

} // namespace bitraf_iot

