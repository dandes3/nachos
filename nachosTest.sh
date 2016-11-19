#!/bin/bash
# chmod +x nachosTest.sh

echo "*****Passing input to shell required the input in a seperate file. I always print the input before running***"
echo "*****Furthermore, passing input to shell doesn't allow the inner program to exit, it must halt. I tested that the program works correctly outside of this bash script, but for the purpose of this script, I made some files halt where they normally exit"
echo ""

sleep 5

echo "*****Running argseqchild"
vm/nachos -x test/argseqchild
echo "*****Finished argseqchild"
echo ""

sleep 1

echo "*****Running argtest*****"
vm/nachos -x test/argtest
echo "*****Finished argtest****"
echo ""

sleep 1

echo "*****Running atomicwrite*****"
rm atomicFile > /dev/null 2>&1
vm/nachos -x test/atomicwrite

sleep 1

echo "*****Printing atomicFile*****"
cat atomicFile
echo ""
echo "*****Finished atomicwrite****"
echo ""

sleep 1

echo "*****Running deepfork******"
vm/nachos -x test/deepfork
echo "*****Finished deepfork*****"
echo ""

sleep 1

echo "*****Running duptest*****"
vm/nachos -x test/duptest
echo "*****Finished duptest****"
echo ""

sleep 1

echo "*****Running fileio************"
rm fileio.out > /dev/null 2>&1
vm/nachos -x test/fileio

sleep 1

echo "*****Printing fileio.input*****"
cat fileio.input
echo "*****Printing fileio.out*******"
cat fileio.out
echo "*****Finished fileio*****"
echo ""

sleep 1

echo "*****Running fork****"
vm/nachos -x test/fork
echo "*****Finished fork***"
echo ""

sleep 1

echo "*****Running  forktest****"
vm/nachos -x test/forktest
echo "*****Finished forktest****"
echo ""

sleep 1

echo "*****Running fromcons*****"
vm/nachos -x test/fromcons <<< 'abcdqQ'
echo "*****Finished fromcons****"
echo ""

sleep 1

echo "*****Running grandchildren*****"
vm/nachos -x test/grandchildren
echo "*****Finished grandchildren****"
echo ""

sleep 1

echo "*****Running hellocons*****"
vm/nachos -x test/hellocons
echo "*****Finished hellocons****"
echo ""

sleep 1

echo "*****Running hellofile*****"
vm/nachos -x test/hellofile
echo "*****Finished hellofile****"
echo ""

sleep 1

echo "*****Running matmult*****"
timeout 1 vm/nachos -x test/matmult
echo "*****Finished matmult****"
echo ""

sleep 1

echo "*****Running maxfork*****"
vm/nachos -x test/maxfork
echo "*****Finished maxfork****"
echo ""

sleep 1

echo "*****Running parentchild*****"
vm/nachos -x test/parentchild
echo "*****Finished parentchild****"
echo ""

sleep 1

echo "*****Running seqchild*****"
vm/nachos -x test/seqchild
echo "*****Finished seqchild****"
echo ""

sleep 1

echo "*****Printing scripttest****"
cat test/scripttest
echo "*****Running scripttest*****"
vm/nachos -x test/scripttest
echo "*****Finished scripttest****"
echo ""

sleep 1

echo "*****Running share*****"
vm/nachos -x test/share
echo "*****Finished share****"
echo ""

sleep 3

echo "*****Running various shell tests*************"

echo "---Printing input to shell---"
cat test/shellargkid
echo "---Finished printing input---"

sleep 1

echo "***Testing exec with args in shell***"
timeout 1 vm/nachos -x test/shell < test/shellargkid
echo "***Finished exec with args in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellargseqchild
echo "---Finished printing input---"

sleep 1

echo "***Testing argseqchild in shell***"
timeout 1 vm/nachos -x test/shell < test/shellargseqchild
echo "***Finished argseqchild in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellargtest
echo "---Finished printing input---"

sleep 1

echo "***Testing argtest in shell***"
timeout 1 vm/nachos -x test/shell < test/shellargtest
echo "***Finished argtest in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellatomicwrite
echo "---Finished printing input---"

sleep 1

echo "***Testing atomic write in shell***"
timeout 1 vm/nachos -x test/shell < test/shellatomicwrite
echo "***Finished atomicwrite in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellcat
echo "---Finished printing input---"

sleep 1

echo "***Testing cat in shell***"
timeout 1 vm/nachos -x test/shell < test/shellcat
echo "***Finished cat in shell**"

sleep 1

echo "---Printing input to shell---"
rm doesNotExist.out
cat test/shellcp
echo "---Finished printing input---"

sleep 1

echo "***Testing cp in shell***"
timeout 1 vm/nachos -x test/shell < test/shellcp
echo "***Finished cp in shell**"

sleep 1

echo "*****Finished running various shell tests*****"
echo ""

sleep 1

echo "---Printing input to shell---"
cat test/shelldeepfork
echo "---Finished printing input---"

sleep 1

echo "***Testing deepfork in shell***"
timeout 1 vm/nachos -x test/shell < test/shelldeepfork
echo "***Finished deepfork in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellfileio
echo "---Finished printing input---"

sleep 1

echo "***Testing fileio in shell***"
timeout 1 vm/nachos -x test/shell < test/shellfileio
echo "***Finished fileio in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellfork
echo "---Finished printing input---"

sleep 1

echo "***Testing fork in shell***"
timeout 1 vm/nachos -x test/shell < test/shellfork
echo "***Finished cat in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellforktest
echo "---Finished printing input---"

sleep 1

echo "***Testing forktest in shell***"
timeout 1 vm/nachos -x test/shell < test/shellforktest
echo "***Finished forktest in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellfromcons
echo "---Finished printing input---"

sleep 1

echo "***Testing fromcons in shell***"
timeout 1 vm/nachos -x test/shell < test/shellfromcons
echo "***Finished fromcons in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellgrandchildren
echo "---Finished printing input---"

sleep 1

echo "***Testing grandchildren in shell***"
timeout 1 vm/nachos -x test/shell < test/shellgrandchildren
echo "***Finished grandchildren in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellhellocons
echo "---Finished printing input---"

sleep 1

echo "***Testing hellocons in shell***"
timeout 1 vm/nachos -x test/shell < test/shellhellocons
echo "***Finished hellocons in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellhellofile
echo "---Finished printing input---"

sleep 1

echo "***Testing hellofile in shell***"
timeout 1 vm/nachos -x test/shell < test/shellhellofile
echo "***Finished hellofile in shell**"


sleep 1

echo "---Printing input to shell---"
cat test/shellmatmult
echo "---Finished printing input---"
sleep 1

echo "***Testing matmult in shell***"
timeout 1 vm/nachos -x test/shell < test/shellmatmult
echo "***Finished matmult in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellmaxfork
echo "---Finished printing input---"
sleep 1

echo "***Testing maxfork in shell***"
timeout 1 vm/nachos -x test/shell < test/shellmaxfork
echo "***Finished maxfork in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellparentchild
echo "---Finished printing input---"
sleep 1

echo "***Testing parentchild in shell***"
timeout 1 vm/nachos -x test/shell < test/shellparentchild
echo "***Finished parentchild in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellseqchild
echo "---Finished printing input---"
sleep 1

echo "***Testing seqchild in shell***"
timeout 1 vm/nachos -x test/shell < test/shellseqchild
echo "***Finished seqchild in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellshare
echo "---Finished printing input---"
sleep 1

echo "***Testing share in shell***"
timeout 1 vm/nachos -x test/shell < test/shellshare
echo "***Finished share in shell**"

sleep 1

echo "---Printing input to shell---"
cat test/shellshell
echo "---Finished printing input---"

sleep 1

echo "***Testing shell in shell***"
timeout 1 vm/nachos -x test/shell < test/shellshell
echo "***Finished shell in shell**"

sleep 1


echo "*****Finished various shell tests****"
echo ""
sleep 1

echo "*****Running sort*****"
timeout 1 vm/nachos -x test/sort
echo "*****Finished sort****"
echo ""

echo "**** All tests finished *****"
echo "******* Cleaning up *********"

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

