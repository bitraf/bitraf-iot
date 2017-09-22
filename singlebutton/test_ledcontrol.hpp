
#include <iostream>

#include "./ledcontrol.hpp"

#include <assert.h>

int main() {

    const ledcontrol::Parameters initial;
    assert(initial.valid == false);

    const auto on = ledcontrol::parse("true");
    assert(on.valid == true);
    assert(on.onpower == 255);
    assert(on.period == -1);

    const auto off = ledcontrol::parse("false");
    assert(off.valid == true);
    assert(off.onpower == 0);
    assert(off.period == -1);

    const auto pwm0 = ledcontrol::parse("0");
    assert(pwm0.valid == true);
    assert(pwm0.onpower == 0);
    assert(pwm0.period == -1);

    const auto pwm = ledcontrol::parse("40");
    assert(pwm.valid == true);
    assert(pwm.onpower == 40);
    assert(pwm.period == -1);

    const auto pwm255 = ledcontrol::parse("255");
    assert(pwm255.valid == true);
    assert(pwm255.onpower == 255);
    assert(pwm255.period == -1);

    const auto animfull = ledcontrol::parse("{ \"animate\": \"onoff\", \"on\": 255.0, \"off\": 1, \"period\": 300.0 }");
    assert(animfull.valid == true);
    assert(animfull.onpower == 255);
    assert(animfull.offpower == 1);
    assert(animfull.period == 300);

    std::cout << "All tests passed" << std::endl;
}
