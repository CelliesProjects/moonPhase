#include <inttypes.h>

#include "moonPhase.h"

template<typename T, typename T2>
inline T map(T2 val, T2 in_min, T2 in_max, T out_min, T out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float moonPhase::_fhour(const struct tm &timeinfo) {
    return timeinfo.tm_hour + map((timeinfo.tm_min * 60) + timeinfo.tm_sec, 0, 3600, 0.0f, 1.0f);
}

static float _Julian(int32_t year, int32_t month, const float &day) {
    int32_t b, c, e;
    b = 0;
    if (month < 3) {
        year--;
        month += 12;
    }
    if (year > 1582 || (year == 1582 && month > 10) ||
        (year == 1582 && month == 10 && day > 15)) {
        int32_t a;
        a = year / 100;
        b = 2 - a + a / 4;
    }
    c = 365.25f * year;
    e = 30.6001f * (month + 1);
    return b + c + e + day + 1720994.5f;
}

static float _sun_position(const float &j) {
    float n, x, e, l, dl, v;
    int32_t i;
    n = 360 / 365.2422f * j;
    i = n / 360;
    n = n - i * 360.0f;
    x = n - 3.762863f;
    x += (x < 0) ? 360 : 0;
    x *= DEG_TO_RAD_F32;
    e = x;
    do {
        dl = e - .016718f * sin(e) - x;
        e = e - dl / (1 - .016718f * cos(e));
    } while (fabs(dl) >= 1e-12f);
    v = 360 / PI * atan(1.01686011182f * tan(e / 2));
    l = v + 282.596403f;
    i = l / 360;
    l = l - i * 360.0f;
    return l;
}

static float _moon_position(const float &j, const float &ls) {
    float ms, l, mm, ev, sms, ae, ec;
    int32_t i;
    ms = 0.985647332099f * j - 3.762863f;
    ms += (ms < 0) ? 360.0f : 0;
    l = 13.176396f * j + 64.975464f;
    i = l / 360;
    l = l - i * 360.0f;
    l += (l < 0) ? 360 : 0;
    mm = l - 0.1114041f * j - 349.383063f;
    i = mm / 360;
    mm -= i * 360.0f;
    ev = 1.2739f * sin((2 * (l - ls) - mm) * DEG_TO_RAD_F32);
    sms = sin(ms * DEG_TO_RAD_F32);
    ae = 0.1858f * sms;
    mm += ev - ae - 0.37f * sms;
    ec = 6.2886f * sin(mm * DEG_TO_RAD_F32);
    l += ev + ec - ae + 0.214f * sin(2 * mm * DEG_TO_RAD_F32);
    l = 0.6583f * sin(2 * (l - ls) * DEG_TO_RAD_F32) + l;
    return l;
}

moonData_t moonPhase::_getPhase(const int32_t year, const int32_t month, const int32_t day, const float &hour) {
    /*
      Calculates the phase of the moon at the given epoch.
      returns the moon phase angle as an int (0-360)
      returns the moon percentage that is lit as a real number (0-1)
    */
    const float j {_Julian(year, month, (float)day + hour / 24.0f) - 2444238.5f};
    const float ls {_sun_position(j)};
    const float lm {_moon_position(j, ls)};
    float angle = lm - ls;
    angle += (angle < 0) ? 360 : 0;
    const moonData_t returnValue {
        (int32_t)angle,
        (1 - cos((lm - ls) * DEG_TO_RAD_F32)) / 2
    };
    return returnValue;
}
