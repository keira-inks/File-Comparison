Keira Inks 811-713-987

    To compile: make
    To run: ./proj3.out [.txt file] [.txt file]; [optional bash cmds separated by ;]

    The times for step 1 and step 2 are different because step 1 compares the bytes in the text files one by one and repeatedly makes system calls, while in step 2 after the few system calls are made, we simply use the bytes contained in the dynamically allocated memory and compare those. Comparing byte-by-byte using dynamically allocated memory is much faster than comparing byte-by-byte using system calls.
