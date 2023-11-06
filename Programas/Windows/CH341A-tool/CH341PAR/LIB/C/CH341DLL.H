// 2004.05.28, 2004.10.20, 2005.01.08, 2005.03.25, 2005.04.28, 2005.07.18, 2005.07.28, 2005.09.19, 2007.12.19, 2008.10.15
//****************************************
//**  Copyright  (C)  W.ch  1999-2008   **
//**  Web:  http://www.winchiphead.com  **
//****************************************
//**  DLL for USB interface chip CH341  **
//**  C, VC5.0                          **
//****************************************
//
// Translated from Chinese using google translate, some extra comments are added
//
// USB bus interface chip CH341 parallel port application layer interface library V2.1
// Nanjing Qinheng Electronics Co., Ltd. Author: W.ch 2008.10
// CH341-DLL V2.1
// Operating environment: Windows 98/ME, Windows 2000/XP
// support USB chip: CH341, CH341A
// USB => Parallel, I2C, SPI, JTAG ...
//

#ifndef		_CH341_DLL_H
#define		_CH341_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

#define		mOFFSET( s, m )			( (ULONG) & ( ( ( s * ) 0 ) -> m ) )	// get relative offset address of a structure member

#ifndef		min
#define		min( a, b )				( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

#ifdef		ExAllocatePool
#undef		ExAllocatePool						// Delete memory allocation with TAG
#endif

#ifndef		NTSTATUS
typedef		LONG	NTSTATUS;
#endif


typedef	struct	_USB_SETUP_PKT {				// Data request packet structure in the establishment phase of USB control transfer
	UCHAR			mUspReqType;				// 00H request type
	UCHAR			mUspRequest;				// 01H request code
	union	{
		struct	{
			UCHAR	mUspValueLow;				// 02H value parameter low byte
			UCHAR	mUspValueHigh;				// 03H value parameter high byte
		};
		USHORT		mUspValue;					// 02H-03H value parameters
	};
	union	{
		struct	{
			UCHAR	mUspIndexLow;				// 04H Index parameter low byte
			UCHAR	mUspIndexHigh;				// 05H Index parameter high byte
		};
		USHORT		mUspIndex;					// 04H-05H index parameters
	};
	USHORT			mLength;					// 06H-07H Data length of data stage
} mUSB_SETUP_PKT, *mPUSB_SETUP_PKT;


#define		mCH341_PACKET_LENGTH	32			// The length of data packets supported by CH341
#define		mCH341_PKT_LEN_SHORT	8			// The length of short data packets supported by CH341


typedef	struct	_WIN32_COMMAND {				// Define the WIN32 command interface structure
	union	{
		ULONG		mFunction;					// Specify the function code or pipe number when entering
		NTSTATUS	mStatus;					// Return to operating status when outputting
	};
	ULONG			mLength;					// Access length, returns the length of subsequent data
	union	{
		mUSB_SETUP_PKT	mSetupPkt;				// Data request during the establishment phase of USB control transfer
		UCHAR			mBuffer[ mCH341_PACKET_LENGTH ];	// Data buffer, length 0 to 255B - nonsense?
	};
} mWIN32_COMMAND, *mPWIN32_COMMAND;


// WIN32 application layer interface commands
#define		IOCTL_CH341_COMMAND		( FILE_DEVICE_UNKNOWN << 16 | FILE_ANY_ACCESS << 14 | 0x0f34 << 2 | METHOD_BUFFERED )	// dedicated interface

#define		mWIN32_COMMAND_HEAD		mOFFSET( mWIN32_COMMAND, mBuffer )	// Header length of WIN32 command interface

#define		mCH341_MAX_NUMBER		16			// Maximum number of CH341s connected simultaneously

#define		mMAX_BUFFER_LENGTH		0x1000		// The maximum length of the data buffer is 4096

#define		mMAX_COMMAND_LENGTH		( mWIN32_COMMAND_HEAD + mMAX_BUFFER_LENGTH )	// The maximum data length plus the length of the command structure header

#define		mDEFAULT_BUFFER_LEN		0x0400		// Data buffer default length 1024

#define		mDEFAULT_COMMAND_LEN	( mWIN32_COMMAND_HEAD + mDEFAULT_BUFFER_LEN )	// The default data length plus the length of the command structure header


// CH341 endpoint address
#define		mCH341_ENDP_INTER_UP	0x81		// The address of the interrupt data upload endpoint of CH341
#define		mCH341_ENDP_INTER_DOWN	0x01		// The address of the interrupt data download endpoint of CH341
#define		mCH341_ENDP_DATA_UP		0x82		// The address of the data block upload endpoint of CH341
#define		mCH341_ENDP_DATA_DOWN	0x02		// The address of the data block download endpoint of CH341


// Pipeline operation commands provided by the device layer interface
#define		mPipeDeviceCtrl			0x00000004	// Integrated control pipeline of CH341
#define		mPipeInterUp			0x00000005	// CH341 interrupt data upload pipeline
#define		mPipeDataUp				0x00000006	// CH341 data block upload pipeline
#define		mPipeDataDown			0x00000007	// CH341 data block download pipeline

// Function code of application layer interface
#define		mFuncNoOperation		0x00000000	// No operation
#define		mFuncGetVersion			0x00000001	// Get the driver version number
#define		mFuncGetConfig			0x00000002	// Get the USB device configuration descriptor
#define		mFuncSetTimeout			0x00000009	// Set USB communication timeout
#define		mFuncSetExclusive		0x0000000b	// Set exclusive use
#define		mFuncResetDevice		0x0000000c	// Reset USB device
#define		mFuncResetPipe			0x0000000d	// Reset USB pipe
#define		mFuncAbortPipe			0x0000000e	// Cancel the data request of the USB pipe

// CH341 parallel port dedicated function code
#define		mFuncSetParaMode		0x0000000f	// Set parallel port mode
#define		mFuncReadData0			0x00000010	// Read data block 0 from the parallel port
#define		mFuncReadData1			0x00000011	// Read data block 1 from the parallel port
#define		mFuncWriteData0			0x00000012	// Write data block 0 to the parallel port
#define		mFuncWriteData1			0x00000013	// Write data block 1 to the parallel port
#define		mFuncWriteRead			0x00000014	// Output first and then input
#define		mFuncBufferMode			0x00000020	// Set the buffer upload mode and query the data length in the buffer
#define		mFuncBufferModeDn		0x00000021	// Set the buffer download mode and query the data length in the buffer


// USB device standard request code
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

// Manufacturer-specific request type for CH341 control transmission
#define		mCH341_VENDOR_READ		0xC0		// CH341 manufacturer-specific read operation implemented through control transmission
#define		mCH341_VENDOR_WRITE		0x40		// CH341 manufacturer-specific write operation implemented through control transmission

// Manufacturer-specific request code for CH341 control transmission
#define		mCH341_PARA_INIT		0xB1		// Initialize parallel port
#define		mCH341_I2C_STATUS		0x52		// Get the status of the I2C interface
#define		mCH341_I2C_COMMAND		0x53		// Issue the command of the I2C interface

// CH341 parallel port operation command code
#define		mCH341_PARA_CMD_R0		0xAC		// Read data 0 from the parallel port, the second byte is the length
#define		mCH341_PARA_CMD_R1		0xAD		// Read data 1 from the parallel port, the second byte is the length
#define		mCH341_PARA_CMD_W0		0xA6		// Write data 0 to the parallel port, starting from the sub-byte as the data stream
#define		mCH341_PARA_CMD_W1		0xA7		// Write data 1 to the parallel port, starting from the sub-byte as the data stream
#define		mCH341_PARA_CMD_STS		0xA0		// Get parallel port status

// CH341A parallel port operation command code
#define		mCH341A_CMD_SET_OUTPUT	0xA1		// Set parallel port outpu
#define		mCH341A_CMD_IO_ADDR		0xA2		// MEM with address read and write/input and output, starting from the sub-byte is the command stream
#define		mCH341A_CMD_PRINT_OUT	0xA3		// PRINT is compatible with printing mode output, starting from the sub-byte as the data stream
#define		mCH341A_CMD_PWM_OUT		0xA4		// Command package for PWM data output, starting from the sub-byte for the data stream
#define		mCH341A_CMD_SHORT_PKT	0xA5		// Short packet, the second byte is the real length of the command packet, the second byte and the following bytes are the original command packet
#define		mCH341A_CMD_SPI_STREAM	0xA8		// Command package of SPI interface, starting from the sub-byte is the data stream
//#define		mCH341A_CMD_SIO_STREAM	0xA9		// The command package of the SIO interface, starting from the sub-byte is the data stream
#define		mCH341A_CMD_I2C_STREAM	0xAA		// Command package of I2C interface, starting from the sub-byte is the I2C command stream
#define		mCH341A_CMD_UIO_STREAM	0xAB		// Command package of UIO interface, starting from the sub-byte is the command stream
#define		mCH341A_CMD_PIO_STREAM	0xAE		// Command package of PIO interface, data stream starting from sub-byte

// Manufacturer-specific request code for CH341A control transmission
#define		mCH341A_BUF_CLEAR		0xB2		// Clear unfinished data
#define		mCH341A_I2C_CMD_X		0x54		// Issue the I2C interface command and execute it immediately
#define		mCH341A_DELAY_MS		0x5E		// Delay the specified time in units of milliseconds
#define		mCH341A_GET_VER			0x5F		// Get the chip version

#define		mCH341_EPP_IO_MAX		( mCH341_PACKET_LENGTH - 1 )	// The maximum length of a single read and write data block of CH341 in EPP/MEM mode
#define		mCH341A_EPP_IO_MAX		0xFF		// The maximum length of a single read and write data block of CH341A in EPP/MEM mode

#define		mCH341A_CMD_IO_ADDR_W	0x00		// MEM command flow with address read and write/input and output: write data, bit 6-bit 0 is the address, the next byte is the data to be written
#define		mCH341A_CMD_IO_ADDR_R	0x80		// MEM command flow with address read and write/input and output: read data, bit 6-bit 0 is the address, read the data and return together

#define		mCH341A_CMD_I2C_STM_STA	0x74		// Command flow of I2C interface: generate start bit
#define		mCH341A_CMD_I2C_STM_STO	0x75		// Command flow of I2C interface: generate stop bit
#define		mCH341A_CMD_I2C_STM_OUT	0x80		// Command flow of I2C interface: output data, bits 5...0 is the length, the subsequent bytes are data, if the length is 0, only one byte is sent and the response is returned
#define		mCH341A_CMD_I2C_STM_IN	0xC0		// Command flow of I2C interface: input data, bits 5...0 is the length, if the length is 0, only one byte will be received and no response will be sent
#define		mCH341A_CMD_I2C_STM_MAX	( min( 0x3F, mCH341_PACKET_LENGTH ) )	// The maximum length of the input and output data of a single command in the command stream of the I2C interface
#define		mCH341A_CMD_I2C_STM_SET	0x60		// Command flow of I2C interface: Set parameters, bit 2=number of SPI I/Os (0=single input, single output, 1=dual input, double output), bit 1, bit 0=I2C speed (00= Low speed, 01=standard, 10=fast, 11=high speed)
#define		mCH341A_CMD_I2C_STM_US	0x40		// Command flow of I2C interface: delay in microseconds, bit 3-bit 0 is the delay value
#define		mCH341A_CMD_I2C_STM_MS	0x50		// Command flow of I2C interface: delay in milliseconds, bit 3-bit 0 is the delay value
#define		mCH341A_CMD_I2C_STM_DLY	0x0F		// The maximum value of a single command delay in the command stream of the I2C interface
#define		mCH341A_CMD_I2C_STM_END	0x00		// Command flow of I2C interface: command packet ends early

#define		mCH341A_CMD_UIO_STM_IN	0x00		// Command flow of UIO interface: input data D7-D0
#define		mCH341A_CMD_UIO_STM_DIR	0x40		// Command flow of UIO interface: Set I/O direction D5-D0, bit 5-bit 0 is the direction data
#define		mCH341A_CMD_UIO_STM_OUT	0x80		// Command flow of UIO interface: output data D5-D0, bit 5-bit 0 is data
#define		mCH341A_CMD_UIO_STM_US	0xC0		// Command flow of UIO interface: delay in microseconds, bit 5-bit 0 is the delay value
#define		mCH341A_CMD_UIO_STM_END	0x20		// Command flow of UIO interface: command package ends early


// CH341 parallel port working mode
#define		mCH341_PARA_MODE_EPP	0x00		// CH341 parallel port working mode is EPP mode
#define		mCH341_PARA_MODE_EPP17	0x00		// CH341A parallel port working mode is EPP mode V1.7
#define		mCH341_PARA_MODE_EPP19	0x01		// CH341A parallel port working mode is EPP mode V1.9
#define		mCH341_PARA_MODE_MEM	0x02		// CH341 parallel port working mode is MEM mode
#define		mCH341_PARA_MODE_ECP	0x03		// CH341A parallel port working mode is ECP mode


// I/O direction setting bit definition, direct input status signal bit definition, direct output bit data definition
#define		mStateBitERR			0x00000100	// Read-only and writable, ERR# pin input status, 1: high level, 0: low level
#define		mStateBitPEMP			0x00000200	// Read-only and writable, PEMP pin input status, 1: high level, 0: low level
#define		mStateBitINT			0x00000400	// Read-only and writable, INT# pin input status, 1: high level, 0: low level
#define		mStateBitSLCT			0x00000800	// Read-only and writable, SLCT pin input status, 1: high level, 0: low level
#define		mStateBitWAIT			0x00002000	// Read-only and writable, WAIT# pin input status, 1: high level, 0: low level
#define		mStateBitDATAS			0x00004000	// Write-only and readable, DATAS#/READ# pin input status, 1: high level, 0: low level
#define		mStateBitADDRS			0x00008000	// Write-only and readable, ADDRS#/ADDR/ALE pin input status, 1: high level, 0: low level
#define		mStateBitRESET			0x00010000	// Write only, RESET# pin input status, 1: high level, 0: low level
#define		mStateBitWRITE			0x00020000	// Write only, WRITE# pin input status, 1: high level, 0: low level
#define		mStateBitSCL			0x00400000	// Read-only, SCL pin input status, 1: high level, 0: low level
#define		mStateBitSDA			0x00800000	// Read-only, SDA pin input status, 1: high level, 0: low level


#define		MAX_DEVICE_PATH_SIZE	128			// Maximum number of characters for device name
#define		MAX_DEVICE_ID_SIZE		64			// Maximum number of characters for device ID


typedef VOID ( CALLBACK * mPCH341_INT_ROUTINE ) (	// Interrupt service routine
	ULONG iStatus );	// Interrupt status data, refer to the bit description below
						// Bit 7-Bit 0 corresponds to the D7-D0 pin of CH341
						// Bit 8 corresponds to the ERR# pin of CH341, bit 9 corresponds to the PEMP pin of CH341, bit 10 corresponds to the INT# pin of CH341, and bit 11 corresponds to the SLCT pin of CH341


HANDLE	WINAPI	CH341OpenDevice(	// Open the CH341 device and return the handle. If an error occurs, it will be invalid.
	ULONG			iIndex );		// Specify the CH341 device serial number, 0 corresponds to the first device


VOID	WINAPI	CH341CloseDevice(	// Shut down the CH341 device
	ULONG			iIndex );		// Specify CH341 device serial number


ULONG	WINAPI	CH341GetVersion( );  // Get the DLL version number and return the version number


ULONG WINAPI CH341DriverCommand(	// Directly pass the command to the driver, return 0 if an error occurs, otherwise return the data length
	ULONG iIndex,					// Specify the CH341 device serial number. DLLs above V1.6 can also be the handle after the device is opened.
	mPWIN32_COMMAND ioCommand );	// Pointer to the command structure
// This program returns the data length after the call, and still returns the command structure. If it is a read operation, the data is returned in the command structure,
// The returned data length is 0 when the operation fails, and is the length of the entire command structure when the operation is successful. For example, if one byte is read, mWIN32_COMMAND_HEAD+1 is returned.
// Before calling the command structure, respectively provide: pipe number or command function code, length of access data (optional), data (optional)
// After the command structure is called, it returns: operation status code, length of subsequent data (optional),
// The operation status code is a code defined by WINDOWS, please refer to NTSTATUS.H,
// The length of subsequent data refers to the length of data returned by the read operation. The data is stored in the subsequent buffer. It is generally 0 for write operations.


ULONG	WINAPI	CH341GetDrvVersion( );  // Get the driver version number, return the version number, or return 0 if an error occurs


BOOL WINAPI CH341ResetDevice(	// Reset USB device
	ULONG iIndex );				// Specify CH341 device serial number	


BOOL WINAPI CH341GetDeviceDescr(	// Read device descriptor
	ULONG iIndex,					// Specify CH341 device serial number
	PVOID oBuffer,					// Points to a buffer large enough to save descriptors
	PULONG ioLength );				// Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length.	


BOOL	WINAPI	CH341GetConfigDescr(  // Read configuration descriptor
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			oBuffer,  // Points to a buffer large enough to save descriptors
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length


BOOL WINAPI CH341SetIntRoutine(			// Set interrupt service routine
	ULONG iIndex,						// Specify CH341 device serial number
	mPCH341_INT_ROUTINE iIntRoutine );	// Specify the interrupt service program. If it is NULL, the interrupt service will be canceled. Otherwise, the program will be called during the interrupt.	


BOOL WINAPI CH341ReadInter(		// Read interrupt data
	ULONG iIndex,				// Specify CH341 device serial number
	PULONG iStatus );			// Points to a double word unit, used to save the read interrupt status data, see below
	// Bit 7-Bit 0 corresponds to the D7-D0 pin of CH341
	// Bit 8 corresponds to the ERR# pin of CH341, bit 9 corresponds to the PEMP pin of CH341, bit 10 corresponds to the INT# pin of CH341, and bit 11 corresponds to the SLCT pin of CH341


BOOL	WINAPI	CH341AbortInter(  // Abort the interrupt data read operation
	ULONG			iIndex );  // Specify CH341 device serial number


BOOL	WINAPI	CH341SetParaMode(  // Set parallel port mode
	ULONG			iIndex,  // Specify CH341 device serial number
	ULONG			iMode );  // Specify parallel port mode: 0 is EPP mode/EPP mode V1.7, 1 is EPP mode V1.9, 2 is MEM mode


BOOL	WINAPI	CH341InitParallel(  // Reset and initialize the parallel port, RST# outputs low level pulse
	ULONG			iIndex,  // Specify CH341 device serial number
	ULONG			iMode );  // Specify the parallel port mode: 0 is EPP mode/EPP mode V1.7, 1 is EPP mode V1.9, 2 is MEM mode, >= 0x00000100 maintain the current mode


BOOL	WINAPI	CH341ReadData0(  // Read data block from port 0#
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			oBuffer,  // Points to a buffer large enough to save the read data
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length


BOOL	WINAPI	CH341ReadData1(  // Read data block from port 1#
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			oBuffer,  // Points to a buffer large enough to save the read data
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length


BOOL	WINAPI	CH341AbortRead(  // Abort the data block read operation
	ULONG			iIndex );  // Specify CH341 device serial numbe


BOOL	WINAPI	CH341WriteData0(  // Write data block to port 0#
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			iBuffer,  // Points to a buffer to place the data to be written out
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be written, and when returned, it is the actual written length.


BOOL	WINAPI	CH341WriteData1(  // Write data block to port 1#
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			iBuffer,  // Points to a buffer to place the data to be written out
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be written, and when returned, it is the actual written length.


BOOL	WINAPI	CH341AbortWrite(  // Abort the data block write operation
	ULONG			iIndex );  // Specify CH341 device serial number


BOOL	WINAPI	CH341GetStatus(  // Directly input data and status through CH34
	ULONG			iIndex,  // Specify CH341 device serial number
	PULONG			iStatus );  // Points to a double word unit used to save status data, refer to the following bit description
//Bit 7-Bit 0 corresponds to the D7-D0 pin of CH341
// Bit 8 corresponds to the ERR# pin of CH341, bit 9 corresponds to the PEMP pin of CH341, bit 10 corresponds to the INT# pin of CH341, bit 11 corresponds to the SLCT pin of CH341, and bit 23 corresponds to the SDA pin of CH341
// Bit 13 corresponds to the BUSY/WAIT# pin of CH341, bit 14 corresponds to the AUTOFD#/DATAS# pin of CH341, and bit 15 corresponds to the SLCTIN#/ADDRS# pin of CH341

BOOL WINAPI CH341ReadI2C( 	// Read one byte of data from the I2C interface
	ULONG iIndex, 			// Specify CH341 device serial number
	UCHAR iDevice, 			// The lower 7 bits specify the I2C device address
	UCHAR iAddr, 			// Specify the address of the data unit
	PUCHAR oByte );			// Points to a byte unit, used to save the read byte data	


BOOL WINAPI CH341WriteI2C(	// Write one byte of data to the I2C interface
	ULONG iIndex,			// Specify CH341 device serial number
	UCHAR iDevice,			// The lower 7 bits specify the I2C device address
	UCHAR iAddr,			// Specify the address of the data unit
	UCHAR iByte );			// Byte data to be written	


BOOL WINAPI CH341EppReadData(	// Read data in EPP mode: WR#=1, DS#=0, AS#=1, D0-D7=input
	ULONG iIndex,				// Specify CH341 device serial number
	PVOID oBuffer,				// Points to a buffer large enough to save the read data
	PULONG ioLength );			// Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length.	


BOOL	WINAPI	CH341EppReadAddr(  // EPP mode read address: WR#=1, DS#=1, AS#=0, D0-D7=input
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			oBuffer,  // Points to a buffer large enough to save the read address data
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length.


BOOL	WINAPI	CH341EppWriteData(  // Write data in EPP mode: WR#=0, DS#=0, AS#=1, D0-D7=output
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			iBuffer,  // Points to a buffer to place the address data to be written ou
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be written, and when returned, it is the actual written length.


BOOL	WINAPI	CH341EppWriteAddr(  // EPP mode write address: WR#=0, DS#=1, AS#=0, D0-D7=output
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			iBuffer,  // Points to a buffer to place the address data to be written out
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be written, and when returned, it is the actual written length.


BOOL	WINAPI	CH341EppSetAddr(  // EPP mode setting address: WR#=0, DS#=1, AS#=0, D0-D7=output
	ULONG			iIndex,  // Specify CH341 device serial number
	UCHAR			iAddr );  // Specify EPP address


BOOL	WINAPI	CH341MemReadAddr0(  // MEM mode read address 0: WR#=1, DS#/RD#=0, AS#/ADDR=0, D0-D7=input
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			oBuffer,  // Points to a buffer large enough to save data read from address 0
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length.


BOOL	WINAPI	CH341MemReadAddr1(  // MEM mode read address 1: WR#=1, DS#/RD#=0, AS#/ADDR=1, D0-D7=inpu
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			oBuffer,  // Points to a buffer large enough to save data read from address 1
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length.


BOOL	WINAPI	CH341MemWriteAddr0(  // MEM mode write address 0: WR#=0, DS#/RD#=1, AS#/ADDR=0, D0-D7=output
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			iBuffer,  // Points to a buffer to place the data to be written to address 0
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be written, and when returned, it is the actual written length.


BOOL	WINAPI	CH341MemWriteAddr1(  // MEM mode write address 1: WR#=0, DS#/RD#=1, AS#/ADDR=1, D0-D7=output
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			iBuffer,  // Points to a buffer to place the data to be written to address 1
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be written, and when returned, it is the actual written length.


BOOL	WINAPI	CH341SetExclusive(  // Set exclusive use of the current CH341 device
	ULONG			iIndex,  // Specify CH341 device serial number
	ULONG			iExclusive );  // If it is 0, the device can be shared and used, if not 0, it can be used exclusively.


BOOL	WINAPI	CH341SetTimeout(  // Set the timeout for USB data reading and writing
	ULONG			iIndex,  // Specify CH341 device serial number
	ULONG			iWriteTimeout,  // Specify the timeout time for USB to write data blocks, in milliseconds (mS), 0xFFFFFFFF specifies no timeout (default value)
	ULONG			iReadTimeout );  // Specify the timeout time for USB to read data blocks, in milliseconds (mS), 0xFFFFFFFF specifies no timeout (default value)


BOOL WINAPI CH341ReadData(	// Read data block
	ULONG iIndex,			// Specify CH341 device serial number
	PVOID oBuffer,			// Points to a buffer large enough to save the read data
	PULONG ioLength );		// Points to the length unit. When input, it is the length to be read, and when returned, it is the actual read length.	


BOOL	WINAPI	CH341WriteData(  // write data block
	ULONG			iIndex,  // Specify CH341 device serial number
	PVOID			iBuffer,  // Points to a buffer to place the address data to be written ou
	PULONG			ioLength );  // Points to the length unit. When input, it is the length to be written, and when returned, it is the actual written length.


PVOID	WINAPI	CH341GetDeviceName(  // Returns a buffer pointing to the CH341 device name, or NULL if an error occurs.
	ULONG			iIndex );  // Specify CH341 device serial number, 0 corresponds to the first device


ULONG	WINAPI	CH341GetVerIC(  // Get the version of CH341 chip, return: 0=Invalid device, 0x10=CH341, 0x20=CH341A
	ULONG			iIndex );  // Specify CH341 device serial number
#define		IC_VER_CH341A		0x20
#define		IC_VER_CH341A3		0x30


BOOL	WINAPI	CH341FlushBuffer(  // Clear the buffer of CH341
	ULONG			iIndex );  // Specify CH341 device serial number


BOOL WINAPI CH341WriteRead(	// Execute data flow command, output first and then input
	ULONG iIndex,			// Specify CH341 device serial number
	ULONG iWriteLength,		// Write length, the length to be written
	PVOID iWriteBuffer,		// Points to a buffer to place the data to be written.
	ULONG iReadStep,		// The length of a single block to be read, the total length to be read is (iReadStep*iReadTimes)
	ULONG iReadTimes,		// Number of times to prepare for reading
	PULONG oReadLength,		// Points to the length unit, and returns the actual read length.
	PVOID oReadBuffer );	// Points to a buffer large enough to save the read data	
	

/*
Mode:
 Bit 1-Bit 0: I2C interface speed/SCL frequency
	00=low speed/20KHz
	01=standard/100KHz (default value)
	10=fast/400KHz
	11=high speed/750KHz
 Bit 2: SPI I/O number/IO pin
	0=single input and single output (D3 clock/D5 output/D7 input) (default value)
	1=double input and double output (D3 clock/D5 output D4 Out/D7 in D6 in)
 Bit 7: Bit order in SPI byte
	0=low end first
	1=high end first
 Other bits reserved, must be 0
*/
BOOL WINAPI CH341SetStream(	// Set the serial port stream mode
	ULONG iIndex,			// Specify CH341 device serial number
	ULONG iMode );			// Specify mode


BOOL WINAPI CH341SetDelaymS(	// Set the hardware asynchronous delay, return soon after the call, and delay the specified number of milliseconds before the next stream operation
	ULONG iIndex,				//Specify CH341 device serial number
	ULONG iDelay );				//Specify the number of milliseconds of delay	
	

// write, then read; set data pointer to NULL, data size to 0 to skip
BOOL WINAPI CH341StreamI2C(	// Processes I2C data stream, 2-wire interface, clock line is SCL pin, data line is SDA pin (quasi-bidirectional I/O), speed is about 56K bytes
	ULONG iIndex,			// Specify CH341 device serial number
	ULONG iWriteLength,		// Number of data bytes to be written
	PVOID iWriteBuffer,		// Points to a buffer to place the data to be written. The first byte is usually the I2C device address and read and write direction bits.
	ULONG iReadLength,		// Number of data bytes to be read
	PVOID oReadBuffer );	// Points to a buffer, and returns the read data	
	

typedef	enum	_EEPROM_TYPE {					// EEPROM model
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


BOOL	WINAPI	CH341ReadEEPROM(  // Read data block from EEPROM, the speed is about 56K bytes
	ULONG			iIndex,  // Specify CH341 device serial number
	EEPROM_TYPE		iEepromID,  // Specify the EEPROM model
	ULONG			iAddr,  // Specify the address of the data uni
	ULONG			iLength,  // Number of data bytes to be read
	PUCHAR			oBuffer );  // Points to a buffer, and returns the read data


BOOL	WINAPI	CH341WriteEEPROM(  // Write data block to EEPROM
	ULONG			iIndex,  // Specify CH341 device serial number
	EEPROM_TYPE		iEepromID,  // Specify the EEPROM model
	ULONG			iAddr,  // Specify the address of the data unit
	ULONG			iLength,  // Number of data bytes to be written
	PUCHAR			iBuffer );  // Points to a buffer to place the address data to be written ou


BOOL	WINAPI	CH341GetInput(  // Directly input data and status through CH341, which is more efficient than CH341GetStatus
	ULONG			iIndex,  // Specify CH341 device serial number
	PULONG			iStatus );  // Points to a double word unit used to save status data, refer to the following bit descriptio
// Bit 7-Bit 0 corresponds to the D7-D0 pin of CH341
// Bit 8 corresponds to the ERR# pin of CH341, bit 9 corresponds to the PEMP pin of CH341, bit 10 corresponds to the INT# pin of CH341, bit 11 corresponds to the SLCT pin of CH341, and bit 23 corresponds to the SDA pin of CH341
// Bit 13 corresponds to the BUSY/WAIT# pin of CH341, bit 14 corresponds to the AUTOFD#/DATAS# pin of CH341, and bit 15 corresponds to the SLCTIN#/ADDRS# pin of CH341



BOOL	WINAPI	CH341SetOutput(  // Set the I/O direction of CH341 and output data directly through CH341
/* ***** Use this API with caution to prevent changing the I/O direction to change the input pin into an output pin, resulting in a short circuit with other output pins and damaging the chip ***** */
	ULONG			iIndex,  // Specify CH341 device serial number
	ULONG			iEnable,  // Data valid flag, refer to the bit description below
// Bit 0 is 1, indicating that bits 15 to 8 of iSetDataOut are valid, otherwise they are ignored.
// Bit 1 is 1, indicating that bits 15 to 8 of iSetDirOut are valid, otherwise they are ignored.
// Bit 2 is 1 indicating that 7-bit 0 of iSetDataOut is valid, otherwise it is ignored
// Bit 3 is 1, indicating that bit 7-bit 0 of iSetDirOut is valid, otherwise it is ignored
// Bit 4 is 1, indicating that bits 23-16 of iSetDataOut are valid, otherwise they are ignored
	ULONG			iSetDirOut,  // Set the I/O direction. If a certain bit is cleared to 0, the corresponding pin is input, and if a certain bit is set to 1, the corresponding pin is output. The default value in parallel port mode is 0x000FC000. Please refer to the following bit description.
	ULONG			iSetDataOut );  // Output data, if the I/O direction is output, then when a certain bit is cleared to 0, the corresponding pin outputs a low level, and when a certain bit is set to 1, the corresponding pin outputs a high level, refer to the following bit descriptions
// Bit 7-Bit 0 corresponds to the D7-D0 pin of CH341
// Bit 8 corresponds to the ERR# pin of CH341, bit 9 corresponds to the PEMP pin of CH341, bit 10 corresponds to the INT# pin of CH341, and bit 11 corresponds to the SLCT pin of CH341
// Bit 13 corresponds to the WAIT# pin of CH341, bit 14 corresponds to the DATAS#/READ# pin of CH341, and bit 15 corresponds to the ADDRS#/ADDR/ALE pin of CH341
//The following pins can only be output, regardless of I/O direction: Bit 16 corresponds to the RESET# pin of CH341, bit 17 corresponds to the WRITE# pin of CH341, bit 18 corresponds to the SCL pin of CH341, and bit 29 corresponds to the CH341 SDA pin


BOOL	WINAPI	CH341Set_D5_D0(  // Set the I/O direction of the D5-D0 pin of CH341, and directly output data through the D5-D0 pin of CH341, which is more efficient than CH341SetOutput
/* ***** Use this API with caution to prevent changing the I/O direction to change the input pin into an output pin, resulting in a short circuit with other output pins and damaging the chip ***** */
	ULONG			iIndex,  // Specify CH341 device serial number
	ULONG			iSetDirOut,  // Set the I/O direction of each pin of D5-D0. If a certain bit is cleared to 0, the corresponding pin is input, and if a certain bit is set to 1, the corresponding pin is output. The default value in parallel port mode is 0x00, all input
	ULONG			iSetDataOut );  // Set the output data of each pin of D5-D0. If the I/O direction is output, then when a certain bit is cleared to 0, the corresponding pin outputs a low level, and when a certain bit is set to 1, the corresponding pin outputs a high level.
// Bit 5-bit 0 of the above data correspond to the D5-D0 pins of CH341 respectively.


BOOL	WINAPI	CH341StreamSPI3(  // This API is invalid, please do not use it
	ULONG			iIndex,
	ULONG			iChipSelect,
	ULONG			iLength,
	PVOID			ioBuffer );


BOOL	WINAPI	CH341StreamSPI4(  // Process SPI data stream, 4-wire interface, the clock line is DCK/D3 pin, the output data line is DOUT/D5 pin, the input data line is DIN/D7 pin, and the chip select line is D0/ D1/D2, speed is about 68K bytes
/* SPI timing: The DCK/D3 pin is the clock output, which defaults to low level. The DOUT/D5 pin outputs during the low level before the rising edge of the clock. The DIN/D7 pin outputs the high level before the falling edge of the clock. Enter during normal times */
	ULONG			iIndex,  // Specify CH341 device serial number
	ULONG			iChipSelect,  // Chip select control, if bit 7 is 0, the chip select control is ignored, if bit 7 is 1, the parameters are valid: Bit 1 and bit 0 are 00/01/10, respectively, select the D0/D1/D2 pin as low level. valid chip select
	ULONG			iLength,  // Number of data bytes to be transmitted
	PVOID			ioBuffer );  // Point to a buffer, place the data to be written from DOUT, and return the data read from DIN


BOOL	WINAPI	CH341StreamSPI5(  // Process SPI data stream, 5-wire interface, the clock line is DCK/D3 pin, the output data line is DOUT/D5 and DOUT2/D4 pin, the input data line is DIN/D7 and DIN2/D6 pin pin, the chip select line is D0/D1/D2, the speed is about 30K bytes*2
/* SPI timing: DCK/D3 pin is clock output, default is low level, DOUT/D5 and DOUT2/D4 pins output during the low level period before the rising edge of the clock, DIN/D7 and DIN2/D6 pins Input during the high period before the falling edge of the clock */
	ULONG			iIndex,  // Specify CH341 device serial number
	ULONG			iChipSelect,  // Chip select control, if bit 7 is 0, the chip select control is ignored, if bit 7 is 1, the parameters are valid: Bit 1 and bit 0 are 00/01/10, respectively, select the D0/D1/D2 pin as low level. valid chip select
	ULONG			iLength,  // Number of data bytes to be transmitted
	PVOID			ioBuffer,  // Points to a buffer to place the data to be written from DOUT. After returning, the data is read from DIN.
	PVOID			ioBuffer2 );  // Point to the second buffer, place the data to be written from DOUT2, and return the data read from DIN2


BOOL	WINAPI	CH341BitStreamSPI(  // Process SPI bit data stream, 4-wire/5-wire interface, clock line is DCK/D3 pin, output data line is DOUT/DOUT2 pin, input data line is DIN/DIN2 pin, chip select The line is D0/D1/D2, the speed is about 8K bits*2
	ULONG			iIndex,  // Specify CH341 device serial number
	ULONG			iLength,  // The number of data bits to be transmitted, up to 896 at a time, and it is recommended not to exceed 256
	PVOID			ioBuffer );  // Point to a buffer, place the data to be written from DOUT/DOUT2/D2-D0, and return the data read from DIN/DIN2
/* SPI timing: DCK/D3 pin is clock output, default is low level, DOUT/D5 and DOUT2/D4 pins output during the low level period before the rising edge of the clock, DIN/D7 and DIN2/D6 pins Input during the high period before the falling edge of the clock*/
/* A total of 8 bits in a byte in ioBuffer correspond to the D7-D0 pins respectively, bit 5 is output to DOUT, bit 4 is output to DOUT2, bit 2-bit 0 is output to D2-D0, bit 7 is input from DIN, bit 6 Input from DIN2, bit 3 data is ignored*/
/* Before calling this API, you should first call CH341Set_D5_D0 to set the I/O direction of the D5-D0 pin of CH341 and set the default level of the pin */


BOOL	WINAPI	CH341SetBufUpload(  // Set internal buffer upload mode
	ULONG			iIndex,  // Specify CH341 device serial number, 0 corresponds to the first device
	ULONG			iEnableOrClear );  // If it is 0, the internal buffer upload mode is disabled and direct upload is used. If it is not 0, the internal buffer upload mode is enabled and the existing data in the buffer is cleared.
// If the internal buffered upload mode is enabled, the CH341 driver creates a thread to automatically receive the USB upload data to the internal buffer and clear the existing data in the buffer. When the application calls CH341ReadData, it will immediately return the existing data in the buffer. data


LONG	WINAPI	CH341QueryBufUpload(  // Query the number of existing data packets in the internal upload buffer. The number of data packets is returned successfully. If an error occurs, -1 is returned.
	ULONG			iIndex );  // Specify CH341 device serial number, 0 corresponds to the first device


BOOL	WINAPI	CH341SetBufDownload(  // 设定内部缓冲下传模式
	ULONG			iIndex,  // Specify CH341 device serial number, 0 corresponds to the first device
	ULONG			iEnableOrClear );  // If it is 0, the internal buffer download mode is disabled and direct download is used. If it is not 0, the internal buffer download mode is enabled and the existing data in the buffer is cleared.
// If the internal buffer download mode is enabled, then when the application calls CH341WriteData, it will just put the USB download data into the internal buffer and return immediately, and the thread created by the CH341 driver will automatically send it until it is completed.


LONG	WINAPI	CH341QueryBufDownload(  // Query the number of remaining data packets in the internal download buffer (not yet sent), return the number of data packets successfully, and return -1 on error
	ULONG			iIndex );  // Specify CH341 device serial number, 0 corresponds to the first device


BOOL	WINAPI	CH341ResetInter(  // Reset interrupt data read operation
	ULONG			iIndex );  // Specify CH341 device serial number


BOOL	WINAPI	CH341ResetRead(  // Reset data block read operation
	ULONG			iIndex );  // Specify CH341 device serial number


BOOL	WINAPI	CH341ResetWrite(  // Reset data block write operation
	ULONG			iIndex );  // Specify CH341 device serial number


typedef		VOID	( CALLBACK	* mPCH341_NOTIFY_ROUTINE ) (  // Device event notification callback program
	ULONG			iEventStatus );  // Device events and current status (defined below): 0=device unplugged event, 3=device plugged in event

#define		CH341_DEVICE_ARRIVAL		3		// Device insertion event, already inserted
#define		CH341_DEVICE_REMOVE_PEND	1		// The device is about to be unplugged
#define		CH341_DEVICE_REMOVE			0		// Device unplug event, has been unplugged


BOOL	WINAPI	CH341SetDeviceNotify(  // Set device event notification program
	ULONG					iIndex,  // Specify CH341 device serial number, 0 corresponds to the first device
	PCHAR					iDeviceID,  // Optional parameter, pointing to a string, specifying the ID of the device being monitored, the string is terminated with \0
	mPCH341_NOTIFY_ROUTINE	iNotifyRoutine );  // Specify the device event callback program. If it is NULL, the event notification will be canceled. Otherwise, the program will be called when the event is detected.


BOOL	WINAPI	CH341SetupSerial(  // Set the serial port characteristics of CH341. This API can only be used for CH341 chips that work in serial port mode.
	ULONG					iIndex,  // Specify CH341 device serial number, 0 corresponds to the first device
	ULONG					iParityMode,  // Specify the data verification mode of the CH341 serial port: NOPARITY/ODDPARITY/EVENPARITY/MARKPARITY/SPACEPARITY
	ULONG					iBaudRate );  // Specify the communication baud rate value of the CH341 serial port, which can be any value between 50 and 3000000

/*  The following APIs can be used for CH341 chips that work in serial port mode. Other APIs can generally only be used for CH341 chips that work in parallel port mode.
	CH341OpenDevice
	CH341CloseDevice
	CH341SetupSerial
	CH341ReadData
	CH341WriteData
	CH341SetBufUpload
	CH341QueryBufUpload
	CH341SetBufDownload
	CH341QueryBufDownload
	CH341SetDeviceNotify
	CH341GetStatus
//  The above is the main API, the following is the secondary API
	CH341GetVersion
	CH341DriverCommand
	CH341GetDrvVersion
	CH341ResetDevice
	CH341GetDeviceDescr
	CH341GetConfigDescr
	CH341SetIntRoutine
	CH341ReadInter
	CH341AbortInter
	CH341AbortRead
	CH341AbortWrite
	CH341ReadI2C
	CH341WriteI2C
	CH341SetExclusive
	CH341SetTimeout
	CH341GetDeviceName
	CH341GetVerIC
	CH341FlushBuffer
	CH341WriteRead
	CH341ResetInter
	CH341ResetRead
	CH341ResetWrite
*/
HANDLE	WINAPI	CH341OpenDeviceEx(   // Open the CH341 device and return the handle. If an error occurs, it will be invalid.
    ULONG			iIndex );        // Specify CH341 device serial number, 0 corresponds to the first device inserted, 1 corresponds to the second device inserted, in order to save device serial number resources, the device must be turned off after use

VOID	WINAPI	CH341CloseDeviceEx(  // Close the CH341 device
	ULONG			iIndex );        // Specify CH341 device serial number

PCHAR	WINAPI	CH341GetDeviceNameEx(   // Returns the buffer pointing to the CH341 device name, or returns NULL if an error occurs
	ULONG			iIndex );           // Specify CH341 device serial number, 0 corresponds to the first device

BOOL	WINAPI	CH341SetDeviceNotifyEx(       // Set device event notification program
	ULONG					iIndex,           // Specify CH341 device serial number, 0 corresponds to the first device
	PCHAR					iDeviceID,        // Optional parameter, pointing to a string, specifying the ID of the device being monitored, the string is terminated with \0
	mPCH341_NOTIFY_ROUTINE	iNotifyRoutine ); // Specify the device event callback program. If it is NULL, the event notification will be canceled. Otherwise, the program will be called when the event is detected.


#ifdef __cplusplus
}
#endif

#endif		// _CH341_DLL_H
