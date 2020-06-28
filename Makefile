CFLAGS = -c -Wall
LDFLAGS = -lcrypto

# Folders
SRC=src
BIN=bin
OBJ=$(BIN)/obj

$(BIN)/example: $(OBJ)/example.o $(OBJ)/votersignature.o
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ)/example.o: $(SRC)/example.c
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ)/votersignature.o: $(SRC)/votersignature.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BIN)
	mkdir -p $(OBJ)
