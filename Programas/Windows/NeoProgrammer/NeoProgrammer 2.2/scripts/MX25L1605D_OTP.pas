// designed for MACRONIX MX25L1605D
// READ / WRITE 512-bit OTP PAGE, read and set OTP_LOCK bit
// WRITE is really OTP operation, be careful, OTP page can not be erased.
{$ READ_ID}
begin
  ID_9F:= CreateByteArray(3);
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read ID');
  // read ID
  SPIWrite (0, 1, $9F);
  SPIRead(1, 3, ID_9F);
  logprint('ID(9F): ' + inttohex((GetArrayItem(ID_9F, 0)),2)+ inttohex((GetArrayItem(ID_9F, 1)),2)+ inttohex((GetArrayItem(ID_9F, 2)),2));
  LogPrint ('End read ID ');
  SPIExitProgMode ();
end

{$ READ_OTP_LOCK_BIT}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  sreg :=0;
  SPIWrite (0, 1, $2B);
  SPIRead  (1, 1, sreg);
  if       (sreg and $01) then LogPrint ('Factory OTP LockBit = 1')
  else if  (sreg and $02) then LogPrint ('Customer OTP LockBit = 1')
  else                         LogPrint ('OTP LockBit = 0');

  SPIExitProgMode ();
end

{$ READ_OTP_PAGE}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start READ 512-bit OTP page of MX25L1605D');
  RegSize :=64;
  SPIWrite (1, 1, $B1);                      //Enter OTP mode
  SPIWrite (0, 4, $03, $00, $00, $00);       //First page of 255 sector (0FF000)
  SPIReadToEditor (1, RegSize);
  SPIWrite (1, 1, $C1);                      //Exit OTP mode

  LogPrint ('End read OTP page of MX25L1605D');
  SPIExitProgMode ();
end


{$ WRITE_OTP_PAGE}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  RegSize :=64;
  sreg :=0;
  repeat       // Write is really OTP, so also confirm
  Confirm := InputBox('Are you really SHURE??? (YES/NO)','','NO');
  until (Confirm = 'YES') or (Confirm = 'NO');
  if (Confirm = 'YES') then
     begin
     LogPrint ('Start WRITE 512-bit OTP page of MX25L1605D');
     SPIWrite (1, 1, $B1);                     // Enter OTP mode
     SPIWrite (0, 1, $2B);
     SPIRead  (1, 1, sreg);
     if (sreg and $03) then LogPrint ('OTP LockBit = 1, write not possible anymore!')
     else for i:=0 to RegSize-1 do
       begin
       SPIWrite (1, 1, $06);                     // write enable
       SPIWrite (0, 4, $02, $00, $00, i);        // write byte-by-byte only!
       SPIWriteFromEditor(1, 1, i);
       repeat                                    //Busy
       SPIWrite(0, 1, $05);
       SPIRead(1, 1, sreg);
       until((sreg and 1) <> 1);
       end;
     SPIWrite (1, 1, $C1);                      //Exit OTP mode
     LogPrint ('End write OTP page of MX25L1605D');
     end;
     SPIExitProgMode ();
end

{$ SET_OTP_LOCK_BIT}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  sreg :=0;
  repeat
  Confirm := InputBox('Are you really SHURE??? (YES/NO)','','NO');
  until (Confirm = 'YES') or (Confirm = 'NO');
  if (Confirm = 'YES') then
     begin
     SPIWrite (1, 1, $2F);                     // write secure reg (customer lock)
     LogPrint ('Customer OTP LockBit written!');
     end
  else LogPrint ('OTP LockBit not written, good choice!');
  SPIExitProgMode ();
end

