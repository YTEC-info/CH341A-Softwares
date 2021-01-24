// designed for GIGADEVICE GD25Q64 and GD25Q128
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

{$ READ_SECURITY_REGISTER_1_GD25Q64_128}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read GD25Q64_128 security register #1');
  RegSize :=1024;
  // read register 1
  SPIWrite (0, 5, $48, $00, $10, $00, $00);
  SPIReadToEditor (1, RegSize);

  LogPrint ('End read GD25Q64_128 security register #1');
  SPIExitProgMode ();
end

{$ READ_SECURITY_REGISTER_2_GD25Q64_128}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read GD25Q64_128 security register #2');
  RegSize :=1024;
  // read register 2
  SPIWrite (0, 5, $48, $00, $20, $00, $00);
  SPIReadToEditor (1, RegSize);

  LogPrint ('End read GD25Q64_128 security register #2');
  SPIExitProgMode ();
end

{$ READ_SECURITY_REGISTER_3_GD25Q64_128}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read GD25Q64_128 security register #3');
  RegSize :=1024;
  // read register 3
  SPIWrite (0, 5, $48, $00, $30, $00, $00);
  SPIReadToEditor (1, RegSize);

  LogPrint ('End read GD25Q64_128 security register #3');
  SPIExitProgMode ();
end

{$ ERASE_SECURITY_REGISTER_1_GD25Q64_128}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start Erase GD25Q64_128 security register #1');
  // write enable
  SPIWrite (1, 1, $06);
  // erase register 1
  SPIWrite (1, 4, $44, $00, $10, $00);

  LogPrint ('End Erase GD25Q64_128 security register #1');
  SPIExitProgMode ();
end

{$ ERASE_SECURITY_REGISTER_2_GD25Q64_128}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start Erase GD25Q64_128 security register #2');

  // write enable
  SPIWrite (1, 1, $06);
  // erase register 2
  SPIWrite (1, 4, $44, $00, $20, $00);

  LogPrint ('End Erase GD25Q64_128 security register #2');
  SPIExitProgMode ();
end

{$ ERASE_SECURITY_REGISTER_3_GD25Q64_128}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start Erase GD25Q64_128 security register #3');

  // write enable
  SPIWrite (1, 1, $06);
  // erase register 3
  SPIWrite (1, 4, $44, $00, $30, $00);

  LogPrint ('End Erase GD25Q64_128 security register #3');
  SPIExitProgMode ();
end

{$ WRITE_SECURITY_REGISTER_1_GD25Q64_128}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write GD25Q64_128 security register #1');
  RegSize :=1024;
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
  LogPrint ('End write GD25Q64_128 security register #1');
  SPIExitProgMode ();
end

{$ WRITE_SECURITY_REGISTER_2_GD25Q64_128}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write GD25Q64_128 security register #2');
  RegSize :=1024;
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
  LogPrint ('End write GD25Q64_128 security register #2');
  SPIExitProgMode ();
end

{$ WRITE_SECURITY_REGISTER_3_GD25Q64_128}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write GD25Q64_128 security register #3');
  RegSize :=1024;
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
  LogPrint ('End write GD25Q64_128 security register #3');
  SPIExitProgMode ();
end

