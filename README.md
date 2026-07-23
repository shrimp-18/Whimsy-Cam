# Whimsy-Cam

WhimsyCam is a small and lightweight keychain camera that lets you capture everyday moments without the distractions of a phone. Clip it to your keys, wear it anywhere, and preserve life's little adventures as they happen!

---



<img width="420" height="595" alt="A5 - 1" src="https://github.com/user-attachments/assets/da4e179a-858f-4fa9-84b7-981f21dc4146" />


**FEATURES** :
+ 2 MEGAPIXEL CAMERA
+ 10FPS FULL COLOUR DISPLAY
+ USB-C CHARGING
+ LONG BATTERY LIFE
+ IN-BUILT FLASH
+ KEYCHAIN/CHARM LANYARD HOLE
+ STURDY CASE
+ EASY SOFTWARE UPDATES
  


## How to Use:
Whimsy Cam is supa simple to use! Just charge it every once in a while, turn it on and click photos! It comes with s 64gb SD-Card, that means LOTSSSS of photos. It is designed to let you attach cute lanyards and charms to it :3 Keep checking this repository often! If there are updates, you can simply update the firmware of your camera too without touching the hardware or opening anything up. 

<img width="382" height="301" alt="image" src="https://github.com/user-attachments/assets/40e89dd7-2e2d-4456-8b0e-a2cf792bf7da" />
<img width="371" height="287" alt="image" src="https://github.com/user-attachments/assets/f1b7b203-121b-414b-a6df-6bb51cc79565" />

---

## Why I made this:
I love photography a lot! And with the recent rise in trend of the kodak keychain disposable camera, I wanted to make something of my own but more durable and long lasting. Whimsy Cam will cost you about 4.5k INR to make, but if you play your cards right(like I did) and use an SD card with lesser storage capacity, you could make it for almost half the price! Which is so much cheaper than whats commercially available, and so much versatile. On the same hardware, you have endless possibilities. In the future I plan to add filters for your photos and improved image quality! And you dont even need to de-assemble your camera to update to the latest software :D, just plug a USB C and hit upload!

---

## How the Camera works:
The Camera is divided into 2 main parts: The Camera Module and the Display Module.
1. The Camera Module:
   The Camera Module is made up of the ESP32 Camera Module with a GC2145 Sensor and an ESP32 Cam MB.It is a TCP(Transmission Control Protocol) Server. It establishes a        WiFi Server and send the captured about 8-10 frames per second to the TCP Client (the display module). When you click a photo, transmission stops for a tiny moment to      capture a high resolution photo and save it as a BMP file to the SD Card. This module gets power from 5V boosted LiPo Battery.
   
3. The Display Module:
   The Display Module is made up of the ESP 32 S3 N1R8 (dev board), a GC9A01 Circual TFT Screen and a tactile button. The ESP 32 S3 is a TCP Client, It recieves frames and    displays them on the screen. When the button is pressed it sends a command to click a photo.This module also gets 5V power from LiPo Battery (after boosting)

The Power Management: 

<img width="472" height="174" alt="image" src="https://github.com/user-attachments/assets/f6ea8ee2-c470-4618-ba23-10975e208178" />

This wonderful diagram shows the power management of the Camera.
A re-chargable LiPo battery that outputs 3.7V is used. Since the ESP 32 modules need strictly either 3.3V or 5V, I used a TPS61023 Boost Converter to convert 3.7V to a stable 5V. There is a slide switch in between to turn the power on and off. The battery can also be recharged using the T4056 Module, I'm using a Type-C variant. This doesn't really have an over-charge or over-discharge protection yet :( But I'll think about it in future. Since the camera hardware cannot be changed so easily, I'll atleast add a battery level monitor on the camera display to at least protect it from over-discharge. And the green light from the T4056 can be your cue to stop charging hence acting as an over-charge protection.

---

## The Wiring Diagram:
<img width="842" height="595" alt="wiringdiagram" src="https://github.com/user-attachments/assets/b3c9e809-e075-4ed3-8ee0-27049ba9535d" />



## The Case:
<img width="288" height="232" alt="image" src="https://github.com/user-attachments/assets/276ffcf9-bbc6-4d8c-a9e7-72f751e52b58" />
<img width="311" height="230" alt="image" src="https://github.com/user-attachments/assets/8055d82b-7b98-462a-88c4-9b8754cfdad0" />

----
## Fusion File:
https://a360.co/4wc3e6t


## How to Build:
So did I finally convince you to make your own :3 <br>
1.Procure all components listed in the BOM.<br>

2.Solder Pins to the Display Module.<br>

3.Get Jumper Wireeeess (you'll need 10) and a metre of normal wire. Copy the wiring diagram and connect everything. Try to use the colour-coded wires like I did. Its much easier to understand and remember!<br>

4.Download the code from the firmware folder inside src in this repository. Connect the ESP32 S3 and the Camera module to your computer. Download the Arduino IDE.Compile and upload the code to the respective boards! Now your camera should work!<br>

5.If youre using an SD Card, make sure its formatted to FAT and not ExFAT. Most SD cards by default will be formatted to ExFAT so you need to format them again using your laptop before you plug them into this camera for use. Otherwise your camera wont save photos :( <br>

6.The Fun Part! 3D-print the CAD Files(Front and Back of Case) given in this repository and fix the components in their respective places. Close the case shut! It is a snap fit case. <br>

7.Make cute lanyards and beaded chains to put on the camera so you can carry it around and be a cool kid. Cool because you have a keychain camera and because YOU MADE IT!<br>

YAY DAS IT ITS DONE
<img width="842" height="595" alt="idea" src="https://github.com/user-attachments/assets/dab57349-09af-4757-9641-f3efe7d35a97" />


---

## Bill of Materials

| S.No. | Product Name | Note | Qty | Cost (INR) | Link |
|-------|--------------|------|-----|------------|------|
| 1 | ESP32 S3 N16R8 | | 1 | 1249 | https://robocraze.com/products/7semi-esp32-s3-dev-boardc-1-n8r8-wifi-bluetooth-dual-usb-c-rgb-led |
| 2 | TPS61023 | | 1 | 109 | https://robocraze.com/products/tps61023-3-7a-5v-output-mini-boost-converter-breakout-board-7semi |
| 3 | TP4056 | | 1 | 19 | https://robocraze.com/products/tp4056-battery-charger-c-type-module-with-protection-1 |
| 4 | LiPo Battery | | 1 | 102 | https://robocraze.com/products/witty-fox-160mah-rechargeable-3-7v-lipo-battery |
| 5 | Slide Switch | | 1 | 16 | https://robocraze.com/products/slide-switch-3-pin-2-way-spdt |
| 6 | Tactile Button | 6x6mm | 1 | 29 | https://robocraze.com/products/4-pins-dip-momentary-square-tactile-push-button-switch-10-pieces-6x6x5mm |
| 7 | GC9A01 TFT Screen | | 1 | 422 | https://www.xcluma.com/1-28-inch-round-screen-tft-module-240x240-hd-ips-full |
| 8 | ESP32 Cam Module | GC2145 Sensor | 1 | 699 | https://quartzcomponents.com/products/esp32-camera-development-board-wifi-bluetooth-with-ov2640-camera-module |
| 9 | ESP32 Cam MB | | 1 | 82 | https://quartzcomponents.com/products/esp32-cam-mb-micro-usb-programmer-module-for-esp32-cam-development-board |
| 10 | SD Card | 64GB | 1 | 1605 | https://quartzcomponents.com/products/sandisk-ultra-64gb-microsdxc-class-10-memory-card-140mb-s |
| 11 | 3D Case - Back | | 1 | 95 | https://zbotic.in/product/online-3d-printing-service/ |
| 12 | 3D Case - Front | | 1 | 93 | https://zbotic.in/product/online-3d-printing-service/ |

**Total: 12 items — ₹4520 (~$47 USD)**

