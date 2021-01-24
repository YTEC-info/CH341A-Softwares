/*
 * This file is part of the ch341prog project.
 *
 * Copyright (C) 2014 Pluto Yang (yangyj.ee@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * verbose functionality forked from https://github.com/vSlipenchuk/ch341prog/commit/5afb03fe27b54dbcc88f6584417971d045dd8dab
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "ch341a.h"
#include <time.h>
#include <stdio.h>

int verbose;

void v_print(int mode, int len) { // mode: begin=0, progress = 1
	if (!verbose) return ;

	static unsigned int size = 0;
	static time_t started,reported;
	unsigned int dur,done;
	time_t now;
	time(&now);

	switch (mode) {
		case 0: // setup
			size = len;
			started = reported = now;
			break;
		case 1: // progress
			if (now == started ) return ;

			dur = now - started;
			done = size-len;
			if (done > 0 && reported != now) {
				printf("Bytes: %d (%d%c),  Time: %d, ETA: %d   \r",done,
						(done * 100) / size, '%', dur, (int) ((1.0 * dur * size) / done-dur));
				fflush(stdout);
				reported = now;
			}
			break;
		case 2: // done
			dur = now - started; if (dur<1) dur=1;
			printf("Total:  %d sec,  average speed  %d  bytes per second.\n", dur, size / dur);
			break;
		default:
			break;
	}
}

int main(int argc, char* argv[])
{
    int32_t ret;
    uint8_t *buf;
    FILE *fp;
    char *filename;
    int cap;
    int length = 0;
    char op = 0;
    uint32_t speed = CH341A_STM_I2C_20K;
    int8_t c;
    int offset = 0;

    const char usage[] =
        "\nUsage:\n"\
        " -h, --help             display this message\n"\
        " -i, --info             read the chip ID info\n"\
        " -e, --erase            erase the entire chip\n"\
        " -v, --verbose          print verbose info\n"\
        " -l, --length <bytes>   manually set length\n"\
        " -w, --write <filename> write chip with data from filename\n"\
        " -o, --offset <bytes>   write data starting from specific offset\n"\
        " -r, --read <filename>  read chip and save data to filename\n"\
        " -t, --turbo            increase the i2c bus speed (-tt to use much faster speed)\n"\
        " -d, --double           double the spi bus speed\n";
    const struct option options[] = {
        {"help",    no_argument,        0, 'h'},
        {"info",    no_argument,        0, 'i'},
        {"erase",   no_argument,        0, 'e'},
        {"write",   required_argument,  0, 'w'},
        {"length",  required_argument,  0, 'l'},
        {"verbose", no_argument,        0, 'v'},
        {"write",   required_argument,  0, 'w'},
        {"offset",  required_argument,  0, 'o'},
        {"read",    required_argument,  0, 'r'},
        {"turbo",   no_argument,        0, 't'},
        {"double",  no_argument,        0, 'd'},
        {0, 0, 0, 0}};

        int32_t optidx = 0;

        while ((c = getopt_long(argc, argv, "hiew:r:l:tdvo:", options, &optidx)) != -1){
            switch (c) {
                case 'i':
                case 'e':
                    if (!op)
                        op = c;
                    else
                        op = 'x';
                    break;
                case 'v':
                    verbose = 1;
                    break;
                case 'w':
                case 'r':
                    if (!op) {
                        op = c;
                        filename = (char*) malloc(strlen(optarg) + 1);
                        strcpy(filename, optarg);
                    } else
                        op = 'x';
                    break;
                case 'l':
                    length = atoi(optarg);
                    break;
                case 't':
                    if ((speed & 3) < 3) {
                        speed++;
                    }
                    break;
                case 'd':
                    speed |= CH341A_STM_SPI_DBL;
                    break;
                case 'o':
                    offset = atoi(optarg);
                    break;
                default:
                    printf("%s\n", usage);
                    return 0;
            }
    }
    if (op == 0) {
        fprintf(stderr, "%s\n", usage);
        return 0;
    }
    if (op == 'x') {
        fprintf(stderr, "Conflicting options, only one option at a time.\n");
        return -1;
    }
    ret = ch341Configure(CH341A_USB_VENDOR, CH341A_USB_PRODUCT);
    if (ret < 0)
        return -1;
    ret = ch341SetStream(speed);
    if (ret < 0) goto out;
    ret = ch341SpiCapacity();
    if (ret < 0) goto out;
    cap = 1 << ret;
    printf("Chip capacity is %d bytes\n", cap);

    if (length != 0){
        cap = length;
    }
    if (op == 'i') goto out;
    if (op == 'e') {
        uint8_t timeout = 0;
        ret = ch341EraseChip();
        if (ret < 0) goto out;
        do {
            sleep(1);
            ret = ch341ReadStatus();
            if (ret < 0) goto out;
            printf(".");
            fflush(stdout);
            timeout++;
            if (timeout == 100) break;
        } while(ret != 0);
        if (timeout == 100)
            fprintf(stderr, "Chip erase timeout.\n");
        else
            printf("Chip erase done!\n");
    }
    if ((op == 'r') || (op == 'w')) {
        buf = (uint8_t *)malloc(cap);
        if (!buf) {
            fprintf(stderr, "Malloc failed for read buffer.\n");
            goto out;
        }
    }
    if (op == 'r') {
        ret = ch341SpiRead(buf, offset, cap);
        if (ret < 0)
            goto out;
        fp = fopen(filename, "wb");
        if (!fp) {
            fprintf(stderr, "Couldn't open file %s for writing.\n", filename);
            goto out;
        }
        fwrite(buf, 1, cap, fp);
        if (ferror(fp))
            fprintf(stderr, "Error writing file [%s]\n", filename);
        fclose(fp);
    }
    if (op == 'w') {
        fp = fopen(filename, "rb");
        if (!fp) {
            fprintf(stderr, "Couldn't open file %s for reading.\n", filename);
            goto out;
        }
        ret = fread(buf, 1, cap, fp);
        if (ferror(fp)) {
            fprintf(stderr, "Error reading file [%s]\n", filename);
            if (fp)
                fclose(fp);
            goto out;
        }
        cap = ret;
        fprintf(stderr, "File Size is [%d]\n", ret);
        ret = ch341SpiWrite(buf, offset, ret);
        if (ret == 0) {
            printf("\nWrite ok! Try to verify... ");
            FILE *test_file;
            char *test_filename;
            test_filename = (char*) malloc(strlen("./test-firmware.bin") + 1);
            strcpy(test_filename, "./test-firmware.bin");

            ret = ch341SpiRead(buf, offset, cap);
            test_file = fopen(test_filename, "w+b");

            if (!test_file) {
                fprintf(stderr, "Couldn't open file %s for writing.\n", test_filename);
                goto out;
            }
            fwrite(buf, 1, cap, test_file);

            if (ferror(test_file))
                fprintf(stderr, "Error writing file [%s]\n", test_filename);

            fseek(fp, 0, SEEK_SET);
            fseek(test_file, 0, SEEK_SET);

            int ch1, ch2;
            int checked_count = 0;
            ch1 = getc(fp);
            ch2 = getc(test_file);

            while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
                ch1 = getc(fp);
                ch2 = getc(test_file);
                checked_count++;
            }

            if (ch1 == ch2 || (checked_count == cap))
                printf("\nWrite completed successfully. \n");
            else
                printf("\nError while writing. Check your device. May be it need to be erased.\n");

            if (remove(test_filename) == 0)
                printf("\nAll done. \n");
            else
                printf("\nTemp file could not be deleted");
        }
        fclose(fp);
    }
out:
    ch341Release();
    return 0;
}
