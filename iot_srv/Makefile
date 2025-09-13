CC = gcc
CFLAGS = -Wall -I../include 
OBJ_DIR = ./obj
BIN_DIR = ./bin
#TARGET = $(BIN_DIR)/cli
SUB_DIRS = elog server db queue obj 
OBJS = $(wildcard $(OBJ_DIR)/*.o)
export CC CFLAGS OBJ_DIR

all: prepare $(SUB_DIRS) 

prepare:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(SUB_DIRS):
	$(MAKE) -C $@


clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*
	@for dir in $(SUB_DIRS); do \
        $(MAKE) -C $$dir clean; \
	done

.PHONY: all prepare $(SUB_DIRS) clean
