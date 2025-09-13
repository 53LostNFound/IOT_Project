OBJ_DIR = ../obj
SRC = sqlite3_op.c 
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(OBJ)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lsqlite3

clean:
	rm -f $(OBJ)

.PHONY: all clean



	 




