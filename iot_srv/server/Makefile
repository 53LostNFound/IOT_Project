OBJ_DIR = ../obj
SRC = iot_server.c recv_coord_thread.c coord.c recv_mob_thread.c out_queue_thread.c out_cmd_thread.c
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(OBJ)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

.PHONY: all clean

	 
