#include "include/led_driver.hpp"
#include "include/constant.hpp"

bool read_line_into_led(fs::File& file, CRGB* led, uint16_t num_leds) { // TODO see if you can optimise this more
    // Return false if EOF
    if (!file.available()) {
        return false;
    }

    char c;
    uint8_t led_values[CHANNEL_COUNT];

    // Read until first comma to skip the frame number
    while (file.available()) {
        c = file.read();
        if (c == ',') {
            break;
        }
    }

    // Read file until newline
    while (file.available() && c != '\n') {
        for (int channel = 0; channel < CHANNEL_COUNT; channel++) {
            led_values[channel] = file.parseInt();

            c = file.read(); // We need to know if the next character is a comma or a newline
            if (c == '\n' && channel != CHANNEL_COUNT - 1) {
                raise_error("Missing some channels at end of frame", CHANNEL_COUNT, channel+1);
            }
        }
        
        led->setRGB(led_values[0], led_values[1], led_values[2]);
        led++;
    }

    return true;
}


bool run_old_file(fs::File& file, const struct configuration& led_config) {
    // Read the number of commas in the first line
    uint16_t num_leds = 0;
    char c;
    while (file.available() && c != '\n') {
        c = file.read();
        if (c == ',') {
            num_leds++;
        }
    }

    // If there are no LEDs, raise an error
    if (num_leds == 0) {
        raise_error("No LED count found in file", 1, num_leds);
    }

    // The number of commas is equal to the number of LED channels, so we should divide by CHANNEL_COUNT
    // First, we check that the number of commas is a multiple of CHANNEL_COUNT
    if (num_leds % CHANNEL_COUNT != 0) {
        raise_error("Invalid number of LED channels. Must be a multiple of CHANNEL_COUNT", CHANNEL_COUNT, num_leds);
    }
    // Then we can divide by CHANNEL_COUNT
    num_leds /= CHANNEL_COUNT;

    // Now that we know how many LEDs there are, we can create the array
    if (num_leds != led_config.num_leds) {
        raise_error("Number of LEDs in file does not match num_leds", led_config.num_leds, num_leds);
    }

    Serial.print("Successfully opened file: ");
    Serial.println(file.name());

    // Read each line of the file and set the LED values
    unsigned long start_time = micros();
    int delay_time;
    while (read_line_into_led(file, FastLED.leds(), num_leds)) {
        FastLED.show();
        delay_time = (int)(1000. / led_config.default_fps - (micros() - start_time)/1000. + 0.5);
        delay(delay_time > 0 ? delay_time : 0);
        start_time = micros();
    }

    return true;
}
