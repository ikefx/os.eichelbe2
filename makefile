# Neil Eichelberger
# cs4760 Assignment 02 makefile
# 19/2/2019

CC = cc
RM = rm
CFLAGS = -Wall -ansi -lm -std=gnu99
LIBS = -lrt
.SUFFIXES: 
.SUFFIXES: .c .o .h

all: oss user

# explicit
oss: oss.o
	$(CC) $(CFLAGS) -o oss oss.o $(LIBS)
user: user.o
	$(CC) $(CFLAGS) -o user user.o $(LIBS)
# implicit

oss.o: oss.c
	$(CC) -c -g $(CFLAGS) oss.c
user.o: user.c
	$(CC) -c -g $(CFLAGS) user.c

# clean up
clean:
	$(RM) -f $(OBJ) oss.o oss user.o user output.txt

