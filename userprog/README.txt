-------------------------------------------
| Don Andes, Jose Acuna, Phillip Favaloro
| Nachos Project 2
-------------------------------------------

Compiled and modified as coursework for CSCI 444, The College of William & Mary, Fall 2016.

-------------------------------------------------------------------------------------------
---- This code is provided as is and has been created strictly for educational purposes ---
-------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------
|                      `...```           .---.                                             |
|  ohooosd-  +hssd`   /y+++/h/       -ohsoo+os+. Nooy:  sy++d-    .+s++o:       :syso+.    |
|  +s    -h. +o  h.   d-    sy`    `sd/`     -h: m` /s  d: -N-  `od/`  `:s/    ym/`  .oy-  |
|  +h     .h`+o  y-  :h  /  .m+    sd. -os++yy.  m: -d  d: :M-  od.       oy` -M- .ss+yy-  |
|  +y  --  -ys+  h.  ho ;:/  +d:  /d- :do` `.    N+ .y++s- -N- -h: -yos+   ys .N- /m//-    |
|  +s  -m+  -d/  d. `N: ::-   ds `om  sh`        mo `:---` `N- /y  y` `d.  -m  .oo-/dh-    |
|  +o  -dy+  ::  d` oh  ://+. -h +-N. om`  --.   do .m//h:  m- /y  d.  m`  `N`   .sy-:ys`  |
|  +y  :y ss     d  d+  s/`yh  +y -sy` +ysyo/ohy`mo  m  s+  h- .m- -s++:   /h- ss-.sd  ho  |
|  +d  +o  ys   `y: m` -d` `d+  ys `oo. ```  -dy do `N  os  s-  /d.  `    :ys d--/++-  hs  |
|  +m..o+   so/:o;y +--ho   -d:.:m+  :oo/:/ohd+` os/od` /y++y-   -s+:..:+s+`` oh/`   .ss`  |
|  -+++/:    ---- : ::::.    /o++ss   `./++/-`   `````   ````      .-:/:-`     `:///+:.    |
--------------------------------------------------------------------------------------------


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HAIKU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ It was just our luck ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ This project was "trivial" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ We stopped giving fucks ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Usage
-----
    This project includes a makefile. In the parent directory, do a %make, this will 
    compile and prepare the full Nachos OS and included programs for execution. 
    In the >userprog directory, there will now be a nachos executable.

    All parts of the assignment have been completed and are expected to work properly. 
    
     Basic commentary/conditions to review before testing: 
     - We have tested all of our implementation functions with a num pages value >= 256.
       Most of our tests work with lower values, but this will assure you (the user) obtain 
       the same results that we intended. As per the specifications, this is the only 
       modification to the machine directory.

     - Assuming userprog/nachos is invoked from the parent, executables should be passed
       in the form test/maxfork.

     - We have implemented the Exec() call with args, meaning any call to Exec() will 
       require either arguments passed in an array of char*s, or in its place a Null Pointer. 

     - We supply our makefile for the test subdirectory, along with all of the pre-made tests
       we have created. If any of these tests are to be run, our makefile will have to be 
       used, not the originally supplied default makefile. 

     - We have included a bash test script that runs all tests required by the specifications,
       and we feel demonstrates effectively that all parts of the assignment are tested working. 
       Check it out. 
       
     - It should be noted exec must be called from within a process. (i.e., you cannot run exec 
       from the line you run nachos- userprog/nachos -x test/argkid arg1 is invalid)

     - Any #SCRIPT file to be executed must follow lines with a newline character. It just does.




Commentary
----------
   (1) On syscalls and exception handling:
       - General notes about exception.cc: 
         We use the ConvertAddr function to constantly and consistantly convert accepted virtual
         addresses to physical addresses. We pull arguments from user memory with ReadArg into 
         the kernel, and then call ConvertAddr to translate the Virtual Address that is passed 
         as an argument. The implemented IncrementPc() function pushes the program counter
         forward at the end of every syscall, except those that end abnormally (such as exec 
         and exit). We made the design decision to handle as much of the syscall related 
         actions inside the AddrSpace class as we logically could. This was, we felt, the most 
         straightforward locatin for these actions to be handled, and allowed us to increase
         orginization and cleanliness of our codebase.

         Syscalls:
         - Create:
           Pulls the filename into the kernel, and uses the embedded fileSystem create function
           to create the file in linux filesystem. In practice and implementation, this does not 
           affect the NachOS thread address space in any way. 
         - Open:
           Pulls the filename into memory, and delegates the management of its file storage
           vector to the AddrSpace class. The AddrSpace fileOpen method creates a new object
           and stores it in the current thread's file vector. In the implementation of Open, it
           will catch if the user opens "/dev/ttyin" and create a new link to ConsoleIn, and 
           if "/dev/ttyout" is opened, will open a new link to ConsoleOut. These are relatively
           small implementations, but important to functionality of the Open call. 
         - Close:
           Simply delegates file vector management to the AddrSpace class again, and will remove
           the OpenFile object at the given OpenFileId (which corresponds to an index in the
           the vector array). If close decrements the number of links to the object to 0, it
           will delete the OpenFile object. This is further explained in the OpenFile section.
         - Read:
           Utilizes the readWrite call from AddrSpace to get at the OpenFile object at the index
           inside the file vector given by the passed argument to read. Checks if the read object
           is Console in or Out, return value is either 1 for Console In, 0 for out, or -1 for 
           another file. This behavior also means it will return a -1 if there is no file present
           at the given OpenFileId. If we are reading from the console in, it uses the SynchConsole
           call (explained later). If the read is from a file, we obtain the offSetLock associated
           with the given openFile and read the designated number of bytes utilizing the Read() 
           OpenFile method. When this method is in use, it is designed to keep track of the pointer
           for current read automatically. 
         - Write:
           Write is implemented in an essential opposite of read. It pushes the string to be written
           in to the Kernel by implementing the ReadArg, and obtains the OpenFile object using
           readWrite- similarly to read. It will then check the object using the isConsoleFile method
           as well. If we are then writing to the Console Out, we implement the respective 
           SynchConsole method. If we are writing to a file, it implements our atomicWrite method
           lock and writes out, which assures that writing directly to a file for an expected amount
           of bytes is effectively atomic. The manner in which atomicWrite is implemented also will
           lock out simultaneous writing to the same object, and the associated movement of the
           currency pointer. 
         - Fork:
           Fork begins by locking access to its codebase to keep any other threads out. The lock
           used by fork is also implemented by Exec, which makes them mutually exclusive, and 
           effectively atomic in a sense. We completely construct the new thread's addrspace and
           register state before performing the kernel level fork. This ensures that if a context
           switch to the new thread, it will already be completely valid. The Copying process from
           the Parent's addrspace and memory is handled in our alternate addrspace constructor(
           more in depth in addrspace). We utilize a global (which feels weird) spaceId value with 
           a semaphore to lock reading/writing that is incremented for each of the forks. This in
           turn allows us to easily assign each new thread its respective spaceId to be used in Join
           and Exit. It also will store the spaceId and a ptr to its respective parent thread for
           use in the Join call. We then add a node to the global JoinList for the freshly created
           parent/child pair (a nice happy family). The current registers (regs currently in the 
           machine from the parent's execution) are prepped for the child by incrementing the
           program counter and writing a 0 to register 2. The SaveUserState() method is then called
           on the new thread to save those register values. This will also be saved in the parent's
           userState when a context switch occurs. When we reach this point, the child essentially
           ready to be run, so the parent will fork the new thread and sleep on our fork semaphore. 
           Before the parent can come back into execution, the child must complete its creation and
           release this fork semaphore. The new child thread on the fork is forced onto the
           CopyThread function where it will: wake its parent, call space -> restoreState to put
           its TLB in the machine, call RestoreUserState() to put its registers in the machine, and
           then call machine -> Run() finally to begin its execution. When the parent wakes up, it 
           will then restore its own registers and write its child's spaceId into the previously
           zero-d register 2, and will then release the fork lock that was aquired at the beginning
           of the syscall. 
         - Exec: 
           Exec will lock its codebase with the fork/exec lock, and then create a new thread. It
           fills this threads addrspace using the original constructor, after opening the executable
           passed to it as an argument. If the passed file does not exist OR is not and executable or
           a script, the constructor will fail, and the exec will fail. In ExecThread, the thread
           will then restore its page table and registers, saves state, and calls the machine -> Run
           method. Its parent, after then prepping the new thread, will release the forkExec lock and
           kill itself using the KillThread function. When this happens, it passes a special exit
           value to killThread to ensure that it will not affect the join list in any way (becuase
           the newly execed thread must exit for the join list to change).
           Exec with arguments is explained in (3). 
         - Join:
           The semantics of join are maintained by the JoinList class. This consists of: a linked
           list of JoinNode structs, which each consist of a Thread pointer called parent, a SpaceId
           called child, and integer as exitval, a semaphore called permission, and a next pointer. 
           The flow of the join is as follows: A thread forks a child, and it creates a node with a
           pointer to itself and the spaceId of the new child, which it then inserts into the list. 
           It will then store a pointer to the itself and the spaceId of the child in the child's 
           addrspace. When the parent then joins, it searches though for a node such that the parent
           pointer is equal to itself, and the childId is the argument that was passed to join. It
           then Ps on the permission semaphore. When the child makes exit, it finds the node that
           contains a pointer to its parent and its own spaceId (the same node as described before). 
           It then updates the exit value and Vs on the semaphore. The parent is then woken up and
           retrieves the exit value, then deletes the node in the list. 
         - Exit:
           Exit executes a KillThread function, which we also use for illegal exceptions and in 
           exec. In the exit function itself, it returns all of its allocated bitmap pages back to
           the bitmap, performs the join call, and then calls Finish(). This puts the thread onto the
           "to be destroyed" position, such that when the next process runs it will be deallocated.
           If no process runs after it exits, the system WILL hang indefinitely. 

       -- Our implentation of SyncConsole is based off of the supplied SyncDisk class, and
          has two main functions. In GetChar, it surrounds all code with a lock to eliminate 
          any chance of non-atomicity, and first Ps a semaphore to get at the 
          console->getChar() function. The V for this semaphore is called externally after 
          the function has completed its last console call, and fills the pointer that was 
          passed in to it. The synch in this function comes from the P, which prevents the 
          calling function from pulling more than one character from the console at a time, 
          making reading atomic. PutChar is similar, it surrounds its internal call with locks 
          to assure atomicity, then implements the console-PutChar() function to write the 
          character to the console. It then Ps on a semaphore, which assures the function 
          will only write once and then sleep until it is released externally. 

   (2) On time slicing:
       - The timer interrupts are generated by utilizing a clone of the hardware timer 
         function. In our slicingTimer, when the TimerExpired() function is called, it 
         schedules an interrupt utilizing the external interrupt->Schedule call, and sets 
         the number of ticks to interrupt at 100. We have found this to be an effective tick 
         value, as it provides varied enough results when compared to non-slicing. 

   (3) On Exec call with arguments:
       - To exec with arguments, we execute the exec call, and when it has created the new thread it
         will then pull the arguments into kernel memory using the CopyExecArgs function. The
         exec-ing thread will then fork the new thread to execThread with a pointer to the buffer
         containing the exec arguments in kernel memory. In the execThread method, the new thread
         will then restore its page table and registers like in the normal exec, but will then
         pull the args from the kernel and force them back onto its stack. As it does this, it 
         converts each byte in virtual memory to physical to maintain memory integrity. It will
         then write argc to register 4, and the stack pointer that points to the array of argument
         string points into register 5. It saves the register state(to preserve in context switch) 
         and calls the machine -> Run method. It then continues as a normal exec. 

       -- The implementation of cat and cp are straightforward. Both of the functions will
           utilize the previously implemented Open() call to attempt to open their respective
           files, and if the file does not will either create it or fail with a minimal
           error message when appropriate. They both include and utilize the supplied 
           prints() function from the test files to write messages to the ConsoleOutput. 

   (4) On Dup:
       - Dup is implemented and executed by the dupFd function residing in AddrSpace. It is given 
         a file descriptor, and then puts a pointer to the OpenFile object held in the passed file
         descriptor into the lowest open index in the file vector- which is then returned. It then
         increments the number of links to the OpenFile object. 

   (5) On the shell and #SCRIPT:
       - The shell reads initially will read a line from stdin. If this line has no arguments, we 
         exec with char* 0 for the second argument. If there are arguments in the line, we parse 
         the string to put the arguments in a char* array. This array is assumed to be under 128. 
         The program then execs the filename with the argument as a char**. If the file does not 
         exist, we print a minimal error message. When the executed file exits, the shell will continue
         to run and wait for a new command. If the line that was input to stdin begins with #, then 
         the line is ignored. If the shell detects >, then we close stdout, dup the new fd on the rhs 
         of >, and close that fd as well to redirect the shell output to file corresponding to the fd.
         For #SCRIPT, we manipulate in addrspace. noff.magic is the first line of the file, so we test
         what noff.magic (in hex) was when the first line in a file is #script, and store this value 
         as #define SCRIPT We modified the opening of files as to allow open files to remember their 
         own name. And when noff. magic is the SCRIPT value, we call a modified version of 
         userprog/protest::startprocess and pass to it test/shell (the name of the executable) 
         and the name of the script. Inside startprocess, we do fileopen (scriptname) to get the 
         openfileid, and then close(0), dup (openfileid) and close (openfileid) in order to get 
         the shell's input to become the script file. The rest of the code for start process remains 
         the same so we can run test/shell as normal. 

   (6) On AddrSpace changes:
       - We made several major changes to addrspace, they include: A failed flag; if fails to allocate
         memory or open the executable, marks failed as true and returns. Checks to see if executable is
         a script. If this is the case, it will bypass most of the normal addrspace instructions and start
         a shell process with redirected input. When filling the page table, we now use the bitmap to find
         physical pages to map to virtual pages, rather that a one to one conversion. We now put the noff 
         file into memory byte by byte, converting each virtual address to a physical one. This is used in
         exec as well as the initialization of the main thread. Creates a big file vector of size 20 and
         initializes it to NULL. 
         We also have added a new constructor that takes an addrspace pointer as an argument, and copies 
         that addrspace to the new addrspace. It copies the file vector exactly, and increments the list
         links for each file. It creates the page table as in the first constructor, with the same virtual
         pages as a parent, but different physical. This follows the semantics of the failed flag. It also
         copies physical memory from parents' physical pages to it's own physical pages, and is used in
         fork. The file vector in addrspace controls most of the instructions regarding opening and
         manipulating files through the syscalls. 

    (7) On OpenFile:
        - We had to change the design to allow for children and parents to share the same OpenFile object. 
          We added a links count, a linklock and offset lock; links count is used to determine how many
          file descriptors currently point at the OpenFile object. If/when one of these files closes, the
          links counter decrements and when it is opened, it increments. Since this can be accessed by 
          both parent and child, it is synchronized using the offset lock. This total object is only 
          deleted when a close is called and links value drops to 0. The offset lock synchronizes the
          existing currency pointer in the OpenFile object, as a parent and child can both read and
          write concurrently, which could cause issues with the currency pointer. 


  --------------------------------------------------------------------------------------------------------
  | We have been able to arrive at and agree on a mutually agreeable distribution of labor, and have all
  | put work in to this project on what we consider a fair level.
  --------------------------------------------------------------------------------------------------------
