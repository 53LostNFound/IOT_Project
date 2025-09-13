OBJ_DIR = ../obj
SRC = elog.c elog_async.c elog_buf.c elog_port.c elog_utils.c
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(OBJ)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

.PHONY: all clean
