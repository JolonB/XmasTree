#pragma once

#include "Arduino.h"

class AnimationFile : public Printable {
public:
  // Constant max_len value
  static const int MAX_LEN = 64;

  AnimationFile(const char *filename);
  /**
   * @brief Get the name of the animation file.
   *
   * @param filename: The name of the animation file. Max length of
   * AnimationFile::MAX_LEN.
   * @return char* The name of the animation file. Max length of
   * AnimationFile::MAX_LEN characters.
   */
  char *getFilename(void);
  /**
   * @brief Check whether the animation file is valid.
   *
   * @return true: If the animation file has either not been run yet, or is
   * valid.
   * @return false: If the animation file has been run and was not valid.
   */
  bool isCorrupt(void);
  /**
   * @brief Set the animation filename.
   *
   * @param filename The name of the animation file. Max length of
   * AnimationFile::MAX_LEN
   * @return true: If the filename was set successfully.
   * @return false: If the filename was longer than AnimationFile::MAX_LEN
   * characters.
   */
  bool setFilename(const char *filename);
  /**
   * @brief Set the animation file as corrupt/invalid.
   *
   */
  void setCorrupt(void);
  size_t printTo(Print &) const;
  /**
   * @brief Get the shortened filename, with a maximum length provided by the
   * first parameter.
   *
   * @param maxLength: The maximmum length of the returned string.
   * @param out: The string to write the shortened filename to.
   * @return char*: The value stored in out. This is provided as a convenience
   * as the value in out is exactly the same.
   */
  char *getShortFilename(const int maxLength, char *out);

private:
  char filename[MAX_LEN + 1];
  bool corrupt = false;
};
