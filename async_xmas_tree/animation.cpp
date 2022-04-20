#include "include/animation.h"

char *AnimationFile::getShortFilename(const int maxLength, char *out) {
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

bool AnimationFile::setFilename(const char *filename) {
  // Check that the filename isn't too long
  if (strlen(filename) > 64) {
    return false;
  }
  strcpy(this->filename, filename);
  return true;
}

AnimationFile::AnimationFile(const char *filename) { setFilename(filename); }

char *AnimationFile::getFilename(void) { return filename; }

bool AnimationFile::isCorrupt(void) { return corrupt; }

void AnimationFile::setCorrupt(void) { corrupt = true; }

size_t AnimationFile::printTo(Print &p) const { return p.print(filename); }
