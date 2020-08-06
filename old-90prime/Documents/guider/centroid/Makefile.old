# Makefile for guide camera centroiding.
#
# G. Grant Williams April 14, 2001
#
 
CC=gcc
CFLAGS=-g -Wall -ansi
#LDFLAGS=-L/usr/X11R6/lib -lm -lX11 -lg2c
LDFLAGS=-lm

OBJ = testcent.o centroid.o mode.o

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

.c.o: cent.h
	$(CC) $(CFLAGS) -c $<

testcent: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o testcent 

centroid: centroid.o
	$(CC) $(LDFLAGS) centroid.o -o centroid

mode: mode.o
	$(CC) $(LDFLAGS) mode.o -o mode

clean:
	rm -f testcent $(OBJ) 
