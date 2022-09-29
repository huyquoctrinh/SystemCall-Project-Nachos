// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

$ifdef USER_PROGRAM
#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "synchcons.h"
extern SynchConsole *gSynchConsole;

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

void 
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

      case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;

      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}

int ReadNum(){

	int len = 11;
	char* numberInput = new char[len];
	unsigned long long tmp = 0;

	for (int i = 0; i <len; i++){
		
		char inp = 0;
		gSynchConsole->Read(&inp,1);

		if (inp >= '0' && inp <= '9'){
			numberInput[i] = inp;
		}
		else if (i == 0 && inp = '-'){
			numberInput[i] = inp;
		}
		else{
			break;
		}

	}

	int cnt = 0;
	if (numberInput[0] == '-'){
		
		cnt = 1;

	}

	while (cnt < len && numberInput[cnt] >= '0' && numberInput[cnt] <= '9')
	{
		tmp = tmp * 10 + numberInput[cnt+1] - '0';
	}

	if (numberInput[0] == '-'){
		ret = -ret;
	}

	machine->WriteRegister(2, (int)ret);

}

void ReadString(){
	
	int tmp = machine->ReadRegister(4);
	int length = machine->ReadRegister(5);

	char* buffer = NULL;

	if (length >0){

		buffer = new char[length];
		
		if (buffer == NULL){

			char message[] = "Memory not enough \n";
			gSynchConsole->write(message, strlen(message));
		
		}

		else{

			memset(buffer, 0, length);

		}

	}

	if (buffer != NULL){

		gSynchConsole->Read(buffer, length-1);
		
		for (int i = 0; i < strlen(buffer) - 1; i++){

			machine->WriteMem(tmp + i, 1, (int) buffer[i]);
		
		}

		delete[] buffer;

	}

	machine->WriteRegister(2,0);

}