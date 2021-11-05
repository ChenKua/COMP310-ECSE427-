//    Final Project COMP 310
//    Your Name: Kua Chen
//    Your McGill ID: 260856888

Solution For Problem 2.

To compile the project, run:
./recompile.sh

To test the project, run:
./mykernel < TESTFILE_PROB4.txt 

This solution is built on the codebase and reuses existent functions / code blocks a lot.

Make void fseekToBlock(FILE *file, int blockID) from DISK_driver public
Make void updateFATOnDisk() from DISK_driver public

Add a new variable in struct FAT, call offset to store the offset within a block so that the read/write commands know 
how many bytes they need to read/write to finish this block.

Modified printCommand because it usess isalnum() function. Problem is: if there is space in the string, this function
will return 0. Specifically, isalnum("  test") will return 0, and this "0" will make the OS think the return string is empty.
So there is nothing to print. 
However, this is a mistake. Obvisouly, "   test" does exist. 