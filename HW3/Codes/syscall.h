/* SPIM S20 MIPS simulator.
   Execute SPIM syscalls, both in simulator and bare mode.

   Copyright (c) 1990-2010, James R. Larus.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

   Neither the name of the James R. Larus nor the names of its contributors may be
   used to endorse or promote products derived from this software without specific
   prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Exported functions. */

int do_syscall ();
void handle_exception ();

////////////// Created by Ali Haydar KURBAN //////////////
// Timer interrupt happens about every 100ms. When it occurs, make the context switching according the Round Robin scheduling.
void SPIM_timerHandler();

// It prints the information about the processes. (0 is for time_interrupt, 1 is for finish)
void print_processes(int time_interrupt_or_finish);

// It creates a new process. It means that storing the address of the new assembly struct.
void _fork(int reg_a0);

// It starts the execution with the next process.
void _execv();

// It occurs when a process was terminated, then it makes the context switching according the Round Robin scheduling.
void terminate_and_execute_new_process();

// It occurs when a SPIMOS_GTU_X.s was terminated, then it terminates the whole program by POSIX PROCESS_EXIT.
void process_exit();
////////////// Created by Ali Haydar KURBAN //////////////

#define PRINT_INT_SYSCALL	1
#define PRINT_FLOAT_SYSCALL	2
#define PRINT_DOUBLE_SYSCALL	3
#define PRINT_STRING_SYSCALL	4

#define READ_INT_SYSCALL	5
#define READ_FLOAT_SYSCALL	6
#define READ_DOUBLE_SYSCALL	7
#define READ_STRING_SYSCALL	8

#define SBRK_SYSCALL		9

#define EXIT_SYSCALL		10

#define PRINT_CHARACTER_SYSCALL	11
#define READ_CHARACTER_SYSCALL	12

#define OPEN_SYSCALL		13
#define READ_SYSCALL		14
#define WRITE_SYSCALL		15
#define CLOSE_SYSCALL		16

#define EXIT2_SYSCALL		17


////////////// Created by Ali Haydar KURBAN //////////////
// It is a syscall that allocates memory to keep the beginning address of the assembly struct
#define ALLOCATE_MEMORY_FOR_ADDRESS_OF_ASSEMBLY_STRUCT 18 

// It is a syscall that takes the assembly struct and call the _fork function
#define TAKE_ASSEMBLY_STRUCT_AND_CALL_FORK 19

// It is a syscall that changes the code image with next process 
#define EXECVE 20 
   
// It is a syscall for SPIMOS_GTU_X.s. This is POSIX PROCESS_EXIT syscall 
#define PROCESS_EXIT 21 

// It is a syscall that terminates the process and execute new process
#define TERMINATE_AND_EXECUTE_NEW_PROCESS 22 

// It is a syscall that generate random integer
#define RANDOM_INT_GENERATOR 42 

// It represents the amount of register
#define REGISTER_COUNT 32

// It is max length for process name
#define MAX_NAME_LENGTH 64
////////////// Created by Ali Haydar KURBAN //////////////