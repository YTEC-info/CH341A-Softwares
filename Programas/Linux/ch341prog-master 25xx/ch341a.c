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

#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include "ch341a.h"

struct libusb_device_handle *devHandle = NULL;
struct sigaction saold;
int force_stop = 0;

void v_print(int mode, int len) ;

/* SIGINT handler */
void sig_int(int signo)
{
    force_stop = 1;
}

/* Configure CH341A, find the device and set the default interface. */
int32_t ch341Configure(uint16_t vid, uint16_t pid)
{
    struct libusb_device *dev;
    int32_t ret;
    struct sigaction sa;

    uint8_t  desc[0x12];

    if (devHandle != NULL) {
        fprintf(stderr, "Call ch341Release before re-configure\n");
        return -1;
    }
    ret = libusb_init(NULL);
    if(ret < 0) {
        fprintf(stderr, "Couldn't initialise libusb\n");
        return -1;
    }

    #if LIBUSB_API_VERSION < 0x01000106
        libusb_set_debug(NULL, 3);
    #else
        libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);
    #endif

    if(!(devHandle = libusb_open_device_with_vid_pid(NULL, vid, pid))) {
        fprintf(stderr, "Couldn't open device [%04x:%04x].\n", vid, pid);
        return -1;
    }

    if(!(dev = libusb_get_device(devHandle))) {
        fprintf(stderr, "Couldn't get bus number and address.\n");
        goto close_handle;
    }

    if(libusb_kernel_driver_active(devHandle, 0)) {
        ret = libusb_detach_kernel_driver(devHandle, 0);
        if(ret) {
            fprintf(stderr, "Failed to detach kernel driver: '%s'\n", strerror(-ret));
            goto close_handle;
        }
    }

    ret = libusb_claim_interface(devHandle, 0);

    if(ret) {
        fprintf(stderr, "Failed to claim interface 0: '%s'\n", strerror(-ret));
        goto close_handle;
    }

    ret = libusb_get_descriptor(devHandle, LIBUSB_DT_DEVICE, 0x00, desc, 0x12);

    if(ret < 0) {
        fprintf(stderr, "Failed to get device descriptor: '%s'\n", strerror(-ret));
        goto release_interface;
    }

    printf("Device reported its revision [%d.%02d]\n", desc[12], desc[13]);
    sa.sa_handler = &sig_int;
    sa.sa_flags = SA_RESTART;
    sigfillset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, &saold) == -1) {
        perror("Error: cannot handle SIGINT"); // Should not happen
    }
    return 0;
release_interface:
    libusb_release_interface(devHandle, 0);
close_handle:
    libusb_close(devHandle);
    devHandle = NULL;
    return -1;
}

/* release libusb structure and ready to exit */
int32_t ch341Release(void)
{
    if (devHandle == NULL) return -1;
    libusb_release_interface(devHandle, 0);
    libusb_close(devHandle);
    libusb_exit(NULL);
    devHandle = NULL;
    sigaction(SIGINT, &saold, NULL);
    return 0;
}

/* Helper function for libusb_bulk_transfer, display error message with the caller name */
int32_t usbTransfer(const char * func, uint8_t type, uint8_t* buf, int len)
{
    int32_t ret;
    int transfered;
    if (devHandle == NULL) return -1;
    ret = libusb_bulk_transfer(devHandle, type, buf, len, &transfered, DEFAULT_TIMEOUT);
    if (ret < 0) {
        fprintf(stderr, "%s: Failed to %s %d bytes '%s'\n", func,
                (type == BULK_WRITE_ENDPOINT) ? "write" : "read", len, libusb_error_name(ret));
        return -1;
    }
    return transfered;
}

/*   set the i2c bus speed (speed(b1b0): 0 = 20kHz; 1 = 100kHz, 2 = 400kHz, 3 = 750kHz)
 *   set the spi bus data width(speed(b2): 0 = Single, 1 = Double)  */
int32_t ch341SetStream(uint32_t speed) {
    uint8_t buf[3];

    if (devHandle == NULL) return -1;
    buf[0] = CH341A_CMD_I2C_STREAM;
    buf[1] = CH341A_CMD_I2C_STM_SET | (speed & 0x7);
    buf[2] = CH341A_CMD_I2C_STM_END;

    return usbTransfer(__func__, BULK_WRITE_ENDPOINT, buf, 3);
}

/* ch341 requres LSB first, swap the bit order before send and after receive  */
uint8_t swapByte(uint8_t c)
{
    static const uint8_t reverse_table[] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
    };
    return reverse_table[c];
}

/* assert or deassert the chip-select pin of the spi device */
void ch341SpiCs(uint8_t *ptr, bool selected)
{
    *ptr++ = CH341A_CMD_UIO_STREAM;
    *ptr++ = CH341A_CMD_UIO_STM_OUT | (selected ? 0x36 : 0x37);
    if (selected)
        *ptr++ = CH341A_CMD_UIO_STM_DIR | 0x3F; // pin direction
    *ptr++ = CH341A_CMD_UIO_STM_END;
}

/* transfer len bytes of data to the spi device */
int32_t ch341SpiStream(uint8_t *out, uint8_t *in, uint32_t len)
{
    uint8_t inBuf[CH341_PACKET_LENGTH], outBuf[CH341_PACKET_LENGTH], *inPtr, *outPtr;
    int32_t ret, packetLen;
    bool done;

    if (devHandle == NULL) return -1;

    ch341SpiCs(outBuf, true);
    ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 4);
    if (ret < 0) return -1;

    inPtr = in;

    do {
        done=true;
        packetLen=len+1;    // STREAM COMMAND + data length
        if (packetLen>CH341_PACKET_LENGTH) {
            packetLen=CH341_PACKET_LENGTH;
            done=false;
        }
        outPtr = outBuf;
        *outPtr++ = CH341A_CMD_SPI_STREAM;
        for (int i = 0; i < packetLen-1; ++i)
            *outPtr++ = swapByte(*out++);
        ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, packetLen);
        if (ret < 0) return -1;
        ret = usbTransfer(__func__, BULK_READ_ENDPOINT, inBuf, packetLen-1);
        if (ret < 0) return -1;
        len -= ret;

        for (int i = 0; i < ret; ++i) // swap the buffer
            *inPtr++ = swapByte(inBuf[i]);
    } while (!done);

    ch341SpiCs(outBuf, false);
    ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 3);
    if (ret < 0) return -1;
    return 0;
}

#define JEDEC_ID_LEN 0x52    // additional byte due to SPI shift
/* read the JEDEC ID of the SPI Flash */
int32_t ch341SpiCapacity(void)
{
    uint8_t out[JEDEC_ID_LEN];
    uint8_t in[JEDEC_ID_LEN], *ptr, cap;
    int32_t ret;

    if (devHandle == NULL)
        return -1;

    ptr = out;
    *ptr++ = 0x9F; // Read JEDEC ID

    for (int i = 0; i < JEDEC_ID_LEN - 1; ++i)
        *ptr++ = 0x00;

    ret = ch341SpiStream(out, in, JEDEC_ID_LEN);

    if (ret < 0)
        return ret;

    if (! (in[1] == 0xFF && in[2] == 0xFF && in[3] == 0xFF))
    {
        printf("Manufacturer ID: %02x\n", in[1]);
        printf("Memory Type: %02x%02x\n", in[2], in[3]);

        if (in[0x11] == 'Q' && in[0x12] == 'R' && in[0x13] == 'Y')
        {
            cap = in[0x28];
            printf("Reading device capacity from CFI structure\n");
        }
        else
        {
            cap = in[3];
            printf("No CFI structure found, trying to get capacity from device ID. Set manually if detection fails.\n");
        }

        printf("Capacity: %02x\n", cap);
    }
    else
    {
        printf("Chip not found or missed in ch341a. Check connection\n");
        exit(0);
    }

    return cap;
}

/* read status register */
int32_t ch341ReadStatus(void)
{
    uint8_t out[2];
    uint8_t in[2];
    int32_t ret;

    if (devHandle == NULL) return -1;
    out[0] = 0x05; // Read status
    ret = ch341SpiStream(out, in, 2);
    if (ret < 0) return ret;
    return (in[1]);
}

/* write status register */
int32_t ch341WriteStatus(uint8_t status)
{
    uint8_t out[2];
    uint8_t in[2];
    int32_t ret;

    if (devHandle == NULL) return -1;
    out[0] = 0x06; // Write enable
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
    out[0] = 0x01; // Write status
    out[1] = status;
    ret = ch341SpiStream(out, in, 2);
    if (ret < 0) return ret;
    out[0] = 0x04; // Write disable
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
    return 0;
}

/* chip erase */
int32_t ch341EraseChip(void)
{
    uint8_t out[1];
    uint8_t in[1];
    int32_t ret;

    if (devHandle == NULL) return -1;
    out[0] = 0x06; // Write enable
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
    out[0] = 0xC7; // Chip erase
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
    out[0] = 0x04; // Write disable
    ret = ch341SpiStream(out, in, 1);
    if (ret < 0) return ret;
    return 0;
}

/* callback for bulk out async transfer */
void cbBulkOut(struct libusb_transfer *transfer)
{
    if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
        fprintf(stderr, "\ncbBulkOut: error : %d\n", transfer->status);
    }
}

struct spi_transfer_in {
    ssize_t bulk_count;
    size_t skip_bytes;
    uint8_t *read_buffer;
};

/* callback for bulk in async transfer */
void cbBulkIn(struct libusb_transfer *transfer)
{
    struct spi_transfer_in *tf = transfer->user_data;
    switch(transfer->status) {
        case LIBUSB_TRANSFER_COMPLETED:
            /* the first package has cmd and address info, so discard 4 bytes */
            if (tf->read_buffer != NULL) {
                int i = tf->bulk_count? 0: tf->skip_bytes;
                for(; i < transfer->actual_length; ++i) {
                    *tf->read_buffer++ = swapByte(transfer->buffer[i]);
                }
            }
            tf->bulk_count++;
            break;
        default:
            fprintf(stderr, "\ncbBulkIn: error : %d\n", transfer->status);
            tf->bulk_count = -1;
    }
    return;
}

/* read the content of SPI device to buf, make sure the buf is big enough before call  */
int32_t ch341SpiRead(uint8_t *buf, uint32_t add, uint32_t len)
{
    uint8_t out[CH341_MAX_PACKET_LEN];
    uint8_t in[CH341_PACKET_LENGTH];
    bool fourbyte = (add + len) > (1 << 24);

    if (devHandle == NULL) return -1;
    /* what subtracted is: 1. first cs package, 2. leading command for every other packages,
     * 3. second package contains read flash command and 3 bytes address */
    const uint32_t max_payload = CH341_MAX_PACKET_LEN - CH341_PACKET_LENGTH
        - CH341_MAX_PACKETS + 1 - 4 - (fourbyte? 1: 0);
    uint32_t pkg_len, pkg_count;
    struct libusb_transfer *xferBulkIn, *xferBulkOut;
    uint32_t idx = 0;
    uint32_t ret;
    int32_t old_counter;
    struct timeval tv = {0, 100};
    struct spi_transfer_in bulk_in = {};

    v_print( 0, len); // verbose

    memset(out, 0xff, CH341_MAX_PACKET_LEN);
    for (int i = 1; i < CH341_MAX_PACKETS; ++i) // fill CH341A_CMD_SPI_STREAM for every packet
        out[i * CH341_PACKET_LENGTH] = CH341A_CMD_SPI_STREAM;
    memset(in, 0x00, CH341_PACKET_LENGTH);
    xferBulkIn  = libusb_alloc_transfer(0);
    xferBulkOut = libusb_alloc_transfer(0);

    printf("Read started!\n");
    while (len > 0) {
        v_print( 1, len); // verbose
        fflush(stdout);
        ch341SpiCs(out, true);
        idx = CH341_PACKET_LENGTH + 1;
        out[idx++] = swapByte(fourbyte? 0x13: 0x03);
        if (fourbyte)
            out[idx++] = swapByte(add >> 24);
        out[idx++] = swapByte(add >> 16);
        out[idx++] = swapByte(add >> 8);
        out[idx++] = swapByte(add);
        bulk_in.skip_bytes = fourbyte? 5: 4;
        if (len > max_payload) {
            pkg_len = CH341_MAX_PACKET_LEN;
            pkg_count = CH341_MAX_PACKETS - 1;
            len -= max_payload;
            add += max_payload;
        } else {
            pkg_count = (len + 4) / (CH341_PACKET_LENGTH - 1);
            if ((len + 4) % (CH341_PACKET_LENGTH - 1)) pkg_count ++;
            pkg_len = (pkg_count) * CH341_PACKET_LENGTH + ((len + 4) % (CH341_PACKET_LENGTH - 1)) + 1;
            len = 0;
        }
        bulk_in.bulk_count = 0;
        bulk_in.read_buffer = buf;
        libusb_fill_bulk_transfer(xferBulkIn, devHandle, BULK_READ_ENDPOINT, in,
                CH341_PACKET_LENGTH, cbBulkIn, &bulk_in, DEFAULT_TIMEOUT);
        buf += max_payload; // advance user's pointer
        libusb_submit_transfer(xferBulkIn);
        libusb_fill_bulk_transfer(xferBulkOut, devHandle, BULK_WRITE_ENDPOINT, out,
                pkg_len, cbBulkOut, NULL, DEFAULT_TIMEOUT);
        libusb_submit_transfer(xferBulkOut);
        old_counter = bulk_in.bulk_count;
        while (bulk_in.bulk_count < pkg_count) {
            libusb_handle_events_timeout(NULL, &tv);
            if (bulk_in.bulk_count == -1) { // encountered error
                len = 0;
                ret = -1;
                break;
            }
            if (old_counter != bulk_in.bulk_count) { // new package came
                if (bulk_in.bulk_count != pkg_count)
                    libusb_submit_transfer(xferBulkIn);  // resubmit bulk in request
                old_counter = bulk_in.bulk_count;
            }
        }
        ch341SpiCs(out, false);
        ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, out, 3);
        if (ret < 0) break;
        if (force_stop == 1) { // user hit ctrl+C
            force_stop = 0;
            if (len > 0)
                fprintf(stderr, "User hit Ctrl+C, reading unfinished.\n");
            break;
        }
    }
    libusb_free_transfer(xferBulkIn);
    libusb_free_transfer(xferBulkOut);
    v_print(2, 0);
    return ret;
}

#define WRITE_PAYLOAD_LENGTH 301 // 301 is the length of a page(256)'s data with protocol overhead
/* write buffer(*buf) to SPI flash */
int32_t ch341SpiWrite(uint8_t *buf, uint32_t add, uint32_t len)
{
    uint8_t out[WRITE_PAYLOAD_LENGTH];
    uint8_t in[CH341_PACKET_LENGTH];
    uint32_t tmp, pkg_count;
    struct libusb_transfer *xferBulkIn, *xferBulkOut;
    uint32_t idx = 0;
    uint32_t ret;
    int32_t old_counter;
    struct timeval tv = {0, 100};
    bool fourbyte = (add + len) > (1 << 24);
    struct spi_transfer_in bulk_in = { .read_buffer = NULL };

    v_print(0, len); // verbose

    if (devHandle == NULL) return -1;
    memset(out, 0xff, WRITE_PAYLOAD_LENGTH);
    xferBulkIn  = libusb_alloc_transfer(0);
    xferBulkOut = libusb_alloc_transfer(0);

    printf("Write started!\n");
    while (len > 0) {
        v_print(1, len);

        out[0] = 0x06; // Write enable
        ret = ch341SpiStream(out, in, 1);
        ch341SpiCs(out, true);
        idx = CH341_PACKET_LENGTH;
        out[idx++] = CH341A_CMD_SPI_STREAM;
        out[idx++] = swapByte(fourbyte? 0x12: 0x02);
        if (fourbyte)
            out[idx++] = swapByte(add >> 24);
        out[idx++] = swapByte(add >> 16);
        out[idx++] = swapByte(add >> 8);
        out[idx++] = swapByte(add);

        tmp = 0;
        pkg_count = 1;

        while ((idx < WRITE_PAYLOAD_LENGTH) && (len > tmp)) {
            if (idx % CH341_PACKET_LENGTH == 0) {
                out[idx++] = CH341A_CMD_SPI_STREAM;
                pkg_count ++;
            } else {
                out[idx++] = swapByte(*buf++);
                tmp++;
                if (((add + tmp) & 0xFF) == 0) // cross page boundary
                    break;
            }
        }
        len -= tmp;
        add += tmp;
        bulk_in.bulk_count = 0;
        libusb_fill_bulk_transfer(xferBulkIn, devHandle, BULK_READ_ENDPOINT, in,
                CH341_PACKET_LENGTH, cbBulkIn, &bulk_in, DEFAULT_TIMEOUT);
        libusb_submit_transfer(xferBulkIn);
        libusb_fill_bulk_transfer(xferBulkOut, devHandle, BULK_WRITE_ENDPOINT, out,
                idx, cbBulkOut, NULL, DEFAULT_TIMEOUT);
        libusb_submit_transfer(xferBulkOut);
        old_counter = bulk_in.bulk_count;
        ret = 0;
        while (bulk_in.bulk_count < pkg_count) {
            libusb_handle_events_timeout(NULL, &tv);
            if (bulk_in.bulk_count == -1) { // encountered error
                ret = -1;
                break;
            }
            if (old_counter != bulk_in.bulk_count) { // new package came
                if (bulk_in.bulk_count != pkg_count)
                    libusb_submit_transfer(xferBulkIn);  // resubmit bulk in request
                old_counter = bulk_in.bulk_count;
            }
        }
        if (ret < 0) break;
        ch341SpiCs(out, false);
        ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, out, 3);
        if (ret < 0) break;
        out[0] = 0x04; // Write disable
        ret = ch341SpiStream(out, in, 1);
        do {
            ret = ch341ReadStatus();
            if (ret != 0)
                libusb_handle_events_timeout(NULL, &tv);
        } while(ret != 0);
        if (force_stop == 1) { // user hit ctrl+C
            force_stop = 0;
            if (len > 0)
                fprintf(stderr, "User hit Ctrl+C, writing unfinished.\n");
            break;
        }
    }
    libusb_free_transfer(xferBulkIn);
    libusb_free_transfer(xferBulkOut);

    v_print(2, 0);
    return ret;
}
