#!/bin/bash
# chmod +x nachosTest.sh
# System setup: NumPhysPages 32
# System setup: NumTracks 64

# .in files are reference files, or the "answer key" for a given run
# .out files are the ones being tested and being compared to the .in files
# both of these exist in the cmp directory

# invoking the -r flag "resets" the .in files and deletes the .out files
# First run of this script should be invoked as ./test.sh -r
# Any following runs that are comparing are invoked as ./test.sh

# most comparisons are being done by identifying the line in which there is a difference between the .in and .out files
 
# One should expect the output for most tests to be the same up until the printed statastics (# of ticks, etc)
# Therefore, I hardcoded the line number the stats appears and check to see if the program is the same up until
# that line

# Some tests are done by checking the number of lines in a file, such as torture
# this is because concurrency should affect the large # of characters, but not the # of lines

# Point structure is taken from the rubric
# When you fail a test, the output of that test is printed and stored in cmp/nameoftest.out

# The biggest downside to this test program is we can't test any files we haven't previously passed. So right now, it
# it thinks we've passed COW. I wish

# I've tested all the files in test1 and test3 for correctness by the rubric



case $1 in
-r)
ext="in"
rm -rf cmp > /dev/null 2>&1
mkdir cmp
cd test1
../vm/nachos -x halt > ../cmp/halt.$ext

../vm/nachos -x parent_child > ../cmp/parent_child.$ext

../vm/nachos -x argtest > ../cmp/argtest.$ext

../vm/nachos -x matmult > ../cmp/matmult.$ext

../vm/nachos -x maxfork > ../cmp/maxfork.$ext

../vm/nachos -x vmtoobig > ../cmp/vmtoobig.$ext

../vm/nachos -x vmtorture > ../cmp/vmtorture.$ext

cd ../test2
../vm/nachos -x cow > ../cmp/cow.$ext

../vm/nachos -x cowwrite > ../cmp/cowwrite.$ext

cd ../test3
../vm/nachos -x ckmatmult > ../cmp/ckmatmult.$ext

../vm/nachos -x ckpttorture > ../cmp/ckpttorture.$ext
;;

*)
ext="out"

#sleep 5
cd test1
echo "*****VM -- 50 pts*****"
echo "test1/halt -- 1 pt"
../vm/nachos -x halt > ../cmp/halt.$ext
statsLine=3
diffOut=$(diff ../cmp/halt.in ../cmp/halt.out)
if [[ -z $diffOut ]]; then
    echo "    passed! +1 pt"
else
    IFS='\n' read -ra diffArr <<< "$diffOut"
    lineDiff=${diffArr[0]:0:1}
    if [[ $lineDiff -eq $statsLine ]]; then
        echo "    passed! +1 pt"
    else
        echo "    FAILED! +0 pts"
        cat ../cmp/halt.out
    fi
fi


echo "test1/parent_child -- 2 pts"
../vm/nachos -x parent_child > ../cmp/parent_child.$ext
statsLine=7
diffOut=$(diff ../cmp/parent_child.in ../cmp/parent_child.out)
if [[ -z $diffOut ]]; then
    echo "    passed! +2 pts"
else
    IFS='\n' read -ra diffArr <<< "$diffOut"
    lineDiff=${diffArr[0]:0:1}
    if [[ $lineDiff -eq $statsLine ]]; then
        echo "    passed! +2 pts"
    else
        echo "    FAILED! +0 pts"
        cat ../cmp/parent_child.out
    fi
fi 


echo "test1/argtest -- 2 pts"
../vm/nachos -x argtest > ../cmp/argtest.$ext
statsLine=9
diffOut=$(diff ../cmp/argtest.in ../cmp/argtest.out)
if [[ -z $diffOut ]]; then
    echo "    passed! +2 pts"
else
    IFS='\n' read -ra diffArr <<< "$diffOut"
    lineDiff=${diffArr[0]:0:1}
    if [[ $lineDiff -eq $statsLine ]]; then
        echo "    passed! +2 pts"
    else
        echo "    FAILED! +0 pts"
        cat ../cmp/argtest.out
    fi
fi 


echo "test1/matmult -- 15 pts"
../vm/nachos -x matmult > ../cmp/matmult.$ext
statsLine=36
diffOut=$(diff ../cmp/matmult.in ../cmp/matmult.out)
if [[ -z $diffOut ]]; then
    echo "    passed! +15 pts"
else
    IFS='\n' read -ra diffArr <<< "$diffOut"
    lineDiff=${diffArr[0]:0:1}
    if [[ $lineDiff -eq $statsLine ]]; then
        echo "    passed! +15 pts"
    else
        echo "    FAILED! +0 pts"
        cat ../cmp/matmult.out
    fi
fi 


echo "test1/maxfork -- 10 pts"
../vm/nachos -x maxfork > ../cmp/maxfork.$ext
statsLine=66
diffOut=$(diff ../cmp/maxfork.in ../cmp/maxfork.out)
if [[ -z $diffOut ]]; then
    echo "    passed! +10 pts"
else
    IFS='\n' read -ra diffArr <<< "$diffOut"
    lineDiff=${diffArr[0]:0:1}
    if [[ $lineDiff -eq $statsLine ]]; then
        echo "    passed! +10 pts"
    else
        echo "    FAILED! +0 pts"
        cat ../cmp/maxfork.out
    fi
fi 


echo "test1/vmtoobig -- 5 pts"
../vm/nachos -x vmtoobig > ../cmp/vmtoobig.$ext
statsLine=6
diffOut=$(diff ../cmp/vmtoobig.in ../cmp/vmtoobig.out)
if [[ -z $diffOut ]]; then
    echo "    passed! +5 pts"
else
    IFS='\n' read -ra diffArr <<< "$diffOut"
    lineDiff=${diffArr[0]:0:1}
    if [[ $lineDiff -eq $statsLine ]]; then
        echo "    passed! +5 pts"
    else
        echo "    FAILED! +0 pts"
        cat ../cmp/vmtoobig.out
    fi
fi 


echo "test1/vmtorture -- 15 pts"
../vm/nachos -x vmtorture > ../cmp/vmtorture.$ext
linesExpected=23
linesRead=$(wc -l < ../cmp/vmtorture.out)
if [[ $linesRead -eq $linesExpected ]]; then
    echo "    passed! +15 pts"
else
    echo "    FAILED! +0 pts"
    cat ../cmp/vmtorture.out
fi



echo ""
echo "*****COW -- 20 pts*****"
cd ../test2

echo "test2/cow"
../vm/nachos -x cow > ../cmp/cow.$ext
statsLine="3"
diffOut=$(diff ../cmp/cow.in ../cmp/cow.out)
if [[ -z $diffOut ]]; then
    echo "    passed! unknown points"
else
    IFS='\n' read -ra diffArr <<< "$diffOut"
    lineDiff=${diffArr[0]:0:1}
    if [[$lineDiff -eq $statsLine]]; then
        echo "    passed! unknown points"
    else
        echo "    FAILED!"
        cat ../cmp/cow.out
    fi
fi


echo "test2/cowwrite"
../vm/nachos -x cowwrite > ../cmp/cowwrite.$ext
statsLine="3"
diffOut=$(diff ../cmp/cowwrite.in ../cmp/cowwrite.out)
if [[ -z $diffOut ]]; then
    echo "    passed! unknown points"
else
    IFS='\n' read -ra diffArr <<< "$diffOut"
    lineDiff=${diffArr[0]:0:1}
    if [[ $lineDiff -eq $statsLine ]]; then
        echo "    passed! unknown points"
    else
        echo "    FAILED!"
        cat ../cmp/cowwrite.out
    fi
fi 

echo ""
echo "*****Checkpoint -- 30 pts*****"

cd ../test3

echo "test3/ckmatmultp -- 15 pts"
../vm/nachos -x ckmatmult > ../cmp/ckmatmult.$ext
statsLine=36
diffOut=$(diff ../cmp/ckmatmult.in ../cmp/ckmatmult.out)
if [[ -z $diffOut ]]; then
    echo "    passed! +15 pts"
else
    IFS='\n' read -ra diffArr <<< "$diffOut"
    lineDiff=${diffArr[0]:0:1}
    if [[ $lineDiff -eq $statsLine ]]; then
        echo "    passed! +15 pts"
    else
        echo "    FAILED! +0 pts"
        cat ../cmp/ckmatmalt.out
    fi
fi 


echo "test3/ckpttorture -- 15 pts"
../vm/nachos -x ckpttorture > ../cmp/ckpttorture.$ext
linesExpected=21
linesRead=$(wc -l < ../cmp/ckpttorture.out)
if [[ $linesExpected -eq $linesRead ]]; then
    echo "    passed! +15 pt"
else
    echo "    FAILED! +0 pts"
    cat ../cmp/ckpttorture.out
fi 

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

