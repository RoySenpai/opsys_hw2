#################################################################################
# 	Operation Systems (OSs) Course Assignment 2 Makefile			#
# 	Authors: Roy Simanovich and Linor Ronen (c) 2023			#
# 	Description: This Makefile compiles the programs and libraries 		#
# 				Date: 2023-04					#
# 			Course: Operating Systems				#
# 				Assignment: 2					#
# 				Compiler: gcc					#
# 				OS: Linux					#
# 			IDE: Visual Studio Code					#
#################################################################################

# Flags for the compiler and linker.
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g
RM = rm -f
SHAREDLIB = -shared
DLIBFLAGS = -ldl

# Phony targets - targets that are not files but commands to be executed by make.
.PHONY: all clean

# Default target - compile everything and create the executables and libraries.
all: cmp copy encode decode stshell


############
# Programs #
############

# Part A #
cmp: cmp.o
	$(CC) $(CFLAGS) -o $@ $^

copy: copy.o
	$(CC) $(CFLAGS) -o $@ $^

# Part B #
encode: encode.o libcodecA.so libcodecB.so
	$(CC) $(CFLAGS) $(DLIBFLAGS) -o $@ encode.o

decode: decode.o libcodecA.so libcodecB.so
	$(CC) $(CFLAGS) $(DLIBFLAGS) -o $@ decode.o

# Part C #
stshell: stshell.o
	$(CC) $(CFLAGS) -o $@ $^


##################################
# Libraries and shared libraries #
##################################
libcodecA.so: codecA.o
	$(CC) $(CFLAGS) $(SHAREDLIB) -o $@ $^

libcodecB.so: codecB.o
	$(CC) $(CFLAGS) $(SHAREDLIB) -o $@ $^

codecA.o: codecA.c codec.h
	$(CC) $(CFLAGS) -fPIC -c $^

codecB.o: codecB.c codec.h
	$(CC) $(CFLAGS) -fPIC -c $^


################
# Object files #
################
stshell.o: stshell.c stshell.h
	$(CC) $(CFLAGS) -c $^

%.o: %.c
	$(CC) $(CFLAGS) -c $^
	
#################
# Cleanup files #
#################
clean:
	$(RM) *.gch *.o *.a *.so *.dll *.dylib cmp copy encode decode stshell