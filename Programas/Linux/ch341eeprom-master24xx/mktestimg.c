/*

   mktestimg is a program to make a 'rolling' file for test ch341eeprom programmer

   Originally written by asbokid <ballymunboy@gmail.com>

   Minor improvements made by Paolo Caroni <kenren89@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http:www.gnu.org/licenses/>.

*/

#include <stdio.h>

int main(int argc, char **argv)
{

    FILE *out=fopen( "test.bin", "w" );
    /*create and open file test.bin*/

    int i, j, k;
    char chip;
    static char *hex = "\00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff";

    printf("Select chip type:\n\na for 24c01\nb for 24c02\nc for 24c04\nd for 24c08\n"\
           "e for 24c16\nf for 24c32\ng for 24c64\nh for 24c128\ni for 24c256\n"\
           "l for 24c512\nm for 24c1024\n\nInsert a letter:");
    scanf("%c",&chip);
    getchar();

    switch(chip) {	/* switch cycle*/

        case 'a':	/* 24c01*/
        k=128;
        break;

        case 'b':	/* 24c02*/
        k=256;
        break;

        case 'c':	/* 24c04*/
        k=512;
        break;

        case 'd':	/* 24c08*/
        k=1024;
        break;

        case 'e':	/* 24c16*/
        k=2048;
        break;

        case 'f':	/* 24c32*/
        k=4096;
        break;

        case 'g':	/* 24c64*/
        k=8192;
        break;

        case 'h':	/* 24c128*/
        k=16384;
        break;

        case 'i':	/* 24c256*/
        k=32768;
        break;

        case 'l':	/* 24c512*/
        k=65536;
        break;

        case 'm':	/* 24c1024*/
        k=131072;
        break;

        default:
	printf("Error, wrong letter\n");   /* in the other case*/
	break;
                 }

   for (j = 0; j < k/16; j++)
	{
        for (i = 0; i < 16; i++)
		{
            fputc(hex[j%16], out);
            /* create a binary file with 'rolling'*/
		}
	}
	
        fflush(out);
        fclose(out);
        /* clean and close file test.bin*/

        printf("saved test image as test.bin\n");

	return 0;
}

