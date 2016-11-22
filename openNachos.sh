#!/bin/bash
# chmod +x openNachos.sh
# run in the main nachos directory
# ./openNachos.sh                     -- opens all test directories
# ./openNachos.sh num1 num2 num3 num4 -- opens test directory corresponding to the num
#     for example, if num1 = 0 and num2 = 3, then test and test3 directories are open
#     takes in 0 - 4 arguments


case $1 in
0)
kate test/*.c
;;
1)
kate test1/*.c
;;
2)
kate test2/*.c
;;
3)
kate test3/*.c
;;
*)
kate test/*.c test1/*.c test2/*.c test3/*.c
;;
esac

case $2 in
0)
kate test/*.c
;;
1)
kate test1/*.c
;;
2)
kate test2/*.c
;;
3)
kate test3/*.c
;;
*)
;;
esac

case $3 in
0)
kate test/*.c
;;
1)
kate test1/*.c
;;
2)
kate test2/*.c
;;
3)
kate test3/*.c
;;
*)
;;
esac

case $4 in
0)
kate test/*.c
;;
1)
kate test1/*.c
;;
2)
kate test2/*.c
;;
3)
kate test3/*.c
;;
*)
;;
esac


kate \
filesys/*.cc filesys/*.h \
machine/*.cc machine/*.h \
#test/*.c test/start.s \
#test1/*.c \
#test2/*.c \
#test3/*.c \
threads/*.cc threads/*.h \
userprog/*.cc userprog/*.h 2>/dev/null &
