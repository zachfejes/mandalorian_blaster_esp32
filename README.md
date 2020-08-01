# Mandalorian Blaster ESP32 Board Software
Code for my custom prop based on the blaster from The Mandalorian, based on ESP32 hardware.

This code is for my custom designed/built Mandalorian Blaster prop. I'm tracking my broader work on that personal project (including CAD, 3D printing, electronics, hardware, and painting) on the Real Prop Forum here: https://www.therpf.com/forums/threads/the-mandalorian-full-costume-project.330650/#post-5083379. Definitely check it out if you're interested!

## Key Features

Always:

* MCU must 'wake up' when safety toggle is set to high
* MCU must 'sleep' when safety toggle is set to low

If MCU is awake:

* Blaster must indicate (via LEDs on the blaster) the approximate remaining battery power
* Blaster must connect to phone via bluetooth low energy (BLE) to precisely indicate the remaining power & state
* MCU must play sound when 'trigger' temporary switch is set to high
* MCU must keep running count of the number of times trigger has been pressed since wakeup, and clear when going to sleep

I'll be flushing out the project issues and this document shortly!
