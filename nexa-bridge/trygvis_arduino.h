#pragma once

#include <Arduino.h>
#include <type_traits>

namespace trygvis {
namespace arduino {

static
const char *nullptr_if_empty(const String& s) {
  for (size_t i = 0; i < s.length(); i++) {
    if (s[i] != ' ') {
      return s.c_str();
    }
  }

  return nullptr;
}

template<int timeout_ms>
class fixed_timer {
    long timeout;

  public:
    fixed_timer() : timeout(millis() + timeout_ms) {}
    ~fixed_timer() {}

    void reset() {
      timeout = millis() + timeout_ms;
    }

    bool expired() {
      return millis() >= timeout;
    }
};

template<int interval_ms>
class fixed_interval_timer {
    long next;

  public:
    fixed_interval_timer() : next(millis() + interval_ms) {}

    ~fixed_interval_timer() {}

    void reset() {
      next = millis() + interval_ms;
    }

    bool expired() {
      auto now = millis();
      bool e = now > next;

      if (!e) {
        return false;
      }

      do {
        next += interval_ms;
      } while (now > next);

      return true;
    }
};

template<typename T, int Depth>
class bounded_buffer {
  public:
    static constexpr auto depth = Depth;

  private:
    T values[Depth];
    int size;

  public:
    bounded_buffer() : size(0) {
    }

    bool append(const T &value) {
      if (size == Depth) {
        return false;
      }

      values[size] = value;
      size++;

      return true;
    }

    bool try_pop(T &value) {
      if (size == 0) {
        return false;
      }

      size--;
      value = values[size];
      return true;
    }

    bool empty() {
      return size == 0;
    }
};


class line_reader {
    String buf;
  public:
    line_reader() {
      buf.reserve(100);
    }

    bool read(String &line) {
      while (Serial.available()) {
        char chr = (char)Serial.read();
        if (chr == '\n' || chr == '\r') {
          if (buf.length() > 0) {
            line = buf;
            buf.remove(0);
            return true;
          }
        } else {
          buf += chr;
        }
      }

      return false;
    }
};

}; // namespace arduino
}; // namespace trygvis

namespace trygvis {
namespace arduino {
namespace test {

template<typename T>
struct converter_tool {
  static String to_string(T t) {
    return String(t);
  }
};

template<typename A, typename B>
struct assertion_traits {
  static bool eq(const A& a, const B& b) {
    return a == b;
  }
};

template<>
struct assertion_traits<const char*, String> {
  static bool eq(const char *a, const String &b) {
    // Serial.println("checking const char vs String");
    // Serial.println(strlen(a));
    // Serial.println(b.length());
    String as(a);
    // Serial.println(as);
    // Serial.println(as.length());
    // Serial.println(as.equals(b));
    // Serial.println(strcmp(a, b.c_str()));
    return as.equals(b);
  }
};
template<>
struct assertion_traits<const char *, const char *> {
  static bool eq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
  }
};

template<typename A, typename B>
void assertEquals(const A a, const B b) {
  if (!assertion_traits<A, B>::eq(a, b)) {
    String a_s = converter_tool<A>::to_string(a);
    String b_s = converter_tool<B>::to_string(b);
    Serial.printf("%s failed\na=%s\nb=%s\n",
                  __PRETTY_FUNCTION__,
                  a_s.c_str(),
                  b_s.c_str());
  }
}

static
__attribute__((used))
void assertTrue(bool x) {
  if (!x) {
    Serial.printf("failed\n");
  }
}

static
__attribute__((used))
void assertFalse(bool x) {
  if (x) {
    Serial.printf("failed\n");
  }
}

} // namespace trygvis
} // namespace arduino
} // namespace test

#ifdef ESP8266
#include <FS.h>

namespace trygvis {
namespace arduino {

class with_spiffs {
  public:
    with_spiffs() {
      if (_count == 0) {
        Serial.println("SPIFFS.begin()");
        _ok = SPIFFS.begin();
      }
      _count++;
    }

    ~with_spiffs() {
      --_count;
      if (_count == 0) {
        Serial.println("SPIFFS.end()");
        SPIFFS.end();
      }
    }

    bool ok() {
      return _ok;
    }

  private:
    static int _count;
    static bool _ok;
};

template<unsigned int interval>
static void display_memory_usage(bool force = false) {
  static fixed_interval_timer<interval> timer;
  static auto changed = true;
  static auto prev = 0, prev_display = 0;

  auto current = ESP.getFreeHeap();

  if (!changed && prev != current) {
    changed = true;
    prev = current;
  }

  if (force || (timer.expired() && changed)) {
    changed = false;

    auto pct = float(current * 100) / float(80 * 1024);
    auto change = current - prev_display;
    Serial.printf("heap change: current usage = % 5d, %.f%%, change=% 5d\n", current, pct, change);
    prev_display = current;
  }
}

}; // namespace arduino
}; // namespace trygvis

#endif // ESP8266

