GCCDIR = /home/f85/kearns/nachos_compiler/bin/
LDFLAGS = -T nachos.ld -n -nostartfiles
ASFLAGS =
CPPFLAGS = $(INCDIR) -DCHANGED

CC = $(GCCDIR)decstation-ultrix-gcc
AS = $(GCCDIR)decstation-ultrix-as
LD = $(GCCDIR)decstation-ultrix-ld

CPP = /usr/bin/cpp -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR) -DCHANGED -nostartfiles

all:	cow cowwrite

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

cow.o: cow.c
	$(CC) $(CFLAGS) -c cow.c
cow: cow.o start.o
	$(LD) $(LDFLAGS) start.o cow.o -o cow.coff
	../bin/coff2noff cow.coff cow

cowwrite.o: cowwrite.c
	$(CC) $(CFLAGS) -c cowwrite.c
cowwrite: cowwrite.o start.o
	$(LD) $(LDFLAGS) start.o cowwrite.o -o cowwrite.coff
	../bin/coff2noff cowwrite.coff cowwrite
