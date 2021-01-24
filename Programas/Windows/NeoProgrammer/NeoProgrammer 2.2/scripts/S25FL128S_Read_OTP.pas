// designed for
{$ READ_JEDEC_ID}
begin
  ID:= CreateByteArray(3);
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read JEDEC ID');
  
  // read ID to test installation 
  SPIWrite (0, 1, $9F);
  SPIRead(1, 3, ID);
  
  logprint('Read ID: ' + inttohex((GetArrayItem(ID, 0)),2)+ inttohex((GetArrayItem(ID, 1)),2)+ inttohex((GetArrayItem(ID, 2)),2));
  LogPrint ('End read JEDEC ID');
  SPIExitProgMode ();
end

{$ Read_OTP}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read OTP');
  RegSize := 1024;
  SPIWrite (0, 5, $4B, $00, $00, $00, $00);
  SPIReadToEditor (1, RegSize);
  LogPrint ('0-15 bytes = Random serial number');
  LogPrint ('16-17 bytes = 32 LockBits for OTP area (1 bit for each 32 byte OTP)');
  LogPrint ('18-31 bytes = Reserved');
  LogPrint ('Other = Writable OTP area ');
  LogPrint ('End read OTP');
  SPIExitProgMode ();
end
 

