#include "credentials.h"
#include "include/animation.h"
#include "include/async_xmas_tree.h"
#include "include/config_parser.h"
#include "include/html_content.h"
#include "include/sdfile.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SD_MMC.h>
#include <WiFi.h>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <utility>
#include <vector>

#define ROOT_DIR "/animations"
#define ROOT_PATH ROOT_DIR "/%s"

std::map<std::string, AnimationFile> animation_files;
std::vector<AnimationFile *> queue;

size_t active_index = 1;
std::vector<int> playback_queue;
enum Ordering { IN_ORDER = 0, SHUFFLE = 1, RANDOM = 2 };
Ordering sequence = IN_ORDER;

bool running_p = false; // false=pause, true=play

// Replace with your network credentials
const char *ssid = CREDENTIALS_PRIVATE_SSID;
const char *password = CREDENTIALS_PRIVATE_PASSWORD;

// These must be the same as the values in the Javascript code
const char *DELETE_INPUT_FILE = "file";
const char *ADD_INPUT_FILE = "file";
const char *REMOVE_INPUT_FILE = "index";
const char *MOVE_INPUT_FILE = "index";
const char *MOVE_SHIFT = "shift";
const char *SHIFT_VALUE = "by";
const char *UPLOAD_INPUT_FILENAME = "fname";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Stolen from esp_xmas_tree.ino (changed FILE_READ to FILE_WRITE)
bool init_file(fs::FS &fs, fs::File &input_file, const char *filename) {
  // Close the file
  if (input_file) {
    input_file.close();
  }

  input_file = fs.open(filename, FILE_WRITE);

  if (!input_file) {
    Serial.write("Error: Could not open file \"");
    Serial.write(filename);
    Serial.write("\"\n");
    return false;
  }

  return true;
}

bool write_to_file(fs::File &input_file, const char *data) {
  if (!input_file) {
    Serial.write("Error: File not available\n");
    return false;
  }

  input_file.print(data);
  return true;
}

bool write_to_file(fs::File &input_file, const uint8_t *data, size_t len) {
  if (!input_file) {
    Serial.write("Error: File not available\n");
    return false;
  }

  input_file.write(data, len);
  return true;
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialise random number generator for random/shuffle function
  srand(time(NULL));

  Serial.println("Just started");
  active_index = 1; // TODO move this elsewhere

  // Initialise the SD card
  if (sdfile::init_sd_card() == -1) {
    return;
  }

  Serial.println("Starting up...");

  // Read filenames from SD card in /animations directory
  fs::File dir;
  if (!sdfile::open_directory(ROOT_DIR, dir)) {
    return;
  }

  // Read each filename in dir
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    if (!entry.isDirectory()) {
      const char *filepath = entry.name();
      AnimationFile animation(filepath);
      animation_files.emplace(
          std::make_pair(animation.getFilename(), animation));
    }
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  Serial.println("Checking wifi status");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/add", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(ADD_INPUT_FILE)) {
      const char *fileToAdd =
          (request->getParam(ADD_INPUT_FILE)->value()).c_str();
      queue.push_back(&animation_files.at(fileToAdd));
      request->send(200, "text/plain", "OK");
    }
  });

  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(DELETE_INPUT_FILE)) {
      const char *fileToDelete =
          (request->getParam(DELETE_INPUT_FILE)->value()).c_str();
      animation_files.erase(fileToDelete);

      // Iterate over queue and remove file if filename matches fileToDelete (or
      // if the file is corrupt)
      queue.erase(std::remove_if(queue.begin(), queue.end(),
                                 [fileToDelete](AnimationFile *&animation) {
                                   return (strcmp(animation->getFilename(),
                                                  fileToDelete) == 0 ||
                                           animation->isCorrupt());
                                 }),
                  queue.end());

      request->send(200, "text/plain", "OK");
    }
  });

  server.on("/remove", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(REMOVE_INPUT_FILE)) {
      int indexToRemove =
          (request->getParam(REMOVE_INPUT_FILE)->value()).toInt();
      Serial.println(indexToRemove);
      if (indexToRemove < 0) {
        // Raise an error
        request->send(400, "text/plain", "Index out of bounds");
      }
      queue.erase(queue.begin() + indexToRemove);

      request->send(200, "text/plain", "OK");
    }
  });

  server.on("/move", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(MOVE_INPUT_FILE)) {
      // Get the index to move
      int indexToMove = (request->getParam(MOVE_INPUT_FILE)->value()).toInt();
      // Get the number of indices to shift it by
      int shift = (request->getParam(MOVE_SHIFT)->value()).toInt();
      // Get the size of the vector, so we know when to wrap around
      int queueSize = queue.size();
      // Get the new index, accounting for the wrap around
      int newIndex =
          ((indexToMove + shift) % queueSize + queueSize) % queueSize;

      // Get the start and end iterators
      auto iter = queue.begin();
      auto fromIndex = iter + indexToMove;
      auto toIndex = iter + newIndex;

      // Move the element: https://stackoverflow.com/a/28846892/6366927
      if (fromIndex < toIndex)
        std::rotate(fromIndex, fromIndex + 1, toIndex + 1);
      else if (fromIndex > toIndex)
        std::rotate(toIndex, fromIndex, fromIndex + 1);

      request->send(200, "text/plain", "OK");
    }
  });

  server.on("/shift", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(SHIFT_VALUE)) {
      int shiftBy = (request->getParam(SHIFT_VALUE)->value()).toInt();

      // Get current size of queue to determine which index to go to
      size_t queue_size = queue.size();
      if (sequence == IN_ORDER || sequence == SHUFFLE) {
        if (shiftBy > 0) {
          active_index++;
          // Wrap around to start
          if (active_index >= queue_size)
            active_index = 0;
        } else if (shiftBy < 0) {
          active_index--;
          // Wrap around to end
          if (active_index < 0)
            active_index = queue_size - 1;
        } else {
          Serial.println("Invalid shift when choosing next/prev animation");
          exit(0);
        }
      } else if (sequence == RANDOM) {
        // Generate a random number equal to 1 less than the queue size,
        // so we don't play the same animation twice
        if (queue_size > 1) {
          size_t tmp_index = rand() % (queue_size - 1);
          // Shift indices once we pass the active index
          if (tmp_index >= active_index)
            tmp_index++;
          // Set the active index to the random index
          active_index = tmp_index;
        } else if (queue_size == 1)
          active_index = 0;
        else
          active_index = -1;

      } else {
        Serial.println("`sequence` value is invalid");
        exit(0);
      }

      request->send(200, "text/plain", "OK");
    }
  });

  server.on("/togglepp", HTTP_GET, [](AsyncWebServerRequest *request) {
    running_p = !running_p;

    request->send(200, "text/plain", "OK");
  }); // TODO maybe have a GET for running status, which can be called whenever
      // the page refreshes

  server.on("/status/play", HTTP_GET, [](AsyncWebServerRequest *request) {
    char msg[2]; // running_p is bool, so is only ever 0 or 1
    sprintf(msg, "%d", running_p);
    request->send(200, "text/plain", msg);
  });

  server.on(
      "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         size_t index, size_t total) {
        // Get the filename
        const char *upload_filename =
            request->getParam(UPLOAD_INPUT_FILENAME)->value().c_str();
        // Open file
        char filepath[256];
        sprintf(filepath, ROOT_PATH, upload_filename);
        fs::File file;
        if (index == 0)
          file = SD_MMC.open(filepath, FILE_WRITE);
        else
          file = SD_MMC.open(filepath, FILE_APPEND);

        if (!file) {
          Serial.println("Error: Could not open file for writing");
          return;
        }

        // Write data to file
        write_to_file(file, data, len);

        if (index + len == total)
          Serial.println("Done uploading");

        // Close file
        file.close();
      });

  // Start server
  server.begin();
}

void loop() {
  // Serial.println("hello");
  // delay(1000);
}
