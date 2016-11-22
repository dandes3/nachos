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

all:	ckmatmult rematmult xmatmult qsort Xkid ckpttorture

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

ckmatmult.o: ckmatmult.c
	$(CC) $(CFLAGS) -c ckmatmult.c
ckmatmult: ckmatmult.o start.o
	$(LD) $(LDFLAGS) start.o ckmatmult.o -o ckmatmult.coff
	../bin/coff2noff ckmatmult.coff ckmatmult

rematmult.o: rematmult.c
	$(CC) $(CFLAGS) -c rematmult.c
rematmult: rematmult.o start.o
	$(LD) $(LDFLAGS) start.o rematmult.o -o rematmult.coff
	../bin/coff2noff rematmult.coff rematmult

xmatmult.o: xmatmult.c
	$(CC) $(CFLAGS) -c xmatmult.c
xmatmult: xmatmult.o start.o
	$(LD) $(LDFLAGS) start.o xmatmult.o -o xmatmult.coff
	../bin/coff2noff xmatmult.coff xmatmult

qsort.o: qsort.c
	$(CC) $(CFLAGS) -c qsort.c
qsort: qsort.o start.o
	$(LD) $(LDFLAGS) start.o qsort.o -o qsort.coff
	../bin/coff2noff qsort.coff qsort

Xkid.o: Xkid.c
	$(CC) $(CFLAGS) -c Xkid.c
Xkid: Xkid.o start.o
	$(LD) $(LDFLAGS) start.o Xkid.o -o Xkid.coff
	../bin/coff2noff Xkid.coff Xkid

ckpttorture.o: ckpttorture.c
	$(CC) $(CFLAGS) -c ckpttorture.c
ckpttorture: ckpttorture.o start.o
	$(LD) $(LDFLAGS) start.o ckpttorture.o -o ckpttorture.coff
	../bin/coff2noff ckpttorture.coff ckpttorture
