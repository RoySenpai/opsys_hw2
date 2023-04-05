CC = gcc
CFLAGS = -Wall 
RM = rm -f

.PHONY: all clean

all: cmp copy encode decode stshell

cmp: cmp.o
	$(CC) $(CFLAGS) -o $@ $^

copy: copy.o
	$(CC) $(CFLAGS) -o $@ $^

encode: encode.o codecA.o codecB.o
	$(CC) $(CFLAGS) -o $@ $^

decode: decode.o codecA.o codecB.o
	$(CC) $(CFLAGS) -o $@ $^

stshell: stshell.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) *.o *.a *.so *.dll *.dylib cmp copy encode decode stshell