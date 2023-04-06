# Author: Roy Simanovich and Linor Ronen (c) 2023
# Description: Makefile for assignment 2 of course "Operating Systems" in Ariel University
# Date: 2023-04
# Version: 1.0
# Course: Operating Systems
# Assignment: 2
# Exercise: 2
# Compiler: gcc
# OS: Linux
# IDE: Visual Studio Code


# Flags for the compiler
CC = gcc
CFLAGS = -Wall 
RM = rm -f
SHAREDLIB = -shared
DLIBFLAGS = -ldl

.PHONY: all clean

# Default target - compile everything
all: cmp copy encode decode stshell

# Programs

# Part A
cmp: cmp.o
	$(CC) $(CFLAGS) -o $@ $^

copy: copy.o
	$(CC) $(CFLAGS) -o $@ $^

# Part B
encode: encode.o libcodecA.so libcodecB.so
	$(CC) $(CFLAGS) $(DLIBFLAGS) -o $@ encode.o

decode: decode.o libcodecA.so libcodecB.so
	$(CC) $(CFLAGS) $(DLIBFLAGS) -o $@ decode.o

# Part C
stshell: stshell.o
	$(CC) $(CFLAGS) -o $@ $^


# Libraries and shared libraries
libcodecA.so: codecA.o
	$(CC) $(CFLAGS) $(SHAREDLIB) -o $@ $^

libcodecB.so: codecB.o
	$(CC) $(CFLAGS) $(SHAREDLIB) -o $@ $^

codecA.o: codecA.c codec.h
	$(CC) $(CFLAGS) -fPIC -c $^

codecB.o: codecB.c codec.h
	$(CC) $(CFLAGS) -fPIC -c $^


# Object files
%.o: %.c
	$(CC) $(CFLAGS) -c $^
	

# Cleanup files
clean:
	$(RM) *.o *.a *.so *.dll *.dylib cmp copy encode decode stshell