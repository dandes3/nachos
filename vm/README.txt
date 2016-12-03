-------------------------------------------
| Don Andes, Jose Acuna, Philip Favaloro
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ God is dead, help us ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Concurrency is a joke ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Please give us an A ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Usage
-----
    This project includes a makefile. In the parent directory, do a %make, this will 
    compile and prepare the full Nachos OS and included programs for execution. 
    In the >vm directory, there will now be a nachos executable.

    ** All parts of the assignment have been completed and are expected to work properly,
        with the exception of handling open files during a checkpoint. ** 
    
     Basic commentary/conditions to review before testing: 

     - All tests for Copy On Write (COW) should be executed on the COW version of our
       submitted kernel. 

     - All tests should be executed from inside the directory where the tests live. 

     - All tests should be run standalone, not inside our implementation of the SHELL. 
     
     - We do not want anything from NachOS 2 re-graded. 


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
 
     On VM:

        -- During the initiation of an addrspace, when constructing the page table we mark
            every page's valid bit as false, and get an open page from the disk map to put
            into the corrosponding index in the diskSectors array. If there is not enough
            space on the disk to hold our process, the creation of the addrspace will fail. 
            To ensure that the data ends up on the correct disk sector, during an EXEC call
            we emulate copying the data byte by byte to memory (as we did in nachOS 2) by
            making a 2d array, with the first dimension as the number of pages that the
            executable, and the second as 128 (size of a page). This emulates what the
            process would appear as in a virtual memory space. We then write each of the
            128 byte arrays to the corrosponding disk sector from the diskSectors array. 

        -- During a fork, we perform the same operation with the page table. We then
            look at the parent's address space, and for each page in the address space
            and if the page is in memory, we set the lock bit to true, and copy the page
            from memory into a local buffer. This buffer is then writter to disk utilizing
            the sector found from diskSectors. If the page is not in memory however, we
            read into the buffer, then write it into the child's sector (from the child's
            diskSector array). 

        -- Our entire implementation of a pageFault protocol is locked with FaultLock. 
            This is the case for both externel and internel machine page faults, as they 
            are executed and handled by the same function, in the same manner.  

        -- During a page fault, we delegate finding a page to remove from memory to a 
            function called pageToRemove (which utilizes our LRU clock algorithm). If
            this function returns a -1, this means there already exists space in memory, 
            and a page will not have to be evicted. If this is the case, we find the new 
            location to place the faulted page with the memory bitmap. 
            In the case that the function does not return a -1, the number returned will 
            be the page we evict and place the new page in. 

        -- During eviction, we go to the faultInfo array, at the location returned 
            by pageToRemove. From the faultData struct we get the thread to be evicted, 
            and which virtual page will be evicted for it. We then transform 
            the valid bit to false, and find a new sector from the diskMap. We update the
            evicted threads disk sectors array at the page we are removing with the sector
            we have just found, and then write the page from memory into that sector. 

        -- We now make a new faultData struct, with the owner set to currentThread. 
            The virtual page is set to the page we faulted on, and the locked bit set to 
            true on internal, false on external. This new faultData struct is now put into
            the faultInfo array at the index corrosponding to the page being replaced. 
            We then retrieve the sector where the faulting page lives from diskSectors for
            the current thread, and read the sector into an internel kernel buffer. We
            then clear the sector on the diskMap, and write it in to main memory byte by
            byte. We then update the page table for the faulting thread, at the faulting
            page by marking the valid bit true, the use bit true (used in replacement alg)
            and by setting the physical page to the page just filled in memory. 
            
        -- Any time we read or write main memory at a given virtual address, we do so byte
            by byte, converting to physical address after every byte. To ensure the page
            we are manipulating is actually in memory, we start by calling the 
            bringIntoMemory function at the initial location. Then for every byte, we
            check to see if we have changed pages from the initial location's page, and
            if so, call bringIntoMemory again with the new location's page. This 
            function faults in a page if it is not in memory. Once it is memory, or if 
            it started in memory, it will set the locked bit in faultInfo at the
            corrosponding physical page to true. The bit will then be switched to false
            once we are finished with that page.

        -- Anytime a thread finishes, for each of its pages, if that page is in memory
            it clears its space from the memory bitmap and deletes it's entry from
            faultInfo. If it is not in memory, it will clear the sector from the diskMap. 
            
        -- Our replacement algorithm is contained within the pageToRemove function as
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

        -- On the checkpoint syscall, we start by creating an openFile object for the
            checkpoint file. We then write the cookie "FUCKNACHOS\n" to indicate that
            it is a checkpoint file. We then write the number of memory pages that the
            process has, and all of it's memory contents, seperated by ":"s, either
            from disk or main memory. We then write the content of each register, also
            seperated by ":"s. Each section is seperated by a newline character. 
            
        -- On a call to EXEC the checkpoint, we detect that a checkpoint is being EXECed
            in the addrspace creation by checking the cookie. The page table creation is
            the same, but instead of copying from the executable onto disk, we copy from
            the checkpoint file. After the addrspace creation, we set a flag inside the
            addrspace to indicate the process has been created from a checkpoint. This
            allows us to easily check and perform different processes when we come back to
            this point in our exception.cc. Inside exception.cc, we now skip adding exec
            args, and instead of initializing registers to an initial value, pull register
            values from the file. The implementation of Exit() and Join() is the same in
            checkpoint as it is in any other normal Exec. 


     On COW: 

        -- Owners now refers to either a process that owns a page that isn't read only,
            or a parent/child pair that own a page that is read-only. 
            
            Note: we originally planned to make COW function for up to 5 children, 
            which is the reason the faultData has up to 5 owners- however, we were 
            unable to make it function. Therefore, when we mention all owners, we mean 
            just the parent and child. This could have been generalized to multiple 
            parents and children had we be been able to make it function as we had 
            originally intended.  
            
        -- On a fork, in the creation of an addrspace, we copy the parent's page table
            exactly, with the exception of setting both the child and parent's read only
            bits for each page to true. We do not copy the memory as we did in a normal, 
            non-COW environment. In both the parent and child's addrspaces, we also
            give them references to eachother's thread pointers. 
            
        -- Like in VM, the page faulting procedure is mutually exclusive. That is, it 
            is locked so only one page fault can be happening at one given time. However, 
            since a page can be owned by multiple processes, while a thread is waiting on
            the lock to page fault it's page table state may change. To combat this, we
            check again after aquiring the lock to ensure the valid bit for the faulting
            page is still false. 

        -- During a page eviction, we now mark the page table false for each owner of
            the page. We also update the diskSectors array for each owner with the same
            new sector. This preserves the "read only-ness" of the page, even after being
            evicted from memory.
            
            This means that a page being faulted in could have many owners. This is 
            indicated by the page having a read only bit set to true in the page table
            (even though it's valid bit is set to false.) If the page being faulted in
            is read-only, the current thread adds itself to the owners of the page in the
            faultInfo array, as well as it's parent or child. It will know it's parent
            or child from the pointers set during a fork. Otherwise, it will only add
            itself as an owner. 

        -- The page is written from disk to memory as before. However, the changes
            previously made to the owner's page table will now be made for each owner. 
            (For each owner, at the faulting page in their page table, the valid bit
            will be set to true, the physical page will be set to the new physical page, 
            and the use bit will be set to true. The read-only bit will be unaffected.)
            
        -- The procedure for handling a read-only exception is locked with readOnlyLock. 
            Similarly to in a page fault, we check to see if the page is still read only
            once the process gets in to the function off the lock. 
            At the time that a read-only exception occurs, the page must be already 
            in memory. The process that is trying to write to the read-only page will 
            keep posession of the page in memory, and all other owners will get a copy 
            on disk.
            
        -- To make the writing process the sole owner of the page, all other owners are
            removed from the faultData struct corrosponding to the page, and its read-only
            bit for that page is set to false. Then, for each other owner, a new sector
            is found from the diskMap, the page is copied to that sector, its diskSectors
            array is updated, and both the valid and read-only bits are set to false.     
            
        -- When a thread is finished, we release the pages from the memMap and diskMap
            as before. However, now that faultData structs have multiple owners, the 
            procedure for cleaning up faultInfo changes. For each faultData struct in
            faultInfo if the page were the only owner on the faultData struct, it 
            removes the faultData struct from faultInfo. If it is one of two owners, it
            removes itself as an owner, then updates the other owner's page table for that
            page as not read-only. This effectively gives the other owner full control
            over the given page. Finally, for the dying thread's family (parent and child), 
            any reference (previously set in a fork) to that thread is set to NULL.
            
        -- Anytime we are manipulating the owners in a faultData struct, we use the
            killLock. This occurs when we are killing a thread, updating faultInfo during
            a fork, updating owners page tables (during a read-only exception), evicting
            a page, and creating a new faultData (with multiple owners) during a page
            fault. 

            Note: this lock never is held during disk I/O. The only lock ever held
            during disk I/O is the faultLock.  

        -- The only change to the replacement algorithm is that when setting an owner's 
            use bit to false, we now do the same for every owner. And when checking if 
            an owner's use bit is true, we check if any owner's use bit is true.  


  -------------------------------------------------------------------------------------------
  | We have been able to arrive at and agree on a mutually agreeable distribution of labor, 
  | and have all put work in to this project on what we consider a fair level.
  -------------------------------------------------------------------------------------------
