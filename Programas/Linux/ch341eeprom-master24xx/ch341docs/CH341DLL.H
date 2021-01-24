//****************************************
//**  Copyright  (C)  W.ch  1999-2005   **
//**  Web:  http://www.winchiphead.com  **
//****************************************
//**  DLL for USB interface chip CH341  **
//**  C, VC5.0                          **
//****************************************
//
// CH341-DLL  V1.9
// support USB chip: CH341, CH341A

#ifndef		_CH341_DLL_H
#define		_CH341_DLL_H

#ifdef __cplusplus                              
extern "C" {
#endif

#define		mOFFSET( s, m )			( (ULONG) & ( ( ( s * ) 0 ) -> m ) )

#ifndef		max
#define		max( a, b )				( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif

#ifndef		min
#define		min( a, b )				( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

#ifdef		ExAllocatePool
#undef		ExAllocatePool
#endif

#ifndef		NTSTATUS
typedef		LONG	NTSTATUS;
#endif


typedef	struct	_USB_SETUP_PKT {
	UCHAR			mUspReqType;
	UCHAR			mUspRequest;
	union	{
		struct	{
			UCHAR	mUspValueLow;
			UCHAR	mUspValueHigh;
		};
		USHORT		mUspValue;
	};
	union	{
		struct	{
			UCHAR	mUspIndexLow;
			UCHAR	mUspIndexHigh;
		};
		USHORT		mUspIndex;
	};
	USHORT			mLength;
} mUSB_SETUP_PKT, *mPUSB_SETUP_PKT;


#define		mCH341_PACKET_LENGTH	32
#define		mCH341_PKT_LEN_SHORT	8


typedef	struct	_WIN32_COMMAND {
	union	{
		ULONG		mFunction;
		NTSTATUS	mStatus;
	};
	ULONG			mLength;
	union	{
		mUSB_SETUP_PKT	mSetupPkt;
		UCHAR			mBuffer[ mCH341_PACKET_LENGTH ];
	};
} mWIN32_COMMAND, *mPWIN32_COMMAND;



#define		IOCTL_CH341_COMMAND		( FILE_DEVICE_UNKNOWN << 16 | FILE_ANY_ACCESS << 14 | 0x0f34 << 2 | METHOD_BUFFERED )
#define		mWIN32_COMMAND_HEAD		mOFFSET( mWIN32_COMMAND, mBuffer )
#define		mCH341_MAX_NUMBER		16
#define		mMAX_BUFFER_LENGTH		0x1000
#define		mMAX_COMMAND_LENGTH		( mWIN32_COMMAND_HEAD + mMAX_BUFFER_LENGTH )
#define		mDEFAULT_BUFFER_LEN		0x0400
#define		mDEFAULT_COMMAND_LEN	( mWIN32_COMMAND_HEAD + mDEFAULT_BUFFER_LEN )

#define		mCH341_ENDP_INTER_UP	0x81
#define		mCH341_ENDP_INTER_DOWN	0x01
#define		mCH341_ENDP_DATA_UP		0x82
#define		mCH341_ENDP_DATA_DOWN	0x02

#define		mPipeDeviceCtrl			0x00000004
#define		mPipeInterUp			0x00000005
#define		mPipeDataUp				0x00000006
#define		mPipeDataDown			0x00000007

#define		mFuncNoOperation		0x00000000
#define		mFuncGetVersion			0x00000001
#define		mFuncGetConfig			0x00000002
#define		mFuncSetTimeout			0x00000009
#define		mFuncSetExclusive		0x0000000b
#define		mFuncResetDevice		0x0000000c
#define		mFuncResetPipe			0x0000000d
#define		mFuncAbortPipe			0x0000000e

#define		mFuncSetParaMode		0x0000000f
#define		mFuncReadData0			0x00000010
#define		mFuncReadData1			0x00000011
#define		mFuncWriteData0			0x00000012
#define		mFuncWriteData1			0x00000013
#define		mFuncWriteRead			0x00000014
#define		mFuncBufferMode			0x00000020
#define		mFuncBufferModeDn		0x00000021

#define		mUSB_CLR_FEATURE		0x01
#define		mUSB_SET_FEATURE		0x03
#define		mUSB_GET_STATUS			0x00
#define		mUSB_SET_ADDRESS		0x05
#define		mUSB_GET_DESCR			0x06
#define		mUSB_SET_DESCR			0x07
#define		mUSB_GET_CONFIG			0x08
#define		mUSB_SET_CONFIG			0x09
#define		mUSB_GET_INTERF			0x0a
#define		mUSB_SET_INTERF			0x0b
#define		mUSB_SYNC_FRAME			0x0c

#define		mCH341_VENDOR_READ		0xC0
#define		mCH341_VENDOR_WRITE		0x40

#define		mCH341_PARA_INIT		0xB1
#define		mCH341_I2C_STATUS		0x52
#define		mCH341_I2C_COMMAND		0x53

#define		mCH341_PARA_CMD_R0		0xAC
#define		mCH341_PARA_CMD_R1		0xAD
#define		mCH341_PARA_CMD_W0		0xA6
#define		mCH341_PARA_CMD_W1		0xA7
#define		mCH341_PARA_CMD_STS		0xA0

#define		mCH341A_CMD_SET_OUTPUT	0xA1
#define		mCH341A_CMD_IO_ADDR		0xA2
#define		mCH341A_CMD_PRINT_OUT	0xA3
#define		mCH341A_CMD_SPI_STREAM	0xA8
#define		mCH341A_CMD_SIO_STREAM	0xA9
#define		mCH341A_CMD_I2C_STREAM	0xAA
#define		mCH341A_CMD_UIO_STREAM	0xAB

#define		mCH341A_BUF_CLEAR		0xB2
#define		mCH341A_I2C_CMD_X		0x54
#define		mCH341A_DELAY_MS		0x5E
#define		mCH341A_GET_VER			0x5F

#define		mCH341_EPP_IO_MAX		( mCH341_PACKET_LENGTH - 1 )
#define		mCH341A_EPP_IO_MAX		0xFF

#define		mCH341A_CMD_IO_ADDR_W	0x00
#define		mCH341A_CMD_IO_ADDR_R	0x80

#define		mCH341A_CMD_I2C_STM_STA	0x74
#define		mCH341A_CMD_I2C_STM_STO	0x75
#define		mCH341A_CMD_I2C_STM_OUT	0x80
#define		mCH341A_CMD_I2C_STM_IN	0xC0
#define		mCH341A_CMD_I2C_STM_MAX	( min( 0x3F, mCH341_PACKET_LENGTH ) )
#define		mCH341A_CMD_I2C_STM_SET	0x60
#define		mCH341A_CMD_I2C_STM_US	0x40
#define		mCH341A_CMD_I2C_STM_MS	0x50
#define		mCH341A_CMD_I2C_STM_DLY	0x0F
#define		mCH341A_CMD_I2C_STM_END	0x00

#define		mCH341A_CMD_UIO_STM_IN	0x00
#define		mCH341A_CMD_UIO_STM_DIR	0x40
#define		mCH341A_CMD_UIO_STM_OUT	0x80
#define		mCH341A_CMD_UIO_STM_US	0xC0
#define		mCH341A_CMD_UIO_STM_END	0x20

#define		mCH341_PARA_MODE_EPP	0x00
#define		mCH341_PARA_MODE_EPP17	0x00
#define		mCH341_PARA_MODE_EPP19	0x01
#define		mCH341_PARA_MODE_MEM	0x02

#define		mStateBitERR			0x00000100
#define		mStateBitPEMP			0x00000200
#define		mStateBitINT			0x00000400
#define		mStateBitSLCT			0x00000800
#define		mStateBitWAIT			0x00002000
#define		mStateBitDATAS			0x00004000
#define		mStateBitADDRS			0x00008000
#define		mStateBitRESET			0x00010000
#define		mStateBitWRITE			0x00020000
#define		mStateBitSDA			0x00800000

#define		MAX_DEVICE_PATH_SIZE	128
#define		MAX_DEVICE_ID_SIZE		64


typedef		VOID	( CALLBACK	* mPCH341_INT_ROUTINE ) ( ULONG	iStatus );

HANDLE	WINAPI	CH341OpenDevice(ULONG iIndex );
VOID	WINAPI	CH341CloseDevice(ULONG iIndex );
ULONG	WINAPI	CH341GetVersion( );

ULONG	WINAPI	CH341DriverCommand( ULONG iIndex, mPWIN32_COMMAND ioCommand );
ULONG	WINAPI	CH341GetDrvVersion();
BOOL	WINAPI	CH341ResetDevice( ULONG iIndex );
BOOL	WINAPI	CH341GetDeviceDescr(ULONG iIndex, PVOID	oBuffer, PULONG	ioLength );
BOOL	WINAPI	CH341GetConfigDescr( ULONG iIndex, PVOID oBuffer, PULONG ioLength );
BOOL	WINAPI	CH341SetIntRoutine( ULONG iIndex, mPCH341_INT_ROUTINE iIntRoutine );
BOOL	WINAPI	CH341ReadInter(	ULONG iIndex, PULONG iStatus );
BOOL	WINAPI	CH341AbortInter( ULONG iIndex );
BOOL	WINAPI	CH341SetParaMode( ULONG	iIndex, ULONG iMode );
BOOL	WINAPI	CH341InitParallel( ULONG iIndex, ULONG iMode );
BOOL	WINAPI	CH341ReadData0( ULONG iIndex, PVOID	oBuffer, PULONG ioLength );
BOOL	WINAPI	CH341ReadData1(	ULONG iIndex, PVOID	oBuffer, PULONG	ioLength );
BOOL	WINAPI	CH341AbortRead(	ULONG iIndex );
BOOL	WINAPI	CH341WriteData0( ULONG iIndex, PVOID iBuffer, PULONG ioLength );
BOOL	WINAPI	CH341WriteData1( ULONG iIndex, PVOID iBuffer, PULONG ioLength );
BOOL	WINAPI	CH341AbortWrite( ULONG iIndex );

BOOL	WINAPI	CH341GetStatus( ULONG iIndex, PULONG iStatus );
BOOL	WINAPI	CH341ReadI2C( ULONG	iIndex, UCHAR iDevice, UCHAR iAddr, PUCHAR oByte );
BOOL	WINAPI	CH341WriteI2C( ULONG iIndex, UCHAR iDevice, UCHAR iAddr, UCHAR iByte );

BOOL	WINAPI	CH341EppReadData( ULONG	iIndex, PVOID oBuffer, PULONG ioLength );
BOOL	WINAPI	CH341EppReadAddr( ULONG	iIndex, PVOID oBuffer, PULONG ioLength );
BOOL	WINAPI	CH341EppWriteData( ULONG iIndex, PVOID iBuffer, PULONG ioLength );
BOOL	WINAPI	CH341EppWriteAddr( ULONG iIndex, PVOID iBuffer, PULONG ioLength );
BOOL	WINAPI	CH341EppSetAddr( ULONG iIndex, UCHAR iAddr );

BOOL	WINAPI	CH341MemReadAddr0( ULONG iIndex, PVOID oBuffer, PULONG ioLength );
BOOL	WINAPI	CH341MemReadAddr1( ULONG iIndex, PVOID oBuffer, PULONG ioLength );
BOOL	WINAPI	CH341MemWriteAddr0( ULONG iIndex, PVOID	iBuffer, PULONG	ioLength );
BOOL	WINAPI	CH341MemWriteAddr1( ULONG iIndex, PVOID	iBuffer, PULONG	ioLength );

BOOL	WINAPI	CH341SetExclusive( ULONG iIndex, ULONG iExclusive );
BOOL	WINAPI	CH341SetTimeout( ULONG iIndex, ULONG iWriteTimeout,	ULONG iReadTimeout );
BOOL	WINAPI	CH341ReadData( ULONG iIndex, PVOID oBuffer, PULONG ioLength );
BOOL	WINAPI	CH341WriteData(	ULONG iIndex, PVOID	iBuffer, PULONG	ioLength );
PVOID	WINAPI	CH341GetDeviceName( ULONG iIndex );
ULONG	WINAPI	CH341GetVerIC( ULONG iIndex );
BOOL	WINAPI	CH341FlushBuffer( ULONG	iIndex );
BOOL	WINAPI	CH341WriteRead(	ULONG iIndex, ULONG	iWriteLength, PVOID	iWriteBuffer, ULONG	iReadStep, ULONG iReadTimes, PULONG	oReadLength, PVOID	oReadBuffer );
BOOL	WINAPI	CH341SetStream( ULONG iIndex, ULONG	iMode );
BOOL	WINAPI	CH341SetDelaymS( ULONG iIndex, ULONG iDelay );
BOOL	WINAPI	CH341StreamI2C( ULONG iIndex, ULONG	iWriteLength, PVOID	iWriteBuffer, ULONG	iReadLength, PVOID oReadBuffer );

typedef	enum _EEPROM_TYPE {
	ID_24C01,
	ID_24C02,
	ID_24C04,
	ID_24C08,
	ID_24C16,
	ID_24C32,
	ID_24C64,
	ID_24C128,
	ID_24C256,
	ID_24C512,
	ID_24C1024,
	ID_24C2048,
	ID_24C4096
} EEPROM_TYPE;


BOOL	WINAPI	CH341ReadEEPROM( ULONG iIndex, EEPROM_TYPE iEepromID, ULONG	iAddr, ULONG iLength, PUCHAR oBuffer );
BOOL	WINAPI	CH341WriteEEPROM( ULONG	iIndex, EEPROM_TYPE	iEepromID, ULONG iAddr, ULONG iLength, PUCHAR iBuffer );
BOOL	WINAPI	CH341GetInput( ULONG iIndex, PULONG	iStatus );
BOOL	WINAPI	CH341SetOutput( ULONG iIndex, ULONG	iEnable, ULONG iSetDirOut, ULONG iSetDataOut );
BOOL	WINAPI	CH341Set_D5_D0(	ULONG iIndex, ULONG	iSetDirOut, ULONG iSetDataOut );
BOOL	WINAPI	CH341StreamSPI3( ULONG iIndex, ULONG iChipSelect, ULONG	iLength, PVOID ioBuffer );
BOOL	WINAPI	CH341StreamSPI4( ULONG iIndex, ULONG iChipSelect, ULONG	iLength, PVOID ioBuffer );
BOOL	WINAPI	CH341StreamSPI5( ULONG iIndex, ULONG iChipSelect, ULONG	iLength, PVOID ioBuffer, PVOID ioBuffer2 );
BOOL	WINAPI	CH341BitStreamSPI( ULONG iIndex, ULONG iLength, PVOID ioBuffer );
BOOL	WINAPI	CH341SetBufUpload( ULONG iIndex, ULONG iEnableOrClear );
LONG	WINAPI	CH341QueryBufUpload( ULONG iIndex );
BOOL	WINAPI	CH341SetBufDownload( ULONG iIndex, ULONG iEnableOrClear );
LONG	WINAPI	CH341QueryBufDownload( ULONG iIndex );
BOOL	WINAPI	CH341ResetInter( ULONG iIndex );
BOOL	WINAPI	CH341ResetRead( ULONG iIndex );
BOOL	WINAPI	CH341ResetWrite( ULONG iIndex );

typedef		VOID	( CALLBACK	* mPCH341_NOTIFY_ROUTINE ) ( ULONG iEventStatus );

#define		CH341_DEVICE_ARRIVAL		3
#define		CH341_DEVICE_REMOVE_PEND	1
#define		CH341_DEVICE_REMOVE			0

BOOL	WINAPI	CH341SetDeviceNotify( ULONG	iIndex, PCHAR iDeviceID, mPCH341_NOTIFY_ROUTINE	iNotifyRoutine );
BOOL	WINAPI	CH341SetupSerial( ULONG	iIndex, ULONG iParityMode, ULONG iBaudRate );

#ifdef __cplusplus
}
#endif

#endif		// _CH341_DLL_H
