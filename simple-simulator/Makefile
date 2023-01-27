EXE=processor

CC=gcc
CFLAGS=
LIBS=

all: $(EXE)

$(EXE): $(EXE).c
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(EXE)