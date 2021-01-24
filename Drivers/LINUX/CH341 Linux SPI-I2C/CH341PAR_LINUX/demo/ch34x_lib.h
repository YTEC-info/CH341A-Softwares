//
//
//****************************************
//**  Copyright  (C)  W.ch  2013        **
//**  Web:  http://www.wch.cn	        **
//****************************************
//**  LIB for USB interface chip CH341  **
//**  C, LINUX                          **
//****************************************
//
//
#ifndef		_CH34X_LIB_H
#define		_CH34X_LIB_H

#ifndef UCHAR	
#define UCHAR		unsigned char
#endif

#ifndef USHORT
#define USHORT		unsigned short
#endif

#ifndef ULONG	
#define ULONG		unsigned long
#endif

#ifndef PUCHAR	
#define PUCHAR		unsigned char *
#endif

#ifndef PCHAR		
#define PCHAR		char *
#endif

#ifndef PUSHORT 
#define PUSHORT		unsigned short *
#endif

#ifndef PULONG
#define PULONG		unsigned long *
#endif

#ifndef VOID   
#define VOID		void
#endif

#ifndef PVOID
#define PVOID		void *
#endif

#define true 1
#define false 0

#ifndef min
#define min( x, y )	(((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#define max( x, y )	(((x) < (y)) ? (y) : (x))
#endif

typedef enum
{
	FALSE_H = 0,
	TRUE_H = !FALSE_H
}BOOL;

#define		CH341_PACKET_LENGTH		32			
#define		CH341_PKT_LEN_SHORT		8			

#define		CH341_MAX_NUMBER			16			
#define		MAX_BUFFER_LENGTH		0x1000		
#define		DEFAULT_BUFFER_LEN		0x0400		


//CH341 endpointer addr
#define		CH341_ENDP_INTER_UP	0x81			// Interrupt up endpoint
#define		CH341_ENDP_INTER_DOWN	0x01		// Interrupt down endpoint
#define		CH341_ENDP_DATA_UP		0x82		// Data up endpoint
#define		CH341_ENDP_DATA_DOWN	0x02		// Data down endpoint



#define		PipeDeviceCtrl			0x00000004	
#define		PipeInterUp			0x00000005	
#define		PipeDataUp				0x00000006	
#define		PipeDataDown			0x00000007	

#define		CH341_VENDOR_READ		0xC0	 //CH341 Read Command
#define		CH341_VENDOR_WRITE		0x40	 //CH341 Write Command




#define		CH341_PARA_INIT		0xB1			// Init para
#define		CH341_I2C_STATUS		0x52		// Get I2C Interface State
#define		CH341_I2C_COMMAND		0x53		// Send I2C Command

#define		CH341_PARA_CMD_R0		0xAC		// Read Data0 From Para
#define		CH341_PARA_CMD_R1		0xAD		// Read Data1 From Para
#define		CH341_PARA_CMD_W0		0xA6		// Write Data0 From Para
#define		CH341_PARA_CMD_W1		0xA7		// Write Data1 From Para
#define		CH341_PARA_CMD_STS		0xA0		// Get Para State


#define		CH341A_CMD_SET_OUTPUT	0xA1		// Set Para Output
#define		CH341A_CMD_IO_ADDR		0xA2		//MEM Addr write/read
#define		CH341A_CMD_PRINT_OUT	0xA3		// Print output
#define		CH341A_CMD_PWM_OUT		0xA4		// PWM Out Command
#define		CH341A_CMD_SHORT_PKT	0xA5		//short package
#define		CH341A_CMD_SPI_STREAM	0xA8		//SPI Interface Command
//#define		mCH341A_CMD_SIO_STREAM	0xA9		
#define		CH341A_CMD_I2C_STREAM	0xAA		// I2C Interface Command
#define		CH341A_CMD_UIO_STREAM	0xAB		// UIO Interface Command
#define		CH341A_CMD_PIO_STREAM	0xAE		// PIO Interface Command


#define		CH341A_BUF_CLEAR		0xB2		// clear uncompleted data
#define		CH341A_I2C_CMD_X		0x54		// Send I2C Interface command
#define		CH341A_DELAY_MS		0x5E			// Set Delay(ms)
#define		CH341A_GET_VER			0x5F		// Get Chip Version

// CH341 Write/Read Max Length Once in EPP/MEM Mode
#define		CH341_EPP_IO_MAX		( mCH341_PACKET_LENGTH - 1 )	
#define		CH341A_EPP_IO_MAX		0xFF		// CH341A Write/Read Max Length Once in EPP/MEM Mode

#define		CH341A_CMD_IO_ADDR_W	0x00		//Mem Write
#define		CH341A_CMD_IO_ADDR_R	0x80		//MEM Read

#define		CH341A_CMD_I2C_STM_STA	0x74		//I2C Stream Start Command
#define		CH341A_CMD_I2C_STM_STO	0x75		//I2C Stream Stop byte Command
#define		CH341A_CMD_I2C_STM_OUT	0x80		//I2C Stream Out Command
#define		CH341A_CMD_I2C_STM_IN	0xC0		//I2C Stream In Command
#define		CH341A_CMD_I2C_STM_MAX	( min( 0x3F, mCH341_PACKET_LENGTH ) )	//I2C Stream Max Length
#define		CH341A_CMD_I2C_STM_SET	0x60		//I2C Stream Set Mode
//-->bit2	spi io (0: one in one out ; 1: two in two out)
//-->bit1~0  I2C SCL Rate
#define		CH341A_CMD_I2C_STM_US	0x40		//I2C Stream Delay(us)
#define		CH341A_CMD_I2C_STM_MS	0x50		//I2C Stream Delay(ms)
#define		CH341A_CMD_I2C_STM_DLY	0x0F		//I2C Stream Set Max Delay
#define		CH341A_CMD_I2C_STM_END	0x00		//I2C Stream End Command

#define		CH341A_CMD_UIO_STM_IN	0x00	 // UIO Interface In ( D0 ~ D7 )
#define		CH341A_CMD_UIO_STM_DIR	0x40	 // UIO interface Dir( set dir of D0~D5 )
#define		CH341A_CMD_UIO_STM_OUT	0x80	 //	UIO Interface Output(D0~D5)
#define		CH341A_CMD_UIO_STM_US	0xC0	 // UIO Interface Delay Command( us )
#define		CH341A_CMD_UIO_STM_END	0x20	 // UIO Interface End Command


#define		CH341_PARA_MODE_EPP	0x00		
#define		CH341_PARA_MODE_EPP17	0x00		
#define		CH341_PARA_MODE_EPP19	0x01		
#define		CH341_PARA_MODE_MEM	0x02		
#define		CH341_PARA_MODE_ECP	0x03		

#define		StateBitERR			0x00000100	
#define		StateBitPEMP			0x00000200
#define		StateBitINT			0x00000400	
#define		StateBitSLCT			0x00000800	
#define		StateBitWAIT			0x00002000
#define		StateBitDATAS			0x00004000	
#define		StateBitADDRS			0x00008000	
#define		StateBitRESET			0x00010000	
#define		StateBitWRITE			0x00020000	
#define		StateBitSCL			0x00400000	
#define		StateBitSDA			0x00800000	

typedef	enum	_EEPROM_TYPE {				
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

int CH34xOpenDevice(UCHAR index);

void CH34xCloseDevice();

BOOL CH34x_GetDrvVer( char *drv_version );

BOOL CH34x_GetVendorId( PULONG VendorId );

BOOL CH34xSetParaMode( ULONG Mode );

BOOL CH34xInitParallel( ULONG Mode );

int CH34xEppRead( PUCHAR oBuffer,       
		  ULONG ioLength,
	          ULONG PipeMode ); // PipiMode->0 : read data
				    // PipeMode->1 : read Addr    

int CH34xEppWrite( PUCHAR iBuffer,		
        	   ULONG ioLength,
		   ULONG PipeMode ); // PipeMode->0 : write data
				     // PipeMode->1 : write data		

int CH34xEppReadData( PUCHAR oBuffer, 	// input buffer     
		      ULONG ioLength ); //wanted length of read     

int CH34xEppWriteData( PUCHAR iBuffer,		
        	       ULONG ioLength );

int CH34xEppWriteAddr( PUCHAR iBuffer, 
		       ULONG ioLength );

int CH34xEppReadAddr( PUCHAR oBuffer, 
		      ULONG ioLength );

int CH34xEppSetAddr( ULONG iAddr );	//EPP: WR#=0,DS#=1,AS#=0,D0~D7 output
	
BOOL CH34xInitMEM(void);

int CH34xMEMReadData( PUCHAR oBuffer,
		     ULONG ioLength,
		     ULONG PipeMode );

int CH34xMEMWriteData( PUCHAR iBuffer,
		     ULONG ioLength,
		     ULONG PipeMode );

BOOL CH34xSetStream( ULONG Mode );		

BOOL CH34xSetDelaymS( ULONG iDelay );

BOOL CH34xReadData( PVOID oBuffer, 
				PULONG ioLength );

BOOL CH34xWriteData( PVOID iBuffer,
			PULONG ioLength );

BOOL CH34xWriteRead( ULONG iWriteLength, 
                        PVOID iWriteBuffer,
                     /*   ULONG iReadStep,
                        ULONG iReadTimes,*/
                        PULONG oReadLength,
                        PVOID oReadBuffer );

BOOL CH34xSetOutput( ULONG iEnable, ULONG iSetDirOut, ULONG iSetDataOut );

BOOL CH34xSet_D5_D0( UCHAR iSetDirOut,UCHAR iSetDataOut);

BOOL CH34xStreamI2C( ULONG iWriteLength, PVOID iWriteBuffer,
			ULONG iReadLength, PVOID oReadBuffer);

BOOL CH34xReadEEPROM( EEPROM_TYPE iEepromID,  
		      ULONG iAddr,  
	    	      ULONG iLength,  
		      PUCHAR oBuffer );  

BOOL CH34xWriteEEPROM( EEPROM_TYPE iEepromID,  
			ULONG iAddr,  
			ULONG iLength,  
			PUCHAR iBuffer );  

BOOL CH34xStreamSPIx( ULONG iChipSelect,
			ULONG iLength,
			PVOID ioBuffer,
			PVOID ioBuffer2 );

BOOL CH34xStreamSPI4( ULONG iChipSelect, 
			ULONG iLength,
			PVOID ioBuffer );



#endif
