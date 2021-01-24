// read_write_95xxx_example.pas
// EEPROM 95xxx example of read & write
// useless, working script just for learning purpose
// READ_MEMORY_IN_ARRAY must be done in assocation with uch_95040_test.bin
cvx
{$ READ_MEMORY}
begin
  if _IC_Name = '' then
  begin
    ShowMessage ('Choose a component in list');
  exit;
  end;
  
  buff:= CreateByteArray(4); // create an array of 4 bytes
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed'); // mandatory programmer initialisation
  LogPrint ('Start read memory'); // blablabla
  //ChipSize := 512; // size of memory in byte (manual)
  ChipSize := _IC_Size; // to recover the size of the chip declared in the application.

  // addressing for 8, 16, 24 bit mode
  SetArrayItem(buff, 0, $03); // $03 read EEprom opcode
  SetArrayItem(buff, 1, 0); // equivalent of buff[1]:= Addr in pure pascal
  SetArrayItem(buff, 2, 0);
  SetArrayItem(buff, 3, 0);

  len := 2;                    // length value depends of addressing mode
                               // 2 for 8bit mode, 3 for 16 bit, 4 for 24 bit
  if (ChipSize > 512) and (ChipSize <= 65536) then len := 3;
  if (ChipSize > 65536)  then len := 4;

  SPIWrite(0, len, buff); // send the read command to the chip, 0 is the state of Chip select (CS) at the end of the command
  SPIReadToEditor (1, ChipSize); // then, read the content of memory and put values in the editor, (CS) high and the end

  LogPrint ('End read memory');
  SPIExitProgMode ();  // close programmer
end

{$ READ_MEMORY_IN_ARRAY}
begin
  if _IC_Name = '' then
  begin
    ShowMessage ('Choose a component in list');
  exit;
  end;
  
  buff:= CreateByteArray(4);
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start read memory');
  //ChipSize := 512; // size of memory in byte (manual)
  ChipSize := _IC_Size; // to recover the size of the chip declared in the application.

  ReadArray:= CreateByteArray(ChipSize); // create an array of chipsize bytes
  ASCIItable:= CreateStringArray(128);

  // addressing for 8, 16, 24 bit mode
  SetArrayItem(buff, 0, $03); // $03 read EEprom opcode
  SetArrayItem(buff, 1, 0); // equivalent of buff[1]:= Addr in pure pascal
  SetArrayItem(buff, 2, 0);
  SetArrayItem(buff, 3, 0);

  len := 2;                    // length value depends of addressing mode
                               // 2 for 8bit mode, 3 for 16 bit, 4 for 24 bit
  if (ChipSize > 512) and (ChipSize <= 65536) then len := 3;
  if (ChipSize > 65536)  then len := 4;;

  SPIWrite(0, len, buff);
  // read the content of memory and put values in an array
  SPIRead(1, ChipSize, ReadArray);
  
  // build ASCII table  0..9, A..Z  (upcases)
  for i:= 0 to 127 do
  begin
    SetArrayItem(ASCIItable, i, ' '); // initialise the table with spaces
  end;

  for i:= $30 to $39 do
  begin
    SetArrayItem(ASCIItable, i, inttostr(i-$30));
  end;
  i:=$40;
  inc (i);SetArrayItem(ASCIItable, i, 'A');
  inc (i);SetArrayItem(ASCIItable, i, 'B');
  inc (i);SetArrayItem(ASCIItable, i, 'C');
  inc (i);SetArrayItem(ASCIItable, i, 'D');
  inc (i);SetArrayItem(ASCIItable, i, 'E');
  inc (i);SetArrayItem(ASCIItable, i, 'F');
  inc (i);SetArrayItem(ASCIItable, i, 'G');
  inc (i);SetArrayItem(ASCIItable, i, 'H');
  inc (i);SetArrayItem(ASCIItable, i, 'I');
  inc (i);SetArrayItem(ASCIItable, i, 'J');
  inc (i);SetArrayItem(ASCIItable, i, 'K');
  inc (i);SetArrayItem(ASCIItable, i, 'L');
  inc (i);SetArrayItem(ASCIItable, i, 'M');
  inc (i);SetArrayItem(ASCIItable, i, 'N');
  inc (i);SetArrayItem(ASCIItable, i, 'O');
  inc (i);SetArrayItem(ASCIItable, i, 'P');
  inc (i);SetArrayItem(ASCIItable, i, 'Q');
  inc (i);SetArrayItem(ASCIItable, i, 'R');
  inc (i);SetArrayItem(ASCIItable, i, 'S');
  inc (i);SetArrayItem(ASCIItable, i, 'T');
  inc (i);SetArrayItem(ASCIItable, i, 'U');
  inc (i);SetArrayItem(ASCIItable, i, 'V');
  inc (i);SetArrayItem(ASCIItable, i, 'W');
  inc (i);SetArrayItem(ASCIItable, i, 'X');
  inc (i);SetArrayItem(ASCIItable, i, 'Y');
  inc (i);SetArrayItem(ASCIItable, i, 'Z');

  s0:='';
  // build VIN number
  for i:= $1AD to $1BD do
  begin
    s0:= s0 + GetArrayItem(ASCIItable, (GetArrayItem(ReadArray, i)));  
  end;
  s0:= 'VIN = ' + s0;
  logprint(s0);

  s1:='';
  // build ISK number
  for i:= $13A to $13F do
  begin
    GetArrayItem(ReadArray, i);
    s1:= s1 + inttohex(GetArrayItem(ReadArray, i),2) + ' ';
  end;
  s1:= 'ISK = ' + s1;
  logprint(s1);

  // PIN value to be calculated by appropriate algorithm
  s2:= 'PIN = ' + 'FF FF FF FF FF FF';

  // ShowMessage multiligne workaround
  // '\n' string is used as a separator and will be replaced by CR LF at display
  ShowMessage(s0+'\n'+s1+'\n'+s2);

  LogPrint ('End read memory');
  SPIExitProgMode ();
end


{$ WRITE_MEMORY}
begin
  if _IC_Name = '' then
  begin
    ShowMessage ('Choose a component in list');
  exit;
  end;
  
  buff:= CreateByteArray(4);
  if not SPIEnterProgMode(_SPI_SPEED_MAX) then LogPrint('Error setting SPI speed');
  LogPrint ('Start write memory');
  //ChipSize := 512; // size of memory in byte (manual)
  ChipSize := _IC_Size; // to recover the size of the chip declared in the application.
  //PageSize := 16;  // size of page (see 95040 datasheet)
  PageSize := _IC_Page; // to recover the page size of the chip declared in the application.
  Addr:= 0;
  sreg :=0;
  
  // show progress bar for the fun
  ProgressBar(0, (ChipSize / PageSize)-1, 0);
  
  // write (more complex than read)
  // writing must be done by chunks equal to page size
  // and addressing is depending of the chip particularities (see datasheet)

  // beginning of writing loop by chunks equal to pagesize value)
  while Addr < ChipSize do
  begin
    // adapted from spi95.pas in asprogrammer source repository
    // https://github.com/nofeletru/UsbAsp-flash/tree/test
    // M35080, ST95P08 are excluded

    if ChipSize < 512 then
    begin
      SetArrayItem(buff, 0, $02);
      SetArrayItem(buff, 1, Addr);
      len := 2;
    end;

    // For 95040, when A8=1, Bit3 of intruction byte must be set
    if ChipSize = 512 then
    begin
      if Addr < 256 then //A8=0
      begin
        SetArrayItem(buff, 0, $02);  // $02 write EEprom opcode
        SetArrayItem(buff, 1, Addr); 
      end;

      if Addr > 255 then  //A8=1
      begin
        SetArrayItem(buff, 0, ($02 or $8));  // set Bit3 of intruction byte
        SetArrayItem(buff, 1, Addr-256);
      end;
      len := 2;
    end;

    if (ChipSize > 512) and (ChipSize <= 65536) then  // 16-bit Address range
    begin                                             // 2 bytes
      SetArrayItem(buff, 0, $02);
      SetArrayItem(buff, 1, (addr shr 8));
      SetArrayItem(buff, 2, (addr));
      len := 3;
    end;

    if (ChipSize > 65536)  then  // 24-bit Address range
    begin                        // 3 bytes
      SetArrayItem(buff, 0, $02);
      SetArrayItem(buff, 1, (addr shr 16));
      SetArrayItem(buff, 2, (addr shr 8));
      SetArrayItem(buff, 3, (addr));
      len := 4;
    end;

    // write enable (see datasheet)
    SPIWrite (1, 1, $06);
    
    // send the write command to the chip
    SPIWrite(0, len, buff);
    // then, write the content of the editor at specified address 
    //SPIWriteFromEditor(cs, size, position)
    SPIWriteFromEditor(1, PageSize, Addr); 

    // wait if busy
    //(see datasheet)
    repeat
      SPIWrite(0, 1, $05);
      SPIRead(1, 1, sreg);
    until((sreg and 1) <> 1);
    
    Inc(Addr, PageSize); // next chunk
    
    // inc progress bar of 1 step
    progressBar(1); 
  end;
   
  LogPrint('End write memory');
  // write disable
  SPIWrite (1, 1, $04);
  // reset progress bar
  ProgressBar(0, 0, 0);
  // close programmer
  SPIExitProgMode ();  
end
