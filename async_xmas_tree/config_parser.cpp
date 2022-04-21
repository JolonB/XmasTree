#include "include/config_parser.h"
#include <ArduinoJson.h>

bool parse_led_config_file(fs::File& file, struct configuration& config) {
    StaticJsonDocument<256> doc;

    // Read contents of file
    file.seek(0);
    size_t file_len = file.size();
    char file_contents[file_len + 1];
    size_t bytes_read = file.readBytes(file_contents, file_len);

    // Check that the file_len and bytes_read are the same
    if (file_len != bytes_read) {
        Serial.println("Error: could not read entire config file");
        return false;
    }

    // Parse json from file_contents
    DeserializationError error = deserializeJson(doc, file_contents);
    // Check for parsing errors
    if (error) {
        Serial.print("Error: config file not valid json: ");
        Serial.println(error.c_str());
        return false;
    }

    // Copy values to config struct
    config.ip_addr = doc["ip_addr"];
    config.num_leds = doc["num_leds"];
    config.max_current = doc["max_current"];
    config.default_fps = doc["default_fps"];

    return true;
}