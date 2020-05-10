# rpi4-pthread-blink
A simple command line program to toggle multiple GPIO pin states on the raspberry pi 4 at pseudo-random intervals using libgpiod and pthread. It uses the standard 14 GPIO pins as valid input choices (see https://pinout.xyz/).

### Requirements/Prerequisites:
You will need a raspberry pi 4 in order to run this example without modification (I am running raspbian on mine and accessing it via ssh). You will also need to install and configure the necessary build tools and libraries (out of scope for this README). The application can then be built using the included Makefile.
