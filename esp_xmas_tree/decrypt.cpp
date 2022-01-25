//#include <cstdio>
//#include <cstdlib>
//#include <string>
//#include <cstdint>
//#include <unistd.h>
//
//#include "include/decrpyt.hpp"
//#include "include/constant.hpp"
//
//
////using namespace std;
//
//bool parse_header(FILE* f, header* h) {
//    char* line = NULL;
//    size_t len = 0;
//    ssize_t bytes_read;
//
//    while ((bytes_read = getline(&line, &len, f)) != -1) {
//        // Print the line
//        printf("%s", line);
//        // Check if line is "---"
//        if (bytes_read >= 3 && line[0] == '-' && line[1] == '-' && line[2] == '-') {
//            break;
//        }
//
//        // Skip line if it contains nothing # TODO is this needed?
//        if (bytes_read == 0) {
//            continue;
//        }
//
//        // Replace last character with null terminator if new line
//        if (line[bytes_read - 1] == '\n') {
//            line[bytes_read - 1] = '\0';
//        }
//
//        // Ignore comment lines
//        if (line[0] == '#') {
//            continue;
//        }
//
//        // Check for "Version:x.x"
//        if (strncmp(line, "Version:", 8) == 0) {
//            char tmp[4]; // max 3 character long version (999)
//            char* version_sep;
//            int index;
//            version_sep = strchr(line, '.');
//            index = (int)(version_sep - line);
//            // Get major version
//            strncpy(tmp, line + 8, index - 8);
//            h->version_maj = atoi(tmp);
//            // Get minor version
//            h->version_min = atoi(line + index + 1);
//        }
//        // Check for "NumLeds:x"
//        else if (strncmp(line, "NumLED:", 7) == 0) {
//            h->num_leds = atoi(line + 7);
//        }
//        // Check for "Framerate:x"
//        else if (strncmp(line, "Framerate:", 10) == 0) {
//            h->framerate = atof(line + 10);
//        } else {
//            return false;
//        }
//    }
//
//    if (h->version_maj != VERSION_MAJOR || h->version_min < 0 || h->version_min > VERSION_MINOR) {
//        printf("Error: Invalid version\n");
//        return false;
//    } if (h->num_leds == 0 || h->framerate == 0) {
//        printf("Error: Missing header information\n");
//        return false;
//    } if (h->num_leds != NUM_LEDS) {
//        printf("Error: Incorrect number of LEDs\n");
//        return false;
//    }
//
//    return true;
//}
//
//bool parse_frame(FILE* f, header* h, uint8_t* frame_data) {
//    // Read file byte by byte
//    unsigned char c;
//    int i = 0;
//    // While not eof
//    while (!feof(f)) {
//        c = fgetc(f);
//        frame_data[i] = (uint8_t)c;
//        i++;
//        if (i % (CH(h->num_leds)) == 0) {
//            return true;
//        }
//    }
//    return false;
//}
//
//// TODO remove this, it is only for testing
//int main(int argc, char const *argv[])
//{
//    // Open the input file
//    FILE* inputFile = fopen("points.xmas", "r");
//
//    // Parse the file header
//    header h;
//    if (!parse_header(inputFile, &h)) {
//        printf("Error parsing header\n");
//        return 1;
//    }
//
//    // Print the header
//    printf("Version: %d.%d\n", h.version_maj, h.version_min);
//    printf("NumLeds: %d\n", h.num_leds);
//    printf("Framerate: %d\n", h.framerate);
//
//    // Parse the file contents
//    uint8_t* frame_data = (uint8_t*)malloc(CH(h.num_leds)*sizeof(uint8_t));
//    while (parse_frame(inputFile, &h, frame_data)) {
//        printf("[ ");
//        for (int i = 0; i < CH(h.num_leds); i++) {
//            printf("%u,", frame_data[i]);
//        }
//        printf(" ]\n");
//    }
//
//    // unsigned char test = 255;
//    // printf("%c\n", test);
//    // // Write test to file
//    // FILE* outputFile = fopen(output, "w");
//    // fwrite(&test, sizeof(unsigned char), 1, outputFile);
//    // fclose(outputFile);
//
//    // // Read output file
//    // FILE* outputFile2 = fopen("outfile", "r");
//    // unsigned char test2;
//    // fread(&test2, sizeof(unsigned char), 1, outputFile2);
//    // printf("%d\n", test2);
//    // fclose(outputFile2);
//
//    // Close the input file
//    fclose(inputFile);
//
//    return 0;
//}
