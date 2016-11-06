#!/bin/bash
# chmod +x nachosTest.sh

echo "*****Passing input to shell required the input in a seperate file. I always print the input before running***"
echo "*****Furthermore, passing input to shell doesn't allow the inner program to exit, it must halt. I tested that the program works correctly outside of this bash script, but for the purpose of this script, I made some files halt where they normally exit"
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

echo "*****Printing scripttest****"
cat test/scripttest
echo "*****Running scripttest*****"
userprog/nachos -x test/scripttest
echo "*****Finished scripttest****"
echo ""

echo "*****Running share*****"
userprog/nachos -x test/share
echo "*****Finished share****"
echo ""



echo "*****Running various shell tests*************"

echo "---Printing input to shell---"
cat test/shellargkid
echo "---Finished printing input---"
echo "***Testing exec with args in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellargkid
echo "***Finished exec with args in shell**"

echo "---Printing input to shell---"
cat test/shellargseqchild
echo "---Finished printing input---"
echo "***Testing argseqchild in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellargseqchild
echo "***Finished argseqchild in shell**"

echo "---Printing input to shell---"
cat test/shellargtest
echo "---Finished printing input---"
echo "***Testing argtest in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellargtest
echo "***Finished argtest in shell**"

echo "---Printing input to shell---"
cat test/shellatomicwrite
echo "---Finished printing input---"
echo "***Testing atomic write in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellatomicwrite
echo "***Finished atomicwrite in shell**"

echo "---Printing input to shell---"
cat test/shellcat
echo "---Finished printing input---"
echo "***Testing cat in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellcat
echo "***Finished cat in shell**"

echo "---Printing input to shell---"
rm doesNotExist.out
cat test/shellcp
echo "---Finished printing input---"
echo "***Testing cp in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellcp
echo "***Finished cp in shell**"

echo "*****Finished running various shell tests*****"
echo ""

echo "---Printing input to shell---"
cat test/shelldeepfork
echo "---Finished printing input---"
echo "***Testing deepfork in shell***"
timeout 1 userprog/nachos -x test/shell < test/shelldeepfork
echo "***Finished deepfork in shell**"

echo "---Printing input to shell---"
cat test/shellfileio
echo "---Finished printing input---"
echo "***Testing fileio in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellfileio
echo "***Finished fileio in shell**"

echo "---Printing input to shell---"
cat test/shellfork
echo "---Finished printing input---"
echo "***Testing fork in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellfork
echo "***Finished cat in shell**"

echo "---Printing input to shell---"
cat test/shellforktest
echo "---Finished printing input---"
echo "***Testing forktest in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellforktest
echo "***Finished forktest in shell**"

echo "---Printing input to shell---"
cat test/shellfromcons
echo "---Finished printing input---"
echo "***Testing fromcons in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellfromcons
echo "***Finished fromcons in shell**"

echo "---Printing input to shell---"
cat test/shellgrandchildren
echo "---Finished printing input---"
echo "***Testing grandchildren in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellgrandchildren
echo "***Finished grandchildren in shell**"

echo "---Printing input to shell---"
cat test/shellhellocons
echo "---Finished printing input---"
echo "***Testing hellocons in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellhellocons
echo "***Finished hellocons in shell**"

echo "---Printing input to shell---"
cat test/shellhellofile
echo "---Finished printing input---"
echo "***Testing hellofile in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellhellofile
echo "***Finished hellofile in shell**"

echo "---Printing input to shell---"
cat test/shellmatmult
echo "---Finished printing input---"
echo "***Testing matmult in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellmatmult
echo "***Finished matmult in shell**"

echo "---Printing input to shell---"
cat test/shellmaxfork
echo "---Finished printing input---"
echo "***Testing maxfork in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellmaxfork
echo "***Finished maxfork in shell**"

echo "---Printing input to shell---"
cat test/shellparentchild
echo "---Finished printing input---"
echo "***Testing parentchild in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellparentchild
echo "***Finished parentchild in shell**"

echo "---Printing input to shell---"
cat test/shellseqchild
echo "---Finished printing input---"
echo "***Testing seqchild in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellseqchild
echo "***Finished seqchild in shell**"

echo "---Printing input to shell---"
cat test/shellshare
echo "---Finished printing input---"
echo "***Testing share in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellshare
echo "***Finished share in shell**"

echo "---Printing input to shell---"
cat test/shellshell
echo "---Finished printing input---"
echo "***Testing shell in shell***"
timeout 1 userprog/nachos -x test/shell < test/shellshell
echo "***Finished shell in shell**"


echo "*****Finished various shell tests****"
echo ""
echo "*****Running sort*****"
timeout 1 userprog/nachos -x test/sort
echo "*****Finished sort****"
echo ""
