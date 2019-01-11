CC		 := gcc
C_FLAGS  := -Wall -Wextra -g -pedantic -std=c99 -pthread -D_GNU_SOURCE
AS		 := as
AS_FLAGS := -Wall -g

RM		 := rm
MV 		 := mv

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib
OBJ 	:= obj

LIBRARIES	:= 'websockets'

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= fifoserver.exe
else
EXECUTABLE	:= fifoserver
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	-$(RM) $(BIN)/$(EXECUTABLE)
	-$(RM) $(OBJ)/*

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): 
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $(SRC)/*.c -c
	$(AS) $(AS_FLAGS) $(SRC)/*.s -c -o $(OBJ)/acode.o
	$(MV) *.o $(OBJ)/
	$(CC) $(OBJ)/*.o -o $(BIN)/$(EXECUTABLE) -l$(LIBRARIES) -L/usr/local/lib
	