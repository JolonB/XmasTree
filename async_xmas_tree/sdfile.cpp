#include "include/sdfile.h"

namespace sdfile {
uint8_t init_sd_card() {
  if (!SD_MMC.begin()) {
    Serial.println("Error: Failed to init SD card");
    return -1;
  }

  uint8_t card_type = SD_MMC.cardType();
  if (card_type == CARD_NONE) {
    Serial.println("Error: No SD_MMC card attached");
    return -1;
  }

  return card_type;
}

bool open_directory(const char *directory_name, fs::File &directory) {
  int attempts = 0, max_attempts = 5;

  // Attempt to open the directory max_attempts times
  while (attempts < max_attempts && !directory) {
    directory = SD_MMC.open(directory_name, FILE_READ);
    if (directory) {
      break;
    } else {
      Serial.println("Warning: Unable to open directory. Creating directory...");
      // Attempt to create the directory if it doesn't exist
      if (!SD_MMC.mkdir(directory_name)) {
        Serial.println("Error: Failed to create directory");
        return false;
      }
    }
    attempts++;
  }

  // Indicate failure if we couldn't open or create the directory
  if (attempts == max_attempts) {
    Serial.println("Error: Failed to open directory");
    return false;
  }

  // Print directory information
  Serial.println("Directory information:");
  Serial.print("  Name: ");
  Serial.println(directory.name());
  Serial.print("  Size: ");
  Serial.println(directory.size());
  Serial.print("  Type: ");
  Serial.println(directory.isDirectory() ? "Directory" : "File");

  // Check the the File is a directory
  if (!directory.isDirectory()) {
    Serial.println("Error: Not a directory");
    return false;
  }

  return true;
}
} // namespace sdfile