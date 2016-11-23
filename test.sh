#!/bin/bash
# chmod +x nachosTest.sh
# System setup: NumPhysPages 32
# System setup: NumTracks 64

case $1 in
-r)
ext="in"
rm -rf cmp > /dev/null 2>&1
mkdir cmp
vm/nachos -x test1/halt > cmp/halt.$ext

vm/nachos -x test1/parent_child > cmp/parent_child.$ext

vm/nachos -x test1/argtest > cmp/argtest.$ext

vm/nachos -x test1/matmult > cmp/matmult.$ext

vm/nachos -x test1/maxfork > cmp/maxfork.$ext

vm/nachos -x test1/vmtoobig > cmp/vmtoobig.$ext

vm/nachos -x test1/vmtorture > cmp/vmtorture.$ext
;;

*)
ext="out"

#sleep 5
echo "*****VM -- 50 pts*****"
vm/nachos -x test1/halt > cmp/halt.$ext

vm/nachos -x test1/parent_child > cmp/parent_child.$ext

vm/nachos -x test1/argtest > cmp/argtest.$ext

echo "**** All tests finished *****"
echo "******* Cleaning up *********"
;;
esac

rm -f a.out
rm -f b.out
rm -f c.out
rm -f doesNotExist.out
rm -f fileio.out
rm -f hello.out

# Binary clean up for repo committing ease

#rm -f  test/argkid
#rm -f  test/argkid.coff
#rm -f  test/argkidhalt
#rm -f  test/argkidhalt.coff
#rm -f  test/argseqchild
#rm -f  test/argseqchild.coff
#rm -f  test/argtest
#rm -f  test/argtest.coff
#rm -f  test/atomicwrite
#rm -f  test/atomicwrite.coff
#rm -f  test/cat
#rm -f  test/cat.coff
#rm -f  test/cathalt
#rm -f  test/cathalt.coff
#rm -f  test/cp
#rm -f  test/cp.coff
#rm -f  test/deepfork
#rm -f  test/deepfork.coff
#rm -f  test/deepkid1
#rm -f  test/deepkid1.coff
#rm -f  test/deepkid2
#rm -f  test/deepkid2.coff
#rm -f  test/dupkid
#rm -f  test/dupkid.coff
#rm -f  test/duptest
#rm -f  test/duptest.coff
#rm -f  test/fileio
#rm -f  test/fileio.coff
#rm -f  test/fork
#rm -f  test/fork.coff
#rm -f  test/forkexit
#rm -f  test/forkexit.coff
#rm -f  test/forktest
#rm -f  test/forktest.coff
#rm -f  test/fromcons
#rm -f  test/fromcons.coff
#rm -f  test/grandchildren
#rm -f  test/grandchildren.coff
#rm -f  test/halt
#rm -f  test/halt.coff
#rm -f  test/hellocons
#rm -f  test/hellocons.coff
#rm -f  test/hellofile
#rm -f  test/hellofile.coff
#rm -f  test/kid
#rm -f  test/kid.coff
#rm -f  test/matmult
#rm -f  test/matmult.coff
#rm -f  test/matmulthalt
#rm -f  test/matmulthalt.coff
#rm -f  test/maxfork
#rm -f  test/maxfork.coff
#rm -f  test/parentchild
#rm -f  test/parentchild.coff
#rm -f  test/seqchild
#rm -f  test/seqchild.coff
#rm -f  test/share
#rm -f  test/share.coff
#rm -f  test/sharekid
#rm -f  test/sharekid.coff
#rm -f  test/shell
#rm -f  test/shell.coff
#rm -f  test/sort
#rm -f  test/sort.coff

