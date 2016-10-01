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
            Where num can be-
                            2 : to call a demonstration of the producer consumer problem.
                            	If this option is called, the <other options> should be of the form,
                            	<x y z> where x is the number of producers desired, y is the number of consumers, and z is 
                            	the desired buffer size. (i.e: ./nachos -P 2 4 5 6 would call producer/consumer with 4 producers, 
                            	5 consumers, and a buffer size of 6.)

                            3 : to call a demonstration of a controlled elevator emulator for THall
                            4 : to call a demonstration of a directionally controlled automobile bridge for VDOT.
       
       TODO: Enumerate on what <other options> are depending on requirements of threadtest methods. 

       



