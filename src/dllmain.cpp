#include "definitions.h"

#include <Windows.h>
#include <intrin.h>
#include <string>
#include <TlHelp32.h>
#include <psapi.h>
#include <detours/detours.h>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

decltype(&LoadLibraryExW) pLoadLibraryExW;
decltype(&CreateProcessAsUserW) pCreateProcessAsUserW;
decltype(&CreateProcessW) pCreateProcessW;

std::string RootPath;

void FuckAppraiser() {
	//CopyFileA("C:\\Windows\\System32\\appraiser_patch_newest.dll", "C:\\$WINDOWS.~BT\\Sources\\appraiser.dll", FALSE);
	//CopyFileA("C:\\Windows\\System32\\Appraiser_Data_bypass.ini", "C:\\$WINDOWS.~BT\\Sources\\Appraiser_Data.ini", FALSE);
	auto patchedDLL = RootPath + "\\appraiser_patch_newest.dll";
	auto target = "C:\\$WINDOWS.~BT\\Sources\\appraiser.dll";
	std::string test;
	test += "Copying file ";
	test += patchedDLL;
	test += " to ";
	test += target;
	auto ret = CopyFileA(patchedDLL.c_str(), target, FALSE);
	auto lastErr = GetLastError();
	test += ", retVal: ";
	test += std::to_string(ret);
	test += ", lastError: ";
	test += std::to_string(lastErr);
	OutputDebugStringA(test.c_str());
}

BOOL hook_CreateProcessAsUserW(
	HANDLE					hToken,
	LPCWSTR					lpApplicationName,
	LPWSTR					lpCommandLine,
	LPSECURITY_ATTRIBUTES	lpProcessAttributes,
	LPSECURITY_ATTRIBUTES	lpThreadAttributes,
	BOOL					bInheritHandles,
	DWORD					dwCreationFlags,
	LPVOID					lpEnvironment,
	LPCWSTR					lpCurrentDirectory,
	LPSTARTUPINFOW			lpStartupInfo,
	LPPROCESS_INFORMATION	lpProcessInformation
) {
	OutputDebugStringA("Successfully hijacked UpdateBox CreateProcessAsUserW!");
	FuckAppraiser();
	return pCreateProcessAsUserW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

BOOL hook_CreateProcessW(
	LPCWSTR               lpApplicationName,
	LPWSTR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCWSTR               lpCurrentDirectory,
	LPSTARTUPINFOW        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
) {
	OutputDebugStringA("Successfully hijacked UpdateBox CreateProcessW!");
	FuckAppraiser();
	return pCreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

HMODULE  WINAPI hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE  hFile, DWORD   dwFlags)
{
	if (lpLibFileName && wcsstr(lpLibFileName, L"appraiser.dll") != nullptr)
    {
		OutputDebugStringA("Successfully hijacked appraiser!");
		return pLoadLibraryExW(L"C:\\Windows\\System32\\appraiser.dll", NULL, dwFlags);
    }
    return pLoadLibraryExW(lpLibFileName, hFile, dwFlags);
}

bool hook() {
	*(void **)&pLoadLibraryExW = GetProcAddress(LoadLibraryA("kernel32"), "LoadLibraryExW");
	*(void**)&pCreateProcessAsUserW = GetProcAddress(LoadLibraryA("Advapi32.dll"), "CreateProcessAsUserW");
	*(void**)&pCreateProcessW = GetProcAddress(LoadLibraryA("kernel32.dll"), "CreateProcessW");

	LONG ret = 0;
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	ret = DetourUpdateThread(GetCurrentThread());
	ret = DetourAttach(&(LPVOID&)pLoadLibraryExW, hook_LoadLibraryExW);
	ret = DetourAttach(&(LPVOID&)pCreateProcessAsUserW, hook_CreateProcessAsUserW);
	ret = DetourAttach(&(LPVOID&)pCreateProcessW, hook_CreateProcessW);
	ret = DetourTransactionCommit();
	return ret == NO_ERROR;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call != DLL_PROCESS_ATTACH)
		return TRUE;
	
	OutputDebugStringA("AppraiserHijack Loaded!");
	//CopyFileA("C:\\Windows\\System32\\appraiser.dll", "C:\\$WINDOWS.~BT\\Sources\\appraiser.dll", FALSE);

	char filePath[0x1000] = { 0 };
	GetModuleFileNameA(hModule, filePath, sizeof(filePath));
	HANDLE hFile = CreateFileA(filePath,
		FILE_READ_EA,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		0,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		OutputDebugStringA("Failed to access current DLL file!");
		return FALSE;
	}
	DWORD rcode = GetFinalPathNameByHandleA(hFile, filePath, sizeof(filePath), FILE_NAME_NORMALIZED);
	if (rcode == 0) {
		OutputDebugStringA("Failed to get current DLL's final path!");
		return FALSE;
	}

	char* realPath = NULL;
	if (filePath[0] == '\\' && filePath[1] == '\\' && filePath[2] == '?' && filePath[3] == '\\') {
		RootPath = realPath + 4;
	}
	else {
		RootPath = realPath;
	}

	fs::path p = realPath;
	RootPath = p.parent_path().string();
	

	if (!hook()) {
		OutputDebugStringA("AppraiserHijack Hook Failed!");
		return FALSE;
	}
	OutputDebugStringA("AppraiserHijack Hook Success!");
	return TRUE;
}