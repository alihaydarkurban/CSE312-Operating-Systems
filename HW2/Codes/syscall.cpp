/* SPIM S20 MIPS simulator.
   Execute SPIM syscalls, both in simulator and bare mode.
   Execute MIPS syscalls in bare mode, when running on MIPS systems.
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


#ifndef _WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "spim.h"
#include "string-stream.h"
#include "inst.h"
#include "reg.h"
#include "mem.h"
#include "sym-tbl.h"
#include "syscall.h"

//////////////////////////////////////
#include "spim-utils.h"
#include "scanner.h"
/////////////////////////////////////

#include <iostream>
using namespace std;

////////////// Created by Ali Haydar KURBAN //////////////
ProcessTable* ProcessTableArr; // ProcessTable pointer
int size_of_process_table = 0; // Size of ProcessTableArr
////////////// Created by Ali Haydar KURBAN //////////////

#ifdef _WIN32
/* Windows has an handler that is invoked when an invalid argument is passed to a system
   call. https://msdn.microsoft.com/en-us/library/a9yf33zb(v=vs.110).aspx

   All good, except that the handler tries to invoke Watson and then kill spim with an exception.

   Override the handler to just report an error.
*/

#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>


void myInvalidParameterHandler(const wchar_t* expression,
 const wchar_t* function, 
 const wchar_t* file, 
 unsigned int line, 
 uintptr_t pReserved)
{
  if (function != NULL)
  {
    run_error ("Bad parameter to system call: %s\n", function);
  }
  else
  {
    run_error ("Bad parameter to system call\n");
  }
}

static _invalid_parameter_handler oldHandler;

void windowsParameterHandlingControl(int flag )
{
  static _invalid_parameter_handler oldHandler;
  static _invalid_parameter_handler newHandler = myInvalidParameterHandler;

  if (flag == 0)
  {
    oldHandler = _set_invalid_parameter_handler(newHandler);
      _CrtSetReportMode(_CRT_ASSERT, 0); // Disable the message box for assertions.
    }
    else
    {
      newHandler = _set_invalid_parameter_handler(oldHandler);
      _CrtSetReportMode(_CRT_ASSERT, 1);  // Enable the message box for assertions.
    }
  }
#endif


////////////// Created by Ali Haydar KURBAN //////////////
// This funtctions runs about every 100ms and it causes of the timer interrupt
// When it works it does context switching operation based on the Round Robin Scheduling mechanism.
// If all processes are then then it goes back to the init process and terminate the execution
// Otherwise it finds the newly running process, stores the lastly worked process and runs the new process   
void SPIM_timerHandler()
{
    if(size_of_process_table != 0 && ProcessTableArr[0].ProcessState == 0)
    {
      print_process_table();

      bool continuable;
      bool allTerminated = true;
      int last_worker_process_index = -1;
      int circular_index = -1;
      int next_run_process_index = -1;

      // This loop checks if all processes had finished the execution 
      for(int i = 1; i < size_of_process_table; ++i)
      {
        if(ProcessTableArr[i].ProcessState != -1)
        {
          allTerminated = false;
        }
      }

      // If all proceses are gone than go back to the init process
      if(allTerminated)
      {
        PC = ProcessTableArr[0].ProgramCounter;
        for(int i = 0 ; i < 32; ++i)
        {
          R[i] = ProcessTableArr[0].Registers[i];
        }
        return;
      }

      // Otherwise it finds the newly running process, stores the lastly worked process and runs the new process
      else
      {
        // Finding lastly worked process 
        for(int i = 1; i < size_of_process_table; ++i)
        {
          if(ProcessTableArr[i].ProcessState == 1)
          {
            last_worker_process_index = i;
            break;
          }
        }

        printf("Process with Process ID : %d was interrupted\n", last_worker_process_index);

        // Stores the information about the lastly worked process into the ProcessTableArr
        ProcessTableArr[last_worker_process_index].ProgramCounter = PC;
        ProcessTableArr[last_worker_process_index].ProcessState = 0;
        for(int i = 0; i < 32; ++i)
        {
          ProcessTableArr[last_worker_process_index].Registers[i] = R[i];
        }

        // Finds and runs the newly running process based on the Round Robin Scheduling mechanism
        circular_index = last_worker_process_index + 1;
        while(true)
        {
          if(circular_index > size_of_process_table - 1) // Simple circular array indexing mechanism
            circular_index = 1;

          // If the process is not working and is not gone, then you found the newly running process
          if(ProcessTableArr[circular_index].ProcessState != 1 && ProcessTableArr[circular_index].ProcessState != -1) 
          {
            next_run_process_index = circular_index; 
          }
          circular_index++;

          if(next_run_process_index != -1)
          {
            printf("Next Running Process ID : %d\n",next_run_process_index );
          }

          // If the newly running process in the ready state(It means that the process has worked in the past)
          if(ProcessTableArr[next_run_process_index].ProcessState == 0)
          {
            ProcessTableArr[next_run_process_index].ProcessState = 1; // Makes its state 1 (1 means that it is in running state) 

            // Assing the register of newly running process to the system regiters
            for(int i = 0; i < 32; ++i)
            {
              R[i] = ProcessTableArr[next_run_process_index].Registers[i];
            }
            // Runs the program from it was stopped
            run_program(ProcessTableArr[next_run_process_index].ProgramCounter, DEFAULT_RUN_STEPS, false, false, &continuable); 
          }

          // If the newly running process in the not_started_yet state(It means that the process has not worked in the past)
          else if(ProcessTableArr[next_run_process_index].ProcessState == 2)
          {
            pop_scanner();
            ProcessTableArr[next_run_process_index].ProcessState = 1; // Makes its state 1 (1 means that it is in running state) 
            run_program(ProcessTableArr[next_run_process_index].ProgramCounter, DEFAULT_RUN_STEPS, false, false, &continuable); // Runs the new process
          }
        } 
      }
    }
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// This function prints the process informations that are in the ProcessTableArr
// It works when the timmer interrupt occurs
void print_process_table()
{
  printf("\n==============================Time Intterupt Occurs=============================\n");
  printf("\n");
  for(int i = 0; i < size_of_process_table; ++i)
  {
    printf("ProcessID : %d\tProcessName : %s\nPC : %d\tStackPointerAddress : %08x\n", 
      ProcessTableArr[i].ProcessID, ProcessTableArr[i].ProcessName,
      ProcessTableArr[i].ProgramCounter, ProcessTableArr[i].Registers[REG_SP]);
  }
  printf("================================================================================\n");

}
////////////// Created by Ali Haydar KURBAN //////////////



/* Decides which syscall to execute or simulate.  Returns zero upon
   exit syscall and non-zero to continue execution. */


int
do_syscall ()
{

#ifdef _WIN32
  windowsParameterHandlingControl(0);
#endif

  /* Syscalls for the source-language version of SPIM.  These are easier to
     use than the real syscall and are portable to non-MIPS operating
     systems. */

  switch (R[REG_V0])
  {
    case PRINT_INT_SYSCALL:
    write_output (console_out, "%d", R[REG_A0]);
    break;

    case PRINT_FLOAT_SYSCALL:
    {
     float val = FPR_S (REG_FA0);

     write_output (console_out, "%.8f", val);
     break;
   }

   case PRINT_DOUBLE_SYSCALL:
   write_output (console_out, "%.18g", FPR[REG_FA0 / 2]);
   break;

   case PRINT_STRING_SYSCALL:
   write_output (console_out, "%s", mem_reference (R[REG_A0]));
   break;

   case READ_INT_SYSCALL:
   {
     static char str [256];

     read_input (str, 256);
     R[REG_RES] = atol (str);
     break;
   }

   case READ_FLOAT_SYSCALL:
   {
     static char str [256];

     read_input (str, 256);
     FPR_S (REG_FRES) = (float) atof (str);
     break;
   }

   case READ_DOUBLE_SYSCALL:
   {
     static char str [256];

     read_input (str, 256);
     FPR [REG_FRES] = atof (str);
     break;
   }

   case READ_STRING_SYSCALL:
   {
     read_input ( (char *) mem_reference (R[REG_A0]), R[REG_A1]);
     data_modified = true;
     break;
   }

   case SBRK_SYSCALL:
   {
     mem_addr x = data_top;
     expand_data (R[REG_A0]);
     R[REG_RES] = x;
     data_modified = true;
     break;
   }

   case PRINT_CHARACTER_SYSCALL:
   write_output (console_out, "%c", R[REG_A0]);
   break;

   case READ_CHARACTER_SYSCALL:
   {
     static char str [2];

     read_input (str, 2);
	if (*str == '\0') *str = '\n';      /* makes xspim = spim */
     R[REG_RES] = (long) str[0];
     break;
   }

   case EXIT_SYSCALL:
   spim_return_value = 0;
   return (0);

   case EXIT2_SYSCALL:
      spim_return_value = R[REG_A0];	/* value passed to spim's exit() call */
   return (0);

   case OPEN_SYSCALL:
   {
#ifdef _WIN32
    R[REG_RES] = _open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#else
    R[REG_RES] = open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#endif
    break;
  }

  case READ_SYSCALL:
  {
	/* Test if address is valid */
   (void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
   R[REG_RES] = _read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
   R[REG_RES] = read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
   data_modified = true;
   break;
 }

 case WRITE_SYSCALL:
 {
	/* Test if address is valid */
   (void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
   R[REG_RES] = _write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
   R[REG_RES] = write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
   break;
 }

 case CLOSE_SYSCALL:
 {
#ifdef _WIN32
   R[REG_RES] = _close(R[REG_A0]);
#else
   R[REG_RES] = close(R[REG_A0]);
#endif
   break;
 }

////////////// Created by Ali Haydar KURBAN //////////////
 case CREATE_PROCESS:
 { 
  int i;
  bool continuable;
      char runnable_command[26];  // Storing the runnable program name that getting in Shell.asm

      int32 tempRegisterArray[32];  // Temporary storing the registers
      mem_addr parent_process_PC = PC; // Program Counter of the main process

      strcpy(runnable_command, (char*)mem_reference (R[REG_A3])); // $a0 register is used for the getting program name from the Shell.asm

      // This loop stores the current register into the tempRegisterArray
      for(i = 0; i < 32; ++i)
      {
        tempRegisterArray[i] = R[i];
      }

      // These are operation of creating a new process

      flush_local_labels(0); 
      initialize_symbol_table();
      
      read_assembly_file(runnable_command); // Read the assembly file that typed at Shell.asm

      pop_scanner();
      
      // Run the program that typed at Shell.asm
      // find_symbol_address("main") finds the main of the program and starts the execution
      run_program(find_symbol_address("main"), DEFAULT_RUN_STEPS, false, false, &continuable); 


      // After new process is over, we need to go back to the main process.
      PC = parent_process_PC; // Assing the PC to Program Counter of the main process

      // This loop sets back the registers of main process to the system' registers 
      for(i = 0; i < 32; ++i)
      {
        R[i] = tempRegisterArray[i];
      }

      break;
    }
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// It gets the total number of process that will be run from the SPIMOS_GTU_X.s
// It initializes the ProcessTableArr 
    case INITIALIZE_PROCESS_TABLE:
    {
      // $a3 register is used for getting the total number of process that will be run
      // This $a3(REG_A3) register comes from the SPIMOS_GTU_X.s
      int amount_of_process = R[REG_A3]; 
      size_of_process_table = amount_of_process;
      ProcessTableArr = (ProcessTable* )malloc(amount_of_process * sizeof(ProcessTable));
      break;
    }
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// This system call generates random integer
// It gets the upper bound interger and assign the $v0 to random integer
// $a0(REG_A0) is used for getting upper bound integer, $v0(REG_RES) is used for returning the random integer
    case RANDOM_INT_GENERATOR:
    {
      srand(time(NULL));
      int random_number, upper_bound = R[REG_A0];
      random_number = rand() % upper_bound;
      R[REG_RES] = random_number;
      break;
    }
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// This system call add the processes which was forked in to the ProcessTableArr
// It is the way of creating new process    
// For each process, all the informations that are needed were assigned
// ProcessTableArr[0] is for the unit for each SPIMOS_GTU_X.s 
    case FORK:
    {
      int process_id; // It stores the process_id that was assigned in the SPIMOS_GTU_X.s
      char process_name[64]; // It stores the process_id that was assigned in the SPIMOS_GTU_X.s
      char process_id_str[10];
      mem_addr current_pc; // It stores the Program Counter

      strcpy(process_name, (char*)mem_reference (R[REG_A0])); // $a0(REG_A0) register is used for the getting process name from the SPIMOS_GTU_X.s
      process_id = R[REG_A1]; // $a1(REG_A1) register is used for the getting process is from the SPIMOS_GTU_X.s

      // It works if the process is not init process 
      if(strcmp(process_name, "init") != 0) 
      {
        sprintf(process_id_str, "%d", process_id); // It is for generating a process name

        initialize_symbol_table();
        read_assembly_file(process_name); // Loads the forked process in to the memory

        current_pc = find_symbol_address("main"); // It finds the main of the program and starts the execution 

        strcat(process_name, "_"); // It is for generating a process name
        strcat(process_name, process_id_str); // It is for generating a process name

        ProcessTableArr[process_id].ProgramCounter = current_pc; // Assigns ProgramCounter of the forked process in the ProcessTableArr
      }

      ProcessTableArr[process_id].ProcessID = process_id; // Assigns Process Id into the ProcessTableArr
      strcpy(ProcessTableArr[process_id].ProcessName, process_name); // Assigns Process Name into the ProcessTableArr
      ProcessTableArr[process_id].ProcessState = 2; // Sets all process as not_start_yet(2 means not started yet) into the ProcessTableArr
      ProcessTableArr[0].ProgramCounter = PC; // Assigns the PC to the ProgramCounter of init

      break;
  }
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// This system call prints the infortaions about the process which are in the ProcessTableArr
  case PRINT_PROCESS_TABLE:
  {
    print_process_table();
    break;
  }
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// This system call makes the init process stop
// After that makes the new process run
  case EXECVE:
  {
    bool continuable;

    // Storing informations about the init process to come back and rerun it.
    ProcessTableArr[0].ProcessState = 0; // Makes init in ready state(0 means that it is in ready state)
    ProcessTableArr[0].ProgramCounter = PC; // Stores the ProgramCounter

    // This loop stores the registers of init process
    for(int i = 0; i < 32; ++i)
    {
      ProcessTableArr[0].Registers[i] = R[i];
    }

    // These operations are for running the next process whic is in the ProcessTableArr
    pop_scanner();
    ProcessTableArr[1].ProcessState = 1; // Makes the new process in running state(1 means that it is in running state)
    run_program(ProcessTableArr[1].ProgramCounter, DEFAULT_RUN_STEPS, false, false, &continuable); // Runs the new process
    break;
  }
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// These system call terminate the execution after all processes are gone.
  case PROCESS_EXIT:
  {
    printf("\n================================================================================\n");
    printf("End of Micro Kernel...\n");
    free(ProcessTableArr);
    printf("Created by Ali Haydar KURBAN\n");
    printf("================================================================================\n");
    exit(1);
  }
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// This system call runs when a process finished its task
// When it works it does context switching operation based on the Round Robin Scheduling mechanism.
// If all processes are then then it goes back to the init process and terminate the execution
// Otherwise it finds the newly running process, stores the lastly worked process and runs the new process  
  case EXIT_AND_EXECUTE_NEW_PROCESS:
  {
    bool continuable;
    bool allTerminated = true;
    int last_worker_process_index = -1;
    int circular_index = -1;
    int next_run_process_index = -1;
    int temp_index = -1;

    // This loop finds the process which finished its task
    for(int i = 1; i < size_of_process_table; ++i)
    {
      if(ProcessTableArr[i].ProcessState == 1)
      {
        temp_index = i;
        ProcessTableArr[i].ProcessState = -1;
        break;
      }
    }

    // This loop checks if all processes had finished the execution 
    for(int i = 1; i < size_of_process_table; ++i)
    {
      if(ProcessTableArr[i].ProcessState != -1)
      {
        allTerminated = false;
      }
    }

    // If all proceses are gone than go back to the init process
    if(allTerminated)
    {
      PC = ProcessTableArr[0].ProgramCounter;
      for(int i = 0 ; i < 32; ++i)
      {
        R[i] = ProcessTableArr[0].Registers[i];
      }
      break;
    }

    // Otherwise it finds the newly running process, stores the lastly worked process and runs the new process
    else
    {
      last_worker_process_index = temp_index;

      printf("Process with Process ID : %d was finished\n", last_worker_process_index);
      
      // Stores the information about the lastly worked process into the ProcessTableArr
      if(ProcessTableArr[last_worker_process_index].ProcessState != -1)
      {
        ProcessTableArr[last_worker_process_index].ProgramCounter = PC;
        ProcessTableArr[last_worker_process_index].ProcessState = 0;
        for(int i = 0; i < 32; ++i)
        {
          ProcessTableArr[last_worker_process_index].Registers[i] = R[i];
        }
      }

      // Finds and runs the newly running process based on the Round Robin Scheduling mechanism
      circular_index = last_worker_process_index + 1;
      while(true)
      {
        if(circular_index > size_of_process_table - 1) // Simple circular array indexing mechanism
          circular_index = 1;

        // If the process is not working and is not gone, then you found the newly running process
        if(ProcessTableArr[circular_index].ProcessState != 1 && ProcessTableArr[circular_index].ProcessState != -1)
        {
          next_run_process_index = circular_index;
        }
        circular_index++;

        if(next_run_process_index != -1)
        {
          printf("Next Running Process ID : %d\n",next_run_process_index );
        }

        // If the newly running process in the ready state(It means that the process has worked in the past)
        if(ProcessTableArr[next_run_process_index].ProcessState == 0)
        {
          ProcessTableArr[next_run_process_index].ProcessState = 1; // Makes its state 1 (1 means that it is in running state) 

          // Assing the register of newly running process to the system regiters
          for(int i = 0; i < 32; ++i)
          {
            R[i] = ProcessTableArr[next_run_process_index].Registers[i];
          }
          // Runs the program from it was stopped
          run_program(ProcessTableArr[next_run_process_index].ProgramCounter, DEFAULT_RUN_STEPS, false, false, &continuable);

        }

        // If the newly running process in the not_started_yet state(It means that the process has not worked in the past)
        else if(ProcessTableArr[next_run_process_index].ProcessState == 2)
        {
          pop_scanner();
          ProcessTableArr[next_run_process_index].ProcessState = 1; // Makes its state 1 (1 means that it is in running state) 
          run_program(ProcessTableArr[next_run_process_index].ProgramCounter, DEFAULT_RUN_STEPS, false, false, &continuable); // Runs the new process
          break;
        }
      } 
    }
    break;
  }
////////////// Created by Ali Haydar KURBAN //////////////


  default:
  run_error ("Unknown system call: %d\n", R[REG_V0]);
  break;
}

#ifdef _WIN32
windowsParameterHandlingControl(1);
#endif
return (1);
}


void
handle_exception ()
{
  if (!quiet && CP0_ExCode != ExcCode_Int)
    error ("Exception occurred at PC=0x%08x\n", CP0_EPC);

  exception_occurred = 0;
  PC = EXCEPTION_ADDR;

  switch (CP0_ExCode)
  {
    case ExcCode_Int:
    break;

    case ExcCode_AdEL:
    if (!quiet)
     error ("  Unaligned address in inst/data fetch: 0x%08x\n", CP0_BadVAddr);
   break;

   case ExcCode_AdES:
   if (!quiet)
     error ("  Unaligned address in store: 0x%08x\n", CP0_BadVAddr);
   break;

   case ExcCode_IBE:
   if (!quiet)
     error ("  Bad address in text read: 0x%08x\n", CP0_BadVAddr);
   break;

   case ExcCode_DBE:
   if (!quiet)
     error ("  Bad address in data/stack read: 0x%08x\n", CP0_BadVAddr);
   break;

   case ExcCode_Sys:
   if (!quiet)
     error ("  Error in syscall\n");
   break;

   case ExcCode_Bp:
   exception_occurred = 0;
   return;

   case ExcCode_RI:
   if (!quiet)
     error ("  Reserved instruction execution\n");
   break;

   case ExcCode_CpU:
   if (!quiet)
     error ("  Coprocessor unuable\n");
   break;

   case ExcCode_Ov:
   if (!quiet)
     error ("  Arithmetic overflow\n");
   break;

   case ExcCode_Tr:
   if (!quiet)
     error ("  Trap\n");
   break;

   case ExcCode_FPE:
   if (!quiet)
     error ("  Floating point\n");
   break;

   default:
   if (!quiet)
     error ("Unknown exception: %d\n", CP0_ExCode);
   break;
 }
}
