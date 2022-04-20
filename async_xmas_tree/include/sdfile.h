#pragma once

#include <FS.h>
#include <SD_MMC.h>

namespace sdfile {
/**
 * @brief Initialise the SD card.
 * 
 * @return uint8_t: -1 if the card could not be initialised. SD card type otherwise.
 */
uint8_t init_sd_card();

/**
 * @brief 
 * 
 * @param directory_name 
 * @param directory 
 * @return true 
 * @return false 
 */
bool open_directory(const char *directory_name, fs::File &directory);

} // namespace sdfile