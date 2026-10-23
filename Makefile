CC = cc
CFLAGS = -std=c99 -O2 -Wall -Wextra -Wpedantic

all: byteforge

byteforge: byteforge.c
	$(CC) $(CFLAGS) byteforge.c -o byteforge

clean:
	rm -f byteforge byteforge.exe

.PHONY: all clean
