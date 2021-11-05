//    Final Project COMP 310
//    Your Name: Kua Chen
//    Your McGill ID: 260856888

Solution For Problem 2.

To compile the project, run:
./recompile.sh

To test thid project, run:
./mykernel < TESTFILE_PROB3.txt

This solution is built on the codebase and reuses existent functions / code blocks a lot.

Make void fseekToBlock(FILE *file, int blockID) in DISK_driver to be public.
Make void updateFATOnDisk() in DISK_driver to be public.

Add a new variable in struct FAT, call offset to store the offset within a block so that the read/write commands know 
how many bytes they need to read/write to finish this block.

Notice: for testing purpose, you should remove partition3.txt if there is already one. 