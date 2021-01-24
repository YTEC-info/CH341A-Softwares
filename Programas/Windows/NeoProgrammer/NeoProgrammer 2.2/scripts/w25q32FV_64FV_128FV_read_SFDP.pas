// designed for WINBOND W25Q32FV/JV W25Q64FV/JV W25Q128FV/JV  (not BV serie)
// Read the Serial Flash Discoverable Parameter (SFDP) REGISTER (if supported)
// A message will be displayed if SFDP is not supported
// script can be adapted for other manufacturers and models by changing 'RegSize' value in each section
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

{$ Read_SFDP_Register}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read SFDP Register');
  RegSize := 256;
  sig:= CreateByteArray(4);
  // read SFDP register
  SPIWrite (0, 5, $5A, $00, $00, $00, $00);
  SPIReadToEditor (1, RegSize);
  // test if SFDP supported
  WriteFromEditor(4, 0, sig);
  if GetArrayItem(sig, 0) <> $53 then ShowMessage('SFDP not supported by this Device');
  LogPrint ('End read SFDP Register');
  SPIExitProgMode ();
end
 

