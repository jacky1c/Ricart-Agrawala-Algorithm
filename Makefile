#
# RicartAgrawala Makefile
#
# Copyright (c) 2017 Robert J. Hilderman.
# All rights reserved.
#

COMPILER=gcc
OPTS=-c -g

DEFS=

OBJS=

INCS=

LIBS=-lrt

all:		environment general

environment:	environment.o
		$(COMPILER) environment.o -o environment

environment.o:	environment.c environment.h
		$(COMPILER) $(OPTS) $(DEFS) $(INCS) environment.c $(LIBS)

general:	ricartAgrawala.o serverThread.o clientThread.o threadIO.o
		$(COMPILER) $(LIBS) ricartAgrawala.o serverThread.o clientThread.o threadIO.o -o general

ricartAgrawala.o: ricartAgrawala.c ricartAgrawala.h
		$(COMPILER) $(OPTS) $(DEFS) $(INCS) ricartAgrawala.c $(LIBS)

serverThread.o:	serverThread.c clientServerThreads.h
		$(COMPILER) $(OPTS) $(DEFS) $(INCS) serverThread.c $(LIBS)

clientThread.o:	clientThread.c clientServerThreads.h
		$(COMPILER) $(OPTS) $(DEFS) $(INCS) clientThread.c $(LIBS)

threadIO.o:	threadIO.c threadIO.h
		$(COMPILER) $(OPTS) $(DEFS) $(INCS) threadIO.c $(LIBS)

clean:
		rm -f environment
		rm -f environment.o
		rm -f general
		rm -f ricartAgrawala.o
		rm -f serverThread.o
		rm -f clientThread.o
		rm -f threadIO.o         

##install:
##		cp -f environment ../bin
##		cp -f general ../bin
