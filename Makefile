#INCLUDE = -I/usr/X11R6/include
#LIBDIR  = -L/usr/X11/lib -L/usr/X11R6/lib
 
COMPILERFLAGS = -fomit-frame-pointer -march=native -Wall -pipe
CC = gcc
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -lglut -lGL -lGLU -lX11 -lXext -lXmu -lXt -lXi -lm

All: viewstl 

viewstl: viewstl.o
	$(CC) $(CFLAGS) -o $@ $(LIBDIR) $< $(LIBRARIES) 