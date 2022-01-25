# SD Card Root

The contents of this directory should be copied to the SD card that is connected to the ESP32.

The directory structure should be as follows:

```
/
├─ config
├─ README.md
└─ animations/
   ├─ animation1.csv
   └─ animation2.xmas
```

You should edit the `config` file to match the parameters of your tree:

- `N_LEDS`: The total number of LEDs that are being controlled. This only supports daisy-chained strips, not parallel ones with different pins.
- `MAX_CURRENT_MA`: The maximum current allowed to be provided to the LEDs. This should be set the no more than the maximum current of your power supply. This will affect the brightness.
- `DEFUALT_FPS`: The FPS of the animation using the old `.csv` format. Animations using the new `.xmas` format will use their own framerate, if present.
