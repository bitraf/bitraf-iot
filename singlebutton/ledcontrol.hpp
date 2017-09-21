
#include <string>
#include <stdio.h>
#include <ArduinoJson.h>

namespace ledcontrol {


struct Parameters {
    Parameters() {}

    Parameters(int on)
        : valid(true)
        , onpower(on)
    {
    }

    bool valid = false;
    int onpower = 0; // [0->255]
    int offpower = 0; // [0->255]
    int period = -1; //
};

Parameters parse(const std::string &in) {
    // Boolean
    const bool on = (in == "1" || in == "true");
    const bool off = (in == "0" || in == "false");
    if (on) {
        Parameters p(255);
        return p;
    }
    if (off) {
        Parameters p(0);
        return p;
    }
    Parameters params;

    StaticJsonBuffer<200> jsonBuffer; // fails if smaller than 200
    JsonVariant root = jsonBuffer.parse(in);
    if (!root.success()) {
        return Parameters {};
    }

    // Plain number
    if (root.is<int>() ) {
        params.valid = true;
        params.onpower = root.as<int>();
        return params;
    }
    // On/off animation
    JsonObject &anim = root.as<JsonObject>();
    if (anim.containsKey("animate")) {
        params.onpower = anim["on"];
        params.offpower = anim["off"];
        params.period = anim["period"];
        params.valid = true;
        return params;
    }
    // Failed
    return Parameters {};
}

struct State {
    long nextSwitch = 0;
    bool isOn = false;
};

int next(State &s, Parameters p, long currentTime) {
    if (!p.valid) {
        return -1;
    }

    if (p.period < 0) {
        return p.onpower;
    } else {
        // on-off animation
        if (currentTime >= s.nextSwitch) {
            s.nextSwitch += p.period/2;
            s.isOn = !s.isOn;
            return s.isOn ? p.onpower : p.offpower;
        }
    }
    return -1;
}

}
