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
     - We have tested all of our implementation functions with a num pages value >= 128. 
       As per the specifications, this is the only modification in the machine directory.

     - When executing shell functions it should be noted that while the shell operates in 
       the userprog directory, the rest of the tests reside in test. (i.e. implement tests 
       in the shell as test/cat, test/maxfork, etc.)

     - We have implemented the Exec() call with args, meaning any call to Exec() will 
       require either an intended argument passed or in its place a Null Pointer. 

     - We supply our makefile for the test subdirectory, along with all of the pre-made tests
       we have created. If any of these tests are to be run, our makefile will have to be 
       used, not the originally supplied default makefile. 

     - We have included a bash test script that runs all tests required by the specifications,
       and we feel demonstrates effectively that all parts of the assignment are tested working. 
       Check it out. 

     - TODO: other commentary/pre-reqs




Commentary
----------
   (1) On syscalls and exception handling:
       - General notes about exception.cc: 
         We use the ConvertAddr function to constantly and consistantly convert accepted virtual
         addresses to physical addresses. We pull arguments from user memory with ReadArg into 
         the kernel, and then call ConvertAddr to translate the Virtual Address that is passed 
         as an argument. The implemented IncrementPc() function pushes the program counter
         forward at the end of every syscall, except those end abnormally (such as exec and
         exit). We made the design decision to handle as much of the syscall related actions
         inside the AddrSpace class as we logically could. This was, we felt, the most 
         straightforward locatino for these actions to be handled, and allowed us to increase
         orginization and cleaniliness of our codebase.

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

   (2) On multiprogramming with time slicing:
       - a) Frames
         b) VA in/out
         c) The timer interrupts are generated by utilizing a clone of the hardware timer 
            function. In our slicingTimer, when the TimerExpired() function is called, it 
            schedules an interrupt utilizing the external interrupt->Schedule call, and sets 
            the number of ticks to interrupt at 100. We have found this to be an effective tick 
            value, as it provides varied enough results when compared to non-slicing. 

   (3) On Exec call with arguments:
       - Defense here
       -- The implementation of cat and cp are straightforward. Both of the functions will
           utilize the previously implemented Open() call to attempt to open their respective
           files, and if the file does not will either create it or fail with a minimal
           error message when appropriate. They both include and utilize the supplied 
           prints() function from the test files to write messages to the ConsoleOutput. 

   (4) On child process file descriptor inheritance:
       - Defense here

   (5) On #SCRIPT execution:
       - Defense here

   (6) On effectively atomic writes:
       - Defense here
  
