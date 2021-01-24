// Created on: Nov 28, 2013
/*
 * File : ch34x_lib.c ch34x_lib.h
 *
 **********************************************************************
 ***********		Copyright (C) WCH	2013.11.28		***************
 ***********			web: http://www.wch.cn			***************
 ***********		AUTHOR: TECH33 (tech@wch.cn)		***************
 ***********	  Used for USB Interface Chip (CH341)	***************
 ***********	Nanjing	QinHeng Electronics Co.,Ltd		***************
 **********************************************************************
 *
 * Running Environment: Linux
 * This file is used for CH34x in Epp/MEM/I2C/SPI
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h> //for 'ioctl' 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ch34x_lib.h"

#define CH34x_GET_DRV_VERSION                   0x00000001
#define CH34x_CHIP_VERSION                      0x00000003
#define CH34x_FUNCTION_SETPARA_MODE             0x00000004
#define CH34x_FUNCTION_READ_MODE                0x00000005
#define CH34x_FUNCTION_WRITE_MODE               0x00000006
#define CH34x_I2C_READ_MODE                     0x00000007
#define CH34x_I2C_WRITE_MODE                    0x00000008
#define CH34x_PIPE_DATA_DOWN                    0x00000009
#define CH34x_PIPE_WRITE_READ                   0x0000000a
#define CH34x_PIPE_DEVICE_CTRL			0x0000000b


int dev_fd;
unsigned char VenIC;
unsigned char StreamMode = 1;

/*
 * ********************************************************************
 * FUNCTION : Open Device
 * arg:
 * index : the number of device
 * ********************************************************************
 */
int CH34xOpenDevice( UCHAR index )
{
	int retval = 0;
	char dev_path[20] = {'\0'};
	memset( dev_path, 0, sizeof(dev_path));
	sprintf( dev_path, "%s%c", "/dev/ch34x_pis", '0' + index);

	dev_fd = open( dev_path, O_RDWR );
	if( dev_fd == -1 )
	{
		printf("open /dev/ch34x_pis error, LINE : %d\n",__LINE__ );	
		return false;
	}
//	printf("Open successful\n");
	
	return dev_fd;
}

/*
 * ********************************************************************
 * FUNCTION : Close Device
 * ********************************************************************
 */
void CH34xCloseDevice()
{
	close( dev_fd );
}

/*
 * ********************************************************************
 * FUNCTION : Get Driver Version
 * arg:
 * drv_version : output buffer of driver version
 * ********************************************************************
 */
BOOL CH34x_GetDrvVer( char *drv_version )
{
	int retval = 0;
	
	retval = ioctl( dev_fd, CH34x_GET_DRV_VERSION, (unsigned long)drv_version );
	if( retval == -1 )	
	{		
		printf( "CH34x_GET_DRV_VERSION function error line:%d\n", __LINE__ );		
		return false;	
	}
	
	return true;
}

/*
 * ********************************************************************
 * FUNCTION : Get Chip ID
 * arg:
 * VendorId : output data of chip version(CH341A : 0x0030)
 * ********************************************************************
 */
BOOL CH34x_GetVendorId( PULONG VendorId )
{
	int retval = 0;
	retval = ioctl( dev_fd, CH34x_CHIP_VERSION, (unsigned long)VendorId);
	if( retval == -1 )	
	{		
		printf("Get Chip Version Error,line:%d\n",__LINE__);	
		return false;	
	}
	
	VenIC = (UCHAR)*VendorId;
//	printf("VenIc is 0x%x\n",VenIC);
	return true;
}

//Mode = 0 ------>EPP Mode
//Mode = 1 ------>EPP Mode
//Mode = 2 ------>MEM Mode
/*
 * ********************************************************************
 * FUNCTION : Set Para Mode
 * arg:
 * Mode : set Para Mode( EPP or MEM )
 * ********************************************************************
 */
BOOL CH34xSetParaMode( ULONG Mode )
{
	int retval = 0;
	
	retval = ioctl( dev_fd, CH34x_FUNCTION_SETPARA_MODE, Mode);
	if( retval < 0 )
	{
		printf("SetPara Error\n");
		return false;
	}

	return true;
}

//Mode = 0 ------>EPP Mode
//Mode = 1 ------>EPP Mode
//Mode = 2 ------>MEM Mode
/*
 * ********************************************************************
 * FUNCTION : Init Para
 * arg:
 * Mode : first reset ,then set Para Mode( EPP or MEM )
 * ********************************************************************
 */
BOOL CH34xInitParallel( ULONG Mode )
{
	int retval = 0;

	retval = ioctl( dev_fd, CH34x_PIPE_DEVICE_CTRL, Mode);
	if( retval < 0 )
	{
		printf("Init Parallel Error,LINE : %d\n", __LINE__);
		return false;
	}
	return true;
}

//PipeMode = 1 ------>Read from Pipe1
//PipeMode = 0 ------>Read from Pipe0
/*
 * ********************************************************************
 * FUNCTION : EPP READ
 * arg:
 * oBuffer : Output buffer
 * ioLength: should read the length of Data/Addr
 * PipeMode: Read Pipe
 * 		   	 -->0  Read Pipe0 Data
 * 			 -->1  Read Pipe1 Addr
 * ********************************************************************
 */
int CH34xEppRead( PUCHAR oBuffer, ULONG ioLength, ULONG PipeMode )
{
	int retval = 0;
	ULONG mReturnlen;
	retval = ioctl( dev_fd, CH34x_FUNCTION_READ_MODE, PipeMode );
	if( retval < 0 )
	{
		printf("Set Pipe Error\n");
		return false;
	}
	mReturnlen = read( dev_fd, oBuffer, ioLength );
	if( mReturnlen != ioLength )
	{
		printf("Numbers of wanted output is not equal to input\n");
		return false;
	}

	return mReturnlen;
}

//PipeMode = 1 ------>Read from Pipe1
//PipeMode = 0 ------>Read from Pipe0
/*
 * ********************************************************************
 * FUNCTION : EPP Write
 * arg:
 * iBuffer : Iutput buffer
 * ioLength: should Write the length of Data/Addr
 * PipeMode: Write Pipe
 * 		   	 -->0  Read Pipe0 Data
 * 			 -->1  Read Pipe1 Addr
 * ********************************************************************
 */
int CH34xEppWrite( PUCHAR iBuffer, ULONG ioLength, ULONG PipeMode )
{
	int retval =0;
	retval = ioctl( dev_fd, CH34x_FUNCTION_WRITE_MODE, PipeMode );
	if( retval < 0 )
	{
		printf("Set Write Pipe Error\n");
		return false;
	}	
	retval = write( dev_fd, iBuffer, ioLength );
	if( retval < 0 )
	{
		printf("Write Epp Error\n");
		return false;
	} 

	return true;
}

/*
 * ********************************************************************
 * FUNCTION : EPP Write Data
 * arg:
 * iBuffer : Output buffer
 * ioLength: should Write the length of Data
 * ********************************************************************
 */
int CH34xEppWriteData( PUCHAR iBuffer, ULONG ioLength )
{
	int retval;
	retval = CH34xEppWrite( iBuffer, ioLength, 0);
	if( !retval )
		return false;
	return true;
}

/*
 * ********************************************************************
 * FUNCTION : EPP Read Data
 * arg:
 * oBuffer : Iutput buffer
 * ioLength: should read the length of Data
 * ********************************************************************
 */
int CH34xEppReadData( PUCHAR oBuffer, ULONG ioLength )
{
	int retval;
	retval = CH34xEppRead( oBuffer, ioLength, 0 );	// set Pipe0,then Read Data from pipe0
	if( !retval )
		return false;
	return retval;
}

/*
 * ********************************************************************
 * FUNCTION : EPP Write Addr
 * arg:
 * iBuffer : Output buffer Data
 * ioLength: should write the length of Addr
 * ********************************************************************
 */
int CH34xEppWriteAddr( PUCHAR iBuffer, ULONG ioLength )
{
        int retval;
        retval = CH34xEppWrite( iBuffer, ioLength, 1 ); // set Pipe1,then Write Data from pipe1
        if( !retval )
                return false;
        return true;
}

/*
 * ********************************************************************
 * FUNCTION : EPP Read Addr
 * arg:
 * oBuffer : Iutput buffer Data
 * ioLength: should read the length of Addr
 * ********************************************************************
 */
int CH34xEppReadAddr( PUCHAR oBuffer, ULONG ioLength )
{
        int retval;
        retval = CH34xEppRead( oBuffer, ioLength, 1 ); // set Pipe1,then Read Data from pipe1
        if( !retval )
                return false;
        return retval;
}

/*
 * ********************************************************************
 * FUNCTION : EPP Set Addr
 * arg:
 * iAddr: Need set the iAddr
 * ********************************************************************
 */
int CH34xEppSetAddr( ULONG iAddr )
{
	int retval;
	UCHAR iBuffer[4];
	ULONG mLength;
	iBuffer[0] = iAddr;
	mLength = 1;
	retval = CH34xEppWriteAddr( iBuffer, mLength );
	if( !retval )
		return false;

	return true;
}

/*
 * ********************************************************************
 * FUNCTION : Init MEM
 * ********************************************************************
 */
BOOL CH34xInitMEM(void)
{
	int retval = 0;
	ULONG iMode;
	iMode = 0x02;
	retval = CH34xInitParallel( iMode );
	if( retval == false )
	{
		printf("Init MEM Mode Error\n");
		return false;
	}
	return true;
}

//PipeMode = 1 ------>Read from Pipe1
//PipeMode = 0 ------>Read from Pipe0
/*
 * ********************************************************************
 * FUNCTION : MEM Read
 * arg:
 * oBuffer : Output buffer
 * ioLength: should Read the length of Data
 * PipeMode: Write Pipe
 * 		   	 -->0  Read Pipe0 Data
 * 			 -->1  Read Pipe1 Data
 * ********************************************************************
 */
int CH34xMEMReadData( PUCHAR oBuffer, ULONG ioLength, ULONG PipeMode )
{
	int retval = 0;
	ULONG mReturnlen;
	retval = ioctl( dev_fd, CH34x_FUNCTION_READ_MODE, PipeMode );
	if( retval < 0 )
	{
		printf("Set Pipe Error\n");
		return false;
	}
	mReturnlen = read( dev_fd, oBuffer, ioLength );
	if( mReturnlen != ioLength )
	{
		printf("Numbers of wanted output is not equal to input\n");
		return false;
	}

	return mReturnlen;
}

//PipeMode = 1 ------>Read from Pipe1
//PipeMode = 0 ------>Read from Pipe0
/*
 * ********************************************************************
 * FUNCTION : MEM Write
 * arg:
 * iBuffer : Iutput buffer
 * ioLength: should Write the length of Data
 * PipeMode: Write Pipe
 * 		   	 -->0  Read Pipe0 Data
 * 			 -->1  Read Pipe1 Data
 * ********************************************************************
 */
int CH34xMEMWriteData( PUCHAR iBuffer, ULONG ioLength, ULONG PipeMode )
{
	int retval =0;
	retval = ioctl( dev_fd, CH34x_FUNCTION_WRITE_MODE, PipeMode );
	if( retval < 0 )
	{
		printf("Set Write Pipe Error\n");
		return false;
	}
	retval = write( dev_fd, iBuffer, ioLength );
	if( retval < 0 )
	{
		printf("Write Epp Error\n");
		return false;
	} 

	return true;
}

/*
 * ********************************************************************
 * FUNCTION : Set Stream Mode
 * arg:
 * Mode : Set Stream Mode
 * -> bit0~1 : set I2C SCL rate
 * 			   --> 00 :	Low Rate /20KHz
 * 			   --> 01 : Default Rate /100KHz
 * 			   --> 10 : Fast Rate /400KHz
 * 			   --> 11 : Full Rate /750KHz
 * -> bit2 : set spi mode
 * 			   --> 0 : one in one out(D3 :clk/ D5 :out/ D7 :in)
 * 			   --> 1 : two in two out(D3 :clk/ D4,D5 :out/ D6,D7 :in)
 * -> bit7 : set spi data mode
 * 			   --> 0 : low bit first
 *       	   --> 1 : high bit first
 * other bits must keep 0
 * ********************************************************************
 */
BOOL CH34xSetStream( ULONG Mode )
{
	UCHAR mBuffer[CH341_PACKET_LENGTH];
	ULONG mLength;
	if( VenIC < 0x20 )
			return false;
	StreamMode = (UCHAR)( Mode & 0x8F );
	mBuffer[0] = CH341A_CMD_I2C_STREAM;
	mBuffer[1] = CH341A_CMD_I2C_STM_SET;
	mBuffer[2] = CH341A_CMD_I2C_STM_END;
	mLength = 3;
	if( CH34xWriteData( mBuffer, &mLength ))
	{
		if( mLength >= 2 )
			return true;
	}
	return false;
}

/*
 * ********************************************************************
 * FUNCTION : Set Delay
 * arg:
 * iDelay : set delay time(ms)
 * ********************************************************************
 */
BOOL CH34xSetDelaymS( ULONG iDelay )
{
	UCHAR mBuffer[CH341_PACKET_LENGTH];
	ULONG mLength;
	if( VenIC < 0x20 )
		return false;
	while( iDelay )
	{
		mLength = iDelay >= CH341A_CMD_I2C_STM_DLY ? CH341A_CMD_I2C_STM_DLY : iDelay;
		iDelay -= mLength;
		mBuffer[0] = CH341A_CMD_I2C_STREAM;
		mBuffer[1] = (UCHAR)( CH341A_CMD_I2C_STM_MS | mLength );
		mBuffer[2] = CH341A_CMD_I2C_STM_END;
		mLength = 3;
		if(( CH34xWriteData( mBuffer, &mLength )) == false )
			return false;
	}
	return true;
}

//This is needless for the moment
BOOL CH34xReadData( PVOID oBuffer, PULONG ioLength )
{
	UCHAR mBuffer[MAX_BUFFER_LENGTH];
	ULONG mLength;
}

/*
 * ********************************************************************
 * FUNCTION : Write Data ( for i2c/flash )
 * arg:
 * iBuffer : should Input  data buffer
 * ioLength : write length of data
 * ********************************************************************
 */
BOOL CH34xWriteData( PVOID iBuffer, PULONG ioLength )
{
	int retval = 0;
	int i=0;
	unsigned long mLen;
	struct{
		ULONG length;
		PUCHAR ByteBuffer;
		}Write;
	if( *ioLength > MAX_BUFFER_LENGTH )
		*ioLength = MAX_BUFFER_LENGTH;
	mLen = *ioLength;
	Write.length = ioLength;
	Write.ByteBuffer = (PUCHAR)malloc( sizeof( unsigned char ) * mLen );
	memcpy( Write.ByteBuffer, (PUCHAR)iBuffer, mLen );
	printf("ByteBuffer:");
	for(i=0;i<mLen;i++) printf("%.2x ",Write.ByteBuffer[i]);
	printf("Write.Lenth:%d \n",*((ULONG *)Write.length));
	retval = ioctl( dev_fd, CH34x_PIPE_DATA_DOWN, (unsigned long)&Write );
	if ( retval == -1 )
	{
		printf("error in pipe down\n");
		return false;	
	}	

	free( Write.ByteBuffer );
	return true;
}

/*
 * ********************************************************************
 * FUNCTION : Write/Read Data ( for i2c/flash )
 * arg:
 * iWriteLength : should write the length of data
 * iWriteBuffer : input buffer
 * oReadLength  : should read the length of data
 * oReadBuffer  : output buffer
 * ********************************************************************
 */
BOOL CH34xWriteRead( ULONG iWriteLength, PVOID iWriteBuffer,
		/*	ULONG iReadStep, ULONG iReadTimes,*/
			PULONG oReadLength, PVOID oReadBuffer)
{
	int retval = 0;
	ULONG mLength, mReadlen;
	ULONG iReadStep,iReadTimes;
	struct{
		ULONG oReadlen;
		PUCHAR iBuf;	
		PUCHAR oBuffer;
		ULONG oReturnlen;
		}Read;
	iReadStep =*(PUCHAR)(iWriteBuffer+iWriteLength-8);
	iReadTimes = *(PUCHAR)(iWriteBuffer+iWriteLength-4);
	mReadlen = iReadStep * iReadTimes;
	if( mReadlen == 0 )
		return false;
	mLength = max( iWriteLength, mReadlen );
#if 0
	printf("iWriteLength : %d\n",iWriteLength);
	printf("iReadTimes : %d\n",iReadTimes);
	printf("iReadStep : %d\n",iReadStep);
#endif
	Read.iBuf = (PUCHAR)iWriteBuffer;
	Read.oBuffer = (PUCHAR)oReadBuffer;
	Read.oReturnlen = oReadLength;
//	printf("iBuffer Addr is ------>:%p\n",Read.iBuf);
	Read.oReadlen = iWriteLength;
	retval = ioctl( dev_fd, CH34x_PIPE_WRITE_READ, (unsigned long)&Read );
	if( retval == -1 )
	{
		printf("Error in pipe write/read\n");
		return false;
	}	
	return true;
}


/*
 * ********************************************************************
 * FUNCTION : Set direction and output data of CH341 
 * arg:
 * Data :	Control direction and data 
 
 * iEnbale : set direction and data enable
 * 			   --> Bit16 High :	effect on Bit15~8 of iSetDataOut
 * 			   --> Bit17 High :	effect on Bit15~8 of iSetDirOut
 * 			   --> Bit18 High :	effect on Bit7~0 of iSetDataOut
 * 			   --> Bit19 High :	effect on Bit7~0 of iSetDirOut
 *			   --> Bit20 High :	effect on Bit23~16 of iSetDataOut
 * iSetDirOut : set io direction
 *			  -- > Bit High : Output 
 *			  -- > Bit Low : Input
 * iSetDataOut : set io data
 * 			 Output:
 *			  -- > Bit High : High level
 *			  -- > Bit Low : Low level
 * Note:
 * Bit7~Bit0<==>D7-D0 
 * Bit8<==>ERR#    Bit9<==>PEMP    Bit10<==>INT#    Bit11<==>SLCT    Bit13<==>WAIT#    Bit14<==>DATAS#/READ#    Bit15<==>ADDRS#/ADDR/ALE
 * The pins below can only be used in output mode:
 * Bit16<==>RESET#    Bit17<==>WRITE#    Bit18<==>SCL    Bit29<==>SDA
 * ********************************************************************
 */

BOOL CH34xSetOutput( ULONG	iEnable, ULONG iSetDirOut, ULONG iSetDataOut)
{
	ULONG mLength;
	UCHAR mBuffer[32];
	mBuffer[0] = CH341A_CMD_SET_OUTPUT;
	mBuffer[1] = 0x6A;
	mBuffer[2] = (UCHAR)( iEnable & 0x1F );
	mBuffer[3] = (UCHAR)( iSetDataOut >> 8 & 0xEF );
	mBuffer[4] = (UCHAR)( iSetDirOut >> 8 & 0xEF | 0x10 );
	mBuffer[5] = (UCHAR)( iSetDataOut & 0xFF );
	mBuffer[6] = (UCHAR)( iSetDirOut & 0xFF );
	mBuffer[7] = (UCHAR)( iSetDataOut >> 16 & 0x0F );
	mBuffer[8] = 0;
	mBuffer[9] = 0;
	mBuffer[10] = 0;
	mLength = 11;
	if ( CH34xWriteData(  mBuffer, &mLength ) ) {  //Write Data 
		if ( mLength >= 8 ) return( true);
	}
	return( false);
}


/*
 * ********************************************************************
 * FUNCTION : Set direction and output data of D5-D0 on CH341
 * arg:
 * Data : Control direction and data 
 * iSetDirOut : set io direction
 *			  -- > Bit High : Output 
 *			  -- > Bit Low : Input
 * iSetDataOut : set io data
 * 			 Output:
 *			  -- > Bit High : High level
 *			  -- > Bit Low : Low level
 * ********************************************************************
 */

BOOL CH34xSet_D5_D0( UCHAR iSetDirOut,UCHAR iSetDataOut)
{
	ULONG mLength;
	UCHAR mBuffer[32];
	mBuffer[0] = CH341A_CMD_UIO_STREAM;
	mBuffer[1] = (UCHAR)( CH341A_CMD_UIO_STM_OUT | iSetDataOut & 0x3F ); 
	mBuffer[2] = (UCHAR)( CH341A_CMD_UIO_STM_DIR | iSetDirOut & 0x3F );  
	mBuffer[3] = CH341A_CMD_UIO_STM_END;  
	mLength = 4;
	if ( CH34xWriteData( mBuffer, &mLength ) ) {  //Write Data 
		if ( mLength >= 3 ) return( true );
	}
	return( false);
}




/*
 * ********************************************************************
 * FUNCTION : Write/Read I2C Data Stream
 * This function issue a set of packets of iWriteBuffer data
 * arg:
 * iWriteLength : should write the length of data
 * iWriteBuffer : input buffer
 * oReadLength  : should read the length of data
 * oReadBuffer  : output buffer
 * ********************************************************************
 */
BOOL CH34xStreamI2C( ULONG iWriteLength, PVOID iWriteBuffer,
			ULONG iReadLength, PVOID oReadBuffer )
{
	UCHAR mBuffer[MAX_BUFFER_LENGTH];
	ULONG i, j, mLength;
	PUCHAR mWrBuf;

	if( VenIC < 0x20 )	
		return false;
	mLength = max( iWriteLength, iReadLength );
	if( mLength > MAX_BUFFER_LENGTH )
		return false;
	if( mLength <= DEFAULT_BUFFER_LEN )
		mWrBuf = (PVOID)mBuffer;
	else
	{
		mWrBuf = (PVOID)malloc( sizeof( UCHAR ) * MAX_BUFFER_LENGTH );
		if( mWrBuf == NULL)
			return false;
	}	
	i = 0;
	mWrBuf[i++] = CH341A_CMD_I2C_STREAM;
	if(( StreamMode & 0x03 ) == 0 )
	{
		mWrBuf[i++] = CH341A_CMD_I2C_STM_US | 10;
		mWrBuf[i++] = CH341A_CMD_I2C_STM_US | 10;
	}
	mWrBuf[i++] = CH341A_CMD_I2C_STM_STA;
	if( iWriteLength )
	{
		for( j = 0; j < iWriteLength; j++)
		{
			mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
			if( mLength <= 2 )
			{
				while( mLength-- )
					mWrBuf[i++] = CH341A_CMD_I2C_STM_END;
				mLength = CH341_PACKET_LENGTH;
			}
			if( mLength >= CH341_PACKET_LENGTH )
			{
				mWrBuf[i++] = CH341A_CMD_I2C_STREAM;
				mLength = CH341_PACKET_LENGTH - 1;

			}
			mLength--;
			mLength--;
			if( mLength > iWriteLength - j)
				mLength = iWriteLength - j;
			mWrBuf[i++] = (UCHAR)(CH341A_CMD_I2C_STM_OUT | mLength);
			while( mLength-- )
				mWrBuf[i++] = *((PUCHAR)iWriteBuffer + j++ );
		}

	}
	if( iReadLength )
	{
		mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
		if( mLength <= 3 )
		{
			while( mLength-- )
				mWrBuf[i++] = CH341A_CMD_I2C_STM_END;
			mLength = CH341_PACKET_LENGTH;
		}
		if( mLength >= CH341_PACKET_LENGTH )
			mWrBuf[i++] = CH341A_CMD_I2C_STREAM;
		if( iWriteLength > 1 )
		{
			mWrBuf[i++] = CH341A_CMD_I2C_STM_STA;
			mWrBuf[i++] = (UCHAR)(CH341A_CMD_I2C_STM_OUT | 1);
			mWrBuf[i++] = *(PUCHAR)iWriteBuffer | 0x01; 
		}
		else if( iWriteLength )
		{
			i--;
			mWrBuf[i++] = *(PUCHAR)iWriteBuffer | 0x01;
		}
		for( j = 1; j < iReadLength; )
		{
			mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;		
			if( mLength <= 1)
			{
				if( mLength )
					mWrBuf[i++] = CH341A_CMD_I2C_STM_END;
				mLength = CH341_PACKET_LENGTH;
			}
			if( mLength >= CH341_PACKET_LENGTH )
				mWrBuf[i++] = CH341A_CMD_I2C_STREAM;
			mLength = iReadLength - j >= CH341_PACKET_LENGTH ? CH341_PACKET_LENGTH : iReadLength - j;
			mWrBuf[i++] = (UCHAR)(CH341A_CMD_I2C_STM_IN | mLength);
			j += mLength;
			if( mLength >= CH341_PACKET_LENGTH )
			{
				mWrBuf[i] = CH341A_CMD_I2C_STM_END;
				i += CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
			}
		}
		mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
		if( mLength <= 1 )
		{
			if( mLength)
				mWrBuf[i++] = CH341A_CMD_I2C_STM_END;
			mLength = CH341_PACKET_LENGTH;
		}
		if( mLength >= CH341_PACKET_LENGTH )
			mWrBuf[i++] = CH341A_CMD_I2C_STREAM;
		mWrBuf[i++] = CH341A_CMD_I2C_STM_IN;
	}	
	mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
	if( mLength <= 1 )
	{
		if( mLength )
			mWrBuf[i++] = CH341A_CMD_I2C_STM_END;
		mLength = CH341_PACKET_LENGTH;
	}	
	if( mLength >= CH341_PACKET_LENGTH )
		mWrBuf[i++] = CH341A_CMD_I2C_STREAM;
	mWrBuf[i++] = CH341A_CMD_I2C_STM_STO;
	mWrBuf[i++] = CH341A_CMD_I2C_STM_END;
	mLength = 0;
	if( iReadLength )	
	{
		mWrBuf[i] = CH341_PACKET_LENGTH;
		mWrBuf[i+4] = (iReadLength + CH341_PACKET_LENGTH -1)/CH341_PACKET_LENGTH;
		i = i + 8;
	}
	if( iReadLength )
	{
		j = CH34xWriteRead( i, mWrBuf, &mLength, oReadBuffer );
		if( mLength != iReadLength )
		{
			printf("Return length is not equal to input length\n");
			j = false;
		}
	}
	else
		j = CH34xWriteData( mWrBuf, &i);

//	printf("Return mLength is %d\n", mLength);
	if( max( iWriteLength, iReadLength ) >= DEFAULT_BUFFER_LEN )
		free( mWrBuf );
	
	return ( j );
}

/*
 * ********************************************************************
 * FUNCTION : Read EEPROM Data (For I2C)
 * arg:
 * iEepromID 	: EEPROM TYPE
 * iAddr 		: the start addr for read
 * iLength  	: should read the length of data
 * oBuffer  	: output buffer
 * ********************************************************************
 */
BOOL CH34xReadEEPROM( EEPROM_TYPE iEepromID, ULONG iAddr,
			ULONG iLength, PUCHAR oBuffer )
{
	ULONG mLen;
	UCHAR mWrBuf[4];

	if( iEepromID >= ID_24C01 && iEepromID <= ID_24C16 )
	{
		while( iLength )
		{
			mWrBuf[0] = (UCHAR)( 0xA0 | (iAddr >> 7) & 0x0E );
			mWrBuf[1] = (UCHAR)iAddr;
			mLen = min( iLength, DEFAULT_BUFFER_LEN );
			if( !CH34xStreamI2C( 2, mWrBuf, mLen, oBuffer ))
				return false;
			iAddr += mLen;
			iLength -= mLen;
			oBuffer += mLen; 
		}
	}
	else if( iEepromID >= ID_24C32 && iEepromID <= ID_24C4096 )
	{
		
		while( iLength )
		{
			mWrBuf[0] = (UCHAR)( 0xA0 | (iAddr >> 15 ) & 0x0E );
			mWrBuf[1] = (UCHAR)( iAddr >> 8 );
			mWrBuf[2] = (UCHAR)iAddr;
			mLen = min( iLength, DEFAULT_BUFFER_LEN );
			if( !CH34xStreamI2C( 3, mWrBuf, mLen, oBuffer ))
				return false;
			iAddr += mLen;
			iLength -= mLen;
			oBuffer += mLen;
		}
	}
	else 
		return false;
	
	return true;
}

/*
 * ********************************************************************
 * FUNCTION : Write EEPROM Data (For I2C)
 * arg:
 * iEepromID 	: EEPROM TYPE
 * iAddr 		: the start addr for read
 * iLength  	: should write the length of data
 * iBuffer  	: Iutput buffer
 * ********************************************************************
 */

//Note : Sometimes you should set Delay time( CH34xSetDelaymS()) after
// CH34xStreamI2C()
BOOL CH34xWriteEEPROM( EEPROM_TYPE iEepromID, ULONG iAddr,
			ULONG iLength, PUCHAR iBuffer )
{
	ULONG mLen;
	UCHAR mWrBuf[256];
	if( iEepromID >= ID_24C01 && iEepromID <= ID_24C16 )
	{
		while( iLength )
		{
			mWrBuf[0] = (UCHAR)( 0xA0 | ( iAddr >>7 ) & 0x0E);
			mWrBuf[1] = (UCHAR)iAddr;
			mLen = iEepromID >= ID_24C04 ? 16 - ( iAddr & 15 ) : 8 - (iAddr & 7 );
			if( mLen > iLength )
				mLen = iLength;
			memcpy( &mWrBuf[2], iBuffer, mLen );
			if( !CH34xStreamI2C( 2 + mLen, mWrBuf, 0, NULL ))
				return false;	
			iAddr += mLen;
			iLength -= mLen;
			iBuffer += mLen; 
		}
	}
	else if( iEepromID >= ID_24C32 && iEepromID <= ID_24C4096 )
	{
		printf("Addr is %d\n",iAddr);
		printf("iLength is %d\n",iLength);
		printf("iBuffer is %x\n",*iBuffer);
		while( iLength )
		{
			mWrBuf[0] = (UCHAR)( 0xA0 | ( iAddr >>15 ) & 0x0E );
			mWrBuf[1] = (UCHAR)( iAddr >> 8 );
			mWrBuf[2] = (UCHAR)iAddr;
			mLen = iEepromID >= ID_24C512 ? 128 - ( iAddr & 127) : (iEepromID >= ID_24C128 ? 64 - ( iAddr & 63 ) : 32 - ( iAddr & 31));
			if( mLen > iLength )
				mLen = iLength;
			memcpy( &mWrBuf[3], iBuffer, mLen );
			printf("mWrBuf[3] is %x\n",mWrBuf[3]);
			if( !CH34xStreamI2C( 3 + mLen, mWrBuf, 0, NULL ))
				return false;
			iAddr += mLen;
			iLength -= mLen;
			iBuffer += mLen;

		}
	}
	else
		return false;

	return true;

}

//Used for high bit transform
const	UCHAR	mMsbTable[256] = { 0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 
				0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,		// 0XH
				0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 
				0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,		// 1XH
			        0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 
				0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,		// 2XH
				0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 
				0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,		// 3XH
				0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 
				0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,		// 4XH
				0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 
				0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,		// 5XH
				0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 
				0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,		// 6XH
				0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 
				0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,		// 7XH
				0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 
				0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,		// 8XH
				0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 
				0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,		// 9XH
				0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 
				0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,		// AXH
				0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 
				0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,		// BXH
				0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 
				0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,		// CXH
				0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 
				0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,		// DXH
				0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 
				0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,		// EXH
				0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 
				0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF };	// FXH

/*
 * ********************************************************************
 * FUNCTION : Write/Read Flash Data (For SPI)
 * arg:
 * iChipselect 	: Flash cs enable
 * iLength 		: the length of data
 * ioBuffer  	: one in one out buffer
 * ioBuffer2  	: two in two out buffer
 * ********************************************************************
 */
BOOL CH34xStreamSPIx( ULONG iChipselect, ULONG iLength,
			PVOID ioBuffer, PVOID ioBuffer2 )
{
	UCHAR mBuffer[DEFAULT_BUFFER_LEN];
	ULONG i, j, mLength, mSelect, mCount,mReturn;
	PUCHAR mWrBuf;
	UCHAR c1, c2;
	mReturn = iLength;
	if( VenIC < 0x20 )
	{
		printf("Not support this device\n");
		return false;
	}
	if( iLength <= DEFAULT_BUFFER_LEN/2 )
	{
		mWrBuf = (PVOID)mBuffer;
		i = DEFAULT_BUFFER_LEN;
	}
	else
	{
		mWrBuf = (PVOID)malloc( sizeof( UCHAR ) * MAX_BUFFER_LENGTH );
		if( mWrBuf == NULL )
			return false;
		i = MAX_BUFFER_LENGTH;
	}
	memset( mWrBuf, 0, sizeof( UCHAR ) * i );	
	i = 0;
	if( iChipselect & 0x80 )
	{
		mWrBuf[i++] = CH341A_CMD_UIO_STREAM;
		switch( iChipselect & 0x03 )
		{
			case 0x00:		//DCK/D3->0, D0 ->0
				mSelect = 0x36;	
				break;
			case 0x01:		//DCK/D3->0, D1 ->0
				mSelect = 0x35;
				break;
			case 0x02:		//DCK/D3->0, D2 ->0
				mSelect = 0x33;
				break;
			default:		//DCK/D3->0, D4 ->0
				mSelect = 0x27;
				break;
		}
	
		mWrBuf[i++] = (UCHAR)( CH341A_CMD_UIO_STM_OUT | mSelect );// Output data
		mWrBuf[i++] = (UCHAR)( CH341A_CMD_UIO_STM_DIR | 0x3F ); // set D5~D0 direct
		mWrBuf[i++] = CH341A_CMD_UIO_STM_END;
		i = CH341_PACKET_LENGTH;
	}	
	if( iLength )
	{
		if( (ULONG)ioBuffer2 < 4 )
		{
			for( j = 0; j < iLength; )
			{
				mLength = CH341_PACKET_LENGTH - 1;
				if( mLength > iLength - j )
					mLength = iLength - j;
				mWrBuf[i++] = CH341A_CMD_SPI_STREAM;	
				if( StreamMode & 0x80 )
				{
					while( mLength-- )
						mWrBuf[i++] = mMsbTable[*((PUCHAR)ioBuffer + j++)];		
				}	
				else
					while( mLength-- )
						mWrBuf[i++] = *((PUCHAR)ioBuffer + j++);
				if( VenIC == 0x20 && i % CH341_PACKET_LENGTH == 0 )
				{
					mWrBuf[i] = mWrBuf[i+1] = 0;
					i += CH341_PACKET_LENGTH;
				}
			}
		}
		else
		{
			for( j = 0; j < iLength; )
			{
				mLength = CH341_PACKET_LENGTH - 1;
				if( mLength > iLength - j )
					mLength = iLength - j;
				mWrBuf[i++] = CH341A_CMD_SPI_STREAM;
				if( StreamMode & 0x80 )	
				{
					while( mLength-- )
					{
						c1 = mMsbTable[*((PUCHAR)ioBuffer + (j >> 1))];
						c2 = mMsbTable[*((PUCHAR)ioBuffer2 + (j >> 1))];
						mWrBuf[i++] = (UCHAR)((j & 0x01)?(c1 & 0xF0 | c2 >> 4 & 0x0F ):(c1 << 4 & 0xF0 | c2 & 0x0F));
						j++;
					}
				}	
				else
				{
					while( mLength-- )
					{
						c1 = *((PUCHAR)ioBuffer + (j >> 1));
						c2 = *((PUCHAR)ioBuffer + (j >> 1));
						mWrBuf[i++] = (UCHAR)((j & 0x01)?(c1 & 0xF0 | c2 >> 4 & 0x0F ):(c1 << 4 & 0xF0 | c2 & 0x0F));
						j++;
					}
				}
				if( VenIC == 0x20 && i % CH341_PACKET_LENGTH == 0 )
				{
					mWrBuf[i] = mWrBuf[i+1] = 0;
					i += CH341_PACKET_LENGTH;
				}
			}
		}
	}
	
	mLength = 0;
	mWrBuf[i] = CH341_PACKET_LENGTH - 1;
	mWrBuf[i+4] = (iLength + CH341_PACKET_LENGTH -1 - 1)/(CH341_PACKET_LENGTH - 1);
	i = i + 8;
	j = CH34xWriteRead( i, mWrBuf, &mLength,((ULONG)ioBuffer2 < 4?ioBuffer:mWrBuf));
//	if( j && mLength != iLength )
//		j = false;

// high bit mode transform normal display mode
	if( iChipselect & 0x80 )
	{
		mBuffer[0] = CH341A_CMD_UIO_STREAM;
		mBuffer[1] = (UCHAR)(CH341A_CMD_UIO_STM_OUT | 0x37);
		mBuffer[2] = CH341A_CMD_UIO_STM_END;
		mLength = 3;
		if( CH34xWriteData( mBuffer, &mLength ))
		{
			if( mLength < 2 )
			{
				printf("Error in SPI......");
				return false;
			}	
		}
		else
			return false;
	}
	if( j )
	{
		if((ULONG)ioBuffer2 < 4 )
		{
			if( StreamMode & 0x80 )
			{
				for( i = 0; i < iLength; i++ )
					*((PUCHAR)ioBuffer + i) = mMsbTable[*((PUCHAR)ioBuffer + i)];
			}
		}
		else
		{
			for( i = 0; i < iLength; i += 2 )
			{
				c1 = mWrBuf[i];
				c2 = mWrBuf[i+1];
				if( StreamMode & 0x80 )
				{
					*((PUCHAR)ioBuffer + (i >> 1)) = mMsbTable[c1 >> 4 & 0xF0 | c2 & 0xF0];
					*((PUCHAR)ioBuffer2 + (i >> 1)) = mMsbTable[c1 & 0x0F | c2 << 4 & 0xF0];
				}
				else
				{
					*((PUCHAR)ioBuffer + (i >> 1)) = (UCHAR)(c1 >> 4 & 0x0F | c2 & 0xF0 );
					*((PUCHAR)ioBuffer + (i >> 1)) = (UCHAR)(c1 & 0x0F | c2 << 4 & 0xF0);
				}
			}
		}
	}
	if( mReturn > DEFAULT_BUFFER_LEN )
		free( mWrBuf );
	return ( j );
}

/*
 * ********************************************************************
 * FUNCTION : Write/Read Flash Data (For one in one out SPI)
 * arg:
 * iChipselect 	: Flash cs enable
 * iLength 		: the length of data
 * ioBuffer  	: one in one out buffer
 * ********************************************************************
 */
// Note : StreamMode must set first
BOOL CH34xStreamSPI4( ULONG iChipSelect, ULONG iLength, PVOID ioBuffer )
{
	int retval = 0;
	if( VenIC >= 0x25 && VenIC < 0x30 )
		return false;
	StreamMode = 0x81;
	if( StreamMode & 0x04 )
	{
		if( CH34xSetStream( StreamMode & 0xFB ) == false )
			return false;
	}	
	retval = CH34xStreamSPIx( iChipSelect, iLength, ioBuffer, (PVOID)1 );
	
	return retval;		
}


