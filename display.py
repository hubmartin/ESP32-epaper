#!/usr/bin/python3

import socket
import time
import array
import datetime

import os, sys
from PIL import Image
from io import BytesIO

IP = "esp-epaper.local"
PORT = 3333

from selenium import webdriver

#$ adb logcat | grep pocasi-data
#- waiting for device -
#10-24 19:43:46.322 27582 27582 D b.a.a.b.a$a: [main] https://data.pocasi-data.cz/data/pocasi/v1/bulletin/dalsi4dny.json - 200
#10-24 19:43:47.004 27582 27637 D b.a.a.b.a$a: [DefaultDispatcher-worker-1] https://data.pocasi-data.cz/data/pocasi/v1/aladin/2020/10/24/12/178/171/prvni2dny.json - 200
#10-24 19:43:47.192 27582 27714 D b.a.a.b.a$a: [DefaultDispatcher-worker-4] https://data.pocasi-data.cz/data/pocasi/v1/aladin/2020/10/24/12/178/171/prvni2dny.json - 200
#10-24 19:43:47.332 27582 27637 D b.a.a.a.b.a.a.a: [DefaultDispatcher-worker-1] https://data.pocasi-data.cz/data/pocasi/v1/aladin/2020/10/24/12/178/171/dnes.json
#10-24 19:43:47.342 27582 27638 D b.a.a.b.a$a: [DefaultDispatcher-worker-2] https://data.pocasi-data.cz/data/pocasi/v1/aladin/2020/10/24/12/178/171/prvni2dny.json - 200
#10-24 19:43:47.436 27582 27639 D b.a.a.b.a$a: [DefaultDispatcher-worker-3] https://data.pocasi-data.cz/data/pocasi/v1/aladin/2020/10/24/12/178/171/dnes.json - 200
#10-24 19:43:56.864 27582 27582 D b.a.a.f.b$c: [main] https://data.pocasi-data.cz//static/html/srazky/radar/mapa-v5.html
#10-24 19:43:59.476 27582 27582 D b.a.a.f.b$c: [main] https://data.pocasi-data.cz//static/html/aladin/mapa-v5.html#typ=srazky
#10-24 19:44:02.442 27582 27582 D b.a.a.f.b$c: [main] https://data.pocasi-data.cz//static/html/vystrahy/mapa-v5.html#orps=
#10-24 19:44:18.637 27582 27582 D b.a.a.f.b$c: [main] https://data.pocasi-data.cz//static/html/meteogram-v2.html#x=178&y=171


url="https://data.pocasi-data.cz//static/html/meteogram-v2.html#x=178&y=171"

chrome_options = webdriver.chrome.options.Options()
chrome_options.add_argument("--headless")
chrome_options.add_argument("--window-size=400,300")
#chrome_options.add_argument("--window-size=512,384")

DRIVER = 'chromedriver'
driver = webdriver.Chrome(DRIVER, options=chrome_options)
#driver.set_window_size(512, 384)
driver.get(url)
screenshot = driver.get_screenshot_as_png()
driver.quit()

screenshot = Image.open(BytesIO(screenshot))

target = screenshot \
    .convert(mode="L") \
    .resize((400, 300))

target.save("out.png")

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP

server_address = (IP, PORT)
print('connecting to %s port %s' % server_address)
sock.connect(server_address)
print("connected")

for y in range(0,300):
    for x in range(0,400):
        pixel = target.getpixel((x, y))
        b = b"\x00"
        if pixel < 250:
            b = b"\x01"
        sock.send(b)

sock.close()
