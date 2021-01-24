/*
 *ch36x_demo.c for ch365/ch367/ch368 pci card of WCH
 * 
 **********************************************************************
 ***********	  Copyright (C) WCH 2013.10.28		***************
 ***********	      web: www.wch.cn			***************
 ***********	  AUTHOR: TECH33 (tech@wch.cn)		***************
 ***********	 Used for PCI/PCIe Chip (CH365/7/8)	***************
 ***********	Nanjing	QinHeng Electronics Co.,Ltd	***************
 **********************************************************************
 *
 * Running Environment: Linux
 * This file is used for testing I/O\MEM\Configuration\ (Read and Write)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>

#include "ch34x_lib.h"
#include "flash_sst.h"

#ifndef CH34x_DEBUG
#define CH34x_DEBUG
#endif

#ifdef CH34x_DEBUG
#define dbg( format, arg...)	printf( format "\n", ##arg );
#endif
#define err( format, arg... )	\
	printf( "error %d: " format "\n", __LINE__, ##arg )

extern int dev_fd;
struct{
	void *DeviceId;
	void *Addr;
	unsigned char *Byte;
}i2c_device;


ULONG enable_data;
ULONG dir_data;
ULONG con_data;

/*
 * ********************************************************************
 * InitPara()
 * Function : Init Parallel
 * ********************************************************************
 */
int InitPara()
{
	int retval;
	unsigned long iMode;
	iMode = 0x00;
	retval = CH34xSetParaMode( iMode );
        if( retval == false )
        {
                err("------>SetPara Error");
                return false;
        }
        retval = CH34xInitParallel( iMode );
        if( retval == false )
        {
                err("------->Init Parallel Error\n");
                return false;
        }
	return true;	
}

/*
 * ********************************************************************
 * CH34x_EppWrite()
 * Function : EPP Write,Write 256 bytes into EPP
 * ********************************************************************
 */
int CH34x_EppWrite()
{
	int retval, i;
	unsigned long iLength;
//	unsigned char iBuffer[64] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};
	unsigned char iBuffer[255];
	for( i = 0; i <= 255; i++ )
		iBuffer[i] = i;
	iLength = 256;
	retval = CH34xEppWriteData( iBuffer, iLength );
	if( retval == false )
	{
		err("----------------->Write Error");
		return false;
	}
	return true;
}

/*
 * ********************************************************************
 * CH34x_EppRead()
 * Function : EPP Read,Read 255 bytes from EPP
 * ********************************************************************
 */
int CH34x_EppRead()
{
	int retval,i;
	unsigned long ioLength;
	unsigned char ioBuffer[MAX_BUFFER_LENGTH];
	ioLength = 256;
	retval = CH34xEppReadData( ioBuffer, ioLength );
	if( retval != ioLength )
	{
		err("------------->Read Error\n");
		return false;
	}
	for( i = 0; i < ioLength; i++ )
	{
		printf("  %d", ioBuffer[i] );
	}
	printf("\n");
	return true;
}

/*
 * ********************************************************************
 * InitMEM()
 * Function : Init Memory
 * ********************************************************************
 */
int InitMEM()
{
	int retval;
        retval = CH34xInitMEM();
        if( retval == false )
        {
                err("------->Init Parallel Error\n");
                return false;
        }
	return true;	
}

/*
 * ********************************************************************
 * CH34x_MEMWrite()
 * Function : MEM Write,Write 255 bytes into MEM
 * ********************************************************************
 */
int CH34x_MEMWrite()
{
	int retval,i;
	unsigned long iLength;
//	unsigned char iBuffer[64] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};
	unsigned char iBuffer[256];
	for(i = 0; i < 256; i++)
		iBuffer[i] = i;
	iLength = 256;
	retval = CH34xMEMWriteData( iBuffer, iLength, 1 );
	if( retval == false )
	{
		err("----------------->Write Error");
		return false;
	}
	return true;
}

/*
 * ********************************************************************
 * CH34x_MEMRead()
 * Function : MEM Read,Read 259 bytes from MEM
 * ********************************************************************
 */
int CH34x_MEMRead()
{
	int retval,i;
	unsigned long ioLength;
	unsigned char ioBuffer[MAX_BUFFER_LENGTH];
	ioLength = 258;
	retval = CH34xMEMReadData( ioBuffer, ioLength, 0 );
	printf("ioLength is %d in MEM Read\n",retval);
	if( retval != ioLength )
	{
		err("------------->Read Error\n");
		return false;
	}
	for( i = 0; i <= ioLength; i++ )
	{
		printf("  %d", ioBuffer[i] );
	}
	printf("\n");
	return true;
}

/*
 * ********************************************************************
 * init_device( void )
 *
 * Function : open device, Get Driver Version, Get Chip ID
 * ********************************************************************
 */
int init_device( void )
{
	int retval = 0;
	
	system("clear");
	dev_fd = CH34xOpenDevice( 0 );
	if( dev_fd <= 0 )
	{
		err("CH34xOpenDevice Open Error");
		return -1;		
	}	
	dbg( "Open sucessful" );
	//Get Driver Version
	char *drv_version = (char *)malloc( sizeof( char ) * 30 );
	
	retval = CH34x_GetDrvVer( drv_version );
	if( retval == false )
	{
		err( "CH34x_GetDrvVer Error" );
		return -1;
	}
	
	printf( "drv_version : %s\n", drv_version );
	free( drv_version );
	//Get Chip Version
	PULONG VendorId = (char *)malloc( sizeof(unsigned long));
	retval = CH34x_GetVendorId( VendorId );
	if( retval == false )
	{
		err( "CH34x_GetVendorId Error" );
		return -1;
	}	
	printf( "VendorId : 0x%04x\n", *VendorId );
	free( VendorId );
	return true;

}

/*
 * ********************************************************************
 * EEPROM_TEST( void )
 *
 * Function : EEPROM Write/Read
 * ********************************************************************
 */
void EEPROM_TEST( void )
{
//Write EEPROM
	int retval = 0;
	UCHAR iBuf;	
	printf("Please input a byte into EEPROM:\n");
	scanf( "%x", &iBuf );
	printf("You input 0x%x\n", iBuf);
	retval = CH34xWriteEEPROM( ID_24C08, 0x0000, 1, &iBuf );
	if( retval == false )
	{
		err("I2C Write Error");
		return -1;
	}

	sleep(1);
//Read EEPROM	
	PUCHAR oBuffer ;
	oBuffer = (PUCHAR)malloc( sizeof(unsigned char) * 30 );
	retval = CH34xReadEEPROM( ID_24C08, 0x0000, 20, oBuffer );
	if( retval == false )
	{
		err("I2C Read Error");
		return -1;
	}
	printf("Output is 0x%x\n", oBuffer[0]);
	free( oBuffer );
}

/*
 * ********************************************************************
 * EPP_TEST()
 *
 * Function : EPP Write/Read
 * ********************************************************************
 */
void EPP_TEST()
{
//EPP Write/Read
	int retval = 0;
	if((retval = InitPara()) == false)
	{
		err("Init Para Error\n");
	}
	if((retval = CH34x_EppWrite()) == false )
	{
		err("Epp Write Error\n");
	}
	if((retval = CH34x_EppRead()) == false )
	{
		err("Read Error");
	}
}

/*
 * ********************************************************************
 * MEM_TEST()
 *
 * Function : MEM Write/Read
 * ********************************************************************
 */
void MEM_TEST()
{
//MEM Write/Read
	int retval = 0;
	if((retval = InitMEM()) == false)
	{
		err("Init MEM Error\n");
	}
	if((retval = CH34x_MEMWrite()) == false )
	{
		err("MEM Write Error\n");
	}
	if((retval = CH34x_MEMRead()) == false )
	{
		err("MEM Read Error");
	}
}

/*
 * ********************************************************************
 * SPI_FLASH_TEST()
 *
 * Function : FLASH Write/Read
 * Note: this function does not include save unused data 
 * ********************************************************************
 */
int SPI_FLASH_TEST()
{
//Read  Block SPI ( MAX 1024 Every one )
/*	UCHAR oBuffer[MAX_BUFFER_LENGTH];
	if( CH34x_Flash_ReadBlock(NULL,0x00) == false )
	{
		printf("Read block Error\n");
	}
*/

	if( CH34xWriteSPI() == false )
	{
		printf("Write SPI Error\n");
		return false;
	}
	if( CH34xReadSPI() == false )
	{
		printf("Read SPI Error\n");
		return false;
	}	

}


/*
 * ********************************************************************
 *  SETOUTPUT_TEST()
 *
 * Function : Set direction and output data of CH341 
 * Note: refer to fuction of CH34xSetOutput() in lib
 * ********************************************************************
 */
int SETOUTPUT_TEST(void)
{
	printf("input the enable data(Hex):\n");
	scanf("%x",&enable_data);
	printf("input the direction data(Hex):\n");
	scanf("%x",&dir_data);
	printf("input the control data(Hex):\n");
	scanf("%x",&con_data); 
	if( CH34xSetOutput(enable_data, dir_data, con_data) == false)
	{
		printf("Set Output Error!\n");
		return false;
	}
	return true;
}

/*
 * ********************************************************************
 *  SETOUTPUT_TEST()
 *
 * Function : Set direction and output data of D5-D0 on CH341
 * Note: refer to fuction of CH34xSet_D5_D0() in lib
 * ********************************************************************
 */

int SET_D5_D0_TEST(void)
{
	printf("input the direction data(Hex):\n");
	scanf("%x",&dir_data);
	printf("input the control data(Hex):\n");
	scanf("%x",&con_data); 
	if(CH34xSet_D5_D0((UCHAR)dir_data, (UCHAR)con_data) == false)
	{
		printf("Set D5_D0 Error!\n");
		return false;
	}
	return true;
}



void ShowMainMenu( void )
{
	printf("This is main menu listed\n");
	printf("-->1: EEPROM TEST\n");
	printf("-->2: EPP TEST\n");
	printf("-->3: MEM TEST\n");
	printf("-->4: SPI FLASH TEST( SST25VF512 )\n");
	printf("-->5: SETOUTPUT TEST\n");
	printf("-->6: SET_D5_D0 TEST\n");
	printf("Please enter your selection:\n");
	
}

int main( int argc, char **argv )
{
	int ch;
	char button = '\0';
	int retval = 0;
	retval = init_device();
	if( retval == -1 )
	{	
		printf("Init device error\n");
		return false;
	}
	while(1)
	{
		ShowMainMenu();
		scanf("%d",&ch);
		printf("You choose %d \n",ch);
		switch(ch)
		{
		    case 1:
			EEPROM_TEST();
			break;
		    case 2:
			EPP_TEST();
			break;
		    case 3:
			MEM_TEST();
			break;
		    case 4:
			SPI_FLASH_TEST();
			break;
		    case 5:
			SETOUTPUT_TEST();
			break;
		    case 6:
			SET_D5_D0_TEST();
			break;
		    default:
			break;	
		}
		do
		{
			printf("\nenter 'q' to exit or 'b' to come back\n");
			scanf(" %c", &button);
		}while( button != 'q' && button != 'b' );
		if( button == 'q' )
			break;
	}

	CH34xCloseDevice(); 
	return 0;
}

int ReadFlashId()
{
	ULONG mLen, iChipselect;
	UCHAR mWrBuf[6];
	mWrBuf[0] = ReadID;
	mWrBuf[1] = 0x00;
	mWrBuf[2] = 0x00;
	mWrBuf[3] = 0x00;
	iChipselect = 0x80;
	mLen = 6;
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;
	else	
		return mWrBuf[5];
}

BOOL CH34xWriteEnable()
{
	ULONG mLen, iChipselect;
	UCHAR mWrBuf[2];
	mWrBuf[0] = WREN;
	mLen = 2;
	iChipselect = 0x80;
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;
	else
		return true;	
}

BOOL CH34xSectorErase( ULONG StartAddr )
{
	ULONG mLen, iChipselect;
	UCHAR mWrBuf[4];
	if( CH34xWriteEnable() == false )
		return false;
	mWrBuf[0] = Sector_Erase;
	mWrBuf[1] = (UCHAR)( StartAddr >> 16 & 0xff );
	mWrBuf[2] = (UCHAR)( StartAddr >> 8 & 0xf0 );
	mWrBuf[3] = 0x00;
	mLen = 4;	
	iChipselect = 0x80;
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;
	
	if( CH34xFlash_Wait() == false )
		return false;
	return true;		
}

int CH34x_Flash_ReadByte( PVOID oBuffer, ULONG iAddr )
{
	ULONG mLen, iChipselect;
	UCHAR mWrBuf[16];
	mLen = 16;
	iChipselect = 0x80;
	mWrBuf[0] = ReadByte;
	mWrBuf[1] = (UCHAR)( iAddr >> 16 & 0xff );
	mWrBuf[2] = (UCHAR)( iAddr >> 8 & 0xff );
	mWrBuf[3] = (UCHAR)( iAddr & 0xff );
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;		
	printf("Read SPI Data,First is %x,Second is %x\n",mWrBuf[4],mWrBuf[5]);	
}

int CH34x_Flash_ReadBlock( PVOID oBuffer, ULONG iAddr)
{
	ULONG mLen, iChipselect;
	UCHAR mWrBuf[MAX_BUFFER_LENGTH];	
	ULONG i;
	mLen = 33;
	iChipselect = 0x80;
//	iAddr = iAddr - iAddr % 0x1000;
	mWrBuf[0] = ReadByte;	
	mWrBuf[1] = (UCHAR)( iAddr >> 16 & 0xff );
	mWrBuf[2] = (UCHAR)( iAddr >> 8 & 0xff );
	mWrBuf[3] = (UCHAR)( iAddr & 0xff );
	if( CH34xStreamSPI4( iChipselect, mLen+4, mWrBuf ) == false )
		return false;
	
	for( i = 0; i < mLen; i++ )
		printf("index is %d, Value is %x\n",i,mWrBuf[i + 4]);
	return true;
}

BOOL CH34xFlash_Wait()
{
	ULONG mLen, iChipselect;
	ULONG i = 0;
	UCHAR mWrBuf[3];
	UCHAR status;
	mLen = 3;
	iChipselect = 0x80;
	mWrBuf[0] = RDSR;
	do{
		mWrBuf[0] = RDSR;
		if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
			return false;		
		status = mWrBuf[1];
	}while( status & 1 );
	return true;
}

BOOL CH34xFlashReadStatus()
{
	ULONG mLen, iChipselect;
	UCHAR status;
	UCHAR mWrBuf[3];
	iChipselect = 0x80;
	mLen = 3;
	mWrBuf[0] = RDSR;
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;		
	status = mWrBuf[1];
	if( status & 0x0c )
	{
		mWrBuf[0] = EWSR;
		mLen = 1;
		if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
			return false;		
		mLen = 2;
		memset( mWrBuf, 0, sizeof( UCHAR ) * mLen );
		mWrBuf[0] = WRSR;
		mWrBuf[1] = status & (~0x0c);
		if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
			return false;
	}	
	mWrBuf[0] = RDSR;
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;		
	status = mWrBuf[1];
	
	return true;
}

BOOL CH34x_Flash_Write( PVOID iBuffer, ULONG iAddr )
{
	ULONG i;
	ULONG iChipselect = 0x80;
	UCHAR mWrBuf[5];
	ULONG mLen = 5;
	for( i = 0; i < 2; i++ )
	{
		if( !CH34xWriteEnable() )
			return false;
		mWrBuf[0] = Byte_Program;
		mWrBuf[1] = (UCHAR)(iAddr >> 16 & 0xff);
		mWrBuf[2] = (UCHAR)(iAddr >> 8 & 0xff);
		mWrBuf[3] = (UCHAR)(iAddr & 0xff);
		iAddr++;
		mWrBuf[4] = *((PUCHAR)iBuffer + i);
		if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
			return false;
		memset( mWrBuf, 0, sizeof( UCHAR ) * mLen );
		if( !CH34xFlash_Wait() )
			return false;		
	}
	return true;
}

BOOL CH34xReadSPI()
{
	ULONG mLen;
	CH34x_Flash_ReadByte( NULL, 0x0000 );
	return true;
}

BOOL CH34xWriteSPI()
{
	int retval;
	int i;
	PUCHAR BufData;
	BufData = malloc( sizeof( UCHAR ) * 4096 );
	if( BufData == NULL )
	{
		printf("malloc error\n");
	}	
	retval = CH34xFlashReadStatus();
	if( retval == false )
	{
		printf("error in flash status\n");
		return false;
	}
/*	retval = CH34x_Flash_ReadByte( BufData, 0x0000 );
	if( retval == false )
	{
		printf("error in flash ReadByte\n");
		return false;
	}
*/
	retval = CH34xSectorErase( 0x0000 );
	if( retval == false )
	{
		printf("error in flash Sector Erase\n");
		return false;
	}

//	BufData[0] = 0xaa;
//	BufData[1] = 0x55;
	printf("Please input 2 number:\n");
	for( i = 0; i < 2; i++ )
		scanf("%x", &BufData[i]);
	retval = CH34x_Flash_Write( BufData, 0x0000 );
	if( retval == false )
	{
		printf("error in flash Write\n");
		return false;
	}
	free( BufData );
	return true;

}
