OBJ_DIR = ../obj
SRC = linkqueue.c 
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(OBJ)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -f $(OBJ)

.PHONY: all clean



	 




