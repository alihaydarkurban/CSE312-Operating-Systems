# Homework#1
##### This homework is about the creating new process and going back to the main process. To do that a simple shell program was developed.
***
### General Informations
There are some .asm file inside the of [Code](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes) folder.<br/> The goal is calling and running [ShowDivisibleNumbers.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/Shell.asm), [BinarySearch.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/BinarySearch.asm), [LinearSearch.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/LinearSearch.asm) and [SelectionSort.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/SelectionSort.asm) files from the [Shell.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/Shell.asm) on the SPIM Similator. SPIM Similator does not support fork, waitpid, and execve system calls, so that we have to implement some system services for this shell.
***
> Requirements and Setup steps were mentioned at [here](https://github.com/alihaydarkurban/CSE312-Operating-Systems/main/README.md). Please check them out.
***
### Compilation and Running
After installing SPIM Packages:<br/>
* Move [ShowDivisibleNumbers.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/Shell.asm), [BinarySearch.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/BinarySearch.asm), [LinearSearch.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/LinearSearch.asm), [SelectionSort.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/SelectionSort.asm) and [Shell.asm](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/Shell.asm) files to the "../spimsimulator-code-r730/spim" path.
* Move [syscall.cpp](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/syscall.cpp) and [syscall.h](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW1/Codes/syscall.h) files to the "../spimsimulator-code-r730/CPU" path.
* Go to the "../spimsimulator-code-r730/spim" and start the execution <br/>

_1. Compilation_
```
make clean
make
sudo make install
```
_2. Running_<br />
```
./spim
load "Shell.asm"
run
```
