// for SPI FLASH 128k max (24 bits address)
{$erase} //секция будет выполняться при нажатии кнопки "стереть"
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  
  BlockSize := 65536; //block size in byte
  sreg := 0;
  buff:= CreateByteArray(4);

  ProgressBar(0, (_IC_SIZE / BlockSize)-1, 0);

  for i:= 0 to (_IC_SIZE / BlockSize)-1 do
  begin
    SPIWrite(1, 1, $06); //wren
    Addr:= i * BlockSize;
    SetArrayItem(buff, 0, $D8);
    SetArrayItem(buff, 1, (addr shr 16));
    SetArrayItem(buff, 2, (addr shr 8));
    SetArrayItem(buff, 3, (addr));
    SPIWrite(1, 4, buff); 

    //Busy?
    repeat
      SPIWrite(0, 1, $05);
      SPIRead(1, 1, sreg);
    until((sreg and 1) <> 1);
    ProgressBar(1);
  end;

  ProgressBar(0, 0, 0);
  SPIExitProgMode();
end
