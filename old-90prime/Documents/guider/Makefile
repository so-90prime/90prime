# Makefile for addnums example routine.
#
# G. Grant Williams May 24, 2001
#
 
CC=gcc
CFLAGS=-g -Wall -fpic -ansi
#LDFLAGS=-L/usr/X11R6/lib -lm -lX11 -lg2c
LDFLAGS=-lm

OBJ = addnums.o

# 
# Compiler flags:
# 
# 	-g		-- enable debugging
# 	-Wall		-- turn on all warnings
#	-ansi		-- Don't use Gnu extensions, stick to Ansi C.
#
#
# Different compilation methods that work.
#
#.c.o: foo.h
#	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<
#
#%.o: %.c foo.h
#	$(CC) -c $(CFLAGS) -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c $<

addnums: $(OBJ)
	$(CC) $(LDFLAGS) -shared -o addnums.so $(OBJ)

clean:
	rm -f addnums.so $(OBJ) 
