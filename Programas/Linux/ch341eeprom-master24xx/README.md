ch341eeprom
===

A libusb based programming tool for 24Cxx serial EEPROMs using the WinChipHead CH341A IC

![WinChipHead CH341A based programmer](https://raw.github.com/commandtab/ch341eeprom/master/pics/programmer.jpg)

**Requirements**

* llvm and make (via Xcode or Xcode Command Line Tools)
* libusb (perhaps via [Homebrew](http://mxcl.github.io/homebrew/): `brew install libusb`)

**Compiling**

After installing the necessary tools and libusb, building `ch341eeprom` is as simple as running the included Makefile:

```
make
```

**Usage**

Using `ch341eeprom` is straightforward:

```
Usage:
 -h, --help             display this text
 -v, --verbose          verbose output
 -d, --debug            debug output
 -s, --size             size of EEPROM {24c01|24c02|24c04|24c08|24c16|24c32|24c64|24c128|24c256|24c512|24c1024}
 -e, --erase            erase EEPROM (fill with 0xff)
 -w, --write <filename> write EEPROM with image from filename
 -r, --read  <filename> read EEPROM and save image to filename
```

For example:

```
$ ./ch341eeprom -v -s 24c64 -e

Searching USB buses for WCH CH341a i2c EEPROM programmer [1a86:5512]
Found [1a86:5512] as device [7] on USB bus [2]
Opened device [1a86:5512]
Claimed device interface [0]
Device reported its revision [3.03]
Configured USB device
Set i2c bus speed to [100kHz]
Erased [8192] bytes of [24c64] EEPROM
Closed USB device
```

**Author**

Originally written by [asbokid](http://sourceforge.net/projects/ch341eepromtool/) and released under the terms of the GNU GPL, version 3, or later. Modifications by [commandtab](https://github.com/commandtab) to make it work under OS X.

**License**

This is free software: you can redistribute it and/or modify it under the terms of
the latest GNU General Public License as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.


**Tested**

| Name              | Write | Read |
|-------------------|-------|------|
| CAT24M01WI        |  OK   |  OK  |
| AT24C256          |  OK   |  OK  |
| Microchip 24LC256 |  OK   |  OK  |
| AT24C32N          |  OK   |  OK  |
| AT24C08           |  OK   |  OK  |

