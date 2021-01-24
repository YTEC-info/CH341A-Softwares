// V1.1

# Include <windows.h>
# Include "CH341DLL.H"

/* This program involves:

   1, 2-wire interface, some of the custom timing, dealing with IIC bus acknowledge bit, and an example of
      the internal 2-wire interface timing analysis
   2, provide examples of procedures, the operation 2-wire interface IIC device X76F640, PCF8574, PCA9554
   3, or MEM with EPP parallel port for a number of multi-bit output or input, analog read-only or write-only SPI timing
   4, with the UIO purpose I / O bit stream order to achieve a custom synchronous serial interface
   5, provide examples of procedures, operations similar to the non-standard serial SPI timing device TLC1549
   6, provides example programs using CH341StreamSPI4 4-wire interface SPI devices 25C512, 25C020
   7, provides example programs using CH341BitStreamSPI, similar non-standard SPI timing device ADC0831
              Another set of CH341 can CH341SetOutput I/O direction, and through any pin of CH341 direct output
      data, does not provide examples, it is recommended instead of using CH341Set_D5_D0
*/

/* Measurement speed
        CH341StreamI2C about 56K bytes
        CH341ReadEEPROM about 56K bytes
        CH341WriteEEPROM about 5K bytes (RAM instead of flash if it is the same speed with CH341ReadEEPROM)
        CH341StreamSPI4 about 68K bytes
        CH341StreamSPI5 about 30K bytes per channel * 2 inputs and 2 outputs
        CH341BitStreamSPI about 8K-bit per channel * at least 2 inputs and 2 outputs (up to 7 inputs 4 outputs)
*/

/* ***************************************************************************************** */
/* Example: a common IIC bus-compatible operating sequence */

BOOL WINAPI IIC_IssueStart (ULONG iIndex) // device number designated CH341
{
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength;

    mBuffer [0] = mCH341A_CMD_I2C_STREAM;                           // command code
    mBuffer [1] = mCH341A_CMD_I2C_STM_STA;                          // generate the start bit
    mBuffer [2] = mCH341A_CMD_I2C_STM_END;                          // end of the current package in advance
    mLength = 3;
    return (CH341WriteData (iIndex, mBuffer, & mLength));           // write the data block
}

BOOL WINAPI IIC_IssueStop (ULONG iIndex)                            // device number designated CH341
{
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength;

    mBuffer [0] = mCH341A_CMD_I2C_STREAM;                           // command code
    mBuffer [1] = mCH341A_CMD_I2C_STM_STO;                          // stop bit generation
    mBuffer [2] = mCH341A_CMD_I2C_STM_END;                          // end of the current package in advance
    mLength = 3;
    return (CH341WriteData (iIndex, mBuffer, & mLength));           // write the data block
}

// IIC_OutBlockSkipAck() - output data block, do not check the response is valid
//   iIndex is ​​the device number allocated by CH341
//   iOutLength ​​ready to write data bytes 29 bytes must be less than a single
//   iOutBuffer is a pointer to a buffer, ready to write the data into place
BOOL WINAPI IIC_OutBlockSkipAck (ULONG iIndex, ULONG iOutLength, PVOID iOutBuffer) 
{
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength;

    if (iOutLength == 0 | | iOutLength> (mCH341_PACKET_LENGTH - 1 - 1 - 1)) 
        return (FALSE);

    mBuffer [0] = mCH341A_CMD_I2C_STREAM;                           // command code
    mBuffer [1] = (UCHAR) (mCH341A_CMD_I2C_STM_OUT | iOutLength);   // output data, bit 5 - bit 0 is the length of

    memcpy (& mBuffer [2], iOutBuffer, iOutLength);                 // data
    mBuffer [1 + 1 + iOutLength] = mCH341A_CMD_I2C_STM_END;         // end of the current package in advance
    mLength = 1 + 1 + iOutLength + 1;

    return (CH341WriteData (iIndex, mBuffer, & mLength));           // write the data block
}


// IIC_OutByteCheckAck() - output a byte of data and check that the response is valid
//   iIndex is te ​​device number designated by CH341
//   iOutByte write the data to prepare
BOOL WINAPI IIC_OutByteCheckAck (ULONG iIndex, UCHAR iOutByte) {
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength, mInLen;

    mBuffer [0] = mCH341A_CMD_I2C_STREAM;                           // command code
    mBuffer [1] = mCH341A_CMD_I2C_STM_OUT;                          // output data, bit 5 - bit 0 of length,length 0 bytes;returns only to send a response
    mBuffer [2] = iOutByte;                                         // data
    mBuffer [3] = mCH341A_CMD_I2C_STM_END; // end of the current package in advance
    mLength = 4;
    mInLen = 0;

    if (CH341WriteRead (iIndex, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, & mInLen, mBuffer)) { // execute command stream, and then input first output
        if (mInLen & & (mBuffer [mInLen - 1] & 0x80) == 0) return (TRUE); // data returned ACK response on behalf of the bit 7 position, ACK = 0 effective
    }
    return (FALSE);
}


// IIC_InBlockByAck() - input data blocks, each input byte is an effective response
//   iIndex is the ​​device number designated by CH341
//   iInLength is ​​ready to read data bytes 32 bytes must be less than a single
//   oInBuffer is a pointer to a buffer, return the data is read

BOOL WINAPI IIC_InBlockByAck (ULONG iIndex, ULONG iInLength, PVOID oInBuffer) {

    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength, mInLen;

    if (iInLength == 0 | | iInLength> mCH341A_CMD_I2C_STM_MAX) return (FALSE);
    mBuffer [0] = mCH341A_CMD_I2C_STREAM; // command code
    mBuffer [1] = (UCHAR) (mCH341A_CMD_I2C_STM_IN | iInLength); // input data, bit 5 - bit 0 is the length of
    mBuffer [2] = mCH341A_CMD_I2C_STM_END; // end of the current package in advance
    mLength = 3;
    mInLen = 0;
    if (CH341WriteRead (iIndex, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, & mInLen, mBuffer)) { // execute command stream, and then input first output
        if (mInLen == iInLength) {
            memcpy (oInBuffer, & mBuffer [0], iInLength); // data
            return (TRUE);
        }
    }
    return (FALSE);
}

// IIC_InByteNoAck() - input a byte of data, but does not produce response
// ​​  iIndex is device number designated CH341
//   oInByte points to a byte buffer is read back the data after
BOOL WINAPI IIC_InByteNoAck (ULONG iIndex, PUCHAR oInByte) {
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength, mInLen;

    mBuffer [0] = mCH341A_CMD_I2C_STREAM;   // command code
    mBuffer [1] = mCH341A_CMD_I2C_STM_IN;   // input data, bit 5 - bit 0 of length, only to receive a length of 0 bytes and sends no response
    mBuffer [2] = mCH341A_CMD_I2C_STM_END;  // end of the current package in advance
    mLength = 3;
    mInLen = 0;
                                                                            // execute command stream and then input first output
    if (CH341WriteRead (iIndex, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, & mInLen, mBuffer)) { 
        if (mInLen) {
            * OInByte = mBuffer [mInLen - 1];   // data
        return (TRUE);
        }
    }
    return (FALSE);
}

/* ********************************************************************************************** */
/* Operation encrypted memory X76F640 */


// X76F640_AckPolling() - query X76F640 answer (include: the start bit, a byte of command data output, check the response is valid)

BOOL WINAPI X76F640_AckPolling (ULONG iIndex) {  // iIndex is device number designated CH341
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength, mInLen;

    mBuffer [0] = mCH341A_CMD_I2C_STREAM;   // command code
    mBuffer [1] = mCH341A_CMD_I2C_STM_STA;  // generate the start bit
    mBuffer [2] = mCH341A_CMD_I2C_STM_OUT;  // output data, bit 5 - bit 0 of length, the length of 0 bytes and returns only to send a response
    mBuffer [3] = 0xF0;                     // query command response codes
    mBuffer [4] = mCH341A_CMD_I2C_STM_END;  // end of the current package in advance
    mLength = 5;
    mInLen = 0;
                                                                            // execute command stream and then input first output
    if (CH341WriteRead (iIndex, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, & mInLen, mBuffer)) {
        if (mInLen & & (mBuffer [mInLen - 1] & 0x80) == 0)
            return (TRUE);                                                  // data returned ACK response on behalf of bit 7 position, ACK = 0 effective
    }
    return (FALSE);
}

// X76F640_CheckPasswd() - send operation commands and check the specified password (including: output start bit, 
//                         9 bytes of output data (1 + 8 password command), the query response, the output 2-byte address)
//  iIndex is device number designated CH341
//  iCommand is operation command code
//  iPasswdBuf is pointer to a buffer with 8 bytes of the password data
//  iAddress) to specify address or password operation of 2-byte data

BOOL WINAPI X76F640_CheckPasswd (ULONG iIndex, ULONG iCommand, PVOID iPasswdBuf, ULONG iAddress) {
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG i, mLength;
    
    i = 0;
    mBuffer [i++] = mCH341A_CMD_I2C_STREAM;                 // command code
    mBuffer [i++] = mCH341A_CMD_I2C_STM_STA;                // generate the start bit
    mBuffer [i++] = (UCHAR) (mCH341A_CMD_I2C_STM_OUT | 9);  // output data, bit 5 - bit 0 is the length of 9 bytes
    mBuffer [i++] = (UCHAR) iCommand;                       // operation command code
    memcpy (& mBuffer [i], iPasswdBuf, 8);                  // 8 byte password data
    i += 8;
    mBuffer [i++] = (UCHAR) (mCH341A_CMD_I2C_STM_MS | 10);  // to bo seconds delay, bit 3 - bit 0 value for the delay, delay 10 ms
    mBuffer [i++] = mCH341A_CMD_I2C_STM_END;                // end of the current package in advance
    mLength = i;
    if (CH341WriteData (iIndex, mBuffer, & mLength)) {      // write the data block
        if (X76F640_AckPolling (iIndex)) {                  // query response effective
            i = 0;
            mBuffer [i++] = mCH341A_CMD_I2C_STREAM;         // command code
            mBuffer [i++] = (UCHAR) (mCH341A_CMD_I2C_STM_OUT | 2);  // output data, bit 5 - bit 0 is the length of
            mBuffer [i++] = (UCHAR) (iAddress & 0x00FF);            // lower 8-bit address
            mBuffer [i++] = (UCHAR) ((iAddress>> 8) & 0x00FF);      // 8 bits of address
            mBuffer [i++] = mCH341A_CMD_I2C_STM_END;                // end of the current package in advance
            mLength = i;
            return (CH341WriteData (iIndex, mBuffer, & mLength));   // write the data block
        }
    else IIC_IssueStop (iIndex);                            // invalid response
    }
    return (FALSE);
}

// X76F640_WriteWithPasswd() - write X76F640 block, using the specified password
BOOL WINAPI X76F640_WriteWithPasswd (
    ULONG iIndex,                                           // ​​device number designated CH341
    ULONG iWriteCommand,                                    // ​​block write command code
    PVOID iPasswdBuf,                                       // ​​pointer to a buffer, place the 8-byte password data
    ULONG iAddress,                                         // ​​address of the specified operation
    ULONG iOutLength,                                       // ​​ready to write data bytes must be less than 32 single-byte (one sector)
    PVOID iOutBuffer)                                       // point to a buffer, ready to write the data to place
{
    UCHAR mBuffer [mDEFAULT_BUFFER_LEN];
    ULONG i, mLength;

    if (iOutLength == 0 | | iOutLength> 32)
        return (FALSE);
    if (X76F640_CheckPasswd (iIndex, iWriteCommand, iPasswdBuf, iAddress)) {    // check the password by issuing the command and
        if (iOutLength> (mCH341_PACKET_LENGTH - 1 - 1 - 1 - 1 - 1)) {           // remove the first 2 byte 3 byte, a packet is not enough
            mLength = iOutLength - (mCH341_PACKET_LENGTH - 1 - 1 - 1 - 1 - 1);  // length of more
            iOutLength -= mLength;                                              // 1st data packet length
        }
        else
            mLength = 0; // 1 is enough to use a package
        i = 0;
        mBuffer [i++] = mCH341A_CMD_I2C_STREAM;                 // command code
        mBuffer [i++] = (UCHAR) (mCH341A_CMD_I2C_STM_OUT | iOutLength); // output data, bit 5 - bit 0 is the length of
        memcpy (& mBuffer [i], iOutBuffer, iOutLength);         // data
        i += iOutLength;
        if (mLength) {                                          // 2 package
            mBuffer [i] = mCH341A_CMD_I2C_STM_END;              // end of the current package in advance
            i += mCH341_PACKET_LENGTH - i% mCH341_PACKET_LENGTH;// skip the rest of the current package
            mBuffer [i++] = mCH341A_CMD_I2C_STREAM;             // the first two packets of the first byte command code is still
            mBuffer [i++] = (UCHAR) (mCH341A_CMD_I2C_STM_OUT | mLength); // output data, bit 5 - bit 0 is the length of
            memcpy (& mBuffer [i], (PUCHAR) iOutBuffer + iOutLength, mLength); // remaining data
            i += mLength;
        }
        mBuffer [i++] = mCH341A_CMD_I2C_STM_STO;                // stop bit generation
        mBuffer [i++] = (UCHAR) (mCH341A_CMD_I2C_STM_MS | 10);  // to bo seconds delay, bit 3 - bit 0 value for the delay, delay 10 ms
        mBuffer [i++] = mCH341A_CMD_I2C_STM_END;                // end of the current package in advance
        return (CH341WriteData (iIndex, mBuffer, & i));         // write the data block
    }
    return (FALSE);
}

//  X76F640_ReadWithPasswd() - read X76F640 block, using the specified password (including: output start bit,
//      9 bytes of output data (1 + 8 password command), the query response, the output 2-byte address, read data blocks)
BOOL WINAPI X76F640_ReadWithPasswd (
ULONG iIndex,       // ​​device number designated CH341
ULONG iReadCommand, // ​​code block read command
PVOID iPasswdBuf,   // ​​pointer to a buffer, place the 8-byte password data
ULONG iAddress,     // ​​address of the specified operation
ULONG iInLength,    // ​​number of bytes ready to read the data, must be less than 512 single-byte (32 * 16 per pack pack = 512 bytes)
PVOID oInBuffer)    // point to a buffer, return the data is read
{
    UCHAR mBuffer [mDEFAULT_BUFFER_LEN];
    ULONG i, mLength, mInLen;

    if (iInLength == 0 | | iInLength> (16 * mCH341_PACKET_LENGTH))
        return (FALSE);
    if (X76F640_CheckPasswd (iIndex, iReadCommand, iPasswdBuf, iAddress)) { // check the password by issuing the command and
        i = 0;
        mBuffer [i++] = mCH341A_CMD_I2C_STREAM;                 // command code
        for (mInLen = 1; mInLen <iInLength;) {
            mLength = iInLength - mInLen> = mCH341A_CMD_I2C_STM_MAX? mCH341A_CMD_I2C_STM_MAX: iInLength - mInLen; // The length of valid data input
            mBuffer [i++] = (UCHAR) (mCH341A_CMD_I2C_STM_IN | mLength); // input data, bit 5 - bit 0 is the length of
            mInLen += mLength;
            if (mLength >= mCH341A_CMD_I2C_STM_MAX) {           // the current package will be full
                mBuffer [i] = mCH341A_CMD_I2C_STM_END;          // end of the current package in advance
                i += mCH341_PACKET_LENGTH - i% mCH341_PACKET_LENGTH; // skip the rest of the current package
                mBuffer [i++] = mCH341A_CMD_I2C_STREAM;         // new packet command code
            }
        }
        mBuffer [i++] = mCH341A_CMD_I2C_STM_IN;                 // last byte of input data, and send only one byte received no response
        mBuffer [i++] = mCH341A_CMD_I2C_STM_STO;                // stop bit generation
        mBuffer [i++] = mCH341A_CMD_I2C_STM_END;                // end of the current package in advance
        mLength = 0;
                                                                // execute command stream and then input the first output
        if (CH341WriteRead (iIndex, i, mBuffer, mCH341A_CMD_I2C_STM_MAX, (iInLength + mCH341A_CMD_I2C_STM_MAX - 1) 
                / mCH341A_CMD_I2C_STM_MAX, & mLength, oInBuffer)) {
            if (mLength == iInLength)
                return (TRUE);
        }
    }
    return (FALSE);
}

/* ************************************************ ********************************************** */
/* Example: operating quasi-bidirectional I / O expansion PCF8574 */

BOOL WINAPI PCF8574_WriteIO (// output the PCF8574 I / O
ULONG iIndex,       // ​​device number designated CH341
ULONG iDeviceAddr,  // ​​device address, the lowest order bit of the direction bit
ULONG iOutByte)     // prepare to write the I / O data
{                                                               // Could directly use CH341StreamI2C (iIndex, 2, mBuffer, 0, NULL) to achieve
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength;

    mBuffer [0] = mCH341A_CMD_I2C_STREAM;                       // command code
    mBuffer [1] = mCH341A_CMD_I2C_STM_STA;                      // generate the start bit
    mBuffer [2] = (UCHAR) (mCH341A_CMD_I2C_STM_OUT | 2);        // output data, bit 5 - bit 0 is the length of 2 bytes
    mBuffer [3] = (UCHAR) (iDeviceAddr & 0xFE);                 // device address, write
    mBuffer [4] = (UCHAR) iOutByte;                             // I / O data
    mBuffer [5] = mCH341A_CMD_I2C_STM_STO;                      // stop bit generation
    mBuffer [6] = mCH341A_CMD_I2C_STM_END;                      // end of the current package in advance
    mLength = 7;
    return (CH341WriteData (iIndex, mBuffer, & mLength));       // write the data block
}

// input the PCF8574 I / O
BOOL WINAPI PCF8574_ReadIO (
ULONG iIndex,               // ​​device number designated CH341
ULONG iDeviceAddr,          // ​​device address, the lowest order bit of the direction bit
PUCHAR oInByte)             // points to a byte buffer is read back after the I / O data
{                                                               // Could directly use CH341StreamI2C (iIndex, 1, mBuffer, 1, oInByte) to achieve
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength, mInLen;

    mBuffer [0] = mCH341A_CMD_I2C_STREAM;                       // command code
    mBuffer [1] = mCH341A_CMD_I2C_STM_STA;                      // generate the start bit
    mBuffer [2] = (UCHAR) (mCH341A_CMD_I2C_STM_OUT | 1);        // output data, bit 5 - bit 0 of length, 1 byte
    mBuffer [3] = (UCHAR) (iDeviceAddr | 0x01);                 // device address, read
    mBuffer [4] = (UCHAR) (mCH341A_CMD_I2C_STM_IN | 1);         // input data, bit 5 - bit 0 of length, 1 byte
    mBuffer [5] = mCH341A_CMD_I2C_STM_STO;                      // stop bit generation
    mBuffer [6] = mCH341A_CMD_I2C_STM_END;                      // end of the current package in advance
    mLength = 7;
    mInLen = 0;
                                                                // execute the command stream, and then input the first output
    if (CH341WriteRead (iIndex, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, & mInLen, mBuffer)) {  
        if (mInLen) {
        *OInByte = mBuffer[mInLen - 1]; // return the data
        return (TRUE);
        }
    }
    return (FALSE);
}

/* ************************************************ ********************************************** */
/* Example: two-way operation of I / O expansion PCA9554 */

BOOL WINAPI PCA9554_SetIO (     // set the PCA9554's I / O direction
ULONG iIndex,                   // ​​device number designated CH341
ULONG iDeviceAddr,              // ​​device address, the lowest order bit of the direction bit
ULONG iSetByte)                 // direction data
{                               // Can also be achieved with CH341WriteI2C
    UCHAR mBuffer [mCH341_PACKET_LENGTH];

    mBuffer [0] = (UCHAR) (iDeviceAddr & 0xFE); // device address, write
    mBuffer [1] = 0x03;
    mBuffer [2] = (UCHAR) iSetByte; // I / O direction of the data
    return (CH341StreamI2C (iIndex, 3, mBuffer, 0, NULL)); // I2C data stream processing
}


BOOL WINAPI PCA9554_WriteIO (// output PCA9554's I / O
ULONG iIndex, // ​​device number designated CH341
ULONG iDeviceAddr, // ​​device address, the lowest order bit of the direction bit
ULONG iOutByte) // prepare to write the I / O data
{// Can also be achieved with CH341WriteI2C

    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    mBuffer [0] = (UCHAR) (iDeviceAddr & 0xFE);             // device address, write
    mBuffer [1] = 0x01; 
    mBuffer [2] = (UCHAR) iOutByte;                         // I / O data
    return (CH341StreamI2C (iIndex, 3, mBuffer, 0, NULL));  // I2C data stream processing
}

BOOL WINAPI PCA9554_ReadIO (// input PCA9554's I / O
ULONG iIndex, // ​​device number designated CH341
ULONG iDeviceAddr, // ​​device address, the lowest order bit of the direction bit
PUCHAR oInByte) // points to a byte buffer is read back after the I / O data
{// Can also be achieved with CH341ReadI2C

    UCHAR mBuffer [mCH341_PACKET_LENGTH];

    mBuffer [0] = (UCHAR) (iDeviceAddr & 0xFE);             // device address, CH341StreamI2C handled automatically read
    mBuffer [1] = 0x00;
    return (CH341StreamI2C (iIndex, 2, mBuffer, 1, oInByte)); // I2C data stream processing
}

