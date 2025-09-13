# obj/Makefile
CC = gcc
CFLAGS = -Wall -I../include 
TARGET = ../bin/srv
OBJS = $(wildcard *.o)

$(TARGET): $(OBJS)
	@mkdir -p ../bin
	$(CC) $(CFLAGS) $^ -o $@  -lpthread -lsqlite3

clean:
	rm -f $(TARGET)

.PHONY: clean

