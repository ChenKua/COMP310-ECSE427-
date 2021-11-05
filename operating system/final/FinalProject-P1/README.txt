//    Final Project COMP 310
//    Your Name: Kua Chen
//    Your McGill ID: 260856888

Solution For Problem 1.

To compile the project, run:
./recompile.sh

This solution is built on the codebase and reuses existent functions / code blocks a lot.
In order to execute read command immediately after write command, 
    read_EXAM command will reset current_location = 0 and then read the file data from the first block.

open_EXAM command will set current_location to 0.

Notice: for testing purpose, you should remove partition1.txt if there is one. 
From the codebase, writeBlock() is Appending by default. 
It means, there will be two [hello world test] in the partition1 after you write for the second time.
