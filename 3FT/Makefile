#--------------------------------------------------------------------
# Makefile for Assignment 4, Part 3
# dt* targets are built using checkerDT
# Author: Mandy Kao & Sunmin Eom
#--------------------------------------------------------------------

GCC = gcc217
#GCC = gcc217m

all: ft

clean:
	rm -f ft *.o meminfo*.out

clobber: clean
	rm -f *~ \#*\#

ft: dynarray.o path.o nodeFT.o ft.o ft_client.o
	$(GCC) -g dynarray.o path.o nodeFT.o ft.o ft_client.o -o ft

dynarray.o: dynarray.c dynarray.h
	$(GCC) -g -c dynarray.c

path.o: path.c dynarray.h path.h a4def.h
	$(GCC) -g -c path.c

ft_client.o: ft_client.c ft.h a4def.h
	$(GCC) -g -c ft_client.c

nodeFT.o: nodeFT.c dynarray.h nodeFT.h path.h a4def.h
	$(GCC) -g -c nodeFT.c

ft.o: ft.c dynarray.h nodeFT.h ft.h path.h a4def.h
	$(GCC) -g -c ft.c
