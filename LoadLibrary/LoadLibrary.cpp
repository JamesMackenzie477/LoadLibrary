#include <iostream>
#include <Windows.h>

namespace std
{

	// Converts the given pointer to the specified procedure.
	template <class T> T* to_procedure(const std::uintptr_t _Addr)
	{
		return reinterpret_cast<T*>(_Addr);
	}

	// Creates a procedure via a module offset.
	template <class T> T* to_procedure(const std::string _Mod, const std::uintptr_t _Off)
	{
		auto _Bse = GetModuleHandleA(_Mod.c_str());

		if (!_Bse)
			throw std::invalid_argument("Module does not exist.");

		return to_procedure<T>(reinterpret_cast<std::uintptr_t>(_Bse) + _Off);
	}

	// Creates a procedure via an exported module function.
	template <class T> T* to_procedure(const std::string _Mod, const std::string _Proc)
	{
		auto _Bse = GetModuleHandleA(_Mod.c_str());

		if (!_Bse)
			throw std::invalid_argument("Module does not exist.");

		auto _Addr = GetProcAddress(_Bse, _Proc.c_str());

		if (!_Addr)
			throw std::invalid_argument("Procedure does not exist.");

		return to_procedure<T>(reinterpret_cast<std::uintptr_t>(_Addr));
	}
}

// Windows types definitions.
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// Windows function imports.
EXTERN_C VOID WINAPI RtlInitUnicodeStringEx(PUNICODE_STRING DestinationString, PCWSTR SourceString);

typedef struct _MODULE_INFO
{
	char char_0x0[0x30]; // 0x0
	HANDLE hModule; // 0x30
} MODULE_INFO, *PMODULE_INFO;

typedef struct _MODULE_RESULT
{
	PMODULE_INFO pModuleInfo;
} MODULE_RESULT, *PMODULE_RESULT;

__int64 __fastcall LdrpLoadDll(PUNICODE_STRING ModuleFileName, PUNICODE_STRING PathToFile, void * ptr1, ULONG Flags, bool bool1, void * ptr2, PMODULE_RESULT pResult)
{
	return std::to_procedure<__int64 __fastcall(PUNICODE_STRING, PUNICODE_STRING, void *, ULONG, bool, void *, PMODULE_RESULT)>("ntdll.dll", 0x261F0)(ModuleFileName, PathToFile, ptr1, Flags, bool1, ptr2, pResult);
}

HANDLE LoadLib(PCWSTR ModuleName, PCWSTR ModulePath)
{
	UNICODE_STRING Name;
	UNICODE_STRING Path;
	MODULE_RESULT Result;

	RtlInitUnicodeStringEx(&Name, ModuleName);
	RtlInitUnicodeStringEx(&Path, ModulePath);

	if (LdrpLoadDll(&Name, &Path, nullptr, NULL, true, nullptr, &Result) >= 0)
	{
		return Result.pModuleInfo->hModule;
	}

	return nullptr;
}

int main()
{

	std::cout << LoadLib(L"api-ms-win-crt-conio-l1-1-0.dll", L"C:\\Windows\\System32") << std::endl;

	std::cin.get();
}