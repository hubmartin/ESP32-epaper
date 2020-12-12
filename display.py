#!/usr/bin/python3

# virtualenv tutorial https://click.palletsprojects.com/en/7.x/quickstart/

import click
import yaml
import subprocess


import socket
import time
import array
import datetime

import os, sys
from PIL import Image
from io import BytesIO

IP = "192.168.1.7"
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


url="https://data.pocasi-data.cz//static/html/meteogram-v2.html#x=84&y=407"


# Not proud, not efficient, working ok
def rgb_to_3c(img):
    img.save('imagemagick_in.png')
    command = 'convert imagemagick_in.png -dither FloydSteinberg -define dither:diffusion-amount=80%% -remap eink-3color.png -type truecolor imagemagick_out.png'
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    process.wait()
    img_load = Image.open('imagemagick_out.png')
    return img_load.convert('RGB')

def drv_webpage(url):
    
    scale = 1
    if data.get('url-scale'):
       scale = data['url-scale']

    capture_w = int(data['width'] * scale)
    capture_h = int(data['height'] * scale)

    click.echo("Scale %s, capturew %d, capuiterh %s" % (scale, capture_w, capture_h))

    chrome_options = webdriver.chrome.options.Options()
    chrome_options.add_argument("--headless")
    chrome_options.add_argument("--window-size=%s,%s" % (capture_w, capture_h))

    DRIVER = 'chromedriver'
    driver = webdriver.Chrome(DRIVER, options=chrome_options)
    driver.set_window_size(capture_w, capture_h)
    driver.get(url)
    screenshot = driver.get_screenshot_as_png()
    driver.quit()

    img = Image.open(BytesIO(screenshot))

    if scale != 1:
        img = img.resize((data['width'], data['height']))

    return rgb_to_3c(img)
    #return img.convert('1').convert("RGB")


def drv_file_image(file_name):
    img = Image.open(file_name)  
    img = img.resize((data['width'], data['height']))
    return rgb_to_3c(img)

def drv_file_image_3c(file_name):
    command = 'convert autumn.jpg -resize %dx%d\!  -dither FloydSteinberg -define dither:diffusion-amount=80%% -remap eink-3color.png -type truecolor output.bmp' % (data['width'], data['height'])
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    process.wait()
    img = Image.open("output.bmp")  
    return img

@click.command()
@click.option('-c', '--config', default="", help='YAML configuration file.')
def main(config):
    """Stream images to your ESP WiFi displays"""
    global data

    try:
        with open(config) as f:
            data = yaml.load(f, Loader=yaml.FullLoader)
            print(data)
    except:
        click.echo('File %s not found!' % config)
        return
  

    # chrome_options = webdriver.chrome.options.Options()
    # chrome_options.add_argument("--headless")
    # chrome_options.add_argument("--window-size=%s,%s" % (data['width'], data['height']))

    # DRIVER = 'chromedriver'
    # driver = webdriver.Chrome(DRIVER, options=chrome_options)
    # #driver.set_window_size(512, 384)
    # driver.get(url)
    # screenshot = driver.get_screenshot_as_png()
    # driver.quit()

    # screenshot = Image.open(BytesIO(screenshot))

    if data.get('url'):
        screenshot = drv_webpage(data['url'])
    elif data.get('file_image'):
        screenshot = drv_file_image(data['file_image'])
    elif data.get('file_image_3c'):
        screenshot = drv_file_image_3c(data['file_image_3c'])

    screenshot.save("out.png")

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP

    server_address = (data['ip'], data['port'])
    print('connecting to %s port %s' % server_address)
    sock.connect(server_address)
    print("connected")

    for y in range(0, data['height']):
        for x in range(0, data['width']):
            pixel = screenshot.getpixel((x, y))

            # if y == 0 and x < 23:
            #     click.echo(pixel)

            # (47, 36, 42) // black
            # (216, 37, 40) // red
            # (242, 244, 239) //white

            b = b"\x01" #black

            if pixel[0] > 100 and pixel[1] < 100 and pixel[2] < 100:
                b = b"\x02" #red
            elif pixel[0] > 100 and pixel[1] > 100 and pixel[2] > 100:
                b = b"\x00" #white
                
            sock.send(b)
            #print(x,y);

    sock.close()
    

if __name__ == '__main__':
    main()
