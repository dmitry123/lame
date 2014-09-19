#include "Library.h"

using Lame::Uint64;
using Lame::Uint32;
using Lame::Uint16;
using Lame::Uint8;

#ifdef LAME_WINDOWS
typedef          __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
#else
typedef  __int64_t LONGLONG;
typedef __uint64_t ULONGLONG;

#endif

typedef long LONG;

#ifndef LAME_WINDOWS

#  define IMAGE_FILE_MACHINE_UNKNOWN           0
#  define IMAGE_FILE_MACHINE_I386              0x014c  // Intel 386.
#  define IMAGE_FILE_MACHINE_R3000             0x0162  // MIPS little-endian, 0x160 big-endian
#  define IMAGE_FILE_MACHINE_R4000             0x0166  // MIPS little-endian
#  define IMAGE_FILE_MACHINE_R10000            0x0168  // MIPS little-endian
#  define IMAGE_FILE_MACHINE_WCEMIPSV2         0x0169  // MIPS little-endian WCE v2
#  define IMAGE_FILE_MACHINE_ALPHA             0x0184  // Alpha_AXP
#  define IMAGE_FILE_MACHINE_SH3               0x01a2  // SH3 little-endian
#  define IMAGE_FILE_MACHINE_SH3DSP            0x01a3
#  define IMAGE_FILE_MACHINE_SH3E              0x01a4  // SH3E little-endian
#  define IMAGE_FILE_MACHINE_SH4               0x01a6  // SH4 little-endian
#  define IMAGE_FILE_MACHINE_SH5               0x01a8  // SH5
#  define IMAGE_FILE_MACHINE_ARM               0x01c0  // ARM Little-Endian
#  define IMAGE_FILE_MACHINE_THUMB             0x01c2  // ARM Thumb/Thumb-2 Little-Endian
#  define IMAGE_FILE_MACHINE_ARMNT             0x01c4  // ARM Thumb-2 Little-Endian
#  define IMAGE_FILE_MACHINE_AM33              0x01d3
#  define IMAGE_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian
#  define IMAGE_FILE_MACHINE_POWERPCFP         0x01f1
#  define IMAGE_FILE_MACHINE_IA64              0x0200  // Intel 64
#  define IMAGE_FILE_MACHINE_MIPS16            0x0266  // MIPS
#  define IMAGE_FILE_MACHINE_ALPHA64           0x0284  // ALPHA64
#  define IMAGE_FILE_MACHINE_MIPSFPU           0x0366  // MIPS
#  define IMAGE_FILE_MACHINE_MIPSFPU16         0x0466  // MIPS
#  define IMAGE_FILE_MACHINE_AXP64             IMAGE_FILE_MACHINE_ALPHA64
#  define IMAGE_FILE_MACHINE_TRICORE           0x0520  // Infineon
#  define IMAGE_FILE_MACHINE_CEF               0x0CEF
#  define IMAGE_FILE_MACHINE_EBC               0x0EBC  // EFI Byte Code
#  define IMAGE_FILE_MACHINE_AMD64             0x8664  // AMD64 (K8)
#  define IMAGE_FILE_MACHINE_M32R              0x9041  // M32R little-endian
#  define IMAGE_FILE_MACHINE_CEE               0xC0EE

#  define PAGE_NOACCESS          0x01     
#  define PAGE_READONLY          0x02     
#  define PAGE_READWRITE         0x04     
#  define PAGE_WRITECOPY         0x08     
#  define PAGE_EXECUTE           0x10     
#  define PAGE_EXECUTE_READ      0x20     
#  define PAGE_EXECUTE_READWRITE 0x40     
#  define PAGE_EXECUTE_WRITECOPY 0x80     
#  define PAGE_GUARD            0x100     
#  define PAGE_NOCACHE          0x200     
#  define PAGE_WRITECOMBINE     0x400     

#  define IMAGE_DOS_SIGNATURE    0x5A4D      // MZ
#  define IMAGE_OS2_SIGNATURE    0x454E      // NE
#  define IMAGE_OS2_SIGNATURE_LE 0x454C      // LE
#  define IMAGE_VXD_SIGNATURE    0x454C      // LE
#  define IMAGE_NT_SIGNATURE     0x00004550  // PE00

#  define DUMMYUNIONNAME
#  define DUMMYSTRUCTNAME

typedef Uint32 DWORD;
typedef int    BOOL;
typedef Uint8  BYTE;
typedef Uint16 WORD;
typedef float  FLOAT;

typedef struct _IMAGE_DATA_DIRECTORY {
	Uint32 VirtualAddress;
	Uint32 Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#  ifndef IMAGE_NUMBEROF_DIRECTORY_ENTRIES
#    define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#  endif

typedef struct _IMAGE_DOS_HEADER {
	Uint16 e_magic;
	Uint16 LastPage;
	Uint16 TotalPages;
	Uint16 TotalRelocations;
	Uint16 ParagraphsSize;
	Uint16 MinExtra;
	Uint16 MaxExtra;
	Uint16 StackSegment;
	Uint16 StackPointer;
	Uint16 Checksum;
	Uint16 InstructionPointer;
	Uint16 CodeSegment;
	Uint16 FileAddress;
	Uint16 OverlayNumber;
	Uint16 Reserved1[4];
	Uint16 IdentifierOEM;
	Uint16 InformationOEM;
	Uint16 Reserved2[10];
	Uint32 e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
	Uint16 Machine;
	Uint16 NumberOfSections;
	Uint32 TimeDateStamp;
	Uint32 PointerToSymbolTable;
	Uint32 NumberOfSymbols;
	Uint16 SizeOfOptionalHeader;
	Uint16 Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
    WORD Magic;
    BYTE MajorLinkerVersion;
    BYTE MinorLinkerVersion;
    DWORD SizeOfCode;
    DWORD SizeOfInitializedData;
    DWORD SizeOfUninitializedData;
    DWORD AddressOfEntryPoint;
    DWORD BaseOfCode;
    ULONGLONG ImageBase;
    DWORD SectionAlignment;
    DWORD FileAlignment;
    WORD MajorOperatingSystemVersion;
    WORD MinorOperatingSystemVersion;
    WORD MajorImageVersion;
    WORD MinorImageVersion;
    WORD MajorSubsystemVersion;
    WORD MinorSubsystemVersion;
    DWORD Win32VersionValue;
    DWORD SizeOfImage;
    DWORD SizeOfHeaders;
    DWORD CheckSum;
    WORD Subsystem;
    WORD DllCharacteristics;
    ULONGLONG SizeOfStackReserve;
    ULONGLONG SizeOfStackCommit;
    ULONGLONG SizeOfHeapReserve;
    ULONGLONG SizeOfHeapCommit;
    DWORD LoaderFlags;
    DWORD NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_OPTIONAL_HEADER {
	Uint16 Magic;
	Uint8  MajorLinkerVersion;
	Uint8  MinorLinkerVersion;
	Uint32 SizeOfCode;
	Uint32 SizeOfInitializedData;
	Uint32 SizeOfUninitializedData;
	Uint32 AddressOfEntryPoint;
	Uint32 BaseOfCode;
	Uint32 BaseOfData;
	Uint32 ImageBase;
	Uint32 SectionAlignment;
	Uint32 FileAlignment;
	Uint16 MajorOperatingSystemVersion;
	Uint16 MinorOperatingSystemVersion;
	Uint16 MajorImageVersion;
	Uint16 MinorImageVersion;
	Uint16 MajorSubsystemVersion;
	Uint16 MinorSubsystemVersion;
	Uint32 Win32VersionValue;
	Uint32 SizeOfImage;
	Uint32 SizeOfHeaders;
	Uint32 CheckSum;
	Uint16 Subsystem;
	Uint16 DllCharacteristics;
	Uint32 SizeOfStackReserve;
	Uint32 SizeOfStackCommit;
	Uint32 SizeOfHeapReserve;
	Uint32 SizeOfHeapCommit;
	Uint32 LoaderFlags;
	Uint32 NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_NT_HEADERS64 {
	Uint32 Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

typedef struct _IMAGE_NT_HEADERS {
	Uint32 Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

#  ifdef LAME_X64
typedef IMAGE_NT_HEADERS64 IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;
#  else
typedef IMAGE_NT_HEADERS32 IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;
#  endif

typedef void* HINSTANCE;
typedef void* LPVOID;
typedef void* PVOID;
typedef void* FARPROC;

#  ifndef IMAGE_SIZEOF_SHORT_NAME
#    define IMAGE_SIZEOF_SHORT_NAME 8
#  endif

typedef struct _IMAGE_SECTION_HEADER {
	Uint8 Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		Uint32 PhysicalAddress;
		Uint32 VirtualSize;
	} Misc;
	Uint32 VirtualAddress;
	Uint32 SizeOfRawData;
	Uint32 PointerToRawData;
	Uint32 PointerToRelocations;
	Uint32 PointerToLinenumbers;
	Uint16 NumberOfRelocations;
	Uint16 NumberOfLinenumbers;
	Uint32 Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef unsigned long DWORD_PTR;
typedef unsigned long ULONG_PTR;
typedef          long LONG_PTR;

#define FIELD_OFFSET(type, field) \
	((LONG)(LONG_PTR)&(((type *)0)->field))

#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER) \
    ((ULONG_PTR)(ntheader) + \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) + \
     ((ntheader))->FileHeader.SizeOfOptionalHeader \
    ))

typedef struct _IMAGE_BASE_RELOCATION {
	Uint32 VirtualAddress;
    Uint32 SizeOfBlock;
} IMAGE_BASE_RELOCATION;

typedef IMAGE_BASE_RELOCATION *PIMAGE_BASE_RELOCATION;

#  define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#  define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#  define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#  define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#  define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#  define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#  define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
#  define IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#  define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#  define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#  define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#  define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#  define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#  define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#  define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#  define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

#  define IMAGE_REL_BASED_ABSOLUTE              0
#  define IMAGE_REL_BASED_HIGH                  1
#  define IMAGE_REL_BASED_LOW                   2
#  define IMAGE_REL_BASED_HIGHLOW               3
#  define IMAGE_REL_BASED_HIGHADJ               4
#  define IMAGE_REL_BASED_MACHINE_SPECIFIC_5    5
#  define IMAGE_REL_BASED_RESERVED              6
#  define IMAGE_REL_BASED_MACHINE_SPECIFIC_7    7
#  define IMAGE_REL_BASED_MACHINE_SPECIFIC_8    8
#  define IMAGE_REL_BASED_MACHINE_SPECIFIC_9    9
#  define IMAGE_REL_BASED_DIR64                 10

#  define IMAGE_SCN_MEM_DISCARDABLE 0x02000000 // Section can be discarded.
#  define IMAGE_SCN_MEM_NOT_CACHED  0x04000000 // Section is not cachable.
#  define IMAGE_SCN_MEM_NOT_PAGED   0x08000000 // Section is not pageable.
#  define IMAGE_SCN_MEM_SHARED      0x10000000 // Section is shareable.
#  define IMAGE_SCN_MEM_EXECUTE     0x20000000 // Section is executable.
#  define IMAGE_SCN_MEM_READ        0x40000000 // Section is readable.
#  define IMAGE_SCN_MEM_WRITE       0x80000000 // Section is writeable.

#  define IMAGE_SCN_CNT_CODE               0x00000020 // Section contains code.
#  define IMAGE_SCN_CNT_INITIALIZED_DATA   0x00000040 // Section contains initialized data.
#  define IMAGE_SCN_CNT_UNINITIALIZED_DATA 0x00000080 // Section contains uninitialized data.

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
	union {
		Uint32 Characteristics;            // 0 for terminating null import descriptor
		Uint32 OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
	} DUMMYUNIONNAME;
	Uint32 TimeDateStamp;                  // 0 if not bound,
	Uint32 ForwarderChain;                 // -1 if no forwarders
	Uint32 Name;
	Uint32 FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} IMAGE_IMPORT_DESCRIPTOR;

typedef IMAGE_IMPORT_DESCRIPTOR *PIMAGE_IMPORT_DESCRIPTOR;

typedef       char* LPSTR;
typedef const char* LPCSTR;
typedef       char  CHAR;

#  define IMAGE_ORDINAL_FLAG64 0x8000000000000000
#  define IMAGE_ORDINAL_FLAG32 0x80000000

#  define IMAGE_SNAP_BY_ORDINAL64(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG64) != 0)
#  define IMAGE_SNAP_BY_ORDINAL32(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG32) != 0)

#  define IMAGE_ORDINAL_FLAG64 0x8000000000000000
#  define IMAGE_ORDINAL_FLAG32 0x80000000
#  define IMAGE_ORDINAL64(Ordinal) (Ordinal & 0xffff)
#  define IMAGE_ORDINAL32(Ordinal) (Ordinal & 0xffff)

#  ifdef LAME_X64
#    define IMAGE_SNAP_BY_ORDINAL IMAGE_SNAP_BY_ORDINAL64
#    define IMAGE_ORDINAL IMAGE_ORDINAL64
#  else
#    define IMAGE_SNAP_BY_ORDINAL IMAGE_SNAP_BY_ORDINAL32
#    define IMAGE_ORDINAL IMAGE_ORDINAL32
#  endif

typedef struct _IMAGE_IMPORT_BY_NAME {
	Uint16 Hint;
	CHAR  Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

typedef struct _IMAGE_TLS_DIRECTORY64 {
	Uint64 StartAddressOfRawData;
	Uint64 EndAddressOfRawData;
	Uint64 AddressOfIndex;         // PDWORD
	Uint64 AddressOfCallBacks;     // PIMAGE_TLS_CALLBACK *;
	Uint32 SizeOfZeroFill;
	union {
		Uint64 Characteristics;
		struct {
			Uint64 Reserved0 : 20;
			Uint64 Alignment : 4;
			Uint64 Reserved1 : 8;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;

} IMAGE_TLS_DIRECTORY64;

typedef struct _IMAGE_TLS_DIRECTORY32 {
	DWORD   StartAddressOfRawData;
	DWORD   EndAddressOfRawData;
	DWORD   AddressOfIndex;             // PDWORD
	DWORD   AddressOfCallBacks;         // PIMAGE_TLS_CALLBACK *
	DWORD   SizeOfZeroFill;
	union {
		DWORD Characteristics;
		struct {
			DWORD Reserved0 : 20;
			DWORD Alignment : 4;
			DWORD Reserved1 : 8;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;

} IMAGE_TLS_DIRECTORY32;

typedef IMAGE_TLS_DIRECTORY64 * PIMAGE_TLS_DIRECTORY64;
typedef IMAGE_TLS_DIRECTORY32 * PIMAGE_TLS_DIRECTORY32;

#  ifdef LAME_X64
typedef PIMAGE_TLS_DIRECTORY64 PIMAGE_TLS_DIRECTORY;
#  else
typedef PIMAGE_TLS_DIRECTORY32 PIMAGE_TLS_DIRECTORY;
#  endif

typedef struct _IMAGE_EXPORT_DIRECTORY {
	DWORD   Characteristics;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	DWORD   Name;
	DWORD   Base;
	DWORD   NumberOfFunctions;
	DWORD   NumberOfNames;
	DWORD   AddressOfFunctions;     // RVA from base of image
	DWORD   AddressOfNames;         // RVA from base of image
	DWORD   AddressOfNameOrdinals;  // RVA from base of image
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

#  ifndef WINAPI
#    define WINAPI
#  endif

#  ifndef NTAPI
#    define NTAPI
#  endif

typedef void VOID;

typedef VOID (*PIMAGE_TLS_CALLBACK) (PVOID DllHandle, 
	DWORD Reason, PVOID Reserved);

#  define DLL_PROCESS_ATTACH   1    
#  define DLL_THREAD_ATTACH    2    
#  define DLL_THREAD_DETACH    3    
#  define DLL_PROCESS_DETACH   0    

#else
#  include <Windows.h>
#endif

#ifdef LAME_X64
#  define POINTER_TYPE ULONGLONG
#else
#  define POINTER_TYPE DWORD
#endif

#ifndef IMAGE_SIZEOF_BASE_RELOCATION
#  define IMAGE_SIZEOF_BASE_RELOCATION (sizeof(IMAGE_BASE_RELOCATION))
#endif

typedef BOOL(WINAPI *EntryProcDLL)(HINSTANCE hinstDLL,
	DWORD fdwReason, LPVOID lpReserved);

#define GET_HEADER_DICTIONARY(module, index) \
	&((ImageNtHeaderPtr)(module)->GetHeader())->OptionalHeader.DataDirectory[index]

#ifndef LAME_WINDOWS
#  include <sys/mman.h>
#endif

#ifndef LAME_WINDOWS
#  define _stricmp strcasecmp
#endif

LAME_BEGIN2(Core)

typedef IMAGE_NT_HEADERS ImageNtHeader, *ImageNtHeaderPtr;

static Void _CopySections(Uint8P blockData, PIMAGE_NT_HEADERS imageNtHeader, LibraryPtr module) {

	ImageNtHeaderPtr moduleHeader = (ImageNtHeaderPtr)module->GetHeader();
	Uint8P codeBase = (Uint8P)module->GetCodeSegment();
	Uint8P destination;

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(moduleHeader);

	for (Uint32 i = 0; i < moduleHeader->FileHeader.NumberOfSections; i++, section++) {

		if (section->SizeOfRawData == 0) {

			Uint32 size = imageNtHeader->OptionalHeader.SectionAlignment;

			if (size > 0) {

#ifdef LAME_WINDOWS
				destination = (unsigned char*)VirtualAlloc(codeBase + section->VirtualAddress,
					size, MEM_COMMIT, PAGE_READWRITE);
#else
				destination = codeBase + section->VirtualAddress;
//                destination = (unsigned char*)mmap(codeBase + section->VirtualAddress,
//                    size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0);
#endif

				section->Misc.PhysicalAddress = (DWORD)(POINTER_TYPE)destination;
				memset(destination, 0, size);
			}

			continue;
		}

#ifdef LAME_WINDOWS
		destination = (Uint8P)VirtualAlloc(codeBase + section->VirtualAddress,
			section->SizeOfRawData, MEM_COMMIT, PAGE_READWRITE);
#else
		destination = codeBase + section->VirtualAddress;
//        destination = (Uint8P)mmap(codeBase + section->VirtualAddress,
//            section->SizeOfRawData, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0);
#endif

		memcpy(destination, blockData + section->PointerToRawData, section->SizeOfRawData);
		section->Misc.PhysicalAddress = (DWORD)(POINTER_TYPE)destination;
	}
}

static Void _BaseRelocation(LibraryPtr module, Uint64 delta) {

	Uint8P codeBase = (Uint8P)module->GetCodeSegment();

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module,
		IMAGE_DIRECTORY_ENTRY_BASERELOC);

	if (directory->Size > 0) {

		PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION)(
			codeBase + directory->VirtualAddress);

		for (; relocation->VirtualAddress > 0;) {

			Uint16P relInfo = (Uint16P)((Uint8P)relocation +
				IMAGE_SIZEOF_BASE_RELOCATION);

			Uint8P dest = codeBase + relocation->VirtualAddress;

			for (Uint32 i = 0; i < ((relocation->SizeOfBlock - IMAGE_SIZEOF_BASE_RELOCATION) / 2); i++, relInfo++) {

				DWORD *patchAddrHL;
				Sint32 type;
				Sint32 offset;

#ifdef LAME_X64
				ULONGLONG *patchAddr64;
#endif

				type = *relInfo >> 12;
				offset = *relInfo & 0xfff;

				switch (type)
				{
				case IMAGE_REL_BASED_ABSOLUTE:
					break;

				case IMAGE_REL_BASED_HIGHLOW:
					patchAddrHL = (DWORD*)(dest + offset);
					*patchAddrHL += (DWORD)delta;
					break;

#ifdef LAME_X64
				case IMAGE_REL_BASED_DIR64:
					patchAddr64 = (ULONGLONG *)(dest + offset);
					*patchAddr64 += (ULONGLONG)delta;
					break;
#endif

				default:
					break;
				}
			}
            
			relocation = (PIMAGE_BASE_RELOCATION)(((Sint8P)relocation) + relocation->SizeOfBlock);
		}
	}
}

#ifndef LAME_WINDOWS

#  include <sys/stat.h>
#  include <fcntl.h>
#  include <sys/types.h>
#  include <unistd.h>

#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>

unsigned char g_ucCalledByTestRoutine = 0;
jmp_buf g_pBuf;

void __cdecl MemTestHandler ( int nSig)
{
	if (g_ucCalledByTestRoutine) {
		longjmp (g_pBuf, 1);
	}
}

unsigned char _IsBadReadPointer ( void* pv, unsigned long ulSize)
{
    char* pc;
    void (__cdecl *pPrev) (int sig);
	
    g_ucCalledByTestRoutine = 1;
	
	if (setjmp(g_pBuf)) {
		return 0;
	}
	
	pPrev = signal(SIGSEGV, MemTestHandler);
	pc = (char*)malloc(ulSize);
	memcpy(pc, pv, ulSize);
	free(pc);
	
    g_ucCalledByTestRoutine = 0;
	
    signal(SIGSEGV, pPrev);
	
	return 1;
}

static int IsBadReadPtr(VoidP pointer, Uint32 size) {
	return _IsBadReadPointer(pointer, size);
}
#endif

static Bool _BuildImportTable(LibraryPtr module) {

	Uint8P codeBase = (Uint8P)module->GetCodeSegment();

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module,
		IMAGE_DIRECTORY_ENTRY_IMPORT);

	if (directory->Size > 0) {

		PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)(
			codeBase + directory->VirtualAddress);
		
//		!IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) &&
		
		for (; importDesc->Name; importDesc++) {

			POINTER_TYPE* thunkRef;
			FARPROC *funcRef;
			LibraryPtr nextLibrary;

			nextLibrary = new Library((LPCSTR)(codeBase + importDesc->Name));

			if (importDesc->OriginalFirstThunk) {
				thunkRef = (POINTER_TYPE *)(codeBase + importDesc->OriginalFirstThunk);
				funcRef = (FARPROC*)(codeBase + importDesc->FirstThunk);
			}
			else {
				thunkRef = (POINTER_TYPE *)(codeBase + importDesc->FirstThunk);
				funcRef = (FARPROC*)(codeBase + importDesc->FirstThunk);
			}

			for (; *thunkRef; thunkRef++, funcRef++) {
				if (IMAGE_SNAP_BY_ORDINAL(*thunkRef)) {
					*funcRef = (FARPROC)nextLibrary->GetProcAddress((LPCSTR)IMAGE_ORDINAL(*thunkRef));
				}
				else {
					PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME)(codeBase + (*thunkRef));
					*funcRef = (FARPROC)nextLibrary->GetProcAddress((LPCSTR)&thunkData->Name);
				}
				if (*funcRef == 0) {
					throw Exception("Procedure not found (%s)", IMAGE_ORDINAL(*thunkRef));
				}
			}

			module->AddModule(nextLibrary);
		}
	}

	return TRUE;
}

static Void _ExecuteTLS(LibraryPtr module) {

	Uint8P codeBase = (Uint8P)module->GetCodeSegment();

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module,
		IMAGE_DIRECTORY_ENTRY_TLS);

	if (directory->VirtualAddress > 0) {

		PIMAGE_TLS_DIRECTORY tls = (PIMAGE_TLS_DIRECTORY)(codeBase + directory->VirtualAddress);
		PIMAGE_TLS_CALLBACK* callback = (PIMAGE_TLS_CALLBACK *)tls->AddressOfCallBacks;

		if (callback) {
			while (*callback) {
				(*callback++)((LPVOID)codeBase, DLL_PROCESS_ATTACH, NULL);
			}
		}
	}
}

static int ProtectionFlags[2][2][2] = {
	{
		{ PAGE_NOACCESS, PAGE_WRITECOPY },
		{ PAGE_READONLY, PAGE_READWRITE },
	}, {
		{ PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY },
		{ PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE },
	},
};

static Void _FinalizeSections(LibraryPtr module) {

	ImageNtHeaderPtr ntHeader = ImageNtHeaderPtr(module->GetHeader());

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(
		ntHeader);

#ifdef LAME_X64
	POINTER_TYPE imageOffset = ntHeader->OptionalHeader.ImageBase & 0xffffffff00000000;
#else
#  define imageOffset 0
#endif

	// loop through all sections and change access flags
	for (Uint32 i = 0; i < ntHeader->FileHeader.NumberOfSections; i++, section++) {

		DWORD protect;
		DWORD size;

		int executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
		int readable = (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;
		int writeable = (section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0;

		if (section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) {
#ifdef LAME_WINDOWS
			VirtualFree((LPVOID)((POINTER_TYPE)section->Misc.PhysicalAddress | imageOffset), section->SizeOfRawData, MEM_DECOMMIT);
#else
            munmap(VoidP(section->Misc.PhysicalAddress | imageOffset), section->SizeOfRawData);
#endif
			continue;
		}

		// determine protection flags based on characteristics
		protect = ProtectionFlags[executable][readable][writeable];

#ifdef LAME_WINDOWS
		DWORD oldProtect = protect;
#endif

		if (section->Characteristics & IMAGE_SCN_MEM_NOT_CACHED) {
			protect |= PAGE_NOCACHE;
		}

		// determine size of region
		size = section->SizeOfRawData;
        
		if (size == 0) {
			if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
				size = ntHeader->OptionalHeader.SizeOfInitializedData;
			}
			else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
				size = ntHeader->OptionalHeader.SizeOfUninitializedData;
			}
		}
        
		if (size > 0) {
#ifdef LAME_WINDOWS
			VirtualProtect((LPVOID)((POINTER_TYPE)section->Misc.PhysicalAddress | imageOffset), size, protect, &oldProtect);
#else
            mprotect((LPVOID)((POINTER_TYPE)section->Misc.PhysicalAddress | imageOffset), size,
                PROT_READ | PROT_WRITE);
#endif
		}
	}
#ifndef LAME_X64
#  undef imageOffset
#endif
}

Void Library::Load(StringC fileName) {

	IMAGE_DOS_HEADER imageDosHeader;
	IMAGE_NT_HEADERS imageNtHeader;

	File fileHandle;

	printf("%s\n", fileName);

	this->fileName = fileName;
	this->nativeHandle = NULL;

#ifdef LAME_WINDOWS
	try {
		fileHandle.Open(fileName, "rb");
	}
	catch (FileException& e) {
		this->nativeHandle = LoadLibraryA(fileName);
	}
#else
	fileHandle.Open(fileName, "rb");
#endif

	if (this->nativeHandle) {
		return /* Skip Loading */;
	}

	fileHandle.Read(&imageDosHeader,
		sizeof(IMAGE_DOS_HEADER));

	if (imageDosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
		throw Exception("File not Win32 executable (%s)", fileName);
	}

	fileHandle.SetPosition((Uint32)imageDosHeader.e_lfanew);
	fileHandle.Read(&imageNtHeader,
		sizeof(IMAGE_NT_HEADERS));

	if (imageNtHeader.Signature != IMAGE_NT_SIGNATURE) {
		throw Exception("File not Win32 executable (%s)", fileName);
	}

	if (imageNtHeader.FileHeader.Machine != IMAGE_FILE_MACHINE_I386 &&
		imageNtHeader.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 &&
		imageNtHeader.FileHeader.Machine != IMAGE_FILE_MACHINE_IA64
	) {
		throw Exception("Only supported architecture I386 (%.4X)", imageNtHeader.FileHeader.Machine);
	}

	this->isAmd64 =
		imageNtHeader.FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 ||
		imageNtHeader.FileHeader.Machine == IMAGE_FILE_MACHINE_IA64;

#ifdef LAME_WINDOWS
	codeSegment = (Uint8P)VirtualAlloc((VoidP)imageNtHeader.OptionalHeader.ImageBase,
		imageNtHeader.OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
    codeSegment = (Uint8P)mmap((VoidP)imageNtHeader.OptionalHeader.ImageBase,
        imageNtHeader.OptionalHeader.SizeOfImage, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0);
#endif

	if (!codeSegment) {

#ifdef LAME_WINDOWS
        codeSegment = (Uint8P)VirtualAlloc(NULL, imageNtHeader.OptionalHeader.SizeOfImage,
			MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
        codeSegment = (Uint8P)mmap(NULL, imageNtHeader.OptionalHeader.SizeOfImage,
			PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0);
#endif

		if (!codeSegment) {
			throw Exception("Unable to allocate memory for code segment (%s)", fileName);
		}
	}

	Uint8P blockData = new Uint8[fileHandle.GetSize()];

	fileHandle.Rewind();
	fileHandle.Read(blockData, fileHandle.GetSize());
	fileHandle.Close();

#ifdef LAME_WINDOWS
	VoidP ntHeader = VirtualAlloc(codeSegment, imageNtHeader.OptionalHeader.SizeOfHeaders,
		MEM_COMMIT, PAGE_READWRITE);
#else
	VoidP ntHeader = (Uint8P)codeSegment;
//    VoidP ntHeader = mmap(codeSegment, imageNtHeader.OptionalHeader.SizeOfHeaders,
//        PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);
#endif

	memcpy(ntHeader, &imageDosHeader,
		imageNtHeader.OptionalHeader.SizeOfHeaders);

	this->ptrHeaders = (Uint8P)&((Uint8P)(blockData))[imageDosHeader.e_lfanew];

	((ImageNtHeaderPtr)this->ptrHeaders)->OptionalHeader.ImageBase =
		POINTER_TYPE(this->codeSegment);

	_CopySections(blockData, &imageNtHeader, this);

	Uint64 locationDelta = Uint64((Uint8P)codeSegment - imageNtHeader.OptionalHeader.ImageBase);

	if (locationDelta != 0) {
		_BaseRelocation(this, locationDelta);
	}

	if (!_BuildImportTable(this)) {
		throw Exception("Unable to build DLL import table (%s)", fileName);
	}

	_FinalizeSections(this);
	_ExecuteTLS(this);

#if 0 /* My DLL's wont have entry point */
	if (((ImageNtHeaderPtr)this->ptrHeaders)->OptionalHeader.AddressOfEntryPoint != 0) {

		EntryProcDLL entryPoint = (EntryProcDLL)(this->GetCodeSegment() +
			((ImageNtHeaderPtr)this->ptrHeaders)->OptionalHeader.AddressOfEntryPoint);

		Uint32 result = (*entryPoint)((HINSTANCE)codeSegment,
			DLL_PROCESS_ATTACH, 0);

		if (!result) {
			throw Exception("Unable to execute DLL's entry point (%s)", fileName);
		}

		this->isInitialized = TRUE;
	}
#endif

	this->blockData = blockData;
}

VoidP Library::GetProcAddress(StringC procName) {

#ifdef LAME_WINDOWS
	if (this->nativeHandle) {
		return ::GetProcAddress(HMODULE(this->nativeHandle), procName);
	}
#endif

	Uint8P codeBase = (Uint8P)this->GetCodeSegment();
	Uint32 idx = -1;
	Uint32 i;
	Uint32P nameRef;
	Uint16P ordinal;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(this,
		IMAGE_DIRECTORY_ENTRY_EXPORT);
	
	PIMAGE_EXPORT_DIRECTORY exports;

	if (directory->Size == 0) {
		throw Exception("Unable to find export table (%s)", procName);
	}

	exports = (PIMAGE_EXPORT_DIRECTORY)(codeBase + directory->VirtualAddress);

	if (exports->NumberOfNames == 0 || exports->NumberOfFunctions == 0) {
		throw Exception("Library doesn't export anything (%s)", this->GetName().data());
	}

	nameRef = (Uint32P)(codeBase + exports->AddressOfNames);
	ordinal = (Uint16P)(codeBase + exports->AddressOfNameOrdinals);

	for (i = 0; i < exports->NumberOfNames; i++, nameRef++, ordinal++) {
		if (_stricmp(procName, (StringC)(codeBase + (*nameRef))) == 0) {
			idx = *ordinal;
			break;
		}
	}

	if (idx == -1) {
		throw Exception("Unable to find procedure (%s/%s)",
			this->fileName.data(), procName);
	}

	if ((DWORD)idx > exports->NumberOfFunctions) {
		throw Exception("Procedure's number and it's index doesn't match", procName);
	}

	return (FARPROC)(codeBase + (*(DWORD *)(codeBase + exports->AddressOfFunctions + (idx * 4))));
}

LAME_END2