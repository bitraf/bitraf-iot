
#include <string>
#include <stdio.h>

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
    // Plain number
    const int8_t pwm_tokens = sscanf(in.c_str(), "%d", &params.onpower);
    if (pwm_tokens == 1) {
        params.valid = true;
        return params;
    }
    // On/off animation
    const int8_t onoff_tokens = sscanf(in.c_str(), "onoff(%d,%d,%d)",
          &params.onpower, &params.offpower, &params.period);
    if (onoff_tokens == 3) {
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
