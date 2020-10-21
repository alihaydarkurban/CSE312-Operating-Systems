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

int amount_of_processes = 0;            // This is a global variable that holds the number of total processes.
int *addres_of_assembly_structs = NULL; // This is a global variable that holds the address of the each assembly struct.

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
// Timer interrupt happens about every 100ms. When it occurs, make the context switching according the Round Robin scheduling.
void SPIM_timerHandler()
{
// These are variables for addres of the assembly struct's items.
  int *index;
  int register_reg_addr;
  int pc_reg_addr;
  int id_reg_addr;
  int name_reg_addr;
  int state_reg_addr;

// It checks are there any process was forked. 
// If there is not any forked processes this will not work
  if(addres_of_assembly_structs != NULL)
  {
  // These are variables for addres of the process of init's items. 
    register_reg_addr = addres_of_assembly_structs[0];
    pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32));
    id_reg_addr = pc_reg_addr + sizeof(mem_addr);
    name_reg_addr = id_reg_addr + sizeof(int);
    state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char));

  // This index is the address of the init's state
    index = (int *)mem_reference(state_reg_addr);

  // *index means that the value of the this index adrres. 
  // *index == 0 means that the init is in the ready state.
    if(*index == 0) 
    {
    // This print the processes acoording to assembly struct.
    // 0 which is the function parameter measn that this print_processes 
    // function will work when the time intterupt occurs.
      print_processes(0);

    // These are variables to lead this task.
      bool continuable;
      bool allTerminated = true;
      int last_worker_process_index = -1;
      int circular_index = -1;
      int next_run_process_index = -1;
      bool flag_next_run = false;

    // This loop is for checking that the all processes are finished.
      for(int i = 1; i < amount_of_processes; ++i)
      {
      // These variables are used to calculate the address of current assembly struct's items.

      register_reg_addr = addres_of_assembly_structs[i]; // This is start address of the register in the current assembly struct
      pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the current assembly struct
      id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start addres of the process id in the current assembly struct
      name_reg_addr = id_reg_addr + sizeof(int); // This is start addres of the process name in the current assembly struct
      state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start addres of the process state in the current assembly struct

      // This index is the address of the process state in the current assembly struct
      index = (int *)mem_reference(state_reg_addr);

      // *index means that the value of the this index adrres. 
      // *index == -1 means that the current process is in the terminate state.
      if(*index != -1)
      {
        allTerminated = false;
      }
    }

    // This works when the all processes are terminated.
    // If all processes are terminated then run back the init processe (which means that SPIMOS_GTU_X.s)
    if(allTerminated)
    {
      int init_addr = addres_of_assembly_structs[0]; // This is the start address of the init assembly struct

      index = (int *)mem_reference(init_addr + (REGISTER_COUNT * sizeof(int32))); // This is the start address of the program counter in init assembly struct
      PC = (mem_addr)*index; // Assign the *index means that the value of the this index adrres to the PC 
      
      // This loop calculate the register address of the init assembly struct and assing it to the R.
      int32 current_reg_value;
      for(int i = 0; i < REGISTER_COUNT; ++i)
      {
        index = (int *)mem_reference(init_addr + (sizeof(int32) * i)); // Calculating the address
        current_reg_value = (int32)(*index);  
        R[i] = current_reg_value; // Assigning the value of that was calculated
      }
      return;
    }

    // This works when the all processes are not terminated.
    // If all processes are not terminated then searches the addres of the new running assembly struct. 
    else
    {
      // This loop is for finding that the last running process.
      for(int i = 1; i < amount_of_processes; ++i)
      {
        register_reg_addr = addres_of_assembly_structs[i]; // This is start address of the register in the current assembly struct
        pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the current assembly struct
        id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start addres of the process id in the current assembly struct
        name_reg_addr = id_reg_addr + sizeof(int); // This is start addres of the process name in the current assembly struct
        state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start addres of the process state in the current assembly struct

        // This index is the address of the process state in the current assembly struct
        index = (int *)mem_reference(state_reg_addr);

        // *index means that the value of the this index adrres. 
        // *index == 1 means that the current process is in the running state.
        if(*index == 1)
        {
          last_worker_process_index = i;
          break;
        }
      }

      // Printing information
      printf("Process with Process ID : %d was interrupted\n", last_worker_process_index);

      register_reg_addr = addres_of_assembly_structs[last_worker_process_index]; // This is start address of the register in the last worker assembly struct
      pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the last worker assembly struct
      id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the last worker assembly struct
      name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the last worker assembly struct
      state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the last worker assembly struct

      ///THIS PART IS FOR SAVING THE INFORMATINON ABOUT THE LAST WORKER ASSEMBLY STRUCT ///

      // This index is the address of the program counter in the last worker assembly struct
      index = (int *)mem_reference(pc_reg_addr);
      *index = PC; // Change the value of the this addres with PC

      // This index is the address of the process state in the last worker assembly struct
      index = (int *)mem_reference(state_reg_addr);
      *index = 0; // Change the value of the this addres with 0 (0 means that ready state)

      // This loop is for calculating the address of the registers in the last worker assembly struct
      // And change the value of this address with R
      for(int i = 0; i < REGISTER_COUNT; ++i)
      {
        index = (int *)mem_reference(register_reg_addr + (sizeof(int32) * i));
        *index = R[i];
      }
      ///END OF SAVING THE INFORMATINON ABOUT THE LAST WORKER ASSEMBLY STRUCT /// 

      // circular_index is for Round Robin scheduling
      circular_index = last_worker_process_index + 1;
      
      // This infinite loop is for Round Robin scheduling
      while(true)
      {
        flag_next_run = false; // This flag is check that the found process can run or not.
        // This if statement makes circular_index the circular. (Same meaning with circular array) 
        if(circular_index > amount_of_processes - 1)
          circular_index = 1;

        register_reg_addr = addres_of_assembly_structs[circular_index]; // This is start address of the register in the current assembly struct
        pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the current assembly struct
        id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the current assembly struct
        name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the current assembly struct
        state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the current assembly struct

        int PS;
        // This index is the address of the process state in the current assembly struct
        index = (int *)mem_reference(state_reg_addr);
        PS = *index; // Assing the value of the this addres to the PS

        if(PS != 1) // It means that the current assembly struct is not in the running state
        {
          // This assignment is for calculating the next run assembly sturct index
          next_run_process_index = circular_index;
        }
        circular_index++; // Edit the circular_index

        register_reg_addr = addres_of_assembly_structs[next_run_process_index]; // This is start address of the register in the next run assembly struct
        pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the next run assembly struct
        id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the next run assembly struct
        name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the next run assembly struct
        state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the next run assembly struct

        // This index is the address of the process state in the next run assembly struct
        index = (int *)mem_reference(state_reg_addr);

        // Controls that the next run assembly struct was found or not
        if(next_run_process_index != -1)
        {
          if(*index == 0 || *index == 2) // This check that the next run assembly struct's process state is ready or not_start_yet
          {
            // Printing information
            printf("Next Running Process ID : %d\n",next_run_process_index );
            flag_next_run = true; // This means that there is at least one assembly struct to run
          }
        }

        if(flag_next_run)
        {
          if(*index == 0) // This check that the next run assembly struct's process state is ready
          {
            index = (int *)mem_reference(state_reg_addr);
            *index = 1; // Change the value of the this addres with 1 (1 means that running)

            // This loop calculate the register address of the next run assembly struct and assing it to the R.
            index = (int *)mem_reference(register_reg_addr);
            int32 current_reg_value;
            for(int i = 0; i < REGISTER_COUNT; ++i)
            {
              index = (int *)mem_reference(register_reg_addr + (sizeof(int32) * i));
              current_reg_value = (int32)(*index);

              R[i] = current_reg_value;
            }

            index = (int *)mem_reference(pc_reg_addr); // This index is the address of the program counter in the next run assembly struct
            mem_addr new_pc = (mem_addr)(*index); // Change the value of the this addres with PC
            run_program(new_pc, DEFAULT_RUN_STEPS, false, false, &continuable); 
          }

          else if(*index == 2) // This check that the next run assembly struct's process state is not_start_yet
          {
            index = (int *)mem_reference(state_reg_addr);
            *index = 1; // Change the value of the this addres with 1 (1 means that running)

            index = (int *)mem_reference(pc_reg_addr); // This index is the address of the program counter in the next run assembly struct
            mem_addr new_pc = (mem_addr)(*index); // Change the value of the this addres with PC

            pop_scanner();
            run_program(new_pc, DEFAULT_RUN_STEPS, false, false, &continuable);
          }
        }
      } 
    }
  }
}
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// It prints the information about the processes. (0 is for time_interrupt, 1 is for finish)
void print_processes(int time_interrupt_or_finish)
{
  // These are variables for addres of the assembly struct's items.
  int *index;
  int register_reg_addr;
  int pc_reg_addr;
  int id_reg_addr;
  int name_reg_addr;
  // int state_reg_addr;

  int id;
  char process_name[MAX_NAME_LENGTH];
  mem_addr program_counter;
  int sp_index = REG_SP;

  char message[MAX_NAME_LENGTH];

  // This print_process function work for both time interrupt and all processes are terminated.
  // To make the print information correct was time_interrupt_or_finish used
  if(time_interrupt_or_finish == 0)
    strcpy(message, "Time Intterupt Occurs");
  else
    strcpy(message, "Processes Finished===");


  // This loop is calcutate each item address for each assembly sturct.
  // And prints the ProcessID ProcessName PC StackPointerAddress
  printf("\n==============================%s=============================\n",message);
  printf("\n");
  for(int i = 0; i < amount_of_processes; ++i)
  {
    register_reg_addr = addres_of_assembly_structs[i];
    pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32));
    id_reg_addr = pc_reg_addr + sizeof(mem_addr);
    name_reg_addr = id_reg_addr + sizeof(int);
      // state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char));

      // These are calculating the addres and printing the value of the this address
    index = (int *)mem_reference(id_reg_addr);
    id = *index;

      // These are calculating the addres and printing the value of the this address
    index = (int *)mem_reference(name_reg_addr);
    strcpy(process_name, (char *)mem_reference(*index));

      // These are calculating the addres and printing the value of the this address
    index = (int *)mem_reference(pc_reg_addr);
    program_counter = (mem_addr)(*index);

      // These are calculating the addres and printing the value of the this address
    index = (int *)mem_reference(register_reg_addr + (sizeof(int32) * sp_index));
    int32 current_reg_value;

    current_reg_value = (int32)(*index);

      // index = (int *)mem_reference(state_reg_addr);
      // printf("State : %d\n", *index);
    printf("ProcessID : %d\tProcessName : %s\nPC : %d\tStackPointerAddress : %08x\n", 
      id, process_name, program_counter, current_reg_value);
  }
  printf("================================================================================\n");
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// It creates a new process. It means that storing the address of the new assembly struct.
void _fork(int reg_a0)
{
  // These are variables for addres of the assembly struct's items.
  int *index;
  int register_reg_addr = reg_a0;
  int pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32));
  int id_reg_addr = pc_reg_addr + sizeof(mem_addr);
  int name_reg_addr = id_reg_addr + sizeof(int);
  // int state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char));

  int process_id;
  char process_name[MAX_NAME_LENGTH];
  mem_addr current_pc;

  // This is the address of the last forked assembly struct's process name address
  index = (int *)mem_reference(name_reg_addr);
  strcpy(process_name, (char *)mem_reference(*index)); // Copy the name

  // This is the address of the last forked assembly struct's process id address
  index = (int *)mem_reference(id_reg_addr);
  process_id = *index; // Assing the value of this address to the process_id

  addres_of_assembly_structs[process_id] = reg_a0; // Save the last forked assembly struct's address.

  if(strcmp(process_name, "init") != 0) // not init
  {
    initialize_symbol_table();
    read_assembly_file(process_name);

    current_pc = find_symbol_address("main");
    index = (int *)mem_reference(pc_reg_addr); // This is the address of the last forked assembly struct's program counter address
    *index = current_pc; // Change the value of this addres with current_pc 
  }

  // This is the address of the init assembly struct's program counter address
  int init_addr = addres_of_assembly_structs[0];
  index = (int *)mem_reference(init_addr + (REGISTER_COUNT * sizeof(int32)));
  *index = PC; // // Change the value of this addres with PC 
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// It starts the execution with the next process.
void _execv()
{
  // These are variables for addres of the assembly struct's items.
  int *index;
  int register_reg_addr;
  int pc_reg_addr;
  int id_reg_addr;
  int name_reg_addr;
  int state_reg_addr;

  bool continuable;

  // These are variables for addres of the process of init's items. 
  int init_process_addr = addres_of_assembly_structs[0];
  register_reg_addr = init_process_addr; // This is start address of the register in the init assembly struct
  pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the init assembly struct
  id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the init assembly struct
  name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the init assembly struct
  state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the init assembly struct

  // This index is the address of the process state in the init assembly struct
  index = (int *)mem_reference(state_reg_addr);
  *index = 0; // Change the value of this address with 0 (0 means that ready)

  // This index is the address of the program counter in the init assembly struct
  index = (int *)mem_reference(pc_reg_addr);
  *index = PC; // Change the value of this address with PC 

  // This loop is for saving the init assembly sturct's registers
  for(int i = 0; i < REGISTER_COUNT; ++i)
  {
    index = (int *)mem_reference(register_reg_addr + (sizeof(int32) * i));
    *index = R[i];
  }

  // These are variables for addres of the process that next forked than init
  int first_process_addr = addres_of_assembly_structs[1];
  register_reg_addr = first_process_addr; // This is start address of the register in the next forked than init assembly struct
  pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the next forked than init assembly struct
  id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the next forked than init assembly struct
  name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the next forked than init assembly struct
  state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the next forked than init assembly struct

  // This index is the address of the process state that next forked than init assembly struct
  index = (int *)mem_reference(state_reg_addr);
  *index = 1; // Change the value of this address with 1 (1 means that running)

  // This index is the address of the program counter that next forked than init assembly struct
  index = (int *)mem_reference(pc_reg_addr);
  mem_addr next_run_pc = (mem_addr)(*index); // Assing the value of this address to the next_run_pc

  pop_scanner();
  run_program(next_run_pc, DEFAULT_RUN_STEPS, false, false, &continuable);
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// It occurs when a process was terminated, then it makes the context switching according the Round Robin scheduling.
void terminate_and_execute_new_process()
{
  // These are variables for addres of the assembly struct's items.
  int *index;
  int register_reg_addr;
  int pc_reg_addr;
  int id_reg_addr;
  int name_reg_addr;
  int state_reg_addr;

  // These are variables to lead this task.
  bool continuable;
  bool allTerminated = true;
  int last_worker_process_index = -1;
  int circular_index = -1;
  int next_run_process_index = -1;
  int temp_index = -1;
  bool flag_next_run = false;

  // This loop is for finding running process and making this process's process state -1 (-1 means that terminate state)
  for(int i = 1; i < amount_of_processes; ++i)
  {
    register_reg_addr = addres_of_assembly_structs[i]; // This is start address of the register in the current assembly struct
    pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the current assembly struct
    id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the current assembly struct
    name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the current assembly struct
    state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the current assembly struct

    // This index is the address of the process state in the current assembly struct
    index = (int *)mem_reference(state_reg_addr);

    // *index means that the value of the this index adrres. 
    // *index == 1 means that the current process is in the runnig state.
    if(*index == 1)
    {
      temp_index = i; // It is going to use for last_worker_process
      *index = -1; // Change the value of this address with -1 (-1 means that terminate state)
      break;
    }
  }

  // This loop is for checking that the all processes are finished.
  for(int i = 1; i < amount_of_processes; ++i)
  {
    // These variables are used to calculate the address of current assembly struct's items.
    register_reg_addr = addres_of_assembly_structs[i]; // This is start address of the register in the current assembly struct
    pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the current assembly struct
    id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the current assembly struct
    name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the current assembly struct
    state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the current assembly struct

    // This index is the address of the process state in the current assembly struct
    index = (int *)mem_reference(state_reg_addr);

    // *index means that the value of the this index adrres. 
    // *index == -1 means that the current process is in the terminate state.
    if(*index != -1) 
    {
      allTerminated = false;
    }
  }

  // This works when the all processes are terminated.
  // If all processes are terminated then run back the init processe (which means that SPIMOS_GTU_X.s)
  if(allTerminated)
  {
    int init_addr = addres_of_assembly_structs[0]; // This is the start address of the init assembly struct

    index = (int *)mem_reference(init_addr + (REGISTER_COUNT * sizeof(int32))); // This is the start address of the program counter in init assembly struct
    PC = (mem_addr)(*index);  // Assign the *index means that the value of the this index adrres to the PC

    // This loop calculate the register address of the init assembly struct and assing it to the R.
    int32 current_reg_value;
    for(int i = 0; i < REGISTER_COUNT; ++i)
    {
      index = (int *)mem_reference(init_addr + (sizeof(int32) * i));
      current_reg_value = (int32)(*index);
      R[i] = current_reg_value;
    }
    return;
  }

  // This works when the all processes are not terminated.
  // If all processes are not terminated then searches the addres of the new running assembly struct.
  else
  {
    last_worker_process_index = temp_index;

    register_reg_addr = addres_of_assembly_structs[last_worker_process_index]; // This is start address of the register in the last worker assembly struct
    pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the last worker assembly struct
    id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the last worker assembly struct
    name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the last worker assembly struct
    state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the last worker assembly struct

    // Printing information
    printf("Process with Process ID : %d was finished\n", last_worker_process_index);
    
    ///THIS PART IS FOR SAVING THE INFORMATINON ABOUT THE LAST WORKER ASSEMBLY STRUCT ///
    index = (int *)mem_reference(state_reg_addr);

    if(*index != -1) 
    {
      // This index is the address of the program counter in the last worker assembly struct
      index = (int *)mem_reference(pc_reg_addr);
      *index = PC; // Change the value of the this addres with PC

      // This index is the address of the process state in the last worker assembly struct
      index = (int *)mem_reference(state_reg_addr);
      *index = 0; // Change the value of the this addres with 0 (0 means that ready state)

      // This loop is for calculating the address of the registers in the last worker assembly struct
      // And change the value of this address with R
      for(int i = 0; i < REGISTER_COUNT; ++i)
      {
        index = (int *)mem_reference(register_reg_addr + (sizeof(int32) * i));
        *index = R[i];
      }
    }
    ///END OF SAVING THE INFORMATINON ABOUT THE LAST WORKER ASSEMBLY STRUCT /// 

    // circular_index is for Round Robin scheduling
    circular_index = last_worker_process_index + 1;
    
    // This infinite loop is for Round Robin scheduling
    while(true)
    {
      flag_next_run = false; // This flag is check that the found process can run or not.
      // This if statement makes circular_index the circular. (Same meaning with circular array)
      if(circular_index > amount_of_processes - 1)
        circular_index = 1;

      register_reg_addr = addres_of_assembly_structs[circular_index]; // This is start address of the register in the current assembly struct
      pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the current assembly struct
      id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the current assembly struct
      name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the current assembly struct
      state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the current assembly struct

      int PS;
      index = (int *)mem_reference(state_reg_addr); // This index is the address of the process state in the current assembly struct
      PS = *index; // Assing the value of the this addres to the PS

      // It means that the current assembly struct is not in the running state
      if(PS != 1) 
      {
        next_run_process_index = circular_index; // This assignment is for calculating the next run assembly sturct index
      }
      circular_index++;

      register_reg_addr = addres_of_assembly_structs[next_run_process_index]; // This is start address of the register in the next run assembly struct
      pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32)); // This is start address of the program counter in the next run assembly struct
      id_reg_addr = pc_reg_addr + sizeof(mem_addr); // This is start address of the process id in the next run assembly struct
      name_reg_addr = id_reg_addr + sizeof(int); // This is start address of the process name in the next run assembly struct
      state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char)); // This is start address of the process state in the next run assembly struct

      // This index is the address of the process state in the next run assembly struct
      index = (int *)mem_reference(state_reg_addr);

      if(next_run_process_index != -1)
      {
        if(*index == 0 || *index == 2) // This check that the next run assembly struct's process state is ready or not_start_yet
        {
          // Printing information
          printf("Next Running Process ID : %d\n",next_run_process_index );
          flag_next_run = true;
        }
      }

      if(flag_next_run)
      {
        if(*index == 0) // This check that the next run assembly struct's process state is ready
        {
          // This index is the address of the process state in the next run assembly struct
          index = (int *)mem_reference(state_reg_addr);
          *index = 1; // Change the value of the this addres with 1 (1 means that running)

          // This loop calculate the register address of the next run assembly struct and assing it to the R.
          index = (int *)mem_reference(register_reg_addr);
          int32 current_reg_value;
          for(int i = 0; i < REGISTER_COUNT; ++i)
          {
            index = (int *)mem_reference(register_reg_addr + (sizeof(int32) * i));
            current_reg_value = (int32)(*index);
            R[i] = current_reg_value;
          }

          // This index is the address of the program counter in the next run assembly struct
          index = (int *)mem_reference(pc_reg_addr);
          mem_addr new_pc = (mem_addr)(*index); // Assing the value of the this addres to the new_pc
          run_program(new_pc, DEFAULT_RUN_STEPS, false, false, &continuable);
        }

        else if(*index == 2) // This check that the next run assembly struct's process state is not_start_yet
        {
          // This index is the address of the process state in the next run assembly struct
          index = (int *)mem_reference(state_reg_addr);
          *index = 1; // Change the value of the this addres with 1 (1 means that running)

          // This index is the address of the program counter in the next run assembly struct
          index = (int *)mem_reference(pc_reg_addr);
          mem_addr new_pc = (mem_addr)(*index); // Assing the value of the this addres to the new_pc

          pop_scanner();
          run_program(new_pc, DEFAULT_RUN_STEPS, false, false, &continuable);
        }
      }
    } 
  }
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
// It occurs when a SPIMOS_GTU_X.s was terminated, then it terminates the whole program by POSIX PROCESS_EXIT.
void process_exit()
{
  // These are variables for addres of the assembly struct's items.
  int *index;
  int register_reg_addr;
  int pc_reg_addr;
  int id_reg_addr;
  int name_reg_addr;
  int state_reg_addr;

  // These variables are assigned with the address of the init assembyl sturct's items
  int init_process_addr = addres_of_assembly_structs[0];

  register_reg_addr = init_process_addr;
  pc_reg_addr = register_reg_addr + (REGISTER_COUNT * sizeof(int32));
  id_reg_addr = pc_reg_addr + sizeof(mem_addr);
  name_reg_addr = id_reg_addr + sizeof(int);
  state_reg_addr = name_reg_addr + (MAX_NAME_LENGTH * sizeof(char));

  // This index is the address of the init's state
  index = (int *)mem_reference(state_reg_addr);
  *index = -1; // Change the value of this addres with -1 (-1 means that process is in the terminate state)

  print_processes(1); // Call print_processes function with 1 function argument.

  // Printing information
  printf("\n================================================================================\n");
  printf("End of Micro Kernel...\n");
  printf("Created by Ali Haydar KURBAN\n");
  printf("================================================================================\n");
  
  free(addres_of_assembly_structs); // Deallocate the start addres ot the assembly struct array 
  exit(1); // POSIX EXIT
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
 case ALLOCATE_MEMORY_FOR_ADDRESS_OF_ASSEMBLY_STRUCT:
 {
  int amount_of_process = R[REG_A3];
  amount_of_processes = amount_of_process;
  addres_of_assembly_structs = (int *)malloc(amount_of_process * sizeof(int));
  break;
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
case TAKE_ASSEMBLY_STRUCT_AND_CALL_FORK:
{
  int reg_a0 = R[REG_A0];
  _fork(reg_a0);
  break;
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
case EXECVE:
{
  _execv();
  break;
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
case PROCESS_EXIT:
{
  process_exit();
  break;
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
case TERMINATE_AND_EXECUTE_NEW_PROCESS:
{
  terminate_and_execute_new_process();
  break;
}
////////////// Created by Ali Haydar KURBAN //////////////


////////////// Created by Ali Haydar KURBAN //////////////
case RANDOM_INT_GENERATOR:
{
  srand(time(NULL));
  int random_number, upper_bound = R[REG_A0];
  random_number = rand() % upper_bound;
  R[REG_RES] = random_number;
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
