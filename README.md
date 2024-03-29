# Computational Xmas Tree (with video animations!)

This project is largely based on the work by [Matt Parker of Stand-up Maths](https://github.com/standupmaths/xmastree2020), his video [*I wired my tree with 500 LED lights and calculated their 3D coordinates*](https://www.youtube.com/watch?v=TvlpIojusBE), and the subsequent work done by [the students of the 2021 Harvard GSD-6338 course](https://github.com/GSD6338/XmasTree).

This project builds upon the work from the previous work in three keys ways:

- The previous work was designed to run on a Raspberry Pi which, while relatively energy efficient, is a bit overkill for running some LEDs. This project implements the LED controller on an ESP32 using Arduino libraries. Due to the massive difference in price between an ESP32 and a Raspberry Pi, this provides a much lower cost and lower power solution than what is provided by the previous work.
- This project provides scripts which convert mp4 video files to a pixel mapping for the tree. This means you can create more animations in less time, simply by finding a video that matches your desired animation. The downside to this is that the tree is treated as a flat plane (by removing the y-axis), so perspective may be lost.
- This project also reduces the animation file size by converting the `.csv` animation files to a custom `.xmas` file which is a lightly compressed version of the original file. The `.xmas` format has other benefits too, such as having a different framerate per animation file.

This readme is a work in progress.
I will be adding rough notes as I go, and will tidy this up once I'm done.

## Usage

### Generating Animations

Before you can run any of the Python scripts, you first need to set up your virtual environment.

```shell
python3 -m venv venv
. venv/bin/activate
pip install -r requirements.txt
```

#### From Videos

TODO

### Building Controller Code

The code has been built using the Arduino libraries and IDE, so I suggest using those.
You will need to install the following libraries by downloading the linked .zip files and, in the Arduino IDE, going *Sketch > Include Library > Add ZIP Library...*

- [FastLED 3.5.0](https://github.com/FastLED/FastLED/archive/refs/tags/3.5.0.zip)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer/archive/1d46269.zip)
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP/archive/ca8ac5f.zip)

You will also need to add the following board using Tools > Board > Boards Manager...
The following link contains instructions for doing this.

- [Arduino-ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-boards-manager)

<!-- TODO move to using platformio -->

#### From Existing Animations

TODO

## Contributions

This is a one-man job... for now.
