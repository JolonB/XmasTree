#include "include/config_parser.hpp"

bool parse_led_config_file(fs::File& file, struct configuration& config) {
    // Read file of the form:
    // N_LEDS,MAX_CURRENT_MA,COLOR_ORDER,DEFAULT_FPS,LED_PIN
    // n1,n2,n3,n4,n5

    // Read number of commas until the first \n
    int parameters = 0;
    char c;
    bool characters_read = false;
    while (file.available()) {
        c = file.read();
        if (c == ',') {
            parameters++;
        }
        if (c == '\n') {
            break;
        }
        characters_read = true;
    }
    parameters++; // Add one for the last parameter

    // Check that there is at least 1 parameter
    if (!characters_read) {
        Serial.println("No parameters read in file");
        return false;
    }

    // Read parameters into config struct
    if (parameters >= 1) {
        config.num_leds = (uint16_t)file.parseInt();
    }
    if (parameters >= 2) {
        config.max_current = (uint16_t)file.parseInt();
    }
    if (parameters >= 3) {
        config.default_fps = (float)file.parseFloat();
    }

    Serial.println("Parsed config file");
    Serial.print("Num LEDs: ");
    Serial.println(config.num_leds);
    Serial.print("Max current: ");
    Serial.println(config.max_current);
    Serial.print("Default FPS: ");
    Serial.println(config.default_fps);
    // Add more parameters if needed

    return true;
}