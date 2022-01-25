#include <FS.h>
#include <FastLED.h>

struct configuration {
    int num_leds;
    uint16_t max_current = 500; // in mA
    float default_fps = 20;
};

bool parse_led_config_file(fs::File&, struct configuration&);