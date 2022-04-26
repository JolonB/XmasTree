#pragma once

#include <Arduino.h>

// Include the minified webui html file
const char index_html[] PROGMEM =
#include "webui.html.h"
    ;

// Replaces placeholder with button section in your web page
String processor(const String &var);
