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
 */
#ifndef __CH341_H__
#define __CH341_H__

#ifdef __cplusplus
extern "C" {
#endif
#define     DEFAULT_TIMEOUT        1000     // 1000mS for USB timeouts
#define     BULK_WRITE_ENDPOINT    0x02
#define     BULK_READ_ENDPOINT     0x82

#define     CH341_PACKET_LENGTH    0x20
#define     CH341_MAX_PACKETS      256
#define     CH341_MAX_PACKET_LEN   (CH341_PACKET_LENGTH * CH341_MAX_PACKETS)
#define     CH341A_USB_VENDOR      0x1A86
#define     CH341A_USB_PRODUCT     0x5512

#define     CH341A_CMD_SET_OUTPUT  0xA1
#define     CH341A_CMD_IO_ADDR     0xA2
#define     CH341A_CMD_PRINT_OUT   0xA3
#define     CH341A_CMD_SPI_STREAM  0xA8
#define     CH341A_CMD_SIO_STREAM  0xA9
#define     CH341A_CMD_I2C_STREAM  0xAA
#define     CH341A_CMD_UIO_STREAM  0xAB

#define     CH341A_CMD_I2C_STM_STA 0x74
#define     CH341A_CMD_I2C_STM_STO 0x75
#define     CH341A_CMD_I2C_STM_OUT 0x80
#define     CH341A_CMD_I2C_STM_IN  0xC0
#define     CH341A_CMD_I2C_STM_MAX ( min( 0x3F, CH341_PACKET_LENGTH ) )
#define     CH341A_CMD_I2C_STM_SET 0x60
#define     CH341A_CMD_I2C_STM_US  0x40
#define     CH341A_CMD_I2C_STM_MS  0x50
#define     CH341A_CMD_I2C_STM_DLY 0x0F
#define     CH341A_CMD_I2C_STM_END 0x00

#define     CH341A_CMD_UIO_STM_IN  0x00
#define     CH341A_CMD_UIO_STM_DIR 0x40
#define     CH341A_CMD_UIO_STM_OUT 0x80
#define     CH341A_CMD_UIO_STM_US  0xC0
#define     CH341A_CMD_UIO_STM_END 0x20

#define     CH341A_STM_I2C_20K     0x00
#define     CH341A_STM_I2C_100K    0x01
#define     CH341A_STM_I2C_400K    0x02
#define     CH341A_STM_I2C_750K    0x03
#define     CH341A_STM_SPI_DBL     0x04

int32_t usbTransfer(const char * func, uint8_t type, uint8_t* buf, int len);
int32_t ch341Configure(uint16_t vid, uint16_t pid);
int32_t ch341SetStream(uint32_t speed);
int32_t ch341SpiStream(uint8_t *out, uint8_t *in, uint32_t len);
int32_t ch341SpiCapacity(void);
int32_t ch341SpiRead(uint8_t *buf, uint32_t add, uint32_t len);
int32_t ch341ReadStatus(void);
int32_t ch341WriteStatus(uint8_t status);
int32_t ch341EraseChip(void);
int32_t ch341SpiWrite(uint8_t *buf, uint32_t add, uint32_t len);
int32_t ch341Release(void);
uint8_t swapByte(uint8_t c);

#ifdef __cplusplus
}
#endif

#endif
