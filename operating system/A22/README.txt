I am not familiar with Makefile and I just follow the pattern of the Makefile from A1 solution.
If there is any error in the makefile, please see the instructions below:

Compile in the following way:
gcc -c shell.c interpreter.c shellmemory.c kernel.c ram.c pcb.c cpu.c 
gcc -o mykernel shell.o interpreter.o shellmemory.o kernel.o ram.o pcb.o cpu.o
./mykernel

If it is for 2.7 Testing your kernel, you should exit the shell first by
 $ quit (if you are in the shell), then:
./mykernel < testfile.txt