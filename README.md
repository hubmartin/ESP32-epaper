# ESP32 e-paper network display

Based on project by https://github.com/danielkucera/esp8266-epaper

This code was ported to ESP32 with these features:

- ESP32 DEVKIT V1
- WaveShare E-Paper ESP32 Driver Board
- WifiManager
- Waveshare 2.7" black-red-white 3 color e-paper
- Waveshare 4.2" black-red-white 3 color e-paper
- Waveshare 7.5" black-white e-paper

Work in progress:
- OTA
- Low power

# Known issues

- With 7.5" epaper there are some firmware issues. The code freezes after few redraws.

# Needed packages and tools

If you use 3 color display you have to install ImageMagick.
Python doesn't have install script for packages, you have to install `click` and `pyyaml`.. probably more packages :)

# Firmware build

Use platformio, before compilation/upload choose CTRL+Shift+P and `Switch project environment` to `esp32doit-devkit-v1`. The other one with `ota` doesn't work for now.

# Compile options

Using defines you set the kind of display.
```
// Uncomment single display you use
//#define EPAPER_270c
//#define EPAPER_420c
#define EPAPER_750_T7
```

If you use WaveShare E-Paper ESP32 Driver Board which has different SPI and control pins, uncomment this line.
`#define WAVESHARE_ALTERNATE_PINS`

# Usage of python tool

Use `./display.py -c <config_file>.yaml`. See the example YAML files in project directory. The YAML file defines:

- `ip` adress and network `port`
- `width`, `height` and number of `colors` of the display

### URL page load
To load a page you have to define `url:` in the YAML file. You can also try optional `url-scale` scaling option. Number 3 means that the page is screen-shotted with 3x time bigger resolution than display, then scaled down.

```
url: https://hardwario.com
url-scale: 3
```

### File image load

To load a file use `file_image: autumn.jpg`