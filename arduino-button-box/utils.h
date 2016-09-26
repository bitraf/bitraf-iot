#pragma once

template<int _interval_ms>
class fixed_interval_timer {
  public:
    const int interval_ms = _interval_ms;
    void reset() {
      next = millis() + _interval_ms;
    }

    bool expired() {
      auto now = millis();
      bool e = now > next;

      if (!e) {
        return false;
      }

      do {
        next += _interval_ms;
      } while (now > next);

      return true;
    }

  private:
    long next;
};

template<unsigned debounce_timeout_ms, int _pin>
class PinPoller {
    bool old_state, _state, _changed;
    unsigned long change_ts;

  public:
    const int pin = _pin;

    PinPoller() : old_state(digitalRead(_pin)), _changed(false), change_ts(millis()) {
    }

    void loop() {
      auto now = millis();
      bool new_state = digitalRead(_pin);
      _changed = false;

      if (new_state != old_state) {
        change_ts = now;
      }
      old_state = new_state;

      // Serial.printf("now=%ul, change_ts=%ul, diff=%ul, new_state=%d, state=%d\r\n", now, change_ts, now - change_ts, new_state, _state);
      if ((now - change_ts) > debounce_timeout_ms) {
        if (new_state != _state) {
          _state = new_state;
          _changed = true;
        }
      }
    }

    bool state() {
      return _state;
    }

    bool changed() {
      return _changed;
    }
};

