# Compiler flags
CFLAGS = -Wall -Wextra -pedantic -O3 -std=c11

# Source files
SRCS = cpu.c main_memory.c fetch.c decode.c execute.c memory.c writeback.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = cpu

.PHONY: all clean

# Default target
all: $(TARGET)

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)