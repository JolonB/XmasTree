#include <FastLED.h>

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

#define NUM_LEDS 20
#define MAX_CURRENT_LIMIT 500
#define COLOR_ORDER RGB // short strip = RGB, long strip = GRB
#define DEFAULT_FPS 20
#define LED_PIN     16
#define BRIGHTNESS  6
#define LED_TYPE    WS2812

#define CHANNEL_COUNT 3
/* Convert an LED to the number of channels it has */
#define CH(x) ((x)*CHANNEL_COUNT)

// static uint8_t COLOR_ORDER[2] = {RGB, BGR}; // TODO use this with configuration.txt
// TODO maybe do the same with LED_TYPE
