//
//   ch341eeprom programmer version 0.1 (Beta) 
//
//   Programming tool for the 24Cxx serial EEPROMs using the Winchiphead CH341A IC
//
//   (c) December 2011 asbokid <ballymunboy@gmail.com> 
//     
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <libusb-1.0/libusb.h> 
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <limits.h>
#include "ch341eeprom.h"

FILE *debugout, *verbout;
uint8_t *readbuf = NULL;

int main(int argc, char **argv) {
    int i, eepromsize = 0, bytesread = 0;
    uint8_t debug = FALSE, verbose = FALSE;
    struct libusb_device_handle *devHandle = NULL;
    char *filename = NULL, eepromname[12], operation = 0;
    FILE *fp;

    struct EEPROM eeprom_info;

    static char version_msg[] =    
        "ch341eeprom - an i2c EEPROM programming tool for the WCH CH341a IC\n" \
        "Version " CH341TOOLVERSION  " copyright (c) 2011 asbokid <ballymunboy@gmail.com>\n\n" \
        "This program comes with absolutely no warranty; This is free software,\n" \
        "and you are welcome to redistribute it under certain conditions:\n" \
        "GNU GPL v3 License: http://www.gnu.org/licenses/gpl.html\n";

    static char usage_msg[] = 
        "Usage:\n" \
        " -h, --help             display this text\n" \
        " -v, --verbose          verbose output\n" \
        " -d, --debug            debug output\n" \
        " -s, --size             size of EEPROM {24c01|24c02|24c04|24c08|24c16|24c32|24c64|24c128|24c256|24c512|24c1024}\n" \
        " -e, --erase            erase EEPROM (fill with 0xff)\n" \
        " -w, --write <filename> write EEPROM with image from filename\n" \
        " -r, --read  <filename> read EEPROM and save image to filename\n\n" \
        "Example: ch341eeprom -v -s 24c64 -w bootrom.bin\n";

    static struct option longopts[] = {
        {"help",    no_argument,       0, 'h'},
        {"verbose", no_argument,       0, 'v'},
        {"debug",   no_argument,       0, 'd'},
        {"erase",   no_argument,       0, 'e'},
        {"size",    required_argument, 0, 's'},
        {"read",    required_argument, 0, 'r'},
        {"write",   required_argument, 0, 'w'},
        {0, 0, 0, 0}
    };

    while (TRUE) {
        int32_t optidx = 0;
        int8_t c = getopt_long(argc,argv,"hvdes:w:r:", longopts, &optidx);
        if (c == -1)
            break;

        switch (c) {
            case 'h': fprintf(stdout, "%s\n%s", version_msg, usage_msg);
                      return 0;
            case 'v': verbose = TRUE;
                      break;
            case 'd': debug = TRUE;
                      break;
            case 's': if((eepromsize = parseEEPsize(optarg, &eeprom_info)) > 0)
                        strncpy(eepromname, optarg, 10);
                      break;
            case 'e': if(!operation)
                        operation = 'e';
                      else {
                        fprintf(stderr, "Conflicting command line options\n");
                        goto shutdown;
                      }
                      break;
            case 'r': if(!operation) {
                        operation = 'r';
                        filename = (char *) malloc(strlen(optarg)+1);
                        strcpy(filename, optarg);
                      } else {
                        fprintf(stderr, "Conflicting command line options\n");
                        goto shutdown;
                      }
                      break;
            case 'w': if(!operation) {
                        operation = 'w';
                        filename = (char *) malloc(strlen(optarg)+1);
                        strcpy(filename, optarg);
                      } else {
                        fprintf(stderr, "Conflicting command line options\n");
                        goto shutdown;
                      }  
                      break;
            default :  
            case '?': fprintf(stdout, "%s", version_msg);
                      fprintf(stderr, "%s", usage_msg);
                      goto shutdown;
        }
    }

    debugout = (debug == TRUE) ? stdout : fopen("/dev/null","w");
    verbout = (verbose == TRUE) ? stdout : fopen("/dev/null","w");
    fprintf(debugout, "Debug Enabled\n"); 

    if(!operation) {        
        fprintf(stderr, "%s\n%s", version_msg, usage_msg);
        goto shutdown;
    } 
    
    if(eepromsize <= 0) {
        fprintf(stderr, "Invalid EEPROM size\n");
        goto shutdown;
    }

    readbuf = (uint8_t *) malloc(MAX_EEPROM_SIZE);   // space to store loaded EEPROM
    if(!readbuf) {
        fprintf(stderr, "Couldnt malloc space needed for EEPROM image\n");
        goto shutdown;
    }

    if(!(devHandle = ch341configure(USB_LOCK_VENDOR, USB_LOCK_PRODUCT))) {
        fprintf(stderr, "Couldnt configure USB device with vendor ID: %04x product ID: %04x\n", USB_LOCK_VENDOR, USB_LOCK_PRODUCT);
        goto shutdown;
    }
    fprintf(verbout, "Configured USB device with vendor ID: %04x product ID: %04x\n", USB_LOCK_VENDOR, USB_LOCK_PRODUCT);

    if(ch341setstream(devHandle, CH341_I2C_STANDARD_SPEED) < 0) {
        fprintf(stderr, "Couldnt set i2c bus speed\n");
        goto shutdown;
    }
    fprintf(verbout, "Set i2c bus speed to [100kHz]\n");

    switch(operation) {
        case 'r':   // read
            memset(readbuf, 0xff, MAX_EEPROM_SIZE);

            if(ch341readEEPROM(devHandle, readbuf, eepromsize, &eeprom_info) < 0) {
                fprintf(stderr, "Couldnt read [%d] bytes from [%s] EEPROM\n", eepromsize, eepromname);
                goto shutdown;
            }
            fprintf(stdout, "Read [%d] bytes from [%s] EEPROM\n", eepromsize, eepromname);
            for(i=0;i<eepromsize;i++) {
                if(!(i%16))
                    fprintf(debugout, "\n%04x: ", i);
                fprintf(debugout, "%02x ", readbuf[i]);
            }
            fprintf(debugout, "\n");

            if(!(fp=fopen(filename, "wb"))) {
                fprintf(stderr, "Couldnt open file [%s] for writing\n", filename);
                goto shutdown;
            }

            fwrite(readbuf, 1, eepromsize, fp);
            if(ferror(fp)) {
                fprintf(stderr, "Error writing file [%s]\n", filename);
                if(fp)
                    fclose(fp);
                goto shutdown;
            }
            fclose(fp);
            fprintf(stdout, "Wrote [%d] bytes to file [%s]\n", eepromsize, filename);
            break;
        case 'w':   // write
            if(!(fp=fopen(filename, "rb"))) {
                fprintf(stderr, "Couldnt open file [%s] for reading\n", filename);
                goto shutdown;
            }
            memset(readbuf, 0xff, MAX_EEPROM_SIZE);
            bytesread = fread(readbuf, 1, MAX_EEPROM_SIZE, fp);
            if(ferror(fp)) {
                fprintf(stderr, "Error reading file [%s]\n", filename);
                if(fp)
                    fclose(fp);
                goto shutdown;
            }
            fclose(fp);
            fprintf(stdout, "Read [%d] bytes from file [%s]\n", bytesread, filename);
        
            if(bytesread < eepromsize)
                fprintf(stdout, "Padded to [%d] bytes for [%s] EEPROM\n", eepromsize, eepromname);

            if(bytesread > eepromsize)
                fprintf(stdout, "Truncated to [%d] bytes for [%s] EEPROM\n", eepromsize, eepromname);

            if(ch341writeEEPROM(devHandle, readbuf, eepromsize, &eeprom_info) < 0) {
                fprintf(stderr,"Failed to write [%d] bytes from [%s] to [%s] EEPROM\n", eepromsize, filename, eepromname);
                goto shutdown;
            }
            fprintf(stdout, "Wrote [%d] bytes to [%s] EEPROM\n", eepromsize, eepromname);
            break;
        case 'e': // erase
            memset(readbuf, 0xff, MAX_EEPROM_SIZE);
            if(ch341writeEEPROM(devHandle, readbuf, eepromsize, &eeprom_info) < 0) {
                fprintf(stderr,"Failed to erase [%d] bytes of [%s] EEPROM\n", eepromsize, eepromname);
                goto shutdown;
            }
            fprintf(stdout, "Erased [%d] bytes of [%s] EEPROM\n", eepromsize, eepromname);
            break;
        default:
            fprintf(stderr, "Unknown option\n");
            goto shutdown;
        }

shutdown:
    if(readbuf)
        free(readbuf);
    if(filename)
        free(filename);
    if(devHandle) {
        libusb_release_interface(devHandle, DEFAULT_INTERFACE);
        fprintf(debugout, "Released device interface [%d]\n", DEFAULT_INTERFACE);
        libusb_close(devHandle);
        fprintf(verbout, "Closed USB device\n");
        libusb_exit(NULL);
    }
    return 0;
}

