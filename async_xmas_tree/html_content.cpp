#include "include/html_content.h"
#include "include/async_xmas_tree.h"

// Replaces placeholder with button section in your web page
String processor(const String &var) {
  // Serial.println(var);
  String html_msg = "";
  // if (var == "ANIMATION_LIST_PLACEHOLDER") {
  //   for (auto const &keyvalue : animation_files) {
  //     AnimationFile animation = keyvalue.second;
  //     char shortname[SHORTNAME_LEN + 1];
  //     char *filename = animation.getFilename();
  //     animation.getShortFilename(SHORTNAME_LEN, shortname);
  //     bool corrupt = animation.isCorrupt();
  //     html_msg = html_msg + "<div class=\"object clearfix" +
  //                (corrupt ? " corruptobj" : "") + "\">" +
  //                "<abbr class=\"fname\" title=\"" + filename + "\">" +
  //                shortname + "</abbr>" +
  //                "<button class=\"delete\" onclick=\"deletef('" + filename +
  //                "')\"><i class=\"gg-trash\"></i></button>" +
  //                (corrupt ? ""
  //                         : "<button class=\"add\" onclick=\"addf('" +
  //                               String(filename) + "')\">+</button>") +
  //                "</div>";
  //   }
  // } else if (var == "ANIMATION_QUEUE_PLACEHOLDER") {
  //   for (int index = 0; index < queue.size(); index++) {
  //     AnimationFile *animation = queue[index];
  //     char shortname[SHORTNAME_LEN + 1];
  //     char *filename = animation->getFilename();
  //     animation->getShortFilename(SHORTNAME_LEN, shortname);
  //     bool corrupt = animation->isCorrupt();
  //     Serial.println(filename);
  //     Serial.println(corrupt);
  //     html_msg = html_msg + "<div class=\"object clearfix" +
  //                (corrupt ? " corruptobj" : "") + "\">" +
  //                "<abbr class=\"fname\" title=\"" + filename + "\">" +
  //                (index == active_index ? "&#9656; " : "") + shortname +
  //                "</abbr>" + "<button class=\"remove\" onclick=\"removef(" +
  //                index + ")\">&#8722;</button>" +
  //                "<button class=\"move\" onclick=\"movef(" + index +
  //                ",+1)\">&#9662;</button>" +
  //                "<button class=\"move\" onclick=\"movef(" + index +
  //                ",-1)\">&#9652;</button>" + "</div>";
  //   }
  // }
  return html_msg;
}
