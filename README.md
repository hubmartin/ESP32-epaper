# ESP32 e-paper network display

Based on project by https://github.com/danielkucera/esp8266-epaper

This code was ported to ESP32 with these features:

- ESP32 DEVKIT V1
- WifiManager
- Waveshare 4.2" black-red-white 3 color e-paper

Work in progress:
- OTA
- Low power


# Firmware build

Use platformio, before compilation/upload choose CTRL+Shift+P and `Switch project environment` to `esp32doit-devkit-v1`. The other one with `ota` doesn't work for now.

# Usage

Use `./display.py` file to load a webpage, create a PNG image and send it to the device IP.
The script is using data from czech webserver for Czech Republic. You'll probably need to completely
rewrite this script or find other weaher service which fits all data nicely on single small screen.