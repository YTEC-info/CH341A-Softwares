# Makefile for WCH CH34x chips

CC := gcc
OBJ := DEMO
OBJ_GTK := DEMO_GUI
CFLAGS := -Wall -g
CFLAGS_GUI := -Wall -g `pkg-config --cflags --libs gtk+-2.0`
OBJ_C := ch34x_demo.o ch34x_lib.o
OBJ_C_GUI := ch34x_demo_gui.o ch34x_lib.o


$(OBJ):$(OBJ_C)
	$(CC) -o $@ $^ $(CFLAGS)
	$(CC) -c ch34x_lib.c $(CFLAGS)
	$(CC) -c ch34x_demo_gui.c $(CFLAGS_GUI)
	$(CC) -o $(OBJ_GTK) $(OBJ_C_GUI) $(CFLAGS_GUI)
%.o:%.c
	$(CC) -c $< $(CFLAGS)

#DEMO:$(OBJ_C)
#	$(CC) -o $@ $(OBJ_C) $(CFLAGS)
clean:
	rm -rf *.o $(OBJ) $(OBJ_GTK)
