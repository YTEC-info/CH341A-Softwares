
{$ READ_ID}
begin
  ID_9F:= CreateByteArray(3);
  ID_90:= CreateByteArray(2);
  ID_AB:= CreateByteArray(2);
  ID_15:= CreateByteArray(4);
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
  SPIRead(1, 4, ID_15);
  if GetArrayItem(ID_15, 0) = $7F then
  logprint('ID(15): ' + inttohex((GetArrayItem(ID_15, 0)),2) + inttohex((GetArrayItem(ID_15, 1)),2)
                      + inttohex((GetArrayItem(ID_15, 2)),2) + inttohex((GetArrayItem(ID_15, 3)),2))
  else
  logprint('ID(15): ' + inttohex((GetArrayItem(ID_15, 0)),2)+ inttohex((GetArrayItem(ID_15, 1)),2));  
  
  LogPrint ('End read ID');
  SPIExitProgMode ();
end
