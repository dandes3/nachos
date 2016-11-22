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

all:	halt parent_child kid argtest argkid maxfork matmult vmtoobig huge qmatmult qsort Xkid vmtorture

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

maxfork.o: maxfork.c
	$(CC) $(CFLAGS) -c maxfork.c
maxfork: maxfork.o start.o
	$(LD) $(LDFLAGS) start.o maxfork.o -o maxfork.coff
	../bin/coff2noff maxfork.coff maxfork

parent_child.o: parent_child.c
	$(CC) $(CFLAGS) -c parent_child.c
parent_child: parent_child.o start.o
	$(LD) $(LDFLAGS) start.o parent_child.o -o parent_child.coff
	../bin/coff2noff parent_child.coff parent_child
kid.o: kid.c
	$(CC) $(CFLAGS) -c kid.c
kid: kid.o start.o
	$(LD) $(LDFLAGS) start.o kid.o -o kid.coff
	../bin/coff2noff kid.coff kid

argtest.o: argtest.c
	$(CC) $(CFLAGS) -c argtest.c
argtest: argtest.o start.o
	$(LD) $(LDFLAGS) start.o argtest.o -o argtest.coff
	../bin/coff2noff argtest.coff argtest

argkid.o: argkid.c
	$(CC) $(CFLAGS) -c argkid.c
argkid: argkid.o start.o
	$(LD) $(LDFLAGS) start.o argkid.o -o argkid.coff
	../bin/coff2noff argkid.coff argkid

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

vmtoobig.o: vmtoobig.c
	$(CC) $(CFLAGS) -c vmtoobig.c
vmtoobig: vmtoobig.o start.o
	$(LD) $(LDFLAGS) start.o vmtoobig.o -o vmtoobig.coff
	../bin/coff2noff vmtoobig.coff vmtoobig

huge.o: huge.c
	$(CC) $(CFLAGS) -c huge.c
huge: huge.o start.o
	$(LD) $(LDFLAGS) start.o huge.o -o huge.coff
	../bin/coff2noff huge.coff huge

qmatmult.o: qmatmult.c
	$(CC) $(CFLAGS) -c qmatmult.c
qmatmult: qmatmult.o start.o
	$(LD) $(LDFLAGS) start.o qmatmult.o -o qmatmult.coff
	../bin/coff2noff qmatmult.coff qmatmult

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


vmtorture.o: vmtorture.c
	$(CC) $(CFLAGS) -c vmtorture.c
vmtorture: vmtorture.o start.o
	$(LD) $(LDFLAGS) start.o vmtorture.o -o vmtorture.coff
	../bin/coff2noff vmtorture.coff vmtorture

