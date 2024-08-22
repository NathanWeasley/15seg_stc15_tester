# Test program for 15seg WhiteBar

## What is 15seg WhiteBar?

15seg WhiteBar is an LED display module which contains 8 15-segment LED display unit, capable of displaying numbers and letters. The LEDs are white, hence the name "WhiteBar"

## Design considerations

Primary considerations when building this module is as follows:
1. Modularized design, can be shared between projects.
2. Can be cascaded to support different display factors.
3. Constant current driver to ensure even brightness.
4. Compact interface that minimizes GPIO requirements.
5. Easy protocol so hardware transmitters can apply to reduce software complexity.

## Design details

The driver circuit is divided into the segment driver part and the digit driver part.

### Segment driver

Since these 15seg unit are common-anode, I chose DM13A for segment driver to sink currents from all segments.

DM13A is a dedicated 16-channel constant-current LED driver with integrated 16-bit shift register and latch, and current setting can be adjusted by a single external resistor. Since DM13A is a CC driver, no current-limiting resistors are needed. And 15 segments of the unit fits into the 16 channels of DM13A.

### Digit driver

Digit driver pushes current into the anodes of each digit. They are controlled by PNP transistors, which are then controlled by a 74HC595. The 15seg WhiteBar has 8 digits so a single 74HC595 is just enough.

Since the hardware interfaces of DM13A and 74HC595 are very similar, they are internally cascaded, forming a combined segment-digit protocol as follows:

MSB --> [N/A S14 S13 ... S2 S1 S0 | D7 D6 D5 ... D2 D1 D0] <-- LSB

At any times, D[7..0] can only have 1 activated signal.

## Why use STC15F104E for the driver?

SPI interfacing is simple enough, indeed, however still requires at least an MCU to make it work.

The idea is to use something small and cheap enough to act as an interface chip, allowing for an even easier protocol, like serial port, so any computer can happily talk to the module through USB-Serial adapters. No MCU programming needed.

Besides that STC15F104E has only 8 pins so they're the smallest MCU I have, of course there is another reason which is **I FIND GETTING AS MUCH AS POSSIBLE OUT OF THESE OUTDATED MCUs REALLY SATISFYING!**

STC15F104E is an 8-bit 8051-compatible(according to STC) MCU in 8-pin package, providing 6 IO ports. It's got internal clock source up to 35MHz, 4KB(104E) flash and 128B RAM. This MCU **DOESN'T HAVE A SERIAL PORT**, so in order to commence serial communication, we have to emulate one.

Since emulating a full-duplex serial port is a pain in the ass, this code only implements an Rx-only serial port, which leaves a reasonable CPU time to display scanning.