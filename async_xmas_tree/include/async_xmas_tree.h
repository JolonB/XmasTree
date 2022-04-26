#pragma once

#include "animation.h"
#include <map>
#include <vector>

const char SHORTNAME_LEN = 20;

extern std::map<std::string, AnimationFile> animation_files;
extern std::vector<AnimationFile *> queue;

extern size_t active_index;
