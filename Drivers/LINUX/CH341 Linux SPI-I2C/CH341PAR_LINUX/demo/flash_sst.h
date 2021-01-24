//
//
//****************************************
//
//**  Copyright  (C)  W.ch  2013        **
//
//**  Web:  http://www.wch.cn	        **
//
//****************************************
//
//**  LIB for USB interface chip CH341  **
//
//**  C, LINUX                          **
//
//****************************************
//
//

#ifndef		_FLASH_SST_H
#define		_FLASH_SST_H

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

int ReadFlashId();

BOOL CH34xWriteEnable();

BOOL CH34xSectorErase( ULONG StartAddr );

int CH34x_Flash_ReadByte( PVOID oBuffer,
			ULONG iAddr);

int CH34x_Flash_ReadBlock( PVOID oBuffer,
			ULONG iAddr);

BOOL CH34xFlash_Wait();

BOOL CH34x_Flash_Write( PVOID iBuffer,
			ULONG iAddr );

BOOL CH34xReadSPI();

BOOL CH34xWriteSPI();

#endif
