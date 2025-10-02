#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Compares one byte at a time and writes the bytes from file1 that is different
 * from those in file2 into another file. Also calculates the amount of time the
 * step takes.
 */
void step1 (int argc, char * argv[]);
/*
 * Compares the bytes in file2 to those in file1 and writes the differing bytes
 * into another file. This step is performed using dynamically allocated arrays.
 * Also calculates the amount of time the step takes.
 */
void step2 (int argc, char * argv[]);

// Handles exceptions for when the seek() system call fails.
void handleSeekExceptions (int fd);

// Handles exceptions for when the write() system call fails.
void handleFDWExceptions (int fdw);

// Handles exceptions for when there is an incorrect number of arguments.
void handleArgExceptions(int argc);
// Handles exceptions for when the read() system call fails.
void handleFDRExceptions (int fdr);

/*
 * Program that uses system calls and dynamically allocated memory to
 * compare the contents of two input files, writing the differences
 * into a third file. Also displays the amount of time each step takes.
 */
int main (int argc, char * argv[]) {

    handleArgExceptions (argc);
    step1 (argc, argv);
    step2 (argc, argv);

    return 0;

} // main

void step2 (int argc, char * argv[]) {
    // To determine the amount of time this step takes
    double step2Time;
    struct timeval startTime;
    struct timeval endTime;
    gettimeofday (&startTime, NULL);

    // Variables to hold file descriptors
    int fdrDifferences = -1;
    int fdFile1 = -1;
    int fdFile2 = -1;

    // Offset (size) from using SEEK_END
    int file1Size = -1;
    int file2Size = -1;

    char * file1 = NULL;
    char * file2 = NULL;

    // Opening files
    fdrDifferences = open ("differencesFoundInFile2.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    handleFDRExceptions (fdrDifferences);

    fdFile1 = open (argv[1], O_RDONLY);
    handleFDRExceptions (fdFile1);
    file1Size = lseek (fdFile1, 0, SEEK_END); // Determining size of file (changes offset)
    handleSeekExceptions (file1Size);
    file1 = (char *) malloc (sizeof (char) * file1Size); // Array that will hold the contents of file 1
    // Resetting offset to beginning of file
    int offset1;
    offset1 = lseek (fdFile1, 0, SEEK_SET);
    //    int currentOffset = lseek(fdFile1, 0, SEEK_CUR); // --------------------------------
    int fdrFile1;
    handleSeekExceptions (offset1);
    fdrFile1 = read (fdFile1, file1, file1Size);

    handleFDRExceptions (fdrFile1);
    file1[file1Size - 1] = '\n';

    fdFile2 = open (argv[2], O_RDONLY);
    handleFDRExceptions (fdFile2);
    file2Size = lseek (fdFile2, 0, SEEK_END); // Determining size of file
    handleSeekExceptions (file2Size);
    file2 = (char *) malloc (sizeof (char) * file2Size); // Array that will hold the contents of file 2
    // Resetting offset to beginning of file
    int offset2;
    offset2 = lseek (fdFile2, 0, SEEK_SET);
    int fdrFile2;
    handleSeekExceptions (offset2);
    fdrFile2 = read (fdFile2, file2, file2Size);
    handleFDRExceptions (fdrFile2);
    //    file2[file2Size - 1] = '\n';

    // Comparing differences
    if (file2Size < file1Size) { // If file2 is smaller than file1
        int count = 0; // To keep track where in the allocated memory we are
        char * differencesInFile2 = (char *) malloc (sizeof (char) * (file2Size));
        // Storing the differences
        for (int i = 0; i < file2Size; i++) {
            if (file2[i] != file1[i]) { // They don't equal each other AND end of file2 has not been reached yet
                differencesInFile2[count] = file2[i];
                count++;

            } // if

        } // for
        // Need to transfer differences in differences array to differences file
        int fdw = write (fdrDifferences, differencesInFile2, count);
        handleFDWExceptions (fdw);
        free (differencesInFile2);


    } else if (file2Size > file1Size) { // If file2 is bigger than file 1
        int count = 0;
        char * differencesInFile2 = (char *) malloc (sizeof (char) * (file2Size));
        // Storing the differences
        for (int i = 0; i < file1Size; i++) {
            if (file2[i] != file1[i]) { // They don't equal each other AND end of file1 has not been reached yet
                differencesInFile2[count] = file2[i];
                count++;
            } // if
        } // for
        // Reached the end of file1, need to add the rest of file2 to the differences array
        for (int i = file1Size ; i < file2Size; i++) {
            differencesInFile2[i] = file2[i];
        } // for

        // Write data contained in differences allocated memory into the correct file
        int fdw = write (fdrDifferences, differencesInFile2, count + (file2Size - file1Size));
        handleFDWExceptions (fdw);
        free (differencesInFile2);

    } else if (file2Size == file1Size) { // If both files are equal
        int count = 0;
        char * differencesInFile2 = (char *) malloc (sizeof (char) * (file2Size));
        // Storing the differences
        for (int i = 0; i < file2Size; i++) {
            if (file2[i] != file1[i]) {
                differencesInFile2[count] = file2[i];
                count++;
            } // if
        } //for

        // Write data contained in differences allocated memory into the correct file
        int fdw = write (fdrDifferences, differencesInFile2, count + (file2Size - file1Size));
        handleFDWExceptions (fdw);
        free (differencesInFile2);

    } // if


    // Closing files and freeing allocated memory
    close (fdrDifferences);
    close (fdFile1);
    close (fdFile2);

    free (file1);
    file1 = NULL;
    free (file2);
    file2 = NULL;

    // To determine the amount of time this step takes
    gettimeofday (&endTime, NULL);
    step2Time = ((endTime.tv_sec - startTime.tv_sec) * 1000.0 + (endTime.tv_usec - startTime.tv_usec) / 1000.0);
    printf ("Step 2 took %.6f milliseconds\n", step2Time);


} // step2


// Puts the stuff found in file1 that's not equal to the corresponding stuff in file2
void step1 (int argc, char * argv[]) {

    // To determine the amount of time this step takes
    double step1Time;
    struct timeval startTime;
    struct timeval endTime;
    gettimeofday (&startTime, NULL);

    // Will be accessing files ONE BYTE AT A TIME - buffer of TWO
    int fdr1 = -1;
    int fdrFile1 = -1;
    int fdrFile2 = -1;

    // Offset (size) from using SEEK_END
    int file1Size = -1;
    int file2Size = -1;

    // Opening files
    fdr1 = open ("differencesFoundInFile1.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    handleFDRExceptions (fdr1);

    fdrFile1 = open (argv[1], O_RDONLY);
    handleFDRExceptions (fdrFile1);
    fdrFile2 = open (argv[2], O_RDONLY);
    handleFDRExceptions (fdrFile2);

    // Determining file sizes (changes the offset)
    file1Size = lseek (fdrFile1, 0, SEEK_END); // Determining size of file
    handleSeekExceptions (file1Size);
    file2Size = lseek (fdrFile2, 0, SEEK_END); // Determining size of file
    handleSeekExceptions (file2Size);

    // Resetting the offset to the beginning of both files
    int offset1 = lseek (fdrFile1, 0, SEEK_SET);
    handleSeekExceptions (offset1);
    int offset2 = lseek (fdrFile2, 0, SEEK_SET);
    handleSeekExceptions (offset2
                          );
    // Buffers
    int bufferSize = 2;
    int fdw;
    // Allocated memory for file 1 comparing char
    char * bufferFile1 = (char *) malloc (sizeof (char) * bufferSize);
    bufferFile1[bufferSize - 1] = '\0';
    // Allocated memory for file 2 comparing char
    char * bufferFile2 = (char *) malloc (sizeof (char) * bufferSize);
    bufferFile2[bufferSize - 1] = '\0';

    int bytesReadFile1 = read (fdrFile1, bufferFile1, bufferSize - 1);
    handleFDRExceptions (bytesReadFile1);
    int bytesReadFile2 = read (fdrFile2, bufferFile2, bufferSize - 1);
    handleFDRExceptions (bytesReadFile2);

    // Comparing the first file with the second file
    if (file1Size > file2Size) { // If file1 is bigger than file2
        while (bytesReadFile2 != 0) { // As long as we're not at the end of the FIRST file

            if (bufferFile1[0] != bufferFile2[0]) { // Actual comparison occurs here
                fdw = write (fdr1, bufferFile1, 1);
                handleFDWExceptions (fdw);
            } // if

            bytesReadFile1 = read (fdrFile1, bufferFile1, bufferSize - 1);
            handleFDRExceptions (bytesReadFile1);
            bytesReadFile2 = read (fdrFile2, bufferFile2, bufferSize - 1);
            handleFDRExceptions (bytesReadFile2);

        } // while
        // Putting the rest of file1 into the differences file
        while (bytesReadFile1 != 0) {
            fdw = write (fdr1, bufferFile1, 1);
            handleFDWExceptions (fdw);
            bytesReadFile1 = read (fdrFile1, bufferFile1, bufferSize - 1);
            handleFDRExceptions (bytesReadFile1);

        } // while

    } else if (file1Size < file2Size) { // If file1 is smaller than file2
        while (bytesReadFile1 != 0) {
            if (bufferFile1[0] != bufferFile2[0]) {
                fdw = write (fdr1, bufferFile1, 1);
                handleFDWExceptions (fdw);
            } // if
            bytesReadFile1 = read (fdrFile1, bufferFile1, bufferSize - 1);
            handleFDRExceptions (bytesReadFile1);
            bytesReadFile2 = read (fdrFile2, bufferFile2, bufferSize - 1);
            handleFDRExceptions (bytesReadFile2);

        } // while

    } else if (file1Size == file2Size) { // If files have equal sizes
        while (bytesReadFile1 != 0) { // Could have also used bytesReadFile2 since they're equal
            if (bufferFile1[0] != bufferFile2[0]) {
                fdw = write (fdr1, bufferFile1, 1);
                handleFDWExceptions (fdw);
            } // if
            bytesReadFile1 = read (fdrFile1, bufferFile1, bufferSize - 1);
            handleFDRExceptions (bytesReadFile1);
            bytesReadFile2 = read (fdrFile2, bufferFile2, bufferSize - 1);
            handleFDRExceptions (bytesReadFile2);

        } // while
    } // if

    // Closing opened files
    close (fdr1);
    close (fdrFile1);
    close (fdrFile2);
    // Freeing allocated memory
    free (bufferFile1);
    bufferFile1 = NULL;
    free (bufferFile2);
    bufferFile2 = NULL;

    // To determine the amount of time this step takes
    gettimeofday (&endTime, NULL);
    step1Time = ((endTime.tv_sec - startTime.tv_sec) * 1000.0 + (endTime.tv_usec - startTime.tv_usec) / 1000.0);
    printf ("Step 1 took %.6f milliseconds\n", step1Time);



} // step1

void handleSeekExceptions (int fd) {
    if (fd < 0) {
        printf ("There was an error seeking the file.\n");
    } // if
} // handleSeekExceptions

void handleFDWExceptions (int fdw) {
    if (fdw < 0) {
        printf ("There was an error writing to a file.\n");
        exit (0);
    } // if
} // handleFDWExceptions

void handleFDRExceptions (int fdr) {
    if (fdr < 0) {
        printf ("There was an error reading a file.\n");
        exit (0);
    } // if
} // handleFDRExceptions

void handleArgExceptions (int argc) {
  if(argc != 3){
    printf("Usage: proj3.out <file1> <file2>\n");
    exit (0);
  }

} // handleArgExceptions
