# Homework#2
##### This homework is about the developing kernels that will support multi-programming, interrupt handling and context switching.
***
### General Informations
There are some assembly files and C++ files inside the of [Code](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes) folder.<br/> 
The goal is running [BinarySearch.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/BinarySearch.s), [LinearSearch.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/LinearSearch.s) and [Collatz.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/Collatz.s) on the different kernels such as : [SPIMOS_GTU_1.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/SPIMOS_GTU_1.s), [SPIMOS_GTU_2.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/SPIMOS_GTU_2.s) and [SPIMOS_GTU_3.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/SPIMOS_GTU_3.s). These kernels will support multi-programming, interrupt handling and context switching. 
***
### Expectation of Kernels
* Kernels have to support fork, execve and other necessary POSIX system call and these are simulated in this homework.
* Kernels have to load miltiple programs into the memory.
* Kernels have to handle multi-programming. To do that ProcessTable is developed. (ProcessTable is in the [syscall.h](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/syscall.h) file.)
* Kernels have to handle timer-interrupts. (SPIM_timerHandler is in the [syscall.cpp](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/syscall.cpp) file.)
* Kernels have to do context switching based on the Round Robin Scheduling.
***
> Requirements and Setup steps were mentioned at [here](https://github.com/alihaydarkurban/CSE312-Operating-Systems/blob/main/README.md). Please check them out.
***
### Compilation and Running
After installing SPIM Packages:<br/>
* Move [BinarySearch.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/BinarySearch.s), [LinearSearch.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/LinearSearch.s), [Collatz.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/Collatz.s), [SPIMOS_GTU_1.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/SPIMOS_GTU_1.s), [SPIMOS_GTU_2.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/SPIMOS_GTU_2.s) and [SPIMOS_GTU_3.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/SPIMOS_GTU_3.s) files to the "../spimsimulator-code-r730/spim" path.
* Move [syscall.cpp](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/syscall.cpp), [syscall.h](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/syscall.h) and [run.cpp](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW2/Codes/run.cpp) files to the "../spimsimulator-code-r730/CPU" path.
* Go to the "../spimsimulator-code-r730/spim" and start the execution <br/>

_1. Compilation_
```
make clean
make
sudo make install
```
_2. Running_<br />
```
./spim -efexception.s -file SPIMOS_GTU_1.s
./spim -efexception.s -file SPIMOS_GTU_2.s
./spim -efexception.s -file SPIMOS_GTU_3.s
```
***
> Expectation of this homework is mentionded [here](https://github.com/alihaydarkurban/CSE312-Operating-Systems/blob/main/HW2/HW2%20spring%202020.pdf).<br/>
> Details of my implementations are mentioned [here](https://github.com/alihaydarkurban/CSE312-Operating-Systems/blob/main/HW2/Report.pdf).

