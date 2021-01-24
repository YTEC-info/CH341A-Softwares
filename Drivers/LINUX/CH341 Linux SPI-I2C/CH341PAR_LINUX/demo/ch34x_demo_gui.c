// Created on: Dec 2, 2013
/*
 * File : ch34x_demo_gui.c
 *
 **********************************************************************
 ***********	   Copyright (C) WCH 2013.12.2		***************
 ***********	     web: http://www.wch.cn		***************
 ***********	   AUTHOR: TECH33 (tech@wch.cn)		***************
 ***********	Used for USB Interface Chip (CH341)	***************
 ***********	Nanjing	QinHeng Electronics Co.,Ltd	***************
 **********************************************************************
 *
 * Running Environment: Linux
 * This file is used for CH34x in Epp/MEM/I2C/SPI
 *
 */
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ch34x_lib.h"
#include "flash.h"

#define MAX_LENGTH	255				//Input Max Length

// GtkWidget
GtkWidget *window_main;
GtkWidget *window_sub;
GtkWidget *entryEpp1;
GtkWidget *entry_data_Epp1;
GtkWidget *entryEpp2;
GtkWidget *entry_data_Epp2;
GtkWidget *entryMem;
GtkWidget *entry_data_Mem;
GtkWidget *entry_addr_I2c1;
GtkWidget *entry_len_I2c1;
GtkWidget *entry_data_I2c1;
GtkWidget *entry_addr_I2c2;
GtkWidget *entry_len_I2c2;
GtkWidget *entry_data_I2c2;
GtkWidget *entry_addr_Flash1;
GtkWidget *entry_len_Flash1;
GtkWidget *entry_data_Flash1;
GtkWidget *entry_addr_Flash2;
GtkWidget *entry_len_Flash2;
GtkWidget *entry_data_Flash2;
GtkWidget *entry_enable_Output;
GtkWidget *entry_dirdata_Output;
GtkWidget *entry_condata_Output;
GtkWidget *entry_dirdata_D5D0;
GtkWidget *entry_condata_D5D0;


extern unsigned long retual_mem_length = 0;

static int DEV_NUM = 4;
static char gtk_dev_list[100];
static char *gtk_dev_path = "/dev/ch34x_pis";
char bufferframe[5][20] = {"EPP MODE",
						   "MEM MODE",
						   "EEPROM MODE",
						   "FLASH MODE",
						   "GPIO MODE"};
char buffer[5][10] = {"EPP", "MEM", "EEPROM", "FLASH","GPIO"};

extern int dev_fd;
EEPROM_TYPE Eeprom_ID;	//EEPROM Type
unsigned long MemMode;	//Mem Pipe
// CHar -> Hex, Used for reading length
unsigned long char_to_hex(const char *str)
{
	int strlength = strlen(str);
	int i = 0;
	unsigned long dst = 0;
	for( i = 0; i < strlength; i++ )
	{
		if(*(str + i) >= '0' && *(str + i) <= '9')
		{
			if(0 != i )
				dst = (dst << 4);
			dst += *(str + i) - '0';
		}
		else if(*(str + i) >= 'a' && *(str + i) <= 'f')
		{
			if(0 != i )
				dst = (dst << 4);
			dst += *(str + i) - 'a' + 10;
		}
		else if(*(str + i) >= 'A' && *(str + i) <= 'F')
		{
			if(0 != i )
				dst = (dst << 4);
			dst += *(str + i) - 'A' + 10;
		}
	}
	return dst;
}
// Hex -> Char, Used for gtk_entry_set_text Display
void hex_to_char(unsigned char *dst, unsigned int src)
{
	UCHAR temp[10];
	int i = 0,j = 0;
	memset(temp, 0, sizeof(UCHAR) *10);
	while(1)
	{
		if( 0 == src )
			break;
		if((src & 0x0f) >= 0 && (src & 0x0f) <= 9)
			temp[i++] = (src & 0xf) + '0';
		else
			temp[i++] = ((src & 0xf) - 10) + 'a';
		src = src >> 4;
	}
	for(j = 0; j < i; j++)
	{
		*(dst + j) = *(temp + (i-1-j));
	}
	return;
}
//String -> Char
void string_to_char(const unsigned char *str, unsigned char *dst)
{
	int strlength = strlen(str);
	unsigned char temp[MAX_LENGTH];
	int i, j = 0;
	memset(temp, 0, sizeof(unsigned char) * MAX_LENGTH);
	for(i = 0; i < strlength; i++)
	{
		if(*(str + i) >= '0' && *(str + i) <= '9')
		{
			temp[i] = *(str + i) -'0';
		}
		else if(*(str + i) >= 'a' && *(str + i) <= 'f')
		{
			temp[i] = *(str + i) -'a' + 10;
		}
		else if(*(str + i) >= 'A' && *(str + i) <= 'F')
		{
			temp[i] = *(str + i) -'A' + 10;
		}
	}
	for(i= 0; i < strlength; i++)
	{
		dst[j++] = temp[i] * 16 + temp[++i];
	}
	return;
}
//Char -> String, Used for gtk_entry_set_text display
void char_to_string(unsigned char *str, char *dst, unsigned long mLen)
{
	int strlength = mLen;
	unsigned char temp[MAX_LENGTH * 3];
	int i, j = 0;
	memset(temp, 0,MAX_LENGTH * 3);
	memset(dst, 0,MAX_LENGTH * 3);
	for( i = 0; i < strlength; i++ )
	{
		temp[j++] = str[i] / 16;
		temp[j++] = str[i] % 16;
		temp[j++] = ' ';
	}
	strlength = j - 1;
	for( j = 0; j < strlength; j++ )
	{
		if( temp[j] == 0x20 )
		{
			dst[j] = temp[j];
			continue;
		}
		if(temp[j] >= 0 && temp[j] <= 9)
			dst[j] = temp[j] + '0';
		else
			dst[j] = temp[j] - 10 + 'a';
	}
	return;
}

static int get_ch34x_dev_list()
{
	char dev_path[20] = {'\0'};
	int i;
	memset(gtk_dev_list, '\0', sizeof(gtk_dev_list));
	/* driver enumeration */
	for( i = 0; i < DEV_NUM; i++ )
	{
		sprintf(dev_path, "%s%c", gtk_dev_path, '0'+i);
		dev_fd = open(dev_path, O_RDWR);
		if( dev_fd == -1 )
			break;
		CH34xCloseDevice();
	}
	sprintf(gtk_dev_list, "Current devices number : %d", i);
	return 0;
}

void gtk_destroy_sub_window(GtkButton *button, gpointer data)
{
	gtk_widget_destroy(window_sub);
	CH34xCloseDevice();
}

// Radio event,Used for EEPROM Type Selection
void On_Eeprom_Radio_Mode(GtkWidget *radio, gint data)
{
	switch((int)data)
	{
		case 1:
			Eeprom_ID = ID_24C01;
			break;
		case 2:
			Eeprom_ID = ID_24C02;
			break;
		case 3:
			Eeprom_ID = ID_24C04;
			break;
		case 4:
			Eeprom_ID = ID_24C08;
			break;
		case 5:
			Eeprom_ID = ID_24C16;
			break;
		case 6:
			Eeprom_ID = ID_24C32;
			break;
		case 7:
			Eeprom_ID = ID_24C64;
			break;
		case 8:
			Eeprom_ID = ID_24C128;
			break;
		case 9:
			Eeprom_ID = ID_24C256;
			break;
		case 10:
			Eeprom_ID = ID_24C512;
			break;
		case 11:
			Eeprom_ID = ID_24C1024;
			break;
		case 12:
			Eeprom_ID = ID_24C2048;
			break;
		case 13:
			Eeprom_ID = ID_24C4096;
			break;
	}
}

// Radio event,Used for MEM Pipe Selection
void On_MemAddr_Radio_Mode(GtkWidget *radio, gint data)
{
	switch((int)data)
	{
		case 1:
			MemMode = 0x00;
//			printf("PipeMode is %d\n",MemMode);
			break;
		case 2:
			MemMode = 0x01;
//			printf("PipeMode is %d\n",MemMode);
			break;
	}
}


//Epp Mode: Date Write
// gtk_entry_get_text	read input data from entry
//gtk_entry_set_text	write data to entry
void CH34x_EppWrite(GtkWidget *Button, gpointer Data)
{
	unsigned long iLength = 0;
	unsigned char oBuffer[MAX_LENGTH];
	unsigned char temp[10];
	unsigned long retual_length;
	const char *len;
	int retval;
	memset(temp, 0, sizeof(temp));
	retval = CH34xSetParaMode( 0 );
	if( retval == false )
		return;
	len = gtk_entry_get_text(GTK_ENTRY(entryEpp1));
	iLength = char_to_hex(len);
	len = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)Data));
	retual_mem_length = strlen((unsigned char *)len) % 2 == 0 ? strlen((unsigned char *)len) / 2 : (strlen((unsigned char *)len) / 2 + 1);
	hex_to_char(temp, retual_mem_length);
	gtk_entry_set_text(GTK_ENTRY(entryEpp1), temp);
	string_to_char(len, oBuffer);
	retval = CH34xEppWriteData( oBuffer, iLength );
	if( retval == false )
	{
		printf("------->Write Error\n");
	}
}


//Epp Mode: Data Read
void CH34x_EppRead(GtkWidget *Button, gpointer Data)
{
	unsigned long iLength = 0;
	unsigned char iBuffer[MAX_LENGTH] = {'\0'};
	const char *len;
	const char *ReadBuffer;
	int retval;
	memset( iBuffer, 0, sizeof(iBuffer));
	ReadBuffer = (char*)malloc(sizeof(unsigned char) * DEFAULT_BUFFER_LEN);
	len = gtk_entry_get_text(GTK_ENTRY(entryEpp1));
	iLength = char_to_hex(len);
	retval = CH34xEppReadData( iBuffer, iLength );
	if( retval != iLength )
	{
		printf("------>Read Error\n");
	}
	char_to_string(iBuffer, ReadBuffer, iLength);
	gtk_entry_set_text(GTK_ENTRY((GtkWidget *)Data), ReadBuffer);
	free( ReadBuffer );
}

//Epp Mode : Addr Write
void CH34x_EppWrite_Addr(GtkWidget *Button, gpointer Data)
{
	unsigned long iLength = 0;
	unsigned char oBuffer[MAX_LENGTH];
	unsigned char temp[10];
	unsigned long retual_length;
	const char *len;
	int retval;
	memset(temp, 0, sizeof(temp));
	memset(oBuffer, 0, sizeof(oBuffer));
	retval = CH34xSetParaMode( 0 );
	if( retval == false )
		return;
	len = gtk_entry_get_text(GTK_ENTRY(entryEpp2));
	iLength = char_to_hex(len);
	len = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)Data));
	retual_mem_length = strlen((unsigned char *)len) % 2 == 0 ? strlen((unsigned char *)len) / 2 : (strlen((unsigned char *)len) / 2 + 1);
	hex_to_char(temp, retual_mem_length);
	gtk_entry_set_text(GTK_ENTRY(entryEpp2), temp);
	string_to_char(len, oBuffer);
	retval = CH34xEppWriteAddr( oBuffer, iLength );
	if( retval == false )
	{
		printf("------->Write Error\n");
	}
}

//Epp Mode : Addr Read
void CH34x_EppRead_Addr(GtkWidget *Button, gpointer Data)
{
	unsigned long iLength = 0;
	unsigned char iBuffer[MAX_LENGTH] = {'\0'};
	const char *len;
	const char *ReadBuffer;
	int retval;
	ReadBuffer = (char*)malloc(sizeof(unsigned char) * DEFAULT_BUFFER_LEN);
	len = gtk_entry_get_text(GTK_ENTRY(entryEpp2));
	iLength = char_to_hex(len);
	retval = CH34xEppReadAddr( iBuffer, iLength );
	if( retval != iLength )
	{
		printf("------>Read Error\n");
	}
	char_to_string(iBuffer, ReadBuffer, iLength);
	gtk_entry_set_text(GTK_ENTRY((GtkWidget *)Data), ReadBuffer);
	free( ReadBuffer );
}

//MEM Mode: Write data
void CH34x_MEMWrite(GtkWidget *Button, gpointer Data)
{
	unsigned long iLength = 0;
	unsigned char oBuffer[MAX_LENGTH];
	const char *len;
	unsigned char temp[10];
	int retval;

	memset(oBuffer, 0,sizeof(oBuffer));
	memset(temp, 0, sizeof(temp));
	retval = CH34xInitMEM();
	if( retval == false )
	{
		printf("CH34xInitMEM Error\n");
	}
	len = gtk_entry_get_text(GTK_ENTRY(entryMem));
	iLength = char_to_hex(len);
	len = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)Data));
	retual_mem_length = strlen((unsigned char *)len) % 2 == 0 ? strlen((unsigned char *)len) / 2 : (strlen((unsigned char *)len) / 2 + 1);
	string_to_char(len, oBuffer);
	hex_to_char(temp, retual_mem_length);
	gtk_entry_set_text(GTK_ENTRY(entryMem), temp);
	retval = CH34xMEMWriteData( oBuffer, iLength, MemMode );
	if( retval == false )
	{
		printf("------->Write Error\n");
	}
}

//MEM Mode: Read Data
void CH34x_MEMRead(GtkWidget *Button, gpointer Data)
{
	unsigned long iLength = 0;
	unsigned char iBuffer[MAX_LENGTH] = {'\0'};
	const char *len;
	const char *ReadBuffer;
	int retval;
	ReadBuffer = (char*)malloc(sizeof(unsigned char) * DEFAULT_BUFFER_LEN);
	len = gtk_entry_get_text(GTK_ENTRY(entryMem));
	iLength = char_to_hex(len);
	retval = CH34xMEMReadData( iBuffer, iLength, MemMode );
	if( retval != iLength )
	{
		printf("------>Read Error\n");
	}
	char_to_string(iBuffer, ReadBuffer, iLength);
	gtk_entry_set_text(GTK_ENTRY((GtkWidget *)Data), ReadBuffer);
	free( ReadBuffer );
}

//EPP Mode Page
void notebook_add_page1(GtkWidget *notebook, GtkWidget *frame)
{
	GtkWidget *label;
	GtkWidget *frame1;
	GtkWidget *frame2;
	GtkWidget *vbox;
	GtkWidget *table;
	GtkWidget *RdBtnD;
	GtkWidget *WrBtnD;
	GtkWidget *RdBtnA;
	GtkWidget *WrBtnA;
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	
	frame1 = gtk_frame_new("EPP DATA READ/WRITE");
	gtk_box_pack_start(GTK_BOX(vbox), frame1, FALSE, FALSE, 5);
	table = gtk_table_new(6, 6, TRUE);
	gtk_container_add(GTK_CONTAINER(frame1), table);
	label = gtk_label_new("EPP Mode Read: WR#=1, DS#=0, AS#=1, D0~D7=input\n" \
			"EPP Mode Write: WR#=0, DS#=0, AS#=1, D0~D7=output");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 4, 0, 2);
	
	label = gtk_label_new("Length :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
	entryEpp1 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entryEpp1, 1, 2, 2, 3);
	
	label = gtk_label_new("(0 ~ 0xff)");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 2, 3, 2, 3);
	
	label = gtk_label_new("Data :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 3, 4);
	entry_data_Epp1 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_data_Epp1, 1, 6, 3, 4);
	
	RdBtnD = gtk_button_new_with_label(" Read ");
	gtk_table_attach_defaults(GTK_TABLE(table), RdBtnD, 3, 4, 6, 7);
	WrBtnD = gtk_button_new_with_label(" Write ");
	gtk_table_attach_defaults(GTK_TABLE(table), WrBtnD, 4, 5, 6, 7);
	
	g_signal_connect(G_OBJECT(RdBtnD), "clicked",
				G_CALLBACK(CH34x_EppRead), (gpointer)entry_data_Epp1);
	g_signal_connect(G_OBJECT(WrBtnD), "clicked",
				G_CALLBACK(CH34x_EppWrite), (gpointer)entry_data_Epp1);


	frame2 = gtk_frame_new("EPP Addr READ/WRITE");
	gtk_box_pack_start(GTK_BOX(vbox), frame2, FALSE, FALSE, 5);
	table = gtk_table_new(6, 6, TRUE);
	gtk_container_add(GTK_CONTAINER(frame2), table);
	label = gtk_label_new("EPP Mode Read: WR#=1, DS#=0, AS#=0, D0~D7=input\n" \
			"EPP Mode Write: WR#=0, DS#=0, AS#=0, D0~D7=output");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 4, 0, 2);
	label = gtk_label_new("Length :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
	entryEpp2 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entryEpp2, 1, 2, 2, 3);
	label = gtk_label_new("(0 ~ 0xff)");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 2, 3, 2, 3);
	label = gtk_label_new("Data :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 3, 4);
	entry_data_Epp2 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_data_Epp2, 1, 6, 3, 4);
	RdBtnA = gtk_button_new_with_label(" Read ");
	gtk_table_attach_defaults(GTK_TABLE(table), RdBtnA, 3, 4, 6, 7);
	WrBtnA = gtk_button_new_with_label(" Write ");
	gtk_table_attach_defaults(GTK_TABLE(table), WrBtnA, 4, 5, 6, 7);
	g_signal_connect(G_OBJECT(RdBtnA), "clicked",
				G_CALLBACK(CH34x_EppRead_Addr), (gpointer)entry_data_Epp2);
	g_signal_connect(G_OBJECT(WrBtnA), "clicked",
				G_CALLBACK(CH34x_EppWrite_Addr), (gpointer)entry_data_Epp2);

	label = gtk_label_new(buffer[0]);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, label);
}

//MEM Mode Page
void notebook_add_page2(GtkWidget *notebook, GtkWidget *frame)
{
	GtkWidget *label;
	GtkWidget *frame1;
	GtkWidget *frame2;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *table;
	GtkWidget *RdBtn;
	GtkWidget *WrBtn;
	GtkWidget *radioBtn1;
	GtkWidget *radioBtn2;

	MemMode = 0x00;
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	frame1 = gtk_frame_new("MEM ADDR MODE CONFIGURE");
	gtk_box_pack_start(GTK_BOX(vbox), frame1, FALSE, FALSE, 20);
	hbox = gtk_hbox_new(FALSE, 20);
	gtk_container_add(GTK_CONTAINER(frame1), hbox);
	radioBtn1 = gtk_radio_button_new_with_label(NULL, "Read/Write MEM Addr0 Mode");
	gtk_box_pack_start(GTK_BOX(hbox), radioBtn1, FALSE, FALSE, 20);
	radioBtn2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn1), "Read/Write MEM Addr1 Mode");
	gtk_box_pack_start(GTK_BOX(hbox), radioBtn2, FALSE, FALSE, 20);
	g_signal_connect(G_OBJECT(radioBtn1), "released",
				G_CALLBACK(On_MemAddr_Radio_Mode), (gpointer)1);
	g_signal_connect(G_OBJECT(radioBtn2), "released",
				G_CALLBACK(On_MemAddr_Radio_Mode), (gpointer)2);
	frame2 = gtk_frame_new("MEM DATA READ/WRITE");
	gtk_box_pack_start(GTK_BOX(vbox), frame2, FALSE, FALSE, 20);
	table = gtk_table_new(6, 6, TRUE);
	gtk_container_add(GTK_CONTAINER(frame2), table);
	label = gtk_label_new("MEM Mode Read: WR#=1, DS#/RD#=0, AS#=0, D0~D7=input\n" \
			"MEM Mode Write: WR#=0, DS#/RD#=1, AS#=0, D0~D7=output");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 5, 0, 2);
	label = gtk_label_new("Length :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
	entryMem = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entryMem, 1, 3, 2, 3);
	label = gtk_label_new("(0 ~ 0xff)");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 3, 4, 2, 3);
	label = gtk_label_new("Data :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 3, 4);
	entry_data_Mem = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_data_Mem, 1, 6, 3, 4);
	RdBtn = gtk_button_new_with_label(" Read ");
	gtk_table_attach_defaults(GTK_TABLE(table), RdBtn, 3, 4, 6, 7);
	WrBtn = gtk_button_new_with_label(" Write ");
	gtk_table_attach_defaults(GTK_TABLE(table), WrBtn, 4, 5, 6, 7);
	g_signal_connect(G_OBJECT(RdBtn), "clicked",
					G_CALLBACK(CH34x_MEMRead), (gpointer)entry_data_Mem);
	g_signal_connect(G_OBJECT(WrBtn), "clicked",
			G_CALLBACK(CH34x_MEMWrite), (gpointer)entry_data_Mem);

	label = gtk_label_new(buffer[1]);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, label);
}

//EEPROM Write
void CH34x_EEPROM_Write(GtkWidget *Button, gpointer Data)
{
	unsigned long iLength = 0;
	unsigned long iAddr;
	unsigned char oBuffer[MAX_LENGTH];
	const char *len;
	unsigned int retual_length;
	unsigned char *Addr_tmp;
	unsigned char temp[10];
	int retval;

	memset(oBuffer, 0, sizeof(oBuffer));
	memset(temp, 0, sizeof(temp));
	Addr_tmp = gtk_entry_get_text(GTK_ENTRY(entry_addr_I2c1));
	iAddr = char_to_hex(Addr_tmp);
	len = gtk_entry_get_text(GTK_ENTRY(entry_len_I2c1));
	iLength = char_to_hex(len);
	len = NULL;
	len = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)Data));
	retual_length = strlen((unsigned char *)len) % 2 == 0 ? strlen((unsigned char *)len) / 2 : (strlen((unsigned char *)len) / 2 + 1);;
	hex_to_char(temp, retual_length);
	gtk_entry_set_text(GTK_ENTRY(entry_len_I2c1), temp);
	string_to_char(len, oBuffer);
	retval = CH34xWriteEEPROM( Eeprom_ID, iAddr, retual_length, oBuffer );
	if( retval == false )
	{
		printf("------->Write Error\n");
	}
}

//EEPROM Mode -> Read
void CH34x_EEPROM_Read(GtkWidget *Button, gpointer Data)
{
	unsigned long iLength = 0;
	unsigned long iAddr;
	unsigned char iBuffer[MAX_LENGTH] = {'\0'};
	const char *len;
	unsigned char *Addr_tmp;
	const char *ReadBuffer;
	int retval;
	int i;
	ReadBuffer = (char*)malloc(sizeof(unsigned char) * DEFAULT_BUFFER_LEN);
	Addr_tmp = gtk_entry_get_text(GTK_ENTRY(entry_addr_I2c2));
	iAddr = char_to_hex(Addr_tmp);
	len = gtk_entry_get_text(GTK_ENTRY(entry_len_I2c2));
	iLength = char_to_hex(len);
	retval = CH34xReadEEPROM( Eeprom_ID, iAddr, iLength, iBuffer );
	if( retval == false )
	{
		printf("Read EEPROM Error\n");
	}
	char_to_string( iBuffer, ReadBuffer, iLength );
	gtk_entry_set_text(GTK_ENTRY((GtkWidget *)Data), ReadBuffer);
	free( ReadBuffer );
}

//EEPROM Mode Page
void notebook_add_page3(GtkWidget *notebook, GtkWidget *frame)
{
	GtkWidget *label;
	GtkWidget *hbox;
	GtkWidget *vbox1;
	GtkWidget *vbox2;
	GtkWidget *WrBtn;
	GtkWidget *RdBtn;
	GtkWidget *frame1;
	GtkWidget *frame2;
	GtkWidget *frame3;
	GtkWidget *table;
	GtkWidget *radioBtn1;
	GtkWidget *radioBtn2;
	GtkWidget *radioBtn3;
	GtkWidget *radioBtn4;
	GtkWidget *radioBtn5;
	GtkWidget *radioBtn6;
	GtkWidget *radioBtn7;
	GtkWidget *radioBtn8;
	GtkWidget *radioBtn9;
	GtkWidget *radioBtn10;
	GtkWidget *radioBtn11;
	GtkWidget *radioBtn12;
	GtkWidget *radioBtn13;

//	GtkWidget *scroll_window;
//	GtkWidget *buffer1;

	Eeprom_ID = ID_24C08;
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), hbox);
	frame1 = gtk_frame_new("EEPROM Type");
	gtk_box_pack_start(GTK_BOX(hbox), frame1, FALSE, FALSE, 10);
	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame1), vbox1);
	radioBtn1 = gtk_radio_button_new_with_label(NULL, "24C01");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn1, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn1),"released",
				G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)1);
	radioBtn2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn1), "24C02");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn2, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn2),"released",
					G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)2);
	radioBtn3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn2), "24C04");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn3, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn3),"released",
					G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)3);
	radioBtn4 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn3), "24C08");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn4, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn4),"released",
					G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)4);
	radioBtn5 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn4), "24C16");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn5, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn5),"released",
					G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)5);
	radioBtn6 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn5), "24C32");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn6, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn6),"released",
					G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)6);
	radioBtn7 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn6), "24C64");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn7, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn7),"released",
					G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)7);
	radioBtn8 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn7), "24C128");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn8, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn8),"released",
						G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)8);
	radioBtn9 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn8), "24C256");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn9, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn9),"released",
						G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)9);
	radioBtn10 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn9), "24C512");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn10, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn10),"released",
						G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)10);
	radioBtn11 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn10), "24C1024");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn11, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn11),"released",
						G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)11);
	radioBtn12 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn11), "24C2048");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn12, FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(radioBtn12),"released",
						G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)12);
	radioBtn13 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioBtn12), "24C4096");
	gtk_box_pack_start(GTK_BOX(vbox1), radioBtn13, FALSE, FALSE, 5);

	gtk_toggle_button_set_active(radioBtn4, TRUE);
	g_signal_connect(G_OBJECT(radioBtn13),"released",
						G_CALLBACK(On_Eeprom_Radio_Mode), (gpointer)13);
	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox2, FALSE, FALSE, 5);
	frame2 = gtk_frame_new("Write Data");
	gtk_container_add(GTK_CONTAINER(vbox2), frame2);
	table = gtk_table_new(8, 6, TRUE);
	gtk_container_add(GTK_CONTAINER(frame2), table);
	label = gtk_label_new("Start Addr Of Data Unit:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 3, 0, 1);
	entry_addr_I2c1 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_addr_I2c1, 0, 2, 1, 2);
	label = gtk_label_new("Write Length:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 2, 3);
	entry_len_I2c1 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_len_I2c1, 0, 2, 3, 4);
	WrBtn = gtk_button_new_with_label(" Write ");
	gtk_table_attach_defaults(GTK_TABLE(table), WrBtn, 3, 4, 3, 4);
	label = gtk_label_new("Data Of Write:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 4, 5);
	entry_data_I2c1 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_data_I2c1, 0, 5, 5, 6);
	g_signal_connect(G_OBJECT(WrBtn), "clicked",
				G_CALLBACK(CH34x_EEPROM_Write), (gpointer)entry_data_I2c1);

	frame3 = gtk_frame_new("Read Data");
	gtk_container_add(GTK_CONTAINER(vbox2), frame3);
	table = gtk_table_new(8, 6, TRUE);
	gtk_container_add(GTK_CONTAINER(frame3), table);
	label = gtk_label_new("Start Addr Of Data Unit:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 3, 0, 1);
	entry_addr_I2c2 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_addr_I2c2, 0, 2, 1, 2);
	label = gtk_label_new("Read Length:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 2, 3);
	entry_len_I2c2 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_len_I2c2, 0, 2, 3, 4);
	RdBtn = gtk_button_new_with_label(" Read ");
	gtk_table_attach_defaults(GTK_TABLE(table), RdBtn, 3, 4, 3, 4);
	label = gtk_label_new("Data Of Read:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 4, 5);
	entry_data_I2c2 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_data_I2c2, 0, 5, 5, 6);
	g_signal_connect(G_OBJECT(RdBtn), "clicked",
				G_CALLBACK(CH34x_EEPROM_Read), (gpointer)entry_data_I2c2);

//	entry_data = gtk_text_view_new();
//	buffer1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry_data));
//	scroll_window = gtk_scrolled_window_new(NULL, NULL);
//	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window),
//									GTK_POLICY_NEVER,
//									GTK_POLICY_AUTOMATIC);
//	gtk_container_add(GTK_CONTAINER(scroll_window), entry_data);
//	gtk_table_attach_defaults(GTK_TABLE(table), scroll_window, 0, 6, 5, 8);
//
//	insert_text(buffer1);


	label = gtk_label_new(buffer[2]);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, label);
}

//Falsh ReadByte
BOOL CH34x_Flash_ReadByte( PUCHAR oBuffer, ULONG iAddr, ULONG iLength )
{
	ULONG mLen, iChipselect;
	mLen = iLength;
	iChipselect = 0x80;
	oBuffer[0] = ReadByte;
	oBuffer[1] = (UCHAR)( iAddr >> 16 & 0xff );
	oBuffer[2] = (UCHAR)( iAddr >> 8 & 0xff );
	oBuffer[3] = (UCHAR)( iAddr & 0xff );
	if( CH34xStreamSPI4( iChipselect, mLen, oBuffer ) == false )
		return false;
	return true;
}

//Write Enable
BOOL CH34xWriteEnable()
{
	ULONG mLen, iChipselect;
	UCHAR mWrBuf[2];
	mWrBuf[0] = WREN;
	mLen = 2;
	iChipselect = 0x80;
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;
	else
		return true;
}

//Write wait
BOOL CH34xFlash_Wait()
{
	ULONG mLen, iChipselect;
	ULONG i = 0;
	UCHAR mWrBuf[3];
	UCHAR status;
	mLen = 3;
	iChipselect = 0x80;
	mWrBuf[0] = RDSR;
	do{
		mWrBuf[0] = RDSR;
		if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
			return false;
		status = mWrBuf[1];
	}while( status & 1 );
	return true;
}

//Sector Erase: Every 4K
BOOL CH34xSectorErase( ULONG StartAddr )
{
	ULONG mLen, iChipselect;
	UCHAR mWrBuf[4];
	if( CH34xWriteEnable() == false )
		return false;
	mWrBuf[0] = Sector_Erase;
	mWrBuf[1] = (UCHAR)( StartAddr >> 16 & 0xff );
	mWrBuf[2] = (UCHAR)( StartAddr >> 8 & 0xf0 );
	mWrBuf[3] = 0x00;
	mLen = 4;
	iChipselect = 0x80;
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;

	if( CH34xFlash_Wait() == false )
		return false;
	return true;
}

//Read Block Data
BOOL CH34x_Flash_ReadBlock( PUCHAR oBuffer, ULONG iAddr, ULONG iLength )
{
	ULONG mLen, iChipselect;
	UCHAR mWrBuf[1028];
	ULONG i;
	mLen = 1024;	//Read length Once
	iChipselect = 0x80;
	iAddr = iAddr - iAddr % MAX_BUFFER_LENGTH;
	for( i=0; i < iLength;)
	{
		memset(mWrBuf, 0, sizeof(mWrBuf));
		mWrBuf[0] = ReadByte;
		mWrBuf[1] = (UCHAR)( iAddr >> 16 & 0xff );
		mWrBuf[2] = (UCHAR)( iAddr >> 8 & 0xff );
		mWrBuf[3] = (UCHAR)( iAddr & 0xff );
		if( CH34xStreamSPI4( iChipselect, mLen+4, mWrBuf ) == false )
			return false;
		memcpy(oBuffer + i, &mWrBuf[4], 1024 * sizeof(unsigned char));
		i = i + 1024;
		iAddr += 1024;
	}

	return true;
}

//Flash Read Status
BOOL CH34xFlashReadStatus()
{
	ULONG mLen, iChipselect;
	UCHAR status;
	UCHAR mWrBuf[3];
	iChipselect = 0x80;
	mLen = 3;
	mWrBuf[0] = RDSR;
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;
	status = mWrBuf[1];
	if( status & 0x0c )
	{
		mWrBuf[0] = EWSR;
		mLen = 1;
		if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
			return false;
		mLen = 2;
		memset( mWrBuf, 0, sizeof( UCHAR ) * mLen );
		mWrBuf[0] = WRSR;
		mWrBuf[1] = status & (~0x0c);
		if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
			return false;
	}
	mWrBuf[0] = RDSR;
	if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
		return false;
	status = mWrBuf[1];
	return true;
}

// One Byte Write
BOOL CH34x_Flash_Write( PVOID iBuffer, ULONG iAddr, ULONG iLength )
{
	ULONG i;
	ULONG iChipselect = 0x80;
	UCHAR mWrBuf[5];
	ULONG mLen = 5;
	for( i = 0; i < iLength; i++ )
	{
		if( !CH34xWriteEnable() )
			return false;
		mWrBuf[0] = Byte_Program;
		mWrBuf[1] = (UCHAR)(iAddr >> 16 & 0xff);
		mWrBuf[2] = (UCHAR)(iAddr >> 8 & 0xff);
		mWrBuf[3] = (UCHAR)(iAddr & 0xff);
		iAddr++;
		mWrBuf[4] = *((PUCHAR)iBuffer + i);
		if( CH34xStreamSPI4( iChipselect, mLen, mWrBuf ) == false )
			return false;
		memset( mWrBuf, 0, sizeof( UCHAR ) * mLen );
		if( !CH34xFlash_Wait() )
			return false;
	}
	return true;
}

//FLASH Mode: Write Block Data
void CH34xWriteSPI(GtkWidget *Button, gpointer Data)
{
	GtkWidget *label;
	int retval;
	unsigned char iBuffer[MAX_BUFFER_LENGTH];
	unsigned char oBuffer[MAX_LENGTH];
	unsigned long iLength = 0;
	unsigned int retual_length;
	unsigned long iAddr = 0;
	unsigned long Cur_state = 0;
	const unsigned char *len;
	unsigned char *Addr_tmp;
	unsigned char temp[10];
	int i;

	memset(iBuffer, 0, sizeof(iBuffer));
	memset(oBuffer, 0, sizeof(iBuffer));
	memset(temp, 0, sizeof(temp));
	Addr_tmp = gtk_entry_get_text(GTK_ENTRY(entry_addr_Flash1));
	iAddr = char_to_hex(Addr_tmp);
	len = gtk_entry_get_text(GTK_ENTRY(entry_len_Flash1));
	iLength = char_to_hex(len);
	retval = CH34xFlashReadStatus();
	if( retval == false )
	{
		printf("error in flash status\n");
		goto error;
	}
	retval = CH34x_Flash_ReadBlock( iBuffer, iAddr, MAX_BUFFER_LENGTH );
	if( retval == false )
	{
		printf("error in flash ReadBlock\n");
		goto error;
	}
	retval = CH34xSectorErase( iAddr );
	if( retval == false )
	{
		printf("error in flash Sector Erase\n");
		goto error;
	}
	len = NULL;
	len = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)Data));
	retual_length = strlen((unsigned char *)len) % 2 == 0 ? strlen((unsigned char *)len) / 2 : (strlen((unsigned char *)len) / 2 + 1);
	hex_to_char(temp, retual_length);
	gtk_entry_set_text(GTK_ENTRY(entry_len_Flash1), temp);
	Cur_state = iAddr % MAX_BUFFER_LENGTH;
	string_to_char(len, oBuffer);
	for( i = 0; i < retual_length; i++ )
		iBuffer[Cur_state + i] = oBuffer[i];
	retval = CH34x_Flash_Write( iBuffer, (iAddr - iAddr % MAX_BUFFER_LENGTH), MAX_BUFFER_LENGTH );
	if( retval == false )
	{
		printf("error in flash Write\n");
		goto error;
	}

	g_message("Success\n");
	return;

error:
	g_message("failed\n");
	return;
}

//FLASH MODE: Read Block Data
void CH34xReadSPI(GtkWidget *Button, gpointer Data)
{
	unsigned long iLength = 0;
	unsigned long iAddr;
	unsigned char iBuffer[MAX_LENGTH] = {'\0'};
	const unsigned char *len;
	unsigned char *Addr_tmp;
	const char *ReadBuffer;
	int retval;
	int i;
	ReadBuffer = (char*)malloc(sizeof(unsigned char) * DEFAULT_BUFFER_LEN);
	Addr_tmp = gtk_entry_get_text(GTK_ENTRY(entry_addr_Flash2));
	iAddr = char_to_hex(Addr_tmp);
	len = gtk_entry_get_text(GTK_ENTRY(entry_len_Flash2));
	iLength = char_to_hex(len);
	retval = CH34x_Flash_ReadByte( iBuffer, iAddr, iLength + 4 );
	if( retval == false )
	{
		printf("Read Flash Error\n");
	}
	char_to_string(&iBuffer[4], ReadBuffer, iLength);
	gtk_entry_set_text(GTK_ENTRY((GtkWidget *)Data), ReadBuffer);
	free( ReadBuffer );
}


void CH34x_SetOutput(GtkWidget *Button, gpointer Data)
{
	unsigned long iEnable;
	unsigned long iDirData;
	unsigned long iConData;
	int retval;
	const char *len;
	len = gtk_entry_get_text(GTK_ENTRY(entry_enable_Output));
	iEnable = char_to_hex(len);
	len = gtk_entry_get_text(GTK_ENTRY(entry_dirdata_Output));
	iDirData = char_to_hex(len);
	len = gtk_entry_get_text(GTK_ENTRY(entry_condata_Output));
   	iConData = char_to_hex(len);
	retval = CH34xSetOutput(iEnable, iDirData, iConData);
	if( retval == false )
	{
		printf("------>Set Output Error!\n");
	}
	else printf("------>Set Output Success!\n");
}


void CH34x_SetD5D0(GtkWidget *Button, gpointer Data)
{
	unsigned char iDirData;
	unsigned char iConData;
	int retval;
	const char *len;
	len = gtk_entry_get_text(GTK_ENTRY(entry_dirdata_D5D0));
	iDirData = char_to_hex(len);
	len = gtk_entry_get_text(GTK_ENTRY(entry_condata_D5D0));
	iConData = char_to_hex(len);
	printf("iDirData:%.2x   ", iDirData);
	printf("iConData:%.2x   ", iConData);
   	retval = CH34xSet_D5_D0(iDirData, iConData);
	if( retval == false )
	{
		printf("------>Set D5D0 Error!\n");
	}
	else printf("------>Set D5D0 Success!\n");
}


//FLASH MODE PAGE
void notebook_add_page4(GtkWidget *notebook, GtkWidget *frame)
{
	GtkWidget *label;
	GtkWidget *vbox;
	GtkWidget *frame1;
	GtkWidget *frame2;
	GtkWidget *table;
	GtkWidget *WrBtn;
	GtkWidget *RdBtn;


	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	frame1 = gtk_frame_new("Flash Write Data");
	gtk_container_add(GTK_CONTAINER(vbox), frame1);
	table = gtk_table_new(8, 8, TRUE);
	gtk_container_add(GTK_CONTAINER(frame1), table);
	label = gtk_label_new("Start Addr Of Data Unit:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 3, 0, 1);
	entry_addr_Flash1 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_addr_Flash1, 0, 2, 1, 2);
	label = gtk_label_new("Write Length:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 2, 3);
	entry_len_Flash1 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_len_Flash1, 0, 2, 3, 4);
	WrBtn = gtk_button_new_with_label(" Write ");
	gtk_table_attach_defaults(GTK_TABLE(table), WrBtn, 3, 4, 3, 4);
	label = gtk_label_new("Data Of Write:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 4, 5);
	entry_data_Flash1 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_data_Flash1, 0, 6, 5, 6);
	g_signal_connect(G_OBJECT(WrBtn), "clicked",
				G_CALLBACK(CH34xWriteSPI), (gpointer)entry_data_Flash1);

	frame2 = gtk_frame_new("Flash Read Data");
	gtk_container_add(GTK_CONTAINER(vbox), frame2);
	table = gtk_table_new(8, 8, TRUE);
	gtk_container_add(GTK_CONTAINER(frame2), table);
	label = gtk_label_new("Start Addr Of Data Unit:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 3, 0, 1);
	entry_addr_Flash2 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_addr_Flash2, 0, 2, 1, 2);
	label = gtk_label_new("Read Length:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 2, 3);
	entry_len_Flash2 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_len_Flash2, 0, 2, 3, 4);
	RdBtn = gtk_button_new_with_label(" Read ");
	gtk_table_attach_defaults(GTK_TABLE(table), RdBtn, 3, 4, 3, 4);
	label = gtk_label_new("Data Of Read:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 2, 4, 5);
	entry_data_Flash2 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_data_Flash2, 0, 6, 5, 6);
	g_signal_connect(G_OBJECT(RdBtn), "clicked",
				G_CALLBACK(CH34xReadSPI), (gpointer)entry_data_Flash2);

	label = gtk_label_new(buffer[3]);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, label);
}


void notebook_add_page5(GtkWidget *notebook, GtkWidget *frame)
{
	GtkWidget *label;
	GtkWidget *frame1;
	GtkWidget *frame2;
	GtkWidget *vbox;
	GtkWidget *table;
	GtkWidget *StBtnO;
	GtkWidget *StBtnD;
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	frame1 = gtk_frame_new("SET OUTPUT");
	gtk_box_pack_start(GTK_BOX(vbox), frame1, FALSE, FALSE, 5);
	table = gtk_table_new(6, 6, TRUE);
	gtk_container_add(GTK_CONTAINER(frame1), table);
	label = gtk_label_new(" Bit7~Bit0<==>D7-D0,  Bit8<==>ERR#,  Bit9<==>PEMP, Bit10<==>INT#,  Bit11<==>SLCT,  Bit13<==>WAIT#,  Bit14<==>DATAS#/READ#\n"\    
" Bit15<==>ADDRS#/ADDR/ALE,  Bit16<==>RESET#,  Bit17<==>WRITE#,  Bit18<==>SCL,  Bit29<==>SDA");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 6, 0, 2);
	
	label = gtk_label_new("Enable :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
	entry_enable_Output = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_enable_Output, 1, 2, 2, 3);
	
	label = gtk_label_new("DirData :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 3, 4);
	entry_dirdata_Output = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_dirdata_Output, 1, 2, 3, 4);

	label = gtk_label_new("ConData :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 4, 5);
	entry_condata_Output = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_condata_Output, 1, 2, 4, 5);

	StBtnO = gtk_button_new_with_label(" Set ");
	gtk_table_attach_defaults(GTK_TABLE(table), StBtnO, 4, 5, 8, 9);
	g_signal_connect(G_OBJECT(StBtnO), "clicked",
				G_CALLBACK(CH34x_SetOutput), (gpointer)entry_enable_Output);

	frame2 = gtk_frame_new("Set D5D0");
	gtk_box_pack_start(GTK_BOX(vbox), frame2, FALSE, FALSE, 5);
	table = gtk_table_new(6, 6, TRUE);
	gtk_container_add(GTK_CONTAINER(frame2), table);
	label = gtk_label_new("Bit5~Bit0<==>D5-D0" );
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 4, 0, 1);
	
	label = gtk_label_new("DirData :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
	entry_dirdata_D5D0 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_dirdata_D5D0, 1, 2, 2, 3);
	
	label = gtk_label_new("ConData :");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 3, 4);
	entry_condata_D5D0 = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(table), entry_condata_D5D0, 1, 2, 3, 4);
	
	StBtnD = gtk_button_new_with_label(" Set ");
	gtk_table_attach_defaults(GTK_TABLE(table), StBtnD, 4, 5, 8, 9);
	g_signal_connect(G_OBJECT(StBtnD), "clicked",
				G_CALLBACK(CH34x_SetD5D0), (gpointer)entry_dirdata_D5D0);

	label = gtk_label_new(buffer[4]);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, label);
}


//Second Window12120x12fff
GtkWidget *create_sub_window(GtkWidget *entry)
{
	GtkWidget *sub_window;
	GtkWidget *frame;
	GtkWidget *label;
	GtkWidget *notebook;
	int retval, index;

	sub_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(sub_window), "delete_event",
					G_CALLBACK(gtk_widget_destroy), sub_window);
	gtk_window_set_title(GTK_WINDOW(sub_window), "WCH CH34x Debug");
	gtk_window_set_position(GTK_WINDOW(sub_window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(sub_window), 600, 600);
	gtk_container_set_border_width(GTK_CONTAINER(sub_window), 10);
	index = *(gtk_entry_get_text(GTK_ENTRY(entry))) - '1';
	retval = CH34xOpenDevice( index );
	if(!retval)
	{
		printf("Function Open Error\n");
		return NULL;
	}
	char *drv_version = (char *)malloc( sizeof( char ) * 30 );
	retval = CH34x_GetDrvVer( drv_version );
	if( retval == false )
	{
		err( "CH34x_GetDrvVer Error" );
		return -1;
	}
	printf( "drv_version : %s\n", drv_version );
	free( drv_version );
	PUCHAR VendorId = (char *)malloc( sizeof(unsigned char));
	retval = CH34x_GetVendorId( VendorId );
	if( retval == false )
	{
		err( "CH34x_GetVendorId Error" );
		return -1;
	}
	printf( "VendorId : 0x%02x\n", *VendorId );
	free( VendorId );
	notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook),GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(sub_window), notebook);

	frame = gtk_frame_new(bufferframe[0]);
	gtk_widget_set_size_request(frame, 600, 500);
	notebook_add_page1(notebook, frame);
	frame = gtk_frame_new(bufferframe[1]);
	gtk_widget_set_size_request(frame, 600, 500);
	notebook_add_page2(notebook, frame);
	frame = gtk_frame_new(bufferframe[2]);
	gtk_widget_set_size_request(frame, 600, 500);
	notebook_add_page3(notebook, frame);
	frame = gtk_frame_new(bufferframe[3]);
	gtk_widget_set_size_request(frame, 600, 500);
	notebook_add_page4(notebook, frame);
	frame = gtk_frame_new(bufferframe[4]);
	gtk_widget_set_size_request(frame, 600, 500);
	notebook_add_page5(notebook, frame);
	g_signal_connect(G_OBJECT(sub_window), "destroy",
					G_CALLBACK(gtk_destroy_sub_window), NULL);
	gtk_widget_show_all(sub_window);
	return sub_window;
}

void gtk_show_sub(GtkWidget *button, gpointer data)
{
	window_sub = create_sub_window((GtkWidget *)data);
}

//Main Window
GtkWidget *create_main_window()
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *label_dev_list;
	GtkWidget *label_input;
	GtkWidget *frame;
	GtkWidget *entry_main;
	GtkWidget *button_next;
	GtkWidget *button_quit;

	int retval;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
	gtk_window_set_title(GTK_WINDOW(window), "WCH CH34x Chips Debug");

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	frame = gtk_frame_new("Current CH34x Device List");

	retval = get_ch34x_dev_list();
	if(retval == 0)
		label_dev_list = gtk_label_new(gtk_dev_list);
	else
		label_dev_list = gtk_label_new("Can't find any devices");
	gtk_container_add(GTK_CONTAINER(frame), label_dev_list);
	gtk_label_set_justify(GTK_LABEL(label_dev_list), GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);

	hbox = gtk_hbox_new(FALSE, 10);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);

	label_input = gtk_label_new("Input Dev Num:");
	gtk_box_pack_start(GTK_BOX(hbox), label_input, FALSE, FALSE, 10);

	entry_main = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox), entry_main, FALSE, FALSE, 10);

	button_next = gtk_button_new_with_label("Next");
	gtk_box_pack_start(GTK_BOX(hbox), button_next, FALSE, FALSE, 10);

	button_quit = gtk_button_new_with_label("Quit");
	gtk_box_pack_start(GTK_BOX(hbox), button_quit, TRUE, TRUE, 10);

	gtk_widget_show_all(window);
	g_signal_connect(G_OBJECT(button_next), "clicked",
						G_CALLBACK(gtk_show_sub), entry_main);
	g_signal_connect(G_OBJECT(button_quit), "clicked",
						G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(window), "destroy",
						G_CALLBACK(gtk_main_quit), NULL);
	return window;
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    window_main = create_main_window();
    gtk_widget_show(window_main);
    gtk_main();
    return 0;
}


