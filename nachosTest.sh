#!/bin/bash
# chmod +x nachosTest.sh
echo "*****Assume everytest halts unless its filename suggests otherwise*****"
echo ""

echo "*****Running argseqchild"
userprog/nachos -x test/argseqchild
echo "*****Finished argseqchild"
echo ""

echo "*****Running argtest*****"
userprog/nachos -x test/argtest
echo "*****Finished argtest****"
echo ""

echo "*****Running atomicwrite*****"
rm atomicFile > /dev/null 2>&1
userprog/nachos -x test/atomicwrite
echo "*****Printing atomicFile*****"
cat atomicFile
echo ""
echo "*****Finished atomicwrite****"
echo ""

echo "*****Running deepfork******"
userprog/nachos -x test/deepfork
echo "*****Finished deepfork*****"
echo ""

echo "*****Running duptest*****"
userprog/nachos -x test/duptest
echo "*****Finished duptest****"
echo ""

echo "*****Running fileio************"
rm fileio.out > /dev/null 2>&1
userprog/nachos -x test/fileio
echo "*****Printing fileio.input*****"
cat fileio.input
echo "*****Printing fileio.out*******"
cat fileio.out
echo "*****Finished fileio*****"
echo ""

echo "*****Running fork****"
userprog/nachos -x test/fork
echo "*****Finished fork***"
echo ""

echo "*****Running  forktest****"
userprog/nachos -x test/forktest
echo "*****Finished forktest****"
echo ""

echo "*****Running fromcons*****"
userprog/nachos -x test/fromcons <<< 'abcdqQ'
echo "*****Finished fromcons****"
echo ""

echo "*****Running grandchildren*****"
userprog/nachos -x test/grandchildren
echo "*****Finished grandchildren****"
echo ""

echo "*****Running hellocons*****"
userprog/nachos -x test/hellocons
echo "*****Finished hellocons****"
echo ""

echo "*****Running hellofile*****"
userprog/nachos -x test/hellofile
echo "*****Finished hellofile****"
echo ""

echo "*****Running matmult*****"
timeout 1 userprog/nachos -x test/matmult
echo "*****Finished matmult****"
echo ""

echo "*****Running maxfork*****"
userprog/nachos -x test/maxfork
echo "*****Finished maxfork****"
echo ""

echo "*****Running parentchild*****"
userprog/nachos -x test/parentchild
echo "*****Finished parentchild****"
echo ""

echo "*****Running seqchild*****"
userprog/nachos -x test/seqchild
echo "*****Finished seqchild****"
echo ""

echo "*****Running script*****"
userprog/nachos -x test/script
echo "*****Finished script****"
echo ""

echo "*****Running share*****"
userprog/nachos -x test/share
echo "*****Finished share****"
echo ""

echo "*****Running various shell tests*****"
echo "***Running shell with argkid***"
timeout 1 userprog/nachos -x test/shell < test/shellArgKid
echo "***Finished shell with argkid**"




echo "*****Running sort*****"
timeout 1 userprog/nachos -x test/sort
echo "*****Finished sort****"
echo ""
