// designed for Eon EN25F80
// READ / WRITE / ERASE OTP PAGE, read and set OTP_LOCK bit

{$ READ_ID}
begin
  ID_9F:= CreateByteArray(3);
  ID_90:= CreateByteArray(2);
  ID_AB:= CreateByteArray(2);
  ID_15:= CreateByteArray(2);
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read ID');

  // read ID
  SPIWrite (0, 1, $9F);
  SPIRead(1, 3, ID_9F);
  logprint('ID(9F): ' + inttohex((GetArrayItem(ID_9F, 0)),2)+ inttohex((GetArrayItem(ID_9F, 1)),2)+ inttohex((GetArrayItem(ID_9F, 2)),2));

  SPIWrite (0, 4, $90, 0, 0, 0);
  SPIRead(1, 2, ID_90);
  logprint('ID(90): ' + inttohex((GetArrayItem(ID_90, 0)),2)+ inttohex((GetArrayItem(ID_90, 1)),2));

  SPIWrite (0, 4, $AB, 0, 0, 0);
  SPIRead(1, 2, ID_AB);
  logprint('ID(AB): ' + inttohex((GetArrayItem(ID_AB, 0)),2)+ inttohex((GetArrayItem(ID_AB, 1)),2));

  SPIWrite (0, 1, $15);
  SPIRead(1, 2, ID_15);
  logprint('ID(15): ' + inttohex((GetArrayItem(ID_15, 0)),2)+ inttohex((GetArrayItem(ID_15, 1)),2));

  LogPrint ('End read ID ');
  SPIExitProgMode ();
end

{$ READ_OTP_LOCK_BIT}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  sreg :=0;
  SPIWrite (1, 1, $3A);                      //Enter OTP mode
  SPIWrite (0, 1, $05);
  SPIRead  (1, 1, sreg);
  SPIWrite (1, 1, $04);                      //Exit OTP mode
  if (sreg and $80) then LogPrint ('OTP LockBit = 1')
  else                   LogPrint ('OTP LockBit = 0');

  SPIExitProgMode ();
end

{$ READ_OTP_PAGE}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read OTP page of EN25F80');
  RegSize :=256;
  SPIWrite (1, 1, $3A);                      //Enter OTP mode
  SPIWrite (0, 4, $03, $0F, $F0, $00);       //First page of 255 sector (0FF000)
  SPIReadToEditor (1, RegSize);
  SPIWrite (1, 1, $04);                      //Exit OTP mode

  LogPrint ('End read OTP page of EN25F80');
  SPIExitProgMode ();
end

{$ ERASE_OTP_PAGE}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start Erase OTP page of EN25F80');
  sreg := 0;
  SPIWrite (1, 1, $06);                     // write enable
  SPIWrite (1, 1, $3A);                     // Enter OTP mode

  SPIWrite (0, 1, $05);
  SPIRead  (1, 1, sreg);
  if       (sreg and $80) then LogPrint ('OTP LockBit = 1, write or erase not possible anymore!')
  else if  (sreg and $1C) then LogPrint ('BP bits are set, please check STATUS')
  else SPIWrite (1, 4, $20, $0F, $F0, $00); // Erase sector 255 (0FF000)

  SPIWrite (1, 1, $04);                     //Exit OTP mode
  LogPrint ('End Erase OTP page of EN25F80');
  SPIExitProgMode ();
end

{$ WRITE_OTP_PAGE}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write OTP page of EN25F80');
  RegSize :=256;
  sreg := 0;

  SPIWrite (1, 1, $06);                     // write enable
  SPIWrite (1, 1, $3A);                     // Enter OTP mode

  SPIWrite (0, 1, $05);
  SPIRead  (1, 1, sreg);
  if       (sreg and $80) then LogPrint ('OTP LockBit = 1, write or erase not possible anymore!')
  else if  (sreg and $1C) then LogPrint ('BP bits are set, please check STATUS')
  else begin
       SPIWrite (0, 4, $02, $0F, $F0, $00);      // First page of 255 sector (0FF000)
       SPIWriteFromEditor(1, RegSize, 0);
       repeat                                    //Busy
         SPIWrite(0, 1, $05);
         SPIRead(1, 1, sreg);
       until((sreg and 1) <> 1);
       end;
  SPIWrite (1, 1, $04);                     // write disable
  LogPrint ('End write OTP page of EN25F80');
  SPIExitProgMode ();
end

{$ SET_OTP_LOCK_BIT_!!!}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  sreg :=0;
  SPIWrite (0, 1, $05);
  SPIRead  (1, 1, sreg);
  if  (sreg and $1C) then LogPrint ('BP bits are set, please check STATUS')
  else 
  begin
    repeat
      Confirm := InputBox('Are you really SHURE??? (YES/NO)','','NO');
    until (Confirm = 'YES') or (Confirm = 'NO');
    if (Confirm = 'YES') then
    begin
      SPIWrite (1, 1, $06);                     // write enable
      SPIWrite (1, 1, $3A);                     // Enter OTP mode
      SPIWrite (1, 2, $01, $00);                // WRSR without data to set OTP_LOCK bit
      SPIWrite (1, 1, $04);                     // Exit OTP mode
      LogPrint ('OTP LockBit written!');
    end
    else LogPrint ('OTP LockBit not written, good choice!');
 end;
  SPIExitProgMode ();
end

