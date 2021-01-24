default:
	clang -o ch341eeprom ch341eeprom.c ch341funcs.c -lusb-1.0
	clang -o mktestimg mktestimg.c

clean:
	rm -f ch341eeprom mktestimg

test02: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=2
	./ch341eeprom -v -s 24c02 -w tmp_random.bin
	./ch341eeprom -v -s 24c02 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	echo "Test 2Kbit/256bytes EEPROM done"

test08: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=8
	./ch341eeprom -v -s 24c08 -w tmp_random.bin
	./ch341eeprom -v -s 24c08 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	echo "Test 8Kbit/1Kbyte EEPROM done"

test32: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=32
	./ch341eeprom -v -s 24c32 -w tmp_random.bin
	./ch341eeprom -v -s 24c32 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	echo "Test 32Kbit/4Kbyte EEPROM done"

test256: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=256
	./ch341eeprom -v -s 24c256 -w tmp_random.bin
	./ch341eeprom -v -s 24c256 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	echo "Test 256Kbit/32Kbyte EEPROM done"

test1024: default
	dd if=/dev/urandom of=tmp_random.bin bs=128 count=1024
	./ch341eeprom -v -s 24c1024 -w tmp_random.bin
	./ch341eeprom -v -s 24c1024 -r tmp_random_readed.bin
	cmp tmp_random.bin tmp_random_readed.bin
	rm -f tmp_random.bin tmp_random_readed.bin
	echo "Test 1024Kbit/128Kbyte EEPROM done"

