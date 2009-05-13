# -*- makefile -*-
# Makefile for Unix with gcc compiler without Posix threads library

CC = g++
DEFS = 
#DEFS = -DPTHREADS 

CFLAGS = -Wall -c -O0 -g $(DEFS)
#CFLAGS = -Wall -c -O5 -g $(DEFS)

LD = $(CC)
LDFLAGS = -g

AR = ar
ARFLAGS = -cru

RANLIB = true

STDLIBS=

TASK_OBJ = ctask.o async.o
#TASK_OBJ = ptask.o

include makefile.uni
