ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	CC = aarch64-none-linux-gnu-gcc
else
	CC = gcc
endif

CFLAGS = -Wall -g
BUILD_DIR = build
EXECUTABLE = $(BUILD_DIR)/writer

all: $(BUILD_DIR) $(EXECUTABLE)

# Rule to link object file(s) and create the executable
$(EXECUTABLE) : $(BUILD_DIR)/writer.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(BUILD_DIR)/writer.o

# Rule to compile source file(s) into object file(s)
$(BUILD_DIR)/writer.o : writer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c writer.c -o $(BUILD_DIR)/writer.o

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR) :
	mkdir -p $(BUILD_DIR)

# Clean up build files
.PHONY : clean
clean :
	rm -rf $(BUILD_DIR)