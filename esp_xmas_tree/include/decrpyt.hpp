#include <cstdint>
#include <cstdio>

struct header {
    uint8_t version_maj;
    uint8_t version_min;
    uint16_t num_leds;
    uint16_t framerate;
};

bool parse_header(FILE*, header*);
bool parse_frame(FILE*, header*, uint8_t*);
