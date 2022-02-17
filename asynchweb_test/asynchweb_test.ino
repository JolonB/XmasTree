#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SD_MMC.h>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include "credentials.h"

#define ROOT_PATH "/animation/%s"

class AnimationFile : public Printable {
    public:
        AnimationFile(const char*);
        char* getFilename(void);
        bool isCorrupt(void);
        bool setFilename(const char*);
        void setCorrupt(void);
        size_t printTo(Print&) const;
        char* getShortFilename(int, char*);

    private:
        char filename[64];
        bool corrupt = false;
};

AnimationFile::AnimationFile(const char* filename) {
    setFilename(filename);
}

char* AnimationFile::getFilename(void) {
    return filename;
}

char* AnimationFile::getShortFilename(int maxLength, char* out) {
    bool toStrip = strlen(filename) > maxLength;
    int i = 0;
    int newMax = (toStrip ? maxLength - 3 : maxLength);
    while (filename[i] != '\0' && i < newMax) {
        out[i] = filename[i];
        i++;
    }
    // Add ellipsis if the filename is too long
    if (toStrip) {
        out[i++] = '.';
        out[i++] = '.';
        out[i++] = '.';
    }
    out[i] = '\0';
    return out;
}

bool AnimationFile::isCorrupt(void) {
    return corrupt;
}

bool AnimationFile::setFilename(const char* filename) {
    // Check that the filename isn't too long
    if (strlen(filename) > 64) {
        return false;
    }
    strcpy(this->filename, filename);
    return true;
}

void AnimationFile::setCorrupt(void) {
    corrupt = true;
}

size_t AnimationFile::printTo(Print& p) const {
    return p.print(filename);
}

std::map<std::string, AnimationFile> animation_files;
std::vector<AnimationFile*> queue;

// Replace with your network credentials
const char* ssid = CREDENTIALS_PRIVATE_SSID;
const char* password = CREDENTIALS_PRIVATE_PASSWORD;

// These must be the same as the values in the Javascript code
const char* DELETE_INPUT_FILE = "file";
const char* ADD_INPUT_FILE = "file";
const char* REMOVE_INPUT_FILE = "index";
const char* UPLOAD_INPUT_FILENAME = "fname";

const uint8_t SHORTNAME_LEN = 20;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Xmas Tree Control Panel</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    body {max-width:1200px;margin:0px auto;padding-bottom:25px;background-color:#f0f0f0;font-family:sans-serif}
    .row{display:flex;width:auto;margin:0 10%%}
    .col{flex:50%%;margin:5%%}
    .object{background-color:#a4a4a4;padding:10px;border-radius:10px;margin:5px 0;position:relative}
    .corruptobj{background:#d3d3d3;color:#747474}
    .clearfix::after{content:"";clear:both;display:table}
    span.fname{float:left;top:50%%;transform:translateY(-50%%);position:absolute}
    .object>button{border-radius:5px;padding:10px;margin:0;float:right}
    button.delete{background-color:#ff7961}
    button.add{background-color:#60ad5e}
    button.remove{background-color:#ffb74d}
    .gg-trash{box-sizing:border-box;position:relative;display:block;transform:scale(var(--ggs,1));width:10px;height:12px;border:2px solid transparent;box-shadow:0 0 0 2px,inset -2px 0 0,inset 2px 0 0;border-bottom-left-radius:1px;border-bottom-right-radius:1px;margin-top:4px}.gg-trash::after,.gg-trash::before{content:"";display:block;box-sizing:border-box;position:absolute}.gg-trash::after{background:currentColor;border-radius:3px;width:16px;height:2px;top:-4px;left:-5px}.gg-trash::before{width:10px;height:4px;border:2px solid;border-bottom:transparent;border-top-left-radius:2px;border-top-right-radius:2px;top:-7px;left:-2px}
  </style>
</head>
<body>
  <h2>Christmas Tree Animations</h2>
  <div class="row">
    <div class="col">
      <h2>Animation Files</h2>
      %ANIMATION_LIST_PLACEHOLDER%
    </div>
    <div class="col">
      <h2>Queue</h2>
      %ANIMATION_QUEUE_PLACEHOLDER%
    </div>
  </div>
  <div class="row">
    <input id="image-file" type="file" onchange="upload(this)" >
  </div>
<script>function deletef(filename){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/delete?file="+filename, false);
  xhr.send();
  location.reload();
}
function removef(index){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/remove?index="+index, false);
  xhr.send();
  location.reload();
}
function addf(filename){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/add?file="+filename, false);
  xhr.send();
  location.reload();
}
function upload(filelist){
  var xhr = new XMLHttpRequest();
  xhr.open("POST", "/upload?fname="+filelist.files[0].name, false);
  xhr.send(filelist.files[0]);
  location.reload();
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
    //Serial.println(var);
    String html_msg = "";
    if (var == "ANIMATION_LIST_PLACEHOLDER") {
        for (auto const& keyvalue : animation_files) {
            AnimationFile animation = keyvalue.second;
            char shortname[SHORTNAME_LEN+1];
            char* filename = animation.getFilename();
            animation.getShortFilename(SHORTNAME_LEN, shortname);
            bool corrupt = animation.isCorrupt();
            html_msg = html_msg + "<div class=\"object clearfix" + (corrupt ? " corruptobj" : "") + "\"><span class=\"fname\">" + shortname + "</span><button class=\"delete\" onclick=\"deletef('" + filename + "')\"><i class=\"gg-trash\"></i></button>" + (corrupt ? "" : "<button class=\"add\" onclick=\"addf('"+String(filename)+"')\">+</button>") + "</div>";
        }
    } else if (var == "ANIMATION_QUEUE_PLACEHOLDER") {
        for (int index = 0; index < queue.size(); index++) {
            AnimationFile *animation = queue[index];
            char shortname[SHORTNAME_LEN+1];
            char* filename = animation->getShortFilename(SHORTNAME_LEN, shortname);
            bool corrupt = animation->isCorrupt();
            Serial.println(filename);
            Serial.println(corrupt);
            html_msg = html_msg + "<div class=\"object clearfix" + (corrupt ? " corruptobj" : "") + "\"><span class=\"fname\">"+shortname+"</span><button class=\"remove\" onclick=\"removef("+index+")\">-</button></div>";
        }
    }
    return html_msg;
}

// Stolen from esp_xmas_tree.ino (changed FILE_READ to FILE_WRITE)
bool init_file(fs::FS &fs, fs::File& input_file, const char* filename) {
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

bool write_to_file(fs::File& input_file, const char* data) {
  if (!input_file) {
    Serial.write("Error: File not available\n");
    return false;
  }

  input_file.print(data);
  return true;
}

bool write_to_file(fs::File& input_file, const uint8_t* data, size_t len) {
  if (!input_file) {
    Serial.write("Error: File not available\n");
    return false;
  }

  input_file.write(data, len);
  return true;
}

void setup(){
    // Serial port for debugging purposes
    Serial.begin(115200);

    // SD card for writing/reading files
    if (!SD_MMC.begin()) {
        Serial.println("Error: SD_MMC.begin() failed");
        return;
    }

    uint8_t card_type = SD_MMC.cardType();
    if (card_type == CARD_NONE) {
        Serial.println("Error: No SD card attached");
        return;
    }

    Serial.println("Starting up...");

    AnimationFile hello = AnimationFile("hello.txt");
    AnimationFile world = AnimationFile("world.txt");
    AnimationFile corrupt = AnimationFile("corrupt.txt");
    AnimationFile alphabet = AnimationFile("abcdefghijklmnopqrstuvwxyz123456789.txt");
    animation_files.emplace(std::make_pair("hello.txt", AnimationFile("hello.txt")));
    animation_files.emplace(std::make_pair("world.txt", AnimationFile("world.txt")));
    animation_files.emplace(std::make_pair("corrupt.txt", AnimationFile("corrupt.txt")));
    animation_files.emplace(std::make_pair("abcdefghijklmnopqrstuvwxyz123456789.txt", AnimationFile("abcdefghijklmnopqrstuvwxyz123456789.txt")));
    // Set corrupt.txt to corrupt
    queue.push_back(&animation_files.at("hello.txt"));
    queue.push_back(&animation_files.at("hello.txt"));
    queue.push_back(&animation_files.at("world.txt"));
    queue.push_back(&animation_files.at("hello.txt"));
    queue.push_back(&animation_files.at("corrupt.txt"));
    animation_files.at("corrupt.txt").setCorrupt();
    
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
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });

    server.on("/delete", HTTP_GET, [] (AsyncWebServerRequest *request) {
        if (request->hasParam(DELETE_INPUT_FILE)) {
            const char* fileToDelete = (request->getParam(DELETE_INPUT_FILE)->value()).c_str();
            Serial.println(fileToDelete);
            animation_files.erase(fileToDelete);

            // Iterate over queue and remove file if filename matches fileToDelete (or if the file is corrupt)
            queue.erase(std::remove_if(queue.begin(), queue.end(), [fileToDelete](AnimationFile*& animation) {
                return (strcmp(animation->getFilename(), fileToDelete) == 0 || animation->isCorrupt());
            }), queue.end());
            
            request->send(200, "text/plain", "OK");
        }
    });

    server.on("/remove", HTTP_GET, [] (AsyncWebServerRequest *request) {
        if (request->hasParam(REMOVE_INPUT_FILE)) {
            int indexToRemove = (request->getParam(REMOVE_INPUT_FILE)->value()).toInt();
            Serial.println(indexToRemove);
            if (indexToRemove < 0) {
                // Raise an error
                request->send(400, "text/plain", "Index out of bounds");
            }
            queue.erase(queue.begin() + indexToRemove);
            
            request->send(200, "text/plain", "OK");
        }
    });

    server.on("/add", HTTP_GET, [] (AsyncWebServerRequest *request) {
        if (request->hasParam(ADD_INPUT_FILE)) {
            const char* fileToAdd = (request->getParam(ADD_INPUT_FILE)->value()).c_str();
            Serial.println(fileToAdd);
            queue.push_back(&animation_files.at(fileToAdd));
            request->send(200, "text/plain", "OK");
        }
    });

    server.on("/upload", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, [] (AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        // Get the filename
        const char *upload_filename = request->getParam(UPLOAD_INPUT_FILENAME)->value().c_str();
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

}
