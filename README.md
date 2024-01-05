Using an esp32 and a ws2812b led strip 144/m, bring light to a ble midi piano and make it possible to use Synthesia to help you learn piano pieces.

Using the excellent ESP32-BLE-MIDI library, it creates both a bluetooth midi client that connects to the piano and lights up the proper led using the FastLED library, but also a bluetooth midi server that when connected to by an external device, copies most signals to the piano but filters everything from Channel 0 and sends it to the led strip.

Hardware: 
I used a generic ESP32 DEVKITV1 devkit, and connected the leds through the VIN, GND, D13 pins using a dupont connector. I should probably use the internal traces of the devkit to pass throught the 5V, but it is convenient to power the device and the leds via a single USB power supply. The fact is that no more than 20 leds will ever be active at the same time, and the FastLED library has a configurable power limit.

My piano is a Thomann DP-28 Plus, not that it is an endorsment of any kind. It is still fortunate that I have a white one, and it has space inside that can fit the led strip, so the lights are internal and I don't need a diffuser.

