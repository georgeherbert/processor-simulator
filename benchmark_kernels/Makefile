CC=riscv64-unknown-elf-gcc
OBJCOPY=riscv64-unknown-elf-objcopy

SRCS=$(wildcard *.c)
ASMS=$(SRCS:.c=.s)
ELFS=$(SRCS:.c=.elf)
BINS=$(SRCS:.c=.bin)

ASFLAGS=-nostdlib -march=rv32i -mabi=ilp32 --entry=_start -S
LDFLAGS=-nostdlib -march=rv32i -mabi=ilp32 --entry=_start

.PHONY: all clean

all: $(ASMS) $(ELFS) $(BINS)

%.s: %.c
	$(CC) $(ASFLAGS) $< -o $@

%.elf: %.s
	$(CC) $(LDFLAGS) $< -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(ASMS) $(ELFS) $(BINS)