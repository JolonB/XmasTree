#include <FastLED.h>
#include <FS.h>
#include <SD_MMC.h>
#include "config_parser.hpp"

static CRGB* leds;

void raise_error(char const*, int, int);
bool run_old_file(fs::File&, const struct configuration&);
bool run_new_file(fs::File&, const struct configuration&);
