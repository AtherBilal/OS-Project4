CC=gcc
CFLAGS=-Wall -std=gnu99
EXECUTABLES=oss proc
HELPERFILES= src/validation.c
all: $(EXECUTABLES)

oss: src/oss.c
	mkdir -p bin/
	touch bin/CreateKeyFile
	touch bin/log
	$(CC) $(CFLAGS) $< $(HELPERFILES) -o bin/$@
proc: src/proc.c
	$(CC) $(CFLAGS) $< $(HELPERFILES) -o bin/$@
clean:
	rm -rf bin/
