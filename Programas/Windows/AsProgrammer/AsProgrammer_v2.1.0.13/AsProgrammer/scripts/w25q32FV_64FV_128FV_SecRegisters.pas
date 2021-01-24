// designed for WINBOND W25Q32FV/JV W25Q64FV/JV W25Q128FV/JV  (not BV serie)
// READ / WRITE / ERASE Security REGISTERS
// script can be adapted for other brand and models by changing 'RegSize' value in each section
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

{$ READ_UNIQUE_ID}
begin
  UID:= CreateByteArray(8);
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read UNIQUE ID');
  
  // read 64bits UNIQUE ID 
  SPIWrite (0, 5, $4B, $00, $00, $00, $00);
  SPIRead(1, 8, UID);
  
  logprint('Read UID: ' + inttohex((GetArrayItem(UID, 0)),2)+ inttohex((GetArrayItem(UID, 1)),2)+ inttohex((GetArrayItem(UID, 2)),2)+ inttohex((GetArrayItem(UID, 3)),2)
                        + inttohex((GetArrayItem(UID, 4)),2)+ inttohex((GetArrayItem(UID, 5)),2)+ inttohex((GetArrayItem(UID, 6)),2)+ inttohex((GetArrayItem(UID, 7)),2));
  LogPrint ('End read UNIQUE ID');
  SPIExitProgMode ();
end

{$ READ_SECURITY_REGISTER_1}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read security register #1');
  RegSize := 256;
  // read register 1
  SPIWrite (0, 5, $48, $00, $10, $00, $00);
  SPIReadToEditor (1, RegSize);

  LogPrint ('End read security register #1');
  SPIExitProgMode ();
end

{$ READ_SECURITY_REGISTER_2}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read security register #2');
  RegSize := 256;
  // read register 2
  SPIWrite (0, 5, $48, $00, $20, $00, $00);
  SPIReadToEditor (1, RegSize);

  LogPrint ('End read security register #2');
  SPIExitProgMode ();
end

{$ READ_SECURITY_REGISTER_3}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read security register #3');
  RegSize := 256;
  // read register 3
  SPIWrite (0, 5, $48, $00, $30, $00, $00);
  SPIReadToEditor (1, RegSize);

  LogPrint ('End read security register #3');
  SPIExitProgMode ();
end

{$ ERASE_SECURITY_REGISTER_1}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start Erase security register #1');
  // write enable
  SPIWrite (1, 1, $06);
  // erase register 1
  SPIWrite (1, 4, $44, $00, $10, $00);

  LogPrint ('End Erase security register #1');
  SPIExitProgMode ();
end

{$ ERASE_SECURITY_REGISTER_2}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start Erase security register #2');

  // write enable
  SPIWrite (1, 1, $06);
  // erase register 2
  SPIWrite (1, 4, $44, $00, $20, $00);

  LogPrint ('End Erase security register #2');
  SPIExitProgMode ();
end

{$ ERASE_SECURITY_REGISTER_3}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start Erase security register #3');

  // write enable
  SPIWrite (1, 1, $06);
  // erase register 3
  SPIWrite (1, 4, $44, $00, $30, $00);

  LogPrint ('End Erase security register #3');
  SPIExitProgMode ();
end

{$ WRITE_SECURITY_REGISTER_1}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write security register #1');
  RegSize := 256;
  sreg := 0;
   
  // write enable
  SPIWrite (1, 1, $06);

  // write register 1
  SPIWrite (0, 4, $42, $00, $10, $00);
  SPIWriteFromEditor(1, Regsize, 0);

  //Busy
  repeat
    SPIWrite(0, 1, $05);
    SPIRead(1, 1, sreg);
  until((sreg and 1) <> 1);
  
  // write disable
  SPIWrite (1, 1, $04);
  LogPrint ('End write security register #1');
  SPIExitProgMode ();
end

{$ WRITE_SECURITY_REGISTER_2}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write security register #2');
  RegSize := 256;
  sreg := 0;
   
  // write enable
  SPIWrite (1, 1, $06);

  // write register 2
  SPIWrite (0, 4, $42, $00, $20, $00);
  SPIWriteFromEditor(1, Regsize, 0);

  //Busy
  repeat
    SPIWrite(0, 1, $05);
    SPIRead(1, 1, sreg);
  until((sreg and 1) <> 1);

  // write disable
  SPIWrite (1, 1, $04);
  LogPrint ('End write security register #2');
  SPIExitProgMode ();
end

{$ WRITE_SECURITY_REGISTER_3}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write security register #3');
  RegSize := 256;
  sreg := 0;

  // write enable
  SPIWrite (1, 1, $06);

  // write register 3
  SPIWrite (0, 4, $42, $00, $30, $00);
  SPIWriteFromEditor(1, Regsize, 0);

  //Busy
  repeat
    SPIWrite(0, 1, $05);
    SPIRead(1, 1, sreg);
  until((sreg and 1) <> 1);

  // write disable
  SPIWrite (1, 1, $04);
  LogPrint ('End write security register #3');
  SPIExitProgMode ();
end

