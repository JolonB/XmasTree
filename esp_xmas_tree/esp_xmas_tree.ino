#include "include/led_driver.hpp"
#include "include/decrpyt.hpp"
#include "include/constant.hpp"

const char* animation_dir = "/animation";
const char* animation_seq_file = "sequence";
const char* config_file = "/config";

header* head;
char filename_buffer[64];

void setup() {
  Serial.begin(115200);

  // Initialise SD card
  if (!SD_MMC.begin()) {
    Serial.println("Failed to init SD card");
    return;
  }

  // Use card type to ensure there is a card attached
  uint8_t card_type = SD_MMC.cardType();
  if (card_type == CARD_NONE) {
    Serial.println("No SD_MMC card attached");
    return;
  }
  
  Serial.println("Successfully initialised SD card");

  // Read the config file
  fs::File file;
  if (!init_file(SD_MMC, file, config_file)) {
    Serial.println("Failed to open config file");
    return;
  }
  struct configuration led_config;
  if (!parse_led_config_file(file, led_config)) {
    Serial.println("Failed to parse config file");
    return;
  }
  file.close();

  leds = new CRGB[led_config.num_leds];

  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, led_config.num_leds).setCorrection(TypicalSMD5050); // was TypicalLEDStrip
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, led_config.max_current);

  main_loop(SD_MMC, led_config);

  return;
}

void main_loop(fs::FS &filesystem, const struct configuration& led_config) {
  fs::File file;

  get_full_path("rgb_coords.csv", filename_buffer);
  if (!init_file(SD_MMC, file, filename_buffer)) {
    raise_error("Failed to read file", 0, 1);
    return;
  }
  
  // If the file starts with "FRAME_ID" we run the basic processing
  // If the file starts with "---" we run the new processing
  char first_char = (char)file.read();
  if (first_char == 'F') {
    run_old_file(file, led_config);
  } else if (first_char == '-') {
    run_new_file(file, led_config);
  } else {
    Serial.print("Invalid file header: ");
    Serial.println(filename_buffer);
  }

  file.close();

  return;
}

void get_full_path(const char* file_name, char* full_path) {
  // Join animation_dir and file_name
  strcpy(full_path, animation_dir);
  if (full_path[strlen(full_path) - 1] != '/') {
    strcat(full_path, "/");
  }
  strcat(full_path, file_name);
}

bool init_file(fs::FS &fs, fs::File& input_file, const char* filename) {
  // Close the file
  if (input_file) {
    input_file.close();
  }

  input_file = fs.open(filename, FILE_READ);

  if (!input_file) {
    Serial.write("Error: Could not open file \"");
    Serial.write(filename);
    Serial.write("\"\n");
    return false;
  }

  return true;
}

void raise_error(const char* error_str, int expected_val, int actual_val) {
  Serial.print("ERROR: ");
  Serial.println(error_str);
  Serial.println("Expected: " + String(expected_val));
  Serial.println("Actual: " + String(actual_val));
  Serial.println("Exiting...");
  // Set the first LED red to indicate an error
  FastLED.clear();
  FastLED.leds()->setRGB(255, 0, 0);
  FastLED.show();

  exit(1);
}


void loop() { /* Do nothing */ }
