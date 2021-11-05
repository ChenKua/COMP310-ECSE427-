Use make to compile: make mykernel
Test: ./mykernel < testfile.txt
To clean, do:
rm *.o; rm mykernel

Alternative compile:
gcc -c shell.c interpreter.c shellmemory.c kernel.c cpu.c pcb.c ram.c memorymanager.c DISK_driver.c
gcc -o mykernel shell.o interpreter.o shellmemory.o kernel.o cpu.o pcb.o ram.o memorymanager.o DISK_driver.o
./mykernel

- Built on top of the official A3 solution.

- Partition file is stored as binary file. Opening it with text editor will have same stange character.

- Initialization: File name for entries in the fat[20] is initlizaed with "_NONE_".
    Therefore, you cannot use name "_NONE_" for write or read commands. e.g. Cannot use: write _NONE_ [1234]

- When open a new file for fat[20], the strategy is: allocate 10 contiguous blocks immediately to the file.
    e.g. For fat[0], it will be allocated with first 10 contiguous blocks in the data region from #0 to #9.
         For fat[1], it will be allocated with 10 contigous blocks in the data region from #10 to #19.
    If partition only have 10 total blocks, you can only have 1 file for fat[20].
    If partition have 20 totoal blocks, you can have 2 files for fat[20].
    This strategy is supported by the Professor Richard Olaniyan discussed in the Piazza @442

- write file [a collection of words] command will remove the space between words.
    Must use open square brackets: "[]" to indiate the words you want to write
    This strategy is supported by the Professor Joseph Vybihal discussed in the Piazza @441
    Consecutive WriteBlock() will not overwrite the data, it will write in the end.
    If you want to overwrite the previous data, please use Mount() to set current_location to 0;

- If you want to read() after write(), you should use mount() again to reset the current_location back to 0.
    This strategy is discussed in Piazza @488 @460

- readBlock() will read the entire block, including 0 if there are any.

- Active file table is cleared when calling mount(), current_location is reset to 0 when mount().




