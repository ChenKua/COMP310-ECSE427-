Extra feature: when use command $ run script.txt or ./mysh < testfile.txt, 
it will print commonds first in the terminal.
This feature proves the correctness of my shell and it is very helpful for testing.

For example, script.txt contains:
help
set var 199
print var

output in terminal:
$ help
supported commands...
$ set var 199
$ print var
199


Compile in the following way:
gcc -c shell.c interpreter.c shellmemory.c
gcc -o mysh shell.o interpreter.o shellmemory.o
./mysh
then type in commands: 
$ run script.txt
$ help
etc... 

If it is for 2.6 Testing your shell, you should exit the shell first by
 $ quit (if you are in the shell), then:
 ./mysh < testfile.txt