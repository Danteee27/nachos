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

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
// #include "filesys.h"
#include "synchconsole.h"
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

// Input: - User space address (int)
//- Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char* User2System(int virtAddr,int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit +1];//need for terminal string
	if (kernelBuf == NULL)
	return kernelBuf;
	memset(kernelBuf,0,limit+1);
	//printf("\n Filename u2s:");
	for (i = 0 ; i < limit ;i++)
	{
	kernel->machine->ReadMem(virtAddr+i,1,&oneChar);
	kernelBuf[i] = (char)oneChar;
	//printf("%c",kernelBuf[i]);
	if (oneChar == 0)
	break;
	}
	return kernelBuf;
}

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
	  {
		DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

		SysHalt();

		ASSERTNOTREACHED();
		break;
	  }
	
	
	  case SC_CreateFile:
		{
			int virtAddr;
			char* filename;
			DEBUG(dbgSys,"\n SC_Create call ...");
			DEBUG(dbgSys,"\n Reading virtual address of filename");
			// Lấy tham số tên tập tin từ thanh ghi r4
			virtAddr = kernel->machine->ReadRegister(4);
			DEBUG (dbgSys,"\n Reading filename.");
			// MaxFileLength là = 32
			filename = User2System(virtAddr,32+1);
			if (filename == NULL)
			{
			printf("\n Not enough memory in system");
			DEBUG(dbgSys,"\n Not enough memory in system");
			kernel->machine->WriteRegister(2,-1); // trả về lỗi cho chương
			// trình người dùng
			delete filename;
			return;
			}
			DEBUG(dbgSys,"\n Finish reading filename.");
			//DEBUG(‘a’,"\n File name : '"<<filename<<"'");
			// Create file with size = 0
			// Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
			// việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
			// hành Linux, chúng ta không quản ly trực tiếp các block trên
			// đĩa cứng cấp phát cho file, việc quản ly các block của file
			// trên ổ đĩa là một đồ án khác
			if (!kernel->fileSystem->Create(filename,0))
			{
			printf("\n Error create file '%s'",filename);
			kernel->machine->WriteRegister(2,-1);
			delete filename;
			return;
			}
			kernel->machine->WriteRegister(2,0); // trả về cho chương trình
			// người dùng thành công
			delete filename;
			break;
		}

		case SC_Print:
		{
			int vitrAddr = kernel->machine->ReadRegister(4);

			char* buffer = User2System(vitrAddr, 128+1);

			int len = 0;
			SynchConsoleOutput* console = new SynchConsoleOutput("stdout");

			while(buffer[len++] != '\0')
			{
				console->PutChar(buffer[len]);
			}
			break;
		}


      

      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    // ASSERTNOTREACHED();
}
