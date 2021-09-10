#include <windows.h>
#include <tlhelp32.h>
#include <cfloat>

bool init = false;

struct memory_ptr {
	char* moduleName;
	DWORD moduleOffset;
	int offsetCount;
	uintptr_t offsets[];
};

// Local Player
memory_ptr localPlayerPtr = {
	"mono-2.0-bdwgc.dll",
	0x00491DE8,
	2,
	{ 0x90, 0xE2C }
};

struct local_player {
	unsigned long int lunarCoins;
	unsigned long int totalCollectedCoins;
};

local_player* localPlayer;

// Player Stats
memory_ptr playerStatsPtr = {
	"mono-2.0-bdwgc.dll",
	0x00491DE8,
	4,
	{ 0xA0, 0xCA0, 0x68, 0x2BC }
};

struct player_stats {
	float baseMaxHeath;
	float baseRegen;
	float baseMaxShield;
	float baseMoveSpeed;
	float baseAcceleration;
	float baseJumpPower;
	float baseDamage;
	float baseAttackSpeed;
	float baseCrit;
	float baseArmor;
	float baseJumpCount;
	float sprintingSpeedMultiplier;
};

player_stats powerPlayerStats = {
	FLT_MAX,
	FLT_MAX,
	FLT_MAX,
	10.0,
	80.0,
	20.0,
	FLT_MAX,
	FLT_MAX,
	FLT_MAX,
	FLT_MAX,
	FLT_MAX,
	2.0
};

player_stats* playerStats;

// Player Inventory
memory_ptr playerInventoryPtr = {
	"mono-2.0-bdwgc.dll",
	0x00491DE8,
	6,
	{ 0xA0, 0xCA0, 0x68, 0x70, 0x30, 0x28 }
};

struct player_inventory {
	unsigned long int items[130];
};

player_inventory* playerInventory;

void* GetDynamicPtr(HANDLE hProc, memory_ptr* ptr) {
	uintptr_t moduleBaseAddr = (uintptr_t)GetModuleHandle(ptr->moduleName);
	uintptr_t dynPtrBaseAddr = moduleBaseAddr + ptr->moduleOffset;

	uintptr_t dynPtr = dynPtrBaseAddr;
	for(int i = 0; i < ptr->offsetCount; ++i)
	{
		ReadProcessMemory(hProc, (void*)dynPtr, &dynPtr, sizeof(dynPtr), 0);
		dynPtr += ptr->offsets[i];
	}
	return (void*)dynPtr;
}

void InitPointers() {
	int pID = GetCurrentProcessId();
	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);

	localPlayer = (local_player*)GetDynamicPtr(procHandle, &localPlayerPtr);
	playerStats = (player_stats*)GetDynamicPtr(procHandle, &playerStatsPtr);
	playerInventory = (player_inventory*)GetDynamicPtr(procHandle, &playerInventoryPtr);

	init = true;
}

DWORD WINAPI MainThread(LPVOID param) {
	InitPointers();

	while (true) {
		if (GetAsyncKeyState(VK_F6) & 0x80000) {		// F6: Add 10 lunar coins
			localPlayer->lunarCoins += 10;
		} else if (GetAsyncKeyState(VK_F5) & 0x80000) {		// F5: Give 16 of every item
			for (int i = 0; i < 130; ++i) playerInventory->items[i] = 16;
		} else if (GetAsyncKeyState(VK_F4) & 0x80000) {		// F4: Set max base stats
			memcpy(playerStats, &powerPlayerStats, sizeof(player_stats));
		}
		Sleep(100);
	}

	return 0;
}

bool WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0, MainThread, hModule, 0, 0);
	}

	return true;
}
