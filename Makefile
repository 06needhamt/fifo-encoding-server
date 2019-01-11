CC		 := gcc
C_FLAGS  := -Wall -Wextra -w -g -pedantic -std=c99 -pthread -D_GNU_SOURCE
AS		 := as
AS_FLAGS := -g

RM		 := rm
MV 		 := mv

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib
OBJ 	:= obj

LIBRARIES	:= -lwebsockets -lpthread

EXECUTABLE	:= fifoserver

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
	$(CC) $(OBJ)/*.o -fPIC -g -o $(BIN)/$(EXECUTABLE) $(LIBRARIES) -L/usr/local/lib
	