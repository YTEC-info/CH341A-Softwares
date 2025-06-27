
{$ READ_ID}
begin
  ID_9F:= CreateByteArray(3);
  ID_90:= CreateByteArray(2);
  ID_AB:= CreateByteArray(2);
  ID_15:= CreateByteArray(2);
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read ID');
  
  // read ID to test installation 
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


{$ READ_BUFFER_1}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read AT45DB161E Buffer 1 ');
  //PageSize :=_IC_Page;
  PageSize := 1056;
  buff:= CreateByteArray(5);
  SetArrayItem(buff, 0, $D4); // read buffer 1 opcode
  SetArrayItem(buff, 1, 0);
  SetArrayItem(buff, 2, 0);
  SetArrayItem(buff, 3, 0);
  SetArrayItem(buff, 4, 0);
  SPIWrite (0, 5, buff);
  SPIReadToEditor (1, PageSize);

  LogPrint ('End ');
  SPIExitProgMode ();
end

{$ WRITE_BUFFER_1}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write AT45DB161E Buffer 1 ');
  //PageSize :=_IC_Page;
  PageSize := 1056;
  buff:= CreateByteArray(4);
  SetArrayItem(buff, 0, $84); // write buffer 1 opcode
  SetArrayItem(buff, 1, 0);
  SetArrayItem(buff, 2, 0);
  SetArrayItem(buff, 3, 0);
  SPIWrite (0, 4, buff);
  SPIWriteFromEditor (1, PageSize, 0);

  LogPrint ('End ');
  SPIExitProgMode ();
end


{$ ENABLE_SECTOR_PROTECTION}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  SPIWrite (1, 4, $3D, $2A, $7F, $A9); //enable
  LogPrint ('Sector protection enabled');
  SPIExitProgMode ();
end

{$ DISABLE_SECTOR_PROTECTION}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  SPIWrite (1, 4, $3D, $2A, $7F, $9A); //disable
  LogPrint ('Sector protection disabled');
  SPIExitProgMode ();
end

{$ READ_SECTOR_PROTECTION_REGISTER}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read AT45DB161E sector protection register ');
  RegSize :=16;
  // read register
  SPIWrite (0, 4, $32, $00, $00, $00);
  SPIReadToEditor (1, RegSize);

  LogPrint ('End read AT45DB161E sector protection register ');
  SPIExitProgMode ();
end

{$ WRITE_SECTOR_PROTECTION_REGISTER}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write AT45DB161E sector protection register ');
  RegSize :=16;
  sreg := 0;
  SPIWrite (1, 4, $3D, $2A, $7F, $CF); //erase
  repeat                               //Busy 45
    SPIWrite(0, 1, $D7);
    SPIRead(1, 1, sreg);
//    LogPrint ('busy, sreg=' + inttohex(sreg,2));
  until((sreg and $80) = $80);

  SPIWrite (0, 4, $3D, $2A, $7F, $FC); //write
  SPIWriteFromEditor(1, RegSize, 0);
  repeat                               //Busy 45
    SPIWrite(0, 1, $D7);
    SPIRead(1, 1, sreg);
  until((sreg and $80) = $80);

  LogPrint ('End write AT45DB161E sector protection register ');
  SPIExitProgMode ();
end

{$ READ_SECTOR_LOCKDOWN_REGISTER}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read AT45DB161E sector lockdown register ');
  RegSize :=16;
  // read register
  SPIWrite (0, 4, $35, $00, $00, $00);
  SPIReadToEditor (1, RegSize);

  LogPrint ('End read AT45DB161E sector lockdown register ');
  SPIExitProgMode ();
end

{$ READ_SECURITY_REGISTER}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read AT45DB161E security register ');
  LogPrint ('00..3F - One-Time User Programmable Area');
  LogPrint ('40..7F - Factory Programmed Area (S/N)');
  RegSize :=128;
  // read register
  SPIWrite (0, 4, $77, $00, $00, $00);
  SPIReadToEditor (1, RegSize);

  LogPrint ('End read AT45DB161E security register ');
  SPIExitProgMode ();
end

{$ WRITE_SECURITY_REGISTER_OTP}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write AT45DB161E security register (OTP)');
  LogPrint ('00..3F - One-Time User Programmable Area');
  RegSize :=64;
  repeat
  Confirm := InputBox('Are you really SHURE??? (YES/NO)','','NO');
  until (Confirm = 'YES') or (Confirm = 'NO');
  if (Confirm = 'YES') then
    begin
    SPIWrite (0, 4, $9B, $00, $00, $00); //write
    SPIWriteFromEditor(1, RegSize, 0);
    LogPrint ('OTP security register written!');
    end
  else LogPrint ('OTP security register not written, good choice!');

  LogPrint ('End write AT45DB161E security register (OTP)');
  SPIExitProgMode ();
end

