// designed for WINBOND W25Q32FV/JV W25Q64FV/JV W25Q128FV/JV  (not BV serie)
// READ  the 3 Serial Flash Discoverable Parameter (SFDP) REGISTERS
// note: W25Q32FV have only one SFDP register
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

{$ Read_SFDP_Register_1}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read SFDP Register #1');
  RegSize := 256;
  // read register 1
  SPIWrite (0, 5, $5A, $00, $10, $00, $00);
  SPIReadToEditor (1, RegSize);
   LogPrint ('End read SFDP Register #1');
  SPIExitProgMode ();
end
 
 
{$ Read_SFDP_Register_2}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read SFDP Register #2');
  RegSize := 256;
  // read register 2
  SPIWrite (0, 5, $5A, $00, $20, $00, $00);
  SPIReadToEditor (1, RegSize);
   LogPrint ('End read SFDP Register #2');
  SPIExitProgMode ();
end

{$ Read_SFDP_Register_3}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read SFDP Register #3');
  RegSize := 256;
  // read register 3
  SPIWrite (0, 5, $5A, $00, $30, $00, $00);
  SPIReadToEditor (1, RegSize);
   LogPrint ('End read SFDP Register #3');
  SPIExitProgMode ();
end
