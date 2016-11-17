-------------------------------------------
| Don Andes, Jose Acuna, Phillip Favaloro
| Nachos Project 3
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ """""""""""""""""""" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ """""""""""""""""""""""""" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ """"""""""""""""""""""" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Haiku Drafts: 
--------------

There is no god here  
Virtual memory sucks
Everything is broke 


God is dead, help us
Concurrency is a joke
Please give us an A


Our code is a mess
We did our absolute best
And we pass most tests




Usage
-----
    This project includes a makefile. In the parent directory, do a %make, this will 
    compile and prepare the full Nachos OS and included programs for execution. 
    In the >vm directory, there will now be a nachos executable.

    **All parts of the assignment have been completed and are expected to work properly.**
    
     Basic commentary/conditions to review before testing: 
     - We have tested all of our implementation functions with a num pages value >= 256.
       Most of our tests work with lower values, but this will assure you (the user) obtain 
       the same results that we intended. As per the specifications, this is the only 
       modification to the machine directory.

     - Assuming userprog/nachos is invoked from the parent, executables should be passed
       in the form test/maxfork.

     - We have implemented the Exec() call with args, meaning any call to Exec() will 
       require either arguments passed in an array of char*s, or in its place a Null Pointer. 

     - We supply our makefile for the test subdirectory, along with all of the pre-made 
       tests we have created. If any of these tests are to be run, our makefile will have to 
       be used, not the originally supplied default makefile. 

     - We have included a bash test script that runs all tests required by the 
       specifications, and we feel demonstrates effectively that all parts of the assignment
       are tested working. Check it out. 
       
     - It should be noted exec must be called from within a process. (i.e., you cannot run 
       exec from the line you run nachos- userprog/nachos -x test/argkid arg1 is invalid)

     - Any #SCRIPT file to be executed must follow lines with a newline character. 
       It just does.




Commentary
----------
    (1) On implementing virtual memory. 
        - General notes about implementation:

   


  -------------------------------------------------------------------------------------------
  | We have been able to arrive at and agree on a mutually agreeable distribution of labor, 
  | and have all put work in to this project on what we consider a fair level.
  -------------------------------------------------------------------------------------------
