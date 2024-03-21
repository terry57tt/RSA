CC=gcc
CFLAGS=-I.
BUILD_DIR=cmake-build-debug

all: $(BUILD_DIR)/serveurTCP $(BUILD_DIR)/clientTCP

$(BUILD_DIR)/serveurTCP: $(BUILD_DIR)/serveurTCP.o
	$(CC) -o $@ $< $(CFLAGS)

$(BUILD_DIR)/serveurTCP.o: serveurTCP.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/clientTCP: $(BUILD_DIR)/clientTCP.o
	$(CC) -o $@ $< $(CFLAGS)

$(BUILD_DIR)/clientTCP.o: clientTCP.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/serveurTCP.o $(BUILD_DIR)/clientTCP.o: $(BUILD_DIR)

clean:
	rm -f $(BUILD_DIR)/*
