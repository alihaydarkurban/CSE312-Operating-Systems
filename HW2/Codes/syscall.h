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

////////////// Created by Ali Haydar KURBAN //////////////
typedef struct 
{
   int32 Registers[32];       // It stores the Registers of the process 
   mem_addr ProgramCounter;   // It stores the Program Counter of the process
   
   int ProcessID;             // It is Process Id that defines the process 
   char ProcessName[64];      // It stores the process name
   int ProcessState;          // -1 terminate, 0 ready, 1 running, 2 not_start_yet

}ProcessTable;                

void SPIM_timerHandler();     // The timer interrupt happens about every 100ms
void print_process_table();   // Printing proces informations that are inside the ProcessTable
////////////// Created by Ali Haydar KURBAN //////////////

int do_syscall ();
void handle_exception ();

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
#define CREATE_PROCESS 18 // It was for homework#1. It is not using in this homework

#define INITIALIZE_PROCESS_TABLE 19 // Allocate memory for Process Table

#define FORK 20 // Add process into the Process Table

#define PRINT_PROCESS_TABLE 21 // Print Process Table

#define PROCESS_EXIT 22 // Exit call for SPIMOS_GTU_X.s

#define EXECVE 23 // Start execution

#define EXIT_AND_EXECUTE_NEW_PROCESS 24 // Exit and execute new process

#define RANDOM_INT_GENERATOR 42 // Generate random integer
////////////// Created by Ali Haydar KURBAN //////////////