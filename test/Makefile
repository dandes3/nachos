# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as
GCCDIR = /home/f85/kearns/nachos_compiler/bin/
#LDFLAGS = -T script -n -nostartfiles
LDFLAGS = -T nachos.ld -n -nostartfiles
ASFLAGS =
CPPFLAGS = $(INCDIR) -DCHANGED


# if you aren't cross-compiling:
#GCCDIR =
#LDFLAGS = -N -T 0
#ASFLAGS =
#CPPFLAGS = -P $(INCDIR)


CC = $(GCCDIR)decstation-ultrix-gcc
AS = $(GCCDIR)decstation-ultrix-as
LD = $(GCCDIR)decstation-ultrix-ld

CPP = /usr/bin/cpp -E
INCDIR =-I../userprog -I../threads
CFLAGS = -g -G 0 -c $(INCDIR) -DCHANGED -nostartfiles

all: halt shell matmult sort hellocons fromcons hellofile fileio forktest fork cat cp grandchildren parentchild kid seqchild deepfork deepkid1 deepkid2 maxfork argkid argtest argseqchild sharekid share duptest dupkid atomicwrite forkexit argkidhalt cathalt matmulthalt

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

hellocons.o: hellocons.c
	$(CC) $(CFLAGS) -c hellocons.c
hellocons: hellocons.o start.o
	$(LD) $(LDFLAGS) start.o hellocons.o -o hellocons.coff
	../bin/coff2noff hellocons.coff hellocons

fromcons.c: fromcons.c
	$(CC) $(CFLAGS) -c fromcons.c
fromcons: fromcons.o start.o
	$(LD) $(LDFLAGS) start.o fromcons.o -o fromcons.coff
	../bin/coff2noff fromcons.coff fromcons

hellofile.c: hellofile.c
	$(CC) $(CFLAGS) -c hellofile.c
hellofile: hellofile.o start.o
	$(LD) $(LDFLAGS) start.o hellofile.o -o hellofile.coff
	../bin/coff2noff hellofile.coff hellofile

fileio.c: fileio.c
	$(CC) $(CFLAGS) -c fileio.c
fileio: fileio.o start.o
	$(LD) $(LDFLAGS) start.o fileio.o -o fileio.coff
	../bin/coff2noff fileio.coff fileio


forktest.c: forktest.c
	$(CC) $(CFLAGS) -c forktest.c
forktest: forktest.o start.o
	$(LD) $(LDFLAGS) start.o forktest.o -o forktest.coff
	../bin/coff2noff forktest.coff forktest

fork.c: fork.c
	$(CC) $(CFLAGS) -c fork.c
fork: fork.o start.o
	$(LD) $(LDFLAGS) start.o fork.o -o fork.coff
	../bin/coff2noff fork.coff fork

cat.o: cat.c
	$(CC) $(CFLAGS) -c cat.c
cat: cat.o start.o
	$(LD) $(LDFLAGS) start.o cat.o -o cat.coff
	../bin/coff2noff cat.coff cat

cp.o: cp.c
	$(CC) $(CFLAGS) -c cp.c
cp: cp.o start.o
	$(LD) $(LDFLAGS) start.o cp.o -o cp.coff
	../bin/coff2noff cp.coff cp

grandchildren.c: grandchildren.c
	$(CC) $(CFLAGS) -c grandchildren.c
grandchildren: grandchildren.o start.o
	$(LD) $(LDFLAGS) start.o grandchildren.o -o grandchildren.coff
	../bin/coff2noff grandchildren.coff grandchildren

parentchild.c: parentchild.c
	$(CC) $(CFLAGS) -c parentchild.c
parentchild: parentchild.o start.o
	$(LD) $(LDFLAGS) start.o parentchild.o -o parentchild.coff
	../bin/coff2noff parentchild.coff parentchild

kid.c: kid.c
	$(CC) $(CFLAGS) -c kid.c
kid: kid.o start.o
	$(LD) $(LDFLAGS) start.o kid.o -o kid.coff
	../bin/coff2noff kid.coff kid

seqchild.c: seqchild.c
	$(CC) $(CFLAGS) -c seqchild.c
seqchild: seqchild.o start.o
	$(LD) $(LDFLAGS) start.o seqchild.o -o seqchild.coff
	../bin/coff2noff seqchild.coff seqchild

deepfork.c: deepfork.c
	$(CC) $(CFLAGS) -c deepfork.c
deepfork: deepfork.o start.o
	$(LD) $(LDFLAGS) start.o deepfork.o -o deepfork.coff
	../bin/coff2noff deepfork.coff deepfork

deepkid1.c: deepkid1.c
	$(CC) $(CFLAGS) -c deepkid1.c
deepkid1: deepkid1.o start.o
	$(LD) $(LDFLAGS) start.o deepkid1.o -o deepkid1.coff
	../bin/coff2noff deepkid1.coff deepkid1

deepkid2.c: deepkid2.c
	$(CC) $(CFLAGS) -c deepkid2.c
deepkid2: deepkid2.o start.o
	$(LD) $(LDFLAGS) start.o deepkid2.o -o deepkid2.coff
	../bin/coff2noff deepkid2.coff deepkid2

maxfork.c: maxfork.c
	$(CC) $(CFLAGS) -c maxfork.c
maxfork: maxfork.o start.o
	$(LD) $(LDFLAGS) start.o maxfork.o -o maxfork.coff
	../bin/coff2noff maxfork.coff maxfork

argkid.c: argkid.c
	$(CC) $(CFLAGS) -c argkid.c
argkid: argkid.o start.o
	$(LD) $(LDFLAGS) start.o argkid.o -o argkid.coff
	../bin/coff2noff argkid.coff argkid

argtest.c: argtest.c
	$(CC) $(CFLAGS) -c argtest.c
argtest: argtest.o start.o
	$(LD) $(LDFLAGS) start.o argtest.o -o argtest.coff
	../bin/coff2noff argtest.coff argtest

argseqchild.c: argseqchild.c
	$(CC) $(CFLAGS) -c argseqchild.c
argseqchild: argseqchild.o start.o
	$(LD) $(LDFLAGS) start.o argseqchild.o -o argseqchild.coff
	../bin/coff2noff argseqchild.coff argseqchild


share.c: share.c
	$(CC) $(CFLAGS) -c share.c
share: share.o start.o
	$(LD) $(LDFLAGS) start.o share.o -o share.coff
	../bin/coff2noff share.coff share


sharekid.c: sharekid.c
	$(CC) $(CFLAGS) -c sharekid.c
sharekid: sharekid.o start.o
	$(LD) $(LDFLAGS) start.o sharekid.o -o sharekid.coff
	../bin/coff2noff sharekid.coff sharekid

dupkid.c: dupkid.c
	$(CC) $(CFLAGS) -c dupkid.c
dupkid: dupkid.o start.o
	$(LD) $(LDFLAGS) start.o dupkid.o -o dupkid.coff
	../bin/coff2noff dupkid.coff dupkid

duptest.c: duptest.c
	$(CC) $(CFLAGS) -c duptest.c
duptest: duptest.o start.o
	$(LD) $(LDFLAGS) start.o duptest.o -o duptest.coff
	../bin/coff2noff duptest.coff duptest

atomicwrite.c: atomicwrite.c
	$(CC) $(CFLAGS) -c atomicwrite.c
atomicwrite: atomicwrite.o start.o
	$(LD) $(LDFLAGS) start.o atomicwrite.o -o atomicwrite.coff
	../bin/coff2noff atomicwrite.coff atomicwrite

forkexit.c: forkexit.c
	$(CC) $(CFLAGS) -c forkexit.c
forkexit: forkexit.o start.o
	$(LD) $(LDFLAGS) start.o forkexit.o -o forkexit.coff
	../bin/coff2noff forkexit.coff forkexit

argkidhalt.c: argkidhalt.c
	$(CC) $(CFLAGS) -c argkidhalt.c
argkidhalt: argkidhalt.o start.o
	$(LD) $(LDFLAGS) start.o argkidhalt.o -o argkidhalt.coff
	../bin/coff2noff argkidhalt.coff argkidhalt

cathalt.c: cathalt.c
	$(CC) $(CFLAGS) -c cathalt.c
cathalt: cathalt.o start.o
	$(LD) $(LDFLAGS) start.o cathalt.o -o cathalt.coff
	../bin/coff2noff cathalt.coff cathalt

matmulthalt.c: matmulthalt.c
	$(CC) $(CFLAGS) -c matmulthalt.c
matmulthalt: matmulthalt.o start.o
	$(LD) $(LDFLAGS) start.o matmulthalt.o -o matmulthalt.coff
	../bin/coff2noff matmulthalt.coff matmulthalt

