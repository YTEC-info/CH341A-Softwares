/*
 * flash.h
 *
 *  Created on: Dec 2, 2013
 *      Author: root
 */

#ifndef FLASH_H_
#define FLASH_H_

#define ReadByte	0x03
#define Sector_Erase	0x20
#define Block_Erase	0x52
#define Chip_Erase	0x60
#define Byte_Program	0x02
#define AAI		0xAF
#define RDSR		0x05
#define EWSR		0x50
#define WRSR		0x01
#define WREN		0x06
#define WRDI		0x04
#define ReadID		0x90

#endif /* FLASH_H_ */
