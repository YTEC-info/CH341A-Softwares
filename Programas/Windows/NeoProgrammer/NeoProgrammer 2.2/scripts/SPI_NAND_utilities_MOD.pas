// designed for SPI NAND
// Read first (full) page of a given block
// Read given (full) page
// Read BBM Look Up Table (winbond)
// Erase a block
// write a given page (full or not)
{$ READ_JEDEC_ID}
begin
  ID:= CreateByteArray(2);
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Read JEDEC ID');
  
  // read ID to test installation 
  SPIWrite (0, 2, $9F, $00);
  SPIRead(1, 2, ID);
  
  logprint('CHIP ID: ' + inttohex((GetArrayItem(ID, 0)),2)+ inttohex((GetArrayItem(ID, 1)),2));
  LogPrint ('End read JEDEC ID');
  SPIExitProgMode ();
end

{$ ERASE_a_BLOCK}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Erase a given block');
  sreg :=$FF;
  buff:= CreateByteArray(4);
  maxpage:= _IC_Size / _IC_Page;
  
  repeat
    BlockNum := InputBox('Enter Block Number (0 to '+ inttostr((maxpage / 64)-1)+')','','0');
  until (BlockNum >=0) and (BlockNum <=(maxpage / 64)-1);

  //reset chip
  SPIWrite(1, 1, $FF);
  // write enable
  SPIWrite (1, 1, $06);
  // reset protection bits
  SPIWrite(1, 3, $1F, $A0, 0);
  // wait if busy
  repeat
    SPIWrite(0, 2, $0F, $C0);
    SPIRead(1, 1, sreg);
  until((sreg and 1) <> 1);
    
  Addr:= BlockNum * 64;
  SetArrayItem(buff, 0, $D8);
  SetArrayItem(buff, 1, (addr shr 16)); // div 65536
  SetArrayItem(buff, 2, (addr shr 8));  // div 256
  SetArrayItem(buff, 3, (addr));
  // erase block
  SPIWrite (1, 4, buff);
  
  // wait if busy
  repeat
    SPIWrite(0, 2, $0F, $C0);
    SPIRead(1, 1, sreg);
  until((sreg and 1) <> 1);

  LogPrint ('End erase block');
  SPIExitProgMode ();
end

{$ READ_64_PAGE}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Read (full) given page');
  sreg :=$FF;
  buff:= CreateByteArray(4);
  bufflen:= _IC_Page + _IC_Spare;
  maxpage:= 64; //_IC_Size / _IC_Page; //uncomment to read full IC
//  repeat
//    PageNum := InputBox('Enter page Number (0 to ' +inttostr(maxpage-1) +')','','0');
//  until (PageNum >=0) and (PageNum <= maxpage-1);

  //reset chip
  SPIWrite(1, 1, $FF);

  for PageNum:=0 to maxpage-1 do
  begin
       Addr:= PageNum;
       SetArrayItem(buff, 0, $13);
       SetArrayItem(buff, 1, (addr shr 16)); // div 65536
       SetArrayItem(buff, 2, (addr shr 8));  // div 256
       SetArrayItem(buff, 3, (addr));
       // transfer page datas to cache
       SPIWrite (1, 4, buff);
  
       // wait if busy
       repeat
             SPIWrite(0, 2, $0F, $C0);
             SPIRead(1, 1, sreg);
       until((sreg and 1) <> 1);

       SetArrayItem(buff, 0, $03);
       SetArrayItem(buff, 1, 0);
       SetArrayItem(buff, 2, 0);
       SetArrayItem(buff, 3, 0); // dummy byte
       SPIWrite (0, 4, buff);
       // read cache from 0, bufflen length
       SPIReadToEditor (1, bufflen);
       ProgressBar(1);
       //LogPrint ('End read page' );
  end;
  SPIExitProgMode ();
end

{$ WRITE_64_PAGE}
begin
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Write data to given page');
  sreg :=$FF;
  buff:= CreateByteArray(4);
  buffer:= CreateByteArray(_IC_Page+_IC_Spare);
  bufflen:= _IC_Page + _IC_Spare;
  maxpage:= 64; //_IC_Size / _IC_Page; //uncomment to write full IC
  
//  repeat
//    PageNum := InputBox('Enter destination page Number (0 to ' +inttostr(maxpage-1) +')','','0');
//  until (PageNum >=0) and (PageNum <= maxpage-1);
  
//  repeat
//    StartAddr := InputBox('Editor Start (decimal) Address (0, 2048,...)','','0');
//  until (StartAddr >=0) and (StartAddr <=_IC_Size -_IC_Page);

  //reset chip
  SPIWrite(1, 1, $FF);
  // write enable
  SPIWrite (1, 1, $06);
  // reset protection bits
  SPIWrite(1, 3, $1F, $A0, 0);
  // wait if busy
  repeat
    SPIWrite(0, 2, $0F, $C0);
    SPIRead(1, 1, sreg);
  until((sreg and 1) <> 1);

  for PageNum:=0 to maxpage-1 do
      begin
      StartAddr:=PageNum*bufflen;
      // transfer editor datas to buffer array
      WriteFromEditor (bufflen, StartAddr, buffer);
      SPIWrite (1, 1, $06);           // write enable

      SetArrayItem(buff, 0, $02);     // write datas to cache
      SetArrayItem(buff, 1, 0);
      SetArrayItem(buff, 2, 0);

      SPIWrite (0, 3, buff);
      SPIWrite (1, bufflen, buffer);
  
      // transfer datas from cache to the specified memory page.
      Addr:= PageNum;
      SetArrayItem(buff, 0, $10);
      SetArrayItem(buff, 1, (addr shr 16)); // div 65536
      SetArrayItem(buff, 2, (addr shr 8));  // div 256
      SetArrayItem(buff, 3, (addr));

      SPIWrite (1, 4, buff);

      // wait if busy
      repeat
            SPIWrite(0, 2, $0F, $C0);
            SPIRead(1, 1, sreg);
      until((sreg and 1) <> 1);
      end;
    //LogPrint ('End write page');
  SPIExitProgMode ();
end

{$ READ_BBM_table}
begin
  BBM:= CreateByteArray(80);
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Read winbond BBM table');
  
  // read winbond BBM table 
  SPIWrite (0, 2, $A5, $00);
  SPIRead(1, 80, BBM);
  
  for i:=0 to 19 do
  logprint('LBA' +inttostr(i)+': ' + inttohex((GetArrayItem(BBM, i+0)),2)  + inttohex((GetArrayItem(BBM, i+1)),2)  + ' PBA' +inttostr(i)+': ' + inttohex((GetArrayItem(BBM, i+2)),2) + inttohex((GetArrayItem(BBM, i+3)),2));
  
  LogPrint ('End read BBM table');
  SPIExitProgMode ();
end
