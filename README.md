To compile: make <br/>
To run: ./project3.out [.txt file] [.txt file]; [optional bash cmds separated by ;] <br/>

This program compares two input text files and writes every byte in file one that is different from file two into a third file named differencesFoundInFile1.txt and every byte that is different in file two from file one into a fourth file named differencesFoundInFile2.txt. <br/>
If either files are in the PWD, program will overwrite the file(s) with new output. If not, program will create the files. Compiles with Makefile. Can use provided text files within the txt-files directory or your own text files! <br/>
Example (Timing will differ): <br/>
./proj3.out txt-files/input1.txt txt-files/input2.txt; cat differencesFoundInFile1.txt differencesFoundInFile2.txt <br/>
RESULT: <br/>
Step 1 took 0.023000 milliseconds <br/>
Step 2 took 0.005000 milliseconds <br/>
cat!dog.
