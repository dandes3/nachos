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
     - All tests for Copy On Write (COW) should be executed on the COW version of our
       submitted kernel. 

     - All tests should be executed from inside the directory where the tests live. 

     - All tests should be run standalone, not inside our implementation of the SHELL. 
     




Data Structures
---------------
    VM:
       (1) faultInfo is a global array of structs, of size numPhysicalPages. The structs
           inside are of type faultData. These structs have a Thread* owner, int virtualPage
           and bool locked value. Any access to faultInfo OR any page table is locked by
           the vmInfoLock. 
 
       (2) megaDisk is a global synchDisk that handles all of our disk I/O. It has a
           corrosponding bitmap to keep track of all open sectors. Access to this bitmap
           is locked by the diskBitLock. 
 
       (3) In each addrspace, there is a diskSectors array of size 2048. This is the size
           of the number of pages that the process can hold at one time. This maps virtual
           pages to their location on the physical disk. When the valid bit is set to 
           false this mapping will be valid, if it is true then it may not be a direct
           map as it not on the disk.
   
       (4) We use a memory bitmap to keep track of all open pages in memory. Access is
           locked with memLock. 


    COW:
       (1) Inside the faultInfo array in COW, the structs have an array of Thread* owners, 
           of size 5, called owners. The 5 is a hard limit on the number of owners. There
           is also an int curOwner, which marks the next open spot in the Thread* owner
           array. 

       (2) megaDisk is unchanged. 
 
       (3) Access to any thread's diskSectors array is locked by a diskSectorsLock.

       (4) Memory bitmap is unchanged. 


Implementation
-------------- 
 
     On VM: During the initiation of an addrspace, when constructing the page table we mark
            every page's valid bit as false, and get an open page from the disk map and put
            into the corrosponding index in the diskSectors array. If there is not enough
            space on the disk to hold our process, the creation of the addrspace will fail. 
            To ensure that the data ends up on the correct disk sector, during an EXEC call
            we emulate copying the data byte by byte to memory (as we did in nachOS 2) by
            making a 2d array, with the first dimension as the number of pages that the
            executable, and the second as 128 (size of a page). This emulates what the
            process would appear as in a virtual memory space. We then write each of the
            128 byte arrays to the corrosponding disk sector from the diskSectors array. 
            During a fork, we do perform the same operation with the page table. We then
            look at the parent's address space, and for each page in the address space
            and if the page is in memory, we set the lock bit to true, and copy the page
            from memory into a local buffer. This buffer is then writter to disk utilizing
            the sector found from diskSectors. If the page is not in memory however, we
            read into the buffer, then write it into the childs sector (from the child's
	    diskSector array). 
            Our entire implementation of a pageFault protocol is locked with FaultLock. 
            This is the case for both externel and internel page faults, as they are
            executed by the same function. 
            During a page fault, we delegate finding a page to remove from memory to a 
            function called pageToRemove (which utilizes our LRU clock algorithm).If
            this function returns a -1, this that there already exists space in memory, 
            and a page will not have to be evicted. If this is the case, we find the new 
	    location to place the faulted page with the memory bitmap. In the case that
            the function does not return a -1, the number returned will be the page we
            evict and place the new page in. During eviction, we go to the faultInfo array, 
            at the location returned by pageToRemove, from the faultData struct we get the
            thread to be evicted, and which virtual page will be evicted for it. We then mark
            the valid bit to false, and find a new sector from the diskMap. We update the
            evicted threads disk sectors array at the page we are removing with the sector
            we have just found. We then write the page from memory into that sector. 
            We now make a new faultData struct, with the owner set to currentThread,
            virutal page is set to the page we faulted on, and the locked bit set to 
            true on internal, false on external. This new faultData struct is now put into
            the faultInfo array at the index corrosponding to the page being replaced. 
            We then retrieve the sector where the faulting page lives from diskSectors for
            the current thread, and read the sector into an internel kernel buffer. We
            then clear the sector on the diskMap, and write it in to main memory byte by
            byte. We then update the page table for the faulting thread, at the faulting
            page by marking the valid bit true, the use bit true(used in replacement alg)
            and by setting the physical page to the page just filled in memory. 
            Any time we read or write main memory at a given virtual address, do so byte
            by byte, converting to physical address after every byte. To ensure the page
            we are manipulating is actually in memory, we start by calling the 
            bringIntoMemory function at the initial location. Then for every byte, we
            check to see if we have changed pages from the initial location's page, and
            if so, call bringIntoMemory again with the new location's page. This 
            function faults in a page if it is not in memory. Once it is memory, or if 
            it started in memory, it will set the locked bit in faultInfo at the
            corrosponding physical page to true. The bit will then be switched to false
            once we are finished with that page. 
            Anytime a thread finishes, for each of its pages, if that page is in memory
            it clears its space from the memory bitmap and deletes it's entry from
            faultInfo. If it is not in memory, it will clear the sector from the diskMap. 
            
            Our replacement algorithm is contained within the pageToRemove function as
            mentioned above. We keep a global variable called clockPos, that mimics the
            commutator in the LRU clock algorithm. It's position is initialized to the 
            middle memory page (somewhat arbitrarily). If there is no space in memory
            on a fault, it iterates through the faultInfo array, and checks if the
            use bit is true for the owner's virtual page. If it is true, it sets it to
            false. If it is false and the page's faultData struct's lock bit false, 
            the page is chosen to be removed. The clockPos is then incremented, and the
            algorithm will continue if a victim has not been chosen, or terminate
            otherwise. 

     On Checkpoint:
            


     On COW:   



  -------------------------------------------------------------------------------------------
  | We have been able to arrive at and agree on a mutually agreeable distribution of labor, 
  | and have all put work in to this project on what we consider a fair level.
  -------------------------------------------------------------------------------------------
