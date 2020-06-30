CFLAGS = -c -Wall
LDFLAGS = -lcrypto

# Folders
SRC=src
BIN=bin
OBJ=$(BIN)/obj

$(BIN)/example: $(OBJ)/example.o $(OBJ)/votersignature.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN)/test_performance: $(OBJ)/test_performance.o $(OBJ)/votersignature.o
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ)/example.o: $(SRC)/example.c
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ)/test_performance.o: $(SRC)/test_performance.c
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ)/votersignature.o: $(SRC)/votersignature.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -rf $(BIN)
	mkdir -p $(OBJ)
