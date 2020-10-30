# Homework#3
##### This homework is about the developing kernels that will support multi-programming, interrupt handling, context switching and keeping Process Table in SPIM memory as an assembly data structure.
***
### General Informations
There are some assembly files and C++ files inside the of [Code](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes) folder.<br/> 
The goal is running [BinarySearch.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/BinarySearch.s), [LinearSearch.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/LinearSearch.s), [Collatz.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/Collatz.s) and [Palindrome.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/Palindrome.s) on the different kernels such as : [SPIMOS_GTU_1.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/SPIMOS_GTU_1.s), [SPIMOS_GTU_2.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/SPIMOS_GTU_2.s) and [SPIMOS_GTU_3.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/SPIMOS_GTU_3.s). These kernels will support multi-programming, interrupt handling, context switching and also they keep the Process Table as an assembly data structure. 
***
> Requirements and Setup steps were mentioned at [here](https://github.com/alihaydarkurban/CSE312-Operating-Systems/blob/main/README.md). Please check them out.
***
### Compilation and Running
After installing SPIM Packages:<br/>
* Move [BinarySearch.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/BinarySearch.s), [LinearSearch.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/LinearSearch.s), [Collatz.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/Collatz.s), [Palindrome.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/Palindrome.s), [SPIMOS_GTU_1.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/SPIMOS_GTU_1.s), [SPIMOS_GTU_2.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/SPIMOS_GTU_2.s) and [SPIMOS_GTU_3.s](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/SPIMOS_GTU_3.s) files to the "../spimsimulator-code-r730/spim" path.
* Move [syscall.cpp](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/syscall.cpp), [syscall.h](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/syscall.h) and [run.cpp](https://github.com/alihaydarkurban/CSE312-Operating-Systems/tree/main/HW3/Codes/run.cpp) files to the "../spimsimulator-code-r730/CPU" path.
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
