-------------------------------------------
| Don Andes, Jose Acuna, Phillip Favaloro
| Nachos Project 1
-------------------------------------------

Compiled, edited, and modified as coursework for CSCI 444, The College of William & Mary, Fall 2016.
This code is provided AS IS and had been created strictly for educational purposes. 


Usage--
      This project includes a makefile. In parent directory, do a %make , this will compile and prepare
      the full NachOS OS and included programs for execution. 
       
      In the >threads directory, there will now be a nachos executable. 
       
      Run with %./nachos -P num <other options>
         
        Where num can be:

            2 : to call a demonstration of the producer consumer problem.
                If this option is called, the <other options> should be of the form, <x y z> where x is the number of producers desired, y is the number of consumers, and z is the desired buffer size. 
                (i.e: ./nachos -P 2 4 5 6 would call producer/consumer with 4 producers, 5 consumers, and a buffer size of 6.) 
                --ASSUMPTIONS-- x y z are all assumed to be greater than 0, and valid integer whole numbers.   

            3 : to call a demonstration of a controlled elevator emulator for THall.
                If this option is called, there are no <other options> to be invoked. 
                (i.e ./nachos -P 3 will call the function demo correctly.)
                --ASSUMPTIONS-- the correct amount of arguments are passed (none).

            4 : to call a demonstration of a directionally controlled automobile bridge for VDOT.
                If this option is called, the <other options> should be of the form, <x> where x is the number of cars desired to negotiate the bridge. (i.e: ./nachos -P 4 8 would call the automobile bridge with 8 cars attempting to negotiate.)
                --ASSUMPTIONS-- x is assumed to be greater than 0, and a valid integer whole number.


Commentary--

      On locks and condition variables:
          We made the design choice to implement locks in the essential sense of a binary semaphore, while still careful to avoid the literal translation of a semaphore. The locks are derived from the supplied Semaphore::V and Semaphore::P models, while implementing the required methods Acquire, Release, and a method to check if the lockholder is the current thread. Condition variables are constructed utilizing the previously defined locks and interrupt disabling as a method to increment machine time. In addition to the required methods, we added a priority wait function, as well as queue-is-empty function. 

      On producer/consumer:
          The created buffer that is printed to the output is incremented by one letter from the phrase "HELLO WORLD" each print. This comes from the chosen producer function executing itself once per thread, which yields a total number of characters printed as the function <<strlen("HELLO WORLD") * numberOfProducers == 11 * numberOfProducers>>. The output printed is relatively convincing for the correctness of our solution, unless multiple producers and consumers are initialized, which will of course yield gibberish. 

      On 



