Ch341Prog
============
A simple command line tool(programmer) interfacing with ch341a under Linux OS and macOS.

Description
------------
There are a lot cheap SPI/IIC programmers based on ch341a, which can be bought
from ebay for as low as $3.  However, there was no Linux based tools associated
with the hardware, and the best I can find is a tool called `ch341eepromtool'
(http://sourceforge.net/projects/ch341eepromtool/) which can only deal with
IIC EEPROMs.  So I decide to write my own.

After reading the source for ch341eeprom and sniffering the usb traffic,
I thinks I have fully understood the usb protocol. So, here is what I have
got so far.

Special thanks to asbokid, the ch341eepromtool's author.

License
------------
This is free software: you can redistribute it and/or modify it under
the terms of the latest GNU General Public License as published by the
Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.

Speed Test
-----------
 * read a 8M flash(W25Q64) costs 101.840 seconds
 * write to an 8M flash(W25Q64) costs 158.811 seconds

Linux Permissions
------------------
To ensure the CH341 programmer will be set up with proper permissions on a Linux computer, run the following command to install a udev rule file into /etc/udev/rules.d:

`sudo make install-udev-rule`
