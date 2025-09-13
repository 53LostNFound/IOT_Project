CC=gcc
CFLAGS=-Wall -g -O0
SRC = $(shell ls *.c)
OBJ=$(patsubst %.c, %.o , $(SRC))
BIN=cli 
all: $(BIN)

$(BIN):$(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -I. -lpthread
%.o: %.c
	$(CC) -c -o $@ $< -I.
	
clean:
	$(RM) .*.sw? $(OBJ)
