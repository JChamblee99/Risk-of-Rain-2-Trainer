#include <windows.h>
#include <tlhelp32.h>

int getProcId(const char* target) {
	DWORD pID = 0;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	do {
		if (strcmp(pe32.szExeFile, target) == 0) {
			CloseHandle(hSnapshot);
			pID = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapshot, &pe32));
	CloseHandle(hSnapshot);
	return pID;
}

int main(int argc, char* argv[]) {
	const char* process = "example.exe";		// Name of the process to inject into
	int pID = getProcId(process);

	char dll[] = "dllmain.dll";			// Name of the dll file to inject
	char dllPath[MAX_PATH] = { 0 };
	GetFullPathNameA(dll, MAX_PATH, dllPath, NULL);
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pID);
	LPVOID pszLibFileRemote = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	WriteProcessMemory(hProcess, pszLibFileRemote, dllPath, strlen(dllPath) + 1, NULL);
	HANDLE handleThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, NULL, NULL);

	WaitForSingleObject(handleThread, INFINITE);
	CloseHandle(handleThread);
	VirtualFreeEx(hProcess, dllPath, 0, MEM_RELEASE);
	CloseHandle(hProcess);

	return 0;
}
