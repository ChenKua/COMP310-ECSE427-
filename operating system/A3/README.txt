This assignment is built on top of the Ass2 Official Solution.

Compile program by: make mykernel
Then test it by: ./mykernel < testfile.txt

// Alternative way
gcc -c shell.c interpreter.c shellmemory.c kernel.c cpu.c pcb.c ram.c memorymanager.c
gcc -o mykernel shell.o interpreter.o shellmemory.o kernel.o cpu.o pcb.o ram.o memorymanager.o
./mykernel

BackingStore file naming policy:
File is named as "program" + "Number" + ".txt", where "Number" is the order of the file stored.
e.g. exec scrip1.txt        scrip1.txt     scrip1.txt 
            |                   |               |
            |                   |               |
        program0.txt        program1.txt    prorgam2.txt
There will be 3 file stored in BackingStore: program0.txt, program1.txt, prorgam2.txt. 

More than 1 "exec" commands will follow the naming policy as well.
exec scrip1.txt  scrip2.txt  scrip3.txt
exec scrip2.txt  scrip3.txt
(5 file in BackingStore: program0.txt, program1.txt, prorgam2.txt, prorgam3.txt, prorgam4.txt)
(Coresponding to         scrip1.txt    scrip2.txt    scrip3.txt    scrip2.txt    scrip3.txt)  

Note:
1. If you are using VS Code, please click "Refresh" to update the BackingStore folder, when there is a file stored into it. 
2. When FindVictim(), the OS will not select frame which is being used by other PCB.
3. Files in the BackingStore will not be deleted after exec command.
