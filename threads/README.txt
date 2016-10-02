-------------------------------------------
| Don Andes, Jose Acuna, Phillip Favaloro
| Nachos Project 1
-------------------------------------------

--Compiled, edited, and modified as coursework for CSCI 444, The College of William & Mary, Fall 2016
  The formatting of this document depends on spaces that may be interpreted differently on different systems. 
  Please view in a basic text editor for best document interpretation.

--------------------------------------------------------------------------------------------------------
  --------This code is provided as is and has been created strictly for educational purposes--------
--------------------------------------------------------------------------------------------------------


       ------------------------------------------------------------------------------------------
      |                      `...```           .---.                                             |                            
      |  ohooosd-  +hssd`   /y+++/h/       -ohsoo+os+. Nooy:  sy++d-    .+s++o:       :syso+.    |                                
      |  +s    -h. +o  h.   d-    sy`    `sd/`     -h: m` /s  d: -N-  `od/`  `:s/    ym/`  .oy-  |                                
      |  +h     .h`+o  y-  :h  /  .m+    sd. -os++yy.  m: -d  d: :M-  od.       oy` -M- .ss+yy-  |                                
      |  +y  --  -ys+  h.  ho ;:/  +d:  /d- :do` `.    N+ .y++s- -N- -h: -yos+   ys .N- /m//-    |                                
      |  +s  -m+  -d/  d. `N: ::-   ds `om  sh`        mo `:---` `N- /y  y` `d.  -m  .oo-/dh-    |                                
      |  +o  -dy+  ::  d` oh  ://+. -h +-N. om`  --.   do .m//h:  m- /y  d.  m`  `N`   .sy-:ys`  |                                
      |  +y  :y ss     d  d+  s/`yh  +y -sy` +ysyo/ohy`mo  m  s+  h- .m- -s++:   /h- ss-.sd  ho  |                                
      |  +d  +o  ys   `y: m` -d` `d+  ys `oo. ```  -dy do `N  os  s-  /d.  `    :ys d--/++-  hs  |                                
      |  +m..o+   so/:o:y +--ho   -d:.:m+  :oo/:/ohd+` os/od` /y++y-   -s+:..:+s+`` oh/`   .ss`  |                                
      |  -+++/:    ---- : ::::.    /o++ss   `./++/-`   `````   ````      .-:/:-`     `:///+:.    |           
       ------------------------------------------------------------------------------------------


Usage--
      This project includes a makefile. In parent directory, do a %make , this will compile and prepare
      the full Nachos OS and included programs for execution. 
       
      In the >threads directory, there will now be a nachos executable. 
       
      Run with %./nachos -P num <other options>
         
        Where num can be:

            2 : to call a demonstration of the producer consumer problem.
                If this option is called, the <other options> should be of the form, <x y z> where x is the number of producers desired, y is the n
                number of consumers, and z is the desired buffer size. 
                (i.e: ./nachos -P 2 4 5 6 would call producer/consumer with 4 producers, 5 consumers, and a buffer size of 6.) 
                --ASSUMPTIONS-- x y z are all assumed to be greater than 0, and valid integer whole numbers.   

            3 : to call a demonstration of a controlled elevator emulator for THall.
                If this option is called, there are no <other options> to be invoked. 
                (i.e ./nachos -P 3 will call the function demo correctly.)
                --ASSUMPTIONS-- the correct amount of additional arguments are passed (none).

            4 : to call a demonstration of a directionally controlled automobile bridge for VDOT.
                If this option is called, the <other options> should be of the form, <x> where x is the number of cars desired to negotiate the bridge
                (i.e: ./nachos -P 4 8 would call the automobile bridge with 8 cars attempting to negotiate.)
                --ASSUMPTIONS-- x is assumed to be greater than 0, and a valid integer whole number.


Commentary--
      On locks and condition variables:
          We made the design choice to implement locks in the essential sense of a binary semaphore, while still careful to avoid the literal t
          translation of a semaphore. The locks are derived from the supplied Semaphore::V and Semaphore::P models, while implementing the required 
          methods Acquire, Release, and a method to check if the lockholder is the current thread. Condition variables are constructed utilizing the 
          previously defined locks and interrupt disabling as a method to increment machine time. In addition to the required methods, we added a 
          priority wait function, as well as queue-is-empty function. 

      On producer/consumer:
          The created buffer that is printed to the output is incremented by one letter from the phrase "HELLO WORLD" each print. This comes from the
          chosen producer function executing itself once per thread, which yields a total number of characters printed as the function <<strlen("HELLO 
          WORLD") * numberOfProducers == 11 * numberOfProducers>>. The output printed is relatively convincing for the correctness of our solution, 
          unless multiple producers and consumers are initialized, which will of course yield gibberish. 

      On the elevator emulator:
          The output to the terminal uses text phrases to demonstrate requesting and picking up, with long dashed lines surrounded by the appropriate 
          number for pick up and drop offs to clearly visualize the movement of the elevator. The function is capped at a preset 10 users to simplify 
          the demonstration process. The correctness for this solution, we believe, is demonstrated by three key points:        
            1) The elevator does not change direction unless there are no more users in the same direction that the
               elevator is currently moving. 
            2) All 10 users request to be picked up, request to be dropped off, and are, in fact, dropped off.
            3) The elevator doesn't excessively skip users. This only occasionally happens due to the actual built
               in unpredictability of thread timing compared with the timing of the terminal output.

      On the automobile traffic sync:
           The output to the terminal is generated as each car makes one of its 5 available checkpoints. They are:
             1) Attempting to arrive when the car pulls up at the light (where the function determines if its flow 
                of traffic has a red or green light). 
             2) Successfully arriving when receives the "green light" from the program. 
             3) Crossing the bridge (car is currently on bridge)
             4) Beginning to exit (car is still on bridge in the process of exiting)
             5) Sucessfully exiting (car has exited and the bridge/traffic light know the bridge is empty)
           These allow the visualization of the bridge to be represented fairly well with plain text. 
           On itialization, the bridge originally sets traffic flow to the direction the first car is traveling (to remain fair). To improve 
           readability and user interaction, the value 0 for travel is mapped to NORTH, and 1 for SOUTH respectively. 
           Since the specifications to not restrict us to fairness, we made the design choice to prioritize NOT running straight into incoming traffic.
           This is accomplished by keeping track of the current flow of traffic, and directly comparing to any car attempting to arrive. 




