CC		 := gcc
C_FLAGS  := -Wall -g -pedantic -std=c99 -pthread -D_GNU_SOURCE
AS		 := as
AS_FLAGS := -g

RM		 := rm
MV 		 := mv

BIN		:= bin
SRC		:= src
INCLUDE	:= -Iinclude
LIB		:= -Llib
OBJ 	:= obj

LIBRARIES	:= -lwebsockets -lpthread -ljansson -lavcodec -lavformat -lavfilter -lavutil -L/usr/local/lib

EXECUTABLE	:= fifoserver

all: $(BIN)/$(EXECUTABLE)

clean:
	-$(RM) $(BIN)/$(EXECUTABLE)
	-$(RM) $(OBJ)/*

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): 
	$(CC) $(C_FLAGS) $(INCLUDE) $(LIB) $(SRC)/*.c -c
	$(AS) $(AS_FLAGS) $(SRC)/*.s -c -o $(OBJ)/acode.o
	$(MV) *.o $(OBJ)/
	$(CC) $(OBJ)/*.o -fPIC -g -o $(BIN)/$(EXECUTABLE) $(LIBRARIES) 
	